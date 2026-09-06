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



#include "GeometrySettingsDialog.hpp"

#include "ui_GeometrySettingsDialog.h"

namespace Designer
{

GeometrySettingsDialog::GeometrySettingsDialog(QWidget* aParent,
                                               Qt::WindowFlags aFlags)
   : QDialog(aParent, aFlags)
   , mUIPtr(new Ui::GeometrySettingsDialog)
{
   mUIPtr->setupUi(this);
   connect(mUIPtr->mOptionsListWidget, &QListWidget::currentItemChanged, this, &GeometrySettingsDialog::PageChanged);
   connect(mUIPtr->mOkButton, &QPushButton::clicked, this, &GeometrySettingsDialog::accept);
   connect(mUIPtr->mCancelButton, &QPushButton::clicked, this, &GeometrySettingsDialog::reject);

   connect(mUIPtr->mMoveCGXButton, &QPushButton::clicked, this, [this]() {emit MoveToCG_X(); });
   connect(mUIPtr->mMoveCGZButton, &QPushButton::clicked, this, [this]() {emit MoveToCG_Z(); });
   connect(mUIPtr->mMoveCGXZButton, &QPushButton::clicked, this, [this]() {emit MoveToCG_XZ(); });
   connect(mUIPtr->mMoveCGAllButton, &QPushButton::clicked, this, [this]() {emit MoveToCG_XYZ(); });

   connect(mUIPtr->mIncreaseAlphaButton, &QPushButton::clicked, this, [this]() {emit AlphaIncrease(); });
   connect(mUIPtr->mDecreaseAlphaButton, &QPushButton::clicked, this, [this]() {emit AlphaDecrease(); });
   connect(mUIPtr->mIncreaseBetaButton, &QPushButton::clicked, this, [this]() {emit BetaIncrease(); });
   connect(mUIPtr->mDecreaseBetaButton, &QPushButton::clicked, this, [this]() {emit BetaDecrease(); });
   connect(mUIPtr->mIncreaseYawButton, &QPushButton::clicked, this, [this]() {emit YawRateIncrease(); });
   connect(mUIPtr->mDecreaseYawButton, &QPushButton::clicked, this, [this]() {emit YawRateDecrease(); });
   connect(mUIPtr->mIncreasePitchButton, &QPushButton::clicked, this, [this]() {emit PitchRateIncrease(); });
   connect(mUIPtr->mDecreasePitchButton, &QPushButton::clicked, this, [this]() {emit PitchRateDecrease(); });
   connect(mUIPtr->mIncreaseRollButton, &QPushButton::clicked, this, [this]() {emit RollRateIncrease(); });
   connect(mUIPtr->mDecreaseRollButton, &QPushButton::clicked, this, [this]() {emit RollRateDecrease(); });
   connect(mUIPtr->mIncreaseSpeedButton, &QPushButton::clicked, this, [this]() {emit SpeedIncrease(); });
   connect(mUIPtr->mDecreaseSpeedButton, &QPushButton::clicked, this, [this]() {emit SpeedDecrease(); });
   connect(mUIPtr->mIncreaseControlAngleButton, &QPushButton::clicked, this, [this]() {emit SurfaceControlAngleIncrease(); });
   connect(mUIPtr->mDecreaseControlAngleButton, &QPushButton::clicked, this, [this]() {emit SurfaceControlAngleDecrease(); });



   mUIPtr->mOptionsListWidget->setCurrentItem(mUIPtr->mOptionsListWidget->item(0));
}


GeometrySettingsDialog::~GeometrySettingsDialog()
{

}

void GeometrySettingsDialog::showEvent(QShowEvent* aEvent)
{

}

void GeometrySettingsDialog::PageChanged(QListWidgetItem* aCurrent, QListWidgetItem* aPrevious)
{
   if (aCurrent == mUIPtr->mOptionsListWidget->item(0))
   {
      //CG Item
      mUIPtr->mStackedWidget->setCurrentIndex(0);
   }
   else if (aCurrent == mUIPtr->mOptionsListWidget->item(1))
   {
      //Aerodynamcis Item
      mUIPtr->mStackedWidget->setCurrentIndex(1);
   }
}

void GeometrySettingsDialog::SetTestAltitude(double aAltitude)
{
   mUIPtr->mTestAltitudeLineEdit->setText(QString::number(aAltitude));
}

void GeometrySettingsDialog::SetTestMach(double aMach)
{
   mUIPtr->mTestMachLineEdit->setText(QString::number(aMach));
}

void GeometrySettingsDialog::SetMaxAlpha(double aAlpha)
{
   mUIPtr->mMaxAlphaLineEdit->setText(QString::number(aAlpha));
}

void GeometrySettingsDialog::SetMinAlpha(double aAlpha)
{
   mUIPtr->mMinAlphaLineEdit->setText(QString::number(aAlpha));
}

void GeometrySettingsDialog::SetMaxMach(double aMach)
{
   mUIPtr->mMaxMachLineEdit->setText(QString::number(aMach));
}

void GeometrySettingsDialog::SetMaxBeta(double aBeta)
{
   mUIPtr->mMaxBetaLineEdit->setText(QString::number(aBeta));
}

void Designer::GeometrySettingsDialog::SetAlphaSymmetry(bool aSymmetric)
{
   mUIPtr->mAlphaSymmetryCheckBox->setChecked(aSymmetric);
}

void Designer::GeometrySettingsDialog::SetBetaSymmetry(bool aSymmetric)
{
   mUIPtr->mBetaSymmetryCheckBox->setChecked(aSymmetric);
}

double GeometrySettingsDialog::GetAltitude()
{
   return mUIPtr->mTestAltitudeLineEdit->text().toDouble();
}

double GeometrySettingsDialog::GetTestMach()
{
   return mUIPtr->mTestMachLineEdit->text().toDouble();
}

double GeometrySettingsDialog::GetMaxMach()
{
   return mUIPtr->mMaxMachLineEdit->text().toDouble();
}

double GeometrySettingsDialog::GetMinAlpha()
{
   return mUIPtr->mMinAlphaLineEdit->text().toDouble();
}

double GeometrySettingsDialog::GetMaxAlpha()
{
   return mUIPtr->mMaxAlphaLineEdit->text().toDouble();
}

double GeometrySettingsDialog::GetMaxBeta()
{
   return mUIPtr->mMaxBetaLineEdit->text().toDouble();
}

bool Designer::GeometrySettingsDialog::GetAlphaSymmetry()
{
   return mUIPtr->mAlphaSymmetryCheckBox->isChecked();
}

bool Designer::GeometrySettingsDialog::GetBetaSymmetry()
{
   return mUIPtr->mBetaSymmetryCheckBox->isChecked();
}

} // namespace Designer
