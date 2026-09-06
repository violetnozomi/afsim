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

#include "UnitConverterDialog.hpp"

#include <QMessageBox>
#include <QSizePolicy>

UnitConverter::Dialog::Dialog(QWidget* aParent, Qt::WindowFlags aFlags)
   : QDialog(aParent, aFlags)
{
   mUi.setupUi(this);

   // Populate unit type combobox with all unit types
   for (auto& unitType : UtUnits::mUnitTypes)
   {
      // Only unit types that have more than one unit to convert to are added.
      if (unitType && (unitType->GetUnitToString().size() > 1))
      {
         mUi.unitTypesComboBox->addItem(QString::fromStdString(unitType->GetName()));
      }
   }

   // Populates maps of function pointers
   SetupMaps();

   connect(mUi.unitTypesComboBox, QOverload<const QString&>::of(&QComboBox::currentIndexChanged), this, &Dialog::UnitTypeChanged);
   connect(mUi.valueLineEdit, &QLineEdit::textChanged, this, &Dialog::Convert);
   connect(mUi.convertToComboBox, QOverload<const QString&>::of(&QComboBox::currentIndexChanged), this, &Dialog::Convert);
   connect(mUi.copyPushButton,
           &QPushButton::pressed,
           [=]()
           {
              mUi.resultLineEdit->selectAll();
              mUi.resultLineEdit->copy();
              mUi.resultLineEdit->deselect();
           });

   // Initialize the dialog to length conversions
   UnitTypeChanged("LENGTH");
}

void UnitConverter::Dialog::SetupMaps()
{
   // Repopulate map
   mRepopulateMap["LENGTH"]       = std::bind(&Dialog::Repopulate<UtLengthValue, UtUnitLength>, this);
   mRepopulateMap["TIME"]         = std::bind(&Dialog::Repopulate<UtTimeValue, UtUnitTime>, this);
   mRepopulateMap["SPEED"]        = std::bind(&Dialog::Repopulate<UtSpeedValue, UtUnitSpeed>, this);
   mRepopulateMap["DATA_SIZE"]    = std::bind(&Dialog::Repopulate<UtDataSizeValue, UtUnitDataSize>, this);
   mRepopulateMap["POWER"]        = std::bind(&Dialog::Repopulate<UtPowerValue, UtUnitPower>, this);
   mRepopulateMap["POWER_DB"]     = std::bind(&Dialog::Repopulate<UtPowerDBValue, UtUnitPowerDB>, this);
   mRepopulateMap["MASS"]         = std::bind(&Dialog::Repopulate<UtMassValue, UtUnitMass>, this);
   mRepopulateMap["FORCE"]        = std::bind(&Dialog::Repopulate<UtForceValue, UtUnitForce>, this);
   mRepopulateMap["AREA"]         = std::bind(&Dialog::Repopulate<UtAreaValue, UtUnitArea>, this);
   mRepopulateMap["AREA_DB"]      = std::bind(&Dialog::Repopulate<UtAreaDBValue, UtUnitAreaDB>, this);
   mRepopulateMap["TIME2"]        = std::bind(&Dialog::Repopulate<UtTime2Value, UtUnitTime2>, this);
   mRepopulateMap["ACCELERATION"] = std::bind(&Dialog::Repopulate<UtAccelerationValue, UtUnitAcceleration>, this);
   mRepopulateMap["FREQUENCY"]    = std::bind(&Dialog::Repopulate<UtFrequencyValue, UtUnitFrequency>, this);
   mRepopulateMap["ANGLE"]        = std::bind(&Dialog::Repopulate<UtAngleValue, UtUnitAngle>, this);
   mRepopulateMap["ANGULAR_RATE"] = std::bind(&Dialog::Repopulate<UtAngularRateValue, UtUnitAngularRate>, this);
   mRepopulateMap["ANGULAR_ACCELERATION"] =
      std::bind(&Dialog::Repopulate<UtAngularAccelerationValue, UtUnitAngularAcceleration>, this);
   mRepopulateMap["ENERGY"]          = std::bind(&Dialog::Repopulate<UtEnergyValue, UtUnitEnergy>, this);
   mRepopulateMap["RATIO"]           = std::bind(&Dialog::Repopulate<UtRatioValue, UtUnitRatio>, this);
   mRepopulateMap["NOISE_PRESSURE"]  = std::bind(&Dialog::Repopulate<UtNoisePressureValue, UtUnitNoisePressure>, this);
   mRepopulateMap["PRESSURE"]        = std::bind(&Dialog::Repopulate<UtPressureValue, UtUnitPressure>, this);
   mRepopulateMap["TEMPERATURE"]     = std::bind(&Dialog::Repopulate<UtTemperatureValue, UtUnitTemperature>, this);
   mRepopulateMap["SPECIFIC_RANGE"]  = std::bind(&Dialog::Repopulate<UtSpecificRangeValue, UtUnitSpecificRange>, this);
   mRepopulateMap["ANGULAR_INERTIA"] = std::bind(&Dialog::Repopulate<UtAngularInertiaValue, UtUnitAngularInertia>, this);
   mRepopulateMap["VOLTAGE"]         = std::bind(&Dialog::Repopulate<UtVoltageValue, UtUnitVoltage>, this);
   mRepopulateMap["CURRENT"]         = std::bind(&Dialog::Repopulate<UtCurrentValue, UtUnitCurrent>, this);
   mRepopulateMap["RESISTANCE"]      = std::bind(&Dialog::Repopulate<UtResistanceValue, UtUnitResistance>, this);
   mRepopulateMap["CAPACITANCE"]     = std::bind(&Dialog::Repopulate<UtCapacitanceValue, UtUnitCapacitance>, this);

   // Convert map
   mConvertMap["LENGTH"]       = std::bind(&Dialog::ConvertToUnit<UtLengthValue, UtUnitLength>, this);
   mConvertMap["TIME"]         = std::bind(&Dialog::ConvertToUnit<UtTimeValue, UtUnitTime>, this);
   mConvertMap["SPEED"]        = std::bind(&Dialog::ConvertToUnit<UtSpeedValue, UtUnitSpeed>, this);
   mConvertMap["DATA_SIZE"]    = std::bind(&Dialog::ConvertToUnit<UtDataSizeValue, UtUnitDataSize>, this);
   mConvertMap["POWER"]        = std::bind(&Dialog::ConvertToUnit<UtPowerValue, UtUnitPower>, this);
   mConvertMap["POWER_DB"]     = std::bind(&Dialog::ConvertToUnit<UtPowerDBValue, UtUnitPowerDB>, this);
   mConvertMap["MASS"]         = std::bind(&Dialog::ConvertToUnit<UtMassValue, UtUnitMass>, this);
   mConvertMap["FORCE"]        = std::bind(&Dialog::ConvertToUnit<UtForceValue, UtUnitForce>, this);
   mConvertMap["AREA"]         = std::bind(&Dialog::ConvertToUnit<UtAreaValue, UtUnitArea>, this);
   mConvertMap["AREA_DB"]      = std::bind(&Dialog::ConvertToUnit<UtAreaDBValue, UtUnitAreaDB>, this);
   mConvertMap["TIME2"]        = std::bind(&Dialog::ConvertToUnit<UtTime2Value, UtUnitTime2>, this);
   mConvertMap["ACCELERATION"] = std::bind(&Dialog::ConvertToUnit<UtAccelerationValue, UtUnitAcceleration>, this);
   mConvertMap["FREQUENCY"]    = std::bind(&Dialog::ConvertToUnit<UtFrequencyValue, UtUnitFrequency>, this);
   mConvertMap["ANGLE"]        = std::bind(&Dialog::ConvertToUnit<UtAngleValue, UtUnitAngle>, this);
   mConvertMap["ANGULAR_RATE"] = std::bind(&Dialog::ConvertToUnit<UtAngularRateValue, UtUnitAngularRate>, this);
   mConvertMap["ANGULAR_ACCELERATION"] =
      std::bind(&Dialog::ConvertToUnit<UtAngularAccelerationValue, UtUnitAngularAcceleration>, this);
   mConvertMap["ENERGY"]          = std::bind(&Dialog::ConvertToUnit<UtEnergyValue, UtUnitEnergy>, this);
   mConvertMap["RATIO"]           = std::bind(&Dialog::ConvertToUnit<UtRatioValue, UtUnitRatio>, this);
   mConvertMap["NOISE_PRESSURE"]  = std::bind(&Dialog::ConvertToUnit<UtNoisePressureValue, UtUnitNoisePressure>, this);
   mConvertMap["PRESSURE"]        = std::bind(&Dialog::ConvertToUnit<UtPressureValue, UtUnitPressure>, this);
   mConvertMap["TEMPERATURE"]     = std::bind(&Dialog::ConvertToUnit<UtTemperatureValue, UtUnitTemperature>, this);
   mConvertMap["SPECIFIC_RANGE"]  = std::bind(&Dialog::ConvertToUnit<UtSpecificRangeValue, UtUnitSpecificRange>, this);
   mConvertMap["ANGULAR_INERTIA"] = std::bind(&Dialog::ConvertToUnit<UtAngularInertiaValue, UtUnitAngularInertia>, this);
   mConvertMap["VOLTAGE"]         = std::bind(&Dialog::ConvertToUnit<UtVoltageValue, UtUnitVoltage>, this);
   mConvertMap["CURRENT"]         = std::bind(&Dialog::ConvertToUnit<UtCurrentValue, UtUnitCurrent>, this);
   mConvertMap["RESISTANCE"]      = std::bind(&Dialog::ConvertToUnit<UtResistanceValue, UtUnitResistance>, this);
   mConvertMap["CAPACITANCE"]     = std::bind(&Dialog::ConvertToUnit<UtCapacitanceValue, UtUnitCapacitance>, this);
}

void UnitConverter::Dialog::UnitTypeChanged(const QString& aString)
{
   mCurrentUnits = mUi.unitTypesComboBox->currentText();

   // search map and find function pointer to repopulate
   if (mRepopulateMap.find(mCurrentUnits) != mRepopulateMap.end())
   {
      mRepopulateMap[aString]();
   }
   else
   {
      // Warning, specified units are not currently supported
      QString unitMsg = "'" + mCurrentUnits + "' units are not currently supported.";
      QMessageBox::warning(this, "Unsupported Unit Type", unitMsg);
   }
}

void UnitConverter::Dialog::Convert()
{
   // search map and find function pointer to convert
   if (mConvertMap.find(mCurrentUnits) != mConvertMap.end())
   {
      mConvertMap[mCurrentUnits]();
   }
   else // the method in mConvertMap will have found and reported invalid input already
   {
      mUi.resultLineEdit->clear();
      mUi.resultLineEdit->setPlaceholderText("Unsupported unit: " + mCurrentUnits);
   }
}

// Repopulates the "Convert To" combobox with appropriate units
template<typename T, typename T2>
void UnitConverter::Dialog::Repopulate()
{
   // temperarily block signals from convertToCombobox while it's being repopulated.
   mUi.convertToComboBox->blockSignals(true);
   mUi.resultLineEdit->clear();

   // Remove old UtQtLineEdit if one exists
   for (auto& child : children())
   {
      if (child->objectName() == "valueLineEdit")
      {
         delete child;
      }
   }

   // Create a new UtQtLineEdit according to the specified unit type
   UtQtUnitValueEdit* ln = new UtQtUnitValueEditT<T>(this);
   ln->setObjectName("valueLineEdit");
   mUi.gridLayout->addWidget(ln, 4, 1, 1, 3);
   setTabOrder(mUi.convertToComboBox, ln);
   connect(ln, &UtQtUnitValueEdit::textChanged, this, &Dialog::ConvertToUnit<T, T2>);

   // Clear and repopulate the 'ConvertTo' combobox with appropriate units
   mUi.convertToComboBox->clear();
   for (auto& unit : T2::mUnitToString)
   {
      mUi.convertToComboBox->addItem(QString::fromStdString(unit.second));
   }
   mUi.convertToComboBox->blockSignals(false);
}

// Converts from the specified unit to the desired units
template<typename T, typename T2>
void UnitConverter::Dialog::ConvertToUnit()
{
   double      value = 0.0;
   std::string fromUnitStr;
   for (auto& child : children())
   {
      if (child->objectName() == "valueLineEdit")
      {
         UtQtUnitValueEditT<T>* ln = dynamic_cast<UtQtUnitValueEditT<T>*>(child);
         if (ln)
         {
            if (ln->IsValid())
            {
               T unitValue = ln->GetValue();
               fromUnitStr = unitValue.GetUnitName();
               value       = unitValue.GetAsUnit();

               QString toUnitStr = mUi.convertToComboBox->currentText();

               int fromUnitInt = T2::ReadUnit(fromUnitStr);
               int toUnitInt   = T2::ReadUnit(toUnitStr.toStdString());

               double standardValue = T2::ConvertToStandard(value, fromUnitInt);
               double resultValue   = T2::ConvertFromStandard(standardValue, toUnitInt);

               mUi.resultLineEdit->setText(QString::number(resultValue) + " " + toUnitStr);
            }
            else if (!ln->text().isEmpty())
            {
               mUi.resultLineEdit->clear();
               mUi.resultLineEdit->setPlaceholderText("Invalid Input");
            }
         }
      }
   }
}
