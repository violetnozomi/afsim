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

#include "Networks.hpp"
#include <ostream>
#include "ui_Networks.h"

#include <QMessageBox>

PostProcessor::Networks::Networks()
   : ReportWidget()
   , mUIPtr(new Ui::Networks)
{
   mUIPtr->setupUi(this);
   mUIPtr->networkFilter->SetColumnHeaders(QStringList() << "Name");
}

PostProcessor::Networks::~Networks()
{
   delete mUIPtr;
}

void PostProcessor::Networks::WriteData(std::ostream& aStream)
{
   QStringList networks = mUIPtr->networkFilter->GetCheckedItems(0);

   if (!mUIPtr->networkFilter->AllChecked())
   {
      aStream << "networks\n";

      for (const auto& n : networks)
      {
         aStream << "\t" << n.toStdString() << "\n";
      }
      aStream << "end_networks\n";
   }
}

bool PostProcessor::Networks::CheckValidity()
{
   bool isValid = true;
   if (mUIPtr->networkFilter->AllUnchecked())
   {
      QMessageBox::critical(this, "Error", "At least one transmitter must be selected.");
      isValid = false;
   }
   return isValid;
}

void PostProcessor::Networks::RestoreDefaults()
{
   // TODO
}
