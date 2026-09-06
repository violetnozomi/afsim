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

#include "OptionsADE.hpp"
#include "ui_OptionsADE.h"

PostProcessor::OptionsADE::OptionsADE()
   : OptionWidget("all_detection_events")
   , mUIPtr(new Ui::OptionsADE)
{
   mUIPtr->setupUi(this);
   connect(mUIPtr->toolButton, &QToolButton::pressed, this, [this]() {emit ReportTypePressed(); });
}

PostProcessor::OptionsADE::~OptionsADE()
{
   delete mUIPtr;
}

void PostProcessor::OptionsADE::WriteData(std::ostream& aStream)
{
   aStream << "options\n";
   if (mUIPtr->mButtonDS->isChecked())
   {
      aStream << "   detailedstatistics\n";
   }
   else if (mUIPtr->mButtonEH->isChecked())
   {
      aStream << "   eventhistory\n";
   }
   aStream << "end_options\n";
}

void PostProcessor::OptionsADE::RestoreDefaults()
{
   mUIPtr->mButtonDS->setChecked(true);
}
