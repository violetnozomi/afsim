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

#include "NewTypeDialog.hpp"

#include "ui_NewTypeDialog.h"

namespace Designer
{

NewTypeDialog::NewTypeDialog(QWidget* aParent,
                             Qt::WindowFlags aFlags /*= Qt::WindowFlags()*/)
   : QDialog(aParent, aFlags)
   , mUIPtr(new Ui::NewTypeDialog)
{
   mUIPtr->setupUi(this);
   connect(mUIPtr->mOkButton, &QPushButton::clicked, this, &NewTypeDialog::OkClicked);
   connect(mUIPtr->mCancelButton, &QPushButton::clicked, this, &NewTypeDialog::CancelClicked);
   connect(mUIPtr->mNewTypeLineEdit, &QLineEdit::returnPressed, this, &NewTypeDialog::accept);

}

NewTypeDialog::~NewTypeDialog()
{
   delete mUIPtr;
}

void NewTypeDialog::OkClicked(bool aClicked)
{
   accept();
}

void NewTypeDialog::CancelClicked(bool aClicked)
{
   reject();
}

QString NewTypeDialog::GetName()
{
   return mUIPtr->mNewTypeLineEdit->text();
}

} // namespace Designer
