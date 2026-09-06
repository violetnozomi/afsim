// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "LiquidRocketEngineWidget.hpp"

#include "ui_LiquidRocketEngineWidget.h"

Designer::LiquidRocketEngineWidget::LiquidRocketEngineWidget(const QString& aName, size_t aNumBaseSpecs, size_t aNumBaseSpecsOptions)
   : EngineWidget(aName, aNumBaseSpecs, aNumBaseSpecsOptions)
   , mUIPtr(new Ui::LiquidRocketEngineWidget)
{
   mUIPtr->setupUi(this);

   mUIPtr->massLineEdit->setValidator(mPositiveValidator.get());
   mUIPtr->refAreaLineEdit->setValidator(mPositiveValidator.get());
   mUIPtr->spinUpLineEdit->setValidator(mPositiveValidator.get());
   mUIPtr->spinDownLineEdit->setValidator(mPositiveValidator.get());
   mUIPtr->thrustSeaLevelLineEdit->setValidator(mPositiveValidator.get());
   mUIPtr->thrustVacuumLineEdit->setValidator(mPositiveValidator.get());
   mUIPtr->specificImpulseSeaLevelLineEdit->setValidator(mPositiveValidator.get());
   mUIPtr->specificImpulseVacuumLineEdit->setValidator(mPositiveValidator.get());

   connect(mUIPtr->massLineEdit,                    &QLineEdit::editingFinished, this, &LiquidRocketEngineWidget::HandleEngineSpecEditingFinished);
   connect(mUIPtr->refAreaLineEdit,                 &QLineEdit::editingFinished, this, &LiquidRocketEngineWidget::HandleEngineSpecEditingFinished);
   connect(mUIPtr->spinUpLineEdit,                  &QLineEdit::editingFinished, this, &LiquidRocketEngineWidget::HandleEngineSpecEditingFinished);
   connect(mUIPtr->spinDownLineEdit,                &QLineEdit::editingFinished, this, &LiquidRocketEngineWidget::HandleEngineSpecEditingFinished);
   connect(mUIPtr->thrustSeaLevelLineEdit,          &QLineEdit::editingFinished, this, &LiquidRocketEngineWidget::HandleEngineSpecEditingFinished);
   connect(mUIPtr->thrustVacuumLineEdit,            &QLineEdit::editingFinished, this, &LiquidRocketEngineWidget::HandleEngineSpecEditingFinished);
   connect(mUIPtr->specificImpulseSeaLevelLineEdit, &QLineEdit::editingFinished, this, &LiquidRocketEngineWidget::HandleEngineSpecEditingFinished);
   connect(mUIPtr->specificImpulseVacuumLineEdit,   &QLineEdit::editingFinished, this, &LiquidRocketEngineWidget::HandleEngineSpecEditingFinished);
}

Designer::LiquidRocketEngineWidget::~LiquidRocketEngineWidget()
{
   delete mUIPtr;
}

QStringList Designer::LiquidRocketEngineWidget::Audit() const
{
   QStringList errorList = EngineWidget::Audit();

   if (mEngineSpecs.at(mNumBaseSpecs + 0) < 0)
   {
      QString errorString(" requires a non-negative reference area");
      errorString = mName + errorString;
      errorList.push_back(errorString);
   }

   if (mEngineSpecs.at(mNumBaseSpecs + 1) < std::numeric_limits<double>::epsilon())
   {
      QString errorString(" requires a positive spin-up rate");
      errorString = mName + errorString;
      errorList.push_back(errorString);
   }

   if (mEngineSpecs.at(mNumBaseSpecs + 2) < std::numeric_limits<double>::epsilon())
   {
      QString errorString(" requires a positive spin-down rate");
      errorString = mName + errorString;
      errorList.push_back(errorString);
   }

   if (mEngineSpecs.at(mNumBaseSpecs + 3) < std::numeric_limits<double>::epsilon())
   {
      QString errorString(" requires a positive sea level thrust");
      errorString = mName + errorString;
      errorList.push_back(errorString);
   }

   if (mEngineSpecs.at(mNumBaseSpecs + 4) < std::numeric_limits<double>::epsilon())
   {
      QString errorString(" requires a positive vacuum thrust");
      errorString = mName + errorString;
      errorList.push_back(errorString);
   }

   if (mEngineSpecs.at(mNumBaseSpecs + 5) < std::numeric_limits<double>::epsilon())
   {
      QString errorString(" requires a positive sea level specific impulse");
      errorString = mName + errorString;
      errorList.push_back(errorString);
   }

   if (mEngineSpecs.at(mNumBaseSpecs + 6) < std::numeric_limits<double>::epsilon())
   {
      QString errorString(" requires a positive vacuum specific impulse");
      errorString = mName + errorString;
      errorList.push_back(errorString);
   }

   return errorList;
}

void Designer::LiquidRocketEngineWidget::HandleEngineSpecEditingFinished()
{
   mEngineSpecs[2]                 = mUIPtr->massLineEdit->text().toDouble();
   mEngineSpecs[mNumBaseSpecs + 0] = mUIPtr->refAreaLineEdit->text().toDouble();
   mEngineSpecs[mNumBaseSpecs + 1] = mUIPtr->spinUpLineEdit->text().toDouble();
   mEngineSpecs[mNumBaseSpecs + 2] = mUIPtr->spinDownLineEdit->text().toDouble();
   mEngineSpecs[mNumBaseSpecs + 3] = mUIPtr->thrustSeaLevelLineEdit->text().toDouble();
   mEngineSpecs[mNumBaseSpecs + 4] = mUIPtr->thrustVacuumLineEdit->text().toDouble();
   mEngineSpecs[mNumBaseSpecs + 5] = mUIPtr->specificImpulseSeaLevelLineEdit->text().toDouble();
   mEngineSpecs[mNumBaseSpecs + 6] = mUIPtr->specificImpulseVacuumLineEdit->text().toDouble();

   emit EngineSpecsChanged();
}

void Designer::LiquidRocketEngineWidget::PopulateLineEdits()
{
   mUIPtr->massLineEdit->setText(QString::number(mEngineSpecs.at(2)));
   mUIPtr->refAreaLineEdit->setText(QString::number(mEngineSpecs.at(mNumBaseSpecs + 0)));
   mUIPtr->spinUpLineEdit->setText(QString::number(mEngineSpecs.at(mNumBaseSpecs + 1)));
   mUIPtr->spinDownLineEdit->setText(QString::number(mEngineSpecs.at(mNumBaseSpecs + 2)));
   mUIPtr->thrustSeaLevelLineEdit->setText(QString::number(mEngineSpecs.at(mNumBaseSpecs + 3)));
   mUIPtr->thrustVacuumLineEdit->setText(QString::number(mEngineSpecs.at(mNumBaseSpecs + 4)));
   mUIPtr->specificImpulseSeaLevelLineEdit->setText(QString::number(mEngineSpecs.at(mNumBaseSpecs + 5)));
   mUIPtr->specificImpulseVacuumLineEdit->setText(QString::number(mEngineSpecs.at(mNumBaseSpecs + 6)));
}