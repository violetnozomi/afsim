// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfPlatformDataPrefObject.hpp"

wkf::PlatformDataPrefObject::PlatformDataPrefObject(QObject* parent /*= nullptr*/)
   : wkf::PrefObjectT<PlatformDataPrefData>(parent, cNAME)
{
}

bool wkf::PlatformDataPrefObject::HasPreferences(const QString& aItemName) const
{
   return mCurrentPrefs.mStateList.find(aItemName) != mCurrentPrefs.mStateList.end();
}

bool wkf::PlatformDataPrefObject::IsVisible(const QString& aItemName) const
{
   const auto& state = mCurrentPrefs.mStateList.find(aItemName);
   if (state != mCurrentPrefs.mStateList.end())
   {
      return state->second;
   }
   return true;
}

void wkf::PlatformDataPrefObject::SetHidden(const QString& aItemName)
{
   mCurrentPrefs.mStateList[aItemName] = false;
}

void wkf::PlatformDataPrefObject::ClearHidden()
{
   for (auto&& item : mCurrentPrefs.mStateList)
   {
      item.second = true;
   }
}

void wkf::PlatformDataPrefObject::Apply() {}

wkf::PlatformDataPrefData wkf::PlatformDataPrefObject::ReadSettings(QSettings& aSettings) const
{
   PlatformDataPrefData pData;
   QStringList          keys = aSettings.childKeys();
   for (auto&& key : keys)
   {
      pData.mStateList[key] = aSettings.value(key).toBool();
   }
   return pData;
}

void wkf::PlatformDataPrefObject::SaveSettingsP(QSettings& aSettings) const
{
   aSettings.remove("");
   for (auto&& item : mCurrentPrefs.mStateList)
   {
      aSettings.setValue(item.first, item.second);
   }
}
