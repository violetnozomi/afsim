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
#include "WkfOrbitPrefObject.hpp"

#include <QSettings>

wkf::OrbitPrefObject::OrbitPrefObject(QObject* parent)
   : wkf::PrefObjectT<wkf::OrbitPrefData>(parent, cNAME)
{
}

void wkf::OrbitPrefObject::SetFadeout(int aFadeout)
{
   mCurrentPrefs.mFadeout = aFadeout;
   emit FadeoutChanged(aFadeout);
}

void wkf::OrbitPrefObject::SetColorMode(OrbitPrefData::ColorMode aMode)
{
   mCurrentPrefs.mColorMode = aMode;
   emit ColorModeChanged(aMode);
}

void wkf::OrbitPrefObject::SetLineWidth(int aLineWidth)
{
   mCurrentPrefs.mLineWidth = aLineWidth;
   emit LineWidthChanged(aLineWidth);
}

void wkf::OrbitPrefObject::SetPeriods(unsigned int aPeriods)
{
   mCurrentPrefs.mPeriods = aPeriods;
   emit PeriodsChanged(aPeriods);
}

void wkf::OrbitPrefObject::EmitAllSignals()
{
   emit FadeoutChanged(mCurrentPrefs.mFadeout);
   emit ColorModeChanged(mCurrentPrefs.mColorMode);
   emit LineWidthChanged(mCurrentPrefs.mLineWidth);
   emit PeriodsChanged(mCurrentPrefs.mPeriods);
}

void wkf::OrbitPrefObject::Apply()
{
   EmitAllSignals();
}

wkf::OrbitPrefData wkf::OrbitPrefObject::ReadSettings(QSettings& aSettings) const
{
   OrbitPrefData pData;
   pData.mFadeout = aSettings.value("fadeout", mDefaultPrefs.mFadeout).toInt();
   pData.mColorMode =
      static_cast<OrbitPrefData::ColorMode>(aSettings.value("colormode", mDefaultPrefs.mColorMode).toInt());
   pData.mLineWidth = aSettings.value("linewidth", mDefaultPrefs.mLineWidth).toInt();
   pData.mPeriods   = aSettings.value("periods", mDefaultPrefs.mPeriods).toUInt();
   return pData;
}

void wkf::OrbitPrefObject::SaveSettingsP(QSettings& aSettings) const
{
   aSettings.setValue("fadeout", mCurrentPrefs.mFadeout);
   aSettings.setValue("colormode", mCurrentPrefs.mColorMode);
   aSettings.setValue("linewidth", mCurrentPrefs.mLineWidth);
   aSettings.setValue("periods", mCurrentPrefs.mPeriods);
}
