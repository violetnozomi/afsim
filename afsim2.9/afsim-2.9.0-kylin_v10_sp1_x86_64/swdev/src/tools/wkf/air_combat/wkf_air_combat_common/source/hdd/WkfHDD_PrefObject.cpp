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
#include "WkfHDD_PrefObject.hpp"

#include <QSettings>

HDD::PrefObject::PrefObject(QObject* parent)
   : wkf::PrefObjectT<PrefData>(parent, "HeadDownViewOptions")
{
}

QStringList const HDD::PrefObject::GetPageList() const
{
   // Return a list of the current pages.  The dock widget uses this when it initializes.
   QStringList list = QStringList();
   list.append(mCurrentPrefs.mPage1);
   list.append(mCurrentPrefs.mPage2);
   list.append(mCurrentPrefs.mPage3);
   list.append(mCurrentPrefs.mPage4);
   list.append(mCurrentPrefs.mPage5);
   list.append(mCurrentPrefs.mPage6);
   list.append(mCurrentPrefs.mPage7);
   list.append(mCurrentPrefs.mPage8);
   list.append(mCurrentPrefs.mPage9);
   list.append(mCurrentPrefs.mPage10);
   list.append(mCurrentPrefs.mPage11);
   list.append(mCurrentPrefs.mPage12);
   return list;
}

void HDD::PrefObject::SetResolution(unsigned int aResX, unsigned int aResY)
{
   mCurrentPrefs.mResX = aResX;
   mCurrentPrefs.mResY = aResY;
   emit ResolutionChanged(aResX, aResY);
}

void HDD::PrefObject::EmitAllSignals()
{
   emit ResolutionChanged(mCurrentPrefs.mResX, mCurrentPrefs.mResY);
}

void HDD::PrefObject::Apply()
{
   EmitAllSignals();
}

HDD::PrefData HDD::PrefObject::ReadSettings(QSettings& aSettings) const
{
   PrefData pData;

   // Resolution
   pData.mResX = aSettings.value("resx", mDefaultPrefs.mResX).toUInt();
   pData.mResY = aSettings.value("resy", mDefaultPrefs.mResY).toUInt();

   // Pages
   pData.mPage1  = aSettings.value("page_1", mDefaultPrefs.mPage1).toString();
   pData.mPage2  = aSettings.value("page_2", mDefaultPrefs.mPage2).toString();
   pData.mPage3  = aSettings.value("page_3", mDefaultPrefs.mPage3).toString();
   pData.mPage4  = aSettings.value("page_4", mDefaultPrefs.mPage4).toString();
   pData.mPage5  = aSettings.value("page_5", mDefaultPrefs.mPage5).toString();
   pData.mPage6  = aSettings.value("page_6", mDefaultPrefs.mPage6).toString();
   pData.mPage7  = aSettings.value("page_7", mDefaultPrefs.mPage7).toString();
   pData.mPage8  = aSettings.value("page_8", mDefaultPrefs.mPage8).toString();
   pData.mPage9  = aSettings.value("page_9", mDefaultPrefs.mPage9).toString();
   pData.mPage10 = aSettings.value("page_10", mDefaultPrefs.mPage10).toString();
   pData.mPage11 = aSettings.value("page_11", mDefaultPrefs.mPage11).toString();
   pData.mPage12 = aSettings.value("page_12", mDefaultPrefs.mPage12).toString();

   return pData;
}

void HDD::PrefObject::SaveSettingsP(QSettings& aSettings) const
{
   // Resolution
   aSettings.setValue("resx", mCurrentPrefs.mResX);
   aSettings.setValue("resy", mCurrentPrefs.mResY);

   // Pages
   aSettings.setValue("page_1", mCurrentPrefs.mPage1);
   aSettings.setValue("page_2", mCurrentPrefs.mPage2);
   aSettings.setValue("page_3", mCurrentPrefs.mPage3);
   aSettings.setValue("page_4", mCurrentPrefs.mPage4);
   aSettings.setValue("page_5", mCurrentPrefs.mPage5);
   aSettings.setValue("page_6", mCurrentPrefs.mPage6);
   aSettings.setValue("page_7", mCurrentPrefs.mPage7);
   aSettings.setValue("page_8", mCurrentPrefs.mPage8);
   aSettings.setValue("page_9", mCurrentPrefs.mPage9);
   aSettings.setValue("page_10", mCurrentPrefs.mPage10);
   aSettings.setValue("page_11", mCurrentPrefs.mPage11);
   aSettings.setValue("page_12", mCurrentPrefs.mPage12);
}
