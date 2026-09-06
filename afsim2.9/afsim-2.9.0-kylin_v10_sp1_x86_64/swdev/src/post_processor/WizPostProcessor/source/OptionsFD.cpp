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

#include "OptionsFD.hpp"
#include "ui_OptionsFD.h"

PostProcessor::OptionsFD::OptionsFD()
   : OptionWidget("first_detections")
   , mUIPtr(new Ui::OptionsFD)
{
   mUIPtr->setupUi(this);
   connect(mUIPtr->toolButton, &QToolButton::pressed, this, [this]() {emit ReportTypePressed(); });
}

PostProcessor::OptionsFD::~OptionsFD()
{
   delete mUIPtr;
}

void PostProcessor::OptionsFD::WriteData(std::ostream& aStream)
{
   std::string optionsOutput = "";
   if (mUIPtr->mCheckBoxPTDP->isChecked())
   {
      optionsOutput += "   platform_type_of_detected_platform\n";
   }
   if (mUIPtr->mCheckBoxSI->isChecked())
   {
      optionsOutput += "   side_indicator\n";
   }
   if (mUIPtr->mCheckBoxRD->isChecked())
   {
      optionsOutput += "   range_of_detection\n";
   }
   if (mUIPtr->mCheckBoxADP->isChecked())
   {
      optionsOutput += "   altitude_of_detected_platform\n";
   }
   if (mUIPtr->mCheckBoxAA->isChecked())
   {
      optionsOutput += "   azimuth_angle\n";
   }
   if (mUIPtr->mCheckBoxEA->isChecked())
   {
      optionsOutput += "   elevation_angle\n";
   }

   if (!optionsOutput.empty())
   {
      aStream << "options\n" << optionsOutput << "end_options\n";
   }
}

void PostProcessor::OptionsFD::RestoreDefaults()
{
   mUIPtr->mCheckBoxPTDP->setChecked(true);
   mUIPtr->mCheckBoxSI->setChecked(true);
   mUIPtr->mCheckBoxRD->setChecked(true);
   mUIPtr->mCheckBoxADP->setChecked(true);
   mUIPtr->mCheckBoxAA->setChecked(true);
   mUIPtr->mCheckBoxEA->setChecked(true);
}
