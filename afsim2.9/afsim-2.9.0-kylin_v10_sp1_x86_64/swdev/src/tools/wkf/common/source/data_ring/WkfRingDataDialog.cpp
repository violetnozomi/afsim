// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfRingDataDialog.hpp"

#include <QMessageBox>

wkf::RingDataDialog::RingDataDialog(QWidget* aParent, Qt::WindowFlags aFlags)
   : QDialog(aParent, aFlags)
{
   mUi.setupUi(this);

   mUi.maxValueBox->setMaximum(INT_MAX);
   mUi.maxValueBox->setMinimum(INT_MIN);
   mUi.minValueBox->setMaximum(INT_MAX);
   mUi.minValueBox->setMinimum(INT_MIN);

   mUi.minValueColor->SetColor(Qt::red);
   mUi.maxValueColor->SetColor(Qt::green);

   connect(mUi.clampToBaseBox, &QCheckBox::toggled, [&](bool aChecked) { mUi.minValueBox->setEnabled(!aChecked); });
   connect(mUi.buttonBox,
           &QDialogButtonBox::accepted,
           [&]()
           {
              if (mUi.maxValueBox->value() > mUi.minValueBox->value())
              {
                 emit accepted();
                 close();
              }
              else
              {
                 QMessageBox::warning(this,
                                      "Value Mismatch",
                                      "The maximum value must be greater than the minimum value.",
                                      QMessageBox::StandardButton::Ok);
              }
           });
   connect(mUi.buttonBox,
           &QDialogButtonBox::rejected,
           [&]()
           {
              emit rejected();
              close();
           });
}

void wkf::RingDataDialog::SetMinValue(double aValue)
{
   mUi.minValueBox->setValue(aValue);
}

void wkf::RingDataDialog::SetMaxValue(double aValue)
{
   mUi.maxValueBox->setValue(aValue);
}

double wkf::RingDataDialog::GetMinValue() const
{
   return mUi.minValueBox->value();
}

double wkf::RingDataDialog::GetMaxValue() const
{
   return mUi.maxValueBox->value();
}

QColor wkf::RingDataDialog::GetMinColor() const
{
   return mUi.minValueColor->GetColor();
}

QColor wkf::RingDataDialog::GetMaxColor() const
{
   return mUi.maxValueColor->GetColor();
}

bool wkf::RingDataDialog::GetClamped() const
{
   return mUi.clampToBaseBox->isChecked();
}

void wkf::RingDataDialog::SetUnit(const QString& aUnit)
{
   mUi.maxValueBox->setSuffix(aUnit);
   mUi.minValueBox->setSuffix(aUnit);
}
