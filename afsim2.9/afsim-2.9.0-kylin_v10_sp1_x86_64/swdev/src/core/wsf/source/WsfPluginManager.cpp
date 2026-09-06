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

#include "WsfPluginManager.hpp"

#include "wsf_export.h"

#include <sstream>

#include "UtDynamicLibrary.hpp"
#include "UtLog.hpp"
#include "UtPath.hpp"
#include "UtScanDir.hpp"
#include "WsfApplication.hpp"
#include "WsfPlugin.hpp"
#include "WsfScenario.hpp"

// WSF_PLUGIN_BUILD is a preprocessor definition, set in WSF's project settings when building
// it as a dynamic library.
// cLOAD_PLUGINS indicates whether the plugin manager should attempt to load plugins.
// If WSF isn't a DLL, then it should not.

// =================================================================================================
WsfPluginManager::WsfPluginManager(WsfApplication& aApplication)
   : UtPluginManager(WSF_PLUGIN_API_MAJOR_VERSION, WSF_PLUGIN_API_MINOR_VERSION, WSF_PLUGIN_API_COMPILER_STRING)
   ,
#ifdef wsf_EXPORTS
   cLOAD_PLUGINS(true)
   ,
#else
   cLOAD_PLUGINS(false)
   ,
#endif
   mApplicationPtr(&aApplication)
   , mLoaded(false)
   , mPluginSearchPaths()
{
}

// =================================================================================================
// virtual
WsfPluginManager::~WsfPluginManager()
{
   for (const auto& loadedPlugin : mLoadedPlugins)
   {
      if (mDynamicLibraryMap.find(loadedPlugin) != mDynamicLibraryMap.end())
      {
         CompleteFuncPtr completeFuncPtr =
            (CompleteFuncPtr)mDynamicLibraryMap[loadedPlugin]->GetSymbol("WSF_PluginComplete");
         if (completeFuncPtr != nullptr)
         {
            (*completeFuncPtr)(*mApplicationPtr);
         }
      }
   }
}

void WsfPluginManager::AddPluginSearchPath(const std::string& aPluginPath, bool aRecursive)
{
   mPluginSearchPaths.emplace_back(aPluginPath, aRecursive);
}

// =================================================================================================
void WsfPluginManager::LoadPlugins()
{
   if (cLOAD_PLUGINS && !mLoaded)
   {
      { // RAII block
         auto out = ut::log::info() << "Plugin API version info:";
         out.AddNote() << "Version: " << GetVersion().mMajor << "." << GetVersion().mMinor;
         out.AddNote() << "Compiler: " << GetVersion().mCompilerVersion;
      }

      // Load plugins from each of the specified paths.
      for (const auto& psp : mPluginSearchPaths)
      {
         UtPath           path(psp.first);
         UtPath::StatType pathStat = path.Stat();

         // If the path string is a specific plugin, then load it.
         if (pathStat == UtPath::cFILE)
         {
            std::string errorString;
            LoadPlugin(path.GetNormalizedPath(), errorString);
         }
         else if (pathStat == UtPath::cDIRECTORY) // Otherwise, load all plugins in the directory, recursing if specified.
         {
            LoadAll(path.GetNormalizedPath(), psp.second);
         }
         mLoaded = true;
      }
      PrintLoadedPlugins();
   }
}

// =================================================================================================
bool WsfPluginManager::LoadPluginInitialize(UtDynamicLibrary* aLibraryPtr, const std::string& aPluginFilename)
{
   // Get the plugin entry point (UtPluginInitPlugin).
   VersionFuncPtr versionFuncPtr = (VersionFuncPtr)aLibraryPtr->GetSymbol("WsfPluginVersion");
   UtPath         path(aPluginFilename);
   if (versionFuncPtr != nullptr)
   {
      // Initialize the plugin
      UtPluginVersion version(0, 0, "");
      (*versionFuncPtr)(version);
      if (ValidateVersion(version))
      {
         SetupFuncPtr setupFuncPtr = (SetupFuncPtr)aLibraryPtr->GetSymbol("WsfPluginSetup");
         if (setupFuncPtr != nullptr)
         {
            (*setupFuncPtr)(*mApplicationPtr);
         }
         else
         {
            auto out = ut::log::warning() << "Failed to load plugin.";
            out.AddNote() << "Plugin: " << path.GetSystemPath();
            out.AddNote() << "Error: Unable to find WsfPluginSetup function.";
            return false;
         }
      }
      else
      {
         const bool versionError = GetVersion().mMajor != version.mMajor || GetVersion().mMinor != version.mMinor;
         auto       out          = ut::log::warning() << "Failed to load plugin.";
         out.AddNote() << "Plugin: " << path.GetSystemPath();
         auto note = out.AddNote() << "Error: " << (versionError ? "Version mismatch." : "Compiler mismatch.");
         note.AddNote() << "Version: " << version.mMajor << "." << version.mMinor;
         note.AddNote() << "Compiler: " << version.mCompilerVersion;
         return false;
      }
   }
   else
   {
      auto out = ut::log::warning() << "Failed to load plugin.";
      out.AddNote() << "Plugin: " << path.GetSystemPath();
      out.AddNote() << "Error: Unable to find WsfPluginVersion symbol.";
      return false;
   }
   return true;
}

void WsfPluginManager::PrintLoadedPlugins()
{
   if (!mLoadedPlugins.empty())
   {
      auto out      = ut::log::info() << "Plugins Loaded:";
      auto listNote = out.AddNote();
      bool first    = true;
      for (const auto& plugin : mLoadedPlugins)
      {
         if (!first)
         {
            listNote << ", ";
         }
         listNote << UtPath(plugin).GetFileName();
         first = false;
      }
   }
}
