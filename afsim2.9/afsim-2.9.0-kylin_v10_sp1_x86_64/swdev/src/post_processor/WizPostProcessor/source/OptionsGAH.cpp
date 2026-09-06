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

#include "OptionsGAH.hpp"
#include "ui_OptionsGAH.h"

PostProcessor::OptionsGAH::OptionsGAH()
   : OptionWidget("geometric_action_history")
   , mUIPtr(new Ui::OptionsGAH)
{
   mUIPtr->setupUi(this);
   connect(mUIPtr->toolButton, &QToolButton::pressed, this, [this]() {emit ReportTypePressed(); });
   connect(mUIPtr->mCheckBoxGRDA, &QCheckBox::clicked, this, &OptionsGAH::EnableGRDAWidgets);

   mUIPtr->mLatEdit->SetValue(0);
   mUIPtr->mLonEdit->SetValue(0);
   EnableGRDAWidgets(false);
}

PostProcessor::OptionsGAH::~OptionsGAH()
{
   delete mUIPtr;
}

void PostProcessor::OptionsGAH::EnableGRDAWidgets(bool aEnable)
{
   mUIPtr->mButtonPlatform->setEnabled(aEnable);
   mUIPtr->mButtonPosition->setEnabled(aEnable);
   mUIPtr->mComboBoxPlatform->setEnabled(aEnable);
   mUIPtr->mLatEdit->setEnabled(aEnable);
   mUIPtr->mLonEdit->setEnabled(aEnable);
}

void PostProcessor::OptionsGAH::WriteData(std::ostream& aStream)
{
   std::string options = "";
   if (mUIPtr->mCheckBoxFAT->isChecked())
   {
      options += "   FullActionText\n";
   }
   if (mUIPtr->mCheckBoxPN->isChecked())
   {
      options += "   PlatformName\n";
   }
   if (mUIPtr->mCheckBoxPTN->isChecked())
   {
      options += "   PlatformTypeName\n";
   }
   if (mUIPtr->mCheckBoxActingIFF->isChecked())
   {
      options += "   ActingPlatformsIFF\n";
   }
   if (mUIPtr->mCheckBoxAgainstIFF->isChecked())
   {
      options += "   AgainstPlatformsIFF\n";
   }
   if (mUIPtr->mCheckBoxActingPos->isChecked())
   {
      options += "   ActingPlatformsPosition\n";
   }
   if (mUIPtr->mCheckBoxAgainstPos->isChecked())
   {
      options += "   AgainstPlatformsPosition\n";
   }
   if (mUIPtr->mCheckBoxMN->isChecked())
   {
      options += "   MissileNumber\n";
   }
   if (mUIPtr->mCheckBoxMP->isChecked())
   {
      options += "   MissilePosition\n";
   }
   if (mUIPtr->mCheckBoxSN->isChecked())
   {
      options += "   SensorName\n";
   }
   if (mUIPtr->mCheckBoxSR->isChecked())
   {
      options += "   SlantRange\n";
   }
   if (mUIPtr->mCheckBoxGR->isChecked())
   {
      options += "   GroundRange\n";
   }
   if (mUIPtr->mCheckBoxGRDA->isChecked())
   {
      options += "   GroundRangeToDefendedAsset\n";
      // TODO: print more options
   }
   if (mUIPtr->mCheckBoxEA->isChecked())
   {
      options += "   ElevationAngle\n";
   }
   if (mUIPtr->mCheckBoxAA->isChecked())
   {
      options += "   AzimuthAngle\n";
   }
   if (mUIPtr->mCheckBoxTBFE->isChecked())
   {
      options += "   TargetBodyFrameElevation\n";
   }
   if (mUIPtr->mCheckBoxTBFA->isChecked())
   {
      options += "   TargetBodyFrameAzimuth\n";
   }
   if (mUIPtr->mCheckBoxWN->isChecked())
   {
      options += "   WeaponName\n";
   }
   if (mUIPtr->mCheckBoxWPN->isChecked())
   {
      options += "   WeaponPlatformName\n";
   }
   if (mUIPtr->mCheckBoxShotNum->isChecked())
   {
      options += "   ShotNumber\n";
   }
   if (mUIPtr->mCheckBoxTN->isChecked())
   {
      options += "   TrackNumber\n";
   }
   if (mUIPtr->mCheckBoxTSN->isChecked())
   {
      options += "   TrackingSensorName\n";
   }
   if (mUIPtr->mCheckBoxTSupN->isChecked())
   {
      options += "   TrackSupporterName\n";
   }
   if (mUIPtr->mCheckBoxCR->isChecked())
   {
      options += "   ClosingRate\n";
   }
   if (mUIPtr->mCheckBoxTTGM->isChecked())
   {
      options += "   TimeToGoForMissile\n";
   }
   if (mUIPtr->mCheckBoxSRTG->isChecked())
   {
      options += "   SlantRangeToGo\n";
   }
   aStream << "options\n" << options << "end_options\n";
}

void PostProcessor::OptionsGAH::RestoreDefaults()
{
   mUIPtr->mCheckBoxFAT->setChecked(false);
   mUIPtr->mCheckBoxPN->setChecked(false);
   mUIPtr->mCheckBoxPTN->setChecked(false);
   mUIPtr->mCheckBoxActingIFF->setChecked(false);
   mUIPtr->mCheckBoxAgainstIFF->setChecked(false);
   mUIPtr->mCheckBoxActingPos->setChecked(true);
   mUIPtr->mCheckBoxAgainstPos->setChecked(true);
   mUIPtr->mCheckBoxMN->setChecked(false);
   mUIPtr->mCheckBoxMP->setChecked(false);
   mUIPtr->mCheckBoxSN->setChecked(false);
   mUIPtr->mCheckBoxSR->setChecked(true);
   mUIPtr->mCheckBoxGR->setChecked(false);
   mUIPtr->mCheckBoxGRDA->setChecked(false);
   mUIPtr->mCheckBoxEA->setChecked(false);
   mUIPtr->mCheckBoxAA->setChecked(true);
   mUIPtr->mCheckBoxTBFE->setChecked(false);
   mUIPtr->mCheckBoxTBFA->setChecked(false);
   mUIPtr->mCheckBoxWN->setChecked(false);
   mUIPtr->mCheckBoxWPN->setChecked(false);
   mUIPtr->mCheckBoxShotNum->setChecked(false);
   mUIPtr->mCheckBoxTN->setChecked(false);
   mUIPtr->mCheckBoxTSN->setChecked(false);
   mUIPtr->mCheckBoxTSupN->setChecked(false);
   mUIPtr->mCheckBoxCR->setChecked(true);
   mUIPtr->mCheckBoxTTGM->setChecked(false);
   mUIPtr->mCheckBoxSRTG->setChecked(false);
}
