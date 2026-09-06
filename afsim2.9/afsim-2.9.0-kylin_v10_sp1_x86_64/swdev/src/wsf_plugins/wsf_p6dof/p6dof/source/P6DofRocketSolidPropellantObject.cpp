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

#include "P6DofRocketSolidPropellantObject.hpp"

#include <fstream>

#include "P6DofFreezeFlags.hpp"
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

P6DofRocketSolidPropellantObject::P6DofRocketSolidPropellantObject(P6DofScenario* aScenario)
   : P6DofThrustProducerObject(aScenario)
   , mThrustSeaLevelVersusTime(nullptr)
   , mThrustVacuumVersusTime(nullptr)
   , mIspAlt(nullptr)
   , mIspSeaLevel(0)
   , mIspVacuum(0)
   , mRatedThrust_lbs(0)
   , mLastThrust_lbs(0)
   , mPropellantMass_lbs(0)
   , mMaxPropellantMass_lbs(0)
   , mIsBurning(false)
   , mBurnTime_sec(0)
   , mCurrentFuelBurnRate_pph(0)
   , mPropellantCmLocation_ft(0.0, 0.0, 0.0)
   , mThrottleSettingYaw(nullptr)
   , mThrottleSettingPitch(nullptr)
   , mGenerateSmokeTrail(true)
{
   mTypeOfThrustProducer = "SolidPropellantRocket";

   // Create the mass properties -- solid-propellant rockets are one of the few
   // thrust producer objects that use mass properties
   SetMassProperties(new P6DofMassProperties());
}

P6DofRocketSolidPropellantObject::~P6DofRocketSolidPropellantObject()
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

P6DofRocketSolidPropellantObject::P6DofRocketSolidPropellantObject(const P6DofRocketSolidPropellantObject& aSrc)
   : P6DofThrustProducerObject(aSrc)
   , mThrustSeaLevelVersusTime(aSrc.mThrustSeaLevelVersusTime)
   , mThrustVacuumVersusTime(aSrc.mThrustVacuumVersusTime)
   , mIspAlt(aSrc.mIspAlt)
   , mIspSeaLevel(aSrc.mIspSeaLevel)
   , mIspVacuum(aSrc.mIspVacuum)
   , mRatedThrust_lbs(aSrc.mRatedThrust_lbs)
   , mLastThrust_lbs(aSrc.mLastThrust_lbs)
   , mPropellantMass_lbs(aSrc.mPropellantMass_lbs)
   , mMaxPropellantMass_lbs(aSrc.mMaxPropellantMass_lbs)
   , mIsBurning(aSrc.mIsBurning)
   , mBurnTime_sec(aSrc.mBurnTime_sec)
   , mCurrentFuelBurnRate_pph(0)
   , mPropellantCmLocation_ft(aSrc.mPropellantCmLocation_ft)
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

P6DofRocketSolidPropellantObject* P6DofRocketSolidPropellantObject::Clone() const
{
   return new P6DofRocketSolidPropellantObject(*this);
}

void P6DofRocketSolidPropellantObject::DeriveFrom(const P6DofRocketSolidPropellantObject* aSrc)
{
   P6DofThrustProducerObject::DeriveFrom(aSrc);

   if (aSrc == nullptr)
   {
      return;
   }

   mIspAlt                   = aSrc->mIspAlt;
   mThrustSeaLevelVersusTime = aSrc->mThrustSeaLevelVersusTime;
   mThrustVacuumVersusTime   = aSrc->mThrustVacuumVersusTime;

   mIspSeaLevel             = aSrc->mIspSeaLevel;
   mIspVacuum               = aSrc->mIspVacuum;
   mRatedThrust_lbs         = aSrc->mRatedThrust_lbs;
   mLastThrust_lbs          = aSrc->mLastThrust_lbs;
   mPropellantMass_lbs      = aSrc->mPropellantMass_lbs;
   mMaxPropellantMass_lbs   = mPropellantMass_lbs;
   mIsBurning               = aSrc->mIsBurning;
   mBurnTime_sec            = aSrc->mBurnTime_sec;
   mCurrentFuelBurnRate_pph = 0.0;
   mPropellantCmLocation_ft = aSrc->mPropellantCmLocation_ft;

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

   CalculateCurrentMassProperties();

   mGenerateSmokeTrail = aSrc->mGenerateSmokeTrail;
}

void P6DofRocketSolidPropellantObject::SetControlInputHandles(const P6DofFlightControlSystem* aFlightControls)
{
   SetControlInputHandle(aFlightControls, mThrottleSettingYaw);
   SetControlInputHandle(aFlightControls, mThrottleSettingPitch);
}

void P6DofRocketSolidPropellantObject::SetControlInputValues(const P6DofFlightControlSystem* aFlightControls)
{
   SetControlInputValue(aFlightControls, mThrottleSettingYaw);
   SetControlInputValue(aFlightControls, mThrottleSettingPitch);
}

bool P6DofRocketSolidPropellantObject::ProcessInput(UtInput& aInput)
{
   bool               myCommand = true;
   const std::string& command   = aInput.GetCommand();

   if (command == "solid_propellant_rocket_type")
   {
      std::string typeName;
      std::string baseType;
      aInput.ReadValue(typeName);
      aInput.ReadValue(baseType);
      SetName(typeName); // Types should have the typeName be their name

      bool derivedFromBase = true;
      if (baseType != "P6DOF_SOLID_PROPELLANT_ROCKET")
      {
         derivedFromBase = false;
         // This is not derived from the base type
         // Determine if this base type exists
         if (!GetScenario()->GetTypeManager()->ThrustProducerObjectTypeExists(baseType))
         {
            // Unknown type -- error since we can't derive from an unknown type
            auto out = ut::log::error() << "P6DofRocketSolidPropellantObject is trying to derive from an unknown type "
                                           "in P6DofRocketSolidPropellantObject::ProcessInput.";
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
         auto out = ut::log::error() << "P6DofRocketSolidPropellantObject is using a duplicate name in "
                                        "P6DofRocketSolidPropellantObject::ProcessInput.";
         out.AddNote() << "Name: " << typeName;
         out.AddNote() << "Location: " << aInput.GetLocation();
         throw UtInput::UnknownCommand(aInput);
      }

      // If derived, clone the data
      if (!derivedFromBase)
      {
         DeriveFrom(dynamic_cast<P6DofRocketSolidPropellantObject*>(
            GetScenario()->GetTypeManager()->GetThrustProducerObjectType(baseType)));
      }

      // Register the object with the manager
      if (!GetScenario()->GetTypeManager()->RegisterThrustProducerObjectType(this))
      {
         auto out =
            ut::log::error()
            << "P6DofRocketSolidPropellantObject unable to register in P6DofRocketSolidPropellantObject::ProcessInput.";
         out.AddNote() << "Name: " << typeName;
         throw UtInput::UnknownCommand(aInput);
      }

      UtInputBlock block(aInput, "end_solid_propellant_rocket_type");
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
         else if (localCommand == "propellant_mass")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cMASS);
            value *= UtMath::cLB_PER_KG;
            mPropellantMass_lbs = value;

            // Set max to same value
            mMaxPropellantMass_lbs = value;
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
         else if (localCommand == "propellant_rel_pos_x")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cLENGTH);

            // mPropellantCmLocation_ft is in feet
            double x, y, z;
            mPropellantCmLocation_ft.Get(x, y, z);
            x = value * UtMath::cFT_PER_M;
            mPropellantCmLocation_ft.Set(x, y, z);
         }
         else if (localCommand == "propellant_rel_pos_y")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cLENGTH);

            // mPropellantCmLocation_ft is in feet
            double x, y, z;
            mPropellantCmLocation_ft.Get(x, y, z);
            y = value * UtMath::cFT_PER_M;
            mPropellantCmLocation_ft.Set(x, y, z);
         }
         else if (localCommand == "propellant_rel_pos_z")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cLENGTH);

            // mPropellantCmLocation_ft is in feet
            double x, y, z;
            mPropellantCmLocation_ft.Get(x, y, z);
            z = value * UtMath::cFT_PER_M;
            mPropellantCmLocation_ft.Set(x, y, z);
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
               ut::log::error() << "Unrecognized command within P6DofRocketSolidPropellantObject::ProcessInput().";
            out.AddNote() << "Command: " << localCommand;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }

   return myCommand;
}

bool P6DofRocketSolidPropellantObject::Initialize(int64_t) //( int64_t aSimTime_nanosec)
{
   mIspSeaLevel = mIspAlt->Lookup(0.0);
   mIspVacuum   = mIspAlt->Lookup(500000.0);

   CalculateCurrentMassProperties();

   return true;
}

double P6DofRocketSolidPropellantObject::Thrust_lbs(double aBurnTime_Sec, double aAlt_ft, double& aMassFlow_pps)
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

void P6DofRocketSolidPropellantObject::CalculateThrust(double                      aDeltaT_sec,
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

void P6DofRocketSolidPropellantObject::UpdateThrust(double                      aDeltaT_sec,
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
}

void P6DofRocketSolidPropellantObject::CalculateThrust(double aDeltaT_sec,
                                                       double aAlt_ft,
                                                       double, //    aDynPress_lbsqft
                                                       double, //    aStatPress_lbssqft,
                                                       double, //    aSpeed_fps,
                                                       double, //    aMach,
                                                       double, //    aAlpha_rad,
                                                       double, //    aBeta_rad,
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

         double igniteFraction              = P6DofUtils::TimeToTime(mIgniteTimeInFrame_nanosec) / aDeltaT_sec;
         mIgniteTimeInFrame_nanosec         = -1;
         double burnTime_Sec                = P6DofUtils::TimeToTime(mIgniteTimeInFrame_nanosec);
         double massFlow_pps                = 0.0;
         double thrust_lbs                  = Thrust_lbs(burnTime_Sec, aAlt_ft, massFlow_pps);
         double fuelBurned_lbs              = massFlow_pps * aDeltaT_sec;
         double remainingPropellantMass_lbs = mPropellantMass_lbs - fuelBurned_lbs;
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
            UtVec3dX force_lbs(0., 0., 0.);
            UtVec3dX moment_ftlbs(0., 0., 0.);

            // Limit thrust to zero
            if (thrust_lbs < 0.0)
            {
               thrust_lbs = 0.0;
            }

            // Guard against tiny deltaT

            // Set returned values
            aFuelBurnRate_pps = fuelBurned_lbs / aDeltaT_sec;
            aFuelBurned_lbs   = fuelBurned_lbs;
            force_lbs         = CalcThrustVectorFromThrust(thrust_lbs);
            moment_ftlbs.Set(0, 0, 0);

            // Set F&M data
            aForceAndMoment.MoveRefPoint_ft(mThrustLocation_ft);
            aForceAndMoment.ClearForcesAndMoments();
            aForceAndMoment.AddForceAndMomentAtReferencePoint(force_lbs, moment_ftlbs);

            if (aUpdateData)
            {
               mLastThrust_lbs = thrust_lbs;
               mBurnTime_sec += aDeltaT_sec;
               mPropellantMass_lbs        = remainingPropellantMass_lbs;
               mCurrentThrustFM_lbs_ftlbs = aForceAndMoment;
               mCurrentFuelBurnRate_pph   = aFuelBurnRate_pps * 3600.0;
               mIsBurning                 = false;
               mPropellantMass_lbs        = 0.0;
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
      effectiveBurnFraction = P6DofUtils::TimeToTime(mShutdownFraction_nanosec) / aDeltaT_sec;

      // Limit the effective burn fraction
      P6DofUtils::LimitThrottleNormalized(effectiveBurnFraction);

      // Clear the flag if updating
      if (aUpdateData)
      {
         mShutdownInProgress = false;
         mIsBurning          = false;
      }
   }
   else if ((!mIsBurning) || (mPropellantMass_lbs <= 0.0))
   {
      // We're not burning, so return zeros
      UtVec3dX force_lbs(0., 0., 0.);
      UtVec3dX moment_ftlbs(0., 0., 0.);

      aFuelBurnRate_pps = 0.0;
      aFuelBurned_lbs   = 0.0;

      // Set F&M data
      aForceAndMoment.MoveRefPoint_ft(mThrustLocation_ft);
      aForceAndMoment.ClearForcesAndMoments();
      aForceAndMoment.AddForceAndMomentAtReferencePoint(force_lbs, moment_ftlbs);

      if (aUpdateData)
      {
         mLastThrust_lbs = 0.0;

         mCurrentThrustFM_lbs_ftlbs = aForceAndMoment;
         mCurrentFuelBurnRate_pph   = aFuelBurnRate_pps * 3600.0;
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

   double remainingPropellantMass_lbs = mPropellantMass_lbs - fuelBurned_lbs;
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

   P6DofForceAndMomentsObject thrustFM;
   CalcThrustProducerFM(thrust_lbs, thrustFM);

   // Guard against tiny deltaT

   // Set returned fuel burn data
   aFuelBurnRate_pps = fuelBurned_lbs / aDeltaT_sec;
   aFuelBurned_lbs   = fuelBurned_lbs;

   // Set F&M data
   aForceAndMoment = thrustFM;

   if (aUpdateData)
   {
      mLastThrust_lbs = thrust_lbs;

      mCurrentThrustFM_lbs_ftlbs = aForceAndMoment;
      mCurrentFuelBurnRate_pph   = aFuelBurnRate_pps * 3600.0;

      // Unlike engines that use fuel tanks, solid-propellant
      // rockets contain their own propellant/fuel, so special
      // handling is needed during "freeze" conditions. If it
      // is *not* frozen, we need to update the burn time and
      // propellant mass and possibly the IsBurning flag.

      if (!GetScenario()->GetFreezeFlags()->GetMasterFreezeFuelBurn())
      {
         mBurnTime_sec += aDeltaT_sec;
         mPropellantMass_lbs = remainingPropellantMass_lbs;

         if (burnedOut)
         {
            mIsBurning          = false;
            mPropellantMass_lbs = 0.0;
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
   P6DofVehicle* vehicle = GetParentVehicle();
   if (vehicle)
   {
      if (mEngineOperating && vehicle->WithinContrailAltitudeBand(aAlt_ft))
      {
         mContrailing = true;
      }
   }
}

double P6DofRocketSolidPropellantObject::GetPercentMaxThrust() const
{
   return 100.0 * (mLastThrust_lbs / mRatedThrust_lbs);
}

double P6DofRocketSolidPropellantObject::GetCurrentPropellant_lbs() const
{
   return mPropellantMass_lbs;
}

double P6DofRocketSolidPropellantObject::GetMaxPropellant_lbs() const
{
   return mMaxPropellantMass_lbs;
}

double P6DofRocketSolidPropellantObject::GetFuelBurnRate_pph() const
{
   return mCurrentFuelBurnRate_pph;
}

double P6DofRocketSolidPropellantObject::GetMaximumPotentialThrust_lbs(double aAlt_ft,
                                                                       double aDynPress_lbsqft,
                                                                       double aStatPress_lbssqft,
                                                                       double aSpeed_fps,
                                                                       double aMach,
                                                                       double aAlpha_rad,
                                                                       double aBeta_rad)
{
   P6DofForceAndMomentsObject forceAndMoment;
   double                     fuelBurnRate_pps = 0;
   double                     fuelBurned_lbs   = 0;

   CalculateThrust(0.01,
                   aAlt_ft,
                   aDynPress_lbsqft,
                   aStatPress_lbssqft,
                   aSpeed_fps,
                   aMach,
                   aAlpha_rad,
                   aBeta_rad,
                   forceAndMoment,
                   fuelBurnRate_pps,
                   fuelBurned_lbs);

   return forceAndMoment.GetForceMagnitude_lbs();
}

double P6DofRocketSolidPropellantObject::GetMinimumPotentialThrust_lbs(double, // aAlt_ft,
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

void P6DofRocketSolidPropellantObject::CalculateCurrentMassProperties()
{
   // Simply return if we lack mass properties
   if (mMassPropertiesPtr == nullptr)
   {
      return;
   }

   // Clear the mass properties to zero
   mMassPropertiesPtr->ClearData();

   // Add the propellant mass at the propellant location
   if (mPropellantMass_lbs > 0)
   {
      mMassPropertiesPtr->AddMassAtLocation(mPropellantMass_lbs, mPropellantCmLocation_ft);
   }
}

void P6DofRocketSolidPropellantObject::SetThrottlePosition(double aThrottleLeverPosition)
{
   // Enforce throttle limits
   P6DofUtils::LimitThrottleNormalized(aThrottleLeverPosition);

   if (aThrottleLeverPosition > 0.5)
   {
      IgniteNow();
   }
}

double P6DofRocketSolidPropellantObject::GetThrottlePosition() const
{
   if (mIsBurning)
   {
      return 1.0;
   }

   return 0.0;
}

void P6DofRocketSolidPropellantObject::Ignite(int64_t aIgniteTimeInFrame_nanosec)
{
   mIsBurning                 = true;
   mIgniteTimeInFrame_nanosec = aIgniteTimeInFrame_nanosec;
}

void P6DofRocketSolidPropellantObject::Shutdown(int64_t aTerminateTime_nanosec /*= 0*/)
{
   if (mIsBurning) // Must be burning to terminate
   {
      mShutdownInProgress       = true;
      mShutdownFraction_nanosec = aTerminateTime_nanosec;
   }
}
