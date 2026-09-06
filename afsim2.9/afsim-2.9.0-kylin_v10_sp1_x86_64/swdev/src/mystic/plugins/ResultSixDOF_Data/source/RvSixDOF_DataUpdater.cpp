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
#include "RvSixDOF_DataUpdater.hpp"

#include "RvResultPlatform.hpp"
#include "RvSixDOF_EventPipeClasses.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtLatPos.hpp"
#include "UtLonPos.hpp"
#include "UtMath.hpp"
#include "WkfEnvironment.hpp"
#include "WkfUnitsObject.hpp"

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::AltitudeUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_CoreData* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_CoreData>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->altitude_m();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::VerticalSpeedUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_CoreData* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_CoreData>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->vertSpeed_msec() * UtMath::cFT_PER_M * 60; // m/s * ft/m = ft/s * 60s/1min = ft/min (fpm);
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::KCAS_Updater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_CoreData* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_CoreData>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->KCAS();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::KTAS_Updater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_CoreData* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_CoreData>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->KCAS();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::MachUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_CoreData* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_CoreData>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->mach();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::HeadingUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_CoreData* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_CoreData>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->heading_rad();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::PitchUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_CoreData* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_CoreData>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->pitch_rad();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::RollUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_CoreData* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_CoreData>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->roll_rad();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::GLoadUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_CoreData* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_CoreData>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->gLoad();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::AlphaUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_CoreData* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_CoreData>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->alpha_rad();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::AvailGsUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_CoreData* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_CoreData>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->gAvail();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::SpeedBrakeAngleUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_CoreData* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_CoreData>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->speedbrakeAngle_rad();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::BetaUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_Kinematic* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_Kinematic>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->beta_rad();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::AlphaDotUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_Kinematic* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_Kinematic>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->alphaDot_rps();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::BetaDotUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_Kinematic* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_Kinematic>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->betaDot_rps();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::YawRateUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_Kinematic* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_Kinematic>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->yawRate_rps();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::PitchRateUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_Kinematic* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_Kinematic>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->pitchRate_rps();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::RollRateUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_Kinematic* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_Kinematic>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->rollRate_rps();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::DynamicPressureUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_Kinematic* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_Kinematic>(aSimTime);
   if (statePtr)
   {
      UtPressureValue v(statePtr->dynamicPressure_psf(), UtUnitPressure::cPSF);
      mValue = v;
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::NxUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_Kinematic* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_Kinematic>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->nx();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::NyUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_Kinematic* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_Kinematic>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->ny();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::FlightPathAngleUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_Kinematic* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_Kinematic>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->flightPathAngle_rad();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::CenterGravityXUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_Kinematic* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_Kinematic>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->centerGravity_x();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::CenterGravityYUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_Kinematic* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_Kinematic>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->centerGravity_y();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::CenterGravityZUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_Kinematic* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_Kinematic>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->centerGravity_z();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::FuelFlowUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_EngineFuel* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_EngineFuel>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->fuelFlow_kgsec();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::FuelInternalUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_EngineFuel* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_EngineFuel>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->fuelInternal_kg();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::FuelExternalUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_EngineFuel* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_EngineFuel>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->fuelExternal_kg();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::FuelInternalRemainingUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_EngineFuel* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_EngineFuel>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->fuelInternalRemaining_kg();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::FuelExternalRemainingUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_EngineFuel* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_EngineFuel>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->fuelExternalRemaining_kg();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::WeightUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_EngineFuel* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_EngineFuel>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->totalWeight_kg();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::AfterburnerUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_EngineFuel* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_EngineFuel>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->afterburner_on();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::ProducingThrustUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_EngineFuel* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_EngineFuel>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->producing_thrust();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::EngineOperatingUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_EngineFuel* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_EngineFuel>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->engine_operating();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::EngineSmokingUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_EngineFuel* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_EngineFuel>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->engine_smoking();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::ContrailingUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_EngineFuel* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_EngineFuel>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->contrailing();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::JokerFuelStateUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_EngineFuel* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_EngineFuel>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->jokerFuelState_kg();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::BingoFuelStateUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_EngineFuel* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_EngineFuel>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->bingoFuelState_kg();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::WeightOnWheelsUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_EngineFuel* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_EngineFuel>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->weightOnWheels();
   }
   return statePtr != nullptr;
}

/** Get the value as a string. */
QString RvSixDOF_Data::ActivePilotUpdater::GetValueString() const
{
   return mPilotType;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::ActivePilotUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_Autopilot* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_Autopilot>(aSimTime);
   if (statePtr)
   {
      mPilotType = statePtr->activePilot().c_str();
   }
   return statePtr != nullptr;
}

/** Get the value as a string. */
QString RvSixDOF_Data::LateralModeNameUpdater::GetValueString() const
{
   return mLateralModeName;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::LateralModeNameUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_Autopilot* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_Autopilot>(aSimTime);
   if (statePtr)
   {
      mLateralModeName = statePtr->lateralModeName().c_str();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::LateralModeValueUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_Autopilot* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_Autopilot>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->lateralModeValue();
   }
   return statePtr != nullptr;
}

/** Get the value as a string. */
QString RvSixDOF_Data::VerticalModeNameUpdater::GetValueString() const
{
   return mVerticalModeName;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::VerticalModeNameUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_Autopilot* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_Autopilot>(aSimTime);
   if (statePtr)
   {
      mVerticalModeName = QString::fromStdString(statePtr->verticalModeName());
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::VerticalModeValueUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_Autopilot* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_Autopilot>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->verticalModeValue();
   }
   return statePtr != nullptr;
}

/** Get the value as a string. */
QString RvSixDOF_Data::SpeedModeNameUpdater::GetValueString() const
{
   return mSpeedModeName;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::SpeedModeNameUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_Autopilot* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_Autopilot>(aSimTime);
   if (statePtr)
   {
      mSpeedModeName = QString::fromStdString(statePtr->speedModeName());
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::SpeedModeValueUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_Autopilot* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_Autopilot>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->speedModeValue();
   }
   return statePtr != nullptr;
}

/** Get the value as a string. */
QString RvSixDOF_Data::CurrentManeuverUpdater::GetValueString() const
{
   return mCurrentManeuver;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::CurrentManeuverUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_Autopilot* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_Autopilot>(aSimTime);
   if (statePtr)
   {
      mCurrentManeuver = QString::fromStdString(statePtr->currentManeuver());
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::PitchGLoadMinUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_AutopilotLimits* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_AutopilotLimits>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->pitchGLoadMin();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::PitchGLoadMaxUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_AutopilotLimits* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_AutopilotLimits>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->pitchGLoadMax();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::AlphaMinUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_AutopilotLimits* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_AutopilotLimits>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->alphaMin();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::AlphaMaxUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_AutopilotLimits* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_AutopilotLimits>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->alphaMax();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::PitchRateMinUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_AutopilotLimits* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_AutopilotLimits>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->pitchRateMin();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::PitchRateMaxUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_AutopilotLimits* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_AutopilotLimits>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->pitchRateMax();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::VerticalSpeedMinUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_AutopilotLimits* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_AutopilotLimits>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->verticalSpeedMin();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::VerticalSpeedMaxUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_AutopilotLimits* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_AutopilotLimits>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->verticalSpeedMax();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::YawGLoadMaxUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_AutopilotLimits* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_AutopilotLimits>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->yawGLoadMax();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::BetaMaxUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_AutopilotLimits* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_AutopilotLimits>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->betaMax();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::YawRateMaxUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_AutopilotLimits* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_AutopilotLimits>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->yawRateMax();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::RollRateMaxUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_AutopilotLimits* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_AutopilotLimits>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->rollRateMax();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::BankAngleMaxUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_AutopilotLimits* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_AutopilotLimits>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->bankAngleMax();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::ForwardAccelMinUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_AutopilotLimits* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_AutopilotLimits>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->forwardAccelMin();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::ForwardAccelMaxUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_AutopilotLimits* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_AutopilotLimits>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->forwardAccelMax();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::TaxiSpeedMaxUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_AutopilotLimits* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_AutopilotLimits>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->taxiSpeedMax();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::TaxiYawRateMaxUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_AutopilotLimits* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_AutopilotLimits>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->taxiYawRateMax();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::StickBackUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_ControlInputs* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_ControlInputs>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->stickBack();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::StickRightUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_ControlInputs* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_ControlInputs>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->stickRight();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::RudderUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_ControlInputs* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_ControlInputs>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->rudderRight();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::ThrottleUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_ControlInputs* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_ControlInputs>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->throttle();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::SpeedBrakeLeverUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_ControlInputs* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_ControlInputs>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->speedBrakeLever();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::FlapsLeverUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_ControlInputs* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_ControlInputs>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->flapsLever();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::LandingGearLeverUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_ControlInputs* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_ControlInputs>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->landingGearLever();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::SpoilersLeverUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_ControlInputs* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_ControlInputs>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->spoilersLever();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::ThrustReverserLeverUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_ControlInputs* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_ControlInputs>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->thrustReverserLever();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::LiftUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_ForceMoment* momentPtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_ForceMoment>(aSimTime);
   if (momentPtr)
   {
      mValue = momentPtr->liftForce_nt();
      return true;
   }
   return false;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::DragUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_ForceMoment* momentPtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_ForceMoment>(aSimTime);
   if (momentPtr)
   {
      mValue = momentPtr->dragForce_nt();
      return true;
   }
   return false;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::ThrustUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_ForceMoment* momentPtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_ForceMoment>(aSimTime);
   if (momentPtr)
   {
      mValue = momentPtr->thrustForce_nt();
      return true;
   }
   return false;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::SideForceUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_ForceMoment* momentPtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_ForceMoment>(aSimTime);
   if (momentPtr)
   {
      mValue = momentPtr->sideForce_nt();
      return true;
   }
   return false;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::YawMomentUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_ForceMoment* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_ForceMoment>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->yawMoment();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::PitchMomentUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_ForceMoment* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_ForceMoment>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->pitchMoment();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvSixDOF_Data::RollMomentUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgSixDOF_ForceMoment* statePtr = aPlatform->FindFirstBefore<rv::MsgSixDOF_ForceMoment>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->rollMoment();
   }
   return statePtr != nullptr;
}
