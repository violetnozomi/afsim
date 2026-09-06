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

#include "OptionsET.hpp"
#include "ui_OptionsET.h"

PostProcessor::OptionsET::OptionsET()
   : OptionWidget("event_timing")
   , mUIPtr(new Ui::OptionsET)
{
   mUIPtr->setupUi(this);
   connect(mUIPtr->toolButton, &QToolButton::pressed, this, [this]() {emit ReportTypePressed(); });
}

PostProcessor::OptionsET::~OptionsET()
{
   delete mUIPtr;
}

void PostProcessor::OptionsET::WriteData(std::ostream& aStream)
{
   aStream << "reference_set";
   QString refSetName = " targets\n";
   if (mUIPtr->mButtonR->isChecked()) // High Power Microwave Damage Report
   {
      refSetName = " receivers\n";
   }
   aStream << refSetName.toStdString();

   QString originatorsName = "   selected\n";
   if (mUIPtr->mButtonAO->isChecked())
   {
      originatorsName = "   all\n";
   }
   aStream << "originators\n" << originatorsName.toStdString() << "end_originators\n";
}

void PostProcessor::OptionsET::RestoreDefaults()
{
   // TODO
}
