// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfSatelliteTetherPrefObject.hpp"

wkf::SatelliteTetherPrefObject::SatelliteTetherPrefObject(QObject* parent /*= nullptr*/)
   : PrefObjectT<wkf::SatelliteTetherPrefData>(parent, cNAME)
{
}

void wkf::SatelliteTetherPrefObject::SetShowFutureTrack(bool aShow)
{
   mCurrentPrefs.mShowFutureTrack = aShow;
   emit ShowFutureTrackChanged(aShow);
}

void wkf::SatelliteTetherPrefObject::SetShowNames(bool aShow)
{
   mCurrentPrefs.mShowNames = aShow;
   emit ShowNamesChanged(aShow);
}

void wkf::SatelliteTetherPrefObject::SetLightingEnabled(bool aState)
{
   mCurrentPrefs.mEnableLighting = aState;
   emit LightingChanged(aState);
}

void wkf::SatelliteTetherPrefObject::EmitAllSignals()
{
   emit ShowFutureTrackChanged(mCurrentPrefs.mShowFutureTrack);
   emit ShowNamesChanged(mCurrentPrefs.mShowNames);
   emit LightingChanged(mCurrentPrefs.mEnableLighting);
}

void wkf::SatelliteTetherPrefObject::Apply()
{
   EmitAllSignals();
}

wkf::SatelliteTetherPrefData wkf::SatelliteTetherPrefObject::ReadSettings(QSettings& aSettings) const
{
   SatelliteTetherPrefData pData;
   pData.mShowFutureTrack = aSettings.value("showFutureTrack", mDefaultPrefs.mShowFutureTrack).toBool();
   pData.mShowNames       = aSettings.value("showNames", mDefaultPrefs.mShowNames).toBool();
   pData.mEnableLighting  = aSettings.value("enableLighting", mDefaultPrefs.mEnableLighting).toBool();
   return pData;
}

void wkf::SatelliteTetherPrefObject::SaveSettingsP(QSettings& aSettings) const
{
   aSettings.setValue("showFutureTrack", mCurrentPrefs.mShowFutureTrack);
   aSettings.setValue("showNames", mCurrentPrefs.mShowNames);
   aSettings.setValue("enableLighting", mCurrentPrefs.mEnableLighting);
}
