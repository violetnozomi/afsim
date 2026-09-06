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
#ifndef WSFPLUGINMANAGER_HPP
#define WSFPLUGINMANAGER_HPP

#include "wsf_export.h"

#include <list>
#include <memory>
#include <string>
#include <utility>

class UtInput;
#include "UtPluginManager.hpp"
class WsfObject;
class WsfApplication;

#define WSF_PLUGIN_DEFINE_VERSION                                           \
   extern "C"                                                               \
   {                                                                        \
      UT_PLUGIN_EXPORT void WSF_PluginVersion(UtPluginVersion* aOutVersion) \
      {                                                                     \
         *aOutVersion = UT_PLUGIN_VERSION_CREATE;                           \
      }                                                                     \
   }

//! The WsfPluginManager class is responsible for loading WSF dynamic plugins
//! and creating and registering the available object types with the appropriate
//! type lists (i.e. WsfSensor::AddType, WsfPlatform::AddType, etc.).

class WSF_EXPORT WsfPluginManager : public UtPluginManager
{
public:
   WsfPluginManager(WsfApplication& aApplication);
   ~WsfPluginManager() override;

   using VersionFuncPtr  = void (*)(UtPluginVersion& aOutVersion);
   using SetupFuncPtr    = void (*)(WsfApplication& aApplicationPtr);
   using CompleteFuncPtr = void (*)(WsfApplication& aApplicationPtr);

   void AddPluginSearchPath(const std::string& aPluginPath, bool aRecursive = true);

   void LoadPlugins();

   bool LoadPluginInitialize(UtDynamicLibrary* aLibraryPtr, const std::string& aPluginFilename) override;

   void PrintLoadedPlugins();

private:
   WsfPluginManager(const WsfPluginManager& aSrc) = delete; // Prevent use of operator= by declaring, but not defining.

   using PluginSearchPath  = std::pair<std::string, bool>;
   using PluginSearchPaths = std::list<PluginSearchPath>;

   const bool cLOAD_PLUGINS;

   WsfApplication*   mApplicationPtr;
   bool              mLoaded;
   PluginSearchPaths mPluginSearchPaths;
};

#endif
