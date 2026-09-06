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

#ifndef WSFAPPLICATION_HPP
#define WSFAPPLICATION_HPP

#include "wsf_export.h"

#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "UtException.hpp"
class UtScriptTypes;
class WsfApplicationExtension;
class WsfExtensionList;
class WsfPluginManager;
class WsfScenario;
class WsfSimulation;
class WsfSystemLog;

//! Define a WSF application.
//!
//! An instance of WsfApplication (or a derived class thereof) should be created for any WSF
//! application and is required to  construct a WsfScenario. Only one instance of this object
//! may exist at any given time.
//!
//! The primary function of this object is to provide for the maintenance of application
//! extensions, plugins and application-wide objects and to support the orderly creation of
//! scenario and simulation objects.
//!
//! This class provides only the minimum functions. A user will typically use WsfStandardApplication,
//! which provides additional features that make it simpler to create WSF applications.
class WSF_EXPORT WsfApplication
{
   friend class WsfScenario;

public:
   //! An application exception
   class Exception : public UtException
   {
   public:
      Exception(const std::string& aWhat)
         : UtException(aWhat)
         , mReturnCode(0)
      {
      }

      int GetReturnCode() const { return mReturnCode; }

   protected:
      int mReturnCode;
   };

   //! An exception that is thrown when a feature to be tested is not present.
   //! This is an internal exception used for testing.
   class FeatureNotPresent : public Exception
   {
   public:
      FeatureNotPresent(bool aTestingEnabled);
      ~FeatureNotPresent() override = default;
   };

   using PluginPaths = std::list<std::string>;

   WsfApplication(const std::string& aApplicationName,
                  int                aArgc,
                  char*              aArgv[],
                  const PluginPaths& aPluginPaths = PluginPaths());
   virtual ~WsfApplication();

   //! Return the name of the application.
   const std::string& GetApplicationName() const { return mApplicationName; }

   //! @name Command line arguments methods.
   //@{
   void GetCommandLineArguments(int& aArgc, char**& aArgv);

   void RemoveCommandLineOptions(int aArgc, int aArgCount);
   //@}

   //! @name Application extension methods.
   //! Methods to add and find extensions.
   //@{
   void RegisterExtension(const std::string& aName, std::unique_ptr<WsfApplicationExtension> aExtensionPtr);

   void ExtensionDepends(const std::string& aExtensionName, const std::string& aDependsOnExtensionName, bool aRequired);

   bool                     ExtensionIsRegistered(const std::string& aName) const;
   WsfApplicationExtension* FindExtension(const std::string& aName) const;
   WsfApplicationExtension& GetExtension(const std::string& aName) const;

   //! Returns all extension dependencies, added by ExtensionDepends()
   const WsfExtensionList& GetExtensions() const { return *mExtensionListPtr; }
   //@}

   //! @name Feature Management.
   //! 'Features' are simply strings that have been registered to indicate that certain features
   //! are present. Input files may test for the presence of features with the 'conditionals' block
   //! or the 'test_feature' command.
   //!
   //! This class registers certain standard features that define the execution environment:
   //! - Operating system: "os:linux", "os:windows", "os:apple"
   //! - Processor: "cpu:intel"
   //! - Addressing mode: "addr:32-bit", "addr:64-bit"
   //! - Build type: "build:debug", "build:release"
   //!
   //! In addition, extensions may register additional features.
   //@{
   //! An application feature, used for test if features exist
   struct Feature
   {
      //! Name of the feature
      std::string mName;
      //! Name of the project which implements the feature
      std::string mProject;
   };

   void RegisterFeature(const std::string& aFeature, const std::string& aProjectName = std::string());

   //! Returns a list of registered features.
   const std::vector<Feature>& GetRegisteredFeatures() { return mRegisteredFeatures; }
   //@}

   //! @name Access to system-wide objects maintained by this class.
   //@{
   //! Return a pointer to the script types/classes.
   UtScriptTypes* GetScriptTypes() const { return mScriptTypesPtr; }

   //! Return a reference to the system log.
   WsfSystemLog& GetSystemLog() { return *mSystemLogPtr; }
   //@}

   //! @name Automated testing methods.
   //@{
   //! Returns 'true' if application is executing under the control of an automated test executive.
   bool IsTestingEnabled() const { return mIsTestingEnabled; }
   //@}

private:
   //! Declare but don't define to prevent copying
   WsfApplication(const WsfApplication& aSrc);

   void RegisterDefaultFeatures();

   void ScenarioCreated(WsfScenario& aScenario);
   void SimulationCreated(WsfSimulation& aSimulation);

   bool InitializeTestEnvironment();

   //! The single application instance
   static WsfApplication* mInstancePtr;

   //! The name of the application
   std::string mApplicationName;

   //! The command-line parameters.  May be modified by extensions.
   std::vector<char*> mArgv;

   //! The list of extensions that have been registered.
   WsfExtensionList* mExtensionListPtr;

   //! Features registered with the application.
   std::vector<Feature> mRegisteredFeatures;

   //! The system log.
   WsfSystemLog* mSystemLogPtr;

   //! Script application classes are shared by the whole application
   UtScriptTypes* mScriptTypesPtr;

   //! Pointer to the manager that supports plug-ins.
   WsfPluginManager* mPluginManagerPtr;

   //! Used to lock extension list during certain operations.
   std::recursive_mutex mMutex;

   //! If true, this application is running in a auto-test environment
   bool mIsTestingEnabled;
};

// Define a macro that, if used in a standard way, provides a way to register an extension without
// the need to bring a header file for the extension. This can potentially eliminate a the need
// to define a lot of include paths to build the application. The extension should define a static
// method of the form:
//
//     void Register_<ExtensionName>(WsfApplication& aApplication);
//
// 'WSF_REGISTER_APPLICATION(app, <ExtensionName>)' generates the code:
//
//     void Register_<ExtensionName>(WsfApplication& aApplication);
//     Register_<ExtensionName>(app);
//
// For instance:
//
//   WsfApplication app;
//   WSF_REGISTER_EXTENSION(app, WsfEventOutput);
//   WSF_REGISTER_EXTENSION(app, WsfDisInterface);
//
// will register the standard event output and DIS interface extensions...

#define WSF_REGISTER_EXTENSION(APP, NAME)                     \
   extern void Register_##NAME(WsfApplication& aApplication); \
   Register_##NAME(APP);

#endif
