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

#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include "sensor_plot_lib_export.h"

#include <string>

#include "UtCallbackHolder.hpp"
#include "script/WsfScriptContext.hpp"

class UtInput;
#include "WsfPlatformAvailability.hpp"
class WsfScenario;
class WsfSimulation;

//! An abstract base class for sensor_plot function.
class SENSOR_PLOT_LIB_EXPORT Function
{
public:
   Function(const Function&) = default;
   Function(const WsfScenario& aScenario, const std::string& aScriptClassName = "");
   virtual ~Function() = default;

   virtual bool Execute(WsfSimulation& aSimulation) = 0;
   virtual bool ProcessInput(UtInput& aInput)       = 0;

   const std::string& GetCommand() const { return mCommand; }
   void               SetCommand(const std::string& aCommand) { mCommand = aCommand; }

   const WsfScenario& GetScenario() const { return mScenario; }

   const std::string&             GetPlotName() const { return mPlotName; }
   const WsfPlatformAvailability& GetPlatformAvailability() const { return mPlatformAvailability; }
   //! Has the user provided a platform_availability block for the plot?
   bool IsPlatformAvailabilitySpecified() const { return mIsPlatformAvailabilitySpecified; }

   static bool IsSensorPlotMode();

protected:
   WsfPlatformAvailability mPlatformAvailability;

private:
   void CallOnComplete(double);

   const WsfScenario& mScenario;
   bool               mIsPlatformAvailabilitySpecified{false};
   std::string        mPlotName;
   std::string        mCommand;
   WsfScriptContext   mScriptContext;
   UtCallbackHolder   mFunctionCallbacks;
};

#endif
