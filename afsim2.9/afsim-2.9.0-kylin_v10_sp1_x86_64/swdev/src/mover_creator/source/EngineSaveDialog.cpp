// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017-2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "EngineSaveDialog.hpp"

#include "ui_EngineSaveDialog.h"
#include "EngineDesignerWidget.hpp"

namespace Designer
{

EngineSaveDialog::EngineSaveDialog(QWidget* aParent,
                                   Qt::WindowFlags aFlags)
   : QDialog(aParent, aFlags)
   , mUIPtr(new Ui::EngineSaveDialog)
{
   mUIPtr->setupUi(this);

   connect(mUIPtr->mSaveButton, &QPushButton::clicked, this, &EngineSaveDialog::SaveClicked);
   connect(mUIPtr->mIgnoreChangesButton, &QPushButton::clicked, this, &EngineSaveDialog::IgnoreClicked);
   connect(mUIPtr->mCancelButton, &QPushButton::clicked, this, &EngineSaveDialog::CancelClicked);
}


EngineSaveDialog::~EngineSaveDialog()
{

}

void EngineSaveDialog::SaveClicked(bool aClicked)
{
   EngineDesignerWidget* engineWidget = qobject_cast<EngineDesignerWidget*>(parentWidget());
   if (engineWidget)
   {
      engineWidget->SaveModel();
      accept();
   }
}


void EngineSaveDialog::IgnoreClicked(bool aClicked)
{
   accept();
}

void EngineSaveDialog::CancelClicked(bool aClicked)
{
   reject();
}

} // namespace Designer
