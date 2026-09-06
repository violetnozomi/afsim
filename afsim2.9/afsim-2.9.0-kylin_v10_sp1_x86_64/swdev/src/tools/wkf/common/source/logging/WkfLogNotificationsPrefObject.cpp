// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfLogNotificationsPrefObject.hpp"

wkf::log::NotificationsPrefObject::NotificationsPrefObject(QObject* aParent /* = nullptr */) noexcept
   : wkf::PrefObjectT<NotificationsPrefData>(aParent, cNAME)
{
}

wkf::log::NotificationLevel wkf::log::NotificationsPrefObject::GetEffectiveNotificationLevel() const noexcept
{
   return mCurrentPrefs.mNotificationLevel & ~mCurrentPrefs.mIgnoreNotifications;
}

void wkf::log::NotificationsPrefObject::SetIgnore(NotificationLevel aLevel)
{
   auto& ignore = mCurrentPrefs.mIgnoreNotifications;
   if ((ignore & aLevel) == NotificationLevel::None)
   {
      ignore = ignore | aLevel;
      emit EffectiveNotificationLevelChanged(GetEffectiveNotificationLevel());
   }
}

void wkf::log::NotificationsPrefObject::Apply()
{
   if (mEffectiveNotificationLevelChanged)
   {
      emit EffectiveNotificationLevelChanged(GetEffectiveNotificationLevel());
      mEffectiveNotificationLevelChanged = false;
   }
}

void wkf::log::NotificationsPrefObject::SetPreferenceDataP(const NotificationsPrefData& aPrefData)
{
   if (mCurrentPrefs.mNotificationLevel != aPrefData.mNotificationLevel ||
       mCurrentPrefs.mIgnoreNotifications != aPrefData.mIgnoreNotifications)
   {
      mEffectiveNotificationLevelChanged = true;
   }

   mCurrentPrefs = aPrefData;
}

wkf::log::NotificationsPrefData wkf::log::NotificationsPrefObject::ReadSettings(QSettings& aSettings) const
{
   const int defaultLevel = static_cast<int>(mDefaultPrefs.mNotificationLevel);
   int       level        = aSettings.value("notificationLevel", defaultLevel).toInt();

   NotificationsPrefData retval;
   retval.mNotificationLevel = static_cast<NotificationLevel>(level) | NotificationLevel::Fatal;
   return retval;
}

void wkf::log::NotificationsPrefObject::SaveSettingsP(QSettings& aSettings) const
{
   aSettings.setValue("notificationLevel", static_cast<int>(mCurrentPrefs.mNotificationLevel));
}
