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

// WsfIRST_Sensor is a simple IRST sensor implementation.

#include "WsfIRST_Sensor.hpp"

#include "UtInput.hpp"
#include "UtMemory.hpp"
#include "WsfDefaultSensorScheduler.hpp"
#include "WsfDefaultSensorTracker.hpp"
#include "WsfPlatform.hpp"
#include "WsfSensorModeList.hpp"
#include "WsfSimulation.hpp"

// =================================================================================================
WsfIRST_Sensor::WsfIRST_Sensor(WsfScenario& aScenario)
   : WsfSensor(aScenario)
   , mIRST_ModeList()
{
   SetClass(cPASSIVE | cINFRARED); // This is a passive EO sensor
   // Create the mode list with the sensor-specific mode template.
   SetModeList(ut::make_unique<WsfSensorModeList>(new IRST_Mode(aScenario)));

   // Assign the default sensor scheduler and tracker
   SetScheduler(ut::make_unique<WsfDefaultSensorScheduler>());
   SetTracker(ut::make_unique<WsfDefaultSensorTracker>(aScenario));
}

// =================================================================================================
WsfIRST_Sensor::WsfIRST_Sensor(const WsfIRST_Sensor& aSrc)
   : WsfSensor(aSrc)
   , mIRST_ModeList()
{
}

// =================================================================================================
// virtual
WsfSensor* WsfIRST_Sensor::Clone() const
{
   return new WsfIRST_Sensor(*this);
}

// =================================================================================================
// virtual
bool WsfIRST_Sensor::Initialize(double aSimTime)
{
   bool ok = WsfSensor::Initialize(aSimTime);

   // Reduce future dynamic casting by extracting derived class mode pointers.
   mModeListPtr->GetDerivedModeList(mIRST_ModeList);

   return ok;
}

// =================================================================================================
// virtual
void WsfIRST_Sensor::Update(double aSimTime)
{
   // Bypass updates if not time for an update.  This avoids unnecessary device updates.
   // (A little slop is allowed to make sure event-driven chances occur as scheduled)
   if (mNextUpdateTime <= (aSimTime + 1.0E-5))
   {
      WsfSensor::Update(aSimTime);
      PerformScheduledDetections(aSimTime); // Perform any required detection attempts
   }
}

// =================================================================================================
// virtual
size_t WsfIRST_Sensor::GetEM_RcvrCount() const
{
   // Returns 0 prior to initialization
   return mIRST_ModeList.empty() ? 0U : 1U;
}

// =================================================================================================
// virtual
WsfEM_Rcvr& WsfIRST_Sensor::GetEM_Rcvr(size_t aIndex) const
{
   return mIRST_ModeList[mModeListPtr->GetCurrentMode()]->GetReceiver();
}

// =================================================================================================
// Definition for the nested class that implements the mode of the sensor.
// =================================================================================================

WsfIRST_Sensor::IRST_Mode::IRST_Mode(WsfScenario& aScenario)
   : WsfEOIR_IRST_SensorMode(aScenario)
{
   SetVisualBandDefault(false); // require 'band' to be specified
}

// =================================================================================================
// virtual
WsfMode* WsfIRST_Sensor::IRST_Mode::Clone() const
{
   return new IRST_Mode(*this);
}

// =================================================================================================
// virtual
bool WsfIRST_Sensor::IRST_Mode::Initialize(double aSimTime)
{
   return WsfEOIR_IRST_SensorMode::Initialize(aSimTime);
}

// =================================================================================================
// virtual
bool WsfIRST_Sensor::IRST_Mode::ProcessInput(UtInput& aInput)
{
   return WsfEOIR_IRST_SensorMode::ProcessInput(aInput);
}

// =================================================================================================
bool WsfIRST_Sensor::IRST_Mode::AttemptToDetect(double aSimTime, WsfPlatform* aTargetPtr, Settings& aSettings, Result& aResult)
{
   if (BeginDetectionAttempt(aSimTime, aTargetPtr, aSettings, aResult))
   {
      if (mUseSimpleVisualDetector)
      {
         AttemptToDetectVisual(aSimTime, aTargetPtr, aSettings, aResult);
      }
      else
      {
         AttemptToDetectInfrared(aSimTime, aTargetPtr, aSettings, aResult);
      }
   }

   return EndDetectionAttempt(aSimTime, aTargetPtr, aSettings, aResult);
}

// =================================================================================================
// virtual
void WsfIRST_Sensor::IRST_Mode::Deselect(double aSimTime)
{
   mRcvr.Deactivate();
}

// =================================================================================================
// virtual
void WsfIRST_Sensor::IRST_Mode::Select(double aSimTime)
{
   mRcvr.Activate();
}
