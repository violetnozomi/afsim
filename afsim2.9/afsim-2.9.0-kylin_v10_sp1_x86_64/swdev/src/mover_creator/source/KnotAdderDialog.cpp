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

#include "KnotAdderDialog.hpp"
#include "ui_KnotAdderDialog.h"

#include <QDoubleValidator>

namespace Designer
{

KnotAdderDialog::KnotAdderDialog(QWidget*        aParent,
                                 Qt::WindowFlags aFlags /* = Qt::WindowFlags()*/)
   : QDialog(aParent, aFlags)
   , mUIPtr(new Ui::KnotAdderDialog)
{
   mUIPtr->setupUi(this);

   QDoubleValidator* validator = new QDoubleValidator(this);
   mUIPtr->xCoordinateLineEdit->setValidator(validator);

   connect(mUIPtr->applyPushButton, &QPushButton::pressed, this, &KnotAdderDialog::HandleApplyButtonPressed);
}

KnotAdderDialog::~KnotAdderDialog()
{
   delete mUIPtr;
}

void KnotAdderDialog::HandleApplyButtonPressed()
{
   emit CreateKnot(mUIPtr->xCoordinateLineEdit->text().toDouble());
}

} // namespace Designer
