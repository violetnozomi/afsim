// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef WKFUNITCOMBOBOX_HPP
#define WKFUNITCOMBOBOX_HPP

#include <QComboBox>

#include "UtUnitTypes.hpp"
#include "WkfEnvironment.hpp"
#include "WkfUnitsObject.hpp"

namespace wkf
{
class UnitComboBox : public QComboBox
{
public:
   UnitComboBox(QWidget* parent = nullptr)
      : QComboBox(parent)
   {
   }
   ~UnitComboBox() override = default;

   virtual double ConvertToStandard(double aValue) = 0;
};

template<class UNIT_TYPE,
         void (UnitsObject::*UNIT_SIGNAL)(typename UNIT_TYPE::BaseUnit),
         typename UNIT_TYPE::BaseUnit (UnitsObject::*DEFAULT_UNIT)() const>
class UnitComboBoxT : public UnitComboBox
{
public:
   UnitComboBoxT(QWidget* parent = nullptr)
      : UnitComboBox(parent)
      , mInitialized(false)
   {
   }

   double ConvertToStandard(double aValue) override
   {
      if (!mInitialized)
      {
         Initialize();
      }
      return UNIT_TYPE::ConvertToStandard(aValue, currentData().toInt());
   }

protected:
   void showEvent(QShowEvent* aEvent) override
   {
      if (!mInitialized)
      {
         Initialize();
      }
   }

   void Initialize()
   {
      for (const auto& unit : UNIT_TYPE::mUnitToString)
      {
         addItem(QString::fromStdString(unit.second), unit.first);
      }

      // Initialize the current index to the item whose userData matches the unit preferences
      auto* unitPrefObject = wkfEnv.GetPreferenceObject<UnitsObject>();
      setCurrentIndex(findData((unitPrefObject->*DEFAULT_UNIT)()));

      connect(unitPrefObject,
              UNIT_SIGNAL,
              [this](typename UNIT_TYPE::BaseUnit aUnit) { setCurrentIndex(findData(aUnit)); });
      mInitialized = true;
   }

   bool mInitialized;
};

using AccelerationComboBox =
   UnitComboBoxT<UtUnitAcceleration, &UnitsObject::AccelerationUnitChanged, &UnitsObject::GetAccelerationUnit>;
using AltitudeComboBox =
   UnitComboBoxT<UtUnitLength, &UnitsObject::LowAltitudeUnitChanged, &UnitsObject::GetLowAltitudeUnit>;
using AngleComboBox = UnitComboBoxT<UtUnitAngle, &UnitsObject::AngleUnitChanged, &UnitsObject::GetAngleUnit>;
using AngularRateComboBox =
   UnitComboBoxT<UtUnitAngularRate, &UnitsObject::AngularRateUnitChanged, &UnitsObject::GetAngularRateUnit>;
using AngularAccelerationComboBox =
   UnitComboBoxT<UtUnitAngularAcceleration, &UnitsObject::AngularAccelerationUnitChanged, &UnitsObject::GetAngularAccelerationUnit>;
using LengthComboBox   = UnitComboBoxT<UtUnitLength, &UnitsObject::LengthUnitChanged, &UnitsObject::GetLengthUnit>;
using SpeedComboBox    = UnitComboBoxT<UtUnitSpeed, &UnitsObject::SpeedUnitChanged, &UnitsObject::GetSpeedUnit>;
using PressureComboBox = UnitComboBoxT<UtUnitPressure, &UnitsObject::PressureUnitChanged, &UnitsObject::GetPressureUnit>;
using RatioComboBox    = UnitComboBoxT<UtUnitRatio, &UnitsObject::RatioUnitChanged, &UnitsObject::GetRatioUnit>;
using TimeComboBox     = UnitComboBoxT<UtUnitTime, &UnitsObject::TimeUnitChanged, &UnitsObject::GetTimeUnit>;
} // namespace wkf
#endif // WKFUNITCOMBOBOX_HPP
