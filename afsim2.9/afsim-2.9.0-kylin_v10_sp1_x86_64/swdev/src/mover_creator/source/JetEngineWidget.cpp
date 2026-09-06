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

#include "JetEngineWidget.hpp"

#include "ui_JetEngineWidget.h"

Designer::JetEngineWidget::JetEngineWidget(const QString& aName, size_t aNumBaseSpecs, size_t aNumBaseSpecsOptions, bool aIncludeAfterburner)
   : EngineWidget(aName, aNumBaseSpecs, aNumBaseSpecsOptions)
   , mUIPtr(new Ui::JetEngineWidget)
   , mIncludeAfterburner(aIncludeAfterburner)
{
   mUIPtr->setupUi(this);

   // Set validators for all the line edits
   mUIPtr->massLineEdit->setValidator(mPositiveValidator.get());
   mUIPtr->refAreaLineEdit->setValidator(mPositiveValidator.get());
   mUIPtr->idleThrustLineEdit->setValidator(mPositiveValidator.get());
   mUIPtr->milThrustLineEdit->setValidator(mPositiveValidator.get());
   mUIPtr->abThrustLineEdit->setValidator(mPositiveValidator.get());
   mUIPtr->idleTSFCLineEdit->setValidator(mPositiveValidator.get());
   mUIPtr->milTSFCLineEdit->setValidator(mPositiveValidator.get());
   mUIPtr->abTSFCLineEdit->setValidator(mPositiveValidator.get());
   mUIPtr->maxAltitudeLineEdit->setValidator(mPositiveValidator.get());
   mUIPtr->maxMachLineEdit->setValidator(mPositiveValidator.get());

   connect(mUIPtr->massLineEdit,         &QLineEdit::editingFinished, this, &JetEngineWidget::HandleEngineSpecEditingFinished);
   connect(mUIPtr->refAreaLineEdit,      &QLineEdit::editingFinished, this, &JetEngineWidget::HandleEngineSpecEditingFinished);
   connect(mUIPtr->idleThrustLineEdit,   &QLineEdit::editingFinished, this, &JetEngineWidget::HandleEngineSpecEditingFinished);
   connect(mUIPtr->milThrustLineEdit,    &QLineEdit::editingFinished, this, &JetEngineWidget::HandleEngineSpecEditingFinished);
   connect(mUIPtr->abThrustLineEdit,     &QLineEdit::editingFinished, this, &JetEngineWidget::HandleEngineSpecEditingFinished);
   connect(mUIPtr->idleTSFCLineEdit,     &QLineEdit::editingFinished, this, &JetEngineWidget::HandleEngineSpecEditingFinished);
   connect(mUIPtr->milTSFCLineEdit,      &QLineEdit::editingFinished, this, &JetEngineWidget::HandleEngineSpecEditingFinished);
   connect(mUIPtr->abTSFCLineEdit,       &QLineEdit::editingFinished, this, &JetEngineWidget::HandleEngineSpecEditingFinished);
   connect(mUIPtr->maxAltitudeLineEdit,  &QLineEdit::editingFinished, this, &JetEngineWidget::HandleEngineSpecEditingFinished);
   connect(mUIPtr->maxMachLineEdit,      &QLineEdit::editingFinished, this, &JetEngineWidget::HandleEngineSpecEditingFinished);

   if (!mIncludeAfterburner)
   {
      mUIPtr->abThrustLineEdit->hide();
      mUIPtr->abThrustLabel->hide();
      mUIPtr->abThrustUnitLabel->hide();

      mUIPtr->abTSFCLineEdit->hide();
      mUIPtr->abTSFCLabel->hide();
      mUIPtr->abTSFCUnitLabel->hide();
   }

}

Designer::JetEngineWidget::~JetEngineWidget()
{
   delete mUIPtr;
}

void Designer::JetEngineWidget::HandleEngineSpecEditingFinished()
{
   mEngineSpecs[2]                 = mUIPtr->massLineEdit->text().toDouble();
   mEngineSpecs[mNumBaseSpecs + 0] = mUIPtr->refAreaLineEdit->text().toDouble();
   mEngineSpecs[mNumBaseSpecs + 1] = mUIPtr->idleThrustLineEdit->text().toDouble();
   mEngineSpecs[mNumBaseSpecs + 2] = mUIPtr->milThrustLineEdit->text().toDouble();
   mEngineSpecs[mNumBaseSpecs + 3] = mUIPtr->abThrustLineEdit->text().toDouble();
   mEngineSpecs[mNumBaseSpecs + 4] = mUIPtr->idleTSFCLineEdit->text().toDouble();
   mEngineSpecs[mNumBaseSpecs + 5] = mUIPtr->milTSFCLineEdit->text().toDouble();
   mEngineSpecs[mNumBaseSpecs + 6] = mUIPtr->abTSFCLineEdit->text().toDouble();
   mEngineSpecs[mNumBaseSpecs + 7] = mUIPtr->maxAltitudeLineEdit->text().toDouble();
   mEngineSpecs[mNumBaseSpecs + 8] = mUIPtr->maxMachLineEdit->text().toDouble();

   emit EngineSpecsChanged();
}

void Designer::JetEngineWidget::PopulateLineEdits()
{
   mUIPtr->massLineEdit->setText(QString::number(mEngineSpecs.at(2)));
   mUIPtr->refAreaLineEdit->setText(QString::number(mEngineSpecs.at(mNumBaseSpecs + 0)));
   mUIPtr->idleThrustLineEdit->setText(QString::number(mEngineSpecs.at(mNumBaseSpecs + 1)));
   mUIPtr->milThrustLineEdit->setText(QString::number(mEngineSpecs.at(mNumBaseSpecs + 2)));
   mUIPtr->abThrustLineEdit->setText(QString::number(mEngineSpecs.at(mNumBaseSpecs + 3)));
   mUIPtr->idleTSFCLineEdit->setText(QString::number(mEngineSpecs.at(mNumBaseSpecs + 4)));
   mUIPtr->milTSFCLineEdit->setText(QString::number(mEngineSpecs.at(mNumBaseSpecs + 5)));
   mUIPtr->abTSFCLineEdit->setText(QString::number(mEngineSpecs.at(mNumBaseSpecs + 6)));
   mUIPtr->maxAltitudeLineEdit->setText(QString::number(mEngineSpecs.at(mNumBaseSpecs + 7)));
   mUIPtr->maxMachLineEdit->setText(QString::number(mEngineSpecs.at(mNumBaseSpecs + 8)));
}

QStringList Designer::JetEngineWidget::Audit() const
{
   QStringList errorList = EngineWidget::Audit();

   if (mEngineSpecs.at(2) < std::numeric_limits<double>::epsilon()) // Mass
   {
      QString errorString(" requires a positive mass");
      errorString = mName + errorString;
      errorList.push_back(errorString);
   }

   if (mEngineSpecs.at(mNumBaseSpecs + 0) < std::numeric_limits<double>::epsilon()) // Ref Area
   {
      QString errorString(" requires a positive reference area");
      errorString = mName + errorString;
      errorList.push_back(errorString);
   }

   if (mEngineSpecs.at(mNumBaseSpecs + 1) < std::numeric_limits<double>::epsilon()) // Idle Thrust
   {
      QString errorString(" requires a positive idle sea level thrust");
      errorString = mName + errorString;
      errorList.push_back(errorString);
   }

   if (mEngineSpecs.at(mNumBaseSpecs + 2) < std::numeric_limits<double>::epsilon()) // Mil Thrust
   {
      QString errorString(" requires a positive military sea level thrust");
      errorString = mName + errorString;
      errorList.push_back(errorString);
   }

   if (mIncludeAfterburner && mEngineSpecs.at(mNumBaseSpecs + 3) < std::numeric_limits<double>::epsilon()) // Afterburner Thrust
   {
      QString errorString(" requires a positive after burner sea level thrust");
      errorString = mName + errorString;
      errorList.push_back(errorString);
   }

   if (mEngineSpecs.at(mNumBaseSpecs + 4) < std::numeric_limits<double>::epsilon()) // Idle Thrust Specific Fuel Consumption
   {
      QString errorString(" requires a positive idle thrust specific fuel consumption");
      errorString = mName + errorString;
      errorList.push_back(errorString);
   }

   if (mEngineSpecs.at(mNumBaseSpecs + 5) < std::numeric_limits<double>::epsilon()) // Mil Thrust Specific Fuel Consumption
   {
      QString errorString(" requires a positive military thrust specific fuel consumption");
      errorString = mName + errorString;
      errorList.push_back(errorString);
   }

   if (mIncludeAfterburner && mEngineSpecs.at(mNumBaseSpecs + 6) < std::numeric_limits<double>::epsilon()) // Afterburner Thrust Specific Fuel Consumption
   {
      QString errorString(" requires a positive after burner thrust specific fuel consumption");
      errorString = mName + errorString;
      errorList.push_back(errorString);
   }

   if (mEngineSpecs.at(mNumBaseSpecs + 7) < 0) // Max Altitude
   {
      QString errorString(" requires a non-negative max altitude");
      errorString = mName + errorString;
      errorList.push_back(errorString);
   }

   if (mEngineSpecs.at(mNumBaseSpecs + 8) < 0) // Max Mach
   {
      QString errorString(" requires a non-negative max mach");
      errorString = mName + errorString;
      errorList.push_back(errorString);
   }

   return errorList;
}