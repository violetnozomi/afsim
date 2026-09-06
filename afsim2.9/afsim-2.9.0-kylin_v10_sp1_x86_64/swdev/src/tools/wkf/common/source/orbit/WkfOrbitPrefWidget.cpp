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
#include "WkfOrbitPrefWidget.hpp"

#include <QComboBox>
#include <QFormLayout>
#include <QSpinBox>

#include "WkfOrbitPrefObject.hpp"

wkf::OrbitPrefWidget::OrbitPrefWidget(bool aRealtime)
   : wkf::PrefWidgetT<wkf::OrbitPrefObject>()
{
   setWindowTitle("Orbits");

   QFormLayout* form = new QFormLayout(this);
   setLayout(form);
   if (aRealtime)
   {
      mFadeout = new QSpinBox(this);
      mFadeout->setRange(1, 99999);
      form->addRow("Fadeout (sec):", mFadeout);
      mColorMode = new QComboBox(this);
      mColorMode->addItem("White", OrbitPrefData::eWHITE);
      mColorMode->addItem("Team", OrbitPrefData::eTEAM);
      mColorMode->addItem("Name", OrbitPrefData::eNAMEHASH);
      mColorMode->addItem("Scenario", OrbitPrefData::eSCENARIO);
      form->addRow("Color:", mColorMode);
   }
   mLineWidth = new QSpinBox(this);
   mLineWidth->setRange(1, 6);
   form->addRow("Line Width:", mLineWidth);
   mPeriods = new QSpinBox(this);
   mPeriods->setRange(1, 16);
   form->addRow("Periods:", mPeriods);
}

void wkf::OrbitPrefWidget::WritePreferenceData(OrbitPrefData& aPrefData)
{
   if (mFadeout)
   {
      aPrefData.mFadeout = mFadeout->value();
   }
   if (mColorMode)
   {
      aPrefData.mColorMode = static_cast<OrbitPrefData::ColorMode>(mColorMode->currentData().toInt());
   }
   aPrefData.mLineWidth = mLineWidth->value();
   aPrefData.mPeriods   = mPeriods->value();
}

void wkf::OrbitPrefWidget::ReadPreferenceData(const OrbitPrefData& aPrefData)
{
   if (mFadeout)
   {
      mFadeout->setValue(aPrefData.mFadeout);
   }
   if (mColorMode)
   {
      switch (aPrefData.mColorMode)
      {
      case (OrbitPrefData::eWHITE):
         mColorMode->setCurrentText("White");
         break;
      case (OrbitPrefData::eTEAM):
         mColorMode->setCurrentText("Team");
         break;
      case (OrbitPrefData::eNAMEHASH):
         mColorMode->setCurrentText("Name");
         break;
      case (OrbitPrefData::eSCENARIO):
         mColorMode->setCurrentText("Scenario");
         break;
      }
   }
   mLineWidth->setValue(aPrefData.mLineWidth);
   mPeriods->setValue(aPrefData.mPeriods);
}
