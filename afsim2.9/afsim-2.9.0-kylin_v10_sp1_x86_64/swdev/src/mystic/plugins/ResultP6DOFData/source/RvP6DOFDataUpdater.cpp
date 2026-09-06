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
#include "RvP6DOFDataUpdater.hpp"

#include "RvP6dofEventPipeClasses.hpp"
#include "RvResultPlatform.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtLatPos.hpp"
#include "UtLonPos.hpp"
#include "UtMath.hpp"
#include "WkfEnvironment.hpp"
#include "WkfUnitsObject.hpp"

/** Read data from a platform at a sim time. */
bool RvP6DOFData::AltitudeUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofCoreData* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofCoreData>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->altitude_m();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::VerticalSpeedUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofCoreData* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofCoreData>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->vertSpeed_msec() * UtMath::cFT_PER_M * 60; // m/s * ft/m = ft/s * 60s/1min = ft/min (fpm);
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::KCAS_Updater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofCoreData* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofCoreData>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->KCAS();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::KTAS_Updater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofCoreData* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofCoreData>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->KCAS();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::MachUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofCoreData* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofCoreData>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->mach();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::HeadingUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofCoreData* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofCoreData>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->heading_rad();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::PitchUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofCoreData* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofCoreData>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->pitch_rad();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::RollUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofCoreData* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofCoreData>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->roll_rad();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::GLoadUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofCoreData* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofCoreData>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->gLoad();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::AlphaUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofCoreData* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofCoreData>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->alpha_rad();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::AvailGsUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofCoreData* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofCoreData>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->gAvail();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::SpeedBrakeAngleUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofCoreData* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofCoreData>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->speedbrakeAngle_rad();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::BetaUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofKinematic* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofKinematic>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->beta_rad();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::AlphaDotUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofKinematic* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofKinematic>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->alphaDot_rps();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::BetaDotUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofKinematic* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofKinematic>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->betaDot_rps();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::YawRateUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofKinematic* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofKinematic>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->yawRate_rps();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::PitchRateUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofKinematic* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofKinematic>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->pitchRate_rps();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::RollRateUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofKinematic* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofKinematic>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->rollRate_rps();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::DynamicPressureUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofKinematic* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofKinematic>(aSimTime);
   if (statePtr)
   {
      UtPressureValue v(statePtr->dynamicPressure_psf(), UtUnitPressure::cPSF);
      mValue = v;
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::NxUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofKinematic* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofKinematic>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->nx();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::NyUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofKinematic* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofKinematic>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->ny();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::FlightPathAngleUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofKinematic* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofKinematic>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->flightPathAngle_rad();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::CenterGravityXUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofKinematic* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofKinematic>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->centerGravity_x();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::CenterGravityYUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofKinematic* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofKinematic>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->centerGravity_y();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::CenterGravityZUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofKinematic* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofKinematic>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->centerGravity_z();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::FuelFlowUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofEngineFuel* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofEngineFuel>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->fuelFlow_kgsec();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::FuelInternalUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofEngineFuel* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofEngineFuel>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->fuelInternal_kg();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::FuelExternalUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofEngineFuel* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofEngineFuel>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->fuelExternal_kg();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::FuelInternalRemainingUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofEngineFuel* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofEngineFuel>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->fuelInternalRemaining_kg();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::FuelExternalRemainingUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofEngineFuel* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofEngineFuel>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->fuelExternalRemaining_kg();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::WeightUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofEngineFuel* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofEngineFuel>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->totalWeight_kg();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::AfterburnerUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofEngineFuel* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofEngineFuel>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->afterburner_on();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::ProducingThrustUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofEngineFuel* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofEngineFuel>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->producing_thrust();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::EngineOperatingUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofEngineFuel* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofEngineFuel>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->engine_operating();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::EngineSmokingUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofEngineFuel* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofEngineFuel>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->engine_smoking();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::ContrailingUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofEngineFuel* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofEngineFuel>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->contrailing();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::JokerFuelStateUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofEngineFuel* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofEngineFuel>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->jokerFuelState_kg();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::BingoFuelStateUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofEngineFuel* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofEngineFuel>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->bingoFuelState_kg();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::WeightOnWheelsUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofEngineFuel* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofEngineFuel>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->weightOnWheels();
   }
   return statePtr != nullptr;
}

/** Get the value as a string. */
QString RvP6DOFData::ActivePilotUpdater::GetValueString() const
{
   return mPilotType;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::ActivePilotUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofAutopilot* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofAutopilot>(aSimTime);
   if (statePtr)
   {
      mPilotType = statePtr->activePilot().c_str();
   }
   return statePtr != nullptr;
}

/** Get the value as a string. */
QString RvP6DOFData::LateralModeNameUpdater::GetValueString() const
{
   return mLateralModeName;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::LateralModeNameUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofAutopilot* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofAutopilot>(aSimTime);
   if (statePtr)
   {
      mLateralModeName = statePtr->lateralModeName().c_str();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::LateralModeValueUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofAutopilot* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofAutopilot>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->lateralModeValue();
   }
   return statePtr != nullptr;
}

/** Get the value as a string. */
QString RvP6DOFData::VerticalModeNameUpdater::GetValueString() const
{
   return mVerticalModeName;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::VerticalModeNameUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofAutopilot* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofAutopilot>(aSimTime);
   if (statePtr)
   {
      mVerticalModeName = statePtr->verticalModeName().c_str();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::VerticalModeValueUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofAutopilot* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofAutopilot>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->verticalModeValue();
   }
   return statePtr != nullptr;
}

/** Get the value as a string. */
QString RvP6DOFData::SpeedModeNameUpdater::GetValueString() const
{
   return mSpeedModeName;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::SpeedModeNameUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofAutopilot* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofAutopilot>(aSimTime);
   if (statePtr)
   {
      mSpeedModeName = statePtr->speedModeName().c_str();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::SpeedModeValueUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofAutopilot* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofAutopilot>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->speedModeValue();
   }
   return statePtr != nullptr;
}

/** Get the value as a string. */
QString RvP6DOFData::CurrentManeuverUpdater::GetValueString() const
{
   return mCurrentManeuver;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::CurrentManeuverUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofAutopilot* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofAutopilot>(aSimTime);
   if (statePtr)
   {
      mCurrentManeuver = QString::fromStdString(statePtr->currentManeuver());
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::PitchGLoadMinUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofAutopilotLimits* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofAutopilotLimits>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->pitchGLoadMin();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::PitchGLoadMaxUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofAutopilotLimits* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofAutopilotLimits>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->pitchGLoadMax();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::AlphaMinUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofAutopilotLimits* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofAutopilotLimits>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->alphaMin();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::AlphaMaxUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofAutopilotLimits* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofAutopilotLimits>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->alphaMax();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::PitchRateMinUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofAutopilotLimits* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofAutopilotLimits>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->pitchRateMin();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::PitchRateMaxUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofAutopilotLimits* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofAutopilotLimits>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->pitchRateMax();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::VerticalSpeedMinUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofAutopilotLimits* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofAutopilotLimits>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->verticalSpeedMin();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::VerticalSpeedMaxUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofAutopilotLimits* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofAutopilotLimits>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->verticalSpeedMax();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::YawGLoadMaxUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofAutopilotLimits* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofAutopilotLimits>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->yawGLoadMax();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::BetaMaxUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofAutopilotLimits* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofAutopilotLimits>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->betaMax();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::YawRateMaxUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofAutopilotLimits* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofAutopilotLimits>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->yawRateMax();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::RollRateMaxUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofAutopilotLimits* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofAutopilotLimits>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->rollRateMax();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::BankAngleMaxUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofAutopilotLimits* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofAutopilotLimits>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->bankAngleMax();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::ForwardAccelMinUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofAutopilotLimits* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofAutopilotLimits>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->forwardAccelMin();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::ForwardAccelMaxUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofAutopilotLimits* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofAutopilotLimits>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->forwardAccelMax();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::TaxiSpeedMaxUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofAutopilotLimits* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofAutopilotLimits>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->taxiSpeedMax();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::TaxiYawRateMaxUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofAutopilotLimits* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofAutopilotLimits>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->taxiYawRateMax();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::StickBackUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofControlInputs* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofControlInputs>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->stickBack();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::StickRightUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofControlInputs* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofControlInputs>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->stickRight();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::RudderUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofControlInputs* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofControlInputs>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->rudderRight();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::ThrottleUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofControlInputs* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofControlInputs>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->throttle();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::SpeedBrakeLeverUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofControlInputs* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofControlInputs>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->speedBrakeLever();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::FlapsLeverUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofControlInputs* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofControlInputs>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->flapsLever();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::LandingGearLeverUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofControlInputs* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofControlInputs>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->landingGearLever();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::SpoilersLeverUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofControlInputs* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofControlInputs>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->spoilersLever();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::ThrustReverserLeverUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofControlInputs* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofControlInputs>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->thrustReverserLever();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::LiftUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofForceMoment* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofForceMoment>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->liftForce_nt();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::DragUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofForceMoment* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofForceMoment>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->dragForce_nt();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::ThrustUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofForceMoment* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofForceMoment>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->thrustForce_nt();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::SideForceUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofForceMoment* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofForceMoment>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->sideForce_nt();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::YawMomentUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofForceMoment* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofForceMoment>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->yawMoment();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::PitchMomentUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofForceMoment* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofForceMoment>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->pitchMoment();
   }
   return statePtr != nullptr;
}

/** Read data from a platform at a sim time. */
bool RvP6DOFData::RollMomentUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const rv::MsgP6dofForceMoment* statePtr = aPlatform->FindFirstBefore<rv::MsgP6dofForceMoment>(aSimTime);
   if (statePtr)
   {
      mValue = statePtr->rollMoment();
   }
   return statePtr != nullptr;
}
