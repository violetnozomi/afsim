// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WKFPLUGINMANAGER_HPP
#define WKFPLUGINMANAGER_HPP

#include <map>

class QSettings;
#include "wkf_export.h"

#include "UtPluginManager.hpp"
#include "VaCallbackHolder.hpp"
#include "WkfPlugin.hpp"

namespace wkf
{
class PluginData;

class PluginManager : public UtPluginManager
{
   friend class PluginData;

public:
   explicit PluginManager(const QString& aFunctionTag);
   PluginManager(const PluginManager& aSrc) = delete;
   ~PluginManager() override;

   enum PluginLoadStatus
   {
      cLOAD_SUCCESS, // Plugin was loaded successfully
      cLOAD_FAIL,    // Plugin attempt to load failed
      cLOAD_DEFER,   // Plugin set to not auto-start
      cLOAD_IGNORE   // Plugin tags do not match plugin manager's function tag
   };

   using PluginIdMap   = std::map<size_t, PluginData>;
   using PluginFileMap = std::map<std::string, size_t>;
   using PluginEntry   = std::pair<size_t, Plugin*>;
   using PluginList    = std::list<PluginEntry>;

   void                   LoadSettings(QSettings& aSettings);
   void                   Initialize();
   std::list<std::string> GetPluginDirectories() const;

   const PluginFileMap&  GetPluginFileMap() const { return mPluginFileMap; }
   WKF_EXPORT PluginList GetLoadedPlugins() const;
   WKF_EXPORT Plugin*    GetPlugin(const size_t aUid) const;
   WKF_EXPORT PluginData GetPluginData(const size_t aUid) const;

   std::string GetDescription(const size_t aUid) const;
   bool        IsAutoStart(const QString& aPluginName) const;
   void        SetAutoStart(const QString& aPluginName, bool aAutoStart);

   bool PluginExistsWithRole(Plugin::Roles aRole) const;

   void FailedToLoadPlugin(const std::string& aPluginFilename, std::string& aErrorString) override;

private:
   // check the tags reports by the plug-in to see if they are usable for the given application function
   bool CheckTags(const QStringList& aTags) const;

   bool LoadPluginInitialize(UtDynamicLibrary* aLibraryPtr, const std::string& aPluginFilename) override;

   using RegistrationFuncPtr = void (*)(PluginRegistration*);
   using GetTagsFuncPtr      = const char* (*)();
   using CreateFuncPtr       = Plugin* (*)(const size_t aUniqueId);

   void SaveSettings(QSettings& aSettings);

   PluginIdMap             mPluginIdMap;
   PluginFileMap           mPluginFileMap; // Key is filename
   vespa::VaCallbackHolder mCallbacks;
   QString                 mFunctionTag;

   enum class PluginAutoLoad
   {
      cNO_LOAD, // User has specified not to load the plugin
      cLOAD,    // User has specified to load the plugin
      cDEFAULT  // User has not specified. Use plugin default setting.
   };
   // Maps storing the user overrides for plugin auto-loading and plugin default settings. Key is plugin name.
   std::map<QString, PluginAutoLoad> mPluginUserLoad;
   std::map<QString, bool>           mPluginDefaultLoad;
   size_t                            mUid;
};

class PluginData
{
   friend class PluginManager;
   friend class PluginManagerDialog;

private:
   PluginData() = default;

   void SetStatus(PluginManager::PluginLoadStatus aLoadStatus, const std::string& aStatusReason)
   {
      mLoadStatus   = aLoadStatus;
      mStatusReason = aStatusReason;
   }

   PluginRegistration              mRegistration;
   Plugin*                         mPluginPtr{nullptr};
   PluginManager::CreateFuncPtr    mCreateFnPtr{nullptr};
   PluginManager::PluginLoadStatus mLoadStatus;
   std::string                     mStatusReason;
   std::string                     mFilePath;
};
} // namespace wkf
#endif // WKFPLUGINMANAGER_HPP
