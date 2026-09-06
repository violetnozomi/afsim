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

#include "OptionsPAPA.hpp"
#include "ui_OptionsPAPA.h"

PostProcessor::OptionsPAPA::OptionsPAPA()
   : OptionWidget("papa")
   , mUIPtr(new Ui::OptionsPAPA)
{
   mUIPtr->setupUi(this);
   connect(mUIPtr->toolButton, &QToolButton::pressed, this, [this]() {emit ReportTypePressed(); });
}

PostProcessor::OptionsPAPA::~OptionsPAPA()
{
   delete mUIPtr;
}

void PostProcessor::OptionsPAPA::WriteData(std::ostream& aStream)
{
   aStream << "options\n";
   if (mUIPtr->mButtonAWRAD->isChecked()) // Aircraft/Weapons Route and Attrition Data
   {
      aStream << "   AWRAD\n";
   }
   else if (mUIPtr->mButtonPkR->isChecked()) // Pk Report
   {
      aStream << "   Pk\n";
   }
   else if (mUIPtr->mButtonSIMDIS->isChecked()) // SIMDIS
   {
      aStream << "   SIMDIS\n";
   }
   aStream << "end_options\n";
}

void PostProcessor::OptionsPAPA::RestoreDefaults()
{
   mUIPtr->mButtonAWRAD->setChecked(true);
}
