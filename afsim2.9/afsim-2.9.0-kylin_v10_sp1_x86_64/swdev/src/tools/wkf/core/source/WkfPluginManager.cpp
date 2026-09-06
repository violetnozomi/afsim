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
#include "WkfPluginManager.hpp"

#include "UtDynamicLibrary.hpp"
#include "UtLog.hpp"
#include "UtPath.hpp"
#include "UtRunEnvManager.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"
#include "WkfObserver.hpp"
#include "WkfPlugin.hpp"

wkf::PluginManager::PluginManager(const QString& aFunctionTag)
   : UtPluginManager(WKF_PLUGIN_API_VERSION, 0, WKF_PLUGIN_API_COMPILER_STRING)
   , mFunctionTag(aFunctionTag)
   , mUid(0)
{
   mCallbacks.Add(Observer::LoadSettings.Connect(&PluginManager::LoadSettings, this));
   mCallbacks.Add(Observer::SaveSettings.Connect(&PluginManager::SaveSettings, this));
}

wkf::PluginManager::~PluginManager()
{
   for (auto& p : mPluginIdMap)
   {
      Plugin* pluginPtr = p.second.mPluginPtr;
      // Shutdown disconnects the callbacks for the warlock::Plugins so that the simulation thread no longer adds events
      // to the event queue. deleteLater, adds an event to the event queue to delete the object.  Thus no simulation
      // thread events will be processed after the delete event is processed.
      if (pluginPtr != nullptr)
      {
         pluginPtr->Shutdown();
         // The deleteLater() call causes crashes on shutdown. We need to understand why this happens and deleteLater()
         // should be used
         //  over "delete pluginPtr" because deleting it immediately may cause rare and unpredictable threading crashes.
         //  Not calling delete or deleteLater will leak the memory.
         delete pluginPtr;
         // it->second.mPluginPtr->deleteLater();
      }
   }
}

void wkf::PluginManager::Initialize()
{
   int loaded = 0;
   for (const auto& dir : GetPluginDirectories())
   {
      loaded += LoadAll(dir, false);
   }

   wkfEnv.GetMainWindow()->ShowStatusMessage(QString("%1 plugins successfully loaded.").arg(loaded), 2500);

   auto out = ut::log::info() << "Plugin Manager, plugin API version:";
   out.AddNote() << "Major: " << GetVersion().mMajor;
   out.AddNote() << "Minor: " << GetVersion().mMinor;
   out.AddNote() << "Compiler: " << GetVersion().mCompilerVersion;
   out.AddNote() << "Successfully Loaded Plugins: " << loaded;
}

std::list<std::string> wkf::PluginManager::GetPluginDirectories() const
{
   std::list<std::string> pluginDirs;
   UtPath                 runPath(UtRunEnvManager::GetRunPath());

   for (const auto& dir : {QString("wkf"), mFunctionTag})
   {
      UtPath dirPath(runPath + dir.toStdString().append("_plugins"));
      pluginDirs.push_back(dirPath.GetSystemPath());
   }

   return pluginDirs;
}

wkf::PluginManager::PluginList wkf::PluginManager::GetLoadedPlugins() const
{
   PluginList plugins;
   for (auto& p : mPluginIdMap)
   {
      Plugin* pluginPtr = p.second.mPluginPtr;
      if (pluginPtr != nullptr)
      {
         plugins.emplace_back(p.first, pluginPtr);
      }
   }

   return plugins;
}

void wkf::PluginManager::LoadSettings(QSettings& aSettings)
{
   mPluginUserLoad.clear();

   aSettings.beginGroup("PluginManager");
   int size = aSettings.beginReadArray("plugin_list");
   for (int i = 0; i < size; ++i)
   {
      aSettings.setArrayIndex(i);
      mPluginUserLoad[aSettings.value("name").toString()] = (PluginAutoLoad)aSettings.value("auto_load").toInt();
   }
   aSettings.endArray();
   aSettings.endGroup();
}

void wkf::PluginManager::SaveSettings(QSettings& aSettings)
{
   aSettings.beginGroup("PluginManager");

   aSettings.remove("plugin_list");
   aSettings.beginWriteArray("plugin_list", (int)mPluginUserLoad.size());
   int i = 0;
   for (const auto& plugin : mPluginUserLoad)
   {
      if (plugin.second != PluginAutoLoad::cDEFAULT)
      {
         aSettings.setArrayIndex(i++);
         aSettings.setValue("name", plugin.first);
         aSettings.setValue("auto_load", (uint)plugin.second);
      }
   }
   aSettings.endArray();
   aSettings.endGroup();
}

wkf::Plugin* wkf::PluginManager::GetPlugin(const size_t aUid) const
{
   auto iter = mPluginIdMap.find(aUid);
   if (iter != mPluginIdMap.end())
   {
      return iter->second.mPluginPtr;
   }
   return nullptr;
}

wkf::PluginData wkf::PluginManager::GetPluginData(const size_t aUid) const
{
   auto iter = mPluginIdMap.find(aUid);
   if (iter != mPluginIdMap.end())
   {
      return iter->second;
   }
   return PluginData();
}

std::string wkf::PluginManager::GetDescription(const size_t aUid) const
{
   std::string description;
   auto        it = mPluginIdMap.find(aUid);
   if (it != std::end(mPluginIdMap))
   {
      description = it->second.mRegistration.mDescription;
   }
   return description;
}

bool wkf::PluginManager::IsAutoStart(const QString& aPluginName) const
{
   auto userit = mPluginUserLoad.find(aPluginName);
   if (userit != std::end(mPluginUserLoad))
   {
      switch (userit->second)
      {
      case PluginAutoLoad::cNO_LOAD:
         return false;
      case PluginAutoLoad::cLOAD:
         return true;
      case PluginAutoLoad::cDEFAULT: // Fall through to the plugin default setting
      default:
         break;
      }
   }
   auto defit = mPluginDefaultLoad.find(aPluginName);
   if (defit != std::end(mPluginDefaultLoad))
   {
      return defit->second;
   }
   return true;
}

void wkf::PluginManager::SetAutoStart(const QString& aPluginName, bool aAutoStart)
{
   mPluginUserLoad[aPluginName] = aAutoStart ? PluginAutoLoad::cLOAD : PluginAutoLoad::cNO_LOAD;
}

bool wkf::PluginManager::PluginExistsWithRole(Plugin::Roles aRole) const
{
   auto plugins = GetLoadedPlugins();
   for (auto& p : plugins)
   {
      if ((p.second->GetRoles() & aRole) || (p.second->GetRoles() == aRole)) // Handle the NO_ROLE case, which is 0
      {
         return true;
      }
   }
   return false;
}

bool wkf::PluginManager::LoadPluginInitialize(UtDynamicLibrary* aLibraryPtr, const std::string& aPluginFilename)
{
   std::string filename   = UtPath(aPluginFilename).GetFileName();
   QString     pluginName = QString("Unknown <%1>").arg(filename.c_str());

   auto nmresult = mPluginFileMap.emplace(filename, 0);
   if (nmresult.second) // if the filename is unique
   {
      size_t id                = ++mUid;
      mPluginFileMap[filename] = id;
      auto        result       = mPluginIdMap.emplace(id, PluginData{});
      PluginData& pluginData   = result.first->second;
      pluginData.mFilePath     = aPluginFilename;
      // Get the plugin registration symbol
      RegistrationFuncPtr regFnPtr = (RegistrationFuncPtr)aLibraryPtr->GetSymbol("wkf_plugin_registration");
      if (regFnPtr != nullptr)
      {
         PluginRegistration& reg = pluginData.mRegistration;
         (*regFnPtr)(&pluginData.mRegistration);
         if (!reg.mName.empty())
         {
            pluginName                     = reg.mName.c_str();
            mPluginDefaultLoad[pluginName] = reg.mLoadByDefault;
            if (ValidateVersion(reg)) // Validate plugin interface version
            {
               // Check for plugin tags match with plugin manager "function"
               GetTagsFuncPtr gettagFnPtr = (GetTagsFuncPtr)aLibraryPtr->GetSymbol("wkf_plugin_get_tags");
               if (gettagFnPtr != nullptr)
               {
                  QString tags = (*gettagFnPtr)();
                  if (CheckTags(tags.split("|")))
                  {
                     // Get symbol to create a plugin object
                     CreateFuncPtr createFnPtr = (CreateFuncPtr)aLibraryPtr->GetSymbol("wkf_plugin_create");
                     if (createFnPtr != nullptr)
                     {
                        pluginData.mCreateFnPtr = createFnPtr;
                        if (IsAutoStart(pluginName)) // Should plugin be loaded automatically?
                        {
                           pluginData.mPluginPtr = (*createFnPtr)(id);
                           if (pluginData.mPluginPtr)
                           {
                              pluginData.SetStatus(cLOAD_SUCCESS, "Plugin loaded successfully");
                           }
                           else
                           {
                              pluginData.SetStatus(cLOAD_FAIL, "Plugin failed to initialize");
                           }
                        }
                        else
                        {
                           pluginData.SetStatus(cLOAD_DEFER, "Plugin not auto-started");
                        }
                     }
                     else
                     {
                        pluginData.SetStatus(cLOAD_FAIL, "No create symbol");
                     }
                  }
                  else
                  {
                     std::string temp = "Tags did not match the application\nThe plugin's application tag is  " +
                                        tags.toStdString() + "\nExpected application tag of " + mFunctionTag.toStdString();
                     pluginData.SetStatus(cLOAD_IGNORE, temp.c_str());
                  }
               }
               else
               {
                  pluginData.SetStatus(cLOAD_FAIL, "No get tags symbol");
               }
            }
            else
            {
               std::string temp = "Incorrect plugin version\nPlugin's version is " + std::to_string(reg.mMajor) +
                                  "\nExpected Plugin version of " + std::to_string(GetVersion().mMajor);
               pluginData.SetStatus(cLOAD_FAIL, temp.c_str());
            }
         }
         else
         {
            pluginData.SetStatus(cLOAD_FAIL, "Plugin did not set name");
         }
      }
      else
      {
         pluginData.SetStatus(cLOAD_FAIL, "no registration symbol");
      }
      return (pluginData.mLoadStatus == cLOAD_SUCCESS);
   }
   return false;
}

bool wkf::PluginManager::CheckTags(const QStringList& aTags) const
{
   for (const auto& tag : aTags)
   {
      if ((tag == "all") || (tag == mFunctionTag))
      {
         return true;
      }
   }
   return false;
}

void wkf::PluginManager::FailedToLoadPlugin(const std::string& aPluginFilename, std::string& aErrorString)
{
   UtPluginManager::FailedToLoadPlugin(aPluginFilename, aErrorString);
   wkfEnv.GetMainWindow()->ShowStatusMessage(aErrorString.c_str(), 2500);

   PluginData pluginData;
   pluginData.SetStatus(cLOAD_FAIL, aErrorString);
   pluginData.mPluginPtr                 = nullptr;
   pluginData.mRegistration.mName        = aPluginFilename;
   pluginData.mRegistration.mDescription = aErrorString;
   mPluginFileMap.emplace(UtPath(aPluginFilename).GetFileName(), ++mUid);
   mPluginIdMap.emplace(mUid, pluginData);
}
