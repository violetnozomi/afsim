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

#include "WkfAirCombatVisualizationPrefWidget.hpp"

wkf::AirCombatVisualizationPrefWidget::AirCombatVisualizationPrefWidget()
   : PrefWidgetT<AirCombatVisualizationPrefObject>()
{
   mUi.setupUi(this);

   mUi.mFirstMeterBox->addItem(QString::fromStdString(mNoRingOption));
   mUi.mFirstMeterBox->addItem(QString::fromStdString(mDefensivenessOption));
   mUi.mFirstMeterBox->addItem(QString::fromStdString(mFuelLevelOption));
   mUi.mFirstMeterBox->addItem(QString::fromStdString(mGLoadOption));

   mUi.mSecondMeterBox->addItem(QString::fromStdString(mNoRingOption));
   mUi.mSecondMeterBox->addItem(QString::fromStdString(mDefensivenessOption));
   mUi.mSecondMeterBox->addItem(QString::fromStdString(mFuelLevelOption));
   mUi.mSecondMeterBox->addItem(QString::fromStdString(mGLoadOption));

   mUi.mThirdMeterBox->addItem(QString::fromStdString(mNoRingOption));
   mUi.mThirdMeterBox->addItem(QString::fromStdString(mDefensivenessOption));
   mUi.mThirdMeterBox->addItem(QString::fromStdString(mFuelLevelOption));
   mUi.mThirdMeterBox->addItem(QString::fromStdString(mGLoadOption));
}

void wkf::AirCombatVisualizationPrefWidget::ReadPreferenceData(const AirCombatVisualizationPrefData& aPrefData)
{
   // Set the first meter combo box to the appropriate value.
   if (aPrefData.mFirstMeter == 0)
   {
      mUi.mFirstMeterBox->setCurrentIndex(mUi.mFirstMeterBox->findText(QString::fromStdString(mNoRingOption)));
   }
   else if (aPrefData.mFirstMeter == 1)
   {
      mUi.mFirstMeterBox->setCurrentIndex(mUi.mFirstMeterBox->findText(QString::fromStdString(mDefensivenessOption)));
   }
   else if (aPrefData.mFirstMeter == 2)
   {
      mUi.mFirstMeterBox->setCurrentIndex(mUi.mFirstMeterBox->findText(QString::fromStdString(mFuelLevelOption)));
   }
   else if (aPrefData.mFirstMeter == 3)
   {
      mUi.mFirstMeterBox->setCurrentIndex(mUi.mFirstMeterBox->findText(QString::fromStdString(mGLoadOption)));
   }

   // Set the second meter combo box to the appropriate value.
   if (aPrefData.mSecondMeter == 0)
   {
      mUi.mSecondMeterBox->setCurrentIndex(mUi.mSecondMeterBox->findText(QString::fromStdString(mNoRingOption)));
   }
   else if (aPrefData.mSecondMeter == 1)
   {
      mUi.mSecondMeterBox->setCurrentIndex(mUi.mSecondMeterBox->findText(QString::fromStdString(mDefensivenessOption)));
   }
   else if (aPrefData.mSecondMeter == 2)
   {
      mUi.mSecondMeterBox->setCurrentIndex(mUi.mSecondMeterBox->findText(QString::fromStdString(mFuelLevelOption)));
   }
   else if (aPrefData.mSecondMeter == 3)
   {
      mUi.mSecondMeterBox->setCurrentIndex(mUi.mSecondMeterBox->findText(QString::fromStdString(mGLoadOption)));
   }

   // Set the second meter combo box to the appropriate value.
   if (aPrefData.mThirdMeter == 0)
   {
      mUi.mThirdMeterBox->setCurrentIndex(mUi.mThirdMeterBox->findText(QString::fromStdString(mNoRingOption)));
   }
   else if (aPrefData.mThirdMeter == 1)
   {
      mUi.mThirdMeterBox->setCurrentIndex(mUi.mThirdMeterBox->findText(QString::fromStdString(mDefensivenessOption)));
   }
   else if (aPrefData.mThirdMeter == 2)
   {
      mUi.mThirdMeterBox->setCurrentIndex(mUi.mThirdMeterBox->findText(QString::fromStdString(mFuelLevelOption)));
   }
   else if (aPrefData.mThirdMeter == 3)
   {
      mUi.mThirdMeterBox->setCurrentIndex(mUi.mThirdMeterBox->findText(QString::fromStdString(mGLoadOption)));
   }

   // Set the checkboxes accordingly.
   mUi.mShowStateData->setChecked(aPrefData.mShowStateData);
   mUi.mShowWeaponsAccent->setChecked(aPrefData.mShowWeaponsAccent);
   mUi.mShowFuelAccent->setChecked(aPrefData.mShowFuelAccent);
   mUi.mShowTransmissionAccent->setChecked(aPrefData.mShowTransmissionAccent);
   mUi.mShowSignatureAccent->setChecked(aPrefData.mShowSignatureAccent);
   mUi.mShowVisualization->setChecked(aPrefData.mShowVisualization);
}

void wkf::AirCombatVisualizationPrefWidget::WritePreferenceData(AirCombatVisualizationPrefData& aPrefData)
{
   // Save the first meter combo box.
   if (mUi.mFirstMeterBox->currentText() == QString::fromStdString(mNoRingOption))
   {
      aPrefData.mFirstMeter = 0;
   }
   else if (mUi.mFirstMeterBox->currentText() == QString::fromStdString(mDefensivenessOption))
   {
      aPrefData.mFirstMeter = 1;
   }
   else if (mUi.mFirstMeterBox->currentText() == QString::fromStdString(mFuelLevelOption))
   {
      aPrefData.mFirstMeter = 2;
   }
   else if (mUi.mFirstMeterBox->currentText() == QString::fromStdString(mGLoadOption))
   {
      aPrefData.mFirstMeter = 3;
   }

   // Save the second meter combo box.
   if (mUi.mSecondMeterBox->currentText() == QString::fromStdString(mNoRingOption))
   {
      aPrefData.mSecondMeter = 0;
   }
   else if (mUi.mSecondMeterBox->currentText() == QString::fromStdString(mDefensivenessOption))
   {
      aPrefData.mSecondMeter = 1;
   }
   else if (mUi.mSecondMeterBox->currentText() == QString::fromStdString(mFuelLevelOption))
   {
      aPrefData.mSecondMeter = 2;
   }
   else if (mUi.mSecondMeterBox->currentText() == QString::fromStdString(mGLoadOption))
   {
      aPrefData.mSecondMeter = 3;
   }

   // Save the third meter combo box.
   if (mUi.mThirdMeterBox->currentText() == QString::fromStdString(mNoRingOption))
   {
      aPrefData.mThirdMeter = 0;
   }
   else if (mUi.mThirdMeterBox->currentText() == QString::fromStdString(mDefensivenessOption))
   {
      aPrefData.mThirdMeter = 1;
   }
   else if (mUi.mThirdMeterBox->currentText() == QString::fromStdString(mFuelLevelOption))
   {
      aPrefData.mThirdMeter = 2;
   }
   else if (mUi.mThirdMeterBox->currentText() == QString::fromStdString(mGLoadOption))
   {
      aPrefData.mThirdMeter = 3;
   }

   // Save the checkbox states.
   aPrefData.mShowStateData          = mUi.mShowStateData->isChecked();
   aPrefData.mShowWeaponsAccent      = mUi.mShowWeaponsAccent->isChecked();
   aPrefData.mShowFuelAccent         = mUi.mShowFuelAccent->isChecked();
   aPrefData.mShowTransmissionAccent = mUi.mShowTransmissionAccent->isChecked();
   aPrefData.mShowSignatureAccent    = mUi.mShowSignatureAccent->isChecked();
   aPrefData.mShowVisualization      = mUi.mShowVisualization->isChecked();
}
