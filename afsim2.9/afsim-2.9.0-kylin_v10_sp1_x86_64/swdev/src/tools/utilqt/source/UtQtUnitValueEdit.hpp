// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef UTQTUNITVALUEEDIT_HPP
#define UTQTUNITVALUEEDIT_HPP

#include "utilqt_export.h"

#include <sstream>

#include <QLineEdit>

#include "UtQtText.hpp"
#include "UtUnitTypes.hpp"
#include "UtUnits.hpp"

// A line edit for any unit-value
class UTILQT_EXPORT UtQtUnitValueEdit : public QLineEdit
{
   Q_OBJECT

public:
   UtQtUnitValueEdit(QWidget* aParent = nullptr, UtUnits::UnitType aUnitType = static_cast<UtUnits::UnitType>(0));

   void        SetUnitType(UtUnits::UnitType aUnitType) { SetUnitType(UtUnits::GetUnitType(aUnitType)); }
   void        SetUnitType(UtUnitType* aUnitType);
   UtUnitType* GetUnitType() const { return mUnitTypePtr; }

   template<typename UNIT_VALUE>
   void SetBounds(const UNIT_VALUE& aMinVal, const UNIT_VALUE& aMaxVal)
   {
      mMinBound = aMinVal;
      mMaxBound = aMaxVal;
   }
   void ClearBounds();

   // get as the specialized unit value
   // see UtLengthValue, etc...
   template<typename UNIT_VALUE>
   bool GetValue(UNIT_VALUE& aVal)
   {
      if (mIsValid)
      {
         aVal.Set(mValue, mUnitId);
         return true;
      }
      else
      {
         return false;
      }
   }

   template<typename UNIT_VALUE>
   void SetValue(const UNIT_VALUE& aVal)
   {
      if (mUnitTypePtr == nullptr)
      { // set unit type if it's not set
         mUnitTypePtr = UtUnits::GetUnitType(static_cast<UtUnits::UnitType>(UNIT_VALUE::UnitType::cUNIT_TYPE_ID));
      }

      mUnitId = aVal.GetUnit();
      mValue  = aVal.GetAsUnit(mUnitId);
      ApplyBounds(mValue, mUnitId);

      std::ostringstream oss;
      oss << UNIT_VALUE(mValue, mUnitId);

      QString newTxt(oss.str().c_str());
      if (newTxt != text())
      {
         bool block = blockSignals(true);
         setText(newTxt);
         blockSignals(block);
      }

      mIsValid = true;
      UpdateValidityColor();
   }

   template<typename UNIT_VALUE>
   void SetPlaceholderValue(const UNIT_VALUE& aVal)
   {
      std::stringstream ss;
      ss << aVal;
      setPlaceholderText(ss.str().c_str());
      UpdateValidityColor();
   }

   bool IsValid() const { return mIsValid; }

   void UpdateValidityColor()
   {
      if (text().isEmpty() && !placeholderText().isEmpty())
      {
         UtQtText::SetValid(true, *this);
      }
      else
      {
         UtQtText::SetValid(mIsValid, *this);
      }
   }

signals:
   void ValueChanged();

protected:
   void HandleTextChanged(const QString& aValue);

   void contextMenuEvent(QContextMenuEvent*) override;

   bool ApplyBounds(double& aValue, const int aUnit);

   bool        mIsValid{false};
   int         mUnitId{0};
   UtUnitType* mUnitTypePtr{nullptr};
   double      mValue{0.0}; // Units are the same as mUnitId

   double mMinBound{std::numeric_limits<double>::lowest()}; // Units are the standard unit
   double mMaxBound{std::numeric_limits<double>::max()};    // Units are the standard unit
};

// A line edit for any unit-value
template<typename UNIT_VALUE>
class UtQtUnitValueEditT : public UtQtUnitValueEdit
{
public:
   UtQtUnitValueEditT(QWidget* parentPtr)
      : UtQtUnitValueEdit(parentPtr, static_cast<UtUnits::UnitType>(UNIT_VALUE::UnitType::cUNIT_TYPE_ID))
   {
   }

   void SetPlaceholderValue(const UNIT_VALUE& aVal) { UtQtUnitValueEdit::SetPlaceholderValue(aVal); }

   void SetValue(const UNIT_VALUE& aVal) { UtQtUnitValueEdit::SetValue(aVal); }

   UNIT_VALUE GetValue()
   {
      UNIT_VALUE v;
      UtQtUnitValueEdit::GetValue(v);
      return v;
   }

private:
   using UtQtUnitValueEdit::SetUnitType; // hide
};

using UtQtLengthValueEdit              = UtQtUnitValueEditT<UtLengthValue>;
using UtQtTimeValueEdit                = UtQtUnitValueEditT<UtTimeValue>;
using UtQtSpeedValueEdit               = UtQtUnitValueEditT<UtSpeedValue>;
using UtQtDataSizeValueEdit            = UtQtUnitValueEditT<UtDataSizeValue>;
using UtQtPowerValueEdit               = UtQtUnitValueEditT<UtPowerValue>;
using UtQtPowerDBValueEdit             = UtQtUnitValueEditT<UtPowerDBValue>;
using UtQtSolidAngleValueEdit          = UtQtUnitValueEditT<UtSolidAngleValue>;
using UtQtMassValueEdit                = UtQtUnitValueEditT<UtMassValue>;
using UtQtForceValueEdit               = UtQtUnitValueEditT<UtForceValue>;
using UtQtTorqueValueEdit              = UtQtUnitValueEditT<UtTorqueValue>;
using UtQtAreaValueEdit                = UtQtUnitValueEditT<UtAreaValue>;
using UtQtAreaDBValueEdit              = UtQtUnitValueEditT<UtAreaDBValue>;
using UtQtVolumeValueEdit              = UtQtUnitValueEditT<UtVolumeValue>;
using UtQtTime2ValueEdit               = UtQtUnitValueEditT<UtTime2Value>;
using UtQtAccelerationValueEdit        = UtQtUnitValueEditT<UtAccelerationValue>;
using UtQtFrequencyValueEdit           = UtQtUnitValueEditT<UtFrequencyValue>;
using UtQtAngleValueEdit               = UtQtUnitValueEditT<UtAngleValue>;
using UtQtAngularRateValueEdit         = UtQtUnitValueEditT<UtAngularRateValue>;
using UtQtAngularAccelerationValueEdit = UtQtUnitValueEditT<UtAngularAccelerationValue>;
using UtQtDataRateValueEdit            = UtQtUnitValueEditT<UtDataRateValue>;
using UtQtMassDensityValueEdit         = UtQtUnitValueEditT<UtMassDensityValue>;
using UtQtMassTransferValueEdit        = UtQtUnitValueEditT<UtMassTransferValue>;
using UtQtEnergyValueEdit              = UtQtUnitValueEditT<UtEnergyValue>;
using UtQtFluenceValueEdit             = UtQtUnitValueEditT<UtFluenceValue>;
using UtQtIrradianceValueEdit          = UtQtUnitValueEditT<UtIrradianceValue>;
using UtQtRatioValueEdit               = UtQtUnitValueEditT<UtRatioValue>;
using UtQtNoisePressureValueEdit       = UtQtUnitValueEditT<UtNoisePressureValue>;
using UtQtPressureValueEdit            = UtQtUnitValueEditT<UtPressureValue>;
using UtQtTemperatureValueEdit         = UtQtUnitValueEditT<UtTemperatureValue>;
using UtQtSpecificRangeValueEdit       = UtQtUnitValueEditT<UtSpecificRangeValue>;
using UtQtAngularInertiaValueEdit      = UtQtUnitValueEditT<UtAngularInertiaValue>;

#endif
