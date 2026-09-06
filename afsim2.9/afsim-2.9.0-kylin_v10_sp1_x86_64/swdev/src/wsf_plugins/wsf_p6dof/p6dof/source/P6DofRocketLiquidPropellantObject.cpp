// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "P6DofRocketLiquidPropellantObject.hpp"

#include <fstream>

#include "P6DofFreezeFlags.hpp"
#include "P6DofFuelTank.hpp"
#include "P6DofPropulsionSystem.hpp"
#include "P6DofScenario.hpp"
#include "P6DofTypeManager.hpp"
#include "P6DofUtils.hpp"
#include "P6DofVehicle.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtInputFile.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"

P6DofRocketLiquidPropellantObject::P6DofRocketLiquidPropellantObject(P6DofScenario* aScenario)
   : P6DofThrustProducerObject(aScenario)
   , mThrottleLeverPosition(0)
   , mLastThrottleLeverPosition(0)
   , mCurrentFuelTank(nullptr)
   , mNormalizedThrustAlt(nullptr)
   , mIspAlt(nullptr)
   , mThrottleVersusTime(nullptr)
   , mMaxThrustSeaLevel(0)
   , mMaxThrottle(1)
   , mMinThrottle(0)
   , mMaxMassFlowSeaLevel(0)
   , mMaxMassFlowVacuum(0)
   , mEngineThrustPercent(0)
   , mIsBurning(false)
   , mBurnTime_Sec(0)
   , mNormalizedSpinUp_per_sec(0)
   , mNormalizedSpinDown_per_sec(0)
   , mCurrentFuelBurnRate_pph(0)
   , mThrottleSettingYaw(nullptr)
   , mThrottleSettingPitch(nullptr)
   , mGenerateSmokeTrail(false)
{
   mTypeOfThrustProducer = "LiquidPropellantRocket";
}

P6DofRocketLiquidPropellantObject::~P6DofRocketLiquidPropellantObject()
{
   if (mThrottleSettingYaw)
   {
      delete mThrottleSettingYaw;
   }
   if (mThrottleSettingPitch)
   {
      delete mThrottleSettingPitch;
   }
}

P6DofRocketLiquidPropellantObject::P6DofRocketLiquidPropellantObject(const P6DofRocketLiquidPropellantObject& aSrc)
   : P6DofThrustProducerObject(aSrc)
   , mThrottleLeverPosition(aSrc.mThrottleLeverPosition)
   , mLastThrottleLeverPosition(aSrc.mLastThrottleLeverPosition)
   , mCurrentFuelTank(aSrc.mCurrentFuelTank)
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
   , mCurrentFuelBurnRate_pph(0)
   , mThrottleSettingYaw(nullptr)
   , mThrottleSettingPitch(nullptr)
   , mGenerateSmokeTrail(aSrc.mGenerateSmokeTrail)
{
   if (aSrc.mThrottleSettingYaw != nullptr)
   {
      mThrottleSettingYaw = new ThrustControlInputValueData(*aSrc.mThrottleSettingYaw);
   }
   if (aSrc.mThrottleSettingPitch != nullptr)
   {
      mThrottleSettingPitch = new ThrustControlInputValueData(*aSrc.mThrottleSettingPitch);
   }
}

P6DofRocketLiquidPropellantObject* P6DofRocketLiquidPropellantObject::Clone() const
{
   return new P6DofRocketLiquidPropellantObject(*this);
}

void P6DofRocketLiquidPropellantObject::DeriveFrom(const P6DofRocketLiquidPropellantObject* aSrc)
{
   P6DofThrustProducerObject::DeriveFrom(aSrc);

   if (aSrc == nullptr)
   {
      return;
   }

   mNormalizedThrustAlt = aSrc->mNormalizedThrustAlt;
   mIspAlt              = aSrc->mIspAlt;
   mThrottleVersusTime  = aSrc->mThrottleVersusTime;

   mCurrentFuelTank            = aSrc->mCurrentFuelTank;
   mThrottleLeverPosition      = aSrc->mThrottleLeverPosition;
   mLastThrottleLeverPosition  = aSrc->mLastThrottleLeverPosition;
   mMaxThrustSeaLevel          = aSrc->mMaxThrustSeaLevel;
   mMaxThrottle                = aSrc->mMaxThrottle;
   mMinThrottle                = aSrc->mMinThrottle;
   mMaxMassFlowSeaLevel        = aSrc->mMaxMassFlowSeaLevel;
   mMaxMassFlowVacuum          = aSrc->mMaxMassFlowVacuum;
   mEngineThrustPercent        = aSrc->mEngineThrustPercent;
   mIsBurning                  = aSrc->mIsBurning;
   mBurnTime_Sec               = aSrc->mBurnTime_Sec;
   mNormalizedSpinUp_per_sec   = aSrc->mNormalizedSpinUp_per_sec;
   mNormalizedSpinDown_per_sec = aSrc->mNormalizedSpinDown_per_sec;
   mCurrentFuelBurnRate_pph    = 0.0;

   if (mThrottleSettingYaw != nullptr)
   {
      delete mThrottleSettingYaw;
   }
   if (mThrottleSettingPitch != nullptr)
   {
      delete mThrottleSettingPitch;
   }
   if (aSrc->mThrottleSettingYaw != nullptr)
   {
      mThrottleSettingYaw = new ThrustControlInputValueData(*aSrc->mThrottleSettingYaw);
   }
   if (aSrc->mThrottleSettingPitch != nullptr)
   {
      mThrottleSettingPitch = new ThrustControlInputValueData(*aSrc->mThrottleSettingPitch);
   }

   mGenerateSmokeTrail = aSrc->mGenerateSmokeTrail;
}

void P6DofRocketLiquidPropellantObject::SetControlInputHandles(const P6DofFlightControlSystem* aFlightControls)
{
   SetControlInputHandle(aFlightControls, mThrottleSettingYaw);
   SetControlInputHandle(aFlightControls, mThrottleSettingPitch);
}

void P6DofRocketLiquidPropellantObject::SetControlInputValues(const P6DofFlightControlSystem* aFlightControls)
{
   SetControlInputValue(aFlightControls, mThrottleSettingYaw);
   SetControlInputValue(aFlightControls, mThrottleSettingPitch);
}

bool P6DofRocketLiquidPropellantObject::ProcessInput(UtInput& aInput)
{
   bool               myCommand = true;
   const std::string& command   = aInput.GetCommand();

   if (command == "liquid_propellant_rocket_type")
   {
      std::string typeName;
      std::string baseType;
      aInput.ReadValue(typeName);
      aInput.ReadValue(baseType);
      SetName(typeName); // Types should have the typeName be their name

      bool derivedFromBase = true;
      if (baseType != "P6DOF_LIQUID_PROPELLANT_ROCKET")
      {
         derivedFromBase = false;
         // This is not derived from the base type
         // Determine if this base type exists
         if (!GetScenario()->GetTypeManager()->ThrustProducerObjectTypeExists(baseType))
         {
            // Unknown type -- error since we can't derive from an unknown type
            auto out = ut::log::error() << "P6DofRocketLiquidPropellantObject is trying to derive from an unknown type "
                                           "in P6DofRocketLiquidPropellantObject::ProcessInput.";
            out.AddNote() << "Base Type: " << baseType;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
      }

      // We have a known type from which to derive
      // Determine if the name is unique
      if (GetScenario()->GetTypeManager()->ThrustProducerObjectTypeExists(typeName))
      {
         // Non-unique type name -- error since we can't have duplicate names
         auto out = ut::log::error() << "P6DofRocketLiquidPropellantObject is using a duplicate name in "
                                        "P6DofRocketLiquidPropellantObject::ProcessInput.";
         out.AddNote() << "Name: " << typeName;
         out.AddNote() << "Location: " << aInput.GetLocation();
         throw UtInput::UnknownCommand(aInput);
      }

      // If derived, clone the data
      if (!derivedFromBase)
      {
         DeriveFrom(dynamic_cast<P6DofRocketLiquidPropellantObject*>(
            GetScenario()->GetTypeManager()->GetThrustProducerObjectType(baseType)));
      }

      // Register the object with the manager
      if (!GetScenario()->GetTypeManager()->RegisterThrustProducerObjectType(this))
      {
         auto out = ut::log::error() << "P6DofRocketLiquidPropellantObject unable to register in "
                                        "P6DofRocketLiquidPropellantObject::ProcessInput.";
         out.AddNote() << "Name: " << typeName;
         throw UtInput::UnknownCommand(aInput);
      }

      double maxThrustVacuum = -1;

      UtInputBlock block(aInput, "end_liquid_propellant_rocket_type");
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
         else if (localCommand == "throttle_setting_yaw")
         {
            std::string inputName;
            aInput.ReadValue(inputName);

            if (mThrottleSettingYaw != nullptr)
            {
               delete mThrottleSettingYaw;
            }

            mThrottleSettingYaw         = new ThrustControlInputValueData;
            mThrottleSettingYaw->handle = 0;
            mThrottleSettingYaw->name   = inputName;
            mThrottleSettingYaw->value  = 0;

            // Since this is capable of thrust vectoring (TV), enable TV by default
            mThrustVectoringEnabled = true;
         }
         else if (localCommand == "throttle_setting_pitch")
         {
            std::string inputName;
            aInput.ReadValue(inputName);

            if (mThrottleSettingPitch != nullptr)
            {
               delete mThrottleSettingPitch;
            }

            mThrottleSettingPitch         = new ThrustControlInputValueData;
            mThrottleSettingPitch->handle = 0;
            mThrottleSettingPitch->name   = inputName;
            mThrottleSettingPitch->value  = 0;

            // Since this is capable of thrust vectoring (TV), enable TV by default
            mThrustVectoringEnabled = true;
         }
         else if (localCommand == "thrust_offset")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cLENGTH);
            value *= UtMath::cFT_PER_M;
            mThrustOffset_ft = value;
         }
         else if (localCommand == "inop_ref_area")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cAREA);
            value *= (UtMath::cFT_PER_M * UtMath::cFT_PER_M);
            mInOpRefArea_ft2  = value;
            mInOpRefAreaValid = true;
         }
         else if (localCommand == "creates_smoke_trail")
         {
            bool value;
            aInput.ReadValue(value);
            mGenerateSmokeTrail = value;
         }
         else
         {
            auto out =
               ut::log::error() << "Unrecognized command within P6DofRocketLiquidPropellantObject::ProcessInput().";
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

bool P6DofRocketLiquidPropellantObject::Initialize(int64_t) //(int64_t aSimTime_nanosec)
{
   // Calculate data
   mMaxMassFlowSeaLevel = mMaxThrustSeaLevel / mIspAlt->Lookup(0.0);

   return true;
}

void P6DofRocketLiquidPropellantObject::CalculateThrust(double                      aDeltaT_sec,
                                                        double                      aAlt_ft,
                                                        double                      aDynPress_lbsqft,
                                                        double                      aStatPress_lbssqft,
                                                        double                      aSpeed_fps,
                                                        double                      aMach,
                                                        double                      aAlpha_rad,
                                                        double                      aBeta_rad,
                                                        P6DofForceAndMomentsObject& aForceAndMoment,
                                                        double&                     aFuelBurnRate_pps,
                                                        double&                     aFuelBurned_lbs)
{
   CalculateThrust(aDeltaT_sec,
                   aAlt_ft,
                   aDynPress_lbsqft,
                   aStatPress_lbssqft,
                   aSpeed_fps,
                   aMach,
                   aAlpha_rad,
                   aBeta_rad,
                   aForceAndMoment,
                   aFuelBurnRate_pps,
                   aFuelBurned_lbs,
                   false);
}

void P6DofRocketLiquidPropellantObject::UpdateThrust(double                      aDeltaT_sec,
                                                     double                      aAlt_ft,
                                                     double                      aDynPress_lbsqft,
                                                     double                      aStatPress_lbssqft,
                                                     double                      aSpeed_fps,
                                                     double                      aMach,
                                                     double                      aAlpha_rad,
                                                     double                      aBeta_rad,
                                                     P6DofForceAndMomentsObject& aForceAndMoment,
                                                     double&                     aFuelBurnRate_pps,
                                                     double&                     aFuelBurned_lbs)
{
   CalculateThrust(aDeltaT_sec,
                   aAlt_ft,
                   aDynPress_lbsqft,
                   aStatPress_lbssqft,
                   aSpeed_fps,
                   aMach,
                   aAlpha_rad,
                   aBeta_rad,
                   aForceAndMoment,
                   aFuelBurnRate_pps,
                   aFuelBurned_lbs,
                   true);

   mCurrentFuelBurnRate_pph = aFuelBurnRate_pps * 3600.0;
}

void P6DofRocketLiquidPropellantObject::CalculateThrust(double aDeltaT_sec,
                                                        double aAlt_ft,
                                                        double, // aDynPress_lbsqft,
                                                        double, // aStatPress_lbssqft,
                                                        double, // aSpeed_fps,
                                                        double, // aMach,
                                                        double, // aAlpha_rad,
                                                        double, // aBeta_rad,
                                                        P6DofForceAndMomentsObject& aForceAndMoment,
                                                        double&                     aFuelBurnRate_pps,
                                                        double&                     aFuelBurned_lbs,
                                                        bool                        aUpdateData)
{
   // Reset flags (they may be set below)
   mEngineOperating     = false;
   mEngineSmoking       = false;
   mAfterburnerOn       = false;
   mContrailing         = false;
   mProducingSmokeTrail = false;

   // Ensure we never divide by 0
   if (aDeltaT_sec < P6DofUtils::cEPSILON_SIMTIME_SEC)
   {
      // Avoid modifying state variables

      aForceAndMoment   = mCurrentThrustFM_lbs_ftlbs;
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
   P6DofUtils::LimitThrottleNormalized(currentEffectiveThrottleLeverPosition);

   // If no lag, the temp throttle is the current lever position (no lag)
   if (GetScenario()->GetFreezeFlags()->GetMasterNoLagTesting())
   {
      currentEffectiveThrottleLeverPosition = mThrottleLeverPosition;
   }

   tempThrottleLever = currentEffectiveThrottleLeverPosition;

   // If the engine is not burning, return appropriate data
   if (!mIsBurning)
   {
      UtVec3dX force_lbs(0., 0., 0.);
      UtVec3dX moment_ftlbs(0., 0., 0.);

      // Since the engine is not burning, it is not producing thrust
      force_lbs.Set(0, 0, 0);
      moment_ftlbs.Set(0, 0, 0);

      // We are not burning fuel
      aFuelBurnRate_pps = 0.0;
      aFuelBurned_lbs   = 0.0;

      // Set the relative location
      // aLocation_ft = mThrustLocation_ft;

      // Set F&M data
      aForceAndMoment.MoveRefPoint_ft(mThrustLocation_ft);
      aForceAndMoment.ClearForcesAndMoments();
      aForceAndMoment.AddForceAndMomentAtReferencePoint(force_lbs, moment_ftlbs);

      if (aUpdateData)
      {
         // Set internal data
         mCurrentThrustFM_lbs_ftlbs = aForceAndMoment;
         mCurrentFuelBurnRate_pph   = 0.0;
      }

      // We can't do anything more ...
      return;
   }

   double effectiveBurnFraction = 1.0;
   if (mShutdownInProgress && mIsBurning)
   {
      effectiveBurnFraction = P6DofUtils::TimeToTime(mShutdownFraction_nanosec) / aDeltaT_sec;

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
      bool flowPathIntact = mCurrentFuelTank->FuelFlowPathIntact(mParentPropulsionSystemPtr);
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
      UtVec3dX force_lbs(0., 0., 0.);
      UtVec3dX moment_ftlbs(0., 0., 0.);

      // The engine is fuel-starved and is not producing thrust
      force_lbs.Set(0, 0, 0);
      moment_ftlbs.Set(0, 0, 0);

      // We are not burning fuel
      aFuelBurnRate_pps = 0.0;
      aFuelBurned_lbs   = 0.0;

      // Set the relative location
      // aLocation_ft = mThrustLocation_ft;

      // Set F&M data
      aForceAndMoment.MoveRefPoint_ft(mThrustLocation_ft);
      aForceAndMoment.ClearForcesAndMoments();
      aForceAndMoment.AddForceAndMomentAtReferencePoint(force_lbs, moment_ftlbs);

      if (aUpdateData)
      {
         // Set internal data
         mCurrentThrustFM_lbs_ftlbs = aForceAndMoment;
         mCurrentFuelBurnRate_pph   = 0.0;
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

   P6DofForceAndMomentsObject thrustFM;
   CalcThrustProducerFM(thrust_lbs, thrustFM);

   // Set the returned argument values for fuel burn
   aFuelBurnRate_pps = fuelActuallyBurned_lbs / aDeltaT_sec;
   aFuelBurned_lbs   = fuelActuallyBurned_lbs;

   // Set F&M data
   aForceAndMoment = thrustFM;

   // Set member attributes if we are "updating"
   if (aUpdateData)
   {
      mLastThrottleLeverPosition = currentEffectiveThrottleLeverPosition;

      // Limit last value, enforcing throttle limits
      P6DofUtils::LimitThrottleNormalized(mLastThrottleLeverPosition);

      mCurrentThrustFM_lbs_ftlbs = aForceAndMoment;
      mCurrentFuelBurnRate_pph   = aFuelBurnRate_pps * 3600.0;

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
   P6DofVehicle* vehicle = GetParentVehicle();
   if (vehicle)
   {
      if (mEngineOperating && vehicle->WithinContrailAltitudeBand(aAlt_ft))
      {
         mContrailing = true;
      }
   }
}

void P6DofRocketLiquidPropellantObject::SetFuelTank(const std::string& aFuelTankName)
{
   mCurrentFuelTank = mParentPropulsionSystemPtr->GetParentVehicle()->GetFuelTankByName(aFuelTankName);

   if (mCurrentFuelTank != nullptr)
   {
      bool flowPathIntact = mCurrentFuelTank->FuelFlowPathIntact(mParentPropulsionSystemPtr);
      if (!flowPathIntact)
      {
         auto out =
            ut::log::error()
            << "Cannot achieve a fuel flow path to fuel tank in P6DofRocketLiquidPropellantObject::SetFuelTank().";
         out.AddNote() << "Fuel Tank: " << aFuelTankName;
      }
   }

   // Provide warning if no tank is present
   if (mCurrentFuelTank == nullptr)
   {
      auto out = ut::log::error() << "Unknown fuel tank selected in P6DofRocketLiquidPropellantObject::SetFuelTank().";
      out.AddNote() << "Fuel Tank: " << aFuelTankName;
   }
}

P6DofFuelTank* P6DofRocketLiquidPropellantObject::GetFuelTank() const
{
   return mCurrentFuelTank;
}

double P6DofRocketLiquidPropellantObject::GetPercentMaxThrust() const
{
   return mEngineThrustPercent;
}

double P6DofRocketLiquidPropellantObject::GetFuelBurnRate_pph() const
{
   return mCurrentFuelBurnRate_pph;
}

double P6DofRocketLiquidPropellantObject::GetMaximumPotentialThrust_lbs(double aAlt_ft,
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

double P6DofRocketLiquidPropellantObject::GetMinimumPotentialThrust_lbs(double, // aAlt_ft,
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

void P6DofRocketLiquidPropellantObject::SetThrottlePosition(double aThrottleLeverPosition)
{
   mThrottleLeverPosition = aThrottleLeverPosition;

   // Enforce throttle limits
   P6DofUtils::LimitThrottleNormalized(mThrottleLeverPosition);

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

double P6DofRocketLiquidPropellantObject::GetThrottlePosition() const
{
   return mThrottleLeverPosition;
}

void P6DofRocketLiquidPropellantObject::Ignite(int64_t aIgniteTimeInFrame_nanosec)
{
   mIsBurning                 = true;
   mIgniteTimeInFrame_nanosec = aIgniteTimeInFrame_nanosec;
   mThrottleLeverPosition     = 1.0;
}

void P6DofRocketLiquidPropellantObject::Shutdown(int64_t aTerminateTime_nanosec /*= 0*/)
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
