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

#include "WkfDebugPrefObject.hpp"

#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"

wkf::DebugPrefObject::DebugPrefObject(QObject* parent /*= nullptr*/)
   : PrefObjectT<DebugPrefData>(parent, cNAME)
{
}

void wkf::DebugPrefObject::Apply()
{
   emit DeveloperMenuVisibilityChanged(mCurrentPrefs.mDeveloperMenu);
}

wkf::DebugPrefData wkf::DebugPrefObject::ReadSettings(QSettings& aSettings) const
{
   DebugPrefData pData;
   pData.mDeveloperMenu = aSettings.value("enabled", mDefaultPrefs.mDeveloperMenu).toBool();
   return pData;
}

void wkf::DebugPrefObject::SaveSettingsP(QSettings& aSettings) const
{
   aSettings.setValue("enabled", mCurrentPrefs.mDeveloperMenu);
}
