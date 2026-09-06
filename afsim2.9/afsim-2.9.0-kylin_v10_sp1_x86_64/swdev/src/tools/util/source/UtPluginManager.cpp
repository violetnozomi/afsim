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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "UtPluginManager.hpp"

#include <iostream>

#include "UtDynamicLibrary.hpp"
#include "UtLog.hpp"
#include "UtPath.hpp"
#include "UtScanDir.hpp"

namespace
{
int RegisterObjectStatic(void* aInstancePtr, const UtPluginRegistration& aRegistration)
{
   int registered = -1;
   if (aInstancePtr != nullptr)
   {
      UtPluginManager* pluginManagerPtr = static_cast<UtPluginManager*>(aInstancePtr);
      registered                        = pluginManagerPtr->RegisterObject(aRegistration);
   }
   return registered;
}

//! The appropriate plugin file extension based on the current operating system.
#ifdef _WIN32
constexpr const char* libraryExtension = "dll";
#else
constexpr const char* libraryExtension = "so";
#endif
} // namespace

std::ostream& operator<<(std::ostream& aOut, const UtPluginVersion& aVersion)
{
   aOut << aVersion.mMajor << '.' << aVersion.mMinor;
   return aOut;
}

// static
UtPluginManager::SystemConfig UtPluginManager::ParsePluginVersion(const UtPluginVersion& aVersion)
{
   SystemConfig       config;
   std::istringstream iss(aVersion.mCompilerVersion);
   std::getline(iss, config.mPlatform, '_'); // platform (win or lnx)
   config.mCompilerVersion = [&iss]()        // compiler version (e.g. 1910 for MSVC)
   {
      std::string temp;
      std::getline(iss, temp, '_');
      return std::stoul(temp);
   }();
   std::getline(iss, config.mWordSize, '_'); // word size (64bit or 32bit)
   std::getline(iss, config.mBuildType);     // build type (release or debug)
   return config;
};

UtPluginManager::UtPluginManager(unsigned int aPluginMajorVersion,
                                 unsigned int aPluginMinorVersion,
                                 const char*  aPluginCompilerString)
   : mRegistrationService({aPluginMajorVersion, aPluginMinorVersion, aPluginCompilerString}, this, RegisterObjectStatic)
{
   mSystemConfig = ParsePluginVersion(GetVersion());
}

UtPluginManager::~UtPluginManager()
{
   Shutdown();
}

//! ValidateVersion (UtPluginManager::ValidateVersion)
//! Determines whether a plugin version matches the version of the registration service
//! @param aVersion The plugin's version.
//! @return Returns true if the version matches exactly.
bool UtPluginManager::ValidateVersion(const UtPluginVersion& aVersion)
{
   bool status = ((GetVersion().mMajor == aVersion.mMajor) && (GetVersion().mMinor == aVersion.mMinor));
   if (status)
   {
      status = ValidateCompilerVersion(aVersion);
   }
   return status;
}

bool UtPluginManager::ValidateCompilerVersion(const UtPluginVersion& aVersion)
{
   bool status = true;

   // sample compiler string:   win_1910_64bit_release

   // parse the plugin version string into its components
   SystemConfig pluginConfig = ParsePluginVersion(aVersion);

   // if the platform, word-size, or build type are different between plugin and host, fail
   if ((pluginConfig.mPlatform != mSystemConfig.mPlatform) || (pluginConfig.mWordSize != mSystemConfig.mWordSize) ||
       (pluginConfig.mBuildType != mSystemConfig.mBuildType) ||
#ifdef _WIN32
       // For windows, we only need to compare the first two digits of the compiler version, which
       // indicate the C++ toolset major number. For example, VS2015 (1900), VS2017 (191x), and VS2019 (192x)
       // are binary compatible.
       (pluginConfig.mCompilerVersion / 100 != mSystemConfig.mCompilerVersion / 100))
#elif defined(__linux__)
       (pluginConfig.mCompilerVersion > mSystemConfig.mCompilerVersion))
#endif
   {
      status = false;
   }

   return status;
}

//! Shuts down each plugin and cleans up memory.
//! @return Returns the number of plugins that were shutdown cleanly.
int UtPluginManager::Shutdown()
{
   int shutdown = 0;

   // Call each of the plugin's exit functions.
   for (auto& exitFunction : mExitFunctions)
   {
      try
      {
         if (exitFunction() == 0)
         {
            ++shutdown;
         }
      }
      catch (std::exception&)
      {
      }
   }

   mRegistrationMap.clear();
   mDynamicLibraryMap.clear();
   mRegistrationMap.clear();
   mExitFunctions.clear();

   return shutdown;
}

//! Initializes the plugin.
//! @param aInitFuncPtr The plugin's initialization function pointer.
//! @return Returns true if the plugin was successfully initialized.
bool UtPluginManager::InitializePlugin(UtPluginInitFunc aInitFuncPtr)
{
   bool initialized = false;

   // Initialize the plugin and acquire the exit function.
   UtPluginExitFunc exitFuncPtr = aInitFuncPtr(&mRegistrationService);
   if (exitFuncPtr != nullptr)
   {
      // Store the exit function. We'll call this when we unload the plugin.
      mExitFunctions.push_back(exitFuncPtr);
      initialized = true;
   }
   return initialized;
}

//! Attempts to load all plugins from the provided directory recursing into sub-directories.
//! @param aPluginDirectory The directory to search and load plugins from.
//! @param aRecursive Whether to recurse into sub-directories or not.
//! @return Returns the number of plugins that were successfully loaded.
int UtPluginManager::LoadAll(const std::string& aPluginDirectory, bool aRecursive)
{
   int loaded = 0;

   if (!aPluginDirectory.empty())
   {
      UtScanDir::DirEntries dirEntries;
      if (aRecursive)
      {
         UtScanDir::ScanDirRecursive(aPluginDirectory, dirEntries, std::string{"*."} + libraryExtension);
      }
      else
      {
         UtScanDir::ScanDir(aPluginDirectory, dirEntries, std::string{"*."} + libraryExtension);
      }

      for (const auto& entry : dirEntries)
      {
         if (!entry.mDirectory)
         {
            std::string filename;
            if (aRecursive)
            {
               filename = entry.mFilename;
            }
            else
            {
               filename = aPluginDirectory + UtScanDir::GetSlashChar() + entry.mFilename;
            }

            std::string errorString;
            if (LoadPlugin(filename, errorString))
            {
               mLoadedPlugins.insert(filename);
               ++loaded;
            }
         }
      }
   }
   return loaded;
}

//! Attempts to load the specified plugin.
//! @param aPluginFilename The plugin's filename including path.
//! @param aErrorString If the plugin fails to load, the error string contains the reason.
//! @return Returns true if the plugin was loaded.
bool UtPluginManager::LoadPlugin(const std::string& aPluginFilename, std::string& aErrorString)
{
   bool loaded = false;

   // Don't load the same dynamic library twice
   if (mDynamicLibraryMap.find(aPluginFilename) == mDynamicLibraryMap.end())
   {
      UtDynamicLibrary* libraryPtr = LoadLibrary(aPluginFilename, aErrorString);
      if (libraryPtr != nullptr)
      {
         loaded = LoadPluginInitialize(libraryPtr, aPluginFilename);
      }
      else
      {
         FailedToLoadPlugin(aPluginFilename, aErrorString);
      }
   }
   return loaded;
}

//! Responds when a plugin has failed to load.  This will usually involve notifying the user
//! @param aPluginFilename The plugin's filename including path.
//! @param aErrorString error string contains the reason the plugin failed to load
void UtPluginManager::FailedToLoadPlugin(const std::string& aPluginFilename, std::string& aErrorString)
{
   auto out = ut::log::error() << "Failed to load plugin.";
   out.AddNote() << aErrorString;
}

//! Makes sure that the plugin parameters are valid.
//! @param aRegistration The plugin registration object.
//! @return Returns true if the plugin parameters are valid.
bool UtPluginManager::IsValidObject(const UtPluginRegistration& aRegistration) const
{
   bool valid = true;
   if ((aRegistration.mCreateFuncPtr == nullptr) || (aRegistration.mDestroyFuncPtr == nullptr) ||
       (aRegistration.mObjectName == nullptr) || (*(aRegistration.mObjectName) == '\0') ||
       (aRegistration.mObjectType == nullptr) || (*(aRegistration.mObjectType) == '\0') ||
       (aRegistration.mVersion.mCompilerVersion == nullptr) || (*(aRegistration.mVersion.mCompilerVersion) == '\0'))
   {
      valid = false;
   }
   return valid;
}

//! Register a plugin object with the plugin manager.
//! @param aRegistration The plugin object's registration object.
//! @return 1 - Indicates a successful registration.
//!         0 - Indicates failure because object type/name already exists.
//!        -1 - Indicates failure for unknown reason.
//!        -2 - Indicates failure because the compiler version doesn't match.
//!        -3 - Indicates failure because the major version doesn't match.
int UtPluginManager::RegisterObject(const UtPluginRegistration& aRegistration)
{
   int registered = -1;

   // Validate the plugin's registration data.
   if (IsValidObject(aRegistration))
   {
      const UtPluginVersion& version = aRegistration.mVersion;

      // Verify that the compiler version matches
      if (strcmp(version.mCompilerVersion, mRegistrationService.mVersion.mCompilerVersion) == 0)
      {
         // Verify that the plugin version matches.
         if (ValidateVersion(version))
         {
            std::string objectType(aRegistration.mObjectType);
            std::string objectName(aRegistration.mObjectName);

            // Add the plugin to the registration map.
            auto iter = mRegistrationMap.find(objectType);
            if (iter == mRegistrationMap.end())
            {
               mRegistrationMap.insert(RegistrationPair(objectType, PluginNameMap()));
               iter = mRegistrationMap.find(objectType);
            }

            // Add the plugin name to the name map.
            if (iter != mRegistrationMap.end())
            {
               PluginNameMap& nameMap = iter->second;

               // Make sure we don't have two objects with the same name.OB
               if (nameMap.find(objectName) == nameMap.end())
               {
                  nameMap[objectName] = std::make_shared<UtPluginRegistration>(aRegistration);
                  registered          = 1; // success.
                  auto out            = ut::log::info() << "Successfully registered plugin object.";
                  out.AddNote() << "Type: " << aRegistration.mObjectType;
                  out.AddNote() << "Name: " << aRegistration.mObjectName;
               }
               else
               {
                  registered = 0; // object type/name already exist.
                  auto out   = ut::log::warning() << "Could not register plugin object because another plugin object "
                                                     "with the same type and name already exists.";
                  out.AddNote() << "Type: " << aRegistration.mObjectType;
                  out.AddNote() << "Name: " << aRegistration.mObjectName;
               }
            }
         }
         else
         {
            registered = -3; // plugin version doesn't match.
            auto out   = ut::log::warning() << "Could not load plugin object because the plugin API version does not "
                                               "match the plugin object version.";
            out.AddNote() << "Type: " << aRegistration.mObjectType;
            out.AddNote() << "Name: " << aRegistration.mObjectName;
            out.AddNote() << "API Version: " << GetVersion();
            out.AddNote() << "Plugin Object Version: " << version;
         }
      }
      else
      {
         registered = -2; // compiler version doesn't match.
         auto out   = ut::log::warning() << "Could not load plugin object because plugin's compiler version does not "
                                            "match the current compiler version.";
         out.AddNote() << "Type: " << aRegistration.mObjectType;
         out.AddNote() << "Name: " << aRegistration.mObjectName;
         out.AddNote() << "Plugin Compiler Version: " << version.mCompilerVersion;
         out.AddNote() << "Current Compiler Version: " << mRegistrationService.mVersion.mCompilerVersion;
      }
   }
   return registered;
}

//! Creates an object of the specified type and name.
//! An example would be aObjectType="WsfSensor" aObjectName="MySensor".
//! @param aObjectType The object type to create.
//! @param aObjectName The specific type to create.
//! @param aObjectDataPtr Any data you need to pass to the plugin for it to create the requested object.
//! @return Returns a void pointer to the object that was created.
void* UtPluginManager::CreateObject(const std::string& aObjectType, const std::string& aObjectName, void* aObjectDataPtr) const
{
   void* objectPtr = nullptr;

   // Make sure we have a plugin of type 'aObjectType'.
   auto regIter = mRegistrationMap.find(aObjectType);
   if (regIter != mRegistrationMap.end())
   {
      // Make sure the plugin supports the specified derived type 'aObjectName'.
      auto nameIter = regIter->second.find(aObjectName);
      if (nameIter != regIter->second.end())
      {
         objectPtr = CreateObject(*nameIter->second, aObjectDataPtr);
      }
   }
   return objectPtr;
}

void* UtPluginManager::CreateObject(const UtPluginRegistration& aRegistration, void* aObjectDataPtr) const
{
   // Have the plugin create the requested object.
   UtPluginObjectParameters op(aRegistration.mObjectType, aRegistration.mObjectName, aObjectDataPtr, &mRegistrationService);
   return aRegistration.CreateObject(op);
}

//! Cleans up the memory of the specified object type.
//! @param aObjectType The object type to destroy.
//! @param aObjectName The specific type to destroy.
//! @param aObjectPtr The object to destroy as void pointer.
void UtPluginManager::DestroyObject(const std::string& aObjectType, const std::string& aObjectName, void* aObjectPtr) const
{
   // Make sure we have a plugin of type 'aObjectType'.
   auto regIter = mRegistrationMap.find(aObjectType);
   if (regIter != mRegistrationMap.end())
   {
      // Make sure the plugin supports the specified derived type 'aObjectName'.
      auto nameIter = regIter->second.find(aObjectName);
      if (nameIter != regIter->second.end())
      {
         DestroyObject(*nameIter->second, aObjectPtr);
      }
   }
}

//! Cleans up the memory of the specified object type.
//! @param aRegistration The plugin's registration parameter.
//! @param aObjectPtr The object to destroy as void pointer.
void UtPluginManager::DestroyObject(const UtPluginRegistration& aRegistration, void* aObjectPtr) const
{
   // Have the plugin destroy the requested object.
   aRegistration.DestroyObject(aObjectPtr);
}

// Loads the specified dynamic library.
//! @param aName The name of the dynamic library.
//! @param aErrorString An error string that is filled by reference.
//! @return Returns the dynamic library object.
UtDynamicLibrary* UtPluginManager::LoadLibrary(const std::string& aName, std::string& aErrorString)
{
   UtDynamicLibrary* libraryPtr = UtDynamicLibrary::Load(aName, aErrorString);
   if (libraryPtr != nullptr)
   {
      // Add library to map, so it can be unloaded
      mDynamicLibraryMap.emplace(aName, std::unique_ptr<UtDynamicLibrary>(libraryPtr));
   }
   return libraryPtr;
}

//! Returns a list of all plugins that can create objects of the specified type.
//! @param aObjectType The object type you're interested in creating.
//! @param aPlugins Returns a 'PluginNameMap' by reference that contains a list of plugins.
void UtPluginManager::GetPluginsOfType(const std::string& aObjectType, PluginNameMap& aPlugins) const
{
   auto iter = mRegistrationMap.find(aObjectType);
   if (iter != mRegistrationMap.end())
   {
      aPlugins = iter->second;
   }
}

const UtPluginManager::RegistrationMap& UtPluginManager::GetRegistrationMap() const
{
   return mRegistrationMap;
}

bool UtPluginManager::LoadPluginInitialize(UtDynamicLibrary* aLibraryPtr, const std::string& aPluginFilename)
{
   // Get the plugin entry point (UtPluginInitPlugin).
   UtPluginInitFunc initFuncPtr = (UtPluginInitFunc)(aLibraryPtr->GetSymbol("UtPluginInitPlugin"));
   if (initFuncPtr != nullptr)
   {
      // Initialize the plugin
      if (!InitializePlugin(initFuncPtr))
      {
         UtPath p(aPluginFilename);
         auto   out = ut::log::warning() << "Failed to initialize plugin.";
         out.AddNote() << "Plugin Name: " << p.GetFileName();
         return false;
      }
   }
   else
   {
      UtPath p(aPluginFilename);
      auto   out = ut::log::warning() << "Failed to initialize plugin due to not finding UtPluginInitFunc.";
      out.AddNote() << "Plugin Name: " << p.GetFileName();
   }

   return true;
}
