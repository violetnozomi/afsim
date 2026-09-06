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

#include "TetherViewPrefObject.hpp"

#include <QSettings>

TetherView::PrefObject::PrefObject(QObject* parent)
   : wkf::PrefObjectT<PrefData>(parent, "TetherViewOptions")
{
}

void TetherView::PrefObject::Apply()
{
   if (mLockCameraToHeadingChanged)
   {
      emit HeadingLockEnabledChanged(mCurrentPrefs.mLockCameraToHeadingEnabled);
      mLockCameraToHeadingChanged = false;
   }
   if (mLightingChanged)
   {
      emit LightingEnabledChanged(mCurrentPrefs.mEnableLighting);
      mLightingChanged = false;
   }
}

TetherView::PrefData TetherView::PrefObject::ReadSettings(QSettings& aSettings) const
{
   PrefData pData;
   pData.mLockCameraToHeadingEnabled =
      aSettings.value("headingLockEnabled", mDefaultPrefs.mLockCameraToHeadingEnabled).toBool();
   pData.mEnableLighting = aSettings.value("lightingEnabled", mDefaultPrefs.mEnableLighting).toBool();
   return pData;
}

void TetherView::PrefObject::SetPreferenceDataP(const PrefData& aPrefData)
{
   mLockCameraToHeadingChanged = mCurrentPrefs.mLockCameraToHeadingEnabled != aPrefData.mLockCameraToHeadingEnabled;
   mLightingChanged            = mCurrentPrefs.mEnableLighting != aPrefData.mEnableLighting;
   BasePrefObject::SetPreferenceDataP(aPrefData);
}

void TetherView::PrefObject::SaveSettingsP(QSettings& aSettings) const
{
   aSettings.setValue("headingLockEnabled", mCurrentPrefs.mLockCameraToHeadingEnabled);
   aSettings.setValue("lightingEnabled", mCurrentPrefs.mEnableLighting);
}
