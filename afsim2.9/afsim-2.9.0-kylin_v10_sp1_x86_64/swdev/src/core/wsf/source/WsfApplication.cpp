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

#include "WsfApplication.hpp"

#include <algorithm>
#include <cstdlib>

#include "GenSocketManager.hpp"
#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtPath.hpp"
#include "UtScriptTypes.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfAttributeContainer.hpp"
#include "WsfExtensionList.hpp"
#include "WsfPluginManager.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfSystemLog.hpp"
#include "WsfVersion.hpp"
#include "script/WsfScriptManager.hpp"

namespace
{
#if defined(_WIN32)
void ActivateWin32_ErrorMode();
#endif
} // namespace

WsfApplication* WsfApplication::mInstancePtr = nullptr;

// ================================================================================================
//! Constructor
//! @param aApplicationName       The name of the application.
//! @param aArgc                  The number of arguments. This is typically the 'argc' argument from 'main()'.
//! @param aArgv                  The argument list. This is typically the 'argv' argument from 'main()'.
//! @param aPluginPaths           Optional list of paths (relative to executable) to load plugins from.
WsfApplication::WsfApplication(const std::string& aApplicationName, int aArgc, char* aArgv[], const PluginPaths& aPluginPaths)
   : mApplicationName(aApplicationName)
   , mIsTestingEnabled(false)
{
   ut::SetupThreadErrorHandling();

   ut::log::Publisher::CaptureStdStreams(true);

   { // RAII block
      auto out = ut::log::info() << aApplicationName << ":";
      out.AddNote() << "WSF Version: " << wsf::version::GetReleaseString();
      out.AddNote() << "Built: " << __DATE__ << ' ' << __TIME__;
   }

   // Check if an instance already exists
   if (mInstancePtr != nullptr)
   {
      ut::log::fatal() << "Only one instance of WsfApplication may exist at any time.";
      exit(1);
   }
   assert(mInstancePtr == nullptr);
   mInstancePtr = this;

   // Copy the arguments into member variables for access by other components.
   for (int i = 0; i < aArgc; ++i)
   {
      mArgv.push_back(aArgv[i]);
   }

   // The original code had the extension list as a member variable, but compile times were getting long.
   mExtensionListPtr = new WsfExtensionList();

   RegisterDefaultFeatures();

   mSystemLogPtr = new WsfSystemLog;

   // Register all of the standard scripts classes
   WsfAttributeContainer::RegisterStandardTypes();
   mScriptTypesPtr = new UtScriptTypes(WsfStringId::GetDictionary());
   WsfScriptManager::RegisterTypes(mScriptTypesPtr);

   GenSockets::GenSocketManager::Ready();

   // Enable support for auto-tests before plugin manager is created so any plug-ins will be aware of
   // whether application is being run in automated testing mode
   InitializeTestEnvironment();

   mPluginManagerPtr = new WsfPluginManager(*this);
   // Append the standard paths to the list of Additional paths,
   // prior to passing the list of paths to check to the Plugin Manager
   char* pluginPathPtr = ::getenv("WSF_PLUGIN_PATH");
   if (pluginPathPtr != nullptr)
   {
      mPluginManagerPtr->AddPluginSearchPath(pluginPathPtr);
   }
   else
   {
      auto pluginPaths(aPluginPaths);
      if (pluginPaths.empty())
      {
         // Application specific plugin directory (needed for co-located applications)
         pluginPaths.push_back("../" + GetApplicationName() + "_plugins");
         // General plugin directory
         pluginPaths.emplace_back("../wsf_plugins");
      }

      for (const auto& pp : pluginPaths)
      {
         UtPath path(UtPath::GetExePath() + pp);
         mPluginManagerPtr->AddPluginSearchPath(path.GetSystemPath());
      }
   }

   mPluginManagerPtr->LoadPlugins();
}

// ================================================================================================
WsfApplication::~WsfApplication()
{
   // Delete in reverse order of creation.
   delete mExtensionListPtr;
   delete mSystemLogPtr;
   WsfAttributeContainer::ClearTypes();
   delete mScriptTypesPtr;
   delete mPluginManagerPtr;

   ut::log::Publisher::CaptureStdStreams(false);

   // Reset the single instance
   mInstancePtr = nullptr;
}

// ================================================================================================
//! Retrieves the command line argument list.
//! @param aArgc The number of command line arguments.
//! @param aArgv The command line arguments.
void WsfApplication::GetCommandLineArguments(int& aArgc, char**& aArgv)
{
   // Note: windows.h redefines GetCommandLine
   aArgc = (int)mArgv.size();
   aArgv = nullptr;
   if (aArgc != 0)
   {
      aArgv = &mArgv[0];
   }
}

// ================================================================================================
//! Removes command line options that have been processed.
//! This should be called to remove any arguments that have been processed by WSF libraries
//! to prevent the application from reading an option.
//! @param aArgc     The index of the first argument to be removed.
//! @param aArgCount The number of arguments to be removed.
void WsfApplication::RemoveCommandLineOptions(int aArgc, int aArgCount)
{
   int endIndex = std::min(aArgc + aArgCount, (int)mArgv.size());
   mArgv.erase(mArgv.begin() + aArgc, mArgv.begin() + endIndex);
}

// ================================================================================================
//! Register an extension.
//! @param aName The name of the extension to be registered
//! @param aExtensionPtr [input] A pointer to the extension instance to be registered.
//! Upon successful registration this class assumes ownership of the registered object and will
//! be responsible for its deletion.
//! @throw DuplicateExtension if an extension of the same name already exists.
void WsfApplication::RegisterExtension(const std::string& aName, std::unique_ptr<WsfApplicationExtension> aExtensionPtr)
{
   auto* ptr = aExtensionPtr.get();
   mExtensionListPtr->AddExtension(aName, std::move(aExtensionPtr));
   ptr->AddedToApplicationP(aName, *this);
}

// ================================================================================================
//! Add an extension dependency.
//! This guarantees that one extension is initialized before another.  Because the initialization
//! order is inherited by the scenario and simulation extensions, the dependency only needs declared here.
//! @param aExtensionName The name of the dependent extension
//! @param aDependsOnExtensionName The name of the extension to be initialized first
//! @param aRequired 'true' if the dependent extension cannot be used without the other
//!                  'false' if the dependent extension can still be used without the other
void WsfApplication::ExtensionDepends(const std::string& aExtensionName,
                                      const std::string& aDependsOnExtensionName,
                                      bool               aRequired)
{
   mExtensionListPtr->AddDependency(aExtensionName, aDependsOnExtensionName, aRequired);
}

// ================================================================================================
//! Determines if the extension with the given name is registered.
//! @param aName The name of the extension.
//! @returns true if the extension is registered or false if not.
bool WsfApplication::ExtensionIsRegistered(const std::string& aName) const
{
   return (mExtensionListPtr->FindExtension(aName) != nullptr);
}

// ================================================================================================
//! Returns a pointer to the extension with the given name.
//! @returns Pointer to the extension or nullptr if the extension is not registered.
WsfApplicationExtension* WsfApplication::FindExtension(const std::string& aName) const
{
   return static_cast<WsfApplicationExtension*>(mExtensionListPtr->FindExtension(aName));
}

// ================================================================================================
//! Returns a reference to the extension with the given name.
//! @throw WsfUnknownExtension if the extension is not registered.
WsfApplicationExtension& WsfApplication::GetExtension(const std::string& aName) const
{
   return static_cast<WsfApplicationExtension&>(mExtensionListPtr->GetExtension(aName));
}

// ================================================================================================
//! Register the presence of a feature.
//! @param aFeature     The name of the feature whose presence is to be registered.
//! @param aProjectName The name of the project/extension with which the feature is associated.
void WsfApplication::RegisterFeature(const std::string& aFeature, const std::string& aProjectName)
{
   Feature feature;
   feature.mName    = aFeature;
   feature.mProject = aProjectName;
   mRegisteredFeatures.push_back(feature);
}

// ================================================================================================
//! Register the default features that indicate the operating environment.
// private
void WsfApplication::RegisterDefaultFeatures()
{
   // Register various features that are only dependent on how the application was build.
   if (sizeof(void*) == 4)
   {
      RegisterFeature("addr:32-bit");
   }
   else if (sizeof(void*) == 8)
   {
      RegisterFeature("addr:64-bit");
   }

#ifdef _WIN32
   RegisterFeature("os:windows");
   RegisterFeature("cpu:intel");

#elif defined(__linux__)
   RegisterFeature("os:linux");
   RegisterFeature("cpu:intel");

#elif defined(__APPLE__)
   RegisterFeature("os:apple");
   RegisterFeature("cpu:intel");
#endif

#if defined(_MSC_VER)
#ifdef _DEBUG
   RegisterFeature("build:debug");
#else
   RegisterFeature("build:release");
#endif

#elif defined(__GNUC__)
#ifdef __OPTIMIZE__
   RegisterFeature("build:release");
#else
   RegisterFeature("build:debug");
#endif

#else
   RegisterFeature("build:unknown");
#endif
}

// ================================================================================================
//! A callback from WsfScenario::WsfScenario() to handle creation of a scenario.
//! This is called as the last thing in WsfScenario constructor.  The main function of this method
//! is to invoke WsfApplicationExtension::ScenarioCreated() for each of the registered application
//! extensions, honoring any dependencies defined by ExtensionDepends().
//! @param aScenario The scenario that is being created.
// private
void WsfApplication::ScenarioCreated(WsfScenario& aScenario)
{
   mScriptTypesPtr->Initialize();

   std::lock_guard<std::recursive_mutex> lock(mMutex);
   mExtensionListPtr->SortExtensions();
   const std::vector<std::string>& extensionOrder = mExtensionListPtr->GetExtensionOrder();
   for (const auto& extString : extensionOrder)
   {
      WsfApplicationExtension* extensionPtr = FindExtension(extString);
      if (extensionPtr != nullptr)
      {
         extensionPtr->ScenarioCreated(aScenario);
      }
   }
}

// ================================================================================================
//! Callback from WsfScenario::SimulationCreated() to indicate a simulation has been created.
//! This invokes WsfApplicationExtension::SimulationCreated() on each of the registered application
//! extensions, honoring any dependencies as defined by WsfApplication::ExtensionDepends().
//! @param aSimulation The simulation being created.
// private
void WsfApplication::SimulationCreated(WsfSimulation& aSimulation)
{
   for (const auto& extString : mExtensionListPtr->GetExtensionOrder())
   {
      WsfApplicationExtension* extensionPtr = FindExtension(extString);
      if (extensionPtr != nullptr)
      {
         extensionPtr->SimulationCreated(aSimulation);
      }
   }
}

// ================================================================================================
//! If running under the automated supervisor, initialize the environment for running the automated tests.
//! This must be called prior to any other function (including Initialize). The primary motivation
//! is to prevent dialog boxes from being posted if a fatal error occurs when running the automated
//! tests under Windows. If a dialog box were to get posted when running unattended, the testing
//! process would hang waiting for a response that will never occur. This may be extended in the future
//! to perform other functions as necessary.
//! @returns true if running under the automated test supervisor.
bool WsfApplication::InitializeTestEnvironment()
{
   // If running under Windows, turn off the dialog box that comes up if the application terminates
   // in an abnormal way. Otherwise the tests will never complete if a severe error that causes the
   // dialog box to be displayed.

   const char* autoTestPtr = getenv("AUTO_TEST");
   if ((autoTestPtr != nullptr) && (autoTestPtr[0] != 0))
   {
      mIsTestingEnabled = true;

#if defined(_WIN32)
      ActivateWin32_ErrorMode();
#endif
   }
   return mIsTestingEnabled;
}

// ================================================================================================
WsfApplication::FeatureNotPresent::FeatureNotPresent(bool aTestingEnabled)
   : Exception("Feature Not Present")
{
   if (!aTestingEnabled)
   {
      mReturnCode = 1;
   }
}

// ================================================================================================
// Note Windows-specific code.  Must remain last due to windows.h inclusion
// ================================================================================================
#if defined(_WIN32)
#include <windows.h>

#include <crtdbg.h>
namespace
{
void ActivateWin32_ErrorMode()
{
   SetErrorMode(SEM_NOGPFAULTERRORBOX);
   _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
   _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
}
} // namespace
#endif
