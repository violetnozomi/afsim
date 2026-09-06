// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfTrackStateController.hpp"

#include <algorithm>
#include <limits>
#include <map>
#include <string>
#include <vector>

#include "UtCast.hpp"
#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtScriptContext.hpp"
#include "UtScriptData.hpp"
#include "WsfEvent.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfLocalTrackStatus.hpp"
#include "WsfPlatform.hpp"
#include "WsfProcessor.hpp"
#include "WsfProcessorObserver.hpp"
#include "WsfSimulation.hpp"
#include "WsfStatusMessage.hpp"
#include "WsfStringId.hpp"
#include "WsfTimeDelayQueue.hpp"
#include "WsfTrackList.hpp"
#include "WsfTrackManager.hpp"
#include "WsfTrackStateControllerEvents.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptStateMachine.hpp"


WsfTrackStateController_RequestEvaluationEvent::WsfTrackStateController_RequestEvaluationEvent(
   double                   aSimTime,
   WsfTrackStateController* aControllerPtr,
   WsfLocalTrackStatus*     aStatusPtr)
   : WsfEvent(aSimTime)
   , mControllerPtr(aControllerPtr)
   , mLocalTrackId(aStatusPtr->GetLocalTrackId())
   , mEvaluationEpoch(aStatusPtr->EvaluationEpoch())
{
   mPlatformIndex = mControllerPtr->GetPlatform()->GetIndex();
}

WsfEvent::EventDisposition WsfTrackStateController_RequestEvaluationEvent::Execute()
{
   if (GetSimulation()->GetPlatformByIndex(mPlatformIndex) != nullptr)
   {
      mControllerPtr->RequestEvaluation(GetTime(), mLocalTrackId, mEvaluationEpoch);
   }
   return cDELETE;
}

WsfTrackStateController_EvaluateRequest::WsfTrackStateController_EvaluateRequest(double                   aTimeRequired,
                                                                                 WsfTrackStateController* aControllerPtr,
                                                                                 WsfLocalTrackStatus*     aStatusPtr)
   : WsfTimeDelayQueue::Request(aTimeRequired)
   , mControllerPtr(aControllerPtr)
   , mLocalTrackId(aStatusPtr->GetLocalTrackId())
   , mEvaluationEpoch(aStatusPtr->EvaluationEpoch())
{
}

WsfTrackStateController_EvaluateRequest::~WsfTrackStateController_EvaluateRequest() {}

bool WsfTrackStateController_EvaluateRequest::CompleteRequest(double aSimTime)
{
   double timeRequired = mControllerPtr->EvaluationComplete(aSimTime, mLocalTrackId, mEvaluationEpoch);
   SetTimeRequired(timeRequired);
   return (timeRequired < 0.0);
}

void WsfTrackStateController::PreConstruct()
{
   mPlatformPtr             = nullptr;
   mProcessorPtr            = nullptr;
   mThinkerPtr              = nullptr;
   mContextPtr              = nullptr;
   mOnTrackDropScriptPtr    = nullptr;
   mStateMachinePtr         = nullptr;
   mTrackManagerPtr         = nullptr;
   mTrackListPtr            = nullptr;
   mEvaluateCandidateTracks = false;
   mShowStateTransitions    = false;
   mClearSignalOnStateExit = false, mIsTurnedOn = true;
   mEvaluationInterval = -1.0;
   mTransitionTime     = -1.0;
   mEvaluationSimTime  = -1.0;
   mEvaluationTrackPtr = nullptr;
   mSignalVarPtr       = nullptr;
   mStatusVarPtr       = nullptr;
   mTrackVarPtr        = nullptr;
}

WsfTrackStateController::WsfTrackStateController()
{
   PreConstruct();
}

// =================================================================================================
WsfTrackStateController::WsfTrackStateController(WsfScriptContext& aContext)
{
   PreConstruct();
   mContextPtr      = &aContext;
   mStateMachinePtr = new TrackStateMachine(*mContextPtr, this);

   // Add additional exposed script variables
   mContextPtr->DeclareVariable("WsfLocalTrackStatus", "STATUS");
   mContextPtr->DeclareVariable("string", "SIGNAL");
   mContextPtr->DeclareVariable("WsfLocalTrack", "TRACK");
}

// =================================================================================================
// protected
WsfTrackStateController::WsfTrackStateController(const WsfTrackStateController& aSrc, WsfScriptContext& aContext)
{
   PreConstruct();
   mContextPtr = &aContext;

   mTimeToEvalByName        = aSrc.mTimeToEvalByName;
   mTimeToEvalByIndex       = aSrc.mTimeToEvalByIndex;
   mEvalIntervalByName      = aSrc.mEvalIntervalByName;
   mEvaluateCandidateTracks = aSrc.mEvaluateCandidateTracks;
   mShowStateTransitions    = aSrc.mShowStateTransitions;

   // Add additional exposed script variables
   mContextPtr->DeclareVariable("WsfLocalTrackStatus", "STATUS");
   mContextPtr->DeclareVariable("string", "SIGNAL");
   mContextPtr->DeclareVariable("WsfLocalTrack", "TRACK");

   mStateMachinePtr = new TrackStateMachine(*aSrc.mStateMachinePtr, *mContextPtr, this);
}

// =================================================================================================
// virtual
WsfTrackStateController::~WsfTrackStateController()
{
   delete mStateMachinePtr;
}

// =================================================================================================
// virtual
bool WsfTrackStateController::Initialize(double             aSimTime,
                                         WsfProcessor*      aProcessorPtr,
                                         WsfTimeDelayQueue* aThinkerPtr,
                                         WsfTrackManager*   aTrackManagerPtr)
{
   bool ok = true;

   mProcessorPtr    = aProcessorPtr;
   mPlatformPtr     = mProcessorPtr->GetPlatform();
   mThinkerPtr      = aThinkerPtr;
   mTrackManagerPtr = aTrackManagerPtr;
   mTrackListPtr    = &(mTrackManagerPtr->GetTrackList());

   // Initialize the state machine if any states were defined. A simple subordinate that only accepts
   // commands and doesn't command anything will typically not have a state machine.

   if (mStateMachinePtr->GetStateCount() > 0)
   {
      ok &= mStateMachinePtr->Initialize(aSimTime);

      // Ensure that each evaluation_interval refers to a valid state.

      for (const auto& ei : mEvalIntervalByName)
      {
         if (mStateMachinePtr->GetStateIndex(ei.first) == 0)
         {
            auto out = ut::log::error() << "Invalid state for evaluation_interval.";
            out.AddNote() << "State: " << ei.first;
            ok = false;
         }
      }

      // Ensure that each state has an evaluation_interval.
      // This also initializes the run-time structures for evaluation time.

      int stateCount = mStateMachinePtr->GetStateCount();
      mEvalIntervalByIndex.resize(stateCount + 1);
      mTimeToEvalByIndex.resize(stateCount + 1, WsfRandomVariable(0.01, WsfRandomVariable::cPOSITIVE));
      mTracksInState.resize(stateCount + 1, 0);
      for (int stateIndex = 1; stateIndex <= stateCount; ++stateIndex)
      {
         WsfStringId                                        stateName = mStateMachinePtr->GetStateName(stateIndex);
         std::map<WsfStringId, WsfRandomVariable>::iterator eii       = mEvalIntervalByName.find(stateName);
         if (eii != mEvalIntervalByName.end())
         {
            ok &= (*eii).second.Initialize("evaluation_interval",
                                           mProcessorPtr,
                                           mContextPtr,
                                           *GetSimulation()); // Resolve possible script variable references
            mEvalIntervalByIndex[stateIndex] = (*eii).second;
         }
         else
         {
            auto out = ut::log::error() << "Unspecified evaluation_interval for state.";
            out.AddNote() << "State: " << stateName;
            ok = false;
         }

         std::map<WsfStringId, WsfRandomVariable>::iterator ttei = mTimeToEvalByName.find(stateName);
         if (ttei != mTimeToEvalByName.end())
         {
            ok &= (*ttei).second.Initialize("time_to_evaluate",
                                            mProcessorPtr,
                                            mContextPtr,
                                            *GetSimulation()); // Resolve possible script variable references
            mTimeToEvalByIndex[stateIndex] = (*ttei).second;
         }
      }

      // Use the first state variables for the entry state variables.
      mEvalIntervalByIndex[0] = mEvalIntervalByIndex[1];
      mTimeToEvalByIndex[0]   = mTimeToEvalByIndex[1];
   }

   // Capture the 'on_track_drop' script pointer to avoid constant lookups.
   mOnTrackDropScriptPtr = mContextPtr->FindScript("on_track_drop");
   mSignalVarPtr         = &mContextPtr->GetContext().Var("SIGNAL");
   mStatusVarPtr         = &mContextPtr->GetContext().Var("STATUS");
   mTrackVarPtr          = &mContextPtr->GetContext().Var("TRACK");
   return ok;
}

// =================================================================================================
// virtual
bool WsfTrackStateController::ProcessInput(const WsfScenario& aScenario, UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "evaluation_interval")
   {
      // A zero evaluation interval means the transition is totally event driven
      std::string       stateName;
      WsfRandomVariable interval(WsfRandomVariable::cNON_NEGATIVE);
      aInput.ReadValue(stateName);
      interval.ProcessInput(aInput, UtInput::cTIME);
      mEvalIntervalByName[stateName] = interval;
   }
   else if (command == "time_to_evaluate")
   {
      std::string       stateName;
      WsfRandomVariable interval(WsfRandomVariable::cPOSITIVE);
      aInput.ReadValue(stateName);
      interval.ProcessInput(aInput, UtInput::cTIME);
      mTimeToEvalByName[stateName] = interval;
   }
   else if (command == "evaluate_candidate_tracks")
   {
      aInput.ReadValue(mEvaluateCandidateTracks);
   }
   else if (command == "show_transitions")
   {
      ut::log::warning() << "'show_transitions' has been deprecated. Use 'show_state_transitions'.";
      mShowStateTransitions = true;
   }
   else if (command == "show_state_transitions")
   {
      // We take over the state machines' 'show_state_transitions' as we provide more data.
      mShowStateTransitions = true;
   }
   else if (command == "on_track_drop")
   {
      mContextPtr->CompileImplicitScript(aInput, "on_track_drop", "void");
   }
   else if (mStateMachinePtr->ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
// virtual
bool WsfTrackStateController::ProcessMessage(double aSimTime, const WsfMessage& aMessage)
{
   bool messageProcessed = false;
   if (aMessage.GetType() == WsfStatusMessage::GetTypeId())
   {
      const WsfStatusMessage& message = dynamic_cast<const WsfStatusMessage&>(aMessage);
      if (message.GetStatusId() == "SIGNAL")
      {
         // This is an out-of-band request to evaluate a track.
         //
         // NOTE: We do not use a thinker for this.

         messageProcessed                  = true;
         const WsfTrackId&    localTrackId = message.GetRequestId();
         WsfLocalTrackStatus* statusPtr    = mTrackStatusList.Find(localTrackId);
         if (statusPtr != nullptr)
         {
            // Tell the state exit processor that we need to clear the SIGNAL variable on a state exit.
            // This is needed to prevent potential cascading through the states.

            mClearSignalOnStateExit = true;
            mSignalVarPtr->SetString(message.GetStatus());

            // When evaluating states in an out-of-band request, no evaluation intervals
            // or thinking delays are imposed. State transitions are performed immediately
            // UNLESS the user imposes a transition delay (via way of SetTransitionTime).

            int  begStateIndex = statusPtr->GetStateIndex();
            bool stateChanged  = true;
            while (stateChanged)
            {
               int oldStateIndex = statusPtr->GetStateIndex();
               if (!EvaluateTrack(aSimTime, statusPtr))
               {
                  statusPtr = nullptr;
                  break; // track was dropped.
               }
               stateChanged = (statusPtr->GetStateIndex() != oldStateIndex);

               if (stateChanged && (mTransitionTime > 0.0))
               {
                  break; // change of state with a non-zero transition time.
               }
            }

            // If there was no state change then we don't do anything with evaluation intervals.
            // If there was a state change then we cancel any pending periodic evaluation and start
            // a new interval if one is defined.

            if ((statusPtr != nullptr) && (statusPtr->GetStateIndex() != begStateIndex))
            {
               // Cancel any pending periodic evaluation.
               statusPtr->UpdateEvaluationEpoch();

               // If the above evaluation loop was terminated because of a state transition with an attending
               // user-defined transition time, an evaluation event is scheduled for the end of the transition time.
               //
               // Otherwise, a new state has been entered and the first periodic review needs to be scheduled
               // (if non-zero evaluation interval has been defined for the new state).

               double nextEvalTime = mTransitionTime;
               if (nextEvalTime <= 0.0)
               {
                  nextEvalTime = mEvaluationInterval;
                  if (nextEvalTime < 0.0)
                  {
                     nextEvalTime = mEvalIntervalByIndex[statusPtr->GetStateIndex()].Draw();
                  }
               }

               // Schedule a the next evaluation if one has been defined.
               if (nextEvalTime > 0.0)
               {
                  GetSimulation()->AddEvent(
                     ut::make_unique<WsfTrackStateController_RequestEvaluationEvent>(aSimTime + nextEvalTime, this, statusPtr));
               }
            }
         }
      }
   }
   return messageProcessed;
}

// =================================================================================================
//! Enter a state directly without evaluating the transition rules for the current state.
//! This method exits the current state, enters the requested state and schedules an immediate
//! evaluation of the rules for the new state.
//! @param aSimTime      The current simulation time.
//! @param aLocalTrackId The local track ID of the track.
//! @param aNewStateName The name (string ID) of the desired state.
//! @note This method does nothing if the track no longer exists or the state is invalid.
//! If the selected state is the same as the current state then it simply schedules a re-evaluation
//! (it does not exit and re-enter).
// virtual
void WsfTrackStateController::EnterState(double aSimTime, const WsfTrackId& aLocalTrackId, WsfStringId aNewStateName)
{
   WsfLocalTrackStatus* statusPtr = mTrackStatusList.Find(aLocalTrackId);
   if (statusPtr != nullptr)
   {
      WsfLocalTrack* trackPtr      = mTrackListPtr->FindTrack(aLocalTrackId);
      int            oldStateIndex = statusPtr->GetStateIndex();
      int            newStateIndex = mStateMachinePtr->GetStateIndex(aNewStateName);
      if ((trackPtr != nullptr) && (newStateIndex > 0))
      {
         if (newStateIndex != oldStateIndex)
         {
            statusPtr->UpdateTargetName(*GetSimulation(), trackPtr);
            mStatusVarPtr->GetPointer()->SetExternalManaged(statusPtr);
            mTrackVarPtr->GetPointer()->SetExternalManaged(trackPtr);
            mEvaluationSimTime  = aSimTime;
            mEvaluationTrackPtr = trackPtr;
            // We can't exit the old state if we really never entered it...
            if (!statusPtr->TransitionPending())
            {
               mStateMachinePtr->ExitState(oldStateIndex);
            }
            statusPtr->SetTransitionPending(false);
            mStateMachinePtr->EnterState(newStateIndex);
            mEvaluationSimTime = -1.0;
            statusPtr->SetState(aSimTime, newStateIndex, mStateMachinePtr->GetStateName(newStateIndex));
            --mTracksInState[oldStateIndex];
            ++mTracksInState[newStateIndex];
         }

         // Cancel any pending periodic evaluation.
         statusPtr->UpdateEvaluationEpoch();

         // Request an evaluation of the new state.
         GetSimulation()->AddEvent(
            ut::make_unique<WsfTrackStateController_RequestEvaluationEvent>(aSimTime, this, statusPtr));
      }
   }
}

// =================================================================================================
//! This is called indirectly from the request queue to indicate that the evaluation has been completed.
//!
//! This is called when the 'evaluation time' (i.e.: the logical time that it takes to perform an evaluation)
//! has elapsed. It is at this point we actually invoke the physical code to complete the evaluation (i.e.:
//! the physical code is executed on the 'falling edge' of the evaluation time).
//!
//! @param aSimTime         The current simulation time.
//! @param aLocalTrackId    The ID of the local track that is being evaluated.
//! @param aEvaluationEpoch The epoch used for check for obsolete evaluations.
//! @returns If less than zero then the request is complete. If greater than or equal to zero then it is
//! the additional time that is required to complete the request.
// virtual
double WsfTrackStateController::EvaluationComplete(double            aSimTime,
                                                   const WsfTrackId& aLocalTrackId,
                                                   unsigned int      aEvaluationEpoch)
{
   double timeRequired = -1.0;

   // Complete the event associated with the thinker
   WsfLocalTrackStatus* statusPtr = mTrackStatusList.Find(aLocalTrackId);
   if ((statusPtr != nullptr) && (statusPtr->EvaluationEpoch() == aEvaluationEpoch))
   {
      // Indicate there is no need to clear SIGNAL on state exit (we're doing it here...).
      mClearSignalOnStateExit = false;
      mSignalVarPtr->SetString("");

      int  oldStateIndex        = statusPtr->GetStateIndex();
      bool oldTransitionPending = statusPtr->TransitionPending();
      if (EvaluateTrack(aSimTime, statusPtr))
      {
         if (oldTransitionPending)
         {
            // A state transition was pending on entry, so it should be fulfilled.
            // Extend the evaluation interval to simulate the thinking time for the first evaluation.

            timeRequired = mTimeToEvalByIndex[statusPtr->GetStateIndex()].Draw();
         }
         else if (statusPtr->GetStateIndex() == oldStateIndex)
         {
            // A state transition did not occur. Schedule the next periodic evaluation.
            // If the evaluation interval is zero then there is no periodic review (exiting the state can
            // only occur because of some external influence).

            double evalInterval = mEvaluationInterval;
            if (evalInterval < 0.0)
            {
               evalInterval = mEvalIntervalByIndex[statusPtr->GetStateIndex()].Draw();
            }
            if (evalInterval > 0.0)
            {
               GetSimulation()->AddEvent(
                  ut::make_unique<WsfTrackStateController_RequestEvaluationEvent>(aSimTime + evalInterval, this, statusPtr));
            }
         }
         else
         {
            // A state transition has been declared.
            //
            // If a dynamically-defined transition time has been defined then schedule a new evaluation
            // event to process entry into the new state and perform the first evaluation.
            //
            // If no transition time was defined then 'continue' thinking for 0 seconds which will then cause this
            // method to be recalled to process the entry into the new state and to perform the first evaluation.

            if (mTransitionTime > 0.0)
            {
               GetSimulation()->AddEvent(
                  ut::make_unique<WsfTrackStateController_RequestEvaluationEvent>(aSimTime + mTransitionTime, this, statusPtr));
            }
            else
            {
               timeRequired = 0.0;
            }
         }
      }
   }
   return timeRequired;
}

// =================================================================================================
//! Request that an evaluation be performed on the specified local track.
//! This is called from the RequestEvaluationEvent to start the process of evaluating a track.
//! An evaluation request is formed and passed to the request queue where it will be assigned
//! a 'thinker'. If the local track status has been dropped then the request will be ignored.
//!
//! @param aSimTime         The current simulation time.
//! @param aLocalTrackId    The ID of the local track to be evaluated.
//! @param aEvaluationEpoch The evaluation epoch used for detecting obsolete events.
// virtual
void WsfTrackStateController::RequestEvaluation(double aSimTime, const WsfTrackId& aLocalTrackId, unsigned int aEvaluationEpoch)
{
   WsfLocalTrackStatus* statusPtr = mTrackStatusList.Find(aLocalTrackId);
   if (statusPtr != nullptr)
   {
      if (aEvaluationEpoch == statusPtr->EvaluationEpoch())
      {
         SubmitRequest(aSimTime, statusPtr);
      }
   }
   else
   {
      DeleteTrack(aSimTime, aLocalTrackId);
   }
}

// =================================================================================================
//! Return the tracks that are in the specified state name (ID).
//! @param aStateName The name (ID) of the state of interest.
//! @param aTrackList The tracks in the state of interest.
// virtual
void WsfTrackStateController::TracksInState(WsfStringId aStateName, std::vector<WsfLocalTrack*>& aTrackList)
{
   aTrackList.resize(0);
   int stateIndex = mStateMachinePtr->GetStateIndex(aStateName);
   if (stateIndex > 0)
   {
      unsigned int count = ut::safe_cast<unsigned int, int>(mTracksInState[stateIndex]);
      aTrackList.reserve(count);
      for (unsigned int i = 0; i < mTrackStatusList.GetCount(); ++i)
      {
         WsfLocalTrackStatus* statusPtr = mTrackStatusList.GetEntry(i);
         if (statusPtr->GetStateIndex() == stateIndex)
         {
            WsfLocalTrack* trackPtr = mTrackListPtr->FindTrack(statusPtr->GetLocalTrackId());
            if (trackPtr != nullptr)
            {
               aTrackList.push_back(trackPtr);
            }
         }
      }
   }
}

// =================================================================================================
//! Return the count of tracks in the specified state name (ID).
//! @param aStateName The name (ID) of the state of interest.
// virtual
int WsfTrackStateController::TracksInState(WsfStringId aStateName)
{
   int count      = 0;
   int stateIndex = mStateMachinePtr->GetStateIndex(aStateName);
   if (stateIndex > 0)
   {
      count = mTracksInState[stateIndex];
   }
   return count;
}

// =================================================================================================
//! Returns the state name (ID) of a given track.
//! @param aLocalTrackId The track ID of the local track to be evaluated.
//! @returns The state name (ID) in which the track is currently in.
// virtual
WsfStringId WsfTrackStateController::StateOfTrack(const WsfTrackId& aLocalTrackId)
{
   WsfLocalTrackStatus* statusPtr = mTrackStatusList.Find(aLocalTrackId);
   if (statusPtr != nullptr)
   {
      return statusPtr->GetStateName();
   }
   else
   {
      return nullptr;
   }
}

// =================================================================================================
//! The following is invoked from the state machine to indicate that we are entering a new state.
//! @param aStateIndex The state that we are entering.
void WsfTrackStateController::EnterState(int aStateIndex)
{
   if ((mEvaluationSimTime >= 0) && (mEvaluationTrackPtr != nullptr))
   {
      WsfObserver::StateEntry(GetSimulation())(mEvaluationSimTime,
                                               mProcessorPtr,
                                               mEvaluationTrackPtr,
                                               mStateMachinePtr->GetStateName(aStateIndex));
      if (mShowStateTransitions)
      {
         std::string targetName("<UNKNOWN>");
         if (mEvaluationTrackPtr->GetTargetName() != 0)
         {
            targetName = mEvaluationTrackPtr->GetTargetName();
         }
         else
         {
            WsfPlatform* platformPtr = GetSimulation()->GetPlatformByIndex(mEvaluationTrackPtr->GetTargetIndex());
            if (platformPtr != nullptr)
            {
               targetName = platformPtr->GetName();
            }
         }

         auto out = ut::log::debug() << "Entering state.";
         out.AddNote() << "T = " << mEvaluationSimTime;
         out.AddNote() << "State: " << mStateMachinePtr->GetStateName(aStateIndex);
         out.AddNote() << "Platform: " << mPlatformPtr->GetName();
         out.AddNote() << "Processor: " << mProcessorPtr->GetName();
         out.AddNote() << "Target: " << targetName;
         out.AddNote() << "Track ID: " << mEvaluationTrackPtr->GetTrackId();
      }
   }
}

// =================================================================================================
//! The following is invoked from the state machine to indicate that we are exiting the current state.
//! @param aStateIndex The state that we are leaving.
void WsfTrackStateController::ExitState(int aStateIndex)
{
   if ((mEvaluationSimTime >= 0) && (mEvaluationTrackPtr != nullptr))
   {
      WsfObserver::StateExit(GetSimulation())(mEvaluationSimTime,
                                              mProcessorPtr,
                                              mEvaluationTrackPtr,
                                              mStateMachinePtr->GetStateName(aStateIndex));
      if (mShowStateTransitions)
      {
         std::string targetName("<UNKNOWN>");
         if (mEvaluationTrackPtr->GetTargetName() != 0)
         {
            targetName = mEvaluationTrackPtr->GetTargetName();
         }
         else
         {
            WsfPlatform* platformPtr = GetSimulation()->GetPlatformByIndex(mEvaluationTrackPtr->GetTargetIndex());
            if (platformPtr != nullptr)
            {
               targetName = platformPtr->GetName();
            }
         }

         auto out = ut::log::debug() << "Exiting state.";
         out.AddNote() << "T = " << mEvaluationSimTime;
         out.AddNote() << "State: " << mStateMachinePtr->GetStateName(aStateIndex);
         out.AddNote() << "Platform: " << mPlatformPtr->GetName();
         out.AddNote() << "Processor: " << mProcessorPtr->GetName();
         out.AddNote() << "Target: " << targetName;
         out.AddNote() << "Track ID: " << mEvaluationTrackPtr->GetTrackId();
      }
   }

   //! When processing a SIGNAL message, we must clear the SIGNAL variable when the state is exited.
   if (mClearSignalOnStateExit)
   {
      mSignalVarPtr->SetNullString();
   }
}

// =================================================================================================
//! The processor to which we're attached has been turned off.
// virtual
void WsfTrackStateController::TurnOff(double aSimTime)
{
   mIsTurnedOn = false;

   // Clear everything from the track status list.
   mTrackStatusList.DeleteAll();

   // Disconnect all the track manager callbacks.
   mCallbacks.Clear();
}

// =================================================================================================
//! The processor to which we're attached has been turned on.
// virtual
void WsfTrackStateController::TurnOn(double aSimTime)
{
   mIsTurnedOn = true;
   ConnectCallbacks();


   // Internally discover all the tracks that are currently in the track manager.
   unsigned int trackCount = mTrackListPtr->GetTrackCount();
   for (unsigned int trackIndex = 0; trackIndex < trackCount; ++trackIndex)
   {
      DiscoverTrack(aSimTime, mTrackListPtr->GetTrackEntry(trackIndex));
   }
}

// =================================================================================================
//! Drop a local track (status).
//! A local track has been dropped, so delete the associated local track status.
//! @param aSimTime      The current simulation time.
//! @param aLocalTrackId The track ID of the local track (status) to be dropped.
// protected
void WsfTrackStateController::DeleteTrack(double aSimTime, const WsfTrackId& aLocalTrackId)
{
   // Find the pointer to track status entry (if it exists) and remove it from the list,
   // but don't delete the entry itself (see below for the reason).
   WsfLocalTrackStatus* statusPtr = mTrackStatusList.Remove(aLocalTrackId).release();
   if (statusPtr != nullptr)
   {
      int stateIndex = statusPtr->GetStateIndex();
      --mTracksInState[stateIndex];
      if (mShowStateTransitions)
      {
         auto out = ut::log::debug() << "Purging track status.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << mPlatformPtr->GetName();
         out.AddNote() << "Processor: " << mProcessorPtr->GetName();
         out.AddNote() << "Track ID: " << aLocalTrackId;
      }
      //! Formerly, whenever a track is dropped DeleteTrack() would also delete the associated TrackStatus entry.
      //! The problem is that external methods invoked by EvaluateTrack() or EnterState() process MAY cause a
      //! track (and the associated TrackStatus entry) to be deleted, thus invalidating the local pointer these
      //! routines have to the TrackStatus entry!
      //! This event simply serves as a way to delay the deletion of the TrackStatus until a more opportune time.
      GetSimulation()->AddEvent(ut::make_unique<WsfOneShotEvent>(aSimTime, [statusPtr]() { delete statusPtr; }));

      // Create a fake track and invoke the observer to exit the last known state and enter the 'dropped'.
      WsfTrack track;
      track.SetTrackId(aLocalTrackId);
      track.SetTargetName(statusPtr->GetTargetName());
      WsfObserver::StateExit(GetSimulation())(aSimTime, mProcessorPtr, &track, mStateMachinePtr->GetStateName(stateIndex));
      WsfObserver::StateEntry(GetSimulation())(aSimTime, mProcessorPtr, &track, nullptr);
   }
}

// =================================================================================================
//! Discover a local track.
//! We have been informed of the existence of a new track (by either the track manager or by turning
//! on the processor and searching the track list). Schedule the initial evaluation of the track.
//! @param aSimTime  The current simulation time.
//! @param aTrackPtr The pointer to the track that has been discovered.
// protected
void WsfTrackStateController::DiscoverTrack(double aSimTime, const WsfLocalTrack* aTrackPtr)
{
   const WsfTrackId& trackId = aTrackPtr->GetTrackId();
   if (!mTrackStatusList.Find(trackId))
   {
      if (mShowStateTransitions)
      {
         auto out = ut::log::debug() << "Discovered track.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << mPlatformPtr->GetName();
         out.AddNote() << "Processor: " << mProcessorPtr->GetName();
         out.AddNote() << "Target: " << aTrackPtr->GetTargetName();
         out.AddNote() << "Track ID: " << trackId;
      }
      auto statusPtr = ut::make_unique<WsfLocalTrackStatus>(aSimTime, *GetSimulation(), aTrackPtr);
      if (!mTracksInState.empty())
      {
         int stateIndex = mStateMachinePtr->GetInitialStateIndex();
         statusPtr->SetState(aSimTime, stateIndex, mStateMachinePtr->GetStateName(stateIndex));
         statusPtr->SetTransitionPending(true);
         ++mTracksInState[stateIndex];
         // Schedule the first evaluation for this track to dispatch at the current time.
         GetSimulation()->AddEvent(
            ut::make_unique<WsfTrackStateController_RequestEvaluationEvent>(aSimTime, this, statusPtr.get()));
      }

      mTrackStatusList.Add(std::move(statusPtr), trackId);
   }
   else
   {
      auto out = ut::log::debug() << "Discovered existing track.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << mPlatformPtr->GetName();
      out.AddNote() << "Processor: " << mProcessorPtr->GetName();
      out.AddNote() << "Target: " << aTrackPtr->GetTargetName();
      out.AddNote() << "Track ID: " << trackId;
   }
}

// =================================================================================================
//! Evaluate the status of a single local track.
//! Only the current state will be evaluated. It is the responsibility of the caller to recall this
//! routine if they want to perform as many transitions as possible.
//!
//! @param aSimTime The current simulation time.
//! @param aStatusPtr The pointer to the local track status to be evaluated.
//! @returns 'true' if evaluated or 'false' if the corresponding local track does not exist.
// protected
bool WsfTrackStateController::EvaluateTrack(double aSimTime, WsfLocalTrackStatus* aStatusPtr)
{
   WsfTrackId     trackId(aStatusPtr->GetLocalTrackId());
   WsfLocalTrack* trackPtr = mTrackListPtr->FindTrack(trackId);
   if (trackPtr != nullptr)
   {
      // This check probably isn't necessary as we wouldn't 'discover' a candidate track unless we're also
      // allowing the evaluation of them (see OnPlatformUpdated). However, I'm leaving this here in case we
      // ever allow a transition back to candidate status.

      if (mEvaluateCandidateTracks || (!trackPtr->IsCandidate()))
      {
         int oldStateIndex = aStatusPtr->GetStateIndex();
         if (mStateMachinePtr->ShowStateEvaluations())
         {
            auto out = ut::log::debug() << "Evaluating state.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "State: " << mStateMachinePtr->GetStateName(oldStateIndex);
            out.AddNote() << "Platform: " << mPlatformPtr->GetName();
            out.AddNote() << "Processor: " << mProcessorPtr->GetName();
            out.AddNote() << "Track ID: " << trackId;
         }
         aStatusPtr->UpdateTargetName(*GetSimulation(), trackPtr);
         mStatusVarPtr->GetPointer()->SetExternalManaged(aStatusPtr);
         mTrackVarPtr->GetPointer()->SetExternalManaged(trackPtr);

         mEvaluationSimTime  = aSimTime;
         mEvaluationTrackPtr = trackPtr;
         mEvaluationInterval = -1.0;
         mTransitionTime     = -1.0;

         // Locking the track prevents it from being deleted during the evaluation process, which would
         // invalidate the TRACK script variable!
         mTrackManagerPtr->LockTrack(aSimTime, trackPtr);

         if (aStatusPtr->TransitionPending())
         {
            aStatusPtr->SetTransitionPending(false);
            mStateMachinePtr->EnterState(oldStateIndex);
         }
         else
         {
            int newStateIndex  = mStateMachinePtr->EvaluateState(oldStateIndex, false);
            mEvaluationSimTime = -1.0;
            if (oldStateIndex != newStateIndex)
            {
               aStatusPtr->SetTransitionPending(true);
               aStatusPtr->SetState(aSimTime, newStateIndex, mStateMachinePtr->GetStateName(newStateIndex));
               --mTracksInState[oldStateIndex];
               ++mTracksInState[newStateIndex];
            }
         }

         mTrackManagerPtr->UnlockTrack(aSimTime, trackPtr);
      }
   }
   else
   {
      DeleteTrack(aSimTime, trackId);
   }
   return (trackPtr != nullptr);
}

// =================================================================================================
//! Assign a server (thinker) to process the evaluation for the specified track.
//!
//! @param aSimTime   The current simulation time.
//! @param aStatusPtr The pointer to the track status to be evaluated.
// protected
void WsfTrackStateController::SubmitRequest(double aSimTime, WsfLocalTrackStatus* aStatusPtr)
{
   double timeToEval = 0.0;
   if (!aStatusPtr->TransitionPending())
   {
      timeToEval = mTimeToEvalByIndex[aStatusPtr->GetStateIndex()].Draw();
   }
   WsfTrackStateController_EvaluateRequest* requestPtr =
      new WsfTrackStateController_EvaluateRequest(timeToEval, this, aStatusPtr);
   mThinkerPtr->SubmitRequest(aSimTime, requestPtr);
}

// =================================================================================================
//! Handle 'candidate track promotion' events (A callback from WsfTrackManager).
// protected
void WsfTrackStateController::CandidateTrackPromoted(double aSimTime, const WsfLocalTrack* aLocalTrackPtr)
{
   if (aLocalTrackPtr != nullptr)
   {
      // If we aren't evaluating candidate tracks then we must discover a track when it gets promoted
      // to a full local track.
      if (!mEvaluateCandidateTracks)
      {
         DiscoverTrack(aSimTime, aLocalTrackPtr);
      }
   }
}

// =================================================================================================
//! Handle 'local track initiation' events (A callback from WsfTrackManager).
// protected
void WsfTrackStateController::LocalTrackInitiated(double               aSimTime,
                                                  const WsfLocalTrack* aLocalTrackPtr,
                                                  const WsfTrack*      aRawTrackPtr)
{
   if (aLocalTrackPtr != nullptr)
   {
      // A candidate track is 'discovered' only if candidate track evaluation is enabled.
      // It will be discovered when it gets promoted.
      //
      // Non-candidate tracks are 'discovered' immediately.
      if (mEvaluateCandidateTracks || (!aLocalTrackPtr->IsCandidate()))
      {
         DiscoverTrack(aSimTime, aLocalTrackPtr);
      }
   }
}

// =================================================================================================
//! Handle 'local track dropped' events (A callback from WsfTrackManager).
// protected
void WsfTrackStateController::LocalTrackDropped(double aSimTime, const WsfLocalTrack* aLocalTrackPtr)
{
   if (aLocalTrackPtr != nullptr)
   {
      const WsfTrackId& trackId = aLocalTrackPtr->GetTrackId();
      if (mTrackStatusList.Find(trackId))
      {
         if (mShowStateTransitions)
         {
            auto out = ut::log::debug() << "Track manager dropped track.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << mPlatformPtr->GetName();
            out.AddNote() << "Processor: " << mProcessorPtr->GetName();
            out.AddNote() << "Target: " << aLocalTrackPtr->GetTargetName();
            out.AddNote() << "Track ID: " << trackId;
         }
      }
      // Invoke the on_track_drop script if defined.
      if (mOnTrackDropScriptPtr != nullptr)
      {
         mTrackVarPtr->GetPointer()->SetExternalManaged(const_cast<WsfLocalTrack*>(aLocalTrackPtr));
         mContextPtr->ExecuteScript(aSimTime, mOnTrackDropScriptPtr);
      }
   }
}

// =================================================================================================
void WsfTrackStateController::TrackManagerChanged(double aSimTime, WsfTrackManager* aNewManagerPtr)
{
   if (aNewManagerPtr)
   {
      mTrackManagerPtr = aNewManagerPtr;
      mTrackListPtr    = &(mTrackManagerPtr->GetTrackList());
   }
}

// =================================================================================================
WsfSimulation* WsfTrackStateController::GetSimulation() const
{
   return mPlatformPtr->GetSimulation();
}

// =================================================================================================
void WsfTrackStateController::ConnectCallbacks()
{
   if (mIsTurnedOn && mTrackManagerPtr)
   {
      // Register callbacks with the track manager to receive notification of needed events.
      mCallbacks.Add(mTrackManagerPtr->LocalTrackInitiated.Connect(&WsfTrackStateController::LocalTrackInitiated, this));
      mCallbacks.Add(mTrackManagerPtr->LocalTrackDropped.Connect(&WsfTrackStateController::LocalTrackDropped, this));
      mCallbacks.Add(
         mTrackManagerPtr->CandidateTrackPromoted.Connect(&WsfTrackStateController::CandidateTrackPromoted, this));
      mCallbacks.Add(mTrackManagerPtr->TrackManagerChanged.Connect(&WsfTrackStateController::TrackManagerChanged, this));
   }
}
