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

#include "ActionTypes.hpp"
#include <ostream>
#include "ui_ActionTypes.h"

#include <QMessageBox>

PostProcessor::ActionTypes::ActionTypes()
   : ReportWidget()
{
   mUI.setupUi(this);
   mUI.actionFilter->SetColumnHeaders(QStringList() << "Name");
}

void PostProcessor::ActionTypes::WriteData(std::ostream& aStream)
{
   QStringList actions = mUI.actionFilter->GetCheckedItems(0);

   if (!mUI.actionFilter->AllChecked())
   {
      aStream << "actions\n";

      for (const auto& p : actions)
      {
         aStream << "\t" << p.toStdString() << "\n";
      }
      aStream << "end_actions\n";
   }
}

bool PostProcessor::ActionTypes::CheckValidity()
{
   bool isValid = true;
   if (mUI.actionFilter->AllUnchecked())
   {
      QMessageBox::critical(this, "Error", "At least one platform must be selected in Detected Platforms.");
      isValid = false;
   }
   return isValid;
}

void PostProcessor::ActionTypes::RestoreDefaults()
{
   // TODO
}
