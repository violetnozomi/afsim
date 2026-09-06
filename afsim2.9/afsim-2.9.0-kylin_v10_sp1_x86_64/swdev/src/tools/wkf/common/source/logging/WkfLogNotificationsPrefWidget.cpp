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

#include "WkfLogNotificationsPrefWidget.hpp"

wkf::log::NotificationsPrefWidget::NotificationsPrefWidget() noexcept
   : wkf::PrefWidgetT<NotificationsPrefObject>(nullptr)
{
   mUi.setupUi(this);
   setWindowTitle(cPREF_WIDGET_TITLE);
}

QString wkf::log::NotificationsPrefWidget::GetCategoryHint() const
{
   return "Simulation Logs";
}

void wkf::log::NotificationsPrefWidget::showEvent(QShowEvent* aEvent)
{
   ReadPreferenceData(mPrefObjectPtr->GetPreferences());
}

void wkf::log::NotificationsPrefWidget::ReadPreferenceData(const NotificationsPrefData& aPrefData)
{
   switch (aPrefData.mNotificationLevel)
   {
   case NotificationLevel::Fatal:
   default:
      mUi.notificationLevel1->setChecked(true);
      break;
   case NotificationLevel::FatalError:
      mUi.notificationLevel2->setChecked(true);
      break;
   case NotificationLevel::Everything:
      mUi.notificationLevel3->setChecked(true);
      break;
   }

   mUi.ignoreErrors->setChecked((aPrefData.mIgnoreNotifications & NotificationLevel::Error) != NotificationLevel::None);
   mUi.ignoreWarnings->setChecked((aPrefData.mIgnoreNotifications & NotificationLevel::Warning) != NotificationLevel::None);
}

void wkf::log::NotificationsPrefWidget::WritePreferenceData(NotificationsPrefData& aPrefData)
{
   if (mUi.notificationLevel1->isChecked())
   {
      aPrefData.mNotificationLevel = NotificationLevel::Fatal;
   }
   else if (mUi.notificationLevel2->isChecked())
   {
      aPrefData.mNotificationLevel = NotificationLevel::FatalError;
   }
   else
   {
      aPrefData.mNotificationLevel = NotificationLevel::Everything;
   }

   aPrefData.mIgnoreNotifications = NotificationLevel::None;
   if (mUi.ignoreErrors->isChecked())
   {
      aPrefData.mIgnoreNotifications = aPrefData.mIgnoreNotifications | NotificationLevel::Error;
   }
   if (mUi.ignoreWarnings->isChecked())
   {
      aPrefData.mIgnoreNotifications = aPrefData.mIgnoreNotifications | NotificationLevel::Warning;
   }
}
