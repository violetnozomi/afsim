// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef WKFUNITUPDATERS_HPP
#define WKFUNITUPDATERS_HPP

#include <sstream>

#include "UtLatPos.hpp"
#include "UtLonPos.hpp"
#include "WkfEnvironment.hpp"
#include "WkfUnitsObject.hpp"
#include "WkfUpdater.hpp"

namespace wkf
{
	template<class UNIT_TYPE, UnitPreference<UNIT_TYPE> UNIT_PREF, UnitSignal<UNIT_TYPE> UNIT_SIGNAL>
	class UnitaryValueUpdaterT : public UnitlessValueUpdater
	{
		using BaseUnit = typename UNIT_TYPE::BaseUnit;

	public:
		UnitaryValueUpdaterT(const QString& aUpdaterName, const std::string& aPlatformName)
			: UnitlessValueUpdater(aUpdaterName, aPlatformName)
		{
			auto* unitPrefObject = wkfEnv.GetPreferenceObject<UnitsObject>();
			mConnection = QObject::connect(unitPrefObject, UNIT_SIGNAL, [this](BaseUnit unit) { mUnit = unit; });
			mUnit = (unitPrefObject->*UNIT_PREF)();
		}

		~UnitaryValueUpdaterT() override { QObject::disconnect(mConnection); }

		QString GetValueString() const override
		{
			auto unitsObject = wkfEnv.GetPreferenceObject<UnitsObject>();
			return unitsObject->GetFormattedValue(static_cast<ValueType>(UNIT_TYPE::cUNIT_TYPE_ID), GetValueInDefaultUnits());
		}

		double GetValueDouble() const override
		{
			UtUnitaryValue<UNIT_TYPE> val(GetValueInDefaultUnits());
			return val.GetAsUnit(mUnit);
		}

	protected:
		virtual double GetValueInDefaultUnits() const { return mValue; }

	private:
		BaseUnit                mUnit;
		QMetaObject::Connection mConnection;
	};

	//! Template to handle special case of altitude values
	template<UnitPreference<UtUnitLength> UNIT_PREF, UnitSignal<UtUnitLength> UNIT_SIGNAL>
	class AltitudeUpdaterT : public UnitaryValueUpdaterT<UtUnitLength, UNIT_PREF, UNIT_SIGNAL>
	{
		using BaseUpdater = UnitaryValueUpdaterT<UtUnitLength, UNIT_PREF, UNIT_SIGNAL>;

	public:
		using BaseUpdater::BaseUpdater;

		QString GetValueString() const override
		{
			auto unitsObject = wkfEnv.GetPreferenceObject<UnitsObject>();
			return unitsObject->GetFormattedValue(ValueType::cALTITUDE, BaseUpdater::GetValueInDefaultUnits());
		}
	};

	template<class FORMAT_TYPE, FormatPreference FORMAT_PREF>
	class FormattedValueUpdaterT : public UnitlessValueUpdater
	{
	public:
		FormattedValueUpdaterT(const QString& aUpdaterName, const std::string& aPlatformName)
			: UnitlessValueUpdater(aUpdaterName, aPlatformName)
		{
		}

		QString GetValueString() const override
		{
			FORMAT_TYPE val(GetValueDouble(), (wkfEnv.GetPreferenceObject<UnitsObject>()->*FORMAT_PREF)());

			std::ostringstream oss;
			oss << val;
			return QString::fromStdString(oss.str());
		}
	};

	using AccelerationValueUpdater =
		UnitaryValueUpdaterT<UtUnitAcceleration, &UnitsObject::GetAccelerationUnit, &UnitsObject::AccelerationUnitChanged>;
	using LowAltitudeValueUpdater = AltitudeUpdaterT<&UnitsObject::GetLowAltitudeUnit, &UnitsObject::LowAltitudeUnitChanged>;
	using HighAltitudeValueUpdater =
		AltitudeUpdaterT<&UnitsObject::GetHighAltitudeUnit, &UnitsObject::HighAltitudeUnitChanged>;
	using AltitudeValueUpdater = LowAltitudeValueUpdater;
	using AngleValueUpdater = UnitaryValueUpdaterT<UtUnitAngle, &UnitsObject::GetAngleUnit, &UnitsObject::AngleUnitChanged>;
	using AngularAccelerationValueUpdater =
		UnitaryValueUpdaterT<UtUnitAngularAcceleration, &UnitsObject::GetAngularAccelerationUnit, &UnitsObject::AngularAccelerationUnitChanged>;
	using AngularRateValueUpdater =
		UnitaryValueUpdaterT<UtUnitAngularRate, &UnitsObject::GetAngularRateUnit, &UnitsObject::AngularRateUnitChanged>;
	using ForceValueUpdater = UnitaryValueUpdaterT<UtUnitForce, &UnitsObject::GetForceUnit, &UnitsObject::ForceUnitChanged>;
	using FrequencyValueUpdater =
		UnitaryValueUpdaterT<UtUnitFrequency, &UnitsObject::GetFrequencyUnit, &UnitsObject::FrequencyUnitChanged>;
	using LengthValueUpdater =
		UnitaryValueUpdaterT<UtUnitLength, &UnitsObject::GetLengthUnit, &UnitsObject::LengthUnitChanged>;
	using MassValueUpdater = UnitaryValueUpdaterT<UtUnitMass, &UnitsObject::GetMassUnit, &UnitsObject::MassUnitChanged>;
	using PowerValueUpdater = UnitaryValueUpdaterT<UtUnitPower, &UnitsObject::GetPowerUnit, &UnitsObject::PowerUnitChanged>;
	using PowerDBValueUpdater =
		UnitaryValueUpdaterT<UtUnitPowerDB, &UnitsObject::GetPowerDBUnit, &UnitsObject::PowerDBUnitChanged>;
	using PressureValueUpdater =
		UnitaryValueUpdaterT<UtUnitPressure, &UnitsObject::GetPressureUnit, &UnitsObject::PressureUnitChanged>;
	using RatioValueUpdater = UnitaryValueUpdaterT<UtUnitRatio, &UnitsObject::GetRatioUnit, &UnitsObject::RatioUnitChanged>;
	using SpeedValueUpdater = UnitaryValueUpdaterT<UtUnitSpeed, &UnitsObject::GetSpeedUnit, &UnitsObject::SpeedUnitChanged>;
	using TimeValueUpdater = UnitaryValueUpdaterT<UtUnitTime, &UnitsObject::GetTimeUnit, &UnitsObject::TimeUnitChanged>;

	using LatitudeValueUpdater = FormattedValueUpdaterT<UtLatPos, &UnitsObject::GetLatLonFormatting>;
	using LongitudeValueUpdater = FormattedValueUpdaterT<UtLonPos, &UnitsObject::GetLatLonFormatting>;
} // namespace wkf

#endif
