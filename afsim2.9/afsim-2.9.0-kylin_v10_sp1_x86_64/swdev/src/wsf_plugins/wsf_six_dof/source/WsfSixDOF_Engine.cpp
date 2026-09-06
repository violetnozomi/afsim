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

#include "WsfSixDOF_Engine.hpp"

#include "WsfSixDOF_PropulsionSystem.hpp"

wsf::six_dof::Engine::Engine(ThrustProducerObject* aParentObject)
   : mParentThrustProducerPtr(aParentObject)
{
}

void wsf::six_dof::Engine::DeriveFrom(const Engine* aSrc)
{
   if (aSrc != nullptr)
   {
      mTypeOfEngine = aSrc->mTypeOfEngine;
   }
}

wsf::six_dof::Engine::Engine(const Engine& aSrc)
   : mParentThrustProducerPtr(aSrc.mParentThrustProducerPtr)
   , mTypeOfEngine(aSrc.mTypeOfEngine)
   , mCurrentFuelTank(nullptr)
   , mCurrentThrust_lbs(aSrc.mCurrentThrust_lbs)
   , mShutdownInProgress(aSrc.mShutdownInProgress)
   , mShutdownFraction_nanosec(aSrc.mShutdownFraction_nanosec)
   , mIgniteTimeInFrame_nanosec(aSrc.mIgniteTimeInFrame_nanosec)
   , mEngineOperating(aSrc.mEngineOperating)
   , mContrailing(aSrc.mContrailing)
   , mAfterburnerPresent(aSrc.mAfterburnerPresent)
   , mAfterburnerOn(aSrc.mAfterburnerOn)
   , mProducingSmokeTrail(aSrc.mProducingSmokeTrail)
   , mEngineDamageSmokeActivated(aSrc.mEngineDamageSmokeActivated)
   , mEngineSmoking(aSrc.mEngineSmoking)
{
}

void wsf::six_dof::Engine::CalculateThrust(double  aDeltaT_sec,
                                           double  aAlt_ft,
                                           double  aDynPress_lbsqft,
                                           double  aStatPress_lbssqft,
                                           double  aSpeed_fps,
                                           double  aMach,
                                           double  aAlpha_rad,
                                           double  aBeta_rad,
                                           double& aForce_lbs,
                                           double& aFuelBurnRate_pps,
                                           double& aFuelBurned_lbs)
{
   CalculateThrust(aDeltaT_sec,
                   aAlt_ft,
                   aDynPress_lbsqft,
                   aStatPress_lbssqft,
                   aSpeed_fps,
                   aMach,
                   aAlpha_rad,
                   aBeta_rad,
                   aForce_lbs,
                   aFuelBurnRate_pps,
                   aFuelBurned_lbs,
                   false);
}

void wsf::six_dof::Engine::UpdateThrust(double  aDeltaT_sec,
                                        double  aAlt_ft,
                                        double  aDynPress_lbsqft,
                                        double  aStatPress_lbssqft,
                                        double  aSpeed_fps,
                                        double  aMach,
                                        double  aAlpha_rad,
                                        double  aBeta_rad,
                                        double& aForce_lbs,
                                        double& aFuelBurnRate_pps,
                                        double& aFuelBurned_lbs)
{
   CalculateThrust(aDeltaT_sec,
                   aAlt_ft,
                   aDynPress_lbsqft,
                   aStatPress_lbssqft,
                   aSpeed_fps,
                   aMach,
                   aAlpha_rad,
                   aBeta_rad,
                   aForce_lbs,
                   aFuelBurnRate_pps,
                   aFuelBurned_lbs,
                   true);
}

bool wsf::six_dof::Engine::CheckCurrentFuelTank()
{
   // Ensure that we have an active fuel flow path to the current tank
   if (mCurrentFuelTank != nullptr)
   {
      bool flowPathIntact = mCurrentFuelTank->FuelFlowPathIntact(mParentThrustProducerPtr->GetParentPropulsionSystem());
      if (flowPathIntact)
      {
         return true;
      }

      // We do not have a fuel flow path to the current tank. This is often because an
      // external fuel tank has been jettisoned.

      // Since the tank is invalid, set pointer to null, since we no longer can get fuel
      mCurrentFuelTank = nullptr;
   }
   return false;
}

bool wsf::six_dof::Engine::SetFuelTank(const std::string& aFuelTankName)
{
   mCurrentFuelTank = mParentThrustProducerPtr->GetParentPropulsionSystem()->GetFuelTank(aFuelTankName);

   if (mCurrentFuelTank != nullptr)
   {
      bool flowPathIntact = mCurrentFuelTank->FuelFlowPathIntact(mParentThrustProducerPtr->GetParentPropulsionSystem());
      if (!flowPathIntact)
      {
         auto out = ut::log::error() << "Cannot achieve a fuel flow path to fuel tank in Engine::SetFuelTank().";
         out.AddNote() << "Fuel Tank: " << aFuelTankName;
         return false;
      }
   }

   // Provide warning if no tank is present
   if (mCurrentFuelTank == nullptr)
   {
      auto out = ut::log::error() << "Unknown fuel tank selected in Engine::SetFuelTank().";
      out.AddNote() << "Fuel Tank: " << aFuelTankName;
      return false;
   }

   return true;
}
