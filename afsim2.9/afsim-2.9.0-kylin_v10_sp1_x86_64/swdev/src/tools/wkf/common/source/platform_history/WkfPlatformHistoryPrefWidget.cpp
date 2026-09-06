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
#include "WkfPlatformHistoryPrefWidget.hpp"

#include "WkfPlatformHistoryStateColorWidget.hpp"

wkf::PlatformHistoryPrefWidget::PlatformHistoryPrefWidget()
   : PrefWidgetT<PlatformHistoryPrefObject>()
{
   mUi.setupUi(this);
   mUi.tracelineColorCombo->addItem(QStringLiteral("team_color"), TraceStateValue::eTRACE_TEAM_COLOR);
   mUi.tracelineColorCombo->addItem(QStringLiteral("name"), TraceStateValue::eTRACE_NAME_HASH);

   mUi.colorSchemeComboBox->addItem(QStringLiteral("Team Color"), WingRibbonStyle::eTEAM);
   mUi.colorSchemeComboBox->addItem(QStringLiteral("Green Scale"), WingRibbonStyle::eGREEN);
   mUi.colorSchemeComboBox->addItem(QStringLiteral("Gray Scale"), WingRibbonStyle::eGRAY);

   connect(mUi.tracelineColorCombo,
           QOverload<const QString&>::of(&QComboBox::currentIndexChanged),
           this,
           &wkf::PlatformHistoryPrefWidget::ChangeTracelineSettings);

   AddTraceLineMode(QStringLiteral("state"), TraceStateValue::eTRACE_STATE, new PlatformHistoryStateColorWidget());
   ChangeTracelineSettings(mUi.tracelineColorCombo->currentText());
}

void wkf::PlatformHistoryPrefWidget::ChangeTracelineSettings(const QString& aTracelineSetting)
{
   for (auto& item : mTracelineSettings)
   {
      item.second->hide();
   }
   auto item = mTracelineSettings.find(aTracelineSetting);
   if (item != mTracelineSettings.end())
   {
      item->second->show();
   }
}

void wkf::PlatformHistoryPrefWidget::WritePreferenceData(TraceLinesPrefData& aPrefData)
{
   aPrefData.mTraceLineColor =
      static_cast<TraceStateValue>(mUi.tracelineColorCombo->itemData(mUi.tracelineColorCombo->currentIndex()).toInt());
   aPrefData.mTraceLineLength                    = mUi.tracelinesLengthLineEdit->text().toDouble();
   aPrefData.mTraceLineWidth                     = mUi.tracelineWidth->value();
   aPrefData.mWingRibbonsLength                  = mUi.wingRibbonLengthLineEdit->text().toDouble();
   aPrefData.mWingRibbonWidthScale               = mUi.wingRibbonWidthScaleField->text().toDouble();
   aPrefData.mWingRibbonDeathTransparencyPercent = mUi.deathTransparencySlider->value();
   aPrefData.mColorScheme = static_cast<WingRibbonStyle>(mUi.colorSchemeComboBox->currentData().toInt());

   for (auto& pref : mTracelineSettings)
   {
      pref.second->WritePreferenceData(aPrefData);
   }
}

void wkf::PlatformHistoryPrefWidget::AddTraceLineMode(const QString&              aModeName,
                                                      int                         aModeEnum,
                                                      PlatformHistoryColorWidget* aModeWidget)
{
   aModeWidget->setParent(this);
   mUi.formLayout->addRow(nullptr, aModeWidget);
   mUi.tracelineColorCombo->addItem(aModeName, aModeEnum);
   mTracelineSettings.emplace(aModeName, aModeWidget);
}

void wkf::PlatformHistoryPrefWidget::ReadPreferenceData(const TraceLinesPrefData& aPrefData)
{
   mUi.tracelineColorCombo->setCurrentIndex(mUi.tracelineColorCombo->findData(aPrefData.mTraceLineColor));
   ChangeTracelineSettings(mUi.tracelineColorCombo->currentText());
   mUi.tracelinesLengthLineEdit->setText(QString::number(aPrefData.mTraceLineLength));
   mUi.tracelineWidth->setValue(aPrefData.mTraceLineWidth);
   mUi.wingRibbonLengthLineEdit->setText(QString::number(aPrefData.mWingRibbonsLength));
   mUi.wingRibbonWidthScaleField->setText(QString::number(aPrefData.mWingRibbonWidthScale));
   mUi.deathTransparencySlider->setValue(aPrefData.mWingRibbonDeathTransparencyPercent);
   mUi.colorSchemeComboBox->setCurrentIndex(aPrefData.mColorScheme);

   for (auto& pref : mTracelineSettings)
   {
      pref.second->ReadPreferenceData(aPrefData);
   }
}
