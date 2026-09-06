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

#include "CalculateAeroDialog.hpp"

#include "ui_CalculateAeroDialog.h"

#include <QTimer>

namespace Designer
{

CalculateAeroDialog::CalculateAeroDialog(QWidget* aParent,
                                         Qt::WindowFlags aFlags)
   : QDialog(aParent, aFlags)
   , mUIPtr(new Ui::CalculateAeroDialog)
{
   mUIPtr->setupUi(this);
   mProgressTimer = new QTimer(this);
   mUIPtr->mCalculatingAeroLabel->setAlignment(Qt::AlignCenter);
   connect(mProgressTimer, &QTimer::timeout, this, &CalculateAeroDialog::UpdateProgressBar);
}


CalculateAeroDialog::~CalculateAeroDialog()
{
   delete mProgressTimer;
}

void CalculateAeroDialog::showEvent(QShowEvent* aEvent)
{
   mProgressTimer->start(1000);
}

void CalculateAeroDialog::UpdateProgressBar()
{
   int curVal = mUIPtr->mCalcAeroProgressBar->value();
   if (curVal < 100)
   {
      mUIPtr->mCalcAeroProgressBar->setValue(curVal + 34);
   }
}


void CalculateAeroDialog::EndDialog()
{
   mProgressTimer->stop();
   close();
}

void CalculateAeroDialog::SetLabelText(QString aStr)
{
   mUIPtr->mCalculatingAeroLabel->setText(aStr);
}
void CalculateAeroDialog::SetWindowTitle(QString aStr)
{
   setWindowTitle(aStr);
}

} // namespace Designer
