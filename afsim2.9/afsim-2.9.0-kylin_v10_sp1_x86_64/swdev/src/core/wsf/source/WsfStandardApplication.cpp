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

#include "WsfStandardApplication.hpp"

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "TimedRegion.hpp"
#include "UtInput.hpp"
#include "UtInputPreprocessor.hpp"
#include "UtMemory.hpp"
#include "UtPath.hpp"
#include "UtStringUtil.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfEventStepSimulation.hpp"
#include "WsfExtensionList.hpp"
#include "WsfFrameStepSimulation.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfSystemLog.hpp"
#include "ext/WsfExtInterface.hpp"

// ================================================================================================
//! Constructor
//! @param aApplicationName       The name of the application.
//! @param aArgc                  The number of arguments. This is typically the 'argc' argument from 'main()'.
//! @param aArgv                  The argument list. This is typically the 'argv' argument from 'main()'.
//! @param aPluginPaths           Optional list of paths (relative to executable) to load plugins from.
WsfStandardApplication::WsfStandardApplication(const std::string& aApplicationName,
                                               int                aArgc,
                                               char*              aArgv[],
                                               const PluginPaths& aPluginPaths)
   : WsfApplication(aApplicationName, aArgc, aArgv, aPluginPaths)
   , mLogServer{ut::log::Message::PresetDeveloper()}
{
}

// ================================================================================================
//! Displays the application command-line usage
void WsfStandardApplication::ShowUsage()
{
   // This one is staying as cout for the time being
   std::cout << GetUsageString();

   // Let extensions print out command line usage here
   const std::vector<std::string>& extensionNames = GetExtensions().GetExtensionOrder();
   for (const auto& extensionName : extensionNames)
   {
      WsfApplicationExtension* extensionPtr = FindExtension(extensionName);
      if (extensionPtr != nullptr)
      {
         extensionPtr->PrintCommandLineArguments();
      }
   }
}

// ================================================================================================
//! Process the command line inputs
//! Returns 'true' if the application should continue
void WsfStandardApplication::ProcessCommandLine(Options& aOptions)
{
   int    argc;
   char** argv;
   GetCommandLineArguments(argc, argv);
   ProcessCommandLine(aOptions, argc, argv);
}

// ================================================================================================
//! Process the command line arguments.
//! Returns 'true' if the application should continue
void WsfStandardApplication::ProcessCommandLine(Options& aOptions, int argc, char* argv[])
{
   int argIndex = 1;
   int argCount = argc;
   while (argIndex < argCount)
   {
      std::string argValue(argv[argIndex]);
      if (argValue == "-rt")
      {
         aOptions.mSimType = cREAL_TIME;
      }
      else if (argValue == "-fs")
      {
         aOptions.mSimType = cFRAME_STEPPED;
      }
      else if (argValue == "-es")
      {
         aOptions.mSimType = cEVENT_STEPPED;
      }
      else if (argValue == "-fio")
      {
         aOptions.mFlushIO = true;
      }
      else if (argValue == "-sm")
      {
         aOptions.mMessageInterval         = 1.0e99;
         aOptions.mRealtimeMessageInterval = 1.0e99;
      }
      // Log server members from command line
      else if (argValue == "-log-server-host")
      {
         mShouldConnectToLogServer = true;
         argIndex++;
         // out of bounds indexing on standard [] is undefined behavior.
         if (argIndex >= argc)
         {
            ut::log::warning() << "Failed to process -log-server-host, attempting connection to localhost";
         }
         else
         {
            mLogServerHost = argv[argIndex];
         }
      }
      else if (argValue == "-log-server-port")
      {
         mShouldConnectToLogServer = true;
         try
         {
            argIndex++;
            // out of bounds indexing on standard [] is undefined behavior.
            if (argIndex >= argc)
            {
               throw std::exception{};
            }
            mLogServerPort = std::stoi(argv[argIndex]);
         }
         catch (std::exception&)
         {
            ut::log::warning() << "Failed to process -log-server-port, attempting connection to 18888.";
         }
      }
      else if (argValue == "-mi")
      {
         double interval = atof(argv[++argIndex]);
         if (interval > 0.0)
         {
            aOptions.mMessageInterval = interval;

            // Print the message at the given
            // interval, which is every 10th interval for real-time,
            // so adjust by a factor of 10.
            aOptions.mRealtimeMessageInterval = interval / 10.0;
         }
      }
      else if (argValue == "-list-variables")
      {
         aOptions.mRunMode = cLIST_VARIABLES;
      }
      else if ((argValue == "-?") || (argValue == "-h") || (argValue == "-help"))
      {
         aOptions.mRunMode = cSHOW_USAGE;
      }
      else
      {
         bool                            processedArg   = false;
         const std::vector<std::string>& extensionNames = GetExtensions().GetExtensionOrder();
         for (const auto& extensionName : extensionNames)
         {
            WsfApplicationExtension* extensionPtr = FindExtension(extensionName);
            if (extensionPtr != nullptr)
            {
               int argsUsed = extensionPtr->ProcessCommandLine(aOptions, argc - argIndex, argv + argIndex);
               assert(argsUsed >= 0);
               if (argsUsed)
               {
                  argIndex += argsUsed - 1;
                  processedArg = true;
                  break;
               }
            }
         }

         if (!processedArg)
         {
            if (argValue == "--")
            {
               // End of arguments
               ++argIndex;
               break;
            }
            else if (argValue[0] != '-')
            {
               // Start of file names
               break;
            }
            else
            {
               ShowUsage();
               throw InvalidCommandLineArgument(argValue);
            }
         }
      }
      ++argIndex;
   }

   for (; argIndex < argCount; ++argIndex)
   {
      aOptions.mInputFiles.emplace_back(argv[argIndex]);
   }
}

// ================================================================================================
//! Process any commands included in the command line.
//! @param aScenario The scenario that is being loaded.
//! @param aOptions  The options from the command line.
void WsfStandardApplication::ProcessCommandLineCommands(WsfScenario& aScenario, const Options& aOptions)
{
   if (aOptions.mRunMode == cLIST_VARIABLES)
   {
      // List all of the preprocessor variables.
      auto                 out             = ut::log::info() << "Input Variables:";
      UtInput&             input           = aScenario.GetInput();
      UtInputPreprocessor* preprocessorPtr = input.GetPreprocessor();
      if (preprocessorPtr != nullptr)
      {
         const auto& variables = preprocessorPtr->GetVariables();
         for (const auto& var : variables)
         {
            std::string def;
            for (char c : var.second)
            {
               if (c == '\n')
               {
                  def += '\\';
               }
               def += c;
            }
            out.AddNote() << "$define " << var.first << " " << def;
         }
         // List unset variables
         const auto& varRefs = preprocessorPtr->GetVariableReferences();
         for (const auto& ref : varRefs)
         {
            if (variables.find(ref.first) == variables.end())
            {
               out.AddNote() << "$undef " << ref.first << '\n';
            }
         }
      }
   }
   else if (aOptions.mRunMode == cSHOW_USAGE)
   {
      ShowUsage();
   }
   else if (aOptions.mRunMode == cRUN_SCENARIO)
   {
      // Set up log server client
      ConnectToLogServer();
   }


   const auto& extensionNames = GetExtensions().GetExtensionOrder();
   for (const auto& extensionName : extensionNames)
   {
      WsfApplicationExtension* extensionPtr = FindExtension(extensionName);
      if (extensionPtr != nullptr)
      {
         extensionPtr->ProcessCommandLineCommands();
      }
   }
}

// ================================================================================================
//! Load the specified scenario with the input from a list of files.
//! @param aScenario The scenario to be loaded from the supplied list of input files.
//! @param aFileList The list of files to be loaded into the scenario.
//! @throws exception if error is encountered.
void WsfStandardApplication::ProcessInputFiles(WsfScenario& aScenario, const std::vector<std::string>& aFileList)
{
   try
   {
      profiling::TimedRegion regionLoadingSimInput("Loading simulation input");
      for (const auto& file : aFileList)
      {
         aScenario.LoadFromFile(file);
      }
      aScenario.CompleteLoad();
   }
   catch (WsfApplication::Exception&)
   {
      throw;
   }
   catch (std::exception& e)
   {
      // throw an exception indicating the correct return code.  Check if input error was expected
      std::ostringstream ss;
      ss << e.what() << "\n";
      ss << "Reading of simulation input failed";
      throw InputProcessingFailure(ss.str(), !aScenario.HadUnexpectedError());
   }
}

// ================================================================================================
//! Creates a simulation given an Options object.
//! @param aScenario   The scenario to be executed.
//! @param aOptions    A reference to the Options object.
//! @param aRunNumber  The Monte-Carlo iteration number.
//! @returns A pointer to the created simulation.
std::unique_ptr<WsfSimulation> WsfStandardApplication::CreateSimulation(WsfScenario&   aScenario,
                                                                        const Options& aOptions,
                                                                        unsigned int   aRunNumber)
{
   return CreateSimulation(aScenario, aOptions.mSimType, aRunNumber);
}

// ================================================================================================
//! Create the specified simulation type.
//! @param aScenario       The scenario to be executed.
//! @param aSimulationType The type of simulation object to be created.
//! @param aRunNumber      The Monte-Carlo iteration number.
//! @returns A pointer to the created simulation.
std::unique_ptr<WsfSimulation> WsfStandardApplication::CreateSimulation(WsfScenario& aScenario,
                                                                        SimType      aSimulationType,
                                                                        unsigned int aRunNumber)
{
   std::unique_ptr<WsfSimulation> simPtr = nullptr;
   try
   {
	   // 三种类型的仿真应用，实时、帧推进、事件驱动
      if (aSimulationType == cREAL_TIME)
      {
         simPtr = ut::make_unique<WsfFrameStepSimulation>(aScenario, aRunNumber);
         simPtr->SetRealtime(0, true);
      }
      else if (aSimulationType == cFRAME_STEPPED)
      {
         simPtr = ut::make_unique<WsfFrameStepSimulation>(aScenario, aRunNumber);
         simPtr->SetRealtime(0, false);
      }
      else
      {
         simPtr = ut::make_unique<WsfEventStepSimulation>(aScenario, aRunNumber);
      }
   }
   catch (WsfException& e)
   {
      { // RAII block
         auto out = ut::log::error() << "Creation of simulation failed.";
         out.AddNote() << e.what();
      }
      simPtr.reset();
   }
   return simPtr;
}

// ================================================================================================
//! Initialize a simulation.
//! @param aSimPtr Pointer to the simulation.
//! @returns true if successful, false if a failure occurred.
bool WsfStandardApplication::InitializeSimulation(WsfSimulation* aSimPtr)
{
   // If not pending initialize, skip
   if (aSimPtr->GetState() != WsfSimulation::cPENDING_INITIALIZE)
   {
      return true;
   }

   auto& classificationString = aSimPtr->GetScenario().GetClassificationString();
   if (!classificationString.empty())
   {
      ut::log::info() << "Classification: " << classificationString;
   }

   // Initialize all of the objects in the simulation.
   try
   {
      profiling::TimedRegion regionInitializingSim("Initializing simulation",
                                                   profiling::TimedRegion::Mode::REGION,
                                                   [aSimPtr](ut::log::MessageStream& stream) {
                                                      // Output run numbers only if there are multiple runs
                                                      if (aSimPtr->GetScenario().GetFinalRunNumber() > 1)
                                                      {
                                                         stream.AddNote() << "Run: " << aSimPtr->GetRunNumber();
                                                      }
                                                   });
      aSimPtr->Initialize();
   }
   catch (std::exception& e)
   {
      auto out = ut::log::error() << "Initialization of simulation failed due to unhandled exception.";
      out.AddNote() << "Type: " << ut::TypeNameOf(e);
      out.AddNote() << "What: " << e.what();
      return false;
   }

   return true;
}

// ================================================================================================
//! Execute the simulation event loop until it is complete or is reset.
//! @param aSimPtr  The pointer to the simulation to execute.
//! @param aOptions The options derived from the command line (or explicitly by code).
//! @returns A SimulationRresult object that indicates the result of the simulation.
WsfStandardApplication::SimulationResult WsfStandardApplication::RunEventLoop(WsfSimulation* aSimPtr, Options aOptions)
{
   // Update deferred connection time and message interval
   UpdateOptionsP(aOptions, aSimPtr);
   WsfStandardApplication::SimulationResult result;

   // Run the simulation
   double messageInterval         = aOptions.mMessageInterval;
   double previousMessageInterval = messageInterval;
   int    messageSkip             = 0;
   double lastMessageTime         = 0.0;
   bool   deferred                = aOptions.mDeferredConnectionTime > 0.0;

   profiling::TimedRegion regionSimulation("RunEventLoop - before \"Starting simulation\"",
                                           profiling::TimedRegion::Mode::SUBREGION);

   if (aSimPtr->GetState() == WsfSimulation::cPENDING_START)
   {
      regionSimulation.StartSubregion("Starting simulation");
      {
         std::ostringstream oss;
         oss << "start " << aSimPtr->GetRunNumber();
         GetSystemLog().WriteLogEntry(oss.str());
      }
      aSimPtr->Start();
   }

   double simTime = 0.0;

   // Loop until the end of the Monte-Carlo run is complete, or an external
   // control has indicated that we should quit or reset.

   while (aSimPtr->IsActive())
   {
      mLogServer.ProcessMessages();
      previousMessageInterval = messageInterval;
      // Determine the messageInterval
      if (aSimPtr->IsRealTime())
      {
         // If RealTime and not Deferred
         if (!deferred)
         {
            messageInterval = aOptions.mRealtimeMessageInterval;
         }
         // Check to see if Deferred is complete
         else if (simTime >= aOptions.mDeferredConnectionTime)
         {
            messageInterval = aOptions.mRealtimeMessageInterval;
            deferred        = false;
         }
         // use standard interval while in Deferred connection state
         else
         {
            messageInterval = aOptions.mMessageInterval;
         }
      }
      else // Use standard interval if not RealTime
      {
         messageInterval = aOptions.mMessageInterval;
      }
      // If the interval changed
      if (messageInterval != previousMessageInterval)
      {
         // Set the last message time to the current time
         // If we don't update the lastMessageTime, we can get the following to happen:
         //  messageInterval was 1000 and changes to 10, with lastMessageTime = 1000 and simTim = 1500
         //  This will cause a message to appear 50 times (1500-1000) / 10, instead of just once
         lastMessageTime = simTime;
         // Print a message when the messageInterval changes
         ut::log::info() << "T = " << simTime;
      }

      aSimPtr->WaitForAdvanceTime();
      simTime = aSimPtr->AdvanceTime();
      // The following block periodically prints a T=(current sim time) message.
      if ((simTime - lastMessageTime) >= messageInterval)
      {
         auto out = ut::log::info();
         out.GetStream().setf(std::ios::fixed | std::ios::showpoint, std::ios::floatfield | std::ios::showpoint);
         out.GetStream().precision(3);

         // For real time simulations, only print the message every 10th pass,
         // unless it is falling behind, in which case we print the message every pass.
         if (aSimPtr->IsRealTime())
         {
            double clockTime  = aSimPtr->GetRealTime();
            double timeBehind = clockTime - simTime;

            if (--messageSkip < 0)
            {
               messageSkip = 9;
            }
            if (timeBehind < 0.5 || aSimPtr->IsFlexibleRealtime())
            {
               if (messageSkip == 0)
               {
                  out << "T = " << simTime;
               }
            }
            else
            {
               // These are going to stay on a single line instead of being made notes
               out << "T = " << simTime << " FALLING BEHIND (" << timeBehind << " sec)";
            }
         }
         else
         {
            out << "T = " << simTime;
         }
         lastMessageTime += messageInterval;
      }
   }

   aSimPtr->Complete(simTime);

   std::string completionReasonStr = aSimPtr->GetCompletionReasonString();
   switch (aSimPtr->GetCompletionReason())
   {
   case WsfSimulation::cEND_TIME_REACHED:
      completionReasonStr = "complete";
      break;
   case WsfSimulation::cRESET:
      result.mResetRequested = true;
      break;
   default:
   {
   }
   }

   UtStringUtil::ToLower(completionReasonStr);
   regionSimulation.StopSubregion("Simulation " + completionReasonStr);

   {
      std::stringstream oss;
      oss.setf(std::ios::fixed | std::ios::showpoint, std::ios::floatfield | std::ios::showpoint);
      oss.precision(3);
      oss << completionReasonStr << ' ' << simTime << ' ' << regionSimulation.GetWallTime() << ' '
          << regionSimulation.GetCpuTime();
      GetSystemLog().WriteLogEntry(oss.str());
   }

   return result;
}

// This needs to be redone for inter-operation with logger.
std::string WsfStandardApplication::GetUsageString() const
{
   return std::string("\nUsage: ") + GetApplicationName() + " [-es | -fs | -rt] <input_file1> [input_file2 ...]" +
          "\n       " + GetApplicationName() +
          " -? | -h | -help"
          "\n"
          "\n-es                 Use the non-real-time event-stepped executive. (DEFAULT)"
          "\n-fs                 Use the non-real-time frame-stepped executive."
          "\n-rt                 Use the real-time executive."
          "\n-fio                Flush the output buffer when the simulation time is advanced"
          "\n-sm                 Suppress the periodic messages indicating advance of the"
          "\n                    simulation time"
          "\n-mi <interval>      Output periodic messages indicating advance of the simulation"
          "\n                    time at the given interval."
          "\n-list-variables     List preprocessor variables used in the input files and quit"
          "\n-log-server-host    Hostname or IP Address of Log Server to which output should be logged"
          "\n-log-server-port    Port of Log Server to which output should be logged"
          "\n-?, -h, -help       Display command line options and quit"
          "\n";
}

// ================================================================================================
// protected
void WsfStandardApplication::UpdateOptionsP(Options& aOptions, WsfSimulation* aSimPtr)
{
   if (aOptions.mDeferredConnectionTime == 0.0)
   {
      WsfExtInterface* extInterfacePtr = static_cast<WsfExtInterface*>(aSimPtr->FindExtension("ext_interface"));
      if (extInterfacePtr != nullptr)
      {
         aOptions.mDeferredConnectionTime = extInterfacePtr->GetDeferredConnectionTime();
      }
   }

   // Check for deferred connection time + real-time combination
   bool deferred = (aOptions.mDeferredConnectionTime != 0.0);
   if (deferred && aSimPtr->IsRealTime())
   {
      aSimPtr->SetRealtime(aSimPtr->GetSimTime(), false);
      // Use a larger message interval while running in the non-real-time mode. It will be
      // switched back to the smaller interval when the deferred connection time elapses.
      aOptions.mMessageInterval = 1000.0;
   }
}

void WsfStandardApplication::ConnectToLogServer()
{
   if (mShouldConnectToLogServer)
   {
      if (mLogServer.GetProcessor().Init(mLogServerHost, mLogServerPort))
      {
         ut::log::newline();
         auto out = ut::log::info() << "Log Client Connected to Logging Server";
         out.AddNote() << "Host: " << mLogServerHost;
         out.AddNote() << "Port: " << mLogServerPort;
         ut::log::newline();
      }
      else
      {
         ut::log::newline();
         auto out = ut::log::warning() << "Log Client Failed to Connect to Logging Server";
         out.AddNote() << "Host: " << mLogServerHost;
         out.AddNote() << "Port: " << mLogServerPort;
         ut::log::newline();
      }
   }
}


// ================================================================================================
// Nested exception classes.
// ================================================================================================

// ================================================================================================
WsfStandardApplication::InvalidCommandLineArgument::InvalidCommandLineArgument(const std::string& aToken)
   : Exception("Invalid command line argument: " + aToken)
{
   mReturnCode = 1;
}

// ================================================================================================
WsfStandardApplication::InputProcessingFailure::InputProcessingFailure(const std::string& aWhat, bool aIsExpected)
   : Exception(aWhat)
{
   mReturnCode = aIsExpected ? 0 : 1;
}
