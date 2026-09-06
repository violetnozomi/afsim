// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfSixDOF_Sequencer.hpp"

#include "UtDCM.hpp"
#include "UtEntity.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtInputFile.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtTable.hpp"
#include "UtVec3.hpp"
#include "WsfSixDOF_Mover.hpp"
#include "WsfSixDOF_ThrustProducerObject.hpp"
#include "WsfSixDOF_Utils.hpp"

wsf::six_dof::Sequencer* wsf::six_dof::Sequencer::Clone() const
{
   return new Sequencer(*this);
}

wsf::six_dof::Sequencer::Sequencer(const Sequencer& aSrc)
   : Object(aSrc)
   , mLastSimTime_nanosec(aSrc.mLastSimTime_nanosec)
   , mElapsedTimer_nanosec(aSrc.mElapsedTimer_nanosec)
   , mActivated(aSrc.mActivated)
   , mAllEvents(aSrc.mAllEvents)
   , mAllEventNeededCount(aSrc.mAllEventNeededCount)
   , mAllEventCurrentCount(aSrc.mAllEventCurrentCount)
   , mExternalActivation(aSrc.mExternalActivation)
{
   for (auto& event : aSrc.mEventList)
   {
      mEventList.push_back(ut::make_unique<SequencerEvent>(*(event.get())));
   }

   for (auto& action : aSrc.mActionList)
   {
      mActionList.push_back(ut::make_unique<SequencerAction>(*(action.get())));
   }

   for (auto& srcSub : aSrc.mSubSequencerList)
   {
      auto sub = srcSub;
      sub->SetParentVehicle(GetParentVehicle());

      mSubSequencerList.push_back(sub);
   }
}

bool wsf::six_dof::Sequencer::PendingTimeEvents(int64_t aSimTime_nanosec, std::set<int64_t>& aTimeOfEvents_nanosec)
{
   std::set<int64_t> timeOfEvents_nanosec;
   bool              valid = false;

   if (mActivated)
   {
      // Check sub-sequencers
      for (auto& subSequencer : mSubSequencerList)
      {
         std::set<int64_t> tempTimeOfEvents_nanosec;
         subSequencer->PendingTimeEvents(aSimTime_nanosec, tempTimeOfEvents_nanosec);

         for (const int64_t& eventTime : tempTimeOfEvents_nanosec)
         {
            timeOfEvents_nanosec.insert(eventTime);
            valid = true;
         }
      }
   }
   else
   {
      int64_t deltaTime_nano      = aSimTime_nanosec - mLastSimTime_nanosec;
      int64_t endElapsedTime_nano = mElapsedTimer_nanosec + deltaTime_nano;

      // Check all events, looking for time-based events
      for (auto& event : mEventList)
      {
         if ((event->eventType == Timer) || (event->eventType == LifeTime))
         {
            int64_t deltaPastTime_nano = 0;

            if (event->eventType == Timer)
            {
               deltaPastTime_nano = endElapsedTime_nano - event->argumentNanoTime;
            }
            else if (event->eventType == LifeTime)
            {
               deltaPastTime_nano = GetParentVehicle()->GetLifeTime_nanosec() - event->argumentNanoTime;
            }

            if (deltaPastTime_nano >= 0)
            {
               // The time the event occurs
               int64_t eventTime_nano = aSimTime_nanosec - deltaPastTime_nano;
               timeOfEvents_nanosec.insert(eventTime_nano);
               valid = true;
            }
         }
      }
   }

   if (valid)
   {
      aTimeOfEvents_nanosec = timeOfEvents_nanosec;
   }

   return valid;
}

void wsf::six_dof::Sequencer::Update(int64_t aSimTime_nanosec)
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
         for (auto& subSequencer : mSubSequencerList)
         {
            subSequencer->StartUp(activationTime_nanosec);
         }

         // Update the sub-sequencers
         for (auto& subSequencer : mSubSequencerList)
         {
            subSequencer->Update(aSimTime_nanosec);
         }
      }
   }
   else
   {
      // Already activated, update the sub-sequencers
      for (auto& subSequencer : mSubSequencerList)
      {
         subSequencer->Update(aSimTime_nanosec);
      }
   }

   // Set the 'last time' the current time
   mLastSimTime_nanosec = aSimTime_nanosec;
}

int64_t wsf::six_dof::Sequencer::TestForActivation(int64_t aSimTime_nanosec)
{
   int64_t timeOfActivation_nanosec = aSimTime_nanosec;

   // Sequencers may require 'all events' to trigger actions
   SetAllEventCounts();

   // This is used to correct for delays in responding to events
   int64_t eventTimeIntoFrame_nanosec = 0;

   for (auto& event : mEventList)
   {
      bool   lastState    = false;
      bool   currentState = false;
      double lastValue    = 0.0;
      double currentValue = 0.0;

      switch (event->eventType)
      {
      case Timer:
         if (mElapsedTimer_nanosec >= event->argumentNanoTime)
         {
            eventTimeIntoFrame_nanosec = mElapsedTimer_nanosec - event->argumentNanoTime;

            if (eventTimeIntoFrame_nanosec < 0)
            {
               eventTimeIntoFrame_nanosec = 0;
            }
            CheckForAllEventActivation();

            timeOfActivation_nanosec = aSimTime_nanosec - eventTimeIntoFrame_nanosec;
         }
         break;
      case LifeTime:
         if (GetParentVehicle())
         {
            if (GetParentVehicle()->GetLifeTime_sec() >= event->argumentValue)
            {
               CheckForAllEventActivation();
            }
         }
         break;
      case AboveAlt_ft:
         if (GetParentVehicle())
         {
            currentValue = GetParentVehicle()->GetAltitudeMSL_ft();
            lastValue    = GetParentVehicle()->GetLastAltitudeMSL_ft();

            if (currentValue > lastValue) // climbing
            {
               if (currentValue > event->argumentValue) // alt check
               {
                  CheckForAllEventActivation();
               }
            }
         }
         break;
      case BelowAlt_ft:
         if (GetParentVehicle())
         {
            currentValue = GetParentVehicle()->GetAltitudeMSL_ft();
            lastValue    = GetParentVehicle()->GetLastAltitudeMSL_ft();

            if (currentValue < lastValue) // descending
            {
               if (currentValue < event->argumentValue) // alt check
               {
                  CheckForAllEventActivation();
               }
            }
         }
         break;
      case DynamicPressureAbove_psf:
         if (GetParentVehicle())
         {
            currentValue = GetParentVehicle()->GetDynamicPressure_psf();
            lastValue    = GetParentVehicle()->GetLastDynamicPressure_psf();

            if (currentValue > lastValue) // increasing
            {
               if (currentValue > event->argumentValue)
               {
                  CheckForAllEventActivation();
               }
            }
         }
         break;
      case DynamicPressureBelow_psf:
         if (GetParentVehicle())
         {
            currentValue = GetParentVehicle()->GetDynamicPressure_psf();
            lastValue    = GetParentVehicle()->GetLastDynamicPressure_psf();

            if (currentValue < lastValue) // decreasing
            {
               if (currentValue < event->argumentValue)
               {
                  CheckForAllEventActivation();
               }
            }
         }
         break;
      case StaticPressureAbove_psf:
         if (GetParentVehicle())
         {
            currentValue = GetParentVehicle()->GetStaticPressure_psf();
            lastValue    = GetParentVehicle()->GetLastStaticPressure_psf();

            if (currentValue > lastValue) // increasing
            {
               if (currentValue > event->argumentValue)
               {
                  CheckForAllEventActivation();
               }
            }
         }
         break;
      case StaticPressureBelow_psf:
         if (GetParentVehicle())
         {
            currentValue = GetParentVehicle()->GetStaticPressure_psf();
            lastValue    = GetParentVehicle()->GetLastStaticPressure_psf();

            if (currentValue < lastValue) // decreasing
            {
               if (currentValue < event->argumentValue)
               {
                  CheckForAllEventActivation();
               }
            }
         }
         break;
      case FuelPercentBelow:
         if (GetParentVehicle())
         {
            currentValue = GetParentVehicle()->GetRemainingFuelPercent();

            if (currentValue < event->argumentValue)
            {
               CheckForAllEventActivation();
            }
         }
         break;
      case GroundDistance_m:
         if (GetParentVehicle())
         {
            currentValue = 1000.0 * GetParentVehicle()->GetRangeSinceStart_km();

            if (currentValue > event->argumentValue)
            {
               CheckForAllEventActivation();
            }
         }
         break;
      case NxAbove_g:
         if (GetParentVehicle())
         {
            currentValue = GetParentVehicle()->GetNx_g();
            lastValue    = GetParentVehicle()->GetLastNx_g();

            if (currentValue > lastValue) // increasing
            {
               if (currentValue > event->argumentValue)
               {
                  CheckForAllEventActivation();
               }
            }
         }
         break;
      case NxBelow_g:
         if (GetParentVehicle())
         {
            currentValue = GetParentVehicle()->GetNx_g();
            lastValue    = GetParentVehicle()->GetLastNx_g();

            if (currentValue < lastValue) // decreasing
            {
               if (currentValue < event->argumentValue)
               {
                  CheckForAllEventActivation();
               }
            }
         }
         break;
      case NyAbove_g:
         if (GetParentVehicle())
         {
            currentValue = GetParentVehicle()->GetNy_g();
            lastValue    = GetParentVehicle()->GetLastNy_g();

            if (currentValue > lastValue) // increasing
            {
               if (currentValue > event->argumentValue)
               {
                  CheckForAllEventActivation();
               }
            }
         }
         break;
      case NyBelow_g:
         if (GetParentVehicle())
         {
            currentValue = GetParentVehicle()->GetNy_g();
            lastValue    = GetParentVehicle()->GetLastNy_g();

            if (currentValue < lastValue) // decreasing
            {
               if (currentValue < event->argumentValue)
               {
                  CheckForAllEventActivation();
               }
            }
         }
         break;
      case NzAbove_g:
         if (GetParentVehicle())
         {
            currentValue = GetParentVehicle()->GetNz_g();
            lastValue    = GetParentVehicle()->GetLastNz_g();

            if (currentValue > lastValue) // increasing
            {
               if (currentValue > event->argumentValue)
               {
                  CheckForAllEventActivation();
               }
            }
         }
         break;
      case NzBelow_g:
         if (GetParentVehicle())
         {
            currentValue = GetParentVehicle()->GetNz_g();
            lastValue    = GetParentVehicle()->GetLastNz_g();

            if (currentValue < lastValue) // decreasing
            {
               if (currentValue < event->argumentValue)
               {
                  CheckForAllEventActivation();
               }
            }
         }
         break;
      case ReleasedFromParent:
         if (GetParentVehicle())
         {
            currentState = GetParentVehicle()->IsCaptive();
            lastState    = GetParentVehicle()->GetLastIsCaptive();

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
         if (GetParentVehicle())
         {
            currentState = GetParentVehicle()->GetBooleanInput(event->argumentHandle);

            if (currentState)
            {
               lastState = GetParentVehicle()->GetLastBooleanInput(event->argumentHandle);
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
   }

   // Set activated flag if externally activated and not yet activated
   if (mExternalActivation && !mActivated)
   {
      mActivated = true;
   }

   // Perform action(s) if activated
   if (mActivated)
   {
      for (auto& action : mActionList)
      {
         std::string           targetObjName;
         Mover*                tgtObj         = nullptr;
         ThrustProducerObject* thrustProducer = nullptr;

         switch (action->actionType)
         {
         case JettisonObject:
            if (GetParentVehicle())
            {
               GetParentVehicle()->JettisonSubobject(action->argumentString, aSimTime_nanosec);
            }
            break;
         case JettisonSelf:
            if (GetParentVehicle())
            {
               // The sequencer's parent is the object we want to Jettison,
               // so get the parent of the parent, and have it Jettison the sequencer's parent.
               Mover* parentsParentObject = GetParentVehicle()->GetParentVehicle();
               if (parentsParentObject)
               {
                  parentsParentObject->JettisonSubobject(GetParentVehicle(), aSimTime_nanosec);
               }
            }
            break;
         case IgniteEngine:
            if (GetParentVehicle())
            {
               // Find the engine
               thrustProducer = GetParentVehicle()->GetThrustProducerObjectByName(action->argumentString);
               if (thrustProducer != nullptr)
               {
                  thrustProducer->Ignite(eventTimeIntoFrame_nanosec);
               }
            }
            break;
         case IgniteSubObject:
            if (GetParentVehicle())
            {
               targetObjName = action->argumentString;
               tgtObj        = GetParentVehicle()->GetSubobjectByName(targetObjName);
               if (tgtObj)
               {
                  tgtObj->IgniteObject(eventTimeIntoFrame_nanosec);
               }
            }
            break;
         case IgniteSelfObject:
            if (GetParentVehicle())
            {
               GetParentVehicle()->IgniteObject(eventTimeIntoFrame_nanosec);
            }
            break;
         case TerminateThrust:
            if (GetParentVehicle())
            {
               GetParentVehicle()->TerminateThrust();
            }
            break;
         case ShutdownEngine:
            if (GetParentVehicle())
            {
               // Find the engine
               thrustProducer = GetParentVehicle()->GetThrustProducerObjectByName(action->argumentString);
               thrustProducer->Shutdown(eventTimeIntoFrame_nanosec);
            }
            break;
         case ShutdownSubObject:
            if (GetParentVehicle())
            {
               targetObjName = action->argumentString;
               tgtObj        = GetParentVehicle()->GetSubobjectByName(targetObjName);
               if (tgtObj)
               {
                  tgtObj->ShutdownObject();
               }
            }
            break;
         case ChangeAeroMode:
            if (GetParentVehicle())
            {
               targetObjName = action->argumentString;

               // Change the aero mode
               GetParentVehicle()->SetActiveAeroObject(targetObjName);
            }
            break;
         case EnableSizeFactor:
            if (GetParentVehicle())
            {
               GetParentVehicle()->SetSizeFactorEnabled(true);
            }
            break;
         case ActivateSubobjectSequencer:
            if (GetParentVehicle())
            {
               GetParentVehicle()->ActivateSequencer(action->argumentString, action->argumentSecondString);
            }
            break;
         case EnableControls:
            if (GetParentVehicle())
            {
               GetParentVehicle()->EnableControls(true);
            }
            break;
         case DisableControls:
            if (GetParentVehicle())
            {
               GetParentVehicle()->EnableControls(false);
            }
            break;
         default:
            break;
         }
      }
   }

   return timeOfActivation_nanosec;
}

void wsf::six_dof::Sequencer::SetParentVehicle(Mover* aParentVehicle)
{
   mParentVehicle = aParentVehicle;

   for (auto& i : mSubSequencerList)
   {
      i->SetParentVehicle(aParentVehicle);
   }
}

bool wsf::six_dof::Sequencer::ProcessInput(UtInput& aInput)
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

            auto event              = ut::make_unique<SequencerEvent>();
            event->eventType        = Timer;
            event->argumentValue    = 0.0;
            event->argumentNanoTime = utils::TimeToTime(value);

            mEventList.push_back(std::move(event));
         }
         else if (blockCommand == "event_timer_int_msec")
         {
            int ivalue;
            aInput.ReadValue(ivalue);

            auto event              = ut::make_unique<SequencerEvent>();
            event->eventType        = Timer;
            event->argumentValue    = 0.0;
            event->argumentNanoTime = ivalue * 1000000;

            mEventList.push_back(std::move(event));
         }
         else if (blockCommand == "event_timer_int_nanosec")
         {
            int ivalue;
            aInput.ReadValue(ivalue);

            auto event              = ut::make_unique<SequencerEvent>();
            event->eventType        = Timer;
            event->argumentValue    = 0.0;
            event->argumentNanoTime = ivalue;

            mEventList.push_back(std::move(event));
         }
         else if (blockCommand == "event_lifetime")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cTIME);

            auto event              = ut::make_unique<SequencerEvent>();
            event->eventType        = LifeTime;
            event->argumentValue    = 0.0;
            event->argumentNanoTime = utils::TimeToTime(value);

            mEventList.push_back(std::move(event));
         }
         else if (blockCommand == "event_lifetime_int_msec")
         {
            int ivalue;
            aInput.ReadValue(ivalue);

            auto event              = ut::make_unique<SequencerEvent>();
            event->eventType        = LifeTime;
            event->argumentValue    = 0.0;
            event->argumentNanoTime = ivalue * 1000000;

            mEventList.push_back(std::move(event));
         }
         else if (blockCommand == "event_lifetime_int_nanosec")
         {
            int ivalue;
            aInput.ReadValue(ivalue);

            auto event              = ut::make_unique<SequencerEvent>();
            event->eventType        = LifeTime;
            event->argumentValue    = 0.0;
            event->argumentNanoTime = ivalue;

            mEventList.push_back(std::move(event));
         }
         else if (blockCommand == "event_above_alt")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cLENGTH);

            auto event              = ut::make_unique<SequencerEvent>();
            event->eventType        = AboveAlt_ft;
            event->argumentValue    = value * UtMath::cFT_PER_M;
            event->argumentNanoTime = 0;

            mEventList.push_back(std::move(event));
         }
         else if (blockCommand == "event_below_alt")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cLENGTH);

            auto event              = ut::make_unique<SequencerEvent>();
            event->eventType        = BelowAlt_ft;
            event->argumentValue    = value * UtMath::cFT_PER_M;
            event->argumentNanoTime = 0;

            mEventList.push_back(std::move(event));
         }
         else if (blockCommand == "event_dynamic_pressure_above")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cPRESSURE);

            auto event              = ut::make_unique<SequencerEvent>();
            event->eventType        = DynamicPressureAbove_psf;
            event->argumentValue    = value / (6894.75729 / 144.0);
            event->argumentNanoTime = 0;

            mEventList.push_back(std::move(event));
         }
         else if (blockCommand == "event_dynamic_pressure_below")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cPRESSURE);

            auto event              = ut::make_unique<SequencerEvent>();
            event->eventType        = DynamicPressureBelow_psf;
            event->argumentValue    = value / (6894.75729 / 144.0);
            event->argumentNanoTime = 0;

            mEventList.push_back(std::move(event));
         }
         else if (blockCommand == "event_static_pressure_above")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cPRESSURE);

            auto event              = ut::make_unique<SequencerEvent>();
            event->eventType        = StaticPressureAbove_psf;
            event->argumentValue    = value / (6894.75729 / 144.0);
            event->argumentNanoTime = 0;

            mEventList.push_back(std::move(event));
         }
         else if (blockCommand == "event_static_pressure_below")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cPRESSURE);

            auto event              = ut::make_unique<SequencerEvent>();
            event->eventType        = StaticPressureBelow_psf;
            event->argumentValue    = value / (6894.75729 / 144.0);
            event->argumentNanoTime = 0;

            mEventList.push_back(std::move(event));
         }
         else if (blockCommand == "event_fuel_percent_below")
         {
            double value;
            aInput.ReadValue(value);

            auto event              = ut::make_unique<SequencerEvent>();
            event->eventType        = FuelPercentBelow;
            event->argumentValue    = value;
            event->argumentNanoTime = 0;

            mEventList.push_back(std::move(event));
         }
         else if (blockCommand == "event_ground_distance")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cLENGTH);

            auto event              = ut::make_unique<SequencerEvent>();
            event->eventType        = GroundDistance_m;
            event->argumentValue    = value;
            event->argumentNanoTime = 0;

            mEventList.push_back(std::move(event));
         }
         else if (blockCommand == "event_nx_above")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cACCELERATION);

            auto event              = ut::make_unique<SequencerEvent>();
            event->eventType        = NxAbove_g;
            event->argumentValue    = value / 9.80665;
            event->argumentNanoTime = 0;

            mEventList.push_back(std::move(event));
         }
         else if (blockCommand == "event_nx_below")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cACCELERATION);

            auto event              = ut::make_unique<SequencerEvent>();
            event->eventType        = NxBelow_g;
            event->argumentValue    = value / 9.80665;
            event->argumentNanoTime = 0;

            mEventList.push_back(std::move(event));
         }
         else if (blockCommand == "event_ny_above")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cACCELERATION);

            auto event              = ut::make_unique<SequencerEvent>();
            event->eventType        = NyAbove_g;
            event->argumentValue    = value / 9.80665;
            event->argumentNanoTime = 0;

            mEventList.push_back(std::move(event));
         }
         else if (blockCommand == "event_ny_below")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cACCELERATION);

            auto event              = ut::make_unique<SequencerEvent>();
            event->eventType        = NyBelow_g;
            event->argumentValue    = value / 9.80665;
            event->argumentNanoTime = 0;

            mEventList.push_back(std::move(event));
         }
         else if (blockCommand == "event_nz_above")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cACCELERATION);

            auto event              = ut::make_unique<SequencerEvent>();
            event->eventType        = NzAbove_g;
            event->argumentValue    = value / 9.80665;
            event->argumentNanoTime = 0;

            mEventList.push_back(std::move(event));
         }
         else if (blockCommand == "event_nz_below")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cACCELERATION);

            auto event              = ut::make_unique<SequencerEvent>();
            event->eventType        = NzBelow_g;
            event->argumentValue    = value / 9.80665;
            event->argumentNanoTime = 0;

            mEventList.push_back(std::move(event));
         }
         else if (blockCommand == "event_released_from_parent")
         {
            auto event              = ut::make_unique<SequencerEvent>();
            event->eventType        = ReleasedFromParent;
            event->argumentNanoTime = 0;

            mEventList.push_back(std::move(event));
         }
         else if (blockCommand == "event_manual_input_trigger")
         {
            int value;
            aInput.ReadValue(value);

            auto event              = ut::make_unique<SequencerEvent>();
            event->eventType        = ManualInputTrigger;
            event->argumentInteger  = value;
            event->argumentNanoTime = 0;

            mEventList.push_back(std::move(event));
         }
         else if (blockCommand == "event_manual_input_button")
         {
            int value;
            aInput.ReadValue(value);

            auto event              = ut::make_unique<SequencerEvent>();
            event->eventType        = ManualInputButton;
            event->argumentInteger  = value;
            event->argumentNanoTime = 0;

            mEventList.push_back(std::move(event));
         }
         else if (blockCommand == "event_manual_input_button_released")
         {
            int value;
            aInput.ReadValue(value);

            auto event              = ut::make_unique<SequencerEvent>();
            event->eventType        = ManualInputButtonReleased;
            event->argumentInteger  = value;
            event->argumentNanoTime = 0;

            mEventList.push_back(std::move(event));
         }
         else if (blockCommand == "event_boolean_input")
         {
            std::string value;
            aInput.ReadValue(value);

            auto event              = ut::make_unique<SequencerEvent>();
            event->eventType        = BooleanInput;
            event->argumentString   = value;
            event->argumentHandle   = 0;
            event->argumentNanoTime = 0;

            mEventList.push_back(std::move(event));
         }
         else if (blockCommand == "event_null")
         {
            auto event              = ut::make_unique<SequencerEvent>();
            event->eventType        = NoEvent;
            event->argumentNanoTime = 0;

            mEventList.push_back(std::move(event));
         }
         else if (blockCommand == "action_null")
         {
            auto action        = ut::make_unique<SequencerAction>();
            action->actionType = NoAction;

            mActionList.push_back(std::move(action));
         }
         else if (blockCommand == "action_jett_obj")
         {
            std::string value;
            aInput.ReadValue(value);

            auto action            = ut::make_unique<SequencerAction>();
            action->actionType     = JettisonObject;
            action->argumentString = value;

            mActionList.push_back(std::move(action));
         }
         else if (blockCommand == "action_jett_self")
         {
            auto action        = ut::make_unique<SequencerAction>();
            action->actionType = JettisonSelf;

            mActionList.push_back(std::move(action));
         }
         else if (blockCommand == "action_ignite_engine")
         {
            std::string value;
            aInput.ReadValue(value);

            auto action            = ut::make_unique<SequencerAction>();
            action->actionType     = IgniteEngine;
            action->argumentString = value;

            mActionList.push_back(std::move(action));
         }
         else if (blockCommand == "action_activate_subobject_sequencer")
         {
            std::string value1;
            aInput.ReadValue(value1);
            std::string value2;
            aInput.ReadValue(value2);

            auto action                  = ut::make_unique<SequencerAction>();
            action->actionType           = ActivateSubobjectSequencer;
            action->argumentString       = value1;
            action->argumentSecondString = value2;

            mActionList.push_back(std::move(action));
         }
         else if (blockCommand == "action_enable_controls")
         {
            auto action        = ut::make_unique<SequencerAction>();
            action->actionType = EnableControls;

            mActionList.push_back(std::move(action));
         }
         else if (blockCommand == "action_disable_controls")
         {
            auto action        = ut::make_unique<SequencerAction>();
            action->actionType = DisableControls;

            mActionList.push_back(std::move(action));
         }
         else if (blockCommand == "action_ignite_subobject")
         {
            std::string value;
            aInput.ReadValue(value);

            auto action            = ut::make_unique<SequencerAction>();
            action->actionType     = IgniteSubObject;
            action->argumentString = value;

            mActionList.push_back(std::move(action));
         }
         else if (blockCommand == "action_ignite_self")
         {
            auto action        = ut::make_unique<SequencerAction>();
            action->actionType = IgniteSelfObject;

            mActionList.push_back(std::move(action));
         }
         else if (blockCommand == "action_terminate_thrust")
         {
            auto action        = ut::make_unique<SequencerAction>();
            action->actionType = TerminateThrust;

            mActionList.push_back(std::move(action));
         }
         else if (blockCommand == "action_shutdown_engine")
         {
            std::string value;
            aInput.ReadValue(value);

            auto action            = ut::make_unique<SequencerAction>();
            action->actionType     = ShutdownEngine;
            action->argumentString = value;

            mActionList.push_back(std::move(action));
         }
         else if (blockCommand == "action_shutdown_subobject")
         {
            std::string value;
            aInput.ReadValue(value);

            auto action            = ut::make_unique<SequencerAction>();
            action->actionType     = ShutdownSubObject;
            action->argumentString = value;

            mActionList.push_back(std::move(action));
         }
         else if (blockCommand == "action_change_aero_mode")
         {
            std::string value;
            aInput.ReadValue(value);

            auto action            = ut::make_unique<SequencerAction>();
            action->actionType     = ChangeAeroMode;
            action->argumentString = value;

            mActionList.push_back(std::move(action));
         }
         else if (blockCommand == "action_enable_size_factor")
         {
            auto action        = ut::make_unique<SequencerAction>();
            action->actionType = EnableSizeFactor;

            mActionList.push_back(std::move(action));
         }
         else if (blockCommand == "sequencer")
         {
            auto tempSequencer = ut::make_unique<Sequencer>();
            tempSequencer->SetParentVehicle(GetParentVehicle());
            tempSequencer->ProcessInput(aInput);

            std::string name = tempSequencer->GetName();
            for (auto& sub : mSubSequencerList)
            {
               if (sub->GetName() == name)
               {
                  throw UtInput::BadValue(aInput, "Duplicate sequencer name (" + name + ")");
               }
            }

            mSubSequencerList.push_back(std::move(tempSequencer));
         }
         else
         {
            auto out = ut::log::error() << "Unrecognized command within Sequencer::ProcessInput().";
            out.AddNote() << "Command: " << blockCommand;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }

   return myCommand;
}

bool wsf::six_dof::Sequencer::Initialize(int64_t aSimTime_nanosec)
{
   mLastSimTime_nanosec  = aSimTime_nanosec;
   mElapsedTimer_nanosec = 0;

   // Get the handles for the boolean inputs
   for (auto& event : mEventList)
   {
      if (event->eventType == BooleanInput)
      {
         event->argumentHandle = GetParentVehicle()->GetInputHandle(event->argumentString);
      }
   }

   return true;
}

void wsf::six_dof::Sequencer::StartUp(int64_t aSimTime_nanosec)
{
   mLastSimTime_nanosec  = aSimTime_nanosec;
   mElapsedTimer_nanosec = 0;
}

int64_t wsf::six_dof::Sequencer::GetTimerTime() const
{
   return mElapsedTimer_nanosec;
}

void wsf::six_dof::Sequencer::AdjustNaming(const std::string& aNamePrefix)
{
   // Adjust naming on any events
   // There are currently no events that involve a Mover name, but this is where
   // name adjusting may occur for events in the future...

   // Adjust naming on any actions
   for (auto& action : mActionList)
   {
      switch (action->actionType)
      {
      case JettisonObject:
      case IgniteSubObject:
      case ShutdownSubObject:
         action->argumentString = aNamePrefix + "_" + action->argumentString;
         break;
      default:
         break;
      }
   }

   // Adjust naming on any subobject sequencers
   for (auto& sequencer : mSubSequencerList)
   {
      sequencer->AdjustNaming(aNamePrefix);
   }
}

void wsf::six_dof::Sequencer::SetAllEventCounts()
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

void wsf::six_dof::Sequencer::CheckForAllEventActivation()
{
   // Increment the count
   ++mAllEventCurrentCount;

   // We're activated if we have the needed count
   if (mAllEventCurrentCount >= mAllEventNeededCount)
   {
      mActivated = true;
   }
}

bool wsf::six_dof::Sequencer::ActivateExternally()
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
