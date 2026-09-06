// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "WkfGeneralPrefWidget.hpp"

#include "WkfEnvironment.hpp"
#include "WkfGeneralPrefObject.hpp"

wkf::GeneralPrefWidget::GeneralPrefWidget(QWidget* parent)
   : PrefWidgetT<GeneralPrefObject>(parent)
{
   mUi.setupUi(this);
   mUi.appBgColorPushButton->ShowAlpha(true);

   if (wkfEnv.GetApplicationName() != "warlock" && wkfEnv.GetApplicationName() != "mystic")
   {
      mUi.appTextSimNameCheckBox->hide();
   }
}

void wkf::GeneralPrefWidget::WritePreferenceData(GeneralPrefData& aPrefData)
{
   aPrefData.mThemeName             = mUi.mThemeBox->currentText();
   aPrefData.mShowStatusBar         = mUi.statusBarCheckBox->isChecked();
   aPrefData.mNumberOfRecentConfigs = mUi.recentConfigSpinBox->value();

   aPrefData.mAppBannerBgColor      = mUi.appBgColorPushButton->GetColor();
   aPrefData.mAppBannerColor        = mUi.appTextColorPushButton->GetColor();
   aPrefData.mAppBannerFont         = mUi.appFontSpinBox->value();
   aPrefData.mAppBannerText         = mUi.appTextLineEdit->text();
   aPrefData.mUseSimNameAsAppBanner = mUi.appTextSimNameCheckBox->isChecked();
   aPrefData.mShowAppBanner         = mUi.applicationGroupBox->isChecked();

   aPrefData.mOverlayBannerFont         = mUi.overlayFontSpinBox->value();
   aPrefData.mShowClassificationOverlay = mUi.overlayGroupBox->isChecked();
}

void wkf::GeneralPrefWidget::showEvent(QShowEvent*)
{
   // This is necessary because the pref widget isn't constructed until after settings are loaded into pref object
   ReadPreferenceData(mPrefObjectPtr->GetPreferences());
}

void wkf::GeneralPrefWidget::ReadPreferenceData(const GeneralPrefData& aPrefData)
{
   mUi.mThemeBox->clear();
   mUi.mThemeBox->addItems(mPrefObjectPtr->GetPaletteKeys());
   mUi.mThemeBox->setCurrentText(aPrefData.mThemeName);

   mUi.statusBarCheckBox->setChecked(aPrefData.mShowStatusBar);
   mUi.recentConfigSpinBox->setValue(aPrefData.mNumberOfRecentConfigs);

   mUi.appBgColorPushButton->SetColor(aPrefData.mAppBannerBgColor);
   mUi.appTextColorPushButton->SetColor(aPrefData.mAppBannerColor);
   mUi.appFontSpinBox->setValue(aPrefData.mAppBannerFont);
   mUi.appTextLineEdit->setText(aPrefData.mAppBannerText);
   mUi.appTextSimNameCheckBox->setChecked(aPrefData.mUseSimNameAsAppBanner);
   mUi.applicationGroupBox->setChecked(aPrefData.mShowAppBanner);

   mUi.overlayFontSpinBox->setValue(aPrefData.mOverlayBannerFont);
   mUi.overlayGroupBox->setChecked(aPrefData.mShowClassificationOverlay);
}
