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

#include "WsfSixDOF_RocketSolidPropellantEngine.hpp"

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

wsf::six_dof::RocketSolidPropellantEngine::RocketSolidPropellantEngine(ThrustProducerObject* aParentObject)
   : Engine(aParentObject)
{
   mTypeOfEngine = "SolidPropellantRocket";
}

wsf::six_dof::RocketSolidPropellantEngine::~RocketSolidPropellantEngine() {}

wsf::six_dof::RocketSolidPropellantEngine::RocketSolidPropellantEngine(const RocketSolidPropellantEngine& aSrc)
   : Engine(aSrc)
   , mThrustSeaLevelVersusTime(aSrc.mThrustSeaLevelVersusTime)
   , mThrustVacuumVersusTime(aSrc.mThrustVacuumVersusTime)
   , mIspAlt(aSrc.mIspAlt)
   , mIspSeaLevel(aSrc.mIspSeaLevel)
   , mIspVacuum(aSrc.mIspVacuum)
   , mRatedThrust_lbs(aSrc.mRatedThrust_lbs)
   , mLastThrust_lbs(aSrc.mLastThrust_lbs)
   , mIsBurning(aSrc.mIsBurning)
   , mBurnTime_sec(aSrc.mBurnTime_sec)
   , mCurrentFuelBurnRate_pph(0)
   , mGenerateSmokeTrail(aSrc.mGenerateSmokeTrail)
{
}

wsf::six_dof::Engine* wsf::six_dof::RocketSolidPropellantEngine::Clone() const
{
   return new RocketSolidPropellantEngine(*this);
}

void wsf::six_dof::RocketSolidPropellantEngine::DeriveFrom(const Engine* aSrc)
{
   auto rocketSrc = dynamic_cast<const RocketSolidPropellantEngine*>(aSrc);

   if (rocketSrc == nullptr)
   {
      return;
   }

   Engine::DeriveFrom(rocketSrc);

   mThrustSeaLevelVersusTime = rocketSrc->mThrustSeaLevelVersusTime;
   mThrustVacuumVersusTime   = rocketSrc->mThrustVacuumVersusTime;

   mIspAlt      = rocketSrc->mIspAlt;
   mIspSeaLevel = rocketSrc->mIspSeaLevel;
   mIspVacuum   = rocketSrc->mIspVacuum;

   mCurrentFuelTank         = rocketSrc->mCurrentFuelTank;
   mRatedThrust_lbs         = rocketSrc->mRatedThrust_lbs;
   mLastThrust_lbs          = rocketSrc->mLastThrust_lbs;
   mIsBurning               = rocketSrc->mIsBurning;
   mBurnTime_sec            = rocketSrc->mBurnTime_sec;
   mCurrentFuelBurnRate_pph = 0.0;

   mGenerateSmokeTrail = rocketSrc->mGenerateSmokeTrail;
}

bool wsf::six_dof::RocketSolidPropellantEngine::ProcessInput(UtInput& aInput, TypeManager* aTypeManager)
{
   bool               myCommand = true;
   const std::string& command   = aInput.GetCommand();

   if (command == "solid_propellant_rocket")
   {
      UtInputBlock block(aInput);
      while (block.ReadCommand())
      {
         std::string localCommand;
         localCommand = block.GetCommand();

         if (localCommand == "rated_thrust")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cFORCE);
            value *= UtMath::cLB_PER_NT;
            mRatedThrust_lbs = value;
         }
         else if (localCommand == "thrust_vs_time_sealevel")
         {
            mThrustSeaLevelVersusTime = ut::make_unique<UtTable::Curve>();

            mThrustSeaLevelVersusTime->ProcessInput(aInput,
                                                    UtInput::cNON_DIMENSIONAL,
                                                    "time_Sec",
                                                    UtTable::ValueGE(0.0),
                                                    UtInput::cNON_DIMENSIONAL,
                                                    "thrust_lbs",
                                                    UtTable::NoCheck());
         }
         else if (localCommand == "thrust_vs_time_vacuum")
         {
            mThrustVacuumVersusTime = ut::make_unique<UtTable::Curve>();

            mThrustVacuumVersusTime->ProcessInput(aInput,
                                                  UtInput::cNON_DIMENSIONAL,
                                                  "time_Sec",
                                                  UtTable::ValueGE(0.0),
                                                  UtInput::cNON_DIMENSIONAL,
                                                  "thrust_lbs",
                                                  UtTable::NoCheck());
         }
         else if (localCommand == "isp_vs_alt")
         {
            mIspAlt = ut::make_unique<UtTable::Curve>();

            mIspAlt->ProcessInput(aInput,
                                  UtInput::cNON_DIMENSIONAL,
                                  "alt",
                                  UtTable::NoCheck(),
                                  UtInput::cNON_DIMENSIONAL,
                                  "isp",
                                  UtTable::ValueGE(0.0));
         }
         else if (localCommand == "creates_smoke_trail")
         {
            bool value;
            aInput.ReadValue(value);
            mGenerateSmokeTrail = value;
         }
         else
         {
            auto out = ut::log::error() << "Unrecognized command within RocketSolidPropellantEngine::ProcessInput().";
            out.AddNote() << "Command: " << localCommand;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }

   return myCommand;
}

bool wsf::six_dof::RocketSolidPropellantEngine::Initialize(int64_t) //( int64_t aSimTime_nanosec)
{
   mIspSeaLevel = mIspAlt->Lookup(0.0);
   mIspVacuum   = mIspAlt->Lookup(500000.0);

   return true;
}

double wsf::six_dof::RocketSolidPropellantEngine::Thrust_lbs(double aBurnTime_Sec, double aAlt_ft, double& aMassFlow_pps)
{
   if (mThrustSeaLevelVersusTime)
   {
      aMassFlow_pps = mThrustSeaLevelVersusTime->Lookup(aBurnTime_Sec) / mIspSeaLevel;
   }
   else if (mThrustVacuumVersusTime)
   {
      aMassFlow_pps = mThrustVacuumVersusTime->Lookup(aBurnTime_Sec) / mIspVacuum;
   }
   else
   {
      aMassFlow_pps = 0.0;
   }

   double isp = mIspAlt->Lookup(aAlt_ft);

   return aMassFlow_pps * isp;
}

void wsf::six_dof::RocketSolidPropellantEngine::UpdatePropellantMass(double aRemainingPropellant_lbs)
{
   mCurrentFuelTank->SetCurrentFuelQuantity(aRemainingPropellant_lbs);
}

void wsf::six_dof::RocketSolidPropellantEngine::CalculateThrust(double aDeltaT_sec,
                                                                double aAlt_ft,
                                                                double, // aDynPress_lbsqft
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

   // Set the relative location of this thrust producer
   // aLocation_ft = mThrustLocation_ft;

   // bool ignitedInPast  = false;
   double ignitionThrust = 0.0;

   // If we have a small dT be sure we have 'some' thrust
   if (fabs(aDeltaT_sec) < 0.00001)
   {
      ignitionThrust = 0.00001;
   }

   const double tinyTick = 0.000000001;

   if (mIgniteTimeInFrame_nanosec > 0)
   {
      // This is only valid if we are just ignited. In this case, mIsBurning
      // is true but we have not yet accumulated any burn time yet.
      if (mBurnTime_sec < tinyTick)
      {
         // The ignition occurred in the past
         // ignitedInPast                    = true;

         double igniteFraction              = utils::TimeToTime(mIgniteTimeInFrame_nanosec) / aDeltaT_sec;
         mIgniteTimeInFrame_nanosec         = -1;
         double burnTime_Sec                = utils::TimeToTime(mIgniteTimeInFrame_nanosec);
         double massFlow_pps                = 0.0;
         double thrust_lbs                  = Thrust_lbs(burnTime_Sec, aAlt_ft, massFlow_pps);
         double fuelBurned_lbs              = massFlow_pps * aDeltaT_sec;
         double remainingPropellantMass_lbs = mCurrentFuelTank->GetCurrentFuelQuantity_lbs() - fuelBurned_lbs;
         bool   burnedOut                   = false;

         if (remainingPropellantMass_lbs < 0.0)
         {
            double unburned     = remainingPropellantMass_lbs;
            double burnFraction = (fuelBurned_lbs - unburned) / fuelBurned_lbs;
            thrust_lbs *= burnFraction;
            fuelBurned_lbs += remainingPropellantMass_lbs;
            remainingPropellantMass_lbs = 0.0;
            burnedOut                   = true;
         }

         if (burnedOut)
         {
            // Limit thrust to zero
            if (thrust_lbs < 0.0)
            {
               thrust_lbs = 0.0;
            }

            // Guard against tiny deltaT

            // Set returned values
            aFuelBurnRate_pps = fuelBurned_lbs / aDeltaT_sec;
            aFuelBurned_lbs   = fuelBurned_lbs;

            // Set F&M data
            aForceAndMoment = thrust_lbs;

            if (aUpdateData)
            {
               UpdatePropellantMass(remainingPropellantMass_lbs);

               mLastThrust_lbs = thrust_lbs;
               mBurnTime_sec += aDeltaT_sec;
               mCurrentThrust_lbs       = aForceAndMoment;
               mCurrentFuelBurnRate_pph = aFuelBurnRate_pps * 3600.0;
               mIsBurning               = false;
            }
            return;
         }
         else
         {
            ignitionThrust = thrust_lbs * igniteFraction;
         }
      } // End just ignited loop
   }

   double effectiveBurnFraction = 1.0;
   if (mShutdownInProgress && mIsBurning)
   {
      effectiveBurnFraction = utils::TimeToTime(mShutdownFraction_nanosec) / aDeltaT_sec;

      // Limit the effective burn fraction
      utils::LimitThrottleNormalized(effectiveBurnFraction);

      // Clear the flag if updating
      if (aUpdateData)
      {
         mShutdownInProgress = false;
         mIsBurning          = false;
      }
   }
   else if ((!mIsBurning) || (mCurrentFuelTank->GetCurrentFuelQuantity_lbs() <= 0.0))
   {
      aFuelBurnRate_pps = 0.0;
      aFuelBurned_lbs   = 0.0;

      // Set F&M data
      aForceAndMoment = 0.0;

      if (aUpdateData)
      {
         mLastThrust_lbs = 0.0;

         mCurrentThrust_lbs       = aForceAndMoment;
         mCurrentFuelBurnRate_pph = aFuelBurnRate_pps * 3600.0;
      }

      return;
   }

   double burnTime_Sec = mBurnTime_sec;
   if (!aUpdateData)
   {
      burnTime_Sec += aDeltaT_sec;
   }
   double massFlow_pps   = 0.0;
   double thrust_lbs     = Thrust_lbs(burnTime_Sec, aAlt_ft, massFlow_pps);
   double fuelBurned_lbs = massFlow_pps * aDeltaT_sec;

   thrust_lbs *= effectiveBurnFraction;
   fuelBurned_lbs *= effectiveBurnFraction;

   double remainingPropellantMass_lbs = mCurrentFuelTank->GetCurrentFuelQuantity_lbs() - fuelBurned_lbs;
   bool   burnedOut                   = false;

   if (remainingPropellantMass_lbs < 0.0)
   {
      double unburned     = remainingPropellantMass_lbs;
      double burnFraction = (fuelBurned_lbs - unburned) / fuelBurned_lbs;
      thrust_lbs *= burnFraction;
      fuelBurned_lbs += remainingPropellantMass_lbs;
      remainingPropellantMass_lbs = 0.0;
      burnedOut                   = true;
   }

   // We have to guard against a condition where the rocket motor has essentially
   // burned out with no thrust_lbs and no fuelBurned_lbs
   if ((thrust_lbs <= 0.0) && (fuelBurned_lbs <= 0.0) && (burnTime_Sec > 0.0))
   {
      remainingPropellantMass_lbs = 0.0;
      burnedOut                   = true;
   }

   // Add in 'ignition thrust'
   thrust_lbs += ignitionThrust;

   // Guard against tiny deltaT

   // Set returned fuel burn data
   aFuelBurnRate_pps = fuelBurned_lbs / aDeltaT_sec;
   aFuelBurned_lbs   = fuelBurned_lbs;

   // Set F&M data
   aForceAndMoment = thrust_lbs;

   if (aUpdateData)
   {
      mLastThrust_lbs = mCurrentThrust_lbs;

      mCurrentThrust_lbs       = aForceAndMoment;
      mCurrentFuelBurnRate_pph = aFuelBurnRate_pps * 3600.0;

      // Unlike engines that use fuel tanks, solid-propellant
      // rockets contain their own propellant/fuel, so special
      // handling is needed during "freeze" conditions. If it
      // is *not* frozen, we need to update the burn time and
      // propellant mass and possibly the IsBurning flag.

      if (!mParentThrustProducerPtr->GetParentVehicle()->GetFreezeFlags()->fuelBurn)
      {
         mBurnTime_sec += aDeltaT_sec;
         UpdatePropellantMass(remainingPropellantMass_lbs);

         if (burnedOut)
         {
            mIsBurning = false;
         }
      }
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

double wsf::six_dof::RocketSolidPropellantEngine::GetPercentMaxThrust() const
{
   return 100.0 * (mLastThrust_lbs / mRatedThrust_lbs);
}

double wsf::six_dof::RocketSolidPropellantEngine::GetFuelBurnRate_pph() const
{
   return mCurrentFuelBurnRate_pph;
}

double wsf::six_dof::RocketSolidPropellantEngine::GetMaximumPotentialThrust_lbs(double aAlt_ft,
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

double wsf::six_dof::RocketSolidPropellantEngine::GetMinimumPotentialThrust_lbs(double, // aAlt_ft,
                                                                                double, // aDynPress_lbsqft,
                                                                                double, // aStatPress_lbssqft,
                                                                                double, // aSpeed_fps,
                                                                                double, // aMach,
                                                                                double, // aAlpha_rad,
                                                                                double) // aBeta_rad
{
   // The min thrust is when the engine is turned off
   return 0.0;
}

void wsf::six_dof::RocketSolidPropellantEngine::SetThrottlePosition(double aThrottleLeverPosition)
{
   // Enforce throttle limits
   utils::LimitThrottleNormalized(aThrottleLeverPosition);

   if (aThrottleLeverPosition > 0.5)
   {
      IgniteNow();
   }
}

double wsf::six_dof::RocketSolidPropellantEngine::GetThrottlePosition() const
{
   if (mIsBurning)
   {
      return 1.0;
   }

   return 0.0;
}

void wsf::six_dof::RocketSolidPropellantEngine::Ignite(int64_t aIgniteTimeInFrame_nanosec)
{
   mIsBurning                 = true;
   mIgniteTimeInFrame_nanosec = aIgniteTimeInFrame_nanosec;
}

void wsf::six_dof::RocketSolidPropellantEngine::Shutdown(int64_t aTerminateTime_nanosec /*= 0*/)
{
   if (mIsBurning) // Must be burning to terminate
   {
      mShutdownInProgress       = true;
      mShutdownFraction_nanosec = aTerminateTime_nanosec;
   }
}
