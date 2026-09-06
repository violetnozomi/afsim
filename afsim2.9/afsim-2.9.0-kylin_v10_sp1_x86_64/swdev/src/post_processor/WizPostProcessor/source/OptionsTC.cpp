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

#include "OptionsTC.hpp"
#include "ui_OptionsTC.h"

PostProcessor::OptionsTC::OptionsTC()
   : OptionWidget("traffic_counts")
   , mUIPtr(new Ui::OptionsTC)
{
   mUIPtr->setupUi(this);
   connect(mUIPtr->toolButton, &QToolButton::pressed, this, [this]() {emit ReportTypePressed(); });
}

PostProcessor::OptionsTC::~OptionsTC()
{
   delete mUIPtr;
}

void PostProcessor::OptionsTC::WriteData(std::ostream& aStream)
{
   aStream << "options\n";
   if (mUIPtr->mButtonP->isChecked())
   {
      aStream << "   platforms\n";
   }
   else if (mUIPtr->mButtonN->isChecked())
   {
      aStream << "   networks\n";
   }
   else if (mUIPtr->mButtonCD->isChecked())
   {
      aStream << "   comm_device\n";
   }
   else if (mUIPtr->mButtonCC->isChecked())
   {
      aStream << "   command_chain\n";
   }
   else if (mUIPtr->mButtonTI->isChecked())
   {
      aStream << "   time_interval\n";
   }
   aStream << "end_options\n";
}

void PostProcessor::OptionsTC::RestoreDefaults()
{
   mUIPtr->mButtonP->setChecked(true);
}
