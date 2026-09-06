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

#ifndef WKFUNITSOBJECT_HPP
#define WKFUNITSOBJECT_HPP

#include "wkf_export.h"

#include <unordered_map>

#include "UtAngle.hpp"
#include "UtTime.hpp"
#include "UtUnitTypes.hpp"
#include "WkfPrefObject.hpp"

//! Helper macro for returning the name of the users preferred unit for a given type (as a QString).
//! Example: mLabel->setText(WKF_UNIT_NAME(Length));
#define WKF_UNIT_NAME(TYPE) \
   QString::fromStdString(UtUnit##TYPE::FindUnitName(wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->Get##TYPE##Unit()))

//! Helper macro for converting a value from standard units to the user's preferred units.
//! Example: mSpinBox->setValue(WKF_UNIT_FROM_STANDARD(Length, altitude_m));
#define WKF_UNIT_FROM_STANDARD(TYPE, VALUE) \
   UtUnit##TYPE::ConvertFromStandard(VALUE, wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->Get##TYPE##Unit())

namespace wkf
{
struct UnitsData
{
   template<typename BaseUnitType>
   struct UnitInfo
   {
      UnitInfo(typename BaseUnitType::BaseUnit aUnit, unsigned int aPrecision)
         : mUnit(aUnit)
         , mPrecision(aPrecision)
      {
      }
      typename BaseUnitType::BaseUnit mUnit;
      unsigned int                    mPrecision;
   };

   template<typename T>
   using DefaultPrecisionMap = std::unordered_map<typename T::BaseUnit, unsigned int, std::hash<int>>;

   enum DateFormat
   {
      DATE_ISO,
      DATE_LONG,
      DATE_SHORT
   };

   static const DefaultPrecisionMap<UtUnitAcceleration>        mAccelerationPrecisionMap;
   static const DefaultPrecisionMap<UtUnitLength>              mAltitudePrecisionMap;
   static const DefaultPrecisionMap<UtUnitAngle>               mAnglePrecisionMap;
   static const DefaultPrecisionMap<UtUnitAngularRate>         mAngularRatePrecisionMap;
   static const DefaultPrecisionMap<UtUnitAngularAcceleration> mAngularAccelerationPrecisionMap;
   static const DefaultPrecisionMap<UtUnitForce>               mForcePrecisionMap;
   static const DefaultPrecisionMap<UtUnitFrequency>           mFrequencyPrecisionMap;
   static const DefaultPrecisionMap<UtUnitLength>              mLengthPrecisionMap;
   static const DefaultPrecisionMap<UtUnitMass>                mMassPrecisionMap;
   static const DefaultPrecisionMap<UtUnitMassTransfer>        mMassTransferPrecisionMap;
   static const DefaultPrecisionMap<UtUnitPower>               mPowerPrecisionMap;
   static const DefaultPrecisionMap<UtUnitPowerDB>             mPowerDBPrecisionMap;
   static const DefaultPrecisionMap<UtUnitPressure>            mPressurePrecisionMap;
   static const DefaultPrecisionMap<UtUnitRatio>               mRatioPrecisionMap;
   static const DefaultPrecisionMap<UtUnitSpeed>               mSpeedPrecisionMap;
   static const DefaultPrecisionMap<UtUnitTime>                mTimePrecisionMap;
   static const DefaultPrecisionMap<UtUnitTorque>              mTorquePrecisionMap;

   UnitInfo<UtUnitAcceleration> mAcceleration{UtUnitAcceleration::cMETERS_PER_SECOND2,
                                              mAccelerationPrecisionMap.at(UtUnitAcceleration::cMETERS_PER_SECOND2)};
   UnitInfo<UtUnitLength> mHighAltitude{UtUnitLength::cKILOMETERS, mAltitudePrecisionMap.at(UtUnitLength::cKILOMETERS)};
   UnitInfo<UtUnitLength> mLowAltitude{UtUnitLength::cFEET, mAltitudePrecisionMap.at(UtUnitLength::cFEET)};
   UnitInfo<UtUnitAngle>  mAngle{UtUnitAngle::cDEGREES, mAnglePrecisionMap.at(UtUnitAngle::cDEGREES)};
   UnitInfo<UtUnitAngularRate>         mAngularRate{UtUnitAngularRate::cRADIANS_PER_SECOND,
                                            mAngularRatePrecisionMap.at(UtUnitAngularRate::cRADIANS_PER_SECOND)};
   UnitInfo<UtUnitAngularAcceleration> mAngularAcceleration{UtUnitAngularAcceleration::cRADIANS_PER_SECOND2,
                                                            mAngularAccelerationPrecisionMap.at(
                                                               UtUnitAngularAcceleration::cRADIANS_PER_SECOND2)};
   UnitInfo<UtUnitForce>               mForce{UtUnitForce::cNEWTONS, mForcePrecisionMap.at(UtUnitForce::cNEWTONS)};
   UnitInfo<UtUnitFrequency>    mFrequency{UtUnitFrequency::cMHZ, mFrequencyPrecisionMap.at(UtUnitFrequency::cMHZ)};
   UnitInfo<UtUnitLength>       mLength{UtUnitLength::cMETERS, mLengthPrecisionMap.at(UtUnitLength::cMETERS)};
   UnitInfo<UtUnitMass>         mMass{UtUnitMass::cKILOGRAMS, mMassPrecisionMap.at(UtUnitMass::cKILOGRAMS)};
   UnitInfo<UtUnitMassTransfer> mMassTransfer{UtUnitMassTransfer::cKILOGRAMS_PER_SECOND,
                                              mMassTransferPrecisionMap.at(UtUnitMassTransfer::cKILOGRAMS_PER_SECOND)};
   UnitInfo<UtUnitPower>        mPower{UtUnitPower::cKILOWATTS, mPowerPrecisionMap.at(UtUnitPower::cKILOWATTS)};
   UnitInfo<UtUnitPowerDB>      mPowerDB{UtUnitPowerDB::cDBW, mPowerDBPrecisionMap.at(UtUnitPowerDB::cDBW)};
   UnitInfo<UtUnitPressure>     mPressure{UtUnitPressure::cPASCAL, mPressurePrecisionMap.at(UtUnitPressure::cPASCAL)};
   UnitInfo<UtUnitRatio>        mRatio{UtUnitRatio::cDB, mRatioPrecisionMap.at(UtUnitRatio::cDB)};
   UnitInfo<UtUnitSpeed> mSpeed{UtUnitSpeed::cMETERS_PER_SECOND, mSpeedPrecisionMap.at(UtUnitSpeed::cMETERS_PER_SECOND)};
   UnitInfo<UtUnitTime>  mTime{UtUnitTime::cSECONDS, mTimePrecisionMap.at(UtUnitTime::cSECONDS)};
   UnitInfo<UtUnitTorque> mTorque{UtUnitTorque::cNEWTON_METERS, mTorquePrecisionMap.at(UtUnitTorque::cNEWTON_METERS)};

   int                   mLatLonFormat{(UtAngle::FmtDMS + 2)};
   int                   mTimeFormat{(UtTime::FmtHMS + 2)};
   UnitsData::DateFormat mDateFormat{DateFormat::DATE_ISO};
};

enum class ValueType
{
   cACCELERATION         = UtUnits::cACCELERATION,
   cANGLE                = UtUnits::cANGLE,
   cANGULAR_RATE         = UtUnits::cANGULAR_RATE,
   cANGULAR_ACCELERATION = UtUnits::cANGULAR_ACCELERATION,
   cFORCE                = UtUnits::cFORCE,
   cFREQUENCY            = UtUnits::cFREQUENCY,
   cLENGTH               = UtUnits::cLENGTH,
   cMASS                 = UtUnits::cMASS,
   cMASS_TRANSFER        = UtUnits::cMASS_TRANSFER,
   cPOWER                = UtUnits::cPOWER,
   cPOWER_DB             = UtUnits::cPOWER_DB,
   cPRESSURE             = UtUnits::cPRESSURE,
   cRATIO                = UtUnits::cRATIO,
   cSPEED                = UtUnits::cSPEED,
   cTIME                 = UtUnits::cTIME,
   cTORQUE               = UtUnits::cTORQUE,

   // Altitude is special case of length
   cALTITUDE = UtUnits::_cUNIT_TYPE_COUNT,

   // These value types are not 'unitary'
   cLATITUDE,
   cLONGITUDE
};

// Some convenient aliases for accessors and signals
class UnitsObject;
template<class UNIT_TYPE>
using UnitPreference = typename UNIT_TYPE::BaseUnit (UnitsObject::*)() const;
template<class UNIT_TYPE>
using UnitSignal       = void (UnitsObject::*)(typename UNIT_TYPE::BaseUnit);
using FormatPreference = int (UnitsObject::*)() const;
using FormatSignal     = void (UnitsObject::*)(int);

class WKF_EXPORT UnitsObject : public PrefObjectT<UnitsData>
{
   Q_OBJECT

public:
   static constexpr const char* cNAME = "UnitObject";

   explicit UnitsObject(QObject* parent = nullptr);
   ~UnitsObject() override = default;

   bool IsSharedPreference() const final { return true; }

   // Due to how the wkf unit templates are constructed, these function names should not be modified
   UtUnitAcceleration::BaseUnit        GetAccelerationUnit() const { return mCurrentPrefs.mAcceleration.mUnit; }
   UtUnitLength::BaseUnit              GetAltitudeUnit(double aAlt_m) const;
   UtUnitLength::BaseUnit              GetHighAltitudeUnit() const { return mCurrentPrefs.mHighAltitude.mUnit; }
   UtUnitLength::BaseUnit              GetLowAltitudeUnit() const { return mCurrentPrefs.mLowAltitude.mUnit; }
   UtUnitAngle::BaseUnit               GetAngleUnit() const { return mCurrentPrefs.mAngle.mUnit; }
   UtUnitAngularRate::BaseUnit         GetAngularRateUnit() const { return mCurrentPrefs.mAngularRate.mUnit; }
   UtUnitAngularAcceleration::BaseUnit GetAngularAccelerationUnit() const
   {
      return mCurrentPrefs.mAngularAcceleration.mUnit;
   }
   UtUnitForce::BaseUnit        GetForceUnit() const { return mCurrentPrefs.mForce.mUnit; }
   UtUnitFrequency::BaseUnit    GetFrequencyUnit() const { return mCurrentPrefs.mFrequency.mUnit; }
   UtUnitLength::BaseUnit       GetLengthUnit() const { return mCurrentPrefs.mLength.mUnit; }
   UtUnitMass::BaseUnit         GetMassUnit() const { return mCurrentPrefs.mMass.mUnit; }
   UtUnitMassTransfer::BaseUnit GetMassTransferUnit() const { return mCurrentPrefs.mMassTransfer.mUnit; }
   UtUnitPower::BaseUnit        GetPowerUnit() const { return mCurrentPrefs.mPower.mUnit; }
   UtUnitPowerDB::BaseUnit      GetPowerDBUnit() const { return mCurrentPrefs.mPowerDB.mUnit; }
   UtUnitPressure::BaseUnit     GetPressureUnit() const { return mCurrentPrefs.mPressure.mUnit; }
   UtUnitRatio::BaseUnit        GetRatioUnit() const { return mCurrentPrefs.mRatio.mUnit; }
   UtUnitSpeed::BaseUnit        GetSpeedUnit() const { return mCurrentPrefs.mSpeed.mUnit; }
   UtUnitTime::BaseUnit         GetTimeUnit() const { return mCurrentPrefs.mTime.mUnit; }
   UtUnitTorque::BaseUnit       GetTorqueUnit() const { return mCurrentPrefs.mTorque.mUnit; }

   // returns the unit for the given unit type.  Returns 0, if the unit is not supported.Note that this won't support
   // altitude or lat/lon
   int          GetUnit(ValueType aType) const;
   unsigned int GetUnitPrecision(ValueType aType) const;

   int                   GetLatLonFormatting() const { return mCurrentPrefs.mLatLonFormat; }
   int                   GetTimeFormatting() const { return mCurrentPrefs.mTimeFormat; }
   UnitsData::DateFormat GetDateFormatting() const { return mCurrentPrefs.mDateFormat; }
   QString               GetFormattedValue(ValueType aType, double aValue) const;

signals:
   // Due to how the wkf unit templates are constructed, these signal names should not be modified
   void AccelerationUnitChanged(UtUnitAcceleration::BaseUnit aUnit);
   void HighAltitudeUnitChanged(UtUnitLength::BaseUnit aUnit);
   void LowAltitudeUnitChanged(UtUnitLength::BaseUnit aUnit);
   void AngleUnitChanged(UtUnitAngle::BaseUnit aUnit);
   void AngularRateUnitChanged(UtUnitAngularRate::BaseUnit aUnit);
   void AngularAccelerationUnitChanged(UtUnitAngularAcceleration::BaseUnit aUnit);
   void ForceUnitChanged(UtUnitForce::BaseUnit aUnit);
   void FrequencyUnitChanged(UtUnitFrequency::BaseUnit aUnit);
   void LengthUnitChanged(UtUnitLength::BaseUnit aUnit);
   void MassUnitChanged(UtUnitMass::BaseUnit aUnit);
   void MassTransferUnitChanged(UtUnitMassTransfer::BaseUnit aUnit);
   void PowerUnitChanged(UtUnitPower::BaseUnit aUnit);
   void PowerDBUnitChanged(UtUnitPowerDB::BaseUnit aUnit);
   void PressureUnitChanged(UtUnitPressure::BaseUnit aUnit);
   void RatioUnitChanged(UtUnitRatio::BaseUnit aUnit);
   void SpeedUnitChanged(UtUnitSpeed::BaseUnit aUnit);
   void TimeUnitChanged(UtUnitTime::BaseUnit aUnit);
   void TorqueUnitChanged(UtUnitTorque::BaseUnit aUnit);

   void LatLonFormatChanged(int aFormat);
   void TimeFormatChanged(int aFormat);
   void DateFormatChanged(UnitsData::DateFormat aFormat);

   // Legacy Support
   void UnitsChanged();

private:
   void      Apply() override;
   UnitsData ReadSettings(QSettings& aSettings) const override;
   void      SaveSettingsP(QSettings& aSettings) const override;
   void      SetVtkEnvironmentUnits();

   void SetPreferenceDataP(const UnitsData& aPrefData) override;

   QString GetFormattedAltitude(double aValue) const;
   QString GetFormattedLatitude(double aValue) const;
   QString GetFormattedLongitude(double aValue) const;

   bool mAccelerationChanged{false};
   bool mHighAltitudeChanged{false};
   bool mLowAltitudeChanged{false};
   bool mAngleChanged{false};
   bool mAngularRateChanged{false};
   bool mAngularAccelerationChanged{false};
   bool mForceChanged{false};
   bool mFrequencyChanged{false};
   bool mLengthChanged{false};
   bool mMassChanged{false};
   bool mMassTransferChanged{false};
   bool mPowerChanged{false};
   bool mPowerDB_Changed{false};
   bool mPressureChanged{false};
   bool mRatioChanged{false};
   bool mSpeedChanged{false};
   bool mTimeChanged{false};
   bool mTorqueChanged{false};

   bool mLatLonFormatChanged{false};
   bool mTimeFormatChanged{false};
   bool mDateFormatChanged{false};
};
} // namespace wkf

Q_DECLARE_METATYPE(UtUnitAcceleration::BaseUnit)
Q_DECLARE_METATYPE(UtUnitLength::BaseUnit)
Q_DECLARE_METATYPE(UtUnitAngle::BaseUnit)
Q_DECLARE_METATYPE(UtUnitAngularRate::BaseUnit)
Q_DECLARE_METATYPE(UtUnitAngularAcceleration::BaseUnit)
Q_DECLARE_METATYPE(UtUnitForce::BaseUnit)
Q_DECLARE_METATYPE(UtUnitFrequency::BaseUnit)
Q_DECLARE_METATYPE(UtUnitMass::BaseUnit)
Q_DECLARE_METATYPE(UtUnitMassTransfer::BaseUnit)
Q_DECLARE_METATYPE(UtUnitPower::BaseUnit)
Q_DECLARE_METATYPE(UtUnitPowerDB::BaseUnit)
Q_DECLARE_METATYPE(UtUnitPressure::BaseUnit)
Q_DECLARE_METATYPE(UtUnitRatio::BaseUnit)
Q_DECLARE_METATYPE(UtUnitSpeed::BaseUnit)
Q_DECLARE_METATYPE(UtUnitTime::BaseUnit)
Q_DECLARE_METATYPE(UtUnitTorque::BaseUnit)
Q_DECLARE_METATYPE(wkf::UnitsData)
#endif
