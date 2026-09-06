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

#include "P6DofSequencer.hpp"

#include "P6DofJetEngineObject.hpp"
#include "P6DofRamjetEngineObject.hpp"
#include "P6DofRocketLiquidPropellantObject.hpp"
#include "P6DofRocketSolidPropellantObject.hpp"
#include "P6DofScenario.hpp"
#include "P6DofUtils.hpp"
#include "P6DofVehicle.hpp"
#include "UtDCM.hpp"
#include "UtEntity.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtInputFile.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtTable.hpp"
#include "UtVec3.hpp"

P6DofSequencer::P6DofSequencer(P6DofScenario* aScenario)
   : P6DofObject(aScenario)
   , mLastSimTime_nanosec(-1)
   , mElapsedTimer_nanosec(0)
   , mActivated(false)
   , mAllEvents(false)
   , mAllEventNeededCount(0)
   , mAllEventCurrentCount(0)
   , mExternalActivation(false)
{
}

P6DofSequencer::~P6DofSequencer()
{
   std::vector<SequencerEvent*>::iterator eventIter = mEventList.begin();
   while (eventIter != mEventList.end())
   {
      SequencerEvent* sequencerEvent = (*eventIter);
      ++eventIter;
      delete sequencerEvent;
   }
   mEventList.clear();

   std::vector<SequencerAction*>::iterator actionIter = mActionList.begin();
   while (actionIter != mActionList.end())
   {
      SequencerAction* sequencerAction = (*actionIter);
      ++actionIter;
      delete sequencerAction;
   }
   mActionList.clear();

   std::vector<P6DofSequencer*>::iterator iter = mSubSequencerList.begin();
   while (iter != mSubSequencerList.end())
   {
      P6DofSequencer* sequencer = (*iter);
      *iter                     = (P6DofSequencer*)nullptr;
      ++iter;
      delete sequencer;
   }
   mSubSequencerList.clear();
}

P6DofSequencer* P6DofSequencer::Clone() const
{
   return new P6DofSequencer(*this);
}

P6DofSequencer::P6DofSequencer(const P6DofSequencer& aSrc)
   : P6DofObject(aSrc)
   , mLastSimTime_nanosec(aSrc.mLastSimTime_nanosec)
   , mElapsedTimer_nanosec(aSrc.mElapsedTimer_nanosec)
   , mActivated(aSrc.mActivated)
   , mAllEvents(aSrc.mAllEvents)
   , mAllEventNeededCount(aSrc.mAllEventNeededCount)
   , mAllEventCurrentCount(aSrc.mAllEventCurrentCount)
   , mExternalActivation(aSrc.mExternalActivation)
{
   std::vector<SequencerEvent*>::const_iterator iterEvent = aSrc.mEventList.begin();
   while (iterEvent != aSrc.mEventList.end())
   {
      SequencerEvent* event = new SequencerEvent;

      // Copy event and save it
      *event = *(*iterEvent);
      mEventList.push_back(event);

      ++iterEvent;
   }

   std::vector<SequencerAction*>::const_iterator iterAction = aSrc.mActionList.begin();
   while (iterAction != aSrc.mActionList.end())
   {
      SequencerAction* action = new SequencerAction;

      // Copy action and save it
      *action = *(*iterAction);
      mActionList.push_back(action);

      ++iterAction;
   }

   std::vector<P6DofSequencer*>::const_iterator iterSequencer = aSrc.mSubSequencerList.begin();
   while (iterSequencer != aSrc.mSubSequencerList.end())
   {
      // Create a copy of the sequencer and save it
      P6DofSequencer* subObj = (*iterSequencer)->Clone();
      subObj->SetParentVehicle(mParentVehicle);
      mSubSequencerList.push_back(subObj);
      ++iterSequencer;
   }
}

bool P6DofSequencer::PendingTimeEvents(int64_t aSimTime_nanosec, std::set<int64_t>& aTimeOfEvents_nanosec)
{
   std::set<int64_t> timeOfEvents_nanosec;
   bool              valid = false;

   if (mActivated)
   {
      // Check sub-sequencers
      std::vector<P6DofSequencer*>::iterator iter = mSubSequencerList.begin();
      while (iter != mSubSequencerList.end())
      {
         std::set<int64_t> tempTimeOfEvents_nanosec;
         (*iter)->PendingTimeEvents(aSimTime_nanosec, tempTimeOfEvents_nanosec);

         for (const int64_t& eventTime : tempTimeOfEvents_nanosec)
         {
            timeOfEvents_nanosec.insert(eventTime);
            valid = true;
         }

         ++iter;
      }
   }
   else
   {
      int64_t deltaTime_nano      = aSimTime_nanosec - mLastSimTime_nanosec;
      int64_t endElapsedTime_nano = mElapsedTimer_nanosec + deltaTime_nano;

      // Check all events, looking for time-based events
      std::vector<SequencerEvent*>::iterator iter = mEventList.begin();
      while (iter != mEventList.end())
      {
         if (((*iter)->eventType == Timer) || ((*iter)->eventType == LifeTime))
         {
            int64_t deltaPastTime_nano = 0;

            if ((*iter)->eventType == Timer)
            {
               deltaPastTime_nano = endElapsedTime_nano - (*iter)->argumentNanoTime;
            }
            else if ((*iter)->eventType == LifeTime)
            {
               deltaPastTime_nano = mParentVehicle->GetLifeTime_nanosec() - (*iter)->argumentNanoTime;
            }

            if (deltaPastTime_nano >= 0)
            {
               // The time the event occurs
               int64_t eventTime_nano = aSimTime_nanosec - deltaPastTime_nano;
               timeOfEvents_nanosec.insert(eventTime_nano);
               valid = true;
            }
         }

         ++iter;
      }
   }

   if (valid)
   {
      aTimeOfEvents_nanosec = timeOfEvents_nanosec;
   }

   return valid;
}

void P6DofSequencer::Update(int64_t aSimTime_nanosec)
{
   // Increment the elapsed time by dT
   int64_t dTnano = aSimTime_nanosec - mLastSimTime_nanosec;
   mElapsedTimer_nanosec += dTnano;

   // If not already activated, check if it should be activated
   if (!mActivated)
   {
      int64_t activationTime_nanosec = TestForActivation(aSimTime_nanosec);

      // Check for newly activated
      if (mActivated)
      {
         // Start up the sub-sequencers
         std::vector<P6DofSequencer*>::iterator iter = mSubSequencerList.begin();
         while (iter != mSubSequencerList.end())
         {
            (*iter)->StartUp(activationTime_nanosec);
            ++iter;
         }

         // Update the sub-sequencers
         iter = mSubSequencerList.begin();
         while (iter != mSubSequencerList.end())
         {
            (*iter)->Update(aSimTime_nanosec);
            ++iter;
         }
      }
   }
   else
   {
      // Already activated, update the sub-sequencers
      std::vector<P6DofSequencer*>::iterator iter = mSubSequencerList.begin();
      while (iter != mSubSequencerList.end())
      {
         (*iter)->Update(aSimTime_nanosec);
         ++iter;
      }
   }

   // Set the 'last time' the current time
   mLastSimTime_nanosec = aSimTime_nanosec;
}

int64_t P6DofSequencer::TestForActivation(int64_t aSimTime_nanosec)
{
   int64_t timeOfActivation_nanosec = aSimTime_nanosec;

   // Sequencers may require 'all events' to trigger actions
   SetAllEventCounts();

   // This is used to correct for delays in responding to events
   int64_t eventTimeIntoFrame_nanosec = 0;

   std::vector<SequencerEvent*>::iterator eiter = mEventList.begin();
   while (eiter != mEventList.end())
   {
      bool   lastState    = false;
      bool   currentState = false;
      double lastValue    = 0.0;
      double currentValue = 0.0;

      switch ((*eiter)->eventType)
      {
      case Timer:
         if (mElapsedTimer_nanosec >= (*eiter)->argumentNanoTime)
         {
            eventTimeIntoFrame_nanosec = mElapsedTimer_nanosec - (*eiter)->argumentNanoTime;

            if (eventTimeIntoFrame_nanosec < 0)
            {
               eventTimeIntoFrame_nanosec = 0;
            }
            CheckForAllEventActivation();

            timeOfActivation_nanosec = aSimTime_nanosec - eventTimeIntoFrame_nanosec;
         }
         break;
      case LifeTime:
         if (mParentVehicle)
         {
            if (mParentVehicle->GetLifeTime_sec() >= (*eiter)->argumentValue)
            {
               CheckForAllEventActivation();
            }
         }
         break;
      case AboveAlt_ft:
         if (mParentVehicle)
         {
            currentValue = mParentVehicle->GetAltitudeMSL_ft();
            lastValue    = mParentVehicle->GetLastAltitudeMSL_ft();

            if (currentValue > lastValue) // climbing
            {
               if (currentValue > (*eiter)->argumentValue) // alt check
               {
                  CheckForAllEventActivation();
               }
            }
         }
         break;
      case BelowAlt_ft:
         if (mParentVehicle)
         {
            currentValue = mParentVehicle->GetAltitudeMSL_ft();
            lastValue    = mParentVehicle->GetLastAltitudeMSL_ft();

            if (currentValue < lastValue) // descending
            {
               if (currentValue < (*eiter)->argumentValue) // alt check
               {
                  CheckForAllEventActivation();
               }
            }
         }
         break;
      case DynamicPressureAbove_psf:
         if (mParentVehicle)
         {
            currentValue = mParentVehicle->GetDynamicPressure_psf();
            lastValue    = mParentVehicle->GetLastDynamicPressure_psf();

            if (currentValue > lastValue) // increasing
            {
               if (currentValue > (*eiter)->argumentValue)
               {
                  CheckForAllEventActivation();
               }
            }
         }
         break;
      case DynamicPressureBelow_psf:
         if (mParentVehicle)
         {
            currentValue = mParentVehicle->GetDynamicPressure_psf();
            lastValue    = mParentVehicle->GetLastDynamicPressure_psf();

            if (currentValue < lastValue) // decreasing
            {
               if (currentValue < (*eiter)->argumentValue)
               {
                  CheckForAllEventActivation();
               }
            }
         }
         break;
      case StaticPressureAbove_psf:
         if (mParentVehicle)
         {
            currentValue = mParentVehicle->GetStaticPressure_psf();
            lastValue    = mParentVehicle->GetLastStaticPressure_psf();

            if (currentValue > lastValue) // increasing
            {
               if (currentValue > (*eiter)->argumentValue)
               {
                  CheckForAllEventActivation();
               }
            }
         }
         break;
      case StaticPressureBelow_psf:
         if (mParentVehicle)
         {
            currentValue = mParentVehicle->GetStaticPressure_psf();
            lastValue    = mParentVehicle->GetLastStaticPressure_psf();

            if (currentValue < lastValue) // decreasing
            {
               if (currentValue < (*eiter)->argumentValue)
               {
                  CheckForAllEventActivation();
               }
            }
         }
         break;
      case FuelPercentBelow:
         if (mParentVehicle)
         {
            currentValue = mParentVehicle->GetRemainingFuelPercent();

            if (currentValue < (*eiter)->argumentValue)
            {
               CheckForAllEventActivation();
            }
         }
         break;
      case GroundDistance_m:
         if (mParentVehicle)
         {
            currentValue = 1000.0 * mParentVehicle->GetRangeSinceStart_km();

            if (currentValue > (*eiter)->argumentValue)
            {
               CheckForAllEventActivation();
            }
         }
         break;
      case NxAbove_g:
         if (mParentVehicle)
         {
            currentValue = mParentVehicle->GetNx_g();
            lastValue    = mParentVehicle->GetLastNx_g();

            if (currentValue > lastValue) // increasing
            {
               if (currentValue > (*eiter)->argumentValue)
               {
                  CheckForAllEventActivation();
               }
            }
         }
         break;
      case NxBelow_g:
         if (mParentVehicle)
         {
            currentValue = mParentVehicle->GetNx_g();
            lastValue    = mParentVehicle->GetLastNx_g();

            if (currentValue < lastValue) // decreasing
            {
               if (currentValue < (*eiter)->argumentValue)
               {
                  CheckForAllEventActivation();
               }
            }
         }
         break;
      case NyAbove_g:
         if (mParentVehicle)
         {
            currentValue = mParentVehicle->GetNy_g();
            lastValue    = mParentVehicle->GetLastNy_g();

            if (currentValue > lastValue) // increasing
            {
               if (currentValue > (*eiter)->argumentValue)
               {
                  CheckForAllEventActivation();
               }
            }
         }
         break;
      case NyBelow_g:
         if (mParentVehicle)
         {
            currentValue = mParentVehicle->GetNy_g();
            lastValue    = mParentVehicle->GetLastNy_g();

            if (currentValue < lastValue) // decreasing
            {
               if (currentValue < (*eiter)->argumentValue)
               {
                  CheckForAllEventActivation();
               }
            }
         }
         break;
      case NzAbove_g:
         if (mParentVehicle)
         {
            currentValue = mParentVehicle->GetNz_g();
            lastValue    = mParentVehicle->GetLastNz_g();

            if (currentValue > lastValue) // increasing
            {
               if (currentValue > (*eiter)->argumentValue)
               {
                  CheckForAllEventActivation();
               }
            }
         }
         break;
      case NzBelow_g:
         if (mParentVehicle)
         {
            currentValue = mParentVehicle->GetNz_g();
            lastValue    = mParentVehicle->GetLastNz_g();

            if (currentValue < lastValue) // decreasing
            {
               if (currentValue < (*eiter)->argumentValue)
               {
                  CheckForAllEventActivation();
               }
            }
         }
         break;
      case ReleasedFromParent:
         if (mParentVehicle)
         {
            currentState = mParentVehicle->IsCaptive();
            lastState    = mParentVehicle->GetLastIsCaptive();

            if (!currentState) // is free
            {
               if (currentState != lastState) // changed
               {
                  CheckForAllEventActivation();
               }
            }
         }
         break;
      case BooleanInput:
         if (mParentVehicle)
         {
            currentState = mParentVehicle->GetBooleanInput((*eiter)->argumentHandle);

            if (currentState)
            {
               lastState = mParentVehicle->GetLastBooleanInput((*eiter)->argumentHandle);
               if (!lastState)
               {
                  CheckForAllEventActivation();
               }
            }
         }
         break;
      case NoEvent:
         // Do nothing
         break;
      default:
         break;
      }

      ++eiter;
   }

   // Set activated flag if externally activated and not yet activated
   if (mExternalActivation && !mActivated)
   {
      mActivated = true;
   }

   // Perform action(s) if activated
   if (mActivated)
   {
      std::vector<SequencerAction*>::iterator aiter = mActionList.begin();
      while (aiter != mActionList.end())
      {
         std::string                targetObjName;
         std::string                parentObjName;
         P6DofVehicle*              tgtObj         = nullptr;
         P6DofThrustProducerObject* thrustProducer = nullptr;

         switch ((*aiter)->actionType)
         {
         case JettisonObject:
            if (mParentVehicle)
            {
               mParentVehicle->JettisonSubobject((*aiter)->argumentString, aSimTime_nanosec);
            }
            break;
         case JettisonSelf:
            if (mParentVehicle)
            {
               // The sequencer's parent is the object we want to Jettison,
               // so get the parent of the parent, and have it Jettison the sequencer's parent.
               P6DofVehicle* parentsParentObject = mParentVehicle->GetParentVehicle();
               if (parentsParentObject)
               {
                  parentsParentObject->JettisonSubobject(mParentVehicle, aSimTime_nanosec);
               }
            }
            break;
         case IgniteEngine:
            if (mParentVehicle)
            {
               // Find the engine
               thrustProducer = mParentVehicle->GetThrustProducerObjectByName((*aiter)->argumentString);
               if (thrustProducer != nullptr)
               {
                  thrustProducer->Ignite(eventTimeIntoFrame_nanosec);
               }
            }
            break;
         case IgniteSubObject:
            if (mParentVehicle)
            {
               targetObjName = (*aiter)->argumentString;
               tgtObj        = mParentVehicle->GetSubobjectByName(targetObjName);
               if (tgtObj)
               {
                  tgtObj->IgniteObject(eventTimeIntoFrame_nanosec);
               }
            }
            break;
         case IgniteSelfObject:
            if (mParentVehicle)
            {
               mParentVehicle->IgniteObject(eventTimeIntoFrame_nanosec);
            }
            break;
         case TerminateThrust:
            if (mParentVehicle)
            {
               mParentVehicle->TerminateThrust();
            }
            break;
         case ShutdownEngine:
            if (mParentVehicle)
            {
               // Find the engine
               thrustProducer = mParentVehicle->GetThrustProducerObjectByName((*aiter)->argumentString);
               thrustProducer->Shutdown(eventTimeIntoFrame_nanosec);
            }
            break;
         case ShutdownSubObject:
            if (mParentVehicle)
            {
               targetObjName = (*aiter)->argumentString;
               tgtObj        = mParentVehicle->GetSubobjectByName(targetObjName);
               if (tgtObj)
               {
                  tgtObj->ShutdownObject();
               }
            }
            break;
         case ChangeAeroMode:
            if (mParentVehicle)
            {
               targetObjName = (*aiter)->argumentString;

               // Change the aero mode
               mParentVehicle->SetActiveAeroObject(targetObjName);
            }
            break;
         case EnableSizeFactor:
            if (mParentVehicle)
            {
               mParentVehicle->SetSizeFactorEnabled(true);
            }
            break;
         case SetGraphicalModel:
            if (mParentVehicle)
            {
               std::string typeId = (*aiter)->argumentString;
               if (typeId == "1")
               {
                  mParentVehicle->SetObjectGraphicalType(1);
               }
               else if (typeId == "2")
               {
                  mParentVehicle->SetObjectGraphicalType(2);
               }
               else if (typeId == "3")
               {
                  mParentVehicle->SetObjectGraphicalType(3);
               }
               else
               {
                  mParentVehicle->SetObjectGraphicalType(0);
               }
            }
            break;
         case ActivateSubobjectSequencer:
            if (mParentVehicle)
            {
               mParentVehicle->ActivateSequencer((*aiter)->argumentString, (*aiter)->argumentSecondString);
            }
            break;
         case EnableControls:
            if (mParentVehicle)
            {
               mParentVehicle->EnableControls(true);
            }
            break;
         case DisableControls:
            if (mParentVehicle)
            {
               mParentVehicle->EnableControls(false);
            }
            break;
         default:
            break;
         }

         ++aiter;
      }
   }

   return timeOfActivation_nanosec;
}

void P6DofSequencer::SetParentVehicle(P6DofVehicle* aParentVehicle)
{
   P6DofObject::SetParentVehicle(aParentVehicle);
   for (auto& i : mSubSequencerList)
   {
      i->SetParentVehicle(aParentVehicle);
   }
}

bool P6DofSequencer::ProcessInput(UtInput& aInput)
{
   bool              myCommand = true;
   const std::string command   = aInput.GetCommand();

   if (command == "sequencer")
   {
      std::string sequencerName;
      aInput.ReadValue(sequencerName);
      SetName(sequencerName);

      UtInputBlock block(aInput, "end_sequencer");
      while (block.ReadCommand())
      {
         std::string blockCommand;
         blockCommand = block.GetCommand();

         if (blockCommand == "all_events")
         {
            mAllEvents = true;
         }
         else if (blockCommand == "event_timer")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cTIME);

            SequencerEvent* event   = new SequencerEvent;
            event->eventType        = Timer;
            event->argumentValue    = 0.0;
            event->argumentNanoTime = P6DofUtils::TimeToTime(value);

            mEventList.push_back(event);
         }
         else if (blockCommand == "event_timer_int_msec")
         {
            int ivalue;
            aInput.ReadValue(ivalue);

            SequencerEvent* event   = new SequencerEvent;
            event->eventType        = Timer;
            event->argumentValue    = 0.0;
            event->argumentNanoTime = ivalue * 1000000;

            mEventList.push_back(event);
         }
         else if (blockCommand == "event_timer_int_nanosec")
         {
            int ivalue;
            aInput.ReadValue(ivalue);

            SequencerEvent* event   = new SequencerEvent;
            event->eventType        = Timer;
            event->argumentValue    = 0.0;
            event->argumentNanoTime = ivalue;

            mEventList.push_back(event);
         }
         else if (blockCommand == "event_lifetime")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cTIME);

            SequencerEvent* event   = new SequencerEvent;
            event->eventType        = LifeTime;
            event->argumentValue    = 0.0;
            event->argumentNanoTime = P6DofUtils::TimeToTime(value);

            mEventList.push_back(event);
         }
         else if (blockCommand == "event_lifetime_int_msec")
         {
            int ivalue;
            aInput.ReadValue(ivalue);

            SequencerEvent* event   = new SequencerEvent;
            event->eventType        = LifeTime;
            event->argumentValue    = 0.0;
            event->argumentNanoTime = ivalue * 1000000;

            mEventList.push_back(event);
         }
         else if (blockCommand == "event_lifetime_int_nanosec")
         {
            int ivalue;
            aInput.ReadValue(ivalue);

            SequencerEvent* event   = new SequencerEvent;
            event->eventType        = LifeTime;
            event->argumentValue    = 0.0;
            event->argumentNanoTime = ivalue;

            mEventList.push_back(event);
         }
         else if (blockCommand == "event_above_alt")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cLENGTH);

            SequencerEvent* event   = new SequencerEvent;
            event->eventType        = AboveAlt_ft;
            event->argumentValue    = value * UtMath::cFT_PER_M;
            event->argumentNanoTime = 0;

            mEventList.push_back(event);
         }
         else if (blockCommand == "event_below_alt")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cLENGTH);

            SequencerEvent* event   = new SequencerEvent;
            event->eventType        = BelowAlt_ft;
            event->argumentValue    = value * UtMath::cFT_PER_M;
            event->argumentNanoTime = 0;

            mEventList.push_back(event);
         }
         else if (blockCommand == "event_dynamic_pressure_above")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cPRESSURE);

            SequencerEvent* event   = new SequencerEvent;
            event->eventType        = DynamicPressureAbove_psf;
            event->argumentValue    = value / (6894.75729 / 144.0);
            event->argumentNanoTime = 0;

            mEventList.push_back(event);
         }
         else if (blockCommand == "event_dynamic_pressure_below")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cPRESSURE);

            SequencerEvent* event   = new SequencerEvent;
            event->eventType        = DynamicPressureBelow_psf;
            event->argumentValue    = value / (6894.75729 / 144.0);
            event->argumentNanoTime = 0;

            mEventList.push_back(event);
         }
         else if (blockCommand == "event_static_pressure_above")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cPRESSURE);

            SequencerEvent* event   = new SequencerEvent;
            event->eventType        = StaticPressureAbove_psf;
            event->argumentValue    = value / (6894.75729 / 144.0);
            event->argumentNanoTime = 0;

            mEventList.push_back(event);
         }
         else if (blockCommand == "event_static_pressure_below")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cPRESSURE);

            SequencerEvent* event   = new SequencerEvent;
            event->eventType        = StaticPressureBelow_psf;
            event->argumentValue    = value / (6894.75729 / 144.0);
            event->argumentNanoTime = 0;

            mEventList.push_back(event);
         }
         else if (blockCommand == "event_fuel_percent_below")
         {
            double value;
            aInput.ReadValue(value);

            SequencerEvent* event   = new SequencerEvent;
            event->eventType        = FuelPercentBelow;
            event->argumentValue    = value;
            event->argumentNanoTime = 0;

            mEventList.push_back(event);
         }
         else if (blockCommand == "event_ground_distance")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cLENGTH);

            SequencerEvent* event   = new SequencerEvent;
            event->eventType        = GroundDistance_m;
            event->argumentValue    = value;
            event->argumentNanoTime = 0;

            mEventList.push_back(event);
         }
         else if (blockCommand == "event_nx_above")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cACCELERATION);

            SequencerEvent* event   = new SequencerEvent;
            event->eventType        = NxAbove_g;
            event->argumentValue    = value / 9.80665;
            event->argumentNanoTime = 0;

            mEventList.push_back(event);
         }
         else if (blockCommand == "event_nx_below")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cACCELERATION);

            SequencerEvent* event   = new SequencerEvent;
            event->eventType        = NxBelow_g;
            event->argumentValue    = value / 9.80665;
            event->argumentNanoTime = 0;

            mEventList.push_back(event);
         }
         else if (blockCommand == "event_ny_above")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cACCELERATION);

            SequencerEvent* event   = new SequencerEvent;
            event->eventType        = NyAbove_g;
            event->argumentValue    = value / 9.80665;
            event->argumentNanoTime = 0;

            mEventList.push_back(event);
         }
         else if (blockCommand == "event_ny_below")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cACCELERATION);

            SequencerEvent* event   = new SequencerEvent;
            event->eventType        = NyBelow_g;
            event->argumentValue    = value / 9.80665;
            event->argumentNanoTime = 0;

            mEventList.push_back(event);
         }
         else if (blockCommand == "event_nz_above")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cACCELERATION);

            SequencerEvent* event   = new SequencerEvent;
            event->eventType        = NzAbove_g;
            event->argumentValue    = value / 9.80665;
            event->argumentNanoTime = 0;

            mEventList.push_back(event);
         }
         else if (blockCommand == "event_nz_below")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cACCELERATION);

            SequencerEvent* event   = new SequencerEvent;
            event->eventType        = NzBelow_g;
            event->argumentValue    = value / 9.80665;
            event->argumentNanoTime = 0;

            mEventList.push_back(event);
         }
         else if (blockCommand == "event_released_from_parent")
         {
            SequencerEvent* event   = new SequencerEvent;
            event->eventType        = ReleasedFromParent;
            event->argumentNanoTime = 0;

            mEventList.push_back(event);
         }
         else if (blockCommand == "event_manual_input_trigger")
         {
            int value;
            aInput.ReadValue(value);

            SequencerEvent* event   = new SequencerEvent;
            event->eventType        = ManualInputTrigger;
            event->argumentInteger  = value;
            event->argumentNanoTime = 0;

            mEventList.push_back(event);
         }
         else if (blockCommand == "event_manual_input_button")
         {
            int value;
            aInput.ReadValue(value);

            SequencerEvent* event   = new SequencerEvent;
            event->eventType        = ManualInputButton;
            event->argumentInteger  = value;
            event->argumentNanoTime = 0;

            mEventList.push_back(event);
         }
         else if (blockCommand == "event_manual_input_button_released")
         {
            int value;
            aInput.ReadValue(value);

            SequencerEvent* event   = new SequencerEvent;
            event->eventType        = ManualInputButtonReleased;
            event->argumentInteger  = value;
            event->argumentNanoTime = 0;

            mEventList.push_back(event);
         }
         else if (blockCommand == "event_boolean_input")
         {
            std::string value;
            aInput.ReadValue(value);

            SequencerEvent* event   = new SequencerEvent;
            event->eventType        = BooleanInput;
            event->argumentString   = value;
            event->argumentHandle   = 0;
            event->argumentNanoTime = 0;

            mEventList.push_back(event);
         }
         else if (blockCommand == "event_null")
         {
            SequencerEvent* event   = new SequencerEvent;
            event->eventType        = NoEvent;
            event->argumentNanoTime = 0;

            mEventList.push_back(event);
         }
         else if (blockCommand == "action_null")
         {
            SequencerAction* action = new SequencerAction;
            action->actionType      = NoAction;

            mActionList.push_back(action);
         }
         else if (blockCommand == "action_jett_obj")
         {
            std::string value;
            aInput.ReadValue(value);

            SequencerAction* action = new SequencerAction;
            action->actionType      = JettisonObject;
            action->argumentString  = value;

            mActionList.push_back(action);
         }
         else if (blockCommand == "action_jett_self")
         {
            SequencerAction* action = new SequencerAction;
            action->actionType      = JettisonSelf;

            mActionList.push_back(action);
         }
         else if (blockCommand == "action_ignite_engine")
         {
            std::string value;
            aInput.ReadValue(value);

            SequencerAction* action = new SequencerAction;
            action->actionType      = IgniteEngine;
            action->argumentString  = value;

            mActionList.push_back(action);
         }
         else if (blockCommand == "action_set_graphical_model")
         {
            int value;
            aInput.ReadValue(value);

            SequencerAction* action = new SequencerAction;
            action->actionType      = SetGraphicalModel;

            switch (value)
            {
            case 0:
               action->argumentString = "0";
               break;
            case 1:
               action->argumentString = "1";
               break;
            case 2:
               action->argumentString = "2";
               break;
            case 3:
               action->argumentString = "3";
               break;
            default:
               action->argumentString = "0";
               break;
            }

            mActionList.push_back(action);
         }
         else if (blockCommand == "action_activate_subobject_sequencer")
         {
            std::string value1;
            aInput.ReadValue(value1);
            std::string value2;
            aInput.ReadValue(value2);

            SequencerAction* action      = new SequencerAction;
            action->actionType           = ActivateSubobjectSequencer;
            action->argumentString       = value1;
            action->argumentSecondString = value2;

            mActionList.push_back(action);
         }
         else if (blockCommand == "action_enable_controls")
         {
            SequencerAction* action = new SequencerAction;
            action->actionType      = EnableControls;

            mActionList.push_back(action);
         }
         else if (blockCommand == "action_disable_controls")
         {
            SequencerAction* action = new SequencerAction;
            action->actionType      = DisableControls;

            mActionList.push_back(action);
         }
         else if (blockCommand == "action_ignite_subobject")
         {
            std::string value;
            aInput.ReadValue(value);

            SequencerAction* action = new SequencerAction;
            action->actionType      = IgniteSubObject;
            action->argumentString  = value;

            mActionList.push_back(action);
         }
         else if (blockCommand == "action_ignite_self")
         {
            SequencerAction* action = new SequencerAction;
            action->actionType      = IgniteSelfObject;

            mActionList.push_back(action);
         }
         else if (blockCommand == "action_terminate_thrust")
         {
            SequencerAction* action = new SequencerAction;
            action->actionType      = TerminateThrust;

            mActionList.push_back(action);
         }
         else if (blockCommand == "action_shutdown_engine")
         {
            std::string value;
            aInput.ReadValue(value);

            SequencerAction* action = new SequencerAction;
            action->actionType      = ShutdownEngine;
            action->argumentString  = value;

            mActionList.push_back(action);
         }
         else if (blockCommand == "action_shutdown_subobject")
         {
            std::string value;
            aInput.ReadValue(value);

            SequencerAction* action = new SequencerAction;
            action->actionType      = ShutdownSubObject;
            action->argumentString  = value;

            mActionList.push_back(action);
         }
         else if (blockCommand == "action_change_aero_mode")
         {
            std::string value;
            aInput.ReadValue(value);

            SequencerAction* action = new SequencerAction;
            action->actionType      = ChangeAeroMode;
            action->argumentString  = value;

            mActionList.push_back(action);
         }
         else if (blockCommand == "action_enable_size_factor")
         {
            SequencerAction* action = new SequencerAction;
            action->actionType      = EnableSizeFactor;

            mActionList.push_back(action);
         }
         else if (blockCommand == "sequencer")
         {
            P6DofSequencer* tempSequencer = new P6DofSequencer(GetScenario());
            tempSequencer->SetParentVehicle(mParentVehicle);
            tempSequencer->ProcessInput(aInput);

            std::string name = tempSequencer->GetName();
            for (std::vector<P6DofSequencer*>::const_iterator iter = mSubSequencerList.begin();
                 iter != mSubSequencerList.end();
                 ++iter)
            {
               if ((*iter)->GetName() == name)
               {
                  throw UtInput::BadValue(aInput, "Duplicate sequencer name (" + name + ")");
               }
            }

            mSubSequencerList.push_back(tempSequencer);
         }
         else
         {
            auto out = ut::log::error() << "Unrecognized command within P6DofSequencer::ProcessInput().";
            out.AddNote() << "Command: " << blockCommand;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }

   return myCommand;
}

bool P6DofSequencer::Initialize(int64_t aSimTime_nanosec)
{
   mLastSimTime_nanosec  = aSimTime_nanosec;
   mElapsedTimer_nanosec = 0;

   // Get the handles for the boolean inputs
   std::vector<SequencerEvent*>::iterator iter = mEventList.begin();
   while (iter != mEventList.end())
   {
      if ((*iter)->eventType == BooleanInput)
      {
         (*iter)->argumentHandle = mParentVehicle->GetInputHandle((*iter)->argumentString);
      }
      ++iter;
   }

   return true;
}

void P6DofSequencer::StartUp(int64_t aSimTime_nanosec)
{
   mLastSimTime_nanosec  = aSimTime_nanosec;
   mElapsedTimer_nanosec = 0;
}

int64_t P6DofSequencer::GetTimerTime() const
{
   return mElapsedTimer_nanosec;
}

void P6DofSequencer::AdjustNaming(const std::string& aNamePrefix)
{
   // Adjust naming on any events
   // There are currently no events that involve a P6DofVehicle name, but this is where
   // name adjusting may occur for events in the future...

   // Adjust naming on any actions
   std::vector<SequencerAction*>::const_iterator iterAction = mActionList.begin();
   while (iterAction != mActionList.end())
   {
      switch ((*iterAction)->actionType)
      {
      case JettisonObject:
      case IgniteSubObject:
      case ShutdownSubObject:
         (*iterAction)->argumentString = aNamePrefix + "_" + (*iterAction)->argumentString;
         break;
      default:
         break;
      }

      ++iterAction;
   }

   // Adjust naming on any subobject sequencers
   std::vector<P6DofSequencer*>::const_iterator iterSequencer = mSubSequencerList.begin();
   while (iterSequencer != mSubSequencerList.end())
   {
      (*iterSequencer)->AdjustNaming(aNamePrefix);
      ++iterSequencer;
   }
}

void P6DofSequencer::SetAllEventCounts()
{
   if (mAllEvents)
   {
      mAllEventNeededCount = mEventList.size();
   }
   else
   {
      mAllEventNeededCount = 1;
   }

   mAllEventCurrentCount = 0;
}

void P6DofSequencer::CheckForAllEventActivation()
{
   // Increment the count
   ++mAllEventCurrentCount;

   // We're activated if we have the needed count
   if (mAllEventCurrentCount >= mAllEventNeededCount)
   {
      mActivated = true;
   }
}

bool P6DofSequencer::ActivateExternally()
{
   // Return false if its already activated
   if (mExternalActivation || mActivated)
   {
      return false;
   }

   // Set the flag so that it will be activated
   mExternalActivation = true;

   return true;
}
