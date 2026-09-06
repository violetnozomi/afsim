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

#ifndef WSFTRACKSTATECONTROLLER_HPP
#define WSFTRACKSTATECONTROLLER_HPP

#include "wsf_export.h"

#include <map>
#include <string>
#include <vector>

#include "UtCallbackHolder.hpp"
#include "UtGenericMappedList.hpp"
#include "WsfLocalTrackStatus.hpp"
class WsfMessage;
class WsfPlatform;
class WsfProcessor;
#include "WsfRandomVariable.hpp"
class WsfScriptContext;
#include "script/WsfScriptStateMachine.hpp"
class WsfTimeDelayQueue;
#include "WsfTrackId.hpp"
#include "WsfTrackList.hpp"
class WsfTrackManager;

//! A processor for directing a set of local tracks through a state machine.
//!
//! This processor takes a collection of local tracks (as maintained by
//! WsfTrackManager or some equivalent), associates a 'state' with each track,
//! and in response to calls to Update() and ProcessMessage(), will invoke the
//! state machine to potentially update the 'state' of the tracks.
//!
//! The states of the state machine may be used to represent anything that is of
//! consequence to the user. The transition rules in
//! the state machine may allocate resources, issue commands, etc., that will cause
//! to state of the local track to change.

class WSF_EXPORT WsfTrackStateController
{
public:
   WsfTrackStateController();
   WsfTrackStateController(WsfScriptContext& aContext);
   WsfTrackStateController(const WsfTrackStateController& aSrc, WsfScriptContext& aContext);
   virtual ~WsfTrackStateController();

   //! Get the platform to which the controller is attached.
   WsfPlatform* GetPlatform() const { return mPlatformPtr; }

   //! Get the processor to which the controller is attached.
   WsfProcessor* GetProcessor() const { return mProcessorPtr; }

   //! @name Common framework methods.
   //@{

   virtual bool Initialize(double             aSimTime,
                           WsfProcessor*      aProcessorPtr,
                           WsfTimeDelayQueue* aThinkerPtr,
                           WsfTrackManager*   aTrackManagerPtr);
   virtual bool ProcessInput(const WsfScenario& aScenario, UtInput& aInput);
   virtual bool ProcessMessage(double aSimTime, const WsfMessage& aMessage);

   bool         IsTurnedOn() const { return mIsTurnedOn; }
   virtual void TurnOff(double aSimTime);
   virtual void TurnOn(double aSimTime);

   void ConnectCallbacks();


   //@}

   //! Dynamically redefine the evaluation interval to be used for scheduling the next evaluation.
   //! @note This is provided to be called by user scripts during the current evaluation.
   void SetEvaluationInterval(double aEvaluationInterval) { mEvaluationInterval = aEvaluationInterval; }

   //! Dynamically specify the transition delay to be applied when transitioning to a new state.
   //! @note This is provided to be called by user scripts during the current evaluation.
   void SetTransitionTime(double aTransitionTime) { mTransitionTime = aTransitionTime; }

   virtual void EnterState(double aSimTime, const WsfTrackId& aTrackId, WsfStringId aNewStateName);

   virtual void TracksInState(WsfStringId aStateName, std::vector<WsfLocalTrack*>& aTrackList);

   virtual int TracksInState(WsfStringId aStateName);

   virtual WsfStringId StateOfTrack(const WsfTrackId& aLocalTrackId);

   WsfSimulation* GetSimulation() const;

   //! @name Callbacks from the state machine (not part of the public interface).
   //@{

   virtual void EnterState(int aStateIndex);
   virtual void ExitState(int aStateIndex);

   virtual double EvaluationComplete(double aSimTime, const WsfTrackId& aLocalTrackId, unsigned int aEvaluationEpoch);

   virtual void RequestEvaluation(double aSimTime, const WsfTrackId& aLocalTrackId, unsigned int aEvaluationEpoch);

   class TrackStateMachine : public WsfScriptStateMachine
   {
   public:
      TrackStateMachine()
         : mControllerPtr(nullptr)
      {
      }

      TrackStateMachine(WsfScriptContext& aContext, WsfTrackStateController* aControllerPtr)
         : WsfScriptStateMachine(aContext)
         , mControllerPtr(aControllerPtr)
      {
      }
      TrackStateMachine(const TrackStateMachine& aSrc, WsfScriptContext& aContext, WsfTrackStateController* aControllerPtr)
         : WsfScriptStateMachine(aSrc, aContext)
         , mControllerPtr(aControllerPtr)
      {
      }
      void EnterStateNotify(int aStateIndex) override { mControllerPtr->EnterState(aStateIndex); }
      void ExitStateNotify(int aStateIndex) override { mControllerPtr->ExitState(aStateIndex); }

   private:
      WsfTrackStateController* mControllerPtr;
   };
   //@}

protected:
   void PreConstruct();

   void DeleteTrack(double aSimTime, const WsfTrackId& aLocalTrackId);

   void DiscoverTrack(double aSimTime, const WsfLocalTrack* aTrackPtr);

   bool EvaluateTrack(double aSimTime, WsfLocalTrackStatus* aStatusPtr);

   void SubmitRequest(double aSimTime, WsfLocalTrackStatus* aStatusPtr);

   void CandidateTrackPromoted(double aSimTime, const WsfLocalTrack* aLocalTrackPtr);

   void LocalTrackInitiated(double aSimTime, const WsfLocalTrack* aLocalTrackPtr, const WsfTrack* aRawTrackPtr);

   void LocalTrackDropped(double aSimTime, const WsfLocalTrack* aLocalTrackPtr);

   void TrackManagerChanged(double aSimTime, WsfTrackManager* aNewManagerPtr);

   WsfPlatform*                                  mPlatformPtr;
   WsfProcessor*                                 mProcessorPtr;
   WsfTimeDelayQueue*                            mThinkerPtr;
   WsfScriptContext*                             mContextPtr;
   UtScript*                                     mOnTrackDropScriptPtr;
   TrackStateMachine*                            mStateMachinePtr;
   WsfTrackManager*                              mTrackManagerPtr;
   UtCallbackHolder                              mCallbacks;
   WsfLocalTrackList*                            mTrackListPtr;
   UtMappedList<WsfLocalTrackStatus, WsfTrackId> mTrackStatusList;

   //! The time it takes to evaluate a state by state name.
   //! This is only used during input processing and is invalid after initialization.
   std::map<WsfStringId, WsfRandomVariable> mTimeToEvalByName;
   //! The time it takes to evaluate a state by state index.
   std::vector<WsfRandomVariable> mTimeToEvalByIndex;

   //! Evaluation interval by state name.
   //! This is only used during input processing and is invalid after initialization.
   std::map<WsfStringId, WsfRandomVariable> mEvalIntervalByName;
   //! Evaluation interval by state index.
   std::vector<WsfRandomVariable> mEvalIntervalByIndex;

   //! The number of tracks in a state, indexed by state index.
   std::vector<int> mTracksInState;

   bool mEvaluateCandidateTracks;
   bool mShowStateTransitions;
   bool mClearSignalOnStateExit;
   bool mIsTurnedOn;

   //! The dynamically-defined evaluation interval.
   //! If greater than zero, this value overrides the evaluation_interval for the current state.
   //! It is set to -1 prior to performing state evaluation. The user may call SetEvaluationInterval()
   //! during evaluation to update the value.
   double mEvaluationInterval;

   //! The dynamically-defined transition delay.
   //! If greater than zero, this value defines the time that must elapsed between exiting the
   //! current state and entering the next state. It is set to -1 prior to performing state evaluation.
   //! The user may call SetTransitionTime() during evaluation to update the value.
   double mTransitionTime;

   //! The following are used only to propagate data from the evaluator to the state machine enter/exit
   //! state callbacks.
   double         mEvaluationSimTime;
   WsfTrackId     mEvaluationTrackId;
   WsfLocalTrack* mEvaluationTrackPtr;

   UtScriptData* mSignalVarPtr;
   UtScriptData* mStatusVarPtr;
   UtScriptData* mTrackVarPtr;
};

#endif
