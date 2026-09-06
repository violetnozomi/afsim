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

// mission is the standard core application that simply
// reads text files containing wsf commands and executes the
// simulation, advancing through time, moving platforms, making decisions and
// performing interactions between objects.
//
// mission has several modes of execution:
//
// * event-stepped or frame-stepped.
// * non-realtime or realtime.
// * purely constructive including multiple Monte-Carlo iterations.
// * as part of Distributed Interactive Simulation (DIS) exercise
//   (see dis_interface).
//
// mission has two major forms of output:
//
// * An event log controlled by the input command event_output. This is a
//   human-readable text file that can be visualized with the program timeview
//   or processed through any variety of scripting programs such as Perl,
//   Python, etc.
// * A 'replay file' that can be visualized with VESPA. The replay file is
//   produced by specifying the record command in the dis_interface block.
//   Note that ability to produce a 'replay file' is mutually exclusive with
//   participation in a DIS exercise.
//
// Command Line:
//
// mission [ -es | -rt | -fs ] file1 [ file2 ... filen ]
//
// Where:
//
// -es (Default)
// Run the event-stepped simulation executive
// Note: By default the event-stepped simulation executive will run in non-
//       realtime mode (i.e.: It will dispatch events as fast as possible).
//       To run event-stepped AND participate in a DIS exercise, be sure to
//       include the command realtime in the input.
// -rt
// Run the frame-stepped simulation executive in realtime mode (frame advance
// will occur in step with the wall clock).
//
// -fs
// Run the frame-stepped simulation executive in non-realtime mode
// {frame advance will occur when all work for a frame is completed).
// Note: This is equivalent to specifying -rt and including the command
//       non-realtime in the input.
//
// file1 [ file2 ... filen ]
// One or more files that contain commands documented in the
// WSF Reference Guide.

#include <iostream>
#include <memory>
#include <sstream>

#include "UtException.hpp"
#include "UtLog.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfSimulationInput.hpp"
#include "WsfStandardApplication.hpp"

// Includes all of the optional projects which can be compiled with WSF
#include "ProfilingCommon.hpp"
#include "ProfilingRegion.hpp"
#include "ProfilingSystem.hpp"
#include "wsf_extensions.hpp"
#include "wsf_version_defines.hpp"

int main(int argc, char* argv[])
try
{
   ut::SetApplicationErrorHandling(&ut::TerminateHandler);

   ut::SetupApplicationLog("mission", WSF_VERSION, "mission-exception.log");

   // 创建仿真应用
   WsfStandardApplication app("mission", argc, argv);

   // Load built-in extensions (defined in wsf_extensions.hpp)
   // 加载内置扩展
   RegisterBuiltinExtensions(app);

   // Load optional extensions (defined in wsf_extensions.hpp)
   // 加载自定义扩展
   RegisterOptionalExtensions(app);

   // Register the XIO simulation interface.
   // 加载内部XIO通信接口
   WSF_REGISTER_EXTENSION(app, xio_interface);

   // Process command-line arguments
   // 仿真应用的选项
   WsfStandardApplication::Options options;

   try
   {
	  // 命令行传入推演文件，解析命令行
      app.ProcessCommandLine(options);
   }
   catch (WsfApplication::Exception& e)
   {
      auto out = ut::log::fatal() << "Could not process command line arguments.";
      out.AddNote() << e.what();
      return e.GetReturnCode();
   }
   // We can't start profiling until we have processed the command-line options.
   // The lambda ensures that we don't create or destroy a ProfilingRegion at main() scope.
   const int outerErrorCode = [&]() {
      std::unique_ptr<profiling::ProfilingSystem> profilingSystem;
      try
      {
         profilingSystem.reset(new profiling::ProfilingSystem(profiling::GlobalProfilingSystemArguments()));
      }
      catch (profiling::Exception& e)
      {
         ut::log::fatal() << e.what();
         return e.GetReturnCode();
      }

      profiling::ProfilingRegion mainRegion("Mission main (after command-line options read)");
      // Create a scenario
	  // 创建一个场景
      WsfScenario scenario(app);

      try
      {
         profiling::ProfilingRegion inputFilesRegion("ProcessInputFiles");
         // Read WSF input files into the scenario
         app.ProcessInputFiles(scenario, options.mInputFiles);
      }
      catch (WsfApplication::Exception& e)
      {
         auto out = ut::log::fatal() << "Could not process input files.";
         out.AddNote() << e.what();
         return e.GetReturnCode();
      }

      app.ProcessCommandLineCommands(scenario, options);

      int errorCode = 0;
      if (options.mRunMode == WsfStandardApplication::cRUN_SCENARIO)
      {
         // Loop while Monte-Carlo runs remain to be executed or an external controller has not told us to exit.
         for (unsigned int runNumber = scenario.GetInitalRunNumber(); runNumber <= scenario.GetFinalRunNumber();)
         {
            const auto runLabel =
               std::string(": Run ") + std::to_string(runNumber) + " of " + std::to_string(scenario.GetFinalRunNumber());
            std::unique_ptr<WsfSimulation> simPtr = [&]()
            {
               profiling::ProfilingRegion createRegion((std::string("CreateSimulation") + runLabel).c_str());
               return app.CreateSimulation(scenario, options, runNumber);
            }();

            {
               profiling::ProfilingRegion initRegion((std::string("InitializeSimulation") + runLabel).c_str());
               if (!app.InitializeSimulation(simPtr.get()))
               {
                  errorCode = 1;
                  break;
               }
            }

			// 没有输入文件，退出
            // If given no inputs, quit now
            if (options.mInputFiles.empty())
            {
               break;
            }

            WsfStandardApplication::SimulationResult result;
            {
               profiling::ProfilingRegion runRegion((std::string("RunEventLoop") + runLabel).c_str());
               result = app.RunEventLoop(simPtr.get(), options);
            }

            // On a reset, re-create the same simulation run.  Otherwise, do the next run
            if (!result.mResetRequested)
            {
               runNumber += scenario.GetRunNumberIncrement();
            }
         }
      }
      return errorCode;
   }();
   return outerErrorCode;
}
catch (...)
{
   ut::ReportException("Mission", std::current_exception());
   return 1;
}
