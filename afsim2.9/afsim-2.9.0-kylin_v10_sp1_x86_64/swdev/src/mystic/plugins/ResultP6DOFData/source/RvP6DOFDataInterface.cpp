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
#include "RvP6DOFDataInterface.hpp"

#include <iomanip>
#include <set>
#include <sstream>

#include "RvEventPipeMessages.hpp"
#include "RvP6dofEventPipeClasses.hpp"
#include "RvResultData.hpp"
#include "RvResultPlatform.hpp"
#include "UtEntity.hpp"
#include "UtMath.hpp"

RvP6DOFData::Interface::Interface(QObject* parent)
   : QObject(parent)
   , mP6dofData()
   , mIndex(SCENARIO)
{
}

/** Set the platform of interest, to see data about it. */
void RvP6DOFData::Interface::SetPlatformOfInterest(const std::string& aPlatformName)
{
   QMutexLocker locker(&mMutex);
   mP6dofData.m_name = aPlatformName.c_str();
   mIndex            = LOOKUP_REQUIRED;
}

/** Advance time and read new data. */
void RvP6DOFData::Interface::AdvanceTimeRead(const rv::ResultData& aData)
{
   QMutexLocker locker(&mMutex);

   // If we need to do a lookup, do one.
   if (mIndex == LOOKUP_REQUIRED)
   {
      mIndex = aData.FindPlatformIndex(mP6dofData.m_name);
   }

   // If we have a platform, read the data
   if (mIndex > 0)
   {
      rv::ResultPlatform* platform = aData.FindPlatform(mIndex);

      // Get visibility values
      mCoreDataVisible        = (platform && platform->HasData<rv::MsgP6dofCoreData>());
      mKinematicVisible       = (platform && platform->HasData<rv::MsgP6dofKinematic>());
      mEngineFuelVisible      = (platform && platform->HasData<rv::MsgP6dofEngineFuel>());
      mAutopilotVisible       = (platform && platform->HasData<rv::MsgP6dofAutopilot>());
      mAutopilotLimitsVisible = (platform && platform->HasData<rv::MsgP6dofAutopilotLimits>());
      mControlInputsVisible   = (platform && platform->HasData<rv::MsgP6dofControlInputs>());
      mControlSurfacesVisible = (platform && platform->HasData<rv::MsgP6dofControlSurfaces>());
      mForceMomentVisible     = (platform && platform->HasData<rv::MsgP6dofForceMoment>());
      mP6DofVisible           = (mCoreDataVisible || mKinematicVisible || mEngineFuelVisible || mAutopilotVisible ||
                       mAutopilotLimitsVisible || mControlInputsVisible || mControlSurfacesVisible || mForceMomentVisible);

      double simTime = aData.GetSimTime();

      // If the platform isn't null
      if (platform != nullptr)
      {
         // Read core data
         const rv::MsgP6dofCoreData* coreDataPtr = platform->FindFirstBefore<rv::MsgP6dofCoreData>(simTime);
         if (nullptr != coreDataPtr)
         {
            mP6dofData.m_altitude_m = coreDataPtr->altitude_m();
            mP6dofData.m_vertSpeed_fpm =
               coreDataPtr->vertSpeed_msec() * UtMath::cFT_PER_M * 60; // m/s * ft/m = ft/s * 60s/1min = ft/min (fpm)
            mP6dofData.m_KCAS                = coreDataPtr->KCAS();
            mP6dofData.m_KTAS                = coreDataPtr->KTAS();
            mP6dofData.m_mach                = coreDataPtr->mach();
            mP6dofData.m_heading_rad         = coreDataPtr->heading_rad();
            mP6dofData.m_pitch_rad           = coreDataPtr->pitch_rad();
            mP6dofData.m_roll_rad            = coreDataPtr->roll_rad();
            mP6dofData.m_gLoad               = coreDataPtr->gLoad();
            mP6dofData.m_alpha_rad           = coreDataPtr->alpha_rad();
            mP6dofData.m_gAvail              = coreDataPtr->gAvail();
            mP6dofData.m_speedbrakeAngle_rad = coreDataPtr->speedbrakeAngle_rad();
         }

         // Read kinematic data
         const rv::MsgP6dofKinematic* kinematicDataPtr = platform->FindFirstBefore<rv::MsgP6dofKinematic>(simTime);
         if (nullptr != kinematicDataPtr)
         {
            mP6dofData.m_beta_rad      = kinematicDataPtr->beta_rad();
            mP6dofData.m_alphaDot_rps  = kinematicDataPtr->alphaDot_rps();
            mP6dofData.m_betaDot_rps   = kinematicDataPtr->betaDot_rps();
            mP6dofData.m_yawRate_rps   = kinematicDataPtr->yawRate_rps();
            mP6dofData.m_pitchRate_rps = kinematicDataPtr->pitchRate_rps();
            mP6dofData.m_rollRate_rps  = kinematicDataPtr->rollRate_rps();
            mP6dofData.m_dynamicPressure_pa =
               kinematicDataPtr->dynamicPressure_psf() * UtMath::cPASCAL_PER_PSF; // needs to be the SI unit.
            mP6dofData.m_nx                  = kinematicDataPtr->nx();
            mP6dofData.m_ny                  = kinematicDataPtr->ny();
            mP6dofData.m_flightPathAngle_rad = kinematicDataPtr->flightPathAngle_rad();
            mP6dofData.m_centerGravity_x     = kinematicDataPtr->centerGravity_x();
            mP6dofData.m_centerGravity_y     = kinematicDataPtr->centerGravity_y();
            mP6dofData.m_centerGravity_z     = kinematicDataPtr->centerGravity_z();
         }

         // Read engine and fuel data
         const rv::MsgP6dofEngineFuel* engineFuelDataPtr = platform->FindFirstBefore<rv::MsgP6dofEngineFuel>(simTime);
         if (nullptr != engineFuelDataPtr)
         {
            mP6dofData.m_fuelFlow_pph =
               engineFuelDataPtr->fuelFlow_kgsec() * UtMath::cLB_PER_KG * 3600.0; // Convert from kg/sec to pph;
            mP6dofData.m_fuelInternal_kg          = engineFuelDataPtr->fuelInternal_kg();
            mP6dofData.m_fuelExternal_kg          = engineFuelDataPtr->fuelExternal_kg();
            mP6dofData.m_fuelInternalRemaining_kg = engineFuelDataPtr->fuelInternalRemaining_kg();
            mP6dofData.m_fuelExternalRemaining_kg = engineFuelDataPtr->fuelExternalRemaining_kg();
            mP6dofData.m_totalWeight_kg           = engineFuelDataPtr->totalWeight_kg();
            mP6dofData.mBits.set(0, engineFuelDataPtr->afterburner_on());
            mP6dofData.mBits.set(1, engineFuelDataPtr->producing_thrust());
            mP6dofData.mBits.set(2, engineFuelDataPtr->engine_operating());
            mP6dofData.mBits.set(3, engineFuelDataPtr->engine_smoking());
            mP6dofData.mBits.set(4, engineFuelDataPtr->contrailing());
            mP6dofData.m_jokerFuelState_kg = engineFuelDataPtr->jokerFuelState_kg();
            mP6dofData.m_bingoFuelState_kg = engineFuelDataPtr->bingoFuelState_kg();
            mP6dofData.mBits.set(5, engineFuelDataPtr->weightOnWheels());
         }

         // Read autopilot data
         const rv::MsgP6dofAutopilot* autopilotDataPtr = platform->FindFirstBefore<rv::MsgP6dofAutopilot>(simTime);
         if (nullptr != autopilotDataPtr)
         {
            mP6dofData.m_activePilot       = autopilotDataPtr->activePilot();
            mP6dofData.m_lateralModeName   = autopilotDataPtr->lateralModeName();
            mP6dofData.m_lateralModeValue  = autopilotDataPtr->lateralModeValue();
            mP6dofData.m_verticalModeName  = autopilotDataPtr->verticalModeName();
            mP6dofData.m_verticalModeValue = autopilotDataPtr->verticalModeValue();
            mP6dofData.m_speedModeName     = autopilotDataPtr->speedModeName();
            mP6dofData.m_speedModeValue    = autopilotDataPtr->speedModeValue();
            mP6dofData.m_currentManeuver   = autopilotDataPtr->currentManeuver();
         }

         // Read autopilot limits data
         const rv::MsgP6dofAutopilotLimits* autopilotLimitsDataPtr =
            platform->FindFirstBefore<rv::MsgP6dofAutopilotLimits>(simTime);
         if (nullptr != autopilotLimitsDataPtr)
         {
            mP6dofData.m_pitchGLoadMin    = autopilotLimitsDataPtr->pitchGLoadMin();
            mP6dofData.m_pitchGLoadMax    = autopilotLimitsDataPtr->pitchGLoadMax();
            mP6dofData.m_alphaMin         = autopilotLimitsDataPtr->alphaMin();
            mP6dofData.m_alphaMax         = autopilotLimitsDataPtr->alphaMax();
            mP6dofData.m_pitchRateMin     = autopilotLimitsDataPtr->pitchRateMin();
            mP6dofData.m_pitchRateMax     = autopilotLimitsDataPtr->pitchRateMax();
            mP6dofData.m_verticalSpeedMin = autopilotLimitsDataPtr->verticalSpeedMin();
            mP6dofData.m_verticalSpeedMax = autopilotLimitsDataPtr->verticalSpeedMax();
            mP6dofData.m_yawGLoadMax      = autopilotLimitsDataPtr->yawGLoadMax();
            mP6dofData.m_betaMax          = autopilotLimitsDataPtr->betaMax();
            mP6dofData.m_yawRateMax       = autopilotLimitsDataPtr->yawRateMax();
            mP6dofData.m_rollRateMax      = autopilotLimitsDataPtr->rollRateMax();
            mP6dofData.m_bankAngleMax     = autopilotLimitsDataPtr->bankAngleMax();
            mP6dofData.m_forwardAccelMin  = autopilotLimitsDataPtr->forwardAccelMin();
            mP6dofData.m_forwardAccelMax  = autopilotLimitsDataPtr->forwardAccelMax();
            mP6dofData.m_taxiSpeedMax     = autopilotLimitsDataPtr->taxiSpeedMax();
            mP6dofData.m_taxiYawRateMax   = autopilotLimitsDataPtr->taxiYawRateMax();
         }

         // Read control inputs data
         const rv::MsgP6dofControlInputs* controlInputsDataPtr =
            platform->FindFirstBefore<rv::MsgP6dofControlInputs>(simTime);
         if (nullptr != controlInputsDataPtr)
         {
            mP6dofData.m_stickBack           = controlInputsDataPtr->stickBack();
            mP6dofData.m_stickRight          = controlInputsDataPtr->stickRight();
            mP6dofData.m_rudderRight         = controlInputsDataPtr->rudderRight();
            mP6dofData.m_throttle            = controlInputsDataPtr->throttle();
            mP6dofData.m_speedBrakeLever     = controlInputsDataPtr->speedBrakeLever();
            mP6dofData.m_flapsLever          = controlInputsDataPtr->flapsLever();
            mP6dofData.m_landingGearLever    = controlInputsDataPtr->landingGearLever();
            mP6dofData.m_spoilersLever       = controlInputsDataPtr->spoilersLever();
            mP6dofData.m_thrustReverserLever = controlInputsDataPtr->thrustReverserLever();
         }

         // Read control surfaces data
         const rv::MsgP6dofControlSurfaces* controlSurfacesDataPtr =
            platform->FindFirstBefore<rv::MsgP6dofControlSurfaces>(simTime);
         if (nullptr != controlSurfacesDataPtr)
         {
            mP6dofData.m_surfaceNames  = controlSurfacesDataPtr->surfaceNames();
            mP6dofData.m_surfaceAngles = controlSurfacesDataPtr->surfaceAngles();
         }

         // Read force and moment data
         const rv::MsgP6dofForceMoment* forceMomentDataPtr = platform->FindFirstBefore<rv::MsgP6dofForceMoment>(simTime);
         if (nullptr != forceMomentDataPtr)
         {
            mP6dofData.m_liftForce_nt   = forceMomentDataPtr->liftForce_nt();
            mP6dofData.m_dragForce_nt   = forceMomentDataPtr->dragForce_nt();
            mP6dofData.m_thrustForce_nt = forceMomentDataPtr->thrustForce_nt();
            mP6dofData.m_sideForce_nt   = forceMomentDataPtr->sideForce_nt();
            mP6dofData.m_yawMoment      = forceMomentDataPtr->yawMoment();
            mP6dofData.m_pitchMoment    = forceMomentDataPtr->pitchMoment();
            mP6dofData.m_rollMoment     = forceMomentDataPtr->rollMoment();
         }
      } // if (platform != nullptr)
   }    // if (mIndex > 0)
}

/** Get the data container, mP6dofData. */
const RvP6DOFData::Interface::P6dofDataContainer RvP6DOFData::Interface::GetP6DOFData()
{
   QMutexLocker locker(&mMutex);
   return mP6dofData;
}

/** Get if P6Dof Data is being populated. */
bool RvP6DOFData::Interface::IsP6DofVisible()
{
   return mP6DofVisible;
}

/** Get if Core Data is being populated. */
bool RvP6DOFData::Interface::IsCoreDataVisible()
{
   return mCoreDataVisible;
}

/** Get if Kinematic is being populated. */
bool RvP6DOFData::Interface::IsKinematicVisible()
{
   return mKinematicVisible;
}

/** Get if Engine Fuel is being populated. */
bool RvP6DOFData::Interface::IsEngineFuelVisible()
{
   return mEngineFuelVisible;
}

/** Get if Autopilot is being populated. */
bool RvP6DOFData::Interface::IsAutopilotVisible()
{
   return mAutopilotVisible;
}

/** Get if Autopilot Limits is being populated. */
bool RvP6DOFData::Interface::IsAutopilotLimitsVisible()
{
   return mAutopilotLimitsVisible;
}

/** Get if Control Inputs is being populated. */
bool RvP6DOFData::Interface::IsControlInputsVisible()
{
   return mControlInputsVisible;
}

/** Get if Control Surfaces is being populated. */
bool RvP6DOFData::Interface::IsControlSurfacesVisible()
{
   return mControlSurfacesVisible;
}

/** Get if Force Moment is being populated. */
bool RvP6DOFData::Interface::IsForceMomentVisible()
{
   return mForceMomentVisible;
}
