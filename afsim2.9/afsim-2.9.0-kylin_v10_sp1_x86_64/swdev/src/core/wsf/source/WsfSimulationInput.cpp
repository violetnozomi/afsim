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

#include "WsfSimulationInput.hpp"

#include <climits>
#include <iostream>
#include <memory>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtRandom.hpp"
#include "UtWallClock.hpp"
#include "WsfDateTime.hpp"
#include "WsfDeferredInput.hpp"
#include "WsfIFF_Manager.hpp"
#include "WsfMover.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformTypes.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrack.hpp"
#include "WsfZone.hpp"

// =================================================================================================
WsfSimulationInput::WsfSimulationInput(WsfScenario& aScenario)
   : mScenarioPtr(&aScenario)
   , mDateTimePtr(new WsfDateTime)
   , mUseConstantRequiredPd(false)
   , mRandomizeFrequency(false)
   , mUseDefaultFrequency(false)
   , mNumberOfThreads(4)
   , mBreakUpdateTime(.5)
   , mDebugMultiThreading(false)
   , mProcessPriority(cPP_ABOVE_NORMAL)
   , mAllowClutterCalculationShortcuts(true)
   , mAllowEM_PropagationCalculationShortcuts(true)
{
   mIsRealTime            = false;
   mMultiThreaded         = false;
   mWallClockTimingMethod = (int)UtWallClock::cDEFAULT;

   mMinimumMoverTimestep = -1.0;
   mEndTime              = 60.0;
   mClockRate            = 1.0;
}

// =================================================================================================
WsfSimulationInput::~WsfSimulationInput()
{
   delete mDateTimePtr;
}

// =================================================================================================
//! Process simulation input.
//!
//! @see WsfObject::ProcessInput for more details.
bool WsfSimulationInput::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "end_time")
   {
      aInput.ReadValueOfType(mEndTime, UtInput::cTIME);
      aInput.ValueGreater(mEndTime, 0.0);
   }
   else if (command == "minimum_mover_timestep")
   {
      aInput.ReadValueOfType(mMinimumMoverTimestep, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mMinimumMoverTimestep, 0.0);
   }
   else if (command == "clock_rate")
   {
      aInput.ReadValue(mClockRate);
      aInput.ValueGreaterOrEqual(mClockRate, 0.0);
   }
   else if (command == "use_constant_required_pd")
   {
      aInput.ReadValue(mUseConstantRequiredPd);
   }
   else if (command == "randomize_radar_frequencies")
   {
      aInput.ReadValue(mRandomizeFrequency);
   }
   else if (command == "use_default_radar_frequencies")
   {
      aInput.ReadValue(mUseDefaultFrequency);
   }
   else if (command == "timing_method")
   {
      std::string method;
      aInput.ReadValue(method);
      if (method == "default")
      {
         mWallClockTimingMethod = UtWallClock::cDEFAULT;
      }
      else if (method == "performance_counter")
      {
         mWallClockTimingMethod = UtWallClock::cPERFORMANCE_COUNTER;
      }
      else if (method == "system_time")
      {
         mWallClockTimingMethod = UtWallClock::cSYSTEM_TIME;
      }
      else if (method == "tick_count")
      {
         mWallClockTimingMethod = UtWallClock::cTICK_COUNT;
      }
   }
   else if (command == "realtime")
   {
      mIsRealTime = true;
   }
   else if (command == "non-realtime")
   {
      mIsRealTime = false;
   }
   else if (command == "multi_thread")
   {
      mMultiThreaded = true;
   }
   else if (command == "multi_threading")
   {
      aInput.ReadValue(mMultiThreaded);
   }
   else if (command == "number_of_threads")
   {
      int nValue(0);
      aInput.ReadValue(nValue);
      aInput.ValueGreaterOrEqual(nValue, 1);
      mNumberOfThreads = static_cast<unsigned int>(nValue);
   }
   else if (command == "sensor_update_break_time")
   {
      aInput.ReadValueOfType(mBreakUpdateTime, UtInput::cTIME);
      aInput.ValueGreater(mBreakUpdateTime, 0.0);
   }
   else if (command == "debug_multi_threading")
   {
      mDebugMultiThreading = aInput.ReadBool();
   }
   else if (command == "allow_clutter_calculation_shortcuts")
   {
      aInput.ReadValue(mAllowClutterCalculationShortcuts);
   }
   else if (command == "allow_propagation_calculation_shortcuts")
   {
      aInput.ReadValue(mAllowEM_PropagationCalculationShortcuts);
   }
   else if (command == "process_priority")
   {
      std::string processPriority;
      aInput.ReadValue(processPriority);
      if (processPriority == "low")
      {
         mProcessPriority = cPP_BELOW_NORMAL;
      }
      else if (processPriority == "normal")
      {
         mProcessPriority = cPP_NORMAL;
      }
      else if (processPriority == "above_normal")
      {
         mProcessPriority = cPP_ABOVE_NORMAL;
      }
      else if (processPriority == "high")
      {
         mProcessPriority = cPP_HIGH;
      }
      else if (processPriority == "realtime")
      {
         mProcessPriority = cPP_REALTIME;
      }
   }
   else if (mDateTimePtr->ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
void WsfSimulationInput::LoadComplete() {}

// =================================================================================================
bool WsfEventStepSimulationInput::ProcessInput(UtInput& aInput, WsfSimulationInput& aSimInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();
   if (command == "multi_thread_update_rate")
   {
      double threadRate;
      aInput.ReadValueOfType(threadRate, UtInput::cFREQUENCY);
      aInput.ValueGreaterOrEqual(threadRate, 0.0);
      if (threadRate > 0.0)
      {
         mThreadUpdateInterval = 1.0 / threadRate;
         aSimInput.SetMultiThreaded(true);
      }
      else
      {
         aSimInput.SetMultiThreaded(false);
      }
   }
   else if (command == "multi_thread_update_interval")
   {
      aInput.ReadValueOfType(mThreadUpdateInterval, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mThreadUpdateInterval, 0.0);
      if (mThreadUpdateInterval > 0.0)
      {
         aSimInput.SetMultiThreaded(true);
      }
      else
      {
         aSimInput.SetMultiThreaded(false);
      }
   }
   else if (command == "platform_update_multiplier")
   {
      aInput.ReadValue(mPlatformThreadUpdateMultiplier);
      aInput.ValueGreater(mPlatformThreadUpdateMultiplier, 0);
   }
   else if (command == "sensor_update_multiplier")
   {
      aInput.ReadValue(mSensorThreadUpdateMultiplier);
      aInput.ValueGreater(mPlatformThreadUpdateMultiplier, 0);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
WsfEventStepSimulationInput::WsfEventStepSimulationInput()
   : mThreadUpdateInterval(1.0)
   , mPlatformThreadUpdateMultiplier(1)
   , mSensorThreadUpdateMultiplier(1)
{
}

// =================================================================================================
WsfFrameStepSimulationInput::WsfFrameStepSimulationInput()
   : mFrameTime(0.25)
{
}

// =================================================================================================
bool WsfFrameStepSimulationInput::ProcessInput(UtInput& aInput, WsfSimulationInput& aSimInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();
   if (command == "frame_rate")
   {
      double frameRate;
      aInput.ReadValueOfType(frameRate, UtInput::cFREQUENCY);
      aInput.ValueGreater(frameRate, 0.0);
      mFrameTime = 1.0 / frameRate;
   }
   else if (command == "frame_time")
   {
      aInput.ReadValueOfType(mFrameTime, UtInput::cTIME);
      aInput.ValueGreater(mFrameTime, 0.0);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
bool WsfDefaultSimulationInput::ProcessInput(UtInput& aInput)
{
   return WsfSimulationInput::ProcessInput(aInput) || WsfEventStepSimulationInput::ProcessInput(aInput, *this) ||
          WsfFrameStepSimulationInput::ProcessInput(aInput, *this);
}

// =================================================================================================
WsfDefaultSimulationInput::WsfDefaultSimulationInput(WsfScenario& aScenario)
   : WsfSimulationInput(aScenario)
{
}

// =================================================================================================
WsfDefaultSimulationInput::~WsfDefaultSimulationInput() {}
