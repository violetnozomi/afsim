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

#include "AuditErrorDialog.hpp"

#include "ui_AuditErrorDialog.h"

namespace Designer
{

AuditErrorDialog::AuditErrorDialog(QWidget* aParent,
                                   Qt::WindowFlags aFlags)
   : mUIPtr(new Ui::AuditErrorDialog)
{
   mUIPtr->setupUi(this);
   mUIPtr->mOverrideButton->hide();

   connect(mUIPtr->mOkButton, &QPushButton::clicked, this, &AuditErrorDialog::OkClicked);
   connect(mUIPtr->mOverrideButton, &QPushButton::clicked, this, &AuditErrorDialog::OverrideClicked);
}

AuditErrorDialog::~AuditErrorDialog()
{
   delete mUIPtr;
}

void AuditErrorDialog::OkClicked()
{
   reject();
}

void AuditErrorDialog::OverrideClicked()
{
   accept();
}

void AuditErrorDialog::SetErrorLabel(QString aStr)
{
   mUIPtr->mErrorLabel->setText(aStr);
}

void AuditErrorDialog::AddError(QString aErrorStr)
{
   mUIPtr->mErrorListWidget->addItem(aErrorStr);
}

void AuditErrorDialog::AddErrors(QStringList aErrorList)
{
   mUIPtr->mErrorListWidget->addItems(aErrorList);
}

void AuditErrorDialog::showEvent(QShowEvent* aEvent)
{
   QWidget::showEvent(aEvent);
}

void AuditErrorDialog::ShowOverrideButton()
{
   mUIPtr->mOverrideButton->show();
}


} // namespace Designer
