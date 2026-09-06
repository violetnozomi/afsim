// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfWsfDrawPrefObject.hpp"

wkf::WsfDrawPrefData wkf::WsfDrawPrefObject::ReadSettings(QSettings& aSettings) const
{
   WsfDrawPrefData        prefs;
   const WsfDrawPrefData& defaults = mDefaultPrefs;

   prefs.mExtendedDraw = aSettings.value("ExtendedBrowser", defaults.mExtendedDraw).toBool();

   emit ExtendedBrowserChanged(prefs.mExtendedDraw);
   return prefs;
}

void wkf::WsfDrawPrefObject::SaveSettingsP(QSettings& aSettings) const
{
   aSettings.setValue("ExtendedBrowser", mCurrentPrefs.mExtendedDraw);
}
