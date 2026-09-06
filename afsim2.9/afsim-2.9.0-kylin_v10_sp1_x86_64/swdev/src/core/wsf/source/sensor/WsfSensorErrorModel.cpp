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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "WsfSensorErrorModel.hpp"

#include "WsfMeasurement.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSimulation.hpp"

// =================================================================================================
//! Perform any necessary initialization.
//! @param aSensorModePtr The sensor mode used by this error model.
bool wsf::SensorErrorModelBase::Initialize(WsfSensorMode* aSensorModePtr)
{
   mSensorModePtr = aSensorModePtr;
   return true;
}

// =================================================================================================
// override
bool wsf::SensorErrorModelBase::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "debug")
   {
      mDebugEnabled = true;
   }
   else
   {
      myCommand = WsfObject::ProcessInput(aInput);
   }
   return myCommand;
}
