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

#include "OptionsTD.hpp"
#include "ui_OptionsTD.h"

PostProcessor::OptionsTD::OptionsTD()
   : OptionWidget("threat_deployment")
   , mUIPtr(new Ui::OptionsTD)
{
   mUIPtr->setupUi(this);
   connect(mUIPtr->toolButton, &QToolButton::pressed, this, [this]() {emit ReportTypePressed(); });
}

PostProcessor::OptionsTD::~OptionsTD()
{
   delete mUIPtr;
}

void PostProcessor::OptionsTD::WriteData(std::ostream& aStream)
{
   std::string options = "";

   if (mUIPtr->mCheckBoxT->isChecked())
   {
      options += "   Totals\n";
   }
   if (mUIPtr->mCheckBoxPT->isChecked())
   {
      options += "PlatformType\n";
   }

   aStream << "options\n" << options << "end_options\n";
}

void PostProcessor::OptionsTD::RestoreDefaults()
{
   mUIPtr->mCheckBoxT->setChecked(true);
   mUIPtr->mCheckBoxPT->setChecked(false);
}