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

#include "WkfZoneBrowserPrefObject.hpp"

namespace
{
const QString cCOLORCHOICE_KEY = "colorChoice";
const QString cDEFAULTLINE_KEY = "lineColor";
const QString cDEFAULTFILL_KEY = "fillColor";
} // namespace

wkf::ZoneBrowserPrefObject::ZoneBrowserPrefObject(QObject* aParentPtr)
   : wkf::PrefObjectT<ZoneBrowserPrefData>(aParentPtr, "ZoneBrowserOptions")
   , mColorChoiceChanged(false)
   , mLineColorChanged(false)
   , mFillColorChanged(false)
{
}

void wkf::ZoneBrowserPrefObject::Apply()
{
   if (mColorChoiceChanged)
   {
      emit ColorChoiceChanged(mCurrentPrefs.mColorChoice);
   }
   if (mLineColorChanged)
   {
      emit DefaultLineColorChanged(mCurrentPrefs.mDefaultLineColor);
   }
   if (mFillColorChanged)
   {
      emit DefaultFillColorChanged(mCurrentPrefs.mDefaultFillColor);
   }
}

void wkf::ZoneBrowserPrefObject::SetPreferenceDataP(const wkf::ZoneBrowserPrefData& aPrefData)
{
   mColorChoiceChanged = aPrefData.mColorChoice != mCurrentPrefs.mColorChoice;
   mLineColorChanged   = aPrefData.mDefaultLineColor != mCurrentPrefs.mDefaultLineColor;
   mFillColorChanged   = aPrefData.mDefaultFillColor != mCurrentPrefs.mDefaultFillColor;
   BasePrefObject::SetPreferenceDataP(aPrefData);
}

wkf::ZoneBrowserPrefData wkf::ZoneBrowserPrefObject::ReadSettings(QSettings& aSettings) const
{
   ZoneBrowserPrefData pData;
   pData.mColorChoice      = aSettings.value(cCOLORCHOICE_KEY, mDefaultPrefs.mColorChoice).value<wkf::ColorOption>();
   pData.mDefaultLineColor = aSettings.value(cDEFAULTLINE_KEY, mDefaultPrefs.mDefaultLineColor).value<QColor>();
   pData.mDefaultFillColor = aSettings.value(cDEFAULTFILL_KEY, mDefaultPrefs.mDefaultFillColor).value<QColor>();
   return pData;
}

void wkf::ZoneBrowserPrefObject::SaveSettingsP(QSettings& aSettings) const
{
   aSettings.setValue(cCOLORCHOICE_KEY, mCurrentPrefs.mColorChoice);
   aSettings.setValue(cDEFAULTLINE_KEY, mCurrentPrefs.mDefaultLineColor);
   aSettings.setValue(cDEFAULTFILL_KEY, mCurrentPrefs.mDefaultFillColor);
}
