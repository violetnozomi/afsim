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
#include "RvPlatformBrowserPrefObject.hpp"

#include <QSettings>

#include "WkfEnvironment.hpp"

RvPlatformBrowser::PrefObject::PrefObject(QObject* parent)
   : wkf::PrefObjectT<PrefData>(parent, "PlatformBrowser")
{
}

void RvPlatformBrowser::PrefObject::Apply()
{
   emit ShowInactiveChanged(mCurrentPrefs.mShowInactive);
   emit PreferencesChanged(QVariant::fromValue(mCurrentPrefs));
}

RvPlatformBrowser::PrefData RvPlatformBrowser::PrefObject::ReadSettings(QSettings& aSettings) const
{
   PrefData        pData;
   const PrefData& defaults = mDefaultPrefs;

   pData.mShowInactive = aSettings.value("showInactive", defaults.mShowInactive).toBool();

   return pData;
}

void RvPlatformBrowser::PrefObject::SaveSettingsP(QSettings& aSettings) const
{
   aSettings.setValue("showInactive", mCurrentPrefs.mShowInactive);
}
