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

#include "KnotEditorDialog.hpp"
#include "ui_KnotEditorDialog.h"

#include <QDoubleValidator>

namespace Designer
{

KnotEditorDialog::KnotEditorDialog(QWidget*        aParent,
                                   Qt::WindowFlags aFlags /*= Qt::WindowFlags()*/)
   : QDialog(aParent, aFlags)
   , mUIPtr(new Ui::KnotEditorDialog)
{
   mUIPtr->setupUi(this);

   QDoubleValidator* validator = new QDoubleValidator(this);
   mUIPtr->xCoordinateLineEdit->setValidator(validator);
   mUIPtr->yCoordinateLineEdit->setValidator(validator);
   mUIPtr->bcValueLineEdit->setValidator(validator);

   mUIPtr->bcTypeComboBox->addItem("Slope", CubicSpline::cFIRST_DERIV);
   mUIPtr->xCoordinateLineEdit->setText("0");
   mUIPtr->yCoordinateLineEdit->setText("0");
   mUIPtr->bcValueLineEdit->setText("0");

   connect(mUIPtr->applyPushButton, &QPushButton::pressed, this, &KnotEditorDialog::HandleApplyButtonPressed);
}

KnotEditorDialog::~KnotEditorDialog()
{
   delete mUIPtr;
}

void KnotEditorDialog::SetXCoordinate(double aVal)
{
   mUIPtr->xCoordinateLineEdit->setText(QString::number(aVal));
}

void KnotEditorDialog::SetYCoordinate(double aVal)
{
   mUIPtr->yCoordinateLineEdit->setText(QString::number(aVal));
}

void KnotEditorDialog::SetBCType(CubicSpline::BC_Type aType)
{
   mUIPtr->bcTypeComboBox->setCurrentIndex(mUIPtr->bcTypeComboBox->findData(aType));
}

void KnotEditorDialog::SetBCValue(double aVal)
{
   mUIPtr->bcValueLineEdit->setText(QString::number(aVal));
}

void KnotEditorDialog::HandleApplyButtonPressed()
{
   CubicSpline::Knot temp;

   temp.coordinates = { mUIPtr->xCoordinateLineEdit->text().toDouble(),
                        mUIPtr->yCoordinateLineEdit->text().toDouble()
                      };
   temp.BCSpecification = (CubicSpline::BC_Type)mUIPtr->bcTypeComboBox->currentData().toInt();
   temp.boundaryCondition = mUIPtr->bcValueLineEdit->text().toDouble();

   emit EditKnot(temp);
}

} // namespace Designer
