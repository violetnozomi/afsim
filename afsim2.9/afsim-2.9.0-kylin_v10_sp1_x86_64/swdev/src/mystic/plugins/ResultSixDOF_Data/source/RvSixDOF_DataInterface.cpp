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
#include "RvSixDOF_DataInterface.hpp"

#include <iomanip>
#include <set>
#include <sstream>

#include "RvEventPipeMessages.hpp"
#include "RvResultData.hpp"
#include "RvResultPlatform.hpp"
#include "RvSixDOF_EventPipeClasses.hpp"
#include "UtEntity.hpp"
#include "UtMath.hpp"

RvSixDOF_Data::Interface::Interface(QObject* parent)
   : QObject(parent)
   , mSixDOF_Data()
   , mIndex(SCENARIO)
{
}

/** Set the platform of interest, to see data about it. */
void RvSixDOF_Data::Interface::SetPlatformOfInterest(const std::string& aPlatformName)
{
   QMutexLocker locker(&mMutex);
   mSixDOF_Data.m_name = aPlatformName;
   mIndex              = LOOKUP_REQUIRED;
}

/** Advance time and read new data. */
void RvSixDOF_Data::Interface::AdvanceTimeRead(const rv::ResultData& aData)
{
   QMutexLocker locker(&mMutex);

   // If we need to do a lookup, do one.
   if (mIndex == LOOKUP_REQUIRED)
   {
      mIndex = aData.FindPlatformIndex(mSixDOF_Data.m_name);
   }

   // If we have a platform, read the data
   if (mIndex > 0)
   {
      rv::ResultPlatform* platform = aData.FindPlatform(mIndex);

      // Get visibility values
      mCoreDataVisible        = (platform && platform->HasData<rv::MsgSixDOF_CoreData>());
      mKinematicVisible       = (platform && platform->HasData<rv::MsgSixDOF_Kinematic>());
      mEngineFuelVisible      = (platform && platform->HasData<rv::MsgSixDOF_EngineFuel>());
      mAutopilotVisible       = (platform && platform->HasData<rv::MsgSixDOF_Autopilot>());
      mAutopilotLimitsVisible = (platform && platform->HasData<rv::MsgSixDOF_AutopilotLimits>());
      mControlInputsVisible   = (platform && platform->HasData<rv::MsgSixDOF_ControlInputs>());
      mControlSurfacesVisible = (platform && platform->HasData<rv::MsgSixDOF_ControlSurfaces>());
      mForceMomentVisible     = (platform && platform->HasData<rv::MsgSixDOF_ForceMoment>());
      mSixDOF_Visible =
         (mCoreDataVisible || mKinematicVisible || mEngineFuelVisible || mAutopilotVisible || mAutopilotLimitsVisible ||
          mControlInputsVisible || mControlSurfacesVisible || mForceMomentVisible || mForceRotationVisible);

      double simTime = aData.GetSimTime();

      // If the platform isn't null
      if (platform != nullptr)
      {
         // Read core data
         const rv::MsgSixDOF_CoreData* coreDataPtr = platform->FindFirstBefore<rv::MsgSixDOF_CoreData>(simTime);
         if (nullptr != coreDataPtr)
         {
            mSixDOF_Data.m_altitude_m = coreDataPtr->altitude_m();
            mSixDOF_Data.m_vertSpeed_fpm =
               coreDataPtr->vertSpeed_msec() * UtMath::cFT_PER_M * 60; // m/s * ft/m = ft/s * 60s/1min = ft/min (fpm)
            mSixDOF_Data.m_KCAS                = coreDataPtr->KCAS();
            mSixDOF_Data.m_KTAS                = coreDataPtr->KTAS();
            mSixDOF_Data.m_mach                = coreDataPtr->mach();
            mSixDOF_Data.m_heading_rad         = coreDataPtr->heading_rad();
            mSixDOF_Data.m_pitch_rad           = coreDataPtr->pitch_rad();
            mSixDOF_Data.m_roll_rad            = coreDataPtr->roll_rad();
            mSixDOF_Data.m_gLoad               = coreDataPtr->gLoad();
            mSixDOF_Data.m_alpha_rad           = coreDataPtr->alpha_rad();
            mSixDOF_Data.m_gAvail              = coreDataPtr->gAvail();
            mSixDOF_Data.m_speedbrakeAngle_rad = coreDataPtr->speedbrakeAngle_rad();
         }

         // Read kinematic data
         const rv::MsgSixDOF_Kinematic* kinematicDataPtr = platform->FindFirstBefore<rv::MsgSixDOF_Kinematic>(simTime);
         if (nullptr != kinematicDataPtr)
         {
            mSixDOF_Data.m_beta_rad      = kinematicDataPtr->beta_rad();
            mSixDOF_Data.m_alphaDot_rps  = kinematicDataPtr->alphaDot_rps();
            mSixDOF_Data.m_betaDot_rps   = kinematicDataPtr->betaDot_rps();
            mSixDOF_Data.m_yawRate_rps   = kinematicDataPtr->yawRate_rps();
            mSixDOF_Data.m_pitchRate_rps = kinematicDataPtr->pitchRate_rps();
            mSixDOF_Data.m_rollRate_rps  = kinematicDataPtr->rollRate_rps();
            mSixDOF_Data.m_dynamicPressure_pa =
               kinematicDataPtr->dynamicPressure_psf() * UtMath::cPASCAL_PER_PSF; // needs to be the SI unit.
            mSixDOF_Data.m_nx                  = kinematicDataPtr->nx();
            mSixDOF_Data.m_ny                  = kinematicDataPtr->ny();
            mSixDOF_Data.m_flightPathAngle_rad = kinematicDataPtr->flightPathAngle_rad();
            mSixDOF_Data.m_centerGravity_x     = kinematicDataPtr->centerGravity_x();
            mSixDOF_Data.m_centerGravity_y     = kinematicDataPtr->centerGravity_y();
            mSixDOF_Data.m_centerGravity_z     = kinematicDataPtr->centerGravity_z();
         }

         // Read engine and fuel data
         const rv::MsgSixDOF_EngineFuel* engineFuelDataPtr = platform->FindFirstBefore<rv::MsgSixDOF_EngineFuel>(simTime);
         if (nullptr != engineFuelDataPtr)
         {
            mSixDOF_Data.m_fuelFlow_pph =
               engineFuelDataPtr->fuelFlow_kgsec() * UtMath::cLB_PER_KG * 3600.0; // Convert from kg/sec to pph;
            mSixDOF_Data.m_fuelInternal_kg          = engineFuelDataPtr->fuelInternal_kg();
            mSixDOF_Data.m_fuelExternal_kg          = engineFuelDataPtr->fuelExternal_kg();
            mSixDOF_Data.m_fuelInternalRemaining_kg = engineFuelDataPtr->fuelInternalRemaining_kg();
            mSixDOF_Data.m_fuelExternalRemaining_kg = engineFuelDataPtr->fuelExternalRemaining_kg();
            mSixDOF_Data.m_totalWeight_kg           = engineFuelDataPtr->totalWeight_kg();
            mSixDOF_Data.mBits.set(0, engineFuelDataPtr->afterburner_on());
            mSixDOF_Data.mBits.set(1, engineFuelDataPtr->producing_thrust());
            mSixDOF_Data.mBits.set(2, engineFuelDataPtr->engine_operating());
            mSixDOF_Data.mBits.set(3, engineFuelDataPtr->engine_smoking());
            mSixDOF_Data.mBits.set(4, engineFuelDataPtr->contrailing());
            mSixDOF_Data.m_jokerFuelState_kg = engineFuelDataPtr->jokerFuelState_kg();
            mSixDOF_Data.m_bingoFuelState_kg = engineFuelDataPtr->bingoFuelState_kg();
            mSixDOF_Data.mBits.set(5, engineFuelDataPtr->weightOnWheels());
         }

         // Read autopilot data
         const rv::MsgSixDOF_Autopilot* autopilotDataPtr = platform->FindFirstBefore<rv::MsgSixDOF_Autopilot>(simTime);
         if (nullptr != autopilotDataPtr)
         {
            mSixDOF_Data.m_activePilot       = autopilotDataPtr->activePilot();
            mSixDOF_Data.m_lateralModeName   = autopilotDataPtr->lateralModeName();
            mSixDOF_Data.m_lateralModeValue  = autopilotDataPtr->lateralModeValue();
            mSixDOF_Data.m_verticalModeName  = autopilotDataPtr->verticalModeName();
            mSixDOF_Data.m_verticalModeValue = autopilotDataPtr->verticalModeValue();
            mSixDOF_Data.m_speedModeName     = autopilotDataPtr->speedModeName();
            mSixDOF_Data.m_speedModeValue    = autopilotDataPtr->speedModeValue();
            mSixDOF_Data.m_currentManeuver   = autopilotDataPtr->currentManeuver();
         }

         // Read autopilot limits data
         const rv::MsgSixDOF_AutopilotLimits* autopilotLimitsDataPtr =
            platform->FindFirstBefore<rv::MsgSixDOF_AutopilotLimits>(simTime);
         if (nullptr != autopilotLimitsDataPtr)
         {
            mSixDOF_Data.m_pitchGLoadMin    = autopilotLimitsDataPtr->pitchGLoadMin();
            mSixDOF_Data.m_pitchGLoadMax    = autopilotLimitsDataPtr->pitchGLoadMax();
            mSixDOF_Data.m_alphaMin         = autopilotLimitsDataPtr->alphaMin();
            mSixDOF_Data.m_alphaMax         = autopilotLimitsDataPtr->alphaMax();
            mSixDOF_Data.m_pitchRateMin     = autopilotLimitsDataPtr->pitchRateMin();
            mSixDOF_Data.m_pitchRateMax     = autopilotLimitsDataPtr->pitchRateMax();
            mSixDOF_Data.m_verticalSpeedMin = autopilotLimitsDataPtr->verticalSpeedMin();
            mSixDOF_Data.m_verticalSpeedMax = autopilotLimitsDataPtr->verticalSpeedMax();
            mSixDOF_Data.m_yawGLoadMax      = autopilotLimitsDataPtr->yawGLoadMax();
            mSixDOF_Data.m_betaMax          = autopilotLimitsDataPtr->betaMax();
            mSixDOF_Data.m_yawRateMax       = autopilotLimitsDataPtr->yawRateMax();
            mSixDOF_Data.m_rollRateMax      = autopilotLimitsDataPtr->rollRateMax();
            mSixDOF_Data.m_bankAngleMax     = autopilotLimitsDataPtr->bankAngleMax();
            mSixDOF_Data.m_forwardAccelMin  = autopilotLimitsDataPtr->forwardAccelMin();
            mSixDOF_Data.m_forwardAccelMax  = autopilotLimitsDataPtr->forwardAccelMax();
            mSixDOF_Data.m_taxiSpeedMax     = autopilotLimitsDataPtr->taxiSpeedMax();
            mSixDOF_Data.m_taxiYawRateMax   = autopilotLimitsDataPtr->taxiYawRateMax();
         }

         // Read control inputs data
         const rv::MsgSixDOF_ControlInputs* controlInputsDataPtr =
            platform->FindFirstBefore<rv::MsgSixDOF_ControlInputs>(simTime);
         if (nullptr != controlInputsDataPtr)
         {
            mSixDOF_Data.m_stickBack           = controlInputsDataPtr->stickBack();
            mSixDOF_Data.m_stickRight          = controlInputsDataPtr->stickRight();
            mSixDOF_Data.m_rudderRight         = controlInputsDataPtr->rudderRight();
            mSixDOF_Data.m_throttle            = controlInputsDataPtr->throttle();
            mSixDOF_Data.m_speedBrakeLever     = controlInputsDataPtr->speedBrakeLever();
            mSixDOF_Data.m_flapsLever          = controlInputsDataPtr->flapsLever();
            mSixDOF_Data.m_landingGearLever    = controlInputsDataPtr->landingGearLever();
            mSixDOF_Data.m_spoilersLever       = controlInputsDataPtr->spoilersLever();
            mSixDOF_Data.m_thrustReverserLever = controlInputsDataPtr->thrustReverserLever();
         }

         // Read control surfaces data
         const rv::MsgSixDOF_ControlSurfaces* controlSurfacesDataPtr =
            platform->FindFirstBefore<rv::MsgSixDOF_ControlSurfaces>(simTime);
         if (nullptr != controlSurfacesDataPtr)
         {
            mSixDOF_Data.m_surfaceNames  = controlSurfacesDataPtr->surfaceNames();
            mSixDOF_Data.m_surfaceAngles = controlSurfacesDataPtr->surfaceAngles();
         }

         // Read force and moment data
         const rv::MsgSixDOF_ForceMoment* forceMomentDataPtr =
            platform->FindFirstBefore<rv::MsgSixDOF_ForceMoment>(simTime);
         if (nullptr != forceMomentDataPtr)
         {
            mSixDOF_Data.m_liftForce_nt   = forceMomentDataPtr->liftForce_nt();
            mSixDOF_Data.m_dragForce_nt   = forceMomentDataPtr->dragForce_nt();
            mSixDOF_Data.m_thrustForce_nt = forceMomentDataPtr->thrustForce_nt();
            mSixDOF_Data.m_sideForce_nt   = forceMomentDataPtr->sideForce_nt();
            mSixDOF_Data.m_yawMoment      = forceMomentDataPtr->yawMoment();
            mSixDOF_Data.m_pitchMoment    = forceMomentDataPtr->pitchMoment();
            mSixDOF_Data.m_rollMoment     = forceMomentDataPtr->rollMoment();
         }
      } // if (platform != nullptr)
   }    // if (mIndex > 0)
}

/** Get the data container, mSixDOF_Data. */
const RvSixDOF_Data::Interface::SixDOF_DataContainer RvSixDOF_Data::Interface::GetSixDOF_Data()
{
   QMutexLocker locker(&mMutex);
   return mSixDOF_Data;
}

/** Get if SixDOF Data is being populated. */
bool RvSixDOF_Data::Interface::IsSixDOF_Visible()
{
   return mSixDOF_Visible;
}

/** Get if Core Data is being populated. */
bool RvSixDOF_Data::Interface::IsCoreDataVisible()
{
   return mCoreDataVisible;
}

/** Get if Kinematic is being populated. */
bool RvSixDOF_Data::Interface::IsKinematicVisible()
{
   return mKinematicVisible;
}

/** Get if Engine Fuel is being populated. */
bool RvSixDOF_Data::Interface::IsEngineFuelVisible()
{
   return mEngineFuelVisible;
}

/** Get if Autopilot is being populated. */
bool RvSixDOF_Data::Interface::IsAutopilotVisible()
{
   return mAutopilotVisible;
}

/** Get if Autopilot Limits is being populated. */
bool RvSixDOF_Data::Interface::IsAutopilotLimitsVisible()
{
   return mAutopilotLimitsVisible;
}

/** Get if Control Inputs is being populated. */
bool RvSixDOF_Data::Interface::IsControlInputsVisible()
{
   return mControlInputsVisible;
}

/** Get if Control Surfaces is being populated. */
bool RvSixDOF_Data::Interface::IsControlSurfacesVisible()
{
   return mControlSurfacesVisible;
}

/** Get if Force Moment is being populated. */
bool RvSixDOF_Data::Interface::IsForceMomentVisible()
{
   return mForceMomentVisible;
}
