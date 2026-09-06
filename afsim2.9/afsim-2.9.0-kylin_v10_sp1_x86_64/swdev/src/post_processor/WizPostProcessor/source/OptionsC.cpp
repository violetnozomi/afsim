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

#include "OptionsC.hpp"
#include "ui_OptionsC.h"

PostProcessor::OptionsC::OptionsC()
   : OptionWidget("connectivity")
   , mUIPtr(new Ui::OptionsC)
{
   mUIPtr->setupUi(this);
   connect(mUIPtr->toolButton, &QToolButton::pressed, this, [this]() { emit ReportTypePressed(); });
}

PostProcessor::OptionsC::~OptionsC()
{
   delete mUIPtr;
}

void PostProcessor::OptionsC::WriteData(std::ostream& aStream)
{
   aStream << "options\n";
   if (mUIPtr->mButtonSC->isChecked())
   {
      aStream << "   status_change\n";
   }
   else
   {
      aStream << "   time_history\n";
   }
   aStream << "end_options\n";
}

void PostProcessor::OptionsC::RestoreDefaults()
{
   mUIPtr->mButtonSC->setChecked(true);
}
