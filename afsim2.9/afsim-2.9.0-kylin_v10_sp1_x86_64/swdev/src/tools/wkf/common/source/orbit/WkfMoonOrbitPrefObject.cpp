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

#include "WkfMoonOrbitPrefObject.hpp"

#include <QSettings>

namespace
{
const QString cVISIBILITY = "orbitVisibility";
const QString cCOLOR      = "orbitColor";
const QString cWIDTH      = "orbitLineWidth";
} // namespace

wkf::MoonOrbitPrefObject::MoonOrbitPrefObject(QObject* parent)
   : wkf::PrefObjectT<wkf::MoonOrbitPrefData>(parent, "LunarOrbitOptions")
{
}

void wkf::MoonOrbitPrefObject::SetVisibility(bool aVisible)
{
   mCurrentPrefs.mVisible = aVisible;
   emit VisibilityChanged(aVisible);
}

void wkf::MoonOrbitPrefObject::SetColor(const QColor& aColor)
{
   mCurrentPrefs.mColor = aColor;
   emit ColorChanged(aColor);
}

void wkf::MoonOrbitPrefObject::SetLineWidth(int aWidth)
{
   mCurrentPrefs.mLineWidth = aWidth;
   emit LineWidthChanged(aWidth);
}

void wkf::MoonOrbitPrefObject::EmitAllSignals()
{
   emit VisibilityChanged(mCurrentPrefs.mVisible);
   emit ColorChanged(mCurrentPrefs.mColor);
   emit LineWidthChanged(mCurrentPrefs.mLineWidth);
}

void wkf::MoonOrbitPrefObject::Apply()
{
   EmitAllSignals();
}

wkf::MoonOrbitPrefData wkf::MoonOrbitPrefObject::ReadSettings(QSettings& aSettings) const
{
   MoonOrbitPrefData pData;
   pData.mVisible   = aSettings.value(cVISIBILITY, mDefaultPrefs.mVisible).toBool();
   pData.mColor     = aSettings.value(cCOLOR, mDefaultPrefs.mColor).value<QColor>();
   pData.mLineWidth = aSettings.value(cWIDTH, mDefaultPrefs.mLineWidth).toInt();
   return pData;
}

void wkf::MoonOrbitPrefObject::SaveSettingsP(QSettings& aSettings) const
{
   aSettings.setValue(cVISIBILITY, mCurrentPrefs.mVisible);
   aSettings.setValue(cCOLOR, mCurrentPrefs.mColor);
   aSettings.setValue(cWIDTH, mCurrentPrefs.mLineWidth);
}
