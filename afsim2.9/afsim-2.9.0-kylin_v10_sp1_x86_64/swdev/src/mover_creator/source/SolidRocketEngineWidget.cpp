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

#include "SolidRocketEngineWidget.hpp"

#include "ui_SolidRocketEngineWidget.h"

#include "MoverCreatorMainWindow.hpp"

Designer::SolidRocketEngineWidget::SolidRocketEngineWidget(const QString& aName, size_t aNumBaseSpecs, size_t aNumBaseSpecsOptions)
   : EngineWidget(aName, aNumBaseSpecs, aNumBaseSpecsOptions)
   , mUIPtr(new Ui::SolidRocketEngineWidget)
{
   mUIPtr->setupUi(this);

   // Set validators for all the line edits
   mUIPtr->massLineEdit->setValidator(mPositiveValidator.get());
   mUIPtr->specificImpulseSeaLevelLineEdit->setValidator(mPositiveValidator.get());
   mUIPtr->specificImpulseVacuumLineEdit->setValidator(mPositiveValidator.get());
   mUIPtr->rampUpTimeLineEdit->setValidator(mPositiveValidator.get());
   mUIPtr->rampDownTimeLineEdit->setValidator(mPositiveValidator.get());
   mUIPtr->maxRampUpThrustLineEdit->setValidator(mPositiveValidator.get());
   mUIPtr->maxRampDownThrustLineEdit->setValidator(mPositiveValidator.get());
   mUIPtr->burnTimeLineEdit->setValidator(mPositiveValidator.get());
   mUIPtr->propellantMassLineEdit->setValidator(mPositiveValidator.get());

   // Add items to combo boxes
   mUIPtr->specificationsComboBox->clear();
   mUIPtr->specificationsComboBox->addItem("Thrust and Burn Time");
   mUIPtr->specificationsComboBox->addItem("Thrust and Propellant Mass");
   mUIPtr->specificationsComboBox->addItem("Burn Time and Propellant Mass");

   mUIPtr->massComboBox->clear();
   mUIPtr->massComboBox->addItem("Empty");
   mUIPtr->massComboBox->addItem("Total");

   mUIPtr->altitudeComboBox->clear();
   mUIPtr->altitudeComboBox->addItem("Sea Level");
   mUIPtr->altitudeComboBox->addItem("Vacuum");

   mUIPtr->propellantMassComboBox->clear();
   mUIPtr->propellantMassComboBox->addItem("Absolute");
   mUIPtr->propellantMassComboBox->addItem("Fraction");

   connect(mUIPtr->massLineEdit, &QLineEdit::editingFinished, this, &SolidRocketEngineWidget::HandleEngineSpecEditingFinished);
   connect(mUIPtr->specificImpulseSeaLevelLineEdit, &QLineEdit::editingFinished, this, &SolidRocketEngineWidget::HandleEngineSpecEditingFinished);
   connect(mUIPtr->specificImpulseVacuumLineEdit, &QLineEdit::editingFinished, this, &SolidRocketEngineWidget::HandleEngineSpecEditingFinished);
   connect(mUIPtr->rampUpTimeLineEdit, &QLineEdit::editingFinished, this, &SolidRocketEngineWidget::HandleEngineSpecEditingFinished);
   connect(mUIPtr->rampDownTimeLineEdit, &QLineEdit::editingFinished, this, &SolidRocketEngineWidget::HandleEngineSpecEditingFinished);
   connect(mUIPtr->maxRampUpThrustLineEdit, &QLineEdit::editingFinished, this, &SolidRocketEngineWidget::HandleEngineSpecEditingFinished);
   connect(mUIPtr->maxRampDownThrustLineEdit, &QLineEdit::editingFinished, this, &SolidRocketEngineWidget::HandleEngineSpecEditingFinished);
   connect(mUIPtr->burnTimeLineEdit, &QLineEdit::editingFinished, this, &SolidRocketEngineWidget::HandleEngineSpecEditingFinished);
   connect(mUIPtr->propellantMassLineEdit, &QLineEdit::editingFinished, this, &SolidRocketEngineWidget::HandleEngineSpecEditingFinished);

   connect(mUIPtr->specificationsComboBox,  static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::activated), this, &SolidRocketEngineWidget::HandleSpecificationsComboBoxChanged);
   connect(mUIPtr->massComboBox,            static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::activated), this, &SolidRocketEngineWidget::HandleMassComboBoxChanged);
   connect(mUIPtr->altitudeComboBox,        static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::activated), this, &SolidRocketEngineWidget::HandleAltitudeComboBoxChanged);
   connect(mUIPtr->propellantMassComboBox,  static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::activated), this, &SolidRocketEngineWidget::HandlePropellantMassComboBoxChanged);
}

Designer::SolidRocketEngineWidget::~SolidRocketEngineWidget()
{
   delete mUIPtr;
}

QStringList Designer::SolidRocketEngineWidget::Audit() const
{
   QStringList errorList = EngineWidget::Audit();

   if (mEngineSpecs.at(mNumBaseSpecs + 0) < std::numeric_limits<double>::epsilon()) // Ref Area
   {
      QString errorString(" requires a positive sea level specific impulse");
      errorString = mName + errorString;
      errorList.push_back(errorString);
   }

   if (mEngineSpecs.at(mNumBaseSpecs + 1) < std::numeric_limits<double>::epsilon()) // Ref Area
   {
      QString errorString(" requires a positive vacuum specific impulse");
      errorString = mName + errorString;
      errorList.push_back(errorString);
   }

   if (mEngineSpecs.at(mNumBaseSpecs + 2) < std::numeric_limits<double>::epsilon()) // Ref Area
   {
      QString errorString(" requires a positive ramp up time");
      errorString = mName + errorString;
      errorList.push_back(errorString);
   }

   if (mEngineSpecs.at(mNumBaseSpecs + 3) < std::numeric_limits<double>::epsilon()) // Ref Area
   {
      QString errorString(" requires a positive ramp down time");
      errorString = mName + errorString;
      errorList.push_back(errorString);
   }

   if (mEngineSpecs.at(mNumBaseSpecs + 4) < std::numeric_limits<double>::epsilon()) // Ref Area
   {
      QString errorString(" requires a positive initial rated thrust");
      errorString = mName + errorString;
      errorList.push_back(errorString);
   }

   if (mEngineSpecs.at(mNumBaseSpecs + 5) < std::numeric_limits<double>::epsilon()) // Ref Area
   {
      QString errorString(" requires a positive final rated thrust");
      errorString = mName + errorString;
      errorList.push_back(errorString);
   }

   if (mEngineSpecs.at(mNumBaseSpecs + 6) < std::numeric_limits<double>::epsilon()) // Ref Area
   {
      QString errorString(" requires a positive burn time");
      errorString = mName + errorString;
      errorList.push_back(errorString);
   }

   if (mEngineSpecs.at(mNumBaseSpecs + 7) < std::numeric_limits<double>::epsilon()) // Ref Area
   {
      QString errorString(" requires a positive propellant mass");
      errorString = mName + errorString;
      errorList.push_back(errorString);
   }

   if (mEngineSpecs.at(mNumBaseSpecs + 8) < std::numeric_limits<double>::epsilon()) // Ref Area
   {
      QString errorString(" requires a positive propellant mass fraction");
      errorString = mName + errorString;
      errorList.push_back(errorString);
   }

   if (mEngineSpecs.at(mNumBaseSpecs + 9) < std::numeric_limits<double>::epsilon()) // Ref Area
   {
      QString errorString(" requires a positive empty mass");
      errorString = mName + errorString;
      errorList.push_back(errorString);
   }

   if (mEngineSpecs.at(mNumBaseSpecs + 10) < std::numeric_limits<double>::epsilon()) // Ref Area
   {
      QString errorString(" requires a positive total mass");
      errorString = mName + errorString;
      errorList.push_back(errorString);
   }

   return errorList;
}

void Designer::SolidRocketEngineWidget::HandleEngineSpecEditingFinished()
{
   // When any of the line edits change, save the values in the line edits in EngineSpecs

   QString massSpecification = mUIPtr->massComboBox->currentText();
   QString propMassSpecification = mUIPtr->propellantMassComboBox->currentText();

   mEngineSpecs[mNumBaseSpecs + 0] = mUIPtr->specificImpulseSeaLevelLineEdit->text().toDouble();
   mEngineSpecs[mNumBaseSpecs + 1] = mUIPtr->specificImpulseVacuumLineEdit->text().toDouble();
   mEngineSpecs[mNumBaseSpecs + 2] = mUIPtr->rampUpTimeLineEdit->text().toDouble();
   mEngineSpecs[mNumBaseSpecs + 3] = mUIPtr->rampDownTimeLineEdit->text().toDouble();
   mEngineSpecs[mNumBaseSpecs + 4] = mUIPtr->maxRampUpThrustLineEdit->text().toDouble();
   mEngineSpecs[mNumBaseSpecs + 5] = mUIPtr->maxRampDownThrustLineEdit->text().toDouble();
   mEngineSpecs[mNumBaseSpecs + 6] = mUIPtr->burnTimeLineEdit->text().toDouble();

   // Propellant Mass
   if (QString::compare(propMassSpecification, "Absolute") == 0)
   {
      mEngineSpecs[mNumBaseSpecs + 7] = mUIPtr->propellantMassLineEdit->text().toDouble();
   }
   else // "Fraction"
   {
      mEngineSpecs[mNumBaseSpecs + 8] = mUIPtr->propellantMassLineEdit->text().toDouble();
   }

   // Mass
   if (QString::compare(massSpecification, "Empty") == 0)
   {
      mEngineSpecs[mNumBaseSpecs + 9] = mUIPtr->massLineEdit->text().toDouble();
   }
   else // "Total"
   {
      mEngineSpecs[mNumBaseSpecs + 10] = mUIPtr->massLineEdit->text().toDouble();
      mEngineSpecs[2] = mUIPtr->massLineEdit->text().toDouble();
   }

   emit EngineSpecsChanged();
}

void Designer::SolidRocketEngineWidget::PopulateLineEdits()
{
   QString specifiedVariables = mEngineSpecsOptions.at(mNumBaseSpecsOptions + 0);
   QString propMassSpec = mEngineSpecsOptions.at(mNumBaseSpecsOptions + 3);
   QString altSpec = mEngineSpecsOptions.at(mNumBaseSpecsOptions + 2);

   // Combo Boxes
   mUIPtr->specificationsComboBox->setCurrentText(specifiedVariables);
   mUIPtr->propellantMassComboBox->setCurrentText(propMassSpec);
   mUIPtr->altitudeComboBox->setCurrentText(altSpec);

   // Mass
   QString massSpecificationOption = mEngineSpecsOptions.at(mNumBaseSpecsOptions + 1);
   if (QString::compare(massSpecificationOption, "Empty") == 0)
   {
      mUIPtr->massComboBox->setCurrentText("Empty");
      mUIPtr->massLineEdit->setText(QString::number(mEngineSpecs.at(mNumBaseSpecs + 9)));
   }
   else
   {
      mUIPtr->massComboBox->setCurrentText("Total");
      mUIPtr->massLineEdit->setText(QString::number(mEngineSpecs.at(mNumBaseSpecs + 10)));
   }

   // Isp
   mUIPtr->specificImpulseSeaLevelLineEdit->setText(QString::number(mEngineSpecs.at(mNumBaseSpecs + 0)));
   mUIPtr->specificImpulseVacuumLineEdit->setText(QString::number(mEngineSpecs.at(mNumBaseSpecs + 1)));

   // Ramp-up / Ramp-down Time
   mUIPtr->rampUpTimeLineEdit->setText(QString::number(mEngineSpecs.at(mNumBaseSpecs + 2)));
   mUIPtr->rampDownTimeLineEdit->setText(QString::number(mEngineSpecs.at(mNumBaseSpecs + 3)));

   // Thrust, Burn Time, Propellant Mass
   mUIPtr->maxRampUpThrustLineEdit->setText(QString::number(mEngineSpecs.at(mNumBaseSpecs + 4)));
   mUIPtr->maxRampDownThrustLineEdit->setText(QString::number(mEngineSpecs.at(mNumBaseSpecs + 5)));

   mUIPtr->burnTimeLineEdit->setText(QString::number(mEngineSpecs.at(mNumBaseSpecs + 6)));

   if (QString::compare(propMassSpec, "Absolute") == 0)
   {
      mUIPtr->propellantMassComboBox->setCurrentText("Absolute");
      mUIPtr->propellantMassLineEdit->setText(QString::number(mEngineSpecs.at(mNumBaseSpecs + 7)));
      mUIPtr->propellantMassUnitLabel->setText("lbs");
   }
   else
   {
      mUIPtr->propellantMassComboBox->setCurrentText("Fraction");
      mUIPtr->propellantMassLineEdit->setText(QString::number(mEngineSpecs.at(mNumBaseSpecs + 8)));
      mUIPtr->propellantMassUnitLabel->clear();
   }

   HandleSpecificationsComboBoxChanged(specifiedVariables);
}

void Designer::SolidRocketEngineWidget::HandleSpecificationsComboBoxChanged(const QString& aSpecifications)
{
   // When the Specified Characteristics change, enable/disable appropriate widgets
   mEngineSpecsOptions[mNumBaseSpecsOptions + 0] = aSpecifications;

   // Rather than calling setEnabled / setDisabled on the line edits, simply set it
   // to read only and change the color palette so that the text is still visible

   QPalette defaultPalette = MoverCreatorMainWindow::GetInstance().GetPalette();

   QPalette palette = defaultPalette;
   palette.setColor(QPalette::Base, defaultPalette.color(QPalette::Disabled, QPalette::Base));

   if (QString::compare(aSpecifications, "Thrust and Burn Time") == 0)
   {
      mUIPtr->maxRampUpThrustLineEdit->setReadOnly(false);
      mUIPtr->maxRampDownThrustLineEdit->setReadOnly(false);
      mUIPtr->burnTimeLineEdit->setReadOnly(false);
      mUIPtr->propellantMassLineEdit->setReadOnly(true); // Disable Propellant Mass

      mUIPtr->maxRampUpThrustLineEdit->setPalette(defaultPalette);
      mUIPtr->maxRampDownThrustLineEdit->setPalette(defaultPalette);
      mUIPtr->burnTimeLineEdit->setPalette(defaultPalette);
      mUIPtr->propellantMassLineEdit->setPalette(palette);
   }
   else if (QString::compare(aSpecifications, "Thrust and Propellant Mass") == 0)
   {
      mUIPtr->maxRampUpThrustLineEdit->setReadOnly(false);
      mUIPtr->maxRampDownThrustLineEdit->setReadOnly(false);
      mUIPtr->burnTimeLineEdit->setReadOnly(true); // Disable Burn Time
      mUIPtr->propellantMassLineEdit->setReadOnly(false);

      mUIPtr->maxRampUpThrustLineEdit->setPalette(defaultPalette);
      mUIPtr->maxRampDownThrustLineEdit->setPalette(defaultPalette);
      mUIPtr->burnTimeLineEdit->setPalette(palette);
      mUIPtr->propellantMassLineEdit->setPalette(defaultPalette);
   }
   else if (QString::compare(aSpecifications, "Burn Time and Propellant Mass") == 0)
   {
      mUIPtr->maxRampUpThrustLineEdit->setReadOnly(true); // Disable Max Thrust
      mUIPtr->maxRampDownThrustLineEdit->setReadOnly(true);// Disable Max Thrust
      mUIPtr->burnTimeLineEdit->setReadOnly(false);
      mUIPtr->propellantMassLineEdit->setReadOnly(false);

      mUIPtr->maxRampUpThrustLineEdit->setPalette(palette);
      mUIPtr->maxRampDownThrustLineEdit->setPalette(palette);
      mUIPtr->burnTimeLineEdit->setPalette(defaultPalette);
      mUIPtr->propellantMassLineEdit->setPalette(defaultPalette);
   }
}

void Designer::SolidRocketEngineWidget::HandleMassComboBoxChanged(const QString& aMassSpecification)
{
   mEngineSpecsOptions[mNumBaseSpecsOptions + 1] = aMassSpecification;
   emit EngineSpecsChanged();
}

void Designer::SolidRocketEngineWidget::HandleAltitudeComboBoxChanged(const QString& aAltSpecification)
{
   mEngineSpecsOptions[mNumBaseSpecsOptions + 2] = aAltSpecification;
   emit EngineSpecsChanged();
}

void Designer::SolidRocketEngineWidget::HandlePropellantMassComboBoxChanged(const QString& aPropellantMassSpecification)
{
   mEngineSpecsOptions[mNumBaseSpecsOptions + 3] = aPropellantMassSpecification;

   if (QString::compare(aPropellantMassSpecification, "Absolute") == 0)
   {
      mUIPtr->propellantMassUnitLabel->setText("lbs");
   }
   else if (QString::compare(aPropellantMassSpecification, "Fraction") == 0)
   {
      mUIPtr->propellantMassUnitLabel->clear();
   }
   emit EngineSpecsChanged();
}
