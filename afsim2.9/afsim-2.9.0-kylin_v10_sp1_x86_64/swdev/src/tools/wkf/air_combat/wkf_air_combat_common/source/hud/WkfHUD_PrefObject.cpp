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

#include "WkfHUD_PrefObject.hpp"

#include <QSettings>

wkf::HUD_PrefObject::HUD_PrefObject(QObject* parent)
   : wkf::PrefObjectT<wkf::HUD_PrefData>(parent, "WkfHUDOptions")
{
}

void wkf::HUD_PrefObject::SetResolution(unsigned int aResX, unsigned int aResY)
{
   mCurrentPrefs.mResX = aResX;
   mCurrentPrefs.mResY = aResY;
   emit ResolutionChanged(aResX, aResY);
}

void wkf::HUD_PrefObject::EmitAllSignals()
{
   emit ResolutionChanged(mCurrentPrefs.mResX, mCurrentPrefs.mResY);
}

void wkf::HUD_PrefObject::Apply()
{
   EmitAllSignals();
}

wkf::HUD_PrefData wkf::HUD_PrefObject::ReadSettings(QSettings& aSettings) const
{
   wkf::HUD_PrefData pData;
   pData.mResX = aSettings.value("resx", mDefaultPrefs.mResX).toUInt();
   pData.mResY = aSettings.value("resy", mDefaultPrefs.mResY).toUInt();
   return pData;
}

void wkf::HUD_PrefObject::SaveSettingsP(QSettings& aSettings) const
{
   aSettings.setValue("resx", mCurrentPrefs.mResX);
   aSettings.setValue("resy", mCurrentPrefs.mResY);
}
