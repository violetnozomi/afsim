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

#include "WsfSixDOF_RocketLiquidPropellantEngine.hpp"

#include <fstream>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtInputFile.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "WsfSixDOF_FuelTank.hpp"
#include "WsfSixDOF_Mover.hpp"
#include "WsfSixDOF_PropulsionSystem.hpp"
#include "WsfSixDOF_TypeManager.hpp"
#include "WsfSixDOF_Utils.hpp"

wsf::six_dof::RocketLiquidPropellantEngine::RocketLiquidPropellantEngine(ThrustProducerObject* aParentObject)
   : Engine(aParentObject)
{
   mTypeOfEngine = "LiquidPropellantRocket";
}

wsf::six_dof::RocketLiquidPropellantEngine::~RocketLiquidPropellantEngine() {}

wsf::six_dof::RocketLiquidPropellantEngine::RocketLiquidPropellantEngine(const RocketLiquidPropellantEngine& aSrc)
   : Engine(aSrc)
   , mThrottleLeverPosition(aSrc.mThrottleLeverPosition)
   , mLastThrottleLeverPosition(aSrc.mLastThrottleLeverPosition)
   , mNormalizedThrustAlt(aSrc.mNormalizedThrustAlt)
   , mIspAlt(aSrc.mIspAlt)
   , mThrottleVersusTime(aSrc.mThrottleVersusTime)
   , mMaxThrustSeaLevel(aSrc.mMaxThrustSeaLevel)
   , mMaxThrottle(aSrc.mMaxThrottle)
   , mMinThrottle(aSrc.mMinThrottle)
   , mMaxMassFlowSeaLevel(aSrc.mMaxMassFlowSeaLevel)
   , mMaxMassFlowVacuum(aSrc.mMaxMassFlowVacuum)
   , mEngineThrustPercent(aSrc.mEngineThrustPercent)
   , mIsBurning(aSrc.mIsBurning)
   , mBurnTime_Sec(aSrc.mBurnTime_Sec)
   , mNormalizedSpinUp_per_sec(aSrc.mNormalizedSpinUp_per_sec)
   , mNormalizedSpinDown_per_sec(aSrc.mNormalizedSpinDown_per_sec)
   , mGenerateSmokeTrail(aSrc.mGenerateSmokeTrail)
{
}

wsf::six_dof::Engine* wsf::six_dof::RocketLiquidPropellantEngine::Clone() const
{
   return new RocketLiquidPropellantEngine(*this);
}

void wsf::six_dof::RocketLiquidPropellantEngine::DeriveFrom(const Engine* aSrc)
{
   auto rocketSrc = dynamic_cast<const RocketLiquidPropellantEngine*>(aSrc);

   if (rocketSrc == nullptr)
   {
      return;
   }

   Engine::DeriveFrom(rocketSrc);

   mNormalizedThrustAlt = rocketSrc->mNormalizedThrustAlt;
   mIspAlt              = rocketSrc->mIspAlt;
   mThrottleVersusTime  = rocketSrc->mThrottleVersusTime;

   mCurrentFuelTank            = rocketSrc->mCurrentFuelTank;
   mThrottleLeverPosition      = rocketSrc->mThrottleLeverPosition;
   mLastThrottleLeverPosition  = rocketSrc->mLastThrottleLeverPosition;
   mMaxThrustSeaLevel          = rocketSrc->mMaxThrustSeaLevel;
   mMaxThrottle                = rocketSrc->mMaxThrottle;
   mMinThrottle                = rocketSrc->mMinThrottle;
   mMaxMassFlowSeaLevel        = rocketSrc->mMaxMassFlowSeaLevel;
   mMaxMassFlowVacuum          = rocketSrc->mMaxMassFlowVacuum;
   mEngineThrustPercent        = rocketSrc->mEngineThrustPercent;
   mIsBurning                  = rocketSrc->mIsBurning;
   mBurnTime_Sec               = rocketSrc->mBurnTime_Sec;
   mNormalizedSpinUp_per_sec   = rocketSrc->mNormalizedSpinUp_per_sec;
   mNormalizedSpinDown_per_sec = rocketSrc->mNormalizedSpinDown_per_sec;
   mCurrentFuelBurnRate_pph    = 0.0;

   mGenerateSmokeTrail = rocketSrc->mGenerateSmokeTrail;
}

bool wsf::six_dof::RocketLiquidPropellantEngine::ProcessInput(UtInput& aInput, TypeManager* aTypeManager)
{
   bool               myCommand = true;
   const std::string& command   = aInput.GetCommand();

   if (command == "liquid_propellant_rocket")
   {
      double maxThrustVacuum = -1;

      UtInputBlock block(aInput);
      while (block.ReadCommand())
      {
         std::string localCommand;
         localCommand = block.GetCommand();

         if (localCommand == "normalized_thrust_vs_alt")
         {
            mNormalizedThrustAlt = ut::make_unique<UtTable::Curve>();
            mNormalizedThrustAlt->ProcessInput(aInput,
                                               UtInput::cNON_DIMENSIONAL,
                                               "alt_ft",
                                               UtTable::NoCheck(),
                                               UtInput::cNON_DIMENSIONAL,
                                               "normalized_thrust",
                                               UtTable::NoCheck());
         }
         else if (localCommand == "isp_vs_alt")
         {
            mIspAlt = ut::make_unique<UtTable::Curve>();
            mIspAlt->ProcessInput(aInput,
                                  UtInput::cNON_DIMENSIONAL,
                                  "alt_ft",
                                  UtTable::NoCheck(),
                                  UtInput::cNON_DIMENSIONAL,
                                  "isp_sec",
                                  UtTable::ValueGE(0.0));
         }
         else if (localCommand == "max_thrust_sealevel")
         {
            double value = 0.0;
            aInput.ReadValueOfType(value, UtInput::cFORCE);
            value *= UtMath::cLB_PER_NT;
            mMaxThrustSeaLevel = value;
         }
         else if (localCommand == "max_thrust_vacuum")
         {
            double value = 0.0;
            aInput.ReadValueOfType(value, UtInput::cFORCE);
            value *= UtMath::cLB_PER_NT;
            maxThrustVacuum = value;
         }
         else if (localCommand == "normalized_spinup")
         {
            double value = 0.0;
            aInput.ReadValue(value);
            mNormalizedSpinUp_per_sec = value;
         }
         else if (localCommand == "normalized_spindown")
         {
            double value = 0.0;
            aInput.ReadValue(value);
            mNormalizedSpinDown_per_sec = value;
         }
         else if (localCommand == "creates_smoke_trail")
         {
            bool value;
            aInput.ReadValue(value);
            mGenerateSmokeTrail = value;
         }
         else
         {
            auto out = ut::log::error() << "Unrecognized command within RocketLiquidPropellantEngine::ProcessInput().";
            out.AddNote() << "Command: " << localCommand;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
      }

      // Calculate data
      mMaxMassFlowSeaLevel = mMaxThrustSeaLevel / mIspAlt->Lookup(0.0);

      if (maxThrustVacuum != -1)
      {
         if (mNormalizedThrustAlt != nullptr)
         {
            mMaxThrustSeaLevel =
               mNormalizedThrustAlt->Lookup(0.0) * maxThrustVacuum / mNormalizedThrustAlt->Lookup(300000.0);
         }
         else
         {
            mMaxThrustSeaLevel = maxThrustVacuum;
         }
      }
   }

   return myCommand;
}

bool wsf::six_dof::RocketLiquidPropellantEngine::Initialize(int64_t) //(int64_t aSimTime_nanosec)
{
   // Calculate data
   mMaxMassFlowSeaLevel = mMaxThrustSeaLevel / mIspAlt->Lookup(0.0);

   return true;
}

void wsf::six_dof::RocketLiquidPropellantEngine::CalculateThrust(double aDeltaT_sec,
                                                                 double aAlt_ft,
                                                                 double, // aDynPress_lbsqft,
                                                                 double, // aStatPress_lbssqft,
                                                                 double, // aSpeed_fps,
                                                                 double, // aMach,
                                                                 double, // aAlpha_rad,
                                                                 double, // aBeta_rad,
                                                                 double& aForceAndMoment,
                                                                 double& aFuelBurnRate_pps,
                                                                 double& aFuelBurned_lbs,
                                                                 bool    aUpdateData)
{
   // Reset flags (they may be set below)
   mEngineOperating     = false;
   mEngineSmoking       = false;
   mAfterburnerOn       = false;
   mContrailing         = false;
   mProducingSmokeTrail = false;

   // Ensure we never divide by 0
   if (aDeltaT_sec < utils::cEPSILON_SIMTIME_SEC)
   {
      // Avoid modifying state variables

      aForceAndMoment   = mCurrentThrust_lbs;
      aFuelBurnRate_pps = mCurrentFuelBurnRate_pph / 3600.0;
      aFuelBurned_lbs   = 0.0;
      return;
   }

   // We assume that rockets may leave a smoke trail but normally do not produce
   // engine smoke per se. Thus, the only way they will smoke is due to "damage",
   // represented by the mEngineDamageSmokeActivated flag.
   if (mEngineDamageSmokeActivated)
   {
      mEngineSmoking = true;
   }

   bool deadEngine = false;

   // Set the relative location of this thrust producer
   // aLocation_ft = mThrustLocation_ft;

   double aThrottleLeverPosition = mThrottleLeverPosition;
   double tempThrottleLever      = 0.0;
   double maxSpinUp              = mNormalizedSpinUp_per_sec * aDeltaT_sec;
   double maxSpinDown            = mNormalizedSpinDown_per_sec * aDeltaT_sec;

   double currentEffectiveThrottleLeverPosition = mLastThrottleLeverPosition;
   double deltaThrottle                         = aThrottleLeverPosition - currentEffectiveThrottleLeverPosition;

   if (deltaThrottle >= 0.0)
   {
      if (deltaThrottle > maxSpinUp)
      {
         deltaThrottle = maxSpinUp;
      }
   }
   else
   {
      if (deltaThrottle < -maxSpinDown)
      {
         deltaThrottle = -maxSpinDown;
      }
   }

   currentEffectiveThrottleLeverPosition += deltaThrottle;

   // Enforce throttle limits
   utils::LimitThrottleNormalized(currentEffectiveThrottleLeverPosition);

   // If no lag, the temp throttle is the current lever position (no lag)
   if (mParentThrustProducerPtr->GetParentVehicle()->GetFreezeFlags()->testingNoLag)
   {
      currentEffectiveThrottleLeverPosition = mThrottleLeverPosition;
   }

   tempThrottleLever = currentEffectiveThrottleLeverPosition;

   // If the engine is not burning, return appropriate data
   if (!mIsBurning)
   {
      // Since the engine is not burning, it is not producing thrust
      aForceAndMoment = 0.0;

      // We are not burning fuel
      aFuelBurnRate_pps = 0.0;
      aFuelBurned_lbs   = 0.0;

      if (aUpdateData)
      {
         // Set internal data
         mCurrentThrust_lbs       = aForceAndMoment;
         mCurrentFuelBurnRate_pph = 0.0;
      }

      // We can't do anything more ...
      return;
   }

   double effectiveBurnFraction = 1.0;
   if (mShutdownInProgress && mIsBurning)
   {
      effectiveBurnFraction = utils::TimeToTime(mShutdownFraction_nanosec) / aDeltaT_sec;

      if (effectiveBurnFraction < 0.0)
      {
         effectiveBurnFraction = 0.0;
      }
      if (effectiveBurnFraction > 1.0)
      {
         effectiveBurnFraction = 1.0;
      }

      // Clear the flag if updating
      if (aUpdateData)
      {
         mShutdownInProgress    = false;
         mIsBurning             = false;
         mThrottleLeverPosition = 0.0;
      }
   }

   // Determine thrust
   double thrust_lbs = 0.0;

   if (mIspAlt && mNormalizedThrustAlt)
   {
      double altFactor = mNormalizedThrustAlt->Lookup(aAlt_ft) / mNormalizedThrustAlt->Lookup(0.0);
      thrust_lbs       = (mMaxMassFlowSeaLevel * tempThrottleLever * mIspAlt->Lookup(aAlt_ft)) * altFactor;
   }

   thrust_lbs *= effectiveBurnFraction;

   // Determine fuel burn
   double fuelBurnRequest_lbs = mMaxMassFlowSeaLevel * tempThrottleLever * aDeltaT_sec;

   fuelBurnRequest_lbs *= effectiveBurnFraction;

   // Returned values
   double   fuelActuallyBurned_lbs = 0.0;
   double   newFuelMass_lbs        = 0.0;
   UtVec3dX newFuelCgLocation_ft(0.0, 0.0, 0.0);

   // Ensure that we still have a fuel flow path to the current tank
   if (mCurrentFuelTank != nullptr)
   {
      bool flowPathIntact = mCurrentFuelTank->FuelFlowPathIntact(mParentThrustProducerPtr->GetParentPropulsionSystem());
      if (!flowPathIntact)
      {
         // We do not have a fuel flow path to the current tank. This is often because an
         // external fuel tank has been jettisoned.

         // Since the tank is invalid, set pointer to null, since we no longer can get fuel
         mCurrentFuelTank = nullptr;
      }
   }

   // Verify mCurrentFuelTank is valid
   if (mCurrentFuelTank == nullptr)
   {
      deadEngine = true;
   }

   bool ableToBurnAllFuel = false;
   if (aUpdateData)
   {
      // We call update, rather than calculate
      ableToBurnAllFuel = mCurrentFuelTank->UpdateFuelBurn(aDeltaT_sec,
                                                           fuelBurnRequest_lbs,
                                                           fuelActuallyBurned_lbs,
                                                           newFuelMass_lbs,
                                                           newFuelCgLocation_ft);
   }
   else
   {
      // We call calculate, rather than update
      ableToBurnAllFuel = mCurrentFuelTank->CalculateFuelBurn(aDeltaT_sec,
                                                              fuelBurnRequest_lbs,
                                                              fuelActuallyBurned_lbs,
                                                              newFuelMass_lbs,
                                                              newFuelCgLocation_ft);
   }

   // If we cannot burn any fuel, the engine is dead
   if (!ableToBurnAllFuel)
   {
      if (fuelBurnRequest_lbs <= 0.0)
      {
         deadEngine = true;
      }
   }

   // Handle a dead engine
   if (deadEngine)
   {
      // We are not burning fuel
      aFuelBurnRate_pps = 0.0;
      aFuelBurned_lbs   = 0.0;

      // Set F&M data
      aForceAndMoment = 0.0;

      if (aUpdateData)
      {
         // Set internal data
         mCurrentThrust_lbs       = aForceAndMoment;
         mCurrentFuelBurnRate_pph = 0.0;
      }

      // We can't do anything more since the engine is dead
      return;
   }

   // At this point, we are burning and producing thrust

   if (!ableToBurnAllFuel)
   {
      // Adjust to have fractional thrust due to burnout
      thrust_lbs *= fuelActuallyBurned_lbs / fuelBurnRequest_lbs;
   }

   // Limit thrust to zero
   if (thrust_lbs < 0.0)
   {
      thrust_lbs = 0.0;
   }

   // Set the returned argument values for fuel burn
   aFuelBurnRate_pps = fuelActuallyBurned_lbs / aDeltaT_sec;
   aFuelBurned_lbs   = fuelActuallyBurned_lbs;

   // Set F&M data
   aForceAndMoment = thrust_lbs;

   // Set member attributes if we are "updating"
   if (aUpdateData)
   {
      mLastThrottleLeverPosition = currentEffectiveThrottleLeverPosition;

      // Limit last value, enforcing throttle limits
      utils::LimitThrottleNormalized(mLastThrottleLeverPosition);

      mCurrentThrust_lbs       = aForceAndMoment;
      mCurrentFuelBurnRate_pph = aFuelBurnRate_pps * 3600.0;

      double altFactor     = mNormalizedThrustAlt->Lookup(aAlt_ft) / mNormalizedThrustAlt->Lookup(0.0);
      mEngineThrustPercent = thrust_lbs / (mMaxThrustSeaLevel * altFactor);
   }

   if (mIsBurning)
   {
      mEngineOperating = true;
      if (mGenerateSmokeTrail)
      {
         mProducingSmokeTrail = true;
      }
   }

   // Standard contrailing effect within contrail altitude band
   Mover* vehicle = mParentThrustProducerPtr->GetParentVehicle();
   if (vehicle)
   {
      if (mEngineOperating && vehicle->WithinContrailAltitudeBand(aAlt_ft))
      {
         mContrailing = true;
      }
   }
}

double wsf::six_dof::RocketLiquidPropellantEngine::GetPercentMaxThrust() const
{
   return mEngineThrustPercent;
}

double wsf::six_dof::RocketLiquidPropellantEngine::GetFuelBurnRate_pph() const
{
   return mCurrentFuelBurnRate_pph;
}

double wsf::six_dof::RocketLiquidPropellantEngine::GetMaximumPotentialThrust_lbs(double aAlt_ft,
                                                                                 double, // aDynPress_lbsqft,
                                                                                 double, // aStatPress_lbssqft,
                                                                                 double, // aSpeed_fps,
                                                                                 double, // aMach,
                                                                                 double, // aAlpha_rad,
                                                                                 double) // aBeta_rad )
{
   // Determine thrust
   double thrust_lbs = 0.0;

   if (mIspAlt && mNormalizedThrustAlt)
   {
      double tempThrottleLever = 1.0; // full thrust
      double altFactor         = mNormalizedThrustAlt->Lookup(aAlt_ft) / mNormalizedThrustAlt->Lookup(0.0);
      thrust_lbs               = (mMaxMassFlowSeaLevel * tempThrottleLever * mIspAlt->Lookup(aAlt_ft)) * altFactor;
   }

   return thrust_lbs;
}

double wsf::six_dof::RocketLiquidPropellantEngine::GetMinimumPotentialThrust_lbs(double, // aAlt_ft,
                                                                                 double, // aDynPress_lbsqft,
                                                                                 double, // aStatPress_lbssqft,
                                                                                 double, // aSpeed_fps,
                                                                                 double, // aMach,
                                                                                 double, // aAlpha_rad,
                                                                                 double) // aBeta_rad )
{
   // The min thrust is when the engine is turned off
   return 0.0;
}

void wsf::six_dof::RocketLiquidPropellantEngine::SetThrottlePosition(double aThrottleLeverPosition)
{
   mThrottleLeverPosition = aThrottleLeverPosition;

   // Enforce throttle limits
   utils::LimitThrottleNormalized(mThrottleLeverPosition);

   // Enforce engine-specific min/max throttle limits
   if (mThrottleLeverPosition < mMinThrottle)
   {
      mThrottleLeverPosition = mMinThrottle;
   }
   if (mThrottleLeverPosition > mMaxThrottle)
   {
      mThrottleLeverPosition = mMaxThrottle;
   }
}

double wsf::six_dof::RocketLiquidPropellantEngine::GetThrottlePosition() const
{
   return mThrottleLeverPosition;
}

void wsf::six_dof::RocketLiquidPropellantEngine::Ignite(int64_t aIgniteTimeInFrame_nanosec)
{
   mIsBurning                 = true;
   mIgniteTimeInFrame_nanosec = aIgniteTimeInFrame_nanosec;
   mThrottleLeverPosition     = 1.0;
}

void wsf::six_dof::RocketLiquidPropellantEngine::Shutdown(int64_t aTerminateTime_nanosec /*= 0*/)
{
   mThrottleLeverPosition = 0.0;

   if (mIsBurning) // Must be burning to terminate
   {
      // Note: The mIsBurning flag will be set to false when the thrust
      // is terminated in the CalculateThrust() function
      mShutdownInProgress       = true;
      mShutdownFraction_nanosec = aTerminateTime_nanosec;
   }
}
