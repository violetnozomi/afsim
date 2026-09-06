// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfOrbitalMissionSequence.hpp"

#include <iostream>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtOrbitalPropagatorBase.hpp"
#include "WsfOrbitalEvent.hpp"
#include "WsfOrbitalEventVisitor.hpp"
#include "WsfSpaceOrbitalMissionContext.hpp"

static std::string AlternateIfEmpty(const std::string& aString, const std::string& aAlternative)
{
   return aString.empty() ? aAlternative : aString;
}

WsfOrbitalMissionSequence::WsfOrbitalMissionSequence(const WsfScenario& aScenario)
   : WsfOrbitalEvent(aScenario)
   , mScenario(aScenario)
   , mMissionEvents()
   , mNextMissionEventIter()
   , mMissionEventIter()
{
   SetType(GetTypeId());
}

WsfOrbitalMissionSequence::WsfOrbitalMissionSequence(const WsfOrbitalMissionSequence& aSrc)
   : WsfOrbitalEvent(aSrc)
   , mScenario(aSrc.mScenario)
   , mMissionEvents(aSrc.mMissionEvents)
   , mNextMissionEventIter()
   , mMissionEventIter()
{
}

//! static
//! Return the maneuver type as a WsfStringId.
WsfStringId WsfOrbitalMissionSequence::GetTypeId()
{
   return "SEQUENCE";
}

//! Return the "current" MissionEvent in the MissionEvent sequence, defined to be either a
//! MissionEvent that is currently executing, or if no MissionEvent is currently executing,
//! the next MissionEvent to execute in a sequence.  If the current MissionEvent is also a sequence
//! (e.g. a Hohmann transfer), this method will continue to recurse to return the current
//! MissionEvent that is not a sequence.
//!@return A pointer to the current (non-sequence) MissionEvent object, or zero if there is no current MissionEvent.
WsfOrbitalEvent* WsfOrbitalMissionSequence::GetCurrentMissionEvent()
{
   WsfOrbitalEvent* currentMissionEventPtr = this;
   if ((mHasExecuted) && (!IsComplete()))
   {
      // For sequences, this method continues to be called until we reach a WsfOrbitalEvent object, which simply returns "this."
      currentMissionEventPtr = (*mMissionEventIter)->GetCurrentMissionEvent();
   }
   return currentMissionEventPtr;
}

//! Return the "current" MissionEvent in the MissionEvent sequence, defined to be either a
//! MissionEvent that is currently executing, or if no MissionEvent is currently executing,
//! the next MissionEvent to execute in a sequence.  If the current MissionEvent is also a sequence
//! (e.g. a Hohmann transfer), this method will continue to recurse to return the current
//! MissionEvent that is not a sequence (const version).
//!@return A const pointer to the current (non-sequence) MissionEvent object, or zero if there is no current MissionEvent.
const WsfOrbitalEvent* WsfOrbitalMissionSequence::GetCurrentMissionEvent() const
{
   const WsfOrbitalEvent* currentMissionEventPtr = this;
   if ((mHasExecuted) && (!IsComplete()))
   {
      // For sequences, this method continues to be called until we reach a WsfOrbitalEvent object, which simply returns "this."
      currentMissionEventPtr = (*mMissionEventIter)->GetCurrentMissionEvent();
   }
   return currentMissionEventPtr;
}

//! Advance to the "next" MissionEvent in the MissionEvent sequence, after the current MissionEvent.
//! (see WsfOrbitalMissionSequence::GetCurrentMissionEvent, above).  The next MissionEvent may also be a MissionEvent
//! sequence (e.g. a Hohmann transfer).  The progression of calls to AdvanceMissionEvent is to return a MissionEvent
//! sequence once, followed by aggregated MissionEvents: in the example of the Hohmann transfer, the aggregate Hohmann
//! transfer is first returned, followed by the two "atomic" MissionEvents that first change to the transfer orbit, and
//! second change to the final orbit. It is the responsibility of the caller to call Execute() on the returned
//! MissionEvent before calling this method again.  This is to allow propagation to occur up to the execution time of
//! the MissionEvent. For instance, a Hohmann transfer may have a constraint upon it to execute at a relative time.  In
//! that case, calling Execute() will advance the MissionEvent to the proper time to begin execution of the two actual
//! transfer burns associated with the MissionEvent.
//!@param aEpoch The current epoch, to be used as the basis for evaluating the next MissionEvent in the sequence.
//!@param aContext The orbital mission context in which this event is being executed.
//!@return A pointer to the next (non-sequence) MissionEvent object, or zero if there is no next MissionEvent.
//!@note Do not call Initialize on the returned MissionEvent object, as it will have already been initialized.
//!@note Call this method in a try ... catch block.  If the next MissionEvent cannot be initialized, an
//! AdvanceMissionEventExeption will
//! be thrown.  Throwing the exception, as opposed to returning a bool or some other mechanism, is done to properly
//! unwind the stack.
WsfOrbitalEvent* WsfOrbitalMissionSequence::AdvanceMissionEvent(const UtCalendar&                        aEpoch,
                                                                const wsf::space::OrbitalMissionContext& aContext)
{
   WsfOrbitalEvent* nextMissionEventPtr = WsfOrbitalEvent::AdvanceMissionEvent(aEpoch, aContext);

   if (nextMissionEventPtr == nullptr)
   {
      if (mDebug)
      {
         auto logger = ut::log::debug() << "Next MissionEvent";
         logger.AddNote() << "Type: " << GetType();
      }
      if (mMissionEventIter != mMissionEvents.end())
      {
         // First try recursive advance MissionEvent
         nextMissionEventPtr = (*mMissionEventIter)->AdvanceMissionEvent(aEpoch, aContext);

         // Only initialize if the returned MissionEvent is directly owned by this instance;
         // otherwise we are passing the value back through the recursive calls.
         if ((nextMissionEventPtr != nullptr) && (mNextMissionEventIter != mMissionEvents.end()) &&
             (nextMissionEventPtr == mNextMissionEventIter->get()))
         {
            if (mDebug)
            {
               auto logger = ut::log::debug() << "MissionEvent Initialize:";
               logger.AddNote() << "MissionEvent Type: " << nextMissionEventPtr->GetType();
               logger.AddNote() << "Type: " << GetType();
               logger.AddNote() << "Epoch: " << aEpoch;
            }
            InitializeOrBreakout(nextMissionEventPtr, aEpoch, aContext);
         }
      }
      if (nextMissionEventPtr == nullptr)
      {
         // If recursive advance didn't work, go to the next MissionEvent in this sequence.
         if (mNextMissionEventIter != mMissionEvents.end()) // A valid next MissionEvent exists.
         {
            nextMissionEventPtr = mNextMissionEventIter->get();
         }
         else if (mMissionEventIter != mMissionEvents.end()) // and a valid next MissionEvent does not exist.
         {
            aContext.CompleteMissionEvent(aEpoch, *this);

            // The sequence is complete; make sure the current MissionEvent no longer points to a valid MissionEvent.
            if (mDebug)
            {
               auto logger = ut::log::debug() << "MissionEvent End";
               logger.AddNote() << "End: " << (*mMissionEventIter)->GetType() << " on " << GetType();
               logger.AddNote() << "End: " << GetType();
               logger.AddNote() << "Epoch: " << aEpoch;
            }
            mIsComplete = true;
         }
         if (nextMissionEventPtr != nullptr)
         {
            if (mDebug)
            {
               auto logger = ut::log::debug() << "Next MissionEvent";
               logger.AddNote() << "End: " << (*mMissionEventIter)->GetType() << " on " << GetType();
               logger.AddNote() << "Initialize: " << (*mNextMissionEventIter)->GetType() << " on " << GetType();
               logger.AddNote() << "Epoch: " << aEpoch;
            }
            InitializeOrBreakout(nextMissionEventPtr, aEpoch, aContext);
         }
      }
   }

   return nextMissionEventPtr;
}

UtCalendar WsfOrbitalMissionSequence::ComputeContraintEpoch(const UtCalendar&              aEpoch,
                                                            const UtOrbitalPropagatorBase& aPropagator,
                                                            bool&                          aSuccess) const
{
   UtCalendar retval{aEpoch};
   double     timeToConstraint = 0.0;
   aSuccess                    = mCondition->GetTimeToCondition(aPropagator, timeToConstraint);
   if (aSuccess && (timeToConstraint != 0.0))
   {
      retval.AdvanceTimeBy(timeToConstraint);
   }
   return retval;
}

// virtual
bool WsfOrbitalMissionSequence::Initialize(const UtCalendar& aEpoch, const wsf::space::OrbitalMissionContext& aContext)
{
   if (aContext.GetSimulation() != nullptr)
   {
      AssignUniqueId(aContext.GetSimulation());
   }
   mMissionEventIter     = mMissionEvents.begin();
   mNextMissionEventIter = mMissionEvents.begin();
   bool ok               = true;

   if (mNextMissionEventIter != mMissionEvents.end())
   {
      mStartTime      = ComputeContraintEpoch(aEpoch, aContext.GetPropagator(), ok);
      mEvaluationTime = mStartTime;
   }

   return ok;
}

void WsfOrbitalMissionSequence::Accept(WsfOrbitalEventVisitor* aVisitorPtr)
{
   aVisitorPtr->VisitMissionSequence(this);
}

bool WsfOrbitalMissionSequence::ProcessInputWithIgnore(UtInput& aInput, bool aIgnoreSomeExceptions)
{
   std::string command   = aInput.GetCommand();
   bool        myCommand = true;
   if ((command == "maneuver") || (command == "event"))
   {
      WsfOrbitalEvent* maneuverPtr = WsfOrbitalEvent::Create(mScenario, aInput, aIgnoreSomeExceptions);
      mMissionEvents.emplace_back(std::unique_ptr<WsfOrbitalEvent>(maneuverPtr));
   }
   else
   {
      myCommand = WsfOrbitalEvent::ProcessInput(aInput);
   }
   return myCommand;
}

bool WsfOrbitalMissionSequence::IsEmpty() const
{
   return mMissionEvents.empty();
}

WsfOrbitalEvent* WsfOrbitalMissionSequence::GetNextMissionEvent() const
{
   return mNextMissionEventIter->get();
}

// virtual
// Sequences only impose an initial time delay; then defer any action to
// the composite MissionEvents.
bool WsfOrbitalMissionSequence::ExecuteEvent(const UtCalendar& aEpoch, wsf::space::OrbitalMissionContext& aContext)
{
   return true;
}

bool WsfOrbitalMissionSequence::EvaluateCompletion(const UtCalendar& aEpoch, const UtOrbitalPropagatorBase& aPropagator)
{
   return (mNextMissionEventIter == mMissionEvents.end());
}

// private
//! Once a MissionEvent has been advanced; try to initialize it.  If the initialization fails,
//! throw an AdvanceMissionEventException.
//! @param aNextMissionEventPtr The next mission event to execute.
//! @param aEpoch The time to which the next mission event should be initialized.
//! @param aContext The orbital mission context in which this event is being executed.
void WsfOrbitalMissionSequence::InitializeOrBreakout(WsfOrbitalEvent*&                        aNextMissionEventPtr,
                                                     const UtCalendar&                        aEpoch,
                                                     const wsf::space::OrbitalMissionContext& aContext)
{
   // Check for overlapping conditions (two subsequent conditions are the same).
   if ((mMissionEventIter != mMissionEvents.end()) &&
       (aNextMissionEventPtr->GetCondition().GetType() == (*mMissionEventIter)->GetCondition().GetType()) &&
       (*mMissionEventIter)->IsFinite() && (aNextMissionEventPtr->GetCondition().GetOrbitNumber() == 0))
   {
      auto logger = ut::log::warning() << "Potentially overlapping conditions for subsequent mission events.";
      logger.AddNote() << "MissionEvent: " << AlternateIfEmpty((*mMissionEventIter)->GetName(), "(unnamed)");
      logger.AddNote() << "Next MissionEvent: " << AlternateIfEmpty(aNextMissionEventPtr->GetName(), "(unnamed)");
      logger.AddNote() << "Condition: " << aNextMissionEventPtr->GetConditionString();
   }
   bool ok = aNextMissionEventPtr->Initialize(aEpoch, aContext);
   if (ok)
   {
      mMissionEventIter = mNextMissionEventIter;
      ++mNextMissionEventIter;
   }
   else
   {
      mNextMissionEventIter = mMissionEvents.end();
      mIsComplete           = true;
      auto logger           = ut::log::warning() << "MissionEvent could not execute.";
      logger.AddNote() << "Type: " << aNextMissionEventPtr->GetType();
      logger.AddNote() << "Descriptor: "
                       << aContext.GetPropagator().GetOrbitalState().GetOrbitalElements().GetDescriptor();
      aNextMissionEventPtr = nullptr;
      mIsComplete          = true;
      throw AdvanceMissionEventException();
   }
}

// virtual
bool WsfOrbitalMissionSequence::ProcessInput(UtInput& aInput)
{
   return ProcessInputWithIgnore(aInput, false);
}

//! Add a maneuver to this maneuver sequence.
//!@param aMissionEventPtr The maneuver to be added.  This instance will
//! take control of the passed object.
void WsfOrbitalMissionSequence::AddMissionEvent(std::unique_ptr<WsfOrbitalEvent>&& aMissionEventPtr)
{
   mMissionEvents.emplace_back(std::move(aMissionEventPtr));
}

//! Return the sum of the delta-v results for all mission events in the sequence.
//! Only completed events will have nonzero values,
//! so it is suggested that this method is only called when IsComplete() is valid.
double WsfOrbitalMissionSequence::GetDeltaV() const
{
   double deltaV = 0.0;
   for (auto& missionEventPtr : mMissionEvents)
   {
      deltaV += missionEventPtr->GetDeltaV();
   }
   return deltaV;
}

//! Return the sum of the required delta-v results for all mission events in the sequence.
//! so it is suggested that this method is only called when IsComplete() is valid.
double WsfOrbitalMissionSequence::GetRequiredDeltaV(const UtOrbitalPropagatorBase& aPropagator) const
{
   double deltaV = 0.0;
   for (auto& missionEventPtr : mMissionEvents)
   {
      deltaV += missionEventPtr->GetRequiredDeltaV(aPropagator);
   }
   return deltaV;
}
