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

#include "OptionsThSm.hpp"
#include "ui_OptionsThSm.h"

PostProcessor::OptionsThSm::OptionsThSm()
   : OptionWidget("threat_summary")
   , mUIPtr(new Ui::OptionsThSm)
{
   mUIPtr->setupUi(this);
   connect(mUIPtr->toolButton, &QToolButton::pressed, this, [this]() {emit ReportTypePressed(); });
}

PostProcessor::OptionsThSm::~OptionsThSm()
{
   delete mUIPtr;
}

void PostProcessor::OptionsThSm::WriteData(std::ostream& aStream)
{
   std::string options = "";
   if (mUIPtr->mCheckBoxT->isChecked())
   {
      options += "   Totals\n";
   }
   if (mUIPtr->mCheckBoxPT->isChecked())
   {
      options += "   PlatformType\n";
   }
   aStream << "options\n" << options << "end_options\n";
}

void PostProcessor::OptionsThSm::RestoreDefaults()
{
   mUIPtr->mCheckBoxT->setChecked(true);
   mUIPtr->mCheckBoxPT->setChecked(false);
}
