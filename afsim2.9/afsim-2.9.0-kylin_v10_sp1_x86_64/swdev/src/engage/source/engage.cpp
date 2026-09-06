// ****************************************************************************
// CUI//REL TO USA ONLY
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

//! ENGAGE is a WSF application used to evaluate the effectiveness of surface weapons (and conversely,
//! the vulnerability of the target).

#include <iostream>

#include "UtMemory.hpp"
#include "UtProcessorTime.hpp"
#include "UtWallClock.hpp"
#include "WsfScenario.hpp"
#include "WsfStandardApplication.hpp"
#include "WsfVersion.hpp"

// Includes all of the optional projects which can be compiled with WSF
#define WSF_EXCLUDE_EXTENSION_sensor_plot_lib
#include "ApplicationExtension.hpp"
#include "InputConfig.hpp"
#include "TaskManager.hpp"
#include "engage_version_defines.hpp"
#include "wsf_extensions.hpp"

int main(int argc, char* argv[])
try
{
   ut::SetApplicationErrorHandling(&ut::TerminateHandler);
   ut::SetupApplicationLog("Engage", ENGAGE_VERSION, "engage-exception.log");

   WsfStandardApplication app("engage", argc, argv);

   // Register our application extension...
   // NOTE: We have to be created AFTER the DIS extension because we have to push data into the
   //       DIS extensions simulation extension prior to it's being initialized.
   app.RegisterExtension("engage", ut::make_unique<engage::ApplicationExtension>());
   app.ExtensionDepends("engage", "dis_interface", true);
   app.ExtensionDepends("engage", "wsf_mil", true);

   // Load built-in extensions (defined in wsf_extensions.hpp)
   RegisterBuiltinExtensions(app);

   // Load optional extensions (defined in wsf_extensions.hpp)
   RegisterOptionalExtensions(app);

   // Create a scenario
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

   int errorCode = 0;
   if (options.mRunMode == WsfStandardApplication::cRUN_SCENARIO)
   {
      // Execute the tasks...

      ut::log::newline();
      ut::log::info() << "Execution starting.";
      UtWallClock         wallClock;
      UtProcessorTime     procTime;
      engage::TaskManager taskManager(scenario, engage::InputConfig::FindExtension(scenario));
      taskManager.Execute();
      double wallTime = wallClock.GetClock();
      double cpuTime  = procTime.GetTime();
      ut::log::newline();
      auto out = ut::log::info() << "Execution complete";
      out.AddNote() << "Elapsed Wall Clock Time: " << wallTime;
      out.AddNote() << "Elapsed Processor Time : " << cpuTime;
   }
   return errorCode;
}
catch (...)
{
   ut::ReportException("Engage", std::current_exception());
   return 1;
}
