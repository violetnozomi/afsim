// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "MapHoverInfoPrefObject.hpp"

//! Returns true if the array could be read successfully.
//! Returns false otherwise.
static bool ReadList(QStringList& aPrefList, const QString& aName, QSettings& aSettings)
{
   if (aSettings.contains(aName + "/size"))
   {
      aPrefList.clear();
      const int size = aSettings.beginReadArray(aName);
      for (int i = 0; i < size; ++i)
      {
         aSettings.setArrayIndex(i);
         aPrefList.push_back(aSettings.value("Name").toString());
      }
      aSettings.endArray();

      return true;
   }

   return false;
}

static void WriteList(const QStringList& aPrefList, const QString& aName, QSettings& aSettings)
{
   aSettings.beginWriteArray(aName);
   {
      int i = 0;
      for (const QString& item : aPrefList)
      {
         aSettings.setArrayIndex(i);
         aSettings.setValue("Name", item);
         ++i;
      }
   }
   aSettings.endArray();
}

MapHoverInfo::PrefObject::PrefObject(QObject* parent /*= nullptr*/)
   : wkf::PrefObjectT<PrefData>(parent, cNAME)
{
}

void MapHoverInfo::PrefObject::Apply()
{
   emit HoverPrefsChanged();
}

MapHoverInfo::PrefData MapHoverInfo::PrefObject::ReadSettings(QSettings& aSettings) const
{
   PrefData pData;
   pData.mShowTooltips   = aSettings.value("ShowTooltips", mDefaultPrefs.mShowTooltips).toBool();
   pData.mShowNames      = aSettings.value("ShowNames", mDefaultPrefs.mShowNames).toBool();
   pData.mShowItemLabels = aSettings.value("ShowLabels", mDefaultPrefs.mShowItemLabels).toBool();

   if (!ReadList(pData.mPlatformUpdaterList, "PlatformUpdaters", aSettings))
   {
      pData.mPlatformUpdaterList = mDefaultPrefs.mPlatformUpdaterList;
   }

   if (!ReadList(pData.mTrackUpdaterList, "TrackUpdaters", aSettings))
   {
      pData.mTrackUpdaterList = mDefaultPrefs.mTrackUpdaterList;
   }

   if (!ReadList(pData.mPlatformEventList, "PlatformEvents", aSettings))
   {
      pData.mPlatformEventList = mDefaultPrefs.mPlatformEventList;
   }

   if (!ReadList(pData.mWeaponEventList, "WeaponEvents", aSettings))
   {
      pData.mWeaponEventList = mDefaultPrefs.mWeaponEventList;
   }

   return pData;
}

void MapHoverInfo::PrefObject::SaveSettingsP(QSettings& aSettings) const
{
   aSettings.setValue("ShowTooltips", mCurrentPrefs.mShowTooltips);
   aSettings.setValue("ShowNames", mCurrentPrefs.mShowNames);
   aSettings.setValue("ShowLabels", mCurrentPrefs.mShowItemLabels);

   WriteList(mCurrentPrefs.mPlatformUpdaterList, "PlatformUpdaters", aSettings);
   WriteList(mCurrentPrefs.mTrackUpdaterList, "TrackUpdaters", aSettings);
   WriteList(mCurrentPrefs.mPlatformEventList, "PlatformEvents", aSettings);
   WriteList(mCurrentPrefs.mWeaponEventList, "WeaponEvents", aSettings);
}
