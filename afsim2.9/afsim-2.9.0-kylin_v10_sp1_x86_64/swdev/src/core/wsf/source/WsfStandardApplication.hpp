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

#ifndef WSFSTANDARDAPPLICATION_HPP
#define WSFSTANDARDAPPLICATION_HPP

#include "wsf_export.h"

#include "PakLogServerSubscriber.hpp"
#include "WsfApplication.hpp"

//! An implementation of WsfApplication that provides features common to 'standard' application.
//!
//! Many (but not all) WSF applications perform a fairly common set of operations. This function
//! of this class is to provide those features.
//!
//! An application that does not wish this standard behavior can simply use WsfApplication.
class WSF_EXPORT WsfStandardApplication : public WsfApplication
{
public:
   //! An exception that is thrown when a command line argument cannot be parsed.
   class InvalidCommandLineArgument : public Exception
   {
   public:
      InvalidCommandLineArgument(const std::string& aToken);
      ~InvalidCommandLineArgument() override = default;
   };

   //! An exception that is thrown when a command line argument cannot be parsed.
   class InputProcessingFailure : public Exception
   {
   public:
      InputProcessingFailure(const std::string& aWhat, bool aIsExpected);
      ~InputProcessingFailure() override = default;
   };

   //! The result of simulation execution.
   struct SimulationResult
   {
      //! A reset was requested by an external interface.
      //! If this is false then the simulation ran to completion.
      bool mResetRequested = false;
   };

   //! The simulation type requested from the command line.
   enum SimType
   {
      cEVENT_STEPPED, //!< '-es' was specified
      cFRAME_STEPPED, //!< '-fs' was specified
      cREAL_TIME      //!< '-rt' was specified
   };

   //! The type of run from the command line.
   enum RunMode
   {
      cRUN_SCENARIO,   //!< Run the scenario (implied if no other command specified)
      cLIST_VARIABLES, //!< List the preprocessor variables.
      cSHOW_USAGE,     //!< Show command line options and exit
      cWRITE_GRAMMAR   //!< Write the grammar file out, used by the IDE
   };

   //! Application options; Command line or otherwise
   struct WSF_EXPORT Options
   {
      SimType                  mSimType = cEVENT_STEPPED;
      bool                     mFlushIO = false;
      std::vector<std::string> mInputFiles;
      double                   mDeferredConnectionTime  = 0.0;
      double                   mMessageInterval         = 1000.0;
      double                   mRealtimeMessageInterval = 1.0;
      RunMode                  mRunMode                 = cRUN_SCENARIO;
   };

   WsfStandardApplication(const std::string& aApplicationName,
                          int                argc,
                          char*              argv[],
                          const PluginPaths& aPluginPaths = PluginPaths());
   ~WsfStandardApplication() override = default;

   void ShowUsage();

   void ProcessCommandLine(Options& aOptions);

   void ProcessCommandLine(Options& aOptions, int argc, char* argv[]);

   void ProcessCommandLineCommands(WsfScenario& aScenario, const Options& aOptions);

   void ProcessInputFiles(WsfScenario& aScenario, const std::vector<std::string>& aFileList);

   virtual std::unique_ptr<WsfSimulation> CreateSimulation(WsfScenario& aScenario,
                                                           SimType      aSimulationType,
                                                           unsigned int aRunNumber);
   virtual std::unique_ptr<WsfSimulation> CreateSimulation(WsfScenario&   aScenario,
                                                           const Options& aOptions,
                                                           unsigned int   aRunNumber);

   bool InitializeSimulation(WsfSimulation* aSimPtr);

   SimulationResult RunEventLoop(WsfSimulation* aSimPtr, Options aOptions);

protected:
   //! Log Server members
   log_server::LogSubscriber mLogServer;
   std::string               mLogServerHost            = "localhost";
   int                       mLogServerPort            = 18888;
   bool                      mShouldConnectToLogServer = false;
   virtual std::string       GetUsageString() const;
   void                      UpdateOptionsP(Options& aOptions, WsfSimulation* aSimPtr);

private:
   //! Set up tcp client connection with log server if we should
   void ConnectToLogServer();
};

#endif
