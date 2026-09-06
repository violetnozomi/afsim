// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfSimpleDopplerSignalProcessor.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMath.hpp"
#include "WsfPlatform.hpp"
#include "WsfRadarSensor.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"

namespace wsf
{

// ================================================================================================
//! The object factory for the MTI signal processor type.
// static
WsfSensorSignalProcessor* SimpleDopplerSignalProcessor::ObjectFactory(const std::string& aTypeName)
{
   WsfSensorSignalProcessor* instancePtr = nullptr;
   if ((aTypeName == "doppler") || (aTypeName == "Doppler") || (aTypeName == "simple_doppler") ||
       (aTypeName == "simple_Doppler"))
   {
      instancePtr = new SimpleDopplerSignalProcessor();
   }
   return instancePtr;
}

// ================================================================================================
// virtual
WsfSensorSignalProcessor* SimpleDopplerSignalProcessor::Clone() const
{
   return new SimpleDopplerSignalProcessor(*this);
}

// ================================================================================================
// virtual
bool SimpleDopplerSignalProcessor::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();
   if (command == "minimum_doppler_speed")
   {
      aInput.ReadValueOfType(mMinDopplerSpeed, UtInput::cSPEED);
      aInput.ValueGreaterOrEqual(mMinDopplerSpeed, 0.0);
   }
   else if (command == "maximum_doppler_speed")
   {
      aInput.ReadValueOfType(mMaxDopplerSpeed, UtInput::cSPEED);
      aInput.ValueGreater(mMaxDopplerSpeed, 0.0);
   }
   else if (command == "unfiltered_doppler_speed")
   {
      mFilterOwnshipFromDoppler = false;
   }
   else if (command == "filtered_doppler_speed")
   {
      mFilterOwnshipFromDoppler = true;
   }
   else
   {
      myCommand = WsfSensorSignalProcessor::ProcessInput(aInput);
   }

   return myCommand;
}

// ================================================================================================
// virtual
void SimpleDopplerSignalProcessor::Execute(double aSimTime, WsfSensorResult& aResult)
{
   if (aResult.GetTarget()->IsFalseTarget() && aResult.GetTarget()->GetSpeed() > 0.0)
   {
      return;
   }

   // Determine if within Doppler limits.
   if ((mMinDopplerSpeed > 0.0) || (mMaxDopplerSpeed < UtMath::cLIGHT_SPEED))
   {
      aResult.mCheckedStatus |= WsfSensorResult::cDOPPLER_LIMITS;
      aResult.mMeasurement.SetRangeRate(aResult.ComputeTargetDopplerSpeed(mFilterOwnshipFromDoppler));
      aResult.mMeasurement.SetRangeRateValid(true);
      double dopplerSpeed = fabs(aResult.mMeasurement.GetRangeRate());
      if ((dopplerSpeed < mMinDopplerSpeed) || (dopplerSpeed > mMaxDopplerSpeed))
      {
         aResult.mFailedStatus |= WsfSensorResult::cDOPPLER_LIMITS;
         aResult.mMeasurement.SetRangeRate(0.0);
         aResult.mMeasurement.SetRangeRateValid(false);
      }
   }
}

} // namespace wsf
