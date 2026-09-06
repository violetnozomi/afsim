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

#include "WsfSensorBeam.hpp"

#include "UtLog.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorComponent.hpp"
#include "WsfSensorMode.hpp"

// =================================================================================================
WsfSensorBeam::WsfSensorBeam()
   : mSensorModePtr(nullptr)
   , mBeamIndex(0)
   , mSignalProcessors()
{
}

// =================================================================================================
WsfSensorBeam& WsfSensorBeam::operator=(const WsfSensorBeam& aRhs)
{
   if (this != &aRhs)
   {
      mSensorModePtr    = aRhs.mSensorModePtr;
      mBeamIndex        = aRhs.mBeamIndex;
      mSignalProcessors = aRhs.mSignalProcessors;
   }
   return *this;
}

// =================================================================================================
bool WsfSensorBeam::ProcessInput(UtInput& aInput)
{
   // Loop over attached components to see if they can process the input.

   bool myCommand(true);

   WsfSensorMode* sensorModePtr(GetSensorMode());
   WsfSensor*     sensorPtr = (sensorModePtr != nullptr) ? sensorModePtr->GetSensor() : nullptr;
   assert((sensorModePtr != nullptr) && (sensorPtr != nullptr));

   if (mSignalProcessors.ProcessInput(aInput))
   {
   }
   else if (sensorPtr != nullptr)
   {
      myCommand = false; // set to false in case the following doesn't process.
      const WsfSensor::ComponentList& components(sensorPtr->GetComponents());
      if (components.HasComponents())
      {
         for (auto component : components)
         {
            if (component->ProcessBeamInput(aInput, *this))
            {
               myCommand = true;
               break;
            }
         }
      }
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

bool WsfSensorBeam::Initialize(double aSimTime)
{
   return mSignalProcessors.Initialize(aSimTime, mSensorModePtr->GetSensor(), mSensorModePtr);
}

// =================================================================================================
// virtual
double WsfSensorBeam::GetIntegrationGain() const
{
   ut::log::error() << "GetIntegrationGain not specified on the WsfSensorBeam class, please implement!!";
   return 1.0;
}

// =================================================================================================
// virtual
void WsfSensorBeam::SetIntegrationGain(double aIntegrationGain)
{
   ut::log::error() << "SetIntegration not specified on the WsfSensorBeam class, please implement!!";
}

// =================================================================================================
// virtual
int WsfSensorBeam::GetNumberOfPulsesIntegrated() const
{
   ut::log::error() << "GetNumberOfPulsesIntegrated not specified on the WsfSensorBeam class, please implement!!";
   return 1;
}
