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

#include "WsfSixDOF_RamjetEngine.hpp"

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

wsf::six_dof::RamjetEngine::RamjetEngine(ThrustProducerObject* aParentObject)
   : Engine(aParentObject)
{
   mTypeOfEngine = "Ramjet";
}

wsf::six_dof::RamjetEngine::RamjetEngine(const RamjetEngine& aSrc)
   : Engine(aSrc)
   , mThrustAltMachTable_lbs(aSrc.mThrustAltMachTable_lbs)
   , mTsfcAltMachTable_pph(aSrc.mTsfcAltMachTable_pph)
   , mCurrentFuelBurnRate_pph(aSrc.mCurrentFuelBurnRate_pph)
   , mAfterburnerAppearanceWhenOperating(aSrc.mAfterburnerAppearanceWhenOperating)
   , mLatchFuelInjection(aSrc.mLatchFuelInjection)
   , mInjectFuelTriggered(aSrc.mInjectFuelTriggered)
   , mUseProportionalThrottle(aSrc.mUseProportionalThrottle)
   , mMinProportionalThrust(aSrc.mMinProportionalThrust)
   , mThrottleLeverPosition(aSrc.mThrottleLeverPosition)
   , mOverallThrustMultiplier(aSrc.mOverallThrustMultiplier)
{
}

void wsf::six_dof::RamjetEngine::DeriveFrom(const Engine* aSrc)
{
   auto ramjetSrc = dynamic_cast<const RamjetEngine*>(aSrc);

   if (ramjetSrc == nullptr)
   {
      return;
   }

   Engine::DeriveFrom(ramjetSrc);

   if (ramjetSrc->mThrustAltMachTable_lbs)
   {
      mThrustAltMachTable_lbs = ramjetSrc->mThrustAltMachTable_lbs;
   }

   if (ramjetSrc->mTsfcAltMachTable_pph)
   {
      mTsfcAltMachTable_pph = ramjetSrc->mTsfcAltMachTable_pph;
   }

   mCurrentFuelBurnRate_pph = 0.0;

   mAfterburnerAppearanceWhenOperating = ramjetSrc->mAfterburnerAppearanceWhenOperating;

   mLatchFuelInjection      = ramjetSrc->mLatchFuelInjection;
   mInjectFuelTriggered     = ramjetSrc->mInjectFuelTriggered;
   mUseProportionalThrottle = ramjetSrc->mUseProportionalThrottle;
   mMinProportionalThrust   = ramjetSrc->mMinProportionalThrust;
   mThrottleLeverPosition   = ramjetSrc->mThrottleLeverPosition;
   mOverallThrustMultiplier = ramjetSrc->mOverallThrustMultiplier;
}

wsf::six_dof::Engine* wsf::six_dof::RamjetEngine::Clone() const
{
   return new RamjetEngine(*this);
}

bool wsf::six_dof::RamjetEngine::ProcessInput(UtInput& aInput, TypeManager* aTypeManager)
{
   bool               myCommand = true;
   const std::string& command   = aInput.GetCommand();

   if (command == "ramjet")
   {
      UtInputBlock block(aInput);
      while (block.ReadCommand())
      {
         std::string localCommand;
         localCommand = block.GetCommand();

         if (localCommand == "thrust_alt_mach_table")
         {
            UtInputBlock subBlock(aInput, "end_thrust_alt_mach_table");
            do
            {
               if (mThrustAltMachTable_lbs)
               {
                  mThrustAltMachTable_lbs = nullptr;
               }
               UtTable::Parameters sParameters;
               sParameters.AddRealParameter("alt", UtInput::cLENGTH, UtTable::NoCheck(), 0);
               sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 1);
               mThrustAltMachTable_lbs =
                  UtTable::LoadInstance(aInput, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
            } while (subBlock.ReadCommand());
         }
         else if (localCommand == "tsfc_alt_mach_table")
         {
            UtInputBlock subBlock(aInput, "end_tsfc_alt_mach_table");
            do
            {
               if (mTsfcAltMachTable_pph)
               {
                  mTsfcAltMachTable_pph = nullptr;
               }
               UtTable::Parameters sParameters;
               sParameters.AddRealParameter("alt", UtInput::cLENGTH, UtTable::NoCheck(), 0);
               sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 1);
               mTsfcAltMachTable_pph =
                  UtTable::LoadInstance(aInput, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
            } while (subBlock.ReadCommand());
         }
         else if (localCommand == "afterburner_appearance_when_operating")
         {
            aInput.ReadValue(mAfterburnerAppearanceWhenOperating);
         }
         else if (localCommand == "latch_fuel_injection")
         {
            aInput.ReadValue(mLatchFuelInjection);
         }
         else if (localCommand == "use_proportional_throttle")
         {
            aInput.ReadValue(mUseProportionalThrottle);
         }
         else if (localCommand == "minimum_proportional_thrust")
         {
            aInput.ReadValue(mMinProportionalThrust);
            aInput.ValueGreaterOrEqual(mMinProportionalThrust, 0.0);
         }
         else
         {
            auto out = ut::log::error() << "Unrecognized command within RamjetEngine::ProcessInput().";
            out.AddNote() << "Command: " << localCommand;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
      }

      // Confirm that we have the necessary parameters
      if (!mThrustAltMachTable_lbs || !mTsfcAltMachTable_pph)
      {
         if (!mThrustAltMachTable_lbs)
         {
            auto out = ut::log::error() << "'thrust_alt_mach_table' not specified in RamjetEngine::ProcessInput().";
            out.AddNote() << "Location: " << aInput.GetLocation();

            throw UtInput::UnknownCommand(aInput);
         }
         if (!mTsfcAltMachTable_pph)
         {
            auto out = ut::log::error() << "'tsfc_alt_mach_table' not specified in RamjetEngine::ProcessInput().";
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
         return false;
      }
   }

   return myCommand;
}

bool wsf::six_dof::RamjetEngine::Initialize(int64_t) //(int64_t aSimTime_nanosec)
{
   return true;
}

void wsf::six_dof::RamjetEngine::CalculateThrust(double aDeltaT_sec,
                                                 double aAlt_ft,
                                                 double aDynPress_lbsqft,
                                                 double, // aStatPress_lbssqft,
                                                 double, // aSpeed_fps,
                                                 double aMach,
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
   mProducingSmokeTrail = false; // Ramjets do not produce smoke trails

   // Ensure we never divide by 0
   if (aDeltaT_sec < utils::cEPSILON_SIMTIME_SEC)
   {
      // Avoid modifying state variables

      aForceAndMoment   = mCurrentThrust_lbs;
      aFuelBurnRate_pps = mCurrentFuelBurnRate_pph / 3600.0;
      aFuelBurned_lbs   = 0.0;
      return;
   }

   // Ramjets tend to burn clean, so the only way they will smoke is due to "damage",
   // represented by the mEngineDamageSmokeActivated flag.
   if (mEngineDamageSmokeActivated)
   {
      mEngineSmoking = true;
   }

   // We use this flag to denote a dead engine (not "burning" or producing thrust)
   bool deadEngine = false;

   // Get *potential* thrust as a function of mach and altitude
   double thrust_lbs = 0.0;
   double lookupArgs[2];
   lookupArgs[0] = aAlt_ft * UtMath::cM_PER_FT;
   lookupArgs[1] = aMach;

   if (mThrustAltMachTable_lbs)
   {
      thrust_lbs = mThrustAltMachTable_lbs->Lookup(lookupArgs);
   }
   else
   {
      // We lack a thrust table, so no thrust and the engine is dead
      deadEngine = true;
   }

   // Get TSFC using mach/alt lookupArgs from above
   double effectiveTSFC_Mil_pps = 0;
   if (mTsfcAltMachTable_pph)
   {
      // Get TSFC in pps (lbs/sec) instead of pph (lbs/hr)
      effectiveTSFC_Mil_pps = mTsfcAltMachTable_pph->Lookup(lookupArgs) / 3600.0;
   }
   else
   {
      // We lack a TSFC table, so no thrust and the engine is dead
      deadEngine = true;
   }

   // Determine fuel burned during this timestep
   double fuelBurn_lbs = effectiveTSFC_Mil_pps * thrust_lbs * aDeltaT_sec;

   // If we have no thrust and/or are not burning fuel,
   // we have a "dead" engine (not producing thrust)
   if ((fuelBurn_lbs <= 0) || (thrust_lbs <= 0))
   {
      deadEngine = true;
   }

   double   fuelBurnRequest_lbs    = fuelBurn_lbs;
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

   // If we lack a fuel tank, the engine is dead
   if (mCurrentFuelTank == nullptr)
   {
      deadEngine = true;
   }

   // Calculate the thrust multiplier, adjust fuel burn request and thrust, and handle fuel injection effects
   deadEngine = CalcThrustMultiplier(deadEngine, fuelBurnRequest_lbs, thrust_lbs);

   // Determine if we have enough fuel to burn for the entire time-step
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

   // If we did not actually burn *any* fuel, the engine is dead
   if (fuelActuallyBurned_lbs <= 0.0)
   {
      deadEngine = true;
   }

   double deadEngineDrag_lbs = 0;
   if (deadEngine || !ableToBurnAllFuel)
   {
      // The engine is fuel-starved and is not producing thrust. We therefore need to
      // calculate the drag on the dead engine.
      deadEngineDrag_lbs = mParentThrustProducerPtr->GetInoperatingDragArea_ft2() * aDynPress_lbsqft;
   }

   // Now, we check for a dead engine, an inability to burn the requested fuel, or a fully
   // operational engine, and handle each case accordingly
   double effectiveThrust_lbs = 0;
   if (deadEngine)
   {
      effectiveThrust_lbs = -deadEngineDrag_lbs;

      // TODO - mEngineSmoking and mContrailing are not yet supported

      // We are not burning fuel
      aFuelBurnRate_pps = 0.0;
      aFuelBurned_lbs   = 0.0;
   }
   else if (!ableToBurnAllFuel)
   {
      // Determine a ratio for burn time to flame-out time
      double burnRatio = fuelActuallyBurned_lbs / fuelBurnRequest_lbs;

      // Calculate the effective thrust
      effectiveThrust_lbs = (thrust_lbs * burnRatio) - deadEngineDrag_lbs * (1 - burnRatio);

      // TODO - mEngineSmoking and mContrailing are not yet supported

      // Set the returned argument values for fuel burn
      aFuelBurnRate_pps = fuelActuallyBurned_lbs / aDeltaT_sec;
      aFuelBurned_lbs   = fuelActuallyBurned_lbs;
   }
   else // everything is fine
   {
      effectiveThrust_lbs = thrust_lbs;

      // TODO - mEngineSmoking and mContrailing are not yet supported

      // Set the returned argument values for fuel burn
      aFuelBurnRate_pps = fuelActuallyBurned_lbs / aDeltaT_sec;
      aFuelBurned_lbs   = fuelActuallyBurned_lbs;
   }

   // Set F&M data
   aForceAndMoment = effectiveThrust_lbs;

   // Set member attributes if we are "updating"
   if (aUpdateData)
   {
      mCurrentThrust_lbs       = aForceAndMoment;
      mCurrentFuelBurnRate_pph = aFuelBurnRate_pps * 3600.0;
   }

   // Set operating appearance, if not dead
   if (!deadEngine)
   {
      mEngineOperating = true;
      if (mAfterburnerAppearanceWhenOperating)
      {
         mAfterburnerOn = true;
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

double wsf::six_dof::RamjetEngine::GetFuelBurnRate_pph() const
{
   return mCurrentFuelBurnRate_pph;
}

double wsf::six_dof::RamjetEngine::GetMaximumPotentialThrust_lbs(double aAlt_ft,
                                                                 double aDynPress_lbsqft,
                                                                 double aStatPress_lbssqft,
                                                                 double aSpeed_fps,
                                                                 double aMach,
                                                                 double aAlpha_rad,
                                                                 double aBeta_rad)
{
   double thrustForce_lbs  = 0;
   double fuelBurnRate_pps = 0;
   double fuelBurned_lbs   = 0;

   Engine::CalculateThrust(0.01,
                           aAlt_ft,
                           aDynPress_lbsqft,
                           aStatPress_lbssqft,
                           aSpeed_fps,
                           aMach,
                           aAlpha_rad,
                           aBeta_rad,
                           thrustForce_lbs,
                           fuelBurnRate_pps,
                           fuelBurned_lbs);

   return thrustForce_lbs;
}

double wsf::six_dof::RamjetEngine::GetMinimumPotentialThrust_lbs(double aAlt_ft,
                                                                 double aDynPress_lbsqft,
                                                                 double aStatPress_lbssqft,
                                                                 double aSpeed_fps,
                                                                 double aMach,
                                                                 double aAlpha_rad,
                                                                 double aBeta_rad)
{
   double thrustForce_lbs  = 0;
   double fuelBurnRate_pps = 0;
   double fuelBurned_lbs   = 0;

   Engine::CalculateThrust(0.01,
                           aAlt_ft,
                           aDynPress_lbsqft,
                           aStatPress_lbssqft,
                           aSpeed_fps,
                           aMach,
                           aAlpha_rad,
                           aBeta_rad,
                           thrustForce_lbs,
                           fuelBurnRate_pps,
                           fuelBurned_lbs);

   return thrustForce_lbs;
}

void wsf::six_dof::RamjetEngine::SetThrottlePosition(double aThrottleLeverPosition)
{
   mThrottleLeverPosition = aThrottleLeverPosition;

   // Enforce throttle limits
   utils::LimitThrottleNormalized(mThrottleLeverPosition);
}

double wsf::six_dof::RamjetEngine::GetThrottlePosition() const
{
   return mThrottleLeverPosition;
}

void wsf::six_dof::RamjetEngine::Ignite(int64_t /*aIgniteTimeInFrame_nanosec*/)
{
   // Set throttle to full and inject fuel
   mThrottleLeverPosition = 1.0;
   mInjectFuel            = true;
}

void wsf::six_dof::RamjetEngine::Shutdown(int64_t /*aTerminateTime_nanosec */)
{
   // Cut the throttle and stop injecting fuel
   mThrottleLeverPosition = 0.0;
   mInjectFuel            = false;
}

bool wsf::six_dof::RamjetEngine::CalcThrustMultiplier(bool aDeadEngine, double& aFuelBurnRequest_lbs, double& aThrust_lbs)
{
   // Set the local dead flag using the incoming argument. This may be true
   // if required thrust tables are missing or a fuel tank is missing.
   bool deadEngine = aDeadEngine;

   if (mUseProportionalThrottle)
   {
      // When using the proportional throttle, it requires that fuel injection is enabled,
      // or no thrust will be produced. This is similar to the jet engine model. The
      // difference, however, is that jet engines have fuel injection set to on by default,
      // while ramjets have it set to off by default.
      if (mInjectFuel)
      {
         double deltaThrust = 1.0 - mMinProportionalThrust;

         mOverallThrustMultiplier = mMinProportionalThrust + deltaThrust * mThrottleLeverPosition;

         // Limit min throttle multiplier
         if (mOverallThrustMultiplier < mMinProportionalThrust)
         {
            mOverallThrustMultiplier = mMinProportionalThrust;
         }

         // Limit max throttle multiplier
         utils::LimitThrottleNormalized(mOverallThrustMultiplier);
      }
      else
      {
         // If we are not injecting fuel, there can be no thrust and the engine is dead
         mOverallThrustMultiplier = 0.0;
         deadEngine               = true;
      }
   }
   else
   {
      // Default throttle operation -- either 0 or 1

      if (mThrottleLeverPosition < 0.5)
      {
         if (!mLatchFuelInjection)
         {
            // Low-side throttle -- set to zero
            mInjectFuel              = false;
            mOverallThrustMultiplier = 0.0;
         }
      }
      else
      {
         // High-side throttle -- set to full thrust
         mInjectFuel              = true;
         mOverallThrustMultiplier = 1.0;
      }
   }

   // Set "triggered" flag
   if (mInjectFuel)
   {
      mInjectFuelTriggered = true;
   }
   else
   {
      // If we are not injecting fuel, there can be no thrust and the engine is dead
      deadEngine = true;
   }

   // Check for latched fuel injection
   if (mLatchFuelInjection && mInjectFuelTriggered)
   {
      mInjectFuel              = true; // Always inject fuel when latched
      mOverallThrustMultiplier = 1.0;  // Always use full throttle when latched
   }

   // Adjust the burn request, based on throttle multiplier
   aFuelBurnRequest_lbs *= mOverallThrustMultiplier;

   // Adjust the thrust, based on throttle multiplier
   aThrust_lbs *= mOverallThrustMultiplier;

   // If we have a dead engine, adjust data accordingly
   if (deadEngine)
   {
      // Dead engines do not produce thrust and do not burn fuel
      mOverallThrustMultiplier = 0.0;
      aFuelBurnRequest_lbs     = 0.0;
      aThrust_lbs              = 0.0;
   }

   return deadEngine;
}
