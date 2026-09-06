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
#include "WkfSharedMapPreferencesObject.hpp"

#include <QDataStream>
#include <QSettings>

#include "UtPath.hpp"
#include "UtRunEnvManager.hpp"
#include "VaObserver.hpp"
#include "VaViewer.hpp"
#include "WkfObserver.hpp"

wkf::SharedMapPreferencesData::SharedMapPreferencesData()
{
   mMaps["Blue Marble"].mFile   = "$RESOURCE_PATH/bluemarble_db/bmng.earth";
   mMaps["Bald Earth"].mFile    = "$RESOURCE_PATH/bald_earth_db/bald.earth";
   mMaps["Political"].mFile     = "$RESOURCE_PATH/political_db/border.earth";
   mMaps["Natural Earth"].mFile = "$RESOURCE_PATH/naturalearth_db/natural.earth";

   mMaps["Blue Marble - flat"].mProjection = "CME_EQR";
   mMaps["Blue Marble - flat"].mFile       = "$RESOURCE_PATH/bluemarble_db/bmng_flat.earth";
   mMaps["Bald Earth - flat"].mProjection  = "CME_EQR";
   mMaps["Bald Earth - flat"].mFile        = "$RESOURCE_PATH/bald_earth_db/bald_flat.earth";
   mMaps["Political - flat"].mProjection   = "CME_EQR";
   mMaps["Political - flat"].mFile         = "$RESOURCE_PATH/political_db/border_flat.earth";
   mMaps.emplace("None", SharedMapData{});

   mProfileMap.emplace(wkf::SharedMapPreferencesData::IMMERSIVE, "Blue Marble");
   mProfileMap.emplace(wkf::SharedMapPreferencesData::NAVIGATIONAL, "Political");
}

wkf::SharedMapPreferencesObject::SharedMapPreferencesObject(QObject* aParent)
   : PrefObjectT<SharedMapPreferencesData>(aParent, cNAME)
   , mViewerDestroyedCallbackId(-1)
{
}

void wkf::SharedMapPreferencesObject::SetPreferenceDataP(const SharedMapPreferencesData& aPrefData)
{
   for (auto& pm : aPrefData.mProfileMap)
   {
      auto it = mCurrentPrefs.mProfileMap.find(pm.first);
      if (it == mCurrentPrefs.mProfileMap.end() || (pm.second != it->second))
      {
         mProfileChanged.insert(pm.first);
      }
   }
   BasePrefObject::SetPreferenceDataP(aPrefData);
}

void wkf::SharedMapPreferencesObject::Apply()
{
   // for every viewer in the shared maps
   //   set the map profile
   for (auto&& it : mProfileViewerMap)
   {
      if (mProfileChanged.find(it.second.second) != mProfileChanged.end())
      {
         SetMapProfile(it.second.second, it.second.first);
      }
   }
   mProfileChanged.clear();
   emit SharedMapsChanged();
   // nothing really needs to know about changes.  If something is using a deleted map it is OK.
}

wkf::SharedMapPreferencesData wkf::SharedMapPreferencesObject::ReadSettings(QSettings& aSettings) const
{
   SharedMapPreferencesData pData;
   int                      numMaps = aSettings.beginReadArray("Maps");
   if (numMaps == 0)
   {
      pData.mMaps = mDefaultPrefs.mMaps;
   }
   else
   {
      for (int i = 0; i < numMaps; ++i)
      {
         aSettings.setArrayIndex(i);
         std::string name       = aSettings.value("name").toString().toStdString();
         std::string projection = aSettings.value("projection").toString().toStdString();
         float       offset     = aSettings.value("offset").toFloat();
         bool        lock       = aSettings.value("lock").toBool();

         pData.mMaps[name].mProjection = projection;
         pData.mMaps[name].mLatOffset  = offset;
         pData.mMaps[name].mLock       = lock;

         // this is for backwards compatibility
         int numFile = aSettings.beginReadArray("files");
         if (numFile > 0)
         {
            aSettings.setArrayIndex(0);
            std::string url         = aSettings.value("url").toString().toStdString();
            pData.mMaps[name].mFile = url;
         }
         aSettings.endArray();

         std::string file = aSettings.value("file").toString().toStdString();
         if (!file.empty())
         {
            pData.mMaps[name].mFile = file;
         }
      }
   }
   aSettings.endArray();
   int numProfiles = aSettings.beginReadArray("MapProfiles");
   if (numProfiles == 0)
   {
      pData.mProfileMap = mDefaultPrefs.mProfileMap;
   }
   else
   {
      for (int i = 0; i < numProfiles; ++i)
      {
         aSettings.setArrayIndex(i);
         wkf::SharedMapPreferencesData::MapType type =
            static_cast<wkf::SharedMapPreferencesData::MapType>(aSettings.value("type").toInt());
         std::string map         = aSettings.value("map").toString().toStdString();
         pData.mProfileMap[type] = map;
      }
   }
   aSettings.endArray();
   return pData;
}

void wkf::SharedMapPreferencesObject::SaveSettingsP(QSettings& aSettings) const
{
   aSettings.beginWriteArray("Maps");
   int i = 0;
   for (auto& it : mCurrentPrefs.mMaps)
   {
      aSettings.setArrayIndex(i);
      aSettings.setValue("name", it.first.c_str());
      aSettings.setValue("projection", it.second.mProjection.c_str());
      aSettings.setValue("offset", it.second.mLatOffset);
      aSettings.setValue("lock", it.second.mLock);
      aSettings.setValue("file", it.second.mFile.c_str());
      ++i;
   }
   aSettings.endArray();
   aSettings.beginWriteArray("MapProfiles");
   i = 0;
   for (auto& it : mCurrentPrefs.mProfileMap)
   {
      aSettings.setArrayIndex(i);
      aSettings.setValue("type", it.first);
      aSettings.setValue("map", it.second.c_str());
      ++i;
   }
   aSettings.endArray();
}

const wkf::SharedMapData* wkf::SharedMapPreferencesObject::GetMapData(const std::string& aName) const
{
   std::map<std::string, SharedMapData>::const_iterator it = mCurrentPrefs.mMaps.find(aName);
   if (it != mCurrentPrefs.mMaps.end())
   {
      return &(it->second);
   }
   else
   {
      return nullptr;
   }
}

const std::map<std::string, wkf::SharedMapData>& wkf::SharedMapPreferencesObject::GetMaps() const
{
   return mCurrentPrefs.mMaps;
}

std::string wkf::SharedMapPreferencesObject::SetMapProfile(const wkf::SharedMapPreferencesData::MapType aType,
                                                           vespa::VaViewer*                             aViewerPtr)
{
   std::map<wkf::SharedMapPreferencesData::MapType, std::string>::const_iterator it =
      mCurrentPrefs.mProfileMap.find(aType);
   if (it != mCurrentPrefs.mProfileMap.end())
   {
      std::string mapName = it->second;
      if (mViewerDestroyedCallbackId == -1)
      {
         mViewerDestroyedCallbackId = mCallbacks.Add(
            vespa::VaObserver::ViewerDestroyed.Connect(&wkf::SharedMapPreferencesObject::ViewerDestroyed, this));
      }
      mProfileViewerMap[aViewerPtr->GetUniqueId()] =
         std::pair<vespa::VaViewer*, wkf::SharedMapPreferencesData::MapType>(aViewerPtr, aType);

      return SetMap(mapName, aViewerPtr);
   }
   else
   {
      return "";
   }
}

//! @details the SharedMapPreferences object needs to reset the profiles, but not the maps.
//! this override does a custom restore of defaults and emits the same signal that the
//! parent RestoreDefaults function emits.
void wkf::SharedMapPreferencesObject::RestoreDefaults()
{
   // restore the default profiles ONLY
   mCurrentPrefs.mProfileMap = mDefaultPrefs.mProfileMap;

   // emit the following signal, just like the parent RestoreDefaults function
   emit PreferencesChanged(QVariant::fromValue(mCurrentPrefs));
}

void wkf::SharedMapPreferencesObject::ViewerDestroyed(vespa::VaViewer* aViewerPtr)
{
   mProfileViewerMap.erase(aViewerPtr->GetUniqueId());
}

std::string wkf::SharedMapPreferencesObject::SetMap(const std::string& aMapName,
                                                    vespa::VaViewer*   aViewerPtr,
                                                    const std::string& aCheckFile) const
{
   if (aViewerPtr)
   {
      std::string file;
      const auto* md = GetMapData(aMapName);
      if (nullptr != md)
      {
         std::string projection = md->mProjection;
         float       offset     = md->mLatOffset;

         // find $RESOURCE_PATH and replace with UtRunEnvManager::GetCmeFolder("maps");
         std::string path       = md->mFile;
         size_t      replaceLoc = path.find("$RESOURCE_PATH");
         if (replaceLoc != std::string::npos)
         {
            std::string replaceWith = UtRunEnvManager::GetCmeRelBase() + "/maps";
            path.replace(replaceLoc, 14, replaceWith);
         }
         if (UtPath(path).Exists())
         {
            file += path;
         }
         if (!file.empty())
         {
            if (file != aCheckFile)
            {
               aViewerPtr->CreateDatabase(file, projection, offset);
               wkf::Observer::StandardMapChanged();
            }
            return file;
         }
         else
         {
            aViewerPtr->ClearTerrain();
            wkf::Observer::StandardMapChanged();
            return "";
         }
      }
      else
      {
         return aCheckFile; // if the map got deleted out from under us, no need to change
      }
   }
   return "";
}
