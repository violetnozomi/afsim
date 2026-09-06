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

#ifndef UTPLUGINMANAGER_HPP
#define UTPLUGINMANAGER_HPP

#include "ut_export.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

class UtDynamicLibrary;
#include <set>

#include "UtPlugin.hpp"

//! The UtPluginManager class is responsible for loading plugins that conform
//! to the interface defined in UtPlugin.hpp. It also provides an interface for
//! querying the types of objects (see  GetRegistrationMap and GetPluginsOfType)
//! that can be created by the available plugins as well as methods to actually
//! create and destroy these objects (see CreateObject and DestroyObject)

class UT_EXPORT UtPluginManager
{
public:
   using PluginNameMap   = std::map<std::string, std::shared_ptr<UtPluginRegistration>>;
   using RegistrationMap = std::map<std::string, PluginNameMap>;

   UtPluginManager(unsigned int aPluginMajorVersion, unsigned int aPluginMinorVersion, const char* aPluginCompilerString);
   UtPluginManager(const UtPluginManager& aSrc) = delete;
   UtPluginManager& operator=(const UtPluginManager&) = delete;
   virtual ~UtPluginManager();

   const UtPluginVersion& GetVersion() const { return mRegistrationService.mVersion; }

   virtual bool ValidateVersion(const UtPluginVersion& aVersion);

   virtual bool ValidateCompilerVersion(const UtPluginVersion& aVersion);

   int Shutdown();

   bool InitializePlugin(UtPluginInitFunc aInitFuncPtr);

   int LoadAll(const std::string& aPluginDirectory, bool aRecursive);

   bool LoadPlugin(const std::string& aPluginFilename, std::string& aErrorString);

   virtual bool LoadPluginInitialize(UtDynamicLibrary* aLibraryPtr, const std::string& aPluginFilename);

   virtual void FailedToLoadPlugin(const std::string& aPluginFilename, std::string& aErrorString);


   bool IsValidObject(const UtPluginRegistration& aRegistration) const;

   int   RegisterObject(const UtPluginRegistration& aRegistration);
   void* CreateObject(const std::string& aObjectType, const std::string& aObjectName, void* aObjectDataPtr) const;
   void* CreateObject(const UtPluginRegistration& aRegistration, void* aObjectDataPtr) const;
   void  DestroyObject(const std::string& aObjectType, const std::string& aObjectName, void* aObjectPtr) const;
   void  DestroyObject(const UtPluginRegistration& aRegistration, void* aObjectPtr) const;

   const RegistrationMap& GetRegistrationMap() const;

   void GetPluginsOfType(const std::string& aObjectType, PluginNameMap& aPlugins) const;

protected:
   using DynamicLibraryMap = std::map<std::string, std::unique_ptr<UtDynamicLibrary>>;
   using ExitFunctions     = std::vector<UtPluginExitFunc>;
   using RegistrationPair  = std::pair<std::string, PluginNameMap>;

   UtDynamicLibrary* LoadLibrary(const std::string& aName, std::string& aErrorString);

   UtPluginRegistrationService mRegistrationService;
   DynamicLibraryMap           mDynamicLibraryMap;
   std::set<std::string>       mLoadedPlugins;
   ExitFunctions               mExitFunctions;
   RegistrationMap             mRegistrationMap;

private:
   struct SystemConfig
   {
      std::string  mPlatform;
      unsigned int mCompilerVersion;
      std::string  mWordSize;
      std::string  mBuildType;
   };

   static SystemConfig ParsePluginVersion(const UtPluginVersion& aPluginVersion);

   SystemConfig mSystemConfig;
};

#endif
