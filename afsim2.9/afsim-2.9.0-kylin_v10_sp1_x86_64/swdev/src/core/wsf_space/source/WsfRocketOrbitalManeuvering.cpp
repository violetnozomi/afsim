// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfRocketOrbitalManeuvering.hpp"

#include <iostream>

#include "UtEarth.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtVec3.hpp"
#include "WsfOrbitalManeuver.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfSpaceMoverBase.hpp"
#include "WsfSpaceObserver.hpp"

WsfRocketOrbitalManeuvering::WsfRocketOrbitalManeuvering(const WsfRocketOrbitalManeuvering& aSrc)
   : WsfOrbitalManeuvering(aSrc)
   , mRocket(aSrc.mRocket)
{
}

// virtual
WsfRocketOrbitalManeuvering::~WsfRocketOrbitalManeuvering() {}

WsfOrbitalManeuvering* WsfRocketOrbitalManeuvering::Clone() const
{
   return new WsfRocketOrbitalManeuvering(*this);
}

// virtual
bool WsfRocketOrbitalManeuvering::Initialize(const UtCalendar& aEpoch)
{
   bool initialized = mRocket.Initialize() && WsfOrbitalManeuvering::Initialize(aEpoch);
   return initialized;
}

// virtual
bool WsfRocketOrbitalManeuvering::Rocket::ProcessInput(UtInput& aInput)
{
   bool        myCommand = false;
   std::string command   = aInput.GetCommand();
   if (command == "stage")
   {
      myCommand = true;
      WsfRocketOrbitalManeuvering::Stage stage;
      UtInputBlock                       block(aInput);
      while (block.ReadCommand())
      {
         myCommand = stage.ProcessInput(aInput);
         if (!myCommand)
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
      mStages.push_back(stage);
   }
   return myCommand;
}

double WsfRocketOrbitalManeuvering::GetRequiredDuration(double aDeltaV) const
{
   return mRocket.GetDuration(aDeltaV);
}

double WsfRocketOrbitalManeuvering::GetRequiredDeltaV(double aDuration) const
{
   return mRocket.GetDeltaV(aDuration);
}

double WsfRocketOrbitalManeuvering::GetAvailableDeltaV() const
{
   return mRocket.GetAvailableDeltaV();
}

double WsfRocketOrbitalManeuvering::GetAvailableDuration() const
{
   return mRocket.GetAvailableDuration();
}

bool WsfRocketOrbitalManeuvering::ReduceAvailableDeltaV_By(double aDeltaV)
{
   double deltaV       = GetAvailableDeltaV();
   bool   hasAvailable = (deltaV > aDeltaV);
   if (hasAvailable)
   {
      mRocket.Thrust(mRocket.GetDuration(aDeltaV), deltaV);
   }
   else if (mDebug)
   {
      ut::log::warning() << "Rocket Orbital Maneuvering: Insufficient fuel to perform desired maneuver.";
   }
   return hasAvailable;
}

bool WsfRocketOrbitalManeuvering::PerformStagingOperation()
{
   return mRocket.PerformStagingOperation();
}

bool WsfRocketOrbitalManeuvering::ProcessInput(UtInput& aInput)
{
   bool processed = true;
   if (mRocket.ProcessInput(aInput))
   {
   }
   else
   {
      processed = WsfOrbitalManeuvering::ProcessInput(aInput);
   }
   return processed;
}

//! Set attributes for empty mass, fuel mass, and payload mass in WsfPlatform so that they will be available in script.
void WsfRocketOrbitalManeuvering::SetPlatformAttributes(WsfPlatform* aPlatform) const
{
   aPlatform->SetEmptyMass(mRocket.GetEmptyMass());
   aPlatform->SetFuelMass(mRocket.GetFuelMass());
   aPlatform->SetPayloadMass(mRocket.GetPayloadMass());
}

//-----------------------------------------------------------------------------
// Stage Methods
WsfRocketOrbitalManeuvering::Stage::Stage()
   : mEmptyMass(0.0)
   , mFuelMass(0.0)
   , mTotalMass(0.0)
   , mBurnRate(0.0)
   , mThrust(0.0)
   , mExhaustVelocity(0.0)
{
}

bool WsfRocketOrbitalManeuvering::Stage::Initialize()
{
   // Burn rate must be specified.
   // Two of the three other variables must be specified.
   bool ok = true;
   if (mThrust == 0.0)
   {
      ut::log::error() << "Rocket Orbital Maneuvering: You must specify a thrust value.";
      return false;
   }
   if (mBurnRate == 0.0)
   {
      if (mExhaustVelocity != 0.0)
      {
         mBurnRate = mThrust / mExhaustVelocity;
      }
      else
      {
         ok = false;
      }
   }
   else if (mExhaustVelocity == 0.0)
   {
      if (mBurnRate != 0.0)
      {
         mExhaustVelocity = mThrust / mBurnRate;
      }
      else
      {
         ok = false;
      }
   }
   if (!ok)
   {
      // Providing input for specific_impulse sets the exhanust velocity variable.
      ut::log::error() << "Rocket Orbital Maneuvering: You must specify at least one of exhaust_velocity, "
                          "specific_impulse, or burn_rate.";
      return false;
   }

   if (mTotalMass == 0.0)
   {
      if ((mEmptyMass != 0.0) && (mFuelMass != 0.0))
      {
         mTotalMass = mEmptyMass + mFuelMass;
      }
      else
      {
         ok = false;
      }
   }
   else if (mEmptyMass == 0.0)
   {
      if ((mTotalMass != 0.0) && (mFuelMass != 0.0))
      {
         if (mFuelMass < mTotalMass)
         {
            mEmptyMass = mTotalMass - mFuelMass;
         }
         else
         {
            ut::log::error() << "Rocket Orbital Maneuvering: fuel_mass must be less than total_mass.";
            return false;
         }
      }
      else
      {
         ok = false;
      }
   }
   else if (mFuelMass == 0.0)
   {
      if ((mTotalMass != 0.0) && (mEmptyMass != 0.0))
      {
         if (mEmptyMass < mTotalMass)
         {
            mFuelMass = mTotalMass - mEmptyMass;
         }
         else
         {
            ut::log::error() << "Rocket Orbital Maneuvering: empty_mass must be less than total_mass.";
            return false;
         }
      }
      else
      {
         ok = false;
      }
   }
   if (!ok)
   {
      ut::log::error()
         << "Rocket Orbital Maneuvering: You must specify any two of total_mass, fuel_mass, and empty_mass.";
   }
   return ok;
}

bool WsfRocketOrbitalManeuvering::Stage::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if ((command == "initial_mass" || command == "total_mass"))
   {
      aInput.ReadValueOfType(mTotalMass, UtInput::cMASS);
      aInput.ValueGreaterOrEqual(mTotalMass, 0.0); // Zero is allow to implement coasting stages
   }
   else if ((command == "final_mass" || command == "empty_mass"))
   {
      aInput.ReadValueOfType(mEmptyMass, UtInput::cMASS);
      aInput.ValueGreaterOrEqual(mEmptyMass, 0.0);
   }
   else if ((command == "propellant_mass") || (command == "fuel_mass"))
   {
      aInput.ReadValueOfType(mFuelMass, UtInput::cMASS);
      aInput.ValueGreaterOrEqual(mFuelMass, 0.0);
   }
   else if (command == "burn_rate")
   {
      aInput.ReadValueOfType(mBurnRate, UtInput::cMASS_TRANSFER);
      aInput.ValueGreater(mBurnRate, 0.0);
   }
   else if (command == "specific_impulse")
   {
      double specificImpulse;
      aInput.ReadValueOfType(specificImpulse, UtInput::cTIME);
      aInput.ValueGreater(specificImpulse, 0.0);
      mExhaustVelocity = specificImpulse * UtEarth::cACCEL_OF_GRAVITY;
   }
   else if (command == "exhaust_velocity")
   {
      aInput.ReadValueOfType(mExhaustVelocity, UtInput::cTIME);
      aInput.ValueGreater(mExhaustVelocity, 0.0);
   }
   else if (command == "thrust")
   {
      aInput.ReadValueOfType(mThrust, UtInput::cFORCE);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

double WsfRocketOrbitalManeuvering::Stage::GetDuration(double aTotalMass, double aDeltaV) const
{
   double duration = aTotalMass / GetBurnRate() * (1.0 - exp(-aDeltaV / mExhaustVelocity));
   return duration;
}

double WsfRocketOrbitalManeuvering::Stage::GetAvailableDeltaV(double aTotalMass) const
{
   return (mExhaustVelocity * log(aTotalMass / (aTotalMass - GetFuelMass())));
}

double WsfRocketOrbitalManeuvering::Stage::GetDeltaV(double aTotalMass, double aDuration) const
{
   double fuelBurned = mBurnRate * aDuration;
   return (mExhaustVelocity * log(aTotalMass / (aTotalMass - fuelBurned)));
}

WsfRocketOrbitalManeuvering::Rocket::Rocket() {}

WsfRocketOrbitalManeuvering::Rocket::Rocket(const Rocket& aSrc)
   : mStages(aSrc.mStages)
   , mCurrentStageIter(mStages.begin())
{
}

bool WsfRocketOrbitalManeuvering::Rocket::Initialize()
{
   bool ok = (!mStages.empty());
   if (ok)
   {
      mCurrentStageIter = mStages.begin();
      for (auto& stage : mStages)
      {
         ok &= stage.Initialize();
      }
   }
   else
   {
      ut::log::error()
         << "Rocket Orbital Maneuvering: You must define at least one stage in rocket orbital maneuvering.";
   }
   return ok;
}

double WsfRocketOrbitalManeuvering::Stage::GetSpecificImpulse() const
{
   return (mExhaustVelocity / UtEarth::cACCEL_OF_GRAVITY);
}

double WsfRocketOrbitalManeuvering::Stage::Thrust(double aTotalMass, double aDuration, double& aDeltaV)
{
   double deltaFuelMass = mBurnRate * aDuration;
   double duration      = aDuration;
   if (deltaFuelMass > mFuelMass)
   {
      duration      = mFuelMass / mBurnRate;
      deltaFuelMass = mFuelMass;
   }

   // The next several lines are order dependent.
   aDeltaV = GetDeltaV(aTotalMass, duration);

   mFuelMass -= deltaFuelMass;
   mTotalMass -= deltaFuelMass;

   return duration;
}

//! Expend fuel / Delta-V for the specified duration.  If the current stage would expend its fuel,
//! and automatic staging is enabled, the rocket will perform a staging event and start burning the next stage.
//!@param aDuration The Specified duration to thrust.
//!@param aDeltaV   The calculated Delta-V.
//!@return The actual duration achieved.
double WsfRocketOrbitalManeuvering::Rocket::Thrust(double aDuration, double& aDeltaV)
{
   double duration = mCurrentStageIter->Thrust(GetTotalMass(), aDuration, aDeltaV);
   return duration;
}

//! Return how long it will take to execute a maneuver with the specified delta-V,
//! given the current mass properties of the rocket.
//!@param aDeltaV The delta-V to expend in the maneuver.
//!@return The time necessary to complete the maneuver.  If the rocket does not have
//! sufficient delta-v to complete the maneuver, WsfOrbitalManeuvering::cINFINITE_DURATION is returned.
double WsfRocketOrbitalManeuvering::Rocket::GetDuration(double aDeltaV) const
{
   double duration  = WsfOrbitalManeuvering::cINFINITE_DURATION;
   double totalMass = GetTotalMass();
   // double deltaV_Remaining = aDeltaV;
   if (mCurrentStageIter->GetAvailableDeltaV(totalMass) >= aDeltaV)
   {
      duration = mCurrentStageIter->GetDuration(totalMass, aDeltaV);
   }
   return duration;
}

//! Return the total delta-v available for the entire multi-stage rocket.
double WsfRocketOrbitalManeuvering::Rocket::GetAvailableDeltaV() const
{
   double totalMass = GetTotalMass(mCurrentStageIter);
   double deltaV    = mCurrentStageIter->GetAvailableDeltaV(totalMass);
   return deltaV;
}

double WsfRocketOrbitalManeuvering::Rocket::GetAvailableDuration() const
{
   return mCurrentStageIter->GetAvailableDuration();
}

//! Return the delta-v necessary to execute a maneuver with the specified duration,
//! given the current mass properties of the rocket.
//!@param aDuration The duration for which the rocket will thrust.
//!@return The delta-v that would be expended to complete the maneuver.  If the rocket does not have
//! sufficient fuel to thrust for the given duration of the maneuver, WsfOrbitalManeuvering::cINFINITE_DELTA_V is returned.
double WsfRocketOrbitalManeuvering::Rocket::GetDeltaV(double aDuration) const
{
   double deltaV    = WsfOrbitalManeuvering::cINFINITE_DELTA_V;
   double totalMass = GetTotalMass(mCurrentStageIter);
   double duration  = mCurrentStageIter->GetAvailableDuration();
   if (duration >= aDuration)
   {
      deltaV = mCurrentStageIter->GetDeltaV(totalMass, aDuration);
   }
   return deltaV;
}

//! Return the total mass of the rocket, including all stages.
double WsfRocketOrbitalManeuvering::Rocket::GetTotalMass() const
{
   double totalMass = 0.0;
   for (Stages::iterator iter = mCurrentStageIter; iter != mStages.end(); ++iter)
   {
      totalMass += iter->GetTotalMass();
   }
   return totalMass;
}

//! Return the fuel mass, considered to be the fuel for this stage only.
double WsfRocketOrbitalManeuvering::Rocket::GetFuelMass() const
{
   return mCurrentStageIter->GetFuelMass();
}

//! Return the empty mass, considered to be the total mass of this stage minus the fuel mass for this stage.
//! @note This definition is consistent with the definition in WsfPlatform, such that the total mass
//!   must equal the empty mass + fuel mass + payload mass.
double WsfRocketOrbitalManeuvering::Rocket::GetEmptyMass() const
{
   return mCurrentStageIter->GetTotalMass() - mCurrentStageIter->GetFuelMass();
}

//! Return the payload mass, which is the total mass of all subsequent stages, if any.
double WsfRocketOrbitalManeuvering::Rocket::GetPayloadMass() const
{
   double           payloadMass = 0.0;
   Stages::iterator nextStage   = mCurrentStageIter;
   ++nextStage;

   // Is there a next stage?
   if (nextStage != mStages.end())
   {
      // Return the total mass of all subsequent stages.
      payloadMass = GetTotalMass(nextStage);
   }

   return payloadMass;
}

//! Perform a staging event for the rocket.  If no other stages are present, false is returned.
bool WsfRocketOrbitalManeuvering::Rocket::PerformStagingOperation()
{
   bool staged = false;
   if (mCurrentStageIter != mStages.end())
   {
      ++mCurrentStageIter;
      if (mCurrentStageIter != mStages.end())
      {
         staged = true;
      }
      else
      {
         --mCurrentStageIter;
      }
   }
   return staged;
}

//! Return the total mass, including the provided stage, to the final stage.
double WsfRocketOrbitalManeuvering::Rocket::GetTotalMass(const Stages::iterator aFromStage) const
{
   double totalMass = 0.0;
   for (Stages::iterator iter = aFromStage; iter != mStages.end(); ++iter)
   {
      totalMass += iter->GetTotalMass();
   }
   return totalMass;
}
