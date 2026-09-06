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
#include "WkfTrackVisibilityPrefWidget.hpp"

#include <QFontDialog>

#include "WkfEnvironment.hpp"
#include "WkfTeamVisibilityPrefObject.hpp"
#include "WkfTrackVisibilityPrefObject.hpp"


wkf::TrackVisibilityPrefWidget::TrackVisibilityPrefWidget(QWidget* aParent)
   : PrefWidgetT<TrackVisibilityPrefObject>(aParent)
{
   mUi.setupUi(this);
   mUi.trackFontButton->SetFont(QFont());
   mUi.trackFontButton->setWindowTitle("Choose Track Label Font");
   mUi.showNetworkedSimulationsTracks->hide();
   connect(mUi.showTeamTracksRadioButton, &QRadioButton::toggled, mUi.showTeamTracksComboBox, &QWidget::setEnabled);
   connect(mUi.customIconRadioButton, &QRadioButton::toggled, mUi.customImageSelector, &ImageSelectionWidget::SetEnabledUpdate);
   connect(mUi.showNoTracksRadioButton,
           &QRadioButton::toggled,
           this,
           [this]() { mUi.iconAppearanceBox->setEnabled(!mUi.showNoTracksRadioButton->isChecked()); });
   connect(mUi.rulesRadioButton, &QRadioButton::toggled, mUi.rulesGroupBox, &QWidget::setEnabled);
}

void wkf::TrackVisibilityPrefWidget::WritePreferenceData(TrackVisibilityPrefData& aPrefData)
{
   aPrefData.mSelectedTeam.clear();
   if (mUi.showLocalTracksRadioButton->isChecked())
   {
      aPrefData.mTrackVisibility = tracks::Visibility::cLOCAL_ONLY;
   }
   else if (mUi.showAllTracksRadioButton->isChecked())
   {
      aPrefData.mTrackVisibility = tracks::Visibility::cALL_VISIBLE_TEAMS;
   }
   else if (mUi.showTeamTracksRadioButton->isChecked())
   {
      aPrefData.mTrackVisibility = tracks::Visibility::cSELECTED_TEAM;
      aPrefData.mSelectedTeam    = mUi.showTeamTracksComboBox->currentText();
   }
   else if (mUi.showNoTracksRadioButton->isChecked())
   {
      aPrefData.mTrackVisibility = tracks::Visibility::cNONE;
   }

   aPrefData.mShowRemoteTracks = mUi.showNetworkedSimulationsTracks->isChecked();
   aPrefData.mShowColor        = mUi.trackColorCheckbox->isChecked();
   aPrefData.mShowLabel        = mUi.trackLabelCheckbox->isChecked();
   aPrefData.mShowDirection    = mUi.trackOrientationCheckbox->isChecked();
   aPrefData.mTrackScale       = mUi.scaleSlider->value();
   aPrefData.mFont             = mUi.trackFontButton->GetFont();
   aPrefData.mCustomIcon       = mUi.customImageSelector->GetText();
   aPrefData.mUsePlatformIcon  = mUi.platformIconCheckBox->isChecked();

   aPrefData.mDomainIcons[SPATIAL_DOMAIN_LAND]       = mUi.landImageSelector->GetText();
   aPrefData.mDomainIcons[SPATIAL_DOMAIN_AIR]        = mUi.airImageSelector->GetText();
   aPrefData.mDomainIcons[SPATIAL_DOMAIN_SURFACE]    = mUi.surfaceImageSelector->GetText();
   aPrefData.mDomainIcons[SPATIAL_DOMAIN_SUBSURFACE] = mUi.subsurfaceImageSelector->GetText();
   aPrefData.mDomainIcons[SPATIAL_DOMAIN_SPACE]      = mUi.spaceImageSelector->GetText();
   aPrefData.mDomainIcons[SPATIAL_DOMAIN_UNKNOWN]    = mUi.unknownImageSelector->GetText();

   if (mUi.customIconRadioButton->isChecked() && mUi.customImageSelector->IsValid())
   {
      aPrefData.mIconType = tracks::IconType::cCUSTOM_ICON;
   }
   else if (mUi.rulesRadioButton->isChecked())
   {
      aPrefData.mIconType = tracks::IconType::cRULES_ICON;
   }
   else
   {
      // Needed in case custom image path is invalid.
      mUi.wedgeRadioButton->setChecked(true);
      aPrefData.mIconType = tracks::IconType::cWEDGE_ICON;
   }
}

void wkf::TrackVisibilityPrefWidget::UpdateSelectedTeamCombo()
{
   QString curText = mUi.showTeamTracksComboBox->currentText();
   mUi.showTeamTracksComboBox->clear();
   auto teamVisibility = wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>()->GetTeamVisibilityMap();
   for (const auto& team : teamVisibility)
   {
      mUi.showTeamTracksComboBox->addItem(team.first);
   }
   mUi.showTeamTracksComboBox->setCurrentIndex(mUi.showTeamTracksComboBox->findText(curText));
}

void wkf::TrackVisibilityPrefWidget::showEvent(QShowEvent*)
{
   // This is necessary because the pref widget isn't constructed until after settings are loaded into pref object
   ReadPreferenceData(mPrefObjectPtr->GetPreferences());
   UpdateSelectedTeamCombo();
}

void wkf::TrackVisibilityPrefWidget::ReadPreferenceData(const TrackVisibilityPrefData& aPrefData)
{
   switch (aPrefData.mTrackVisibility)
   {
   case tracks::Visibility::cLOCAL_ONLY:
      mUi.showLocalTracksRadioButton->setChecked(true);
      break;
   case tracks::Visibility::cALL_VISIBLE_TEAMS:
      mUi.showAllTracksRadioButton->setChecked(true);
      break;
   case tracks::Visibility::cSELECTED_TEAM:
      mUi.showTeamTracksRadioButton->setChecked(true);
      break;
   default:
      mUi.showNoTracksRadioButton->setChecked(true);
   }

   mUi.showTeamTracksComboBox->setEnabled(aPrefData.mTrackVisibility == tracks::Visibility::cSELECTED_TEAM);

   UpdateSelectedTeamCombo();
   int index = mUi.showTeamTracksComboBox->findText(aPrefData.mSelectedTeam);
   if (index >= 0)
   {
      mUi.showTeamTracksComboBox->setCurrentIndex(index);
   }
   else
   {
      mUi.showTeamTracksComboBox->setCurrentIndex(0);
   }

   mUi.showNetworkedSimulationsTracks->setChecked(aPrefData.mShowRemoteTracks);

   switch (aPrefData.mIconType)
   {
   case tracks::IconType::cCUSTOM_ICON:
      mUi.customIconRadioButton->setChecked(true);
      mUi.customImageSelector->setEnabled(true);
      mUi.rulesGroupBox->setEnabled(false);
      break;
   case tracks::IconType::cRULES_ICON:
      mUi.rulesRadioButton->setChecked(true);
      mUi.customImageSelector->setEnabled(false);
      mUi.rulesGroupBox->setEnabled(true);
      break;
   default:
      mUi.wedgeRadioButton->setChecked(true);
      mUi.customImageSelector->setEnabled(false);
      mUi.rulesGroupBox->setEnabled(false);
   }

   mUi.trackColorCheckbox->setChecked(aPrefData.mShowColor);
   mUi.trackLabelCheckbox->setChecked(aPrefData.mShowLabel);
   mUi.trackOrientationCheckbox->setChecked(aPrefData.mShowDirection);
   mUi.scaleSlider->setValue(aPrefData.mTrackScale);
   mUi.trackFontButton->SetFont(aPrefData.mFont);
   mUi.customImageSelector->SetText(aPrefData.mCustomIcon);
   mUi.platformIconCheckBox->setChecked(aPrefData.mUsePlatformIcon);

   mUi.landImageSelector->SetText(aPrefData.mDomainIcons[SPATIAL_DOMAIN_LAND]);
   mUi.airImageSelector->SetText(aPrefData.mDomainIcons[SPATIAL_DOMAIN_AIR]);
   mUi.surfaceImageSelector->SetText(aPrefData.mDomainIcons[SPATIAL_DOMAIN_SURFACE]);
   mUi.subsurfaceImageSelector->SetText(aPrefData.mDomainIcons[SPATIAL_DOMAIN_SUBSURFACE]);
   mUi.spaceImageSelector->SetText(aPrefData.mDomainIcons[SPATIAL_DOMAIN_SPACE]);
   mUi.unknownImageSelector->SetText(aPrefData.mDomainIcons[SPATIAL_DOMAIN_UNKNOWN]);
}
