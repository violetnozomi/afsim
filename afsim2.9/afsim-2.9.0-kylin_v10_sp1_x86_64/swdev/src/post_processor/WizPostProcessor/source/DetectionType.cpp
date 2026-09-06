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

#include "DetectionType.hpp"
#include "ui_DetectionType.h"

#include "OptionsADE.hpp"
#include "OptionsFD.hpp"

PostProcessor::DetectionType::DetectionType()
   : ReportWidget()
   , mUIPtr(new Ui::DetectionType)
   , mOptionName("total_detections")
{
   mUIPtr->setupUi(this);
   connect(mUIPtr->mButtonFD, &QRadioButton::pressed, this, &DetectionType::FDPressedHandler);
   connect(mUIPtr->mButtonADE, &QRadioButton::pressed, this, &DetectionType::ADEPressedHandler);
   connect(mUIPtr->mButtonTD, &QRadioButton::pressed, this, &DetectionType::TDPressedHandler);
   connect(mUIPtr->mButtonDB, &QRadioButton::pressed, this, &DetectionType::DBPressedHandler);
   connect(mUIPtr->mButtonA, &QRadioButton::pressed, this, &DetectionType::AccessPressedHandler);

   connect(mUIPtr->toolButton, &QToolButton::pressed, this, &DetectionType::emitReportOptionsPressed);

   mUIPtr->toolButton->setEnabled(false);

   mOptionWidgets = { new OptionsFD(),
                     new OptionsADE()
   };
}

PostProcessor::DetectionType::~DetectionType()
{
   delete mUIPtr;
}

void PostProcessor::DetectionType::FDPressedHandler()
{
   mOptionName = "first_detections";
   mUIPtr->toolButton->setEnabled(true);
}

void PostProcessor::DetectionType::ADEPressedHandler()
{
   mOptionName = "all_detectionevents";
   mUIPtr->toolButton->setEnabled(true);
}

void PostProcessor::DetectionType::TDPressedHandler()
{
   mOptionName = "total_detections";
   mUIPtr->toolButton->setEnabled(false);
}

void PostProcessor::DetectionType::DBPressedHandler()
{
   mOptionName = "detected_by";
   mUIPtr->toolButton->setEnabled(false);
}

void PostProcessor::DetectionType::AccessPressedHandler()
{
   mOptionName = "access";
   mUIPtr->toolButton->setEnabled(false);
}

void PostProcessor::DetectionType::WriteData(std::ostream& aStream)
{
   aStream << "report_type " << mOptionName << "\n";
   for (auto w : mOptionWidgets)
   {
      if (mOptionName.compare(w->GetName()) == 0)
      {
         w->WriteData(aStream);
         break;
      }
   }
}

void PostProcessor::DetectionType::RestoreDefaults()
{
   mOptionName = "total_detections";
   mUIPtr->mButtonTD->setChecked(true);
   mUIPtr->toolButton->setEnabled(false);
}
