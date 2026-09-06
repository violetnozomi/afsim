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

#include "RamjetEngineWidget.hpp"

#include "ui_RamjetEngineWidget.h"

Designer::RamjetEngineWidget::RamjetEngineWidget(const QString& aName, size_t aNumBaseSpecs, size_t aNumBaseSpecsOptions)
   : EngineWidget(aName, aNumBaseSpecs, aNumBaseSpecsOptions)
   , mUIPtr(new Ui::RamjetEngineWidget)
{
   mUIPtr->setupUi(this);

   mUIPtr->referenceAreaLineEdit->setValidator(mPositiveValidator.get());
   mUIPtr->designAltitudeLineEdit->setValidator(mPositiveValidator.get());
   mUIPtr->designMachLineEdit->setValidator(mPositiveValidator.get());
   mUIPtr->designThrustLineEdit->setValidator(mPositiveValidator.get());
   mUIPtr->tsfcLineEdit->setValidator(mPositiveValidator.get());
   mUIPtr->maxAltitudeLineEdit->setValidator(mPositiveValidator.get());
   mUIPtr->maxMachLineEdit->setValidator(mPositiveValidator.get());

   connect(mUIPtr->massLineEdit,           &QLineEdit::editingFinished, this, &RamjetEngineWidget::HandleEngineSpecEditingFinished);
   connect(mUIPtr->referenceAreaLineEdit,  &QLineEdit::editingFinished, this, &RamjetEngineWidget::HandleEngineSpecEditingFinished);
   connect(mUIPtr->designAltitudeLineEdit, &QLineEdit::editingFinished, this, &RamjetEngineWidget::HandleEngineSpecEditingFinished);
   connect(mUIPtr->designMachLineEdit,     &QLineEdit::editingFinished, this, &RamjetEngineWidget::HandleEngineSpecEditingFinished);
   connect(mUIPtr->designThrustLineEdit,   &QLineEdit::editingFinished, this, &RamjetEngineWidget::HandleEngineSpecEditingFinished);
   connect(mUIPtr->tsfcLineEdit,           &QLineEdit::editingFinished, this, &RamjetEngineWidget::HandleEngineSpecEditingFinished);
   connect(mUIPtr->maxAltitudeLineEdit,    &QLineEdit::editingFinished, this, &RamjetEngineWidget::HandleEngineSpecEditingFinished);
   connect(mUIPtr->maxMachLineEdit,        &QLineEdit::editingFinished, this, &RamjetEngineWidget::HandleEngineSpecEditingFinished);
}

Designer::RamjetEngineWidget::~RamjetEngineWidget()
{
   delete mUIPtr;
}

void Designer::RamjetEngineWidget::HandleEngineSpecEditingFinished()
{
   mEngineSpecs[2]                 = mUIPtr->massLineEdit->text().toDouble();
   mEngineSpecs[mNumBaseSpecs + 0] = mUIPtr->referenceAreaLineEdit->text().toDouble();
   mEngineSpecs[mNumBaseSpecs + 1] = mUIPtr->designAltitudeLineEdit->text().toDouble();
   mEngineSpecs[mNumBaseSpecs + 2] = mUIPtr->designMachLineEdit->text().toDouble();
   mEngineSpecs[mNumBaseSpecs + 3] = mUIPtr->designThrustLineEdit->text().toDouble();
   mEngineSpecs[mNumBaseSpecs + 4] = mUIPtr->tsfcLineEdit->text().toDouble();
   mEngineSpecs[mNumBaseSpecs + 5] = mUIPtr->maxAltitudeLineEdit->text().toDouble();
   mEngineSpecs[mNumBaseSpecs + 6] = mUIPtr->maxMachLineEdit->text().toDouble();

   emit EngineSpecsChanged();
}

void Designer::RamjetEngineWidget::PopulateLineEdits()
{
   mUIPtr->massLineEdit->setText(QString::number(mEngineSpecs.at(2)));
   mUIPtr->referenceAreaLineEdit->setText(QString::number(mEngineSpecs.at(mNumBaseSpecs + 0)));
   mUIPtr->designAltitudeLineEdit->setText(QString::number(mEngineSpecs.at(mNumBaseSpecs + 1)));
   mUIPtr->designMachLineEdit->setText(QString::number(mEngineSpecs.at(mNumBaseSpecs + 2)));
   mUIPtr->designThrustLineEdit->setText(QString::number(mEngineSpecs.at(mNumBaseSpecs + 3)));
   mUIPtr->tsfcLineEdit->setText(QString::number(mEngineSpecs.at(mNumBaseSpecs + 4)));
   mUIPtr->maxAltitudeLineEdit->setText(QString::number(mEngineSpecs.at(mNumBaseSpecs + 5)));
   mUIPtr->maxMachLineEdit->setText(QString::number(mEngineSpecs.at(mNumBaseSpecs + 6)));
}

QStringList Designer::RamjetEngineWidget::Audit() const
{
   QStringList errorList = EngineWidget::Audit();

   if (mEngineSpecs.at(2) < std::numeric_limits<double>::epsilon()) // Mass
   {
      QString errorString(" requires a positive mass");
      errorString = mName + errorString;
      errorList.push_back(errorString);
   }

   if (mEngineSpecs.at(mNumBaseSpecs + 0) < std::numeric_limits<double>::epsilon())
   {
      QString errorString(" requires a positive reference area");
      errorString = mName + errorString;
      errorList.push_back(errorString);
   }

   if (mEngineSpecs.at(mNumBaseSpecs + 1) < std::numeric_limits<double>::epsilon())
   {
      QString errorString(" requires a positive design altitude");
      errorString = mName + errorString;
      errorList.push_back(errorString);
   }

   if (mEngineSpecs.at(mNumBaseSpecs + 2) < std::numeric_limits<double>::epsilon())
   {
      QString errorString(" requires a positive design mach");
      errorString = mName + errorString;
      errorList.push_back(errorString);
   }

   if (mEngineSpecs.at(mNumBaseSpecs + 3) < std::numeric_limits<double>::epsilon())
   {
      QString errorString(" requires a positive design thrust");
      errorString = mName + errorString;
      errorList.push_back(errorString);
   }

   if (mEngineSpecs.at(mNumBaseSpecs + 4) < std::numeric_limits<double>::epsilon())
   {
      QString errorString(" requires a positive thrust specific fuel consumption (Tsfc)");
      errorString = mName + errorString;
      errorList.push_back(errorString);
   }

   if (mEngineSpecs.at(mNumBaseSpecs + 5) < 0)
   {
      QString errorString(" requires a non-negative max altitude");
      errorString = mName + errorString;
      errorList.push_back(errorString);
   }

   if (mEngineSpecs.at(mNumBaseSpecs + 6) < 0)
   {
      QString errorString(" requires a non-negative max mach");
      errorString = mName + errorString;
      errorList.push_back(errorString);
   }

   return errorList;
}
