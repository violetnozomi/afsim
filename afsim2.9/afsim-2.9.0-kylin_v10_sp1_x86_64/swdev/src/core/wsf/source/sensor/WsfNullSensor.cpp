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

#include "WsfNullSensor.hpp"

#include "WsfSensorResult.hpp"

// =================================================================================================
WsfNullSensor::WsfNullSensor(WsfScenario& aScenario)
   : WsfSensor(aScenario)
{
   SetUpdateInterval(1000.0); // Force it to a large value.
}

// =================================================================================================
WsfNullSensor::WsfNullSensor(const WsfNullSensor& aSrc)
   : WsfSensor(aSrc)
{
}

// =================================================================================================
// virtual
WsfNullSensor::~WsfNullSensor() {}

// =================================================================================================
// virtual
bool WsfNullSensor::AttemptToDetect(double aSimTime, WsfPlatform* aTargetPtr, Settings& aSettings, WsfSensorResult& aResult)
{
   // The 'Update()' method doesn't do anything so this shouldn't be called.
   // But in case it does we will return indicate nothing got checked.
   aResult.Reset();
   return false;
}

// =================================================================================================
// virtual
WsfSensor* WsfNullSensor::Clone() const
{
   return new WsfNullSensor(*this);
}

// =================================================================================================
// virtual
bool WsfNullSensor::ProcessInput(UtInput& aInput)
{
   // The base class is allowed to process its input. If it isn't a base class command then
   // we just ignore the command, thus allowing any input...
   WsfSensor::ProcessInput(aInput);
   return true;
}

// =================================================================================================
// virtual
void WsfNullSensor::Update(double aSimTime)
{
   // Does nothing.
}
