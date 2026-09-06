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

#include "OptionsTMS.hpp"
#include "ui_OptionsTMS.h"

PostProcessor::OptionsTMS::OptionsTMS()
   : OptionWidget("threat_mode_summary")
   , mUIPtr(new Ui::OptionsTMS)
{
   mUIPtr->setupUi(this);
   connect(mUIPtr->toolButton, &QToolButton::pressed, this, [this]() {emit ReportTypePressed(); });
}

PostProcessor::OptionsTMS::~OptionsTMS()
{
   delete mUIPtr;
}

void PostProcessor::OptionsTMS::WriteData(std::ostream& aStream)
{
   aStream << "options\n";
   if (mUIPtr->mButtonFSO->isChecked())
   {
      aStream << "   FirstShotOpportunity\n";
   }
   else if (mUIPtr->mButtonBSO->isChecked())
   {
      aStream << "   BestShotOpportunity\n";
   }
   else if (mUIPtr->mButtonPSS->isChecked())
   {
      aStream << "   PossibleShot\n";
   }
   else if (mUIPtr->mButtonSH->isChecked())
   {
      aStream << "   ShotHistory\n";
   }
   aStream << "end_options\n";
}

void PostProcessor::OptionsTMS::RestoreDefaults()
{
   mUIPtr->mButtonFSO->setChecked(true);
}