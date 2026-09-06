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

#include "WsfOrbitalEvent.hpp"

#include "UtCalendar.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtOrbitalPropagatorBase.hpp"
#include "UtStringUtil.hpp"
#include "UtVec3.hpp"
#include "WsfDateTime.hpp"
#include "WsfOrbitalMissionEventTypes.hpp"
#include "WsfScenario.hpp"
#include "WsfSpaceOrbitalMissionContext.hpp"
#include "WsfSpaceOrbitalPropagatorConditionTypes.hpp"
#include "WsfStringId.hpp"

bool WsfOrbitalEvent::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();

   if (command == "execute_at")
   {
      myCommand = true;
      aInput.ReadCommand(command);

      unsigned orbitInput{0};
      if (command == "orbit")
      {
         aInput.ReadValue(orbitInput);
         aInput.ReadCommand(command);
      }

      const auto& types        = wsf::space::OrbitalPropagatorConditionTypes::Get(mScenario);
      auto        conditionPtr = std::unique_ptr<wsf::space::OrbitalPropagatorCondition>(types.Clone(command));
      if (conditionPtr)
      {
         conditionPtr->ProcessInput(aInput);
         conditionPtr->SetOrbitNumber(orbitInput);
         mCondition = std::move(conditionPtr);
      }
      else
      {
         throw UtInput::UnknownCommand{aInput};
      }
   }
   else if (command == "finite")
   {
      mIsFinite = true;
   }
   else if (command == "duration")
   {
      aInput.ReadUnitValue(mDuration);
      aInput.ValueGreaterOrEqual<double>(mDuration, 0.0);
      mIsFinite = (mDuration > 0.0);
   }
   else if (command == "update_interval")
   {
      aInput.ReadUnitValue(mUpdateInterval);
      aInput.ValueGreater<double>(mUpdateInterval, 0.0);
   }
   else if (command == "debug")
   {
      mDebug = true;
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

//! Factory method to create an orbital event.
//! @param aScenario The scenario object that owns the orbital event types factory.
//! @param aInput The input object specifying the orbital event.
//! @param aIgnoreSomeExceptions Indicate if this method should ignore bad value exceptions.
//! @return The newly-created orbital event object.
WsfOrbitalEvent* WsfOrbitalEvent::Create(const WsfScenario& aScenario, UtInput& aInput, bool aIgnoreSomeExceptions)
{
   UtInputBlock block(aInput);
   std::string  type;
   aInput.ReadCommand(type);

   // When reading from the input, type should be lower case, but actual types are uppercase; covert.
   UtStringUtil::ToUpper(type);

   WsfOrbitalEvent* eventPtr = WsfOrbitalEvent::Create(aScenario, WsfStringId(type));
   if (eventPtr == nullptr)
   {
      throw UtInput::UnknownCommand(aInput);
   }
   std::string command;
   while (block.ReadCommand(command))
   {
      try
      {
         bool myCommand = eventPtr->ProcessInput(block.GetInput());
         if (!myCommand)
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
      catch (UtInput::BadValue&)
      {
         if (!aIgnoreSomeExceptions)
         {
            throw;
         }
      }
   }
   return eventPtr;
}

//! Direct the space mover associated with this MissionEvent to execute the MissionEvent.  The space mover may
//! or may not be successful in actually executing the MissionEvent due to delta-V limitations.  In the case
//! of finite MissionEvents, this method is called multiple times at the given update interval.
bool WsfOrbitalEvent::Execute(const UtCalendar& aEpoch, wsf::space::OrbitalMissionContext& aContext)
{
   // Inform observers that the MissionEvent has started or is being updated (finite MissionEvents only).
   if (aEpoch == mStartTime)
   {
      aContext.InitiateMissionEvent(aEpoch, *this);
   }
   else
   {
      aContext.UpdateMissionEvent(aEpoch, *this);
   }

   mHasExecuted = ExecuteEvent(aEpoch, aContext);

   if (mHasExecuted && (!IsComplete()))
   {
      mIsComplete = EvaluateCompletion(aEpoch, aContext.GetPropagator());

      if (IsComplete())
      {
         aContext.CompleteMissionEvent(aEpoch, *this);
      }
   }

   return mHasExecuted;
}

//! Set the full duration of the event.  A value of 0 implies and impulsive event; otherwise it is finite.
void WsfOrbitalEvent::SetDuration(const UtTimeValue& aDuration)
{
   mDuration = aDuration;
   mIsFinite = (mDuration > 0.0);
}

//! Set only the duration.
//!
//! Unlike SetDuration, this method has not side effects on if the event is considered to
//! be finite or not. Note that if the event is not set to be finite through some other
//! means, this may result in no effect on the resulting simulation.
//!
//!@param aDuration - The duration to set on the event.
void WsfOrbitalEvent::SetOnlyDuration(const UtTimeValue& aDuration)
{
   mDuration = aDuration;
}

//! Advance to the "next" MissionEvent in the MissionEvent sequence, after the current MissionEvent.
//! The base class' version simply returns nullptr if it has executed, or "this" if not (see
//! WsfOrbitalMissionSequence::AdvanceMissionEvent).
//!@param aEpoch The current epoch, to be used as the basis for evaluating the next MissionEvent in the sequence.
//!@param aContext The orbital mission context in which this event is being executed.
//!@return nullptr if this MissionEvent has executed; otherwise, "this".
WsfOrbitalEvent* WsfOrbitalEvent::AdvanceMissionEvent(const UtCalendar&                        aEpoch,
                                                      const wsf::space::OrbitalMissionContext& aContext)
{
   if (mHasExecuted)
   {
      return nullptr;
   }
   else
   {
      return this;
   }
}

WsfOrbitalEvent::AdvanceMissionEventException::AdvanceMissionEventException()
   : UtException("WsfOrbitalMissionSequence: Orbital event initialization failed.")
{
}

//! Initialize this MissionEvent in preparation to execute it.
//!@param aEpoch The current epoch, to be used as the basis for evaluating the next mission event in the sequence.
//!@param aContext The orbital mission context in which this event is executing.
bool WsfOrbitalEvent::Initialize(const UtCalendar& aEpoch, const wsf::space::OrbitalMissionContext& aContext)
{
   if (aContext.GetSimulation() != nullptr)
   {
      AssignUniqueId(aContext.GetSimulation());
   }
   bool ok = VerifyCondition();
   if (ok)
   {
      ok = InitializeTimes(aEpoch, aContext);
   }
   else
   {
      auto logger = ut::log::error() << "Condition could not be met.";
      logger.AddNote() << "Condition: " << mCondition->GetConditionString();
   }

   return ok;
}

//! Verify that the condition set in the event is one of the possible valid constraints.
//! Note that when executed the constraint may not be valid at the time.
bool WsfOrbitalEvent::VerifyCondition() const
{
   auto t = mCondition->GetType();
   return (t == wsf::space::NoneCondition::cTYPE || t == wsf::space::RelativeTimeCondition::cTYPE ||
           wsf::space::ApoapsisCondition::cTYPE || t == wsf::space::PeriapsisCondition::cTYPE ||
           wsf::space::AscendingNodeCondition::cTYPE || t == wsf::space::DescendingNodeCondition::cTYPE ||
           wsf::space::EclipseEntryCondition::cTYPE || t == wsf::space::EclipseExitCondition::cTYPE);
}

//! As part of event verification, evaluate any preconditions associated with the event.
//! Usually this involves making sure that the provided WsfOrbitalEvent::Condition is correct.
//! @param aContext The orbital mission context in which this event is executing.
//! @return True if the preconditions are met, false otherwise.
bool WsfOrbitalEvent::EvaluatePreconditions(const wsf::space::OrbitalMissionContext& aContext) const
{
   return true;
}

//! As part of event verification, evaluate any required postconditions associated with the event.
//! These checks involve making sure that this is an expected post-event orbit; for instance,
//! there is no intersection with the earth and it is not hyperbolic.
//! @param aContext The orbital mission context in which this event is executing.
//! @return True if the postcondititions are met, false otherwise.
bool WsfOrbitalEvent::EvaluatePostconditions(const wsf::space::OrbitalMissionContext& aContext) const
{
   return true;
}

//! Check that the event's parameters are in the valid range for each parameter.
//! When deriving from WsfOrbitalEvent, the subclass should call into this method to apply the checks
//! on any parameters related to the event being finite.
//! @param aExecutingPlatformName The name of the platform that would be executing this event.
//! @return True if the event parameters are all in allowed ranges.
bool WsfOrbitalEvent::ValidateParameterRanges(const std::string& /*aExecutingPlatformName*/) const
{
   bool retval = mCondition->ValidateParameterRanges();
   if (mIsFinite)
   {
      if (mDuration < 0.0)
      {
         auto logger = ut::log::error() << "Duration must be greater than or equal to 0.";
         logger.AddNote() << "Type: " << GetType();
         retval = false;
      }
      if (mUpdateInterval <= 0.0)
      {
         auto logger = ut::log::error() << "update_interval must be greater than 0.";
         logger.AddNote() << "Type: " << GetType();
         retval = false;
      }
   }
   return retval;
}

// private
//! Initialize the start and evaluation times for the event.
//! In the case of finite maneuvers set the start time according to a patched conic approximation.
//!@note For finite maneuvers the start time can be before the epoch; in that case we advance the maneuver
//! one or more orbits and print a warning.
bool WsfOrbitalEvent::InitializeTimes(const UtCalendar& aEpoch, const wsf::space::OrbitalMissionContext& aContext)
{
   mStartTime             = aEpoch;
   double timeToCondition = 0.0;
   bool   ok              = mCondition->GetTimeToCondition(aContext.GetPropagator(), timeToCondition);
   if (ok)
   {
      if (timeToCondition != 0.0)
      {
         mStartTime.AdvanceTimeBy(timeToCondition);
      }
      mEvaluationTime = mStartTime;

      ok = InitializeVariables(aEpoch, mEvaluationTime, aContext);
   }
   return ok;
}

bool WsfOrbitalEvent::GetNextExecutionTime(const UtCalendar&              aCurrentTime,
                                           const UtOrbitalPropagatorBase& aContext,
                                           UtCalendar&                    aNextExecutionTime)
{
   bool shouldExecute = false;
   aNextExecutionTime = aCurrentTime;
   if (IsFinite())
   {
      shouldExecute = true;
      aNextExecutionTime.AdvanceTimeBy(GetUpdateInterval());
   }
   return shouldExecute;
}

WsfOrbitalEvent::WsfOrbitalEvent(const WsfScenario& aScenario)
   : WsfUniqueId()
   , WsfObject()
   , mCondition(new wsf::space::NoneCondition{})
   , mIsFinite(false)
   , mHasExecuted(false)
   , mIsComplete(false)
   , mIsCanceled(false)
   , mDebug(false)
   , mCanVerify(true)
   , mStartTime()
   , mEvaluationTime()
   , mDuration(0.0)
   , mUpdateInterval(1.0)
   , mScenario(aScenario)
{
}

//! Called during InitializeTimes(), this method should be overridden to ensure
//! proper initialization of any variables used in ComputeDeltaV.
//! Default is to do nothing.
//!@param aCurrentTime The current time.
//!@param aEvaluationTime The evaluation time of the event.
//!@param aContext The orbital mission context in which this event is being executed.
//!@return True if initialization was successful; false otherwise.
bool WsfOrbitalEvent::InitializeVariables(const UtCalendar&                        aCurrentTime,
                                          const UtCalendar&                        aEvaluationTime,
                                          const wsf::space::OrbitalMissionContext& aContext)
{
   return true;
}

// protected
//! Evaluate whether an event is marked as complete.  If it is complete, mIsComplete is true in Execute(),
//! and the event sequence will progress to the next event.
//!@note This is a template method called within Execute(...).
bool WsfOrbitalEvent::EvaluateCompletion(const UtCalendar& aEpoch, const UtOrbitalPropagatorBase& aPropagator)
{
   bool complete = true;
   if (mDuration > 0.0)
   {
      complete = (aEpoch.GetTimeSince(mStartTime) >= mDuration);
   }
   return complete;
}

WsfOrbitalEvent::WsfOrbitalEvent(const WsfOrbitalEvent& aSrc)
   : WsfUniqueId(aSrc)
   , WsfObject(aSrc)
   , mCondition(aSrc.mCondition)
   , mIsFinite(aSrc.mIsFinite)
   , mHasExecuted(aSrc.mHasExecuted)
   , mIsComplete(aSrc.mIsComplete)
   , mIsCanceled(aSrc.mIsCanceled)
   , mDebug(aSrc.mDebug)
   , mCanVerify(aSrc.mCanVerify)
   , mStartTime(aSrc.mStartTime)
   , mEvaluationTime()
   , mDuration(aSrc.mDuration)
   , mUpdateInterval(aSrc.mUpdateInterval)
   , mScenario(aSrc.mScenario)
{
}

// virtual
//! Per-mover factory method to create events by type.
//!@param aScenario The current scenario (used to find and clone a prototype event).
//!@param aType The type of event to create.
WsfOrbitalEvent* WsfOrbitalEvent::Create(const WsfScenario& aScenario, WsfStringId aType)
{
   WsfOrbitalEvent* prototypePtr =
      static_cast<WsfOrbitalMissionEventTypes*>(aScenario.GetTypes("orbital_mission_event"))->Find(aType);
   WsfOrbitalEvent* eventPtr = nullptr;
   if (prototypePtr != nullptr)
   {
      eventPtr = prototypePtr->Clone();
   }
   if (eventPtr == nullptr)
   {
      std::string msg = "event type " + aType + " not recognized.";
      UtException ex  = UtException(msg);
      throw ex;
   }
   return eventPtr;
}
