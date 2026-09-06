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
#include "WkfUnitsObject.hpp"

#include <iomanip>
#include <sstream>

#include <QMetaMethod>
#include <QSettings>

#include "UtAngleR.hpp"
#include "UtSpeed.hpp"
#include "WkfVtkEnvironment.hpp"

namespace
{
const double cALTITUDE_SEPARATOR = 100000; // 100km or ~300000ft
}

const std::unordered_map<UtUnitAcceleration::BaseUnit, unsigned int, std::hash<int>> wkf::UnitsData::mAccelerationPrecisionMap =
   {{UtUnitAcceleration::cMETERS_PER_SECOND2, 2}, {UtUnitAcceleration::cFEET_PER_SECOND2, 2}, {UtUnitAcceleration::cG, 2}};

const std::unordered_map<UtUnitLength::BaseUnit, unsigned int, std::hash<int>> wkf::UnitsData::mAltitudePrecisionMap = {
   {UtUnitLength::cMETERS, 0},
   {UtUnitLength::cKILOMETERS, 2},
   {UtUnitLength::cMEGAMETERS, 2},
   {UtUnitLength::cFEET, 0},
   {UtUnitLength::cKILOFEET, 2},
   {UtUnitLength::cMILES, 2},
   {UtUnitLength::cNAUTICAL_MILES, 2},
   {UtUnitLength::cCENTIMETERS, 0},
   {UtUnitLength::cMILLIMETERS, 0},
   {UtUnitLength::cMICROMETERS, 0},
   {UtUnitLength::cNANOMETERS, 0},
   {UtUnitLength::cANGSTROMS, 0},
   {UtUnitLength::cINCHES, 0},
   {UtUnitLength::cASTRONOMICAL_UNIT, 6}};

const std::unordered_map<UtUnitAngle::BaseUnit, unsigned int, std::hash<int>> wkf::UnitsData::mAnglePrecisionMap =
   {{UtUnitAngle::cRADIANS, 4}, {UtUnitAngle::cDEGREES, 0}, {UtUnitAngle::cMILS, 4}, {UtUnitAngle::cARCSECONDS, 4}};

const std::unordered_map<UtUnitAngularRate::BaseUnit, unsigned int, std::hash<int>> wkf::UnitsData::mAngularRatePrecisionMap =
   {{UtUnitAngularRate::cRADIANS_PER_SECOND, 4}, {UtUnitAngularRate::cDEGREES_PER_SECOND, 2}, {UtUnitAngularRate::cRPM, 2}};

const std::unordered_map<UtUnitAngularAcceleration::BaseUnit, unsigned int, std::hash<int>>
   wkf::UnitsData::mAngularAccelerationPrecisionMap = {{UtUnitAngularAcceleration::cRADIANS_PER_SECOND2, 4},
                                                       {UtUnitAngularAcceleration::cDEGREES_PER_SECOND2, 2}};

const std::unordered_map<UtUnitForce::BaseUnit, unsigned int, std::hash<int>> wkf::UnitsData::mForcePrecisionMap = {
   {UtUnitForce::cNEWTONS, 4},
   {UtUnitForce::cKGF, 4},
   {UtUnitForce::cLBF, 4},
};

const std::unordered_map<UtUnitFrequency::BaseUnit, unsigned int, std::hash<int>> wkf::UnitsData::mFrequencyPrecisionMap =
   {{UtUnitFrequency::cHZ, 2}, {UtUnitFrequency::cKHZ, 4}, {UtUnitFrequency::cMHZ, 4}, {UtUnitFrequency::cGHZ, 4}};

const std::unordered_map<UtUnitLength::BaseUnit, unsigned int, std::hash<int>> wkf::UnitsData::mLengthPrecisionMap = {
   {UtUnitLength::cMETERS, 0},
   {UtUnitLength::cKILOMETERS, 2},
   {UtUnitLength::cMEGAMETERS, 2},
   {UtUnitLength::cFEET, 0},
   {UtUnitLength::cKILOFEET, 2},
   {UtUnitLength::cMILES, 2},
   {UtUnitLength::cNAUTICAL_MILES, 2},
   {UtUnitLength::cCENTIMETERS, 0},
   {UtUnitLength::cMILLIMETERS, 0},
   {UtUnitLength::cMICROMETERS, 0},
   {UtUnitLength::cNANOMETERS, 0},
   {UtUnitLength::cANGSTROMS, 0},
   {UtUnitLength::cINCHES, 0},
   {UtUnitLength::cASTRONOMICAL_UNIT, 6}};

const std::unordered_map<UtUnitMass::BaseUnit, unsigned int, std::hash<int>> wkf::UnitsData::mMassPrecisionMap = {
   {UtUnitMass::cKILOGRAMS, 2},
   {UtUnitMass::cGRAMS, 0},
   {UtUnitMass::cPOUNDS, 2},
   {UtUnitMass::cKLB, 4},
   {UtUnitMass::cTONS, 4},
   {UtUnitMass::cTONNES, 4},
   {UtUnitMass::cSLUGS, 2}};

const std::unordered_map<UtUnitMassTransfer::BaseUnit, unsigned int, std::hash<int>> wkf::UnitsData::mMassTransferPrecisionMap = {
   {UtUnitMassTransfer::cKILOGRAMS_PER_SECOND, 2}};

const std::unordered_map<UtUnitPower::BaseUnit, unsigned int, std::hash<int>> wkf::UnitsData::mPowerPrecisionMap = {
   {UtUnitPower::cWATTS, 2},
   {UtUnitPower::cKILOWATTS, 4},
   {UtUnitPower::cMEGAWATTS, 4},
   {UtUnitPower::cGIGAWATTS, 4},
   {UtUnitPower::cMILLIWATTS, 0},
   {UtUnitPower::cMICROWATTS, 0}};

const std::unordered_map<UtUnitPowerDB::BaseUnit, unsigned int, std::hash<int>> wkf::UnitsData::mPowerDBPrecisionMap = {
   {UtUnitPowerDB::cWATTS, 2},
   {UtUnitPowerDB::cKILOWATTS, 4},
   {UtUnitPowerDB::cMEGAWATTS, 4},
   {UtUnitPowerDB::cGIGAWATTS, 4},
   {UtUnitPowerDB::cMILLIWATTS, 0},
   {UtUnitPowerDB::cMICROWATTS, 0},
   {UtUnitPowerDB::cDBW, 2},
   {UtUnitPowerDB::cDBM, 2}};

const std::unordered_map<UtUnitPressure::BaseUnit, unsigned int, std::hash<int>> wkf::UnitsData::mPressurePrecisionMap = {
   {UtUnitPressure::cPASCAL, 2},
   {UtUnitPressure::cKILOPASCALS, 4},
   {UtUnitPressure::cMICROPASCALS, 0},
   {UtUnitPressure::cNANOPASCALS, 0},
   {UtUnitPressure::cPSI, 2},
   {UtUnitPressure::cPSF, 2},
   {UtUnitPressure::cDBPA, 2},
   {UtUnitPressure::cDBUPA, 2}};

const std::unordered_map<UtUnitRatio::BaseUnit, unsigned int, std::hash<int>> wkf::UnitsData::mRatioPrecisionMap =
   {{UtUnitRatio::cABSOLUTE, 2}, {UtUnitRatio::cDB, 2}};

const std::unordered_map<UtUnitSpeed::BaseUnit, unsigned int, std::hash<int>> wkf::UnitsData::mSpeedPrecisionMap = {
   {UtUnitSpeed::cMETERS_PER_SECOND, 0},
   {UtUnitSpeed::cKILOMETERS_PER_HOUR, 2},
   {UtUnitSpeed::cFEET_PER_SECOND, 2},
   {UtUnitSpeed::cFEET_PER_MINUTE, 2},
   {UtUnitSpeed::cMILES_PER_HOUR, 2},
   {UtUnitSpeed::cKNOTS, 2}};

const std::unordered_map<UtUnitTime::BaseUnit, unsigned int, std::hash<int>> wkf::UnitsData::mTimePrecisionMap = {
   {UtUnitTime::cSECONDS, 2},
   {UtUnitTime::cMINUTES, 2},
   {UtUnitTime::cHOURS, 2},
   {UtUnitTime::cMILLISECONDS, 2},
   {UtUnitTime::cMICROSECONDS, 2},
   {UtUnitTime::cNANOSECONDS, 2},
   {UtUnitTime::cDAYS, 2}};

const std::unordered_map<UtUnitTorque::BaseUnit, unsigned int, std::hash<int>> wkf::UnitsData::mTorquePrecisionMap = {
   {UtUnitTorque::cNEWTON_METERS, 4}};

wkf::UnitsObject::UnitsObject(QObject* parent)
   : PrefObjectT<UnitsData>(parent, cNAME)
{
}

UtUnitLength::BaseUnit wkf::UnitsObject::GetAltitudeUnit(double aAlt_m) const
{
   if (aAlt_m > cALTITUDE_SEPARATOR)
   {
      return mCurrentPrefs.mHighAltitude.mUnit;
   }
   else
   {
      return mCurrentPrefs.mLowAltitude.mUnit;
   }
}

void wkf::UnitsObject::Apply()
{
   SetVtkEnvironmentUnits();

   // Unit Signals
   bool unitsChanged = mAccelerationChanged || mHighAltitudeChanged || mLowAltitudeChanged || mAngleChanged ||
                       mAngularRateChanged || mAngularAccelerationChanged || mForceChanged || mFrequencyChanged ||
                       mLengthChanged || mMassChanged || mMassTransferChanged || mPowerChanged || mPowerDB_Changed ||
                       mPressureChanged || mRatioChanged || mSpeedChanged || mTimeChanged || mLatLonFormatChanged ||
                       mTimeFormatChanged || mDateFormatChanged || mTorqueChanged;

   if (mAccelerationChanged)
   {
      emit AccelerationUnitChanged(mCurrentPrefs.mAcceleration.mUnit);
      mAccelerationChanged = false;
   }
   if (mHighAltitudeChanged)
   {
      emit HighAltitudeUnitChanged(mCurrentPrefs.mHighAltitude.mUnit);
      mHighAltitudeChanged = false;
   }
   if (mLowAltitudeChanged)
   {
      emit LowAltitudeUnitChanged(mCurrentPrefs.mLowAltitude.mUnit);
      mLowAltitudeChanged = false;
   }
   if (mAngleChanged)
   {
      emit AngleUnitChanged(mCurrentPrefs.mAngle.mUnit);
      mAngleChanged = false;
   }
   if (mAngularRateChanged)
   {
      emit AngularRateUnitChanged(mCurrentPrefs.mAngularRate.mUnit);
      mAngularRateChanged = false;
   }
   if (mAngularAccelerationChanged)
   {
      emit AngularAccelerationUnitChanged(mCurrentPrefs.mAngularAcceleration.mUnit);
      mAngularAccelerationChanged = false;
   }
   if (mForceChanged)
   {
      emit ForceUnitChanged(mCurrentPrefs.mForce.mUnit);
      mForceChanged = false;
   }
   if (mFrequencyChanged)
   {
      emit FrequencyUnitChanged(mCurrentPrefs.mFrequency.mUnit);
      mFrequencyChanged = false;
   }
   if (mLengthChanged)
   {
      emit LengthUnitChanged(mCurrentPrefs.mLength.mUnit);
      mLengthChanged = false;
   }
   if (mMassChanged)
   {
      emit MassUnitChanged(mCurrentPrefs.mMass.mUnit);
      mMassChanged = false;
   }
   if (mMassTransferChanged)
   {
      emit MassTransferUnitChanged(mCurrentPrefs.mMassTransfer.mUnit);
      mMassTransferChanged = false;
   }
   if (mPowerChanged)
   {
      emit PowerUnitChanged(mCurrentPrefs.mPower.mUnit);
      mPowerChanged = false;
   }
   if (mPowerDB_Changed)
   {
      emit PowerDBUnitChanged(mCurrentPrefs.mPowerDB.mUnit);
      mPowerDB_Changed = false;
   }
   if (mPressureChanged)
   {
      emit PressureUnitChanged(mCurrentPrefs.mPressure.mUnit);
      mPressureChanged = false;
   }
   if (mRatioChanged)
   {
      emit RatioUnitChanged(mCurrentPrefs.mRatio.mUnit);
      mRatioChanged = false;
   }
   if (mSpeedChanged)
   {
      emit SpeedUnitChanged(mCurrentPrefs.mSpeed.mUnit);
      mSpeedChanged = false;
   }
   if (mTimeChanged)
   {
      emit TimeUnitChanged(mCurrentPrefs.mTime.mUnit);
      mTimeChanged = false;
   }
   if (mLatLonFormatChanged)
   {
      emit LatLonFormatChanged(mCurrentPrefs.mLatLonFormat);
      mLatLonFormatChanged = false;
   }
   if (mTimeFormatChanged)
   {
      emit TimeFormatChanged(mCurrentPrefs.mTimeFormat);
      mTimeFormatChanged = false;
   }
   if (mDateFormatChanged)
   {
      emit DateFormatChanged(mCurrentPrefs.mDateFormat);
      mDateFormatChanged = false;
   }
   if (mTorqueChanged)
   {
      emit TorqueUnitChanged(mCurrentPrefs.mTorque.mUnit);
      mTorqueChanged = false;
   }
   if (unitsChanged)
   {
      emit UnitsChanged();
   }
}

wkf::UnitsData wkf::UnitsObject::ReadSettings(QSettings& aSettings) const
{
   UnitsData pdata;

   pdata.mAcceleration.mUnit =
      (UtUnitAcceleration::BaseUnit)aSettings.value("accel", mDefaultPrefs.mAcceleration.mUnit).toInt();
   pdata.mHighAltitude.mUnit =
      (UtUnitLength::BaseUnit)aSettings.value("highAlt", mDefaultPrefs.mHighAltitude.mUnit).toInt();
   pdata.mLowAltitude.mUnit = (UtUnitLength::BaseUnit)aSettings.value("lowAlt", mDefaultPrefs.mLowAltitude.mUnit).toInt();
   pdata.mAngle.mUnit       = (UtUnitAngle::BaseUnit)aSettings.value("angle", mDefaultPrefs.mAngle.mUnit).toInt();
   pdata.mAngularRate.mUnit =
      (UtUnitAngularRate::BaseUnit)aSettings.value("angularRate", mDefaultPrefs.mAngularRate.mUnit).toInt();
   pdata.mAngularAcceleration.mUnit = (UtUnitAngularAcceleration::BaseUnit)aSettings
                                         .value("angularAcceleration", mDefaultPrefs.mAngularAcceleration.mUnit)
                                         .toInt();
   pdata.mForce.mUnit = (UtUnitForce::BaseUnit)aSettings.value("force", mDefaultPrefs.mForce.mUnit).toInt();
   pdata.mFrequency.mUnit =
      (UtUnitFrequency::BaseUnit)aSettings.value("frequency", mDefaultPrefs.mFrequency.mUnit).toInt();
   pdata.mLatLonFormat = aSettings.value("latLon", mDefaultPrefs.mLatLonFormat).toInt();
   pdata.mLength.mUnit = (UtUnitLength::BaseUnit)aSettings.value("length", mDefaultPrefs.mLength.mUnit).toInt();
   pdata.mMass.mUnit   = (UtUnitMass::BaseUnit)aSettings.value("mass", mDefaultPrefs.mMass.mUnit).toInt();
   pdata.mMassTransfer.mUnit =
      (UtUnitMassTransfer::BaseUnit)aSettings.value("massTransfer", mDefaultPrefs.mMassTransfer.mUnit).toInt();
   pdata.mPower.mUnit    = (UtUnitPower::BaseUnit)aSettings.value("power", mDefaultPrefs.mPower.mUnit).toInt();
   pdata.mPowerDB.mUnit  = (UtUnitPowerDB::BaseUnit)aSettings.value("powerDB", mDefaultPrefs.mPowerDB.mUnit).toInt();
   pdata.mPressure.mUnit = (UtUnitPressure::BaseUnit)aSettings.value("pressure", mDefaultPrefs.mPressure.mUnit).toInt();
   pdata.mRatio.mUnit    = (UtUnitRatio::BaseUnit)aSettings.value("ratio", mDefaultPrefs.mRatio.mUnit).toInt();
   pdata.mSpeed.mUnit    = (UtUnitSpeed::BaseUnit)aSettings.value("speed", mDefaultPrefs.mSpeed.mUnit).toInt();
   pdata.mTime.mUnit     = (UtUnitTime::BaseUnit)aSettings.value("timeUnit", mDefaultPrefs.mTime.mUnit).toInt();
   pdata.mTimeFormat     = aSettings.value("timeFormat", mDefaultPrefs.mTimeFormat).toInt();
   pdata.mDateFormat =
      static_cast<UnitsData::DateFormat>(aSettings.value("dateFormat", mDefaultPrefs.mDateFormat).toInt());
   pdata.mTorque.mUnit = (UtUnitTorque::BaseUnit)aSettings.value("torque", mDefaultPrefs.mTorque.mUnit).toInt();

   pdata.mAcceleration.mPrecision = aSettings.value("accelPrecision", mDefaultPrefs.mAcceleration.mPrecision).toUInt();
   pdata.mHighAltitude.mPrecision = aSettings.value("highAltPrecision", mDefaultPrefs.mHighAltitude.mPrecision).toUInt();
   pdata.mLowAltitude.mPrecision  = aSettings.value("lowAltPrecision", mDefaultPrefs.mLowAltitude.mPrecision).toUInt();
   pdata.mAngle.mPrecision        = aSettings.value("anglePrecision", mDefaultPrefs.mAngle.mPrecision).toUInt();
   pdata.mAngularRate.mPrecision = aSettings.value("angularRatePrecision", mDefaultPrefs.mAngularRate.mPrecision).toUInt();
   pdata.mAngularAcceleration.mPrecision =
      aSettings.value("angularAccelerationPrecision", mDefaultPrefs.mAngularAcceleration.mPrecision).toUInt();
   pdata.mForce.mPrecision     = aSettings.value("forcePrecision", mDefaultPrefs.mForce.mPrecision).toUInt();
   pdata.mFrequency.mPrecision = aSettings.value("frequencyPrecision", mDefaultPrefs.mFrequency.mPrecision).toUInt();
   pdata.mLength.mPrecision    = aSettings.value("lengthPrecision", mDefaultPrefs.mLength.mPrecision).toUInt();
   pdata.mMass.mPrecision      = aSettings.value("massPrecision", mDefaultPrefs.mMass.mPrecision).toUInt();
   pdata.mMassTransfer.mPrecision =
      aSettings.value("massTransferPrecision", mDefaultPrefs.mMassTransfer.mPrecision).toUInt();
   pdata.mPower.mPrecision    = aSettings.value("powerPrecision", mDefaultPrefs.mPower.mPrecision).toUInt();
   pdata.mPowerDB.mPrecision  = aSettings.value("powerDBPrecision", mDefaultPrefs.mPowerDB.mPrecision).toUInt();
   pdata.mPressure.mPrecision = aSettings.value("pressurePrecision", mDefaultPrefs.mPressure.mPrecision).toUInt();
   pdata.mRatio.mPrecision    = aSettings.value("ratioPrecision", mDefaultPrefs.mRatio.mPrecision).toUInt();
   pdata.mSpeed.mPrecision    = aSettings.value("speedPrecision", mDefaultPrefs.mSpeed.mPrecision).toUInt();
   pdata.mTime.mPrecision     = aSettings.value("timePrecision", mDefaultPrefs.mTime.mPrecision).toInt();
   pdata.mTorque.mPrecision   = aSettings.value("torquePrecision", mDefaultPrefs.mTorque.mPrecision).toUInt();

   return pdata;
}

void wkf::UnitsObject::SaveSettingsP(QSettings& aSettings) const
{
   aSettings.setValue("accel", mCurrentPrefs.mAcceleration.mUnit);
   aSettings.setValue("highAlt", mCurrentPrefs.mHighAltitude.mUnit);
   aSettings.setValue("lowAlt", mCurrentPrefs.mLowAltitude.mUnit);
   aSettings.setValue("angle", mCurrentPrefs.mAngle.mUnit);
   aSettings.setValue("angularRate", mCurrentPrefs.mAngularRate.mUnit);
   aSettings.setValue("angularAcceleration", mCurrentPrefs.mAngularAcceleration.mUnit);
   aSettings.setValue("force", mCurrentPrefs.mForce.mUnit);
   aSettings.setValue("frequency", mCurrentPrefs.mFrequency.mUnit);
   aSettings.setValue("latLon", mCurrentPrefs.mLatLonFormat);
   aSettings.setValue("length", mCurrentPrefs.mLength.mUnit);
   aSettings.setValue("mass", mCurrentPrefs.mMass.mUnit);
   aSettings.setValue("massTransfer", mCurrentPrefs.mMassTransfer.mUnit);
   aSettings.setValue("power", mCurrentPrefs.mPower.mUnit);
   aSettings.setValue("powerDB", mCurrentPrefs.mPowerDB.mUnit);
   aSettings.setValue("pressure", mCurrentPrefs.mPressure.mUnit);
   aSettings.setValue("ratio", mCurrentPrefs.mRatio.mUnit);
   aSettings.setValue("speed", mCurrentPrefs.mSpeed.mUnit);
   aSettings.setValue("timeUnit", mCurrentPrefs.mTime.mUnit);
   aSettings.setValue("timeFormat", mCurrentPrefs.mTimeFormat);
   aSettings.setValue("dateFormat", mCurrentPrefs.mDateFormat);
   aSettings.setValue("torque", mCurrentPrefs.mTorque.mUnit);

   aSettings.setValue("accelPrecision", mCurrentPrefs.mAcceleration.mPrecision);
   aSettings.setValue("highAltPrecision", mCurrentPrefs.mHighAltitude.mPrecision);
   aSettings.setValue("lowAltPrecision", mCurrentPrefs.mLowAltitude.mPrecision);
   aSettings.setValue("anglePrecision", mCurrentPrefs.mAngle.mPrecision);
   aSettings.setValue("angularRatePrecision", mCurrentPrefs.mAngularRate.mPrecision);
   aSettings.setValue("angularAccelerationPrecision", mCurrentPrefs.mAngularAcceleration.mPrecision);
   aSettings.setValue("forcePrecision", mCurrentPrefs.mForce.mPrecision);
   aSettings.setValue("frequencyPrecision", mCurrentPrefs.mFrequency.mPrecision);
   aSettings.setValue("lengthPrecision", mCurrentPrefs.mLength.mPrecision);
   aSettings.setValue("massPrecision", mCurrentPrefs.mMass.mPrecision);
   aSettings.setValue("massTransferPrecision", mCurrentPrefs.mMassTransfer.mPrecision);
   aSettings.setValue("powerPrecision", mCurrentPrefs.mPower.mPrecision);
   aSettings.setValue("powerDBPrecision", mCurrentPrefs.mPowerDB.mPrecision);
   aSettings.setValue("pressurePrecision", mCurrentPrefs.mPressure.mPrecision);
   aSettings.setValue("ratioPrecision", mCurrentPrefs.mRatio.mPrecision);
   aSettings.setValue("speedPrecision", mCurrentPrefs.mSpeed.mPrecision);
   aSettings.setValue("timePrecision", mCurrentPrefs.mTime.mPrecision);
   aSettings.setValue("torquePrecision", mCurrentPrefs.mTorque.mPrecision);
}

void wkf::UnitsObject::SetVtkEnvironmentUnits()
{
   switch (mCurrentPrefs.mLength.mUnit)
   {
   case UtUnitLength::cANGSTROMS:
      vaEnv.SetUnitFormat(vespa::VaEnvironment::DistanceMeasure, UtLength::FmtAngsM);
      break;
      // case (UtUnitLength::cASTRONOMICAL_UNIT): // unhandled by utlength
      // break;
   case UtUnitLength::cCENTIMETERS:
      vaEnv.SetUnitFormat(vespa::VaEnvironment::DistanceMeasure, UtLength::FmtCentiM);
      break;
   case UtUnitLength::cFEET:
      vaEnv.SetUnitFormat(vespa::VaEnvironment::DistanceMeasure, UtLength::FmtFT + 1);
      break;
   case UtUnitLength::cINCHES:
      vaEnv.SetUnitFormat(vespa::VaEnvironment::DistanceMeasure, UtLength::FmtInch);
      break;
   case UtUnitLength::cKILOFEET:
      vaEnv.SetUnitFormat(vespa::VaEnvironment::DistanceMeasure, UtLength::FmtKFT + 2);
      break;
   case UtUnitLength::cKILOMETERS:
      vaEnv.SetUnitFormat(vespa::VaEnvironment::DistanceMeasure, UtLength::FmtKM + 2);
      break;
   case UtUnitLength::cMEGAMETERS:
      vaEnv.SetUnitFormat(vespa::VaEnvironment::DistanceMeasure, UtLength::FmtMegaM + 3);
      break;
   case UtUnitLength::cNAUTICAL_MILES:
      vaEnv.SetUnitFormat(vespa::VaEnvironment::DistanceMeasure, UtLength::FmtNM + 2);
      break;
   case UtUnitLength::cMICROMETERS:
      vaEnv.SetUnitFormat(vespa::VaEnvironment::DistanceMeasure, UtLength::FmtMicroM);
      break;
   case UtUnitLength::cMILES:
      vaEnv.SetUnitFormat(vespa::VaEnvironment::DistanceMeasure, UtLength::FmtMI + 2);
      break;
      // case(UtUnitLength::cMILLIMETERS): // unhandled by utlength
      // break;
   case UtUnitLength::cNANOMETERS:
      vaEnv.SetUnitFormat(vespa::VaEnvironment::DistanceMeasure, UtLength::FmtNanoM);
      break;
   case UtUnitLength::cMETERS:
      vaEnv.SetUnitFormat(vespa::VaEnvironment::DistanceMeasure, UtLength::FmtM + 1);
   default:
      break;
   }

   switch (mCurrentPrefs.mAngle.mUnit)
   {
   case UtUnitAngle::cRADIANS:
      vaEnv.SetUnitFormat(vespa::VaEnvironment::AngleMeasure, UtAngleR::FmtRad + 3);
      break;
   case UtUnitAngle::cMILS:
      vaEnv.SetUnitFormat(vespa::VaEnvironment::AngleMeasure, UtAngleR::FmtMils + 1);
      break;
   case UtUnitAngle::cDEGREES:
   default:
      vaEnv.SetUnitFormat(vespa::VaEnvironment::AngleMeasure, UtAngleR::FmtDeg + 1);
      break;
   }

   switch (mCurrentPrefs.mSpeed.mUnit)
   {
   case UtUnitSpeed::cFEET_PER_MINUTE:
      vaEnv.SetUnitFormat(vespa::VaEnvironment::SpeedMeasure, UtSpeed::FmtFPM + 1);
      break;
   case UtUnitSpeed::cFEET_PER_SECOND:
      vaEnv.SetUnitFormat(vespa::VaEnvironment::SpeedMeasure, UtSpeed::FmtFPS + 1);
      break;
   case UtUnitSpeed::cKNOTS:
      vaEnv.SetUnitFormat(vespa::VaEnvironment::SpeedMeasure, UtSpeed::FmtKTS + 1);
      break;
   case UtUnitSpeed::cMILES_PER_HOUR:
      vaEnv.SetUnitFormat(vespa::VaEnvironment::SpeedMeasure, UtSpeed::FmtMPH + 1);
      break;
   case UtUnitSpeed::cKILOMETERS_PER_HOUR:
      vaEnv.SetUnitFormat(vespa::VaEnvironment::SpeedMeasure, UtSpeed::FmtKPH + 1);
      break;
   case UtUnitSpeed::cMETERS_PER_SECOND:
   default:
      vaEnv.SetUnitFormat(vespa::VaEnvironment::SpeedMeasure, UtSpeed::FmtMPS + 1);
      break;
   }

   vaEnv.SetUnitFormat(vespa::VaEnvironment::TimeMeasure, mCurrentPrefs.mTimeFormat);
}

void wkf::UnitsObject::SetPreferenceDataP(const UnitsData& aPrefData)
{
   mAccelerationChanged = mCurrentPrefs.mAcceleration.mUnit != aPrefData.mAcceleration.mUnit ||
                          mCurrentPrefs.mAcceleration.mPrecision != aPrefData.mAcceleration.mPrecision;
   mHighAltitudeChanged = mCurrentPrefs.mHighAltitude.mUnit != aPrefData.mHighAltitude.mUnit ||
                          mCurrentPrefs.mHighAltitude.mPrecision != aPrefData.mHighAltitude.mPrecision;
   mLowAltitudeChanged = mCurrentPrefs.mLowAltitude.mUnit != aPrefData.mLowAltitude.mUnit ||
                         mCurrentPrefs.mLowAltitude.mPrecision != aPrefData.mLowAltitude.mPrecision;
   mAngleChanged = mCurrentPrefs.mAngle.mUnit != aPrefData.mAngle.mUnit ||
                   mCurrentPrefs.mAngle.mPrecision != aPrefData.mAngle.mPrecision;
   mAngularRateChanged = mCurrentPrefs.mAngularRate.mUnit != aPrefData.mAngularRate.mUnit ||
                         mCurrentPrefs.mAngularRate.mPrecision != aPrefData.mAngularRate.mPrecision;
   mAngularAccelerationChanged = mCurrentPrefs.mAngularAcceleration.mUnit != aPrefData.mAngularAcceleration.mUnit ||
                                 mCurrentPrefs.mAngularAcceleration.mPrecision != aPrefData.mAngularAcceleration.mPrecision;
   mForceChanged = mCurrentPrefs.mForce.mUnit != aPrefData.mForce.mUnit ||
                   mCurrentPrefs.mForce.mPrecision != aPrefData.mForce.mPrecision;
   mFrequencyChanged = mCurrentPrefs.mFrequency.mUnit != aPrefData.mFrequency.mUnit ||
                       mCurrentPrefs.mFrequency.mPrecision != aPrefData.mFrequency.mPrecision;
   mLengthChanged = mCurrentPrefs.mLength.mUnit != aPrefData.mLength.mUnit ||
                    mCurrentPrefs.mLength.mPrecision != aPrefData.mLength.mPrecision;
   mMassChanged = mCurrentPrefs.mMass.mUnit != aPrefData.mMass.mUnit ||
                  mCurrentPrefs.mMass.mPrecision != aPrefData.mMass.mPrecision;
   mMassTransferChanged = mCurrentPrefs.mMassTransfer.mUnit != aPrefData.mMassTransfer.mUnit ||
                          mCurrentPrefs.mMassTransfer.mPrecision != aPrefData.mMassTransfer.mPrecision;
   mPowerChanged = mCurrentPrefs.mPower.mUnit != aPrefData.mPower.mUnit ||
                   mCurrentPrefs.mPower.mPrecision != aPrefData.mPower.mPrecision;
   mPowerDB_Changed = mCurrentPrefs.mPowerDB.mUnit != aPrefData.mPowerDB.mUnit ||
                      mCurrentPrefs.mPowerDB.mPrecision != aPrefData.mPowerDB.mPrecision;
   mPressureChanged = mCurrentPrefs.mPressure.mUnit != aPrefData.mPressure.mUnit ||
                      mCurrentPrefs.mPressure.mPrecision != aPrefData.mPressure.mPrecision;
   mRatioChanged = mCurrentPrefs.mRatio.mUnit != aPrefData.mRatio.mUnit ||
                   mCurrentPrefs.mRatio.mPrecision != aPrefData.mRatio.mPrecision;
   mSpeedChanged = mCurrentPrefs.mSpeed.mUnit != aPrefData.mSpeed.mUnit ||
                   mCurrentPrefs.mSpeed.mPrecision != aPrefData.mSpeed.mPrecision;
   mTimeChanged = mCurrentPrefs.mTime.mUnit != aPrefData.mTime.mUnit ||
                  mCurrentPrefs.mTime.mPrecision != aPrefData.mTime.mPrecision;

   mLatLonFormatChanged = mCurrentPrefs.mLatLonFormat != aPrefData.mLatLonFormat;
   mTimeFormatChanged   = mCurrentPrefs.mTimeFormat != aPrefData.mTimeFormat;
   mDateFormatChanged   = mCurrentPrefs.mDateFormat != aPrefData.mDateFormat;
   mTorqueChanged       = mCurrentPrefs.mTorque.mUnit != aPrefData.mTorque.mUnit ||
                    mCurrentPrefs.mTorque.mPrecision != aPrefData.mTorque.mPrecision;

   BasePrefObject::SetPreferenceDataP(aPrefData);
}

int wkf::UnitsObject::GetUnit(ValueType aType) const
{
   switch (aType)
   {
   case ValueType::cACCELERATION:
      return mCurrentPrefs.mAcceleration.mUnit;
   case ValueType::cALTITUDE:
      return mCurrentPrefs.mLowAltitude.mUnit;
   case ValueType::cANGLE:
      return mCurrentPrefs.mAngle.mUnit;
   case ValueType::cANGULAR_RATE:
      return mCurrentPrefs.mAngularRate.mUnit;
   case ValueType::cANGULAR_ACCELERATION:
      return mCurrentPrefs.mAngularAcceleration.mUnit;
   case ValueType::cFORCE:
      return mCurrentPrefs.mForce.mUnit;
   case ValueType::cFREQUENCY:
      return mCurrentPrefs.mFrequency.mUnit;
   case ValueType::cLENGTH:
      return mCurrentPrefs.mLength.mUnit;
   case ValueType::cMASS:
      return mCurrentPrefs.mMass.mUnit;
   case ValueType::cMASS_TRANSFER:
      return mCurrentPrefs.mMassTransfer.mUnit;
   case ValueType::cPOWER:
      return mCurrentPrefs.mPower.mUnit;
   case ValueType::cPOWER_DB:
      return mCurrentPrefs.mPowerDB.mUnit;
   case ValueType::cPRESSURE:
      return mCurrentPrefs.mPressure.mUnit;
   case ValueType::cRATIO:
      return mCurrentPrefs.mRatio.mUnit;
   case ValueType::cSPEED:
      return mCurrentPrefs.mSpeed.mUnit;
   case ValueType::cTIME:
      return mCurrentPrefs.mTime.mUnit;
   case ValueType::cTORQUE:
      return mCurrentPrefs.mTorque.mUnit;
   case ValueType::cLATITUDE: // intentional fall-through
   case ValueType::cLONGITUDE:
   default:
      return 0;
   }
}

unsigned int wkf::UnitsObject::GetUnitPrecision(ValueType aType) const
{
   switch (aType)
   {
   case ValueType::cACCELERATION:
      return mCurrentPrefs.mAcceleration.mPrecision;
   case ValueType::cALTITUDE:
      return mCurrentPrefs.mLowAltitude.mPrecision;
   case ValueType::cANGLE:
      return mCurrentPrefs.mAngle.mPrecision;
   case ValueType::cANGULAR_RATE:
      return mCurrentPrefs.mAngularRate.mPrecision;
   case ValueType::cANGULAR_ACCELERATION:
      return mCurrentPrefs.mAngularAcceleration.mPrecision;
   case ValueType::cFORCE:
      return mCurrentPrefs.mForce.mPrecision;
   case ValueType::cFREQUENCY:
      return mCurrentPrefs.mFrequency.mPrecision;
   case ValueType::cLENGTH:
      return mCurrentPrefs.mLength.mPrecision;
   case ValueType::cMASS:
      return mCurrentPrefs.mMass.mPrecision;
   case ValueType::cMASS_TRANSFER:
      return mCurrentPrefs.mMassTransfer.mPrecision;
   case ValueType::cPOWER:
      return mCurrentPrefs.mPower.mPrecision;
   case ValueType::cPOWER_DB:
      return mCurrentPrefs.mPowerDB.mPrecision;
   case ValueType::cPRESSURE:
      return mCurrentPrefs.mPressure.mPrecision;
   case ValueType::cRATIO:
      return mCurrentPrefs.mRatio.mPrecision;
   case ValueType::cSPEED:
      return mCurrentPrefs.mSpeed.mPrecision;
   case ValueType::cTIME:
      return mCurrentPrefs.mTime.mPrecision;
   case ValueType::cTORQUE:
      return mCurrentPrefs.mTorque.mPrecision;
   case ValueType::cLATITUDE: // intentional fall-through
   case ValueType::cLONGITUDE:
   default:
      return 6;
   }
}

QString wkf::UnitsObject::GetFormattedValue(ValueType aType, double aValue) const
{
   switch (aType)
   {
   case ValueType::cALTITUDE:
      return GetFormattedAltitude(aValue);
   case ValueType::cLATITUDE:
      return GetFormattedLatitude(aValue);
   case ValueType::cLONGITUDE:
      return GetFormattedLongitude(aValue);
   default:
      UtUnitType*  unitType       = UtUnits::GetUnitType(static_cast<UtUnits::UnitType>(aType));
      double       convertedValue = unitType->ConvertFromStandard(aValue, GetUnit(aType));
      unsigned int precision      = GetUnitPrecision(aType);
      double       roundedValue   = std::round(convertedValue * std::pow(10, precision)) / std::pow(10, precision);
      std::string  unitName       = unitType->FindUnitName(GetUnit(aType));

      std::stringstream stream;
      stream << std::fixed << std::setprecision(precision) << roundedValue << " " << unitName;
      return QString::fromStdString(stream.str());
   }
}

QString wkf::UnitsObject::GetFormattedAltitude(double aValue) const
{
   UtUnitLength::BaseUnit       unit = GetAltitudeUnit(aValue);
   UtUnitaryValue<UtUnitLength> value(aValue);
   double                       convertedValue = value.GetAsUnit(GetAltitudeUnit(aValue));
   unsigned int                 precision =
      (aValue > cALTITUDE_SEPARATOR) ? mCurrentPrefs.mHighAltitude.mPrecision : mCurrentPrefs.mLowAltitude.mPrecision;
   double      roundedValue = std::round(convertedValue * std::pow(10, precision)) / std::pow(10, precision);
   std::string unitName     = UtUnitLength::FindUnitName(unit);

   std::stringstream stream;
   stream << std::fixed << std::setprecision(precision) << roundedValue << " " << unitName;
   return QString::fromStdString(stream.str());
}

QString wkf::UnitsObject::GetFormattedLatitude(double aValue) const
{
   UtLatPos latpos(aValue);
   latpos.SetFormat(GetLatLonFormatting());
   std::stringstream stream;
   stream << latpos;
   return QString::fromStdString(stream.str());
}

QString wkf::UnitsObject::GetFormattedLongitude(double aValue) const
{
   UtLonPos lonpos(aValue);
   lonpos.SetFormat(GetLatLonFormatting());
   std::stringstream stream;
   stream << lonpos;
   return QString::fromStdString(stream.str());
}
