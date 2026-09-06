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

#include "Function.hpp"

#include "WsfScenario.hpp"
#include "WsfSensorPlot.hpp"
#include "WsfSimulation.hpp"
#include "WsfSimulationObserver.hpp"

Function::Function(const WsfScenario& aScenario, const std::string& aScriptClassName)
   : mScenario(aScenario)
   , mCommand("undefined")
   , mScriptContext(*aScenario.GetScriptContext(), aScriptClassName)
{
   mIsPlatformAvailabilitySpecified = false;
   if (!IsSensorPlotMode())
   {
      mPlatformAvailability.SetDefaultAvailability(0.0);
   }
}

bool Function::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;
   if (mPlatformAvailability.ProcessInput(aInput))
   {
      mIsPlatformAvailabilitySpecified = true;
   }
   else if (aInput.GetCommand() == "name")
   {
      aInput.ReadValue(mPlotName);
   }
   else if (mScriptContext.ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

bool Function::Execute(WsfSimulation& aSimulation)
{
   mScriptContext.Initialize(0.0, aSimulation.GetScriptContext(), this);

   mFunctionCallbacks += WsfObserver::SimulationComplete(&aSimulation).Connect(&Function::CallOnComplete, this);
   mScriptContext.ExecuteScript(0.0, "on_execute");
   return true;
}

void Function::CallOnComplete(double)
{
   mScriptContext.ExecuteScript(0.0, "on_complete");
}

bool Function::IsSensorPlotMode()
{
   return WsfSensorPlotExtension::IsSensorPlotMode();
}
