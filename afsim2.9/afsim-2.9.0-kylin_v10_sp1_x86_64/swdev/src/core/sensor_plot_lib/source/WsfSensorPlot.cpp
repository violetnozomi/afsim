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

#include "WsfSensorPlot.hpp"

#include <cctype>
#include <cstring>
#include <memory>

#include "AntennaPlotFunction.hpp"
#include "ClutterTableFunction.hpp"
#include "FlightPathAnalysisFunction.hpp"
#include "HorizontalCoverageFunction.hpp"
#include "HorizontalMapFunction.hpp"
#include "RadarEnvelopeFunction.hpp"
#include "RadarLookupTableFunction.hpp"
#include "SphericalMapFunction.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "UtProcessorTime.hpp"
#include "UtStringUtil.hpp"
#include "UtWallClock.hpp"
#include "VerticalCoverageFunction.hpp"
#include "VerticalMapFunction.hpp"
#include "WsfApplication.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"

namespace
{
// If set to true, the sensor_plot library should behave as the standard sensor_plot application
// This means:
// 1. All plots are run at startup
// 2. platform availability defaults to 1
bool gSensorPlotMode = false;
} // namespace

bool WsfSensorPlotExtension::IsSensorPlotMode()
{
   return gSensorPlotMode;
}

class WsfSensorPlotApplicationExtension : public WsfApplicationExtension
{
public:
   WsfSensorPlotApplicationExtension()
      : mPlotAll(false)
   {
   }
   bool DoAllPlots() const { return mPlotAll || gSensorPlotMode; }
   bool ShouldRunPlots() const { return DoAllPlots() || !mPlotsToRun.empty() || !mPlotsToRunByNumber.empty(); }

   bool ShouldRunPlot(int aPlotIndex, Function* aPlotPtr) const
   {
      return DoAllPlots() ||
             std::find(mPlotsToRun.begin(), mPlotsToRun.end(), aPlotPtr->GetPlotName()) != mPlotsToRun.end() ||
             std::find(mPlotsToRunByNumber.begin(), mPlotsToRunByNumber.end(), aPlotIndex + 1) !=
                mPlotsToRunByNumber.end();
   }

private:
   void ScenarioCreated(WsfScenario& aScenario) override
   {
      aScenario.RegisterExtension(GetExtensionName(), ut::make_unique<WsfSensorPlotExtension>(this));
   }
   int ProcessCommandLine(WsfStandardApplication::Options& aOptions, int aArgc, char* aArgv[]) override
   {
      if (!gSensorPlotMode)
      {
         if (0 == strcmp(aArgv[0], "-plot"))
         {
            if (aArgc > 1)
            {
               bool        isNumber = true;
               std::string plotName = aArgv[1];
               for (char ch : plotName)
               {
                  if (!isdigit(ch))
                  {
                     isNumber = false;
                     break;
                  }
               }
               if (isNumber)
               {
                  mPlotsToRunByNumber.push_back(UtStringUtil::ToInt(plotName));
               }
               else
               {
                  mPlotsToRun.push_back(plotName);
               }
               return 2;
            }
         }
         else if (0 == strcmp(aArgv[0], "-plot-all"))
         {
            mPlotAll = true;
            return 1;
         }
      }
      return 0;
   }

   void PrintCommandLineArguments() const override
   {
      // run all plots in sensorplot mode
      if (!gSensorPlotMode)
      {
         std::cout << "\n-plot <name>        Plot any sensor_plot plots with the specified name or number"
                      "\n-plot-all           Plot all sensor_plot plots";
      }
   }

public:
   // List of plot names to execute at startup
   std::vector<std::string> mPlotsToRun;
   // List of plot numbers to execute at startup (plots are numbered 1..N)
   std::vector<int> mPlotsToRunByNumber;

private:
   // Plot all plots at startup?
   bool mPlotAll;
};


void WsfSensorPlotExtension::EnableSensorPlotMode()
{
   gSensorPlotMode = true;
}

#ifdef _WIN32
__declspec(dllexport)
#endif
   void Register_sensor_plot_lib(WsfApplication& aApplication)
{
   aApplication.RegisterFeature("sensor_plot");
   aApplication.RegisterExtension("sensor_plot", ut::make_unique<WsfSensorPlotApplicationExtension>());
}

// =================================================================================================
WsfSensorPlotExtension* WsfSensorPlotExtension::Find(WsfScenario& aScenario)
{
   return static_cast<WsfSensorPlotExtension*>(aScenario.FindExtension("sensor_plot"));
}

// =================================================================================================
WsfSensorPlotExtension::WsfSensorPlotExtension(WsfSensorPlotApplicationExtension* aAppExtensionPtr)
{
   mAppExtensionPtr = aAppExtensionPtr;
}

// =================================================================================================
// protected

void WsfSensorPlotExtension::AddedToScenario()
{
   // Register built-in functions with the factory
   RegisterFunction<AntennaPlotFunction>("antenna_plot");
   RegisterFunction<ClutterTableFunction>("clutter_table");
   RegisterFunction<FlightPathAnalysisFunction>("flight_path_analysis");
   RegisterFunction<HorizontalMapFunction>("horizontal_map");
   RegisterFunction<HorizontalMapFunction>("multi_dimensional_horizontal_map");
   RegisterFunction<HorizontalCoverageFunction>("horizontal_coverage");
   RegisterFunction<RadarLookupTableFunction>("radar_lookup_table");
   RegisterFunction<RadarEnvelopeFunction>("radar_envelope");
   RegisterFunction<SphericalMapFunction>("spherical_map");
   RegisterFunction<VerticalCoverageFunction>("vertical_coverage");
   RegisterFunction<VerticalMapFunction>("vertical_map");
}

bool WsfSensorPlotExtension::ProcessInput(UtInput& aInput)
{
   std::string command(aInput.GetCommand());

   auto iter      = mFunctionFactory.find(command);
   bool myCommand = (iter != mFunctionFactory.end());
   if (myCommand)
   {
      // If this was one of our commands, process the input block for the command.
      auto functionPtr = (iter->second)(GetScenario());

      // Set the associated command name (for diagnostics).
      functionPtr->SetCommand(command);

      // If this Function* is actually a MapPlotFunction* then inform it about registered
      // extensions before it attempts to process its input block.
      MapPlotFunction* mapPlotFunc = dynamic_cast<MapPlotFunction*>(functionPtr.get());
      if (mapPlotFunc != nullptr)
      {
         for (auto& mpv : mMapPlotVariableMap)
         {
            mapPlotFunc->RegisterMapPlotVariable(mpv.first, mpv.second);
         }
      }

      UtInputBlock inputBlock(aInput);
      inputBlock.ProcessInput(functionPtr.get());

      mFunctionList.push_back(std::move(functionPtr));
   }

   return myCommand;
}

// =================================================================================================
bool WsfSensorPlotExtension::Complete2()
{
   if (!IsSensorPlotMode())
   {
      bool ok = ExecutePlots();
      if (!ok)
      {
         exit(1);
      }
      if (mAppExtensionPtr->ShouldRunPlots())
      {
         exit(0);
      }
   }
   return true;
}

// =================================================================================================
bool WsfSensorPlotExtension::ExecutePlots()
{
   bool ok = true;
   if (mAppExtensionPtr->ShouldRunPlots())
   {
      for (size_t i = 0; i < mFunctionList.size() && ok; ++i)
      {
         if (mAppExtensionPtr->ShouldRunPlot((int)i, mFunctionList[i].get()))
         {
            if (!RunFunction(mFunctionList[i].get()))
            {
               ok = false;
            }
         }
      }
   }

   return ok;
}

//! Override the standard simulation's platform availability determination
//! to use the one specified in the plot
class Simulation : public WsfSimulation
{
public:
   Simulation(WsfScenario& aScenario, Function* aFunctionPtr)
      : WsfSimulation(aScenario, 0)
      , mFunctionPtr(aFunctionPtr)
   {
   }

   bool PlatformIsAvailable(WsfPlatform* aPlatformPtr) override
   {
      return mFunctionPtr->GetPlatformAvailability().PlatformIsAvailable(aPlatformPtr, *this);
   }

   Function* mFunctionPtr;
};

bool WsfSensorPlotExtension::RunFunction(Function* aFunctionPtr)
{
   Simulation sim(GetScenario(), aFunctionPtr);

   ut::log::info() << "Processing function: " << aFunctionPtr->GetCommand();
   try
   {
      ut::log::info() << "Initializing simulation.";
      sim.Initialize();
   }
   catch (std::exception& e)
   {
      auto out = ut::log::error() << "Initialization of simulation failed due to an unhandled exception.";
      out.AddNote() << "Type: " << ut::TypeNameOf(e);
      out.AddNote() << "What: " << e.what();
      return false;
   }

   sim.Start();
   UtWallClock     wallClock;
   UtProcessorTime procClock;
   bool            ok = aFunctionPtr->Execute(sim);
   if (ok)
   {
      ut::log::info() << "Elapsed wall time: " << wallClock.GetClock();
      ut::log::info() << "Elapsed CPU  time: " << procClock.GetTime();
   }
   else
   {
      auto out = ut::log::error() << "Failed processing of command.";
      out.AddNote() << "Command: " << aFunctionPtr->GetCommand();
   }
   sim.Complete(sim.GetSimTime());
   return ok;
}

void WsfSensorPlotExtension::RegisterMapPlotVariable(const std::string&               aVarName,
                                                     std::shared_ptr<MapPlotVariable> aVariablePtr)
{
   auto valid = mMapPlotVariableMap.emplace(aVarName, aVariablePtr);
   if (!valid.second)
   {
      throw UtException("ERROR: Attempting to register multiple '" + aVarName + "' variable extensions.");
   }
}
