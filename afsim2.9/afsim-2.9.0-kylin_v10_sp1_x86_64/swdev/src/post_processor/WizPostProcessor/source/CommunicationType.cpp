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

#include "CommunicationType.hpp"
#include "ui_CommunicationType.h"

#include "OptionsC.hpp"
#include "OptionsTC.hpp"

PostProcessor::CommunicationType::CommunicationType()
   : ReportWidget()
   , mOptionName("traffic_counts") // by default, the Traffic Counts radio button is selected
{
   mUI.setupUi(this);
   connect(mUI.mButtonTC, &QRadioButton::pressed, this, &CommunicationType::TCPressedHandler);
   connect(mUI.mButtonC, &QRadioButton::pressed, this, &CommunicationType::CPressedHandler);

   connect(mUI.toolButton, &QToolButton::pressed, this, &CommunicationType::emitReportOptionsPressed);

   mUI.toolButton->setEnabled(true);

   mOptionWidgets = { new OptionsTC(), new OptionsC() };

}

void PostProcessor::CommunicationType::TCPressedHandler()
{
   mOptionName = "traffic_counts";
   mUI.toolButton->setEnabled(true);
}

void PostProcessor::CommunicationType::CPressedHandler()
{
   mOptionName = "connectivity";
   mUI.toolButton->setEnabled(true);
}

void PostProcessor::CommunicationType::WriteData(std::ostream& aStream)
{
   aStream << "report_type " << mOptionName << "\n";
   // call WriteData on whatever option is selected
   for (auto w : mOptionWidgets)
   {
      if (mOptionName.compare(w->GetName()) == 0)
      {
         w->WriteData(aStream);
         break;
      }
   }
}

void PostProcessor::CommunicationType::RestoreDefaults()
{
   mOptionName = "traffic_counts";
   mUI.mButtonTC->setChecked(true);
}
