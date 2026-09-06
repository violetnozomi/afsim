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

#include "MapHoverInfoPrefWidget.hpp"

#include "WkfEnvironment.hpp"
#include "WkfPlugin.hpp"
#include "WkfPluginManager.hpp"

const QStringList cPOSITION_DATA   = {"Latitude", "Longitude", "Altitude", "Position"};
const QStringList cEVENT_BASE_DATA = {"Event Type", "Time"};

MapHoverInfo::PrefWidget::PrefWidget()
   : wkf::PrefWidgetT<PrefObject>()
{
   mUi.setupUi(this);

   if (wkfEnv.GetApplicationName() == "wizard")
   {
      mUi.trackHoverInfo->hide();
      mUi.platformEventHoverInfo->hide();
      mUi.weaponEventHoverInfo->hide();

      mUi.widgetLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
   }

   mUi.platformHoverInfo->setTitle("Platforms");
   mUi.trackHoverInfo->setTitle("Tracks");
   mUi.platformEventHoverInfo->setTitle("Platform Events");
   mUi.weaponEventHoverInfo->setTitle("Weapon Events");

   mUi.trackHoverInfo->SetAvailableOptions(cPOSITION_DATA);
   mUi.trackHoverInfo->AddOptions(
      {"Bearing (Bullseye)", "Heading", "Mach", "Range (Bullseye)", "Side", "Spatial Domain", "Speed", "Type"});

   mUi.platformEventHoverInfo->SetAvailableOptions(cPOSITION_DATA + cEVENT_BASE_DATA);
   mUi.platformEventHoverInfo->AddOption("Platform");

   mUi.weaponEventHoverInfo->SetAvailableOptions(cPOSITION_DATA + cEVENT_BASE_DATA);
   mUi.weaponEventHoverInfo->AddOptions({"Firing Platform",
                                         "Weapon Platform",
                                         "Target Platform",
                                         "Result",
                                         "Intercept PK",
                                         "PK Drawn",
                                         "Miss Distance",
                                         "Weapon Speed",
                                         "Target Speed"});
}

void MapHoverInfo::PrefWidget::ReadPreferenceData(const PrefData& aPrefData)
{
   // Setting platform options here because updaters are not registered when the plugin is initialized.
   mUi.platformHoverInfo->SetAvailableOptions(wkfEnv.GetUpdaterList());

   mUi.enableHoverInfoGroupBox->setChecked(aPrefData.mShowTooltips);
   mUi.showNamesCheckBox->setChecked(aPrefData.mShowNames);
   mUi.showLabelsCheckBox->setChecked(aPrefData.mShowItemLabels);

   mUi.platformHoverInfo->SetSelectedOptions(aPrefData.mPlatformUpdaterList);
   mUi.trackHoverInfo->SetSelectedOptions(aPrefData.mTrackUpdaterList);

   mUi.platformEventHoverInfo->SetSelectedOptions(aPrefData.mPlatformEventList);
   mUi.weaponEventHoverInfo->SetSelectedOptions(aPrefData.mWeaponEventList);
}

void MapHoverInfo::PrefWidget::WritePreferenceData(PrefData& aPrefData)
{
   aPrefData.mShowTooltips   = mUi.enableHoverInfoGroupBox->isChecked();
   aPrefData.mShowNames      = mUi.showNamesCheckBox->isChecked();
   aPrefData.mShowItemLabels = mUi.showLabelsCheckBox->isChecked();

   aPrefData.mPlatformUpdaterList = mUi.platformHoverInfo->GetSelectedOptions();
   aPrefData.mTrackUpdaterList    = mUi.trackHoverInfo->GetSelectedOptions();

   aPrefData.mPlatformEventList = mUi.platformEventHoverInfo->GetSelectedOptions();
   aPrefData.mWeaponEventList   = mUi.weaponEventHoverInfo->GetSelectedOptions();
}
