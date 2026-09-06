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

#include "OptionsDFH.hpp"
#include "ui_OptionsDFH.h"

PostProcessor::OptionsDFH::OptionsDFH()
   : OptionWidget("damage_factor_history")
   , mUIPtr(new Ui::OptionsDFH)
{
   mUIPtr->setupUi(this);
   connect(mUIPtr->toolButton, &QToolButton::pressed, this, [this]() {emit ReportTypePressed(); });
   connect(mUIPtr->mButtonDEDR, &QRadioButton::toggled, this, &OptionsDFH::EnableDEWidgets);

   EnableDEWidgets(false);
}

PostProcessor::OptionsDFH::~OptionsDFH()
{
   delete mUIPtr;
}

void PostProcessor::OptionsDFH::EnableDEWidgets(bool aEnable)
{
   mUIPtr->mCheckBoxSS->setEnabled(aEnable);
   mUIPtr->mCheckBoxAPI->setEnabled(aEnable);
   mUIPtr->mCheckBoxPPI->setEnabled(aEnable);
   mUIPtr->mCheckBoxAP->setEnabled(aEnable);
   mUIPtr->mCheckBoxPC->setEnabled(aEnable);
   mUIPtr->mCheckBoxWPC->setEnabled(aEnable);
   mUIPtr->mCheckBoxMKT->setEnabled(aEnable);
   mUIPtr->mCheckBoxLF->setEnabled(aEnable);
   mUIPtr->mCheckBoxBDRD->setEnabled(aEnable);
   mUIPtr->mCheckBoxDT->setEnabled(aEnable);
   mUIPtr->mCheckBoxA->setEnabled(aEnable);
   mUIPtr->mCheckBoxDPk->setEnabled(aEnable);
   mUIPtr->mCheckBoxAgainstIFF->setEnabled(aEnable);
   mUIPtr->mCheckBoxAA->setEnabled(aEnable);
   mUIPtr->mCheckBoxPA->setEnabled(aEnable);
   mUIPtr->mCheckBoxAPk->setEnabled(aEnable);
   mUIPtr->mCheckBoxAKM->setEnabled(aEnable);
   mUIPtr->mCheckBoxPKM->setEnabled(aEnable);
   mUIPtr->mCheckBoxABS->setEnabled(aEnable);
   mUIPtr->mCheckBoxLP->setEnabled(aEnable);
   mUIPtr->mCheckBoxPBS->setEnabled(aEnable);
   mUIPtr->mCheckBoxTST->setEnabled(aEnable);
   mUIPtr->mCheckBoxDEAH->setEnabled(aEnable);
   mUIPtr->mCheckBoxActingIFF->setEnabled(aEnable);
   mUIPtr->mCheckBoxBDP->setEnabled(aEnable);
}

void PostProcessor::OptionsDFH::WriteData(std::ostream& aStream)
{
   aStream << "options\n";
   QString name = "";
   if (mUIPtr->mButtonHPMDR->isChecked()) // High Power Microwave Damage Report
   {
      name = "   high_power_microwave\n";
   }
   else if (mUIPtr->mButtonPDR->isChecked()) // Partial Damage Report
   {
      name = "   partial\n";
   }
   else // Directed Energy Damage Report
   {
      name = "   directed_energy\n";
   }
   aStream << name.toStdString() << "end_options\n";

   if (QString::compare(name, "directed_energy") == 0) // If the Directed Energy option is selected, more options apply
   {
      std::string options = "\n";

      if (mUIPtr->mCheckBoxDEAH->isChecked())
      {
         options += "   DEActionHistory\n";
      }
      if (mUIPtr->mCheckBoxActingIFF->isChecked())
      {
         options += "   ActingPlatformsIFF\n";
      }
      if (mUIPtr->mCheckBoxAgainstIFF->isChecked())
      {
         options += "   AgainstPlatformsIFF\n";
      }
      if (mUIPtr->mCheckBoxLP->isChecked())
      {
         options += "   LaunchingPlatform\n";
      }
      if (mUIPtr->mCheckBoxTST->isChecked())
      {
         options += "   TotalSlewTime\n";
      }
      if (mUIPtr->mCheckBoxDT->isChecked())
      {
         options += "   DwellTime\n";
      }
      if (mUIPtr->mCheckBoxMKT->isChecked())
      {
         options += "   MaximumKillTime\n";
      }
      if (mUIPtr->mCheckBoxLF->isChecked())
      {
         options += "   LaserFuel\n";
      }
      if (mUIPtr->mCheckBoxDPk->isChecked())
      {
         options += "   DesiredPk\n";
      }
      if (mUIPtr->mCheckBoxPA->isChecked())
      {
         options += "   PlannedAccumulation\n";
      }
      if (mUIPtr->mCheckBoxPKM->isChecked())
      {
         options += "   PlannedKillMetric\n";
      }
      if (mUIPtr->mCheckBoxPBS->isChecked())
      {
         options += "   PlannedBeamSpread\n";
      }
      if (mUIPtr->mCheckBoxPPI->isChecked())
      {
         options += "   PlannedPeakIntensity\n";
      }
      if (mUIPtr->mCheckBoxSS->isChecked())
      {
         options += "   SpotSize\n";
      }
      if (mUIPtr->mCheckBoxA->isChecked())
      {
         options += "   Angle\n";
      }
      if (mUIPtr->mCheckBoxAP->isChecked())
      {
         options += "   AimPoint\n";
      }
      if (mUIPtr->mCheckBoxAPk->isChecked())
      {
         options += "   ActualPk\n";
      }
      if (mUIPtr->mCheckBoxAA->isChecked())
      {
         options += "   ActualAccumulation\n";
      }
      if (mUIPtr->mCheckBoxAKM->isChecked())
      {
         options += "   ActualKillMetric\n";
      }
      if (mUIPtr->mCheckBoxABS->isChecked())
      {
         options += "   ActualBeamSpread\n";
      }
      if (mUIPtr->mCheckBoxAPI->isChecked())
      {
         options += "   ActualPeakIntensity\n";
      }
      if (mUIPtr->mCheckBoxPC->isChecked())
      {
         options += "   PowerCorrection\n";
      }
      if (mUIPtr->mCheckBoxWPC->isChecked())
      {
         options += "   WavefrontPowerCorrection\n";
      }
      if (mUIPtr->mCheckBoxBDRD->isChecked())
      {
         options += "   BeamDeliveryRandomDraw\n";
      }
      if (mUIPtr->mCheckBoxBDP->isChecked())
      {
         options += "   BeamDeliveryProbability\n";
      }

      aStream << "directed_energy_options" << options << "end_directed_energy_options\n";
   }
}

void PostProcessor::OptionsDFH::RestoreDefaults()
{
   mUIPtr->mButtonHPMDR->setChecked(true);
   EnableDEWidgets(false);

   mUIPtr->mCheckBoxSS->setChecked(false);
   mUIPtr->mCheckBoxAPI->setChecked(false);
   mUIPtr->mCheckBoxPPI->setChecked(false);
   mUIPtr->mCheckBoxAP->setChecked(false);
   mUIPtr->mCheckBoxPC->setChecked(false);
   mUIPtr->mCheckBoxWPC->setChecked(false);
   mUIPtr->mCheckBoxMKT->setChecked(false);
   mUIPtr->mCheckBoxLF->setChecked(false);
   mUIPtr->mCheckBoxBDRD->setChecked(false);
   mUIPtr->mCheckBoxDT->setChecked(false);
   mUIPtr->mCheckBoxA->setChecked(false);
   mUIPtr->mCheckBoxDPk->setChecked(false);
   mUIPtr->mCheckBoxAgainstIFF->setChecked(false);
   mUIPtr->mCheckBoxAA->setChecked(false);
   mUIPtr->mCheckBoxPA->setChecked(false);
   mUIPtr->mCheckBoxAPk->setChecked(false);
   mUIPtr->mCheckBoxAKM->setChecked(false);
   mUIPtr->mCheckBoxPKM->setChecked(false);
   mUIPtr->mCheckBoxABS->setChecked(false);
   mUIPtr->mCheckBoxLP->setChecked(false);
   mUIPtr->mCheckBoxPBS->setChecked(false);
   mUIPtr->mCheckBoxTST->setChecked(false);
   mUIPtr->mCheckBoxDEAH->setChecked(false);
   mUIPtr->mCheckBoxActingIFF->setChecked(false);
   mUIPtr->mCheckBoxBDP->setChecked(false);
}
