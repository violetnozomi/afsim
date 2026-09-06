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

// This program can be used to generate detection plots and antenna pattern
// plots.  The results can be plotted with GNUPLOT or sigview.

#include <cstring>
#include <iostream>

#include "UtMemory.hpp"
#include "UtProcessorTime.hpp"
#include "UtWallClock.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfStandardApplication.hpp"

// Includes all of the optional projects which can be compiled with WSF
#include "StubInterface.hpp"
#include "WsfSensorPlot.hpp"
#include "sensor_plot_version_defines.hpp"
#include "wsf_extensions.hpp"


namespace
{
void ShowHelp()
{
   std::cout << "Usage: sensor_plot file1 [file2 ... filen]\n" << std::endl;
}

class SensorPlotExtension : public WsfApplicationExtension
{
public:
   // =================================================================================================
   SensorPlotExtension()
      : WsfApplicationExtension()
   {
   }

   // =================================================================================================
   void ScenarioCreated(WsfScenario& aScenario) override
   {
      aScenario.RegisterExtension("stub_interface", ut::make_unique<StubInterface>(aScenario));
   }

   // virtual
   int ProcessCommandLine(WsfStandardApplication::Options& aOptions, int aArgc, char* aArgv[]) override
   {
      if (strcmp(aArgv[0], "--") == 0)
      {
         return 1;
      }
      else if ((strcmp(aArgv[0], "-?") == 0) || (strcmp(aArgv[0], "-h") == 0))
      {
         ShowHelp();
         return 1;
      }
      return 0;
   }
};
} // namespace


int main(int argc, char* argv[])
try
{
   ut::SetApplicationErrorHandling(&ut::TerminateHandler);
   ut::SetupApplicationLog("Sensor Plot", SENSOR_PLOT_VERSION, "sensor-plot-exception.log");

   WsfSensorPlotExtension::EnableSensorPlotMode();

   WsfStandardApplication app("sensor_plot", argc, argv);

   // Register our application extension...
   app.RegisterExtension("sensor_plot_ext", ut::make_unique<SensorPlotExtension>());
   app.ExtensionDepends("sensor_plot_ext", "wsf_mil", true);

   // Load built-in extensions (defined in wsf_extensions.hpp)
   RegisterBuiltinExtensions(app);

   // Load optional extensions (defined in wsf_extensions.hpp)
   RegisterOptionalExtensions(app);

   // Register the XIO simulation interface.
   WSF_REGISTER_EXTENSION(app, xio_interface);

   WsfScenario scenario(app);

   // Process command-line arguments
   WsfStandardApplication::Options options;

   try
   {
      app.ProcessCommandLine(options);
      // Read WSF input files into the scenario
      app.ProcessInputFiles(scenario, options.mInputFiles);
   }
   catch (WsfApplication::Exception& e)
   {
      auto out = ut::log::fatal() << "Could not process command line arguments.";
      out.AddNote() << e.what();
      return e.GetReturnCode();
   }

   app.ProcessCommandLineCommands(scenario, options);

   bool ok = true;
   if (options.mRunMode == WsfStandardApplication::cRUN_SCENARIO)
   {
      // Execute the plots...
      WsfSensorPlotExtension* sensorPlotExt = WsfSensorPlotExtension::Find(scenario);
      ok                                    = sensorPlotExt->ExecutePlots();
   }
   return ok ? 0 : 1;
}
catch (...)
{
   ut::ReportException("Sensor Plot", std::current_exception());
   return 1;
}
