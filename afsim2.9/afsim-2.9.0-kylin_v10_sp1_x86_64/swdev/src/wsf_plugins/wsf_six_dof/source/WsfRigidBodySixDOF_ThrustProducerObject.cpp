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

#include "WsfRigidBodySixDOF_ThrustProducerObject.hpp"

#include "UtDCM.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtInputFile.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "WsfRigidBodySixDOF_PropulsionSystem.hpp"
#include "WsfSixDOF_TypeManager.hpp"

wsf::six_dof::RigidBodyThrustProducerObject::RigidBodyThrustProducerObject(const RigidBodyThrustProducerObject& aSrc)
   : ThrustProducerObject(aSrc)
   , mCurrentThrustVectorDirection(aSrc.mCurrentThrustVectorDirection)
   , mThrustVectorYaw_rad(aSrc.mThrustVectorYaw_rad)
   , mThrustVectorPitch_rad(aSrc.mThrustVectorPitch_rad)
   , mThrustReverserEnabled(aSrc.mThrustReverserEnabled)
   , mThrustReverserSetting(aSrc.mThrustReverserSetting)
   , mShutdownInProgress(aSrc.mShutdownInProgress)
   , mShutdownFraction_nanosec(aSrc.mShutdownFraction_nanosec)
   , mIgniteTimeInFrame_nanosec(aSrc.mIgniteTimeInFrame_nanosec)
   , mCurrentThrustFactor(aSrc.mCurrentThrustFactor)
{
   if (aSrc.mThrottleSettingMIL != nullptr)
   {
      mThrottleSettingMIL = ut::make_unique<ThrustControlInputValueData>(*aSrc.mThrottleSettingMIL.get());
   }
   if (aSrc.mThrottleSettingAB != nullptr)
   {
      mThrottleSettingAB = ut::make_unique<ThrustControlInputValueData>(*aSrc.mThrottleSettingAB.get());
   }
   if (aSrc.mThrottleSettingReverser != nullptr)
   {
      mThrottleSettingReverser = ut::make_unique<ThrustControlInputValueData>(*aSrc.mThrottleSettingReverser.get());
   }
   if (aSrc.mThrottleSettingYaw != nullptr)
   {
      mThrottleSettingYaw = ut::make_unique<ThrustControlInputValueData>(*aSrc.mThrottleSettingYaw.get());
   }
   if (aSrc.mThrottleSettingPitch != nullptr)
   {
      mThrottleSettingPitch = ut::make_unique<ThrustControlInputValueData>(*aSrc.mThrottleSettingPitch.get());
   }
}

void wsf::six_dof::RigidBodyThrustProducerObject::DeriveFrom(const ThrustProducerObject* aSrc)
{
   ThrustProducerObject::DeriveFrom(aSrc);

   auto rigidBodyThrustProducer = dynamic_cast<const RigidBodyThrustProducerObject*>(aSrc);
   if (rigidBodyThrustProducer == nullptr)
   {
      // Unknown type -- error since we can't derive from an unknown type
      auto out = ut::log::error() << "RigidBodyThrustProducerObject is trying to derive from a different class.";
      out.AddNote() << "Base Type: " << mBaseName;
      return;
   }

   mCurrentThrustVectorDirection = rigidBodyThrustProducer->mCurrentThrustVectorDirection;
   mThrustVectorYaw_rad          = rigidBodyThrustProducer->mThrustVectorYaw_rad;
   mThrustVectorPitch_rad        = rigidBodyThrustProducer->mThrustVectorPitch_rad;
   mThrustReverserEnabled        = rigidBodyThrustProducer->mThrustReverserEnabled;
   mThrustReverserSetting        = rigidBodyThrustProducer->mThrustReverserSetting;
   mShutdownInProgress           = rigidBodyThrustProducer->mShutdownInProgress;
   mShutdownFraction_nanosec     = rigidBodyThrustProducer->mShutdownFraction_nanosec;
   mIgniteTimeInFrame_nanosec    = rigidBodyThrustProducer->mIgniteTimeInFrame_nanosec;
   mCurrentThrustFactor          = rigidBodyThrustProducer->mCurrentThrustFactor;

   if (rigidBodyThrustProducer->mThrottleSettingMIL != nullptr)
   {
      mThrottleSettingMIL =
         ut::make_unique<ThrustControlInputValueData>(*rigidBodyThrustProducer->mThrottleSettingMIL.get());
   }
   if (rigidBodyThrustProducer->mThrottleSettingAB != nullptr)
   {
      mThrottleSettingAB =
         ut::make_unique<ThrustControlInputValueData>(*rigidBodyThrustProducer->mThrottleSettingAB.get());
   }
   if (rigidBodyThrustProducer->mThrottleSettingReverser != nullptr)
   {
      mThrottleSettingReverser =
         ut::make_unique<ThrustControlInputValueData>(*rigidBodyThrustProducer->mThrottleSettingReverser.get());
   }
   if (rigidBodyThrustProducer->mThrottleSettingYaw != nullptr)
   {
      mThrottleSettingYaw =
         ut::make_unique<ThrustControlInputValueData>(*rigidBodyThrustProducer->mThrottleSettingYaw.get());
   }
   if (rigidBodyThrustProducer->mThrottleSettingPitch != nullptr)
   {
      mThrottleSettingPitch =
         ut::make_unique<ThrustControlInputValueData>(*rigidBodyThrustProducer->mThrottleSettingPitch.get());
   }
}

wsf::six_dof::ThrustProducerObject* wsf::six_dof::RigidBodyThrustProducerObject::Clone() const
{
   return new RigidBodyThrustProducerObject(*this);
}

void wsf::six_dof::RigidBodyThrustProducerObject::CalculateThrust(double                 aDeltaT_sec,
                                                                  double                 aAlt_ft,
                                                                  double                 aDynPress_lbsqft,
                                                                  double                 aStatPress_lbssqft,
                                                                  double                 aSpeed_fps,
                                                                  double                 aMach,
                                                                  double                 aAlpha_rad,
                                                                  double                 aBeta_rad,
                                                                  ForceAndMomentsObject& aForceAndMoment,
                                                                  double&                aFuelBurnRate_pps,
                                                                  double&                aFuelBurned_lbs)
{
   double thrustForce_lbs = 0.0;

   mEnginePtr->CalculateThrust(aDeltaT_sec,
                               aAlt_ft,
                               aDynPress_lbsqft,
                               aStatPress_lbssqft,
                               aSpeed_fps,
                               aMach,
                               aAlpha_rad,
                               aBeta_rad,
                               thrustForce_lbs,
                               aFuelBurnRate_pps,
                               aFuelBurned_lbs);

   CalcThrustProducerFM(thrustForce_lbs, aForceAndMoment);
}

void wsf::six_dof::RigidBodyThrustProducerObject::UpdateThrust(double                 aDeltaT_sec,
                                                               double                 aAlt_ft,
                                                               double                 aDynPress_lbsqft,
                                                               double                 aStatPress_lbssqft,
                                                               double                 aSpeed_fps,
                                                               double                 aMach,
                                                               double                 aAlpha_rad,
                                                               double                 aBeta_rad,
                                                               ForceAndMomentsObject& aForceAndMoment,
                                                               double&                aFuelBurnRate_pps,
                                                               double&                aFuelBurned_lbs)
{
   double thrustForce_lbs = 0.0;

   mEnginePtr->UpdateThrust(aDeltaT_sec,
                            aAlt_ft,
                            aDynPress_lbsqft,
                            aStatPress_lbssqft,
                            aSpeed_fps,
                            aMach,
                            aAlpha_rad,
                            aBeta_rad,
                            thrustForce_lbs,
                            aFuelBurnRate_pps,
                            aFuelBurned_lbs);

   CalcThrustProducerFM(thrustForce_lbs, aForceAndMoment);
}

void wsf::six_dof::RigidBodyThrustProducerObject::SetControlInputHandles(const RigidBodyFlightControlSystem* aFlightControls)
{
   SetControlInputHandle(aFlightControls, mThrottleSettingMIL.get());
   SetControlInputHandle(aFlightControls, mThrottleSettingAB.get());
   SetControlInputHandle(aFlightControls, mThrottleSettingReverser.get());
   SetControlInputHandle(aFlightControls, mThrottleSettingYaw.get());
   SetControlInputHandle(aFlightControls, mThrottleSettingPitch.get());
}

void wsf::six_dof::RigidBodyThrustProducerObject::SetControlInputValues(const RigidBodyFlightControlSystem* aFlightControls)
{
   SetControlInputValue(aFlightControls, mThrottleSettingMIL.get());
   SetControlInputValue(aFlightControls, mThrottleSettingAB.get());
   SetControlInputValue(aFlightControls, mThrottleSettingReverser.get());
   SetControlInputValue(aFlightControls, mThrottleSettingYaw.get());
   SetControlInputValue(aFlightControls, mThrottleSettingPitch.get());
}

bool wsf::six_dof::RigidBodyThrustProducerObject::ProcessInput(UtInput& aInput, TypeManager* aTypeManager)
{
   const std::string& command = aInput.GetCommand();
   UtInputBlock       block(aInput);

   ProcessInputType(aInput, aTypeManager);

   while (block.ReadCommand())
   {
      if (ThrustProducerObject::ProcessInput(aInput, aTypeManager))
      {
      }
      else if (command == "throttle_setting_mil")
      {
         std::string inputName;
         aInput.ReadValue(inputName);

         mThrottleSettingMIL         = ut::make_unique<ThrustControlInputValueData>();
         mThrottleSettingMIL->handle = 0;
         mThrottleSettingMIL->name   = inputName;
         mThrottleSettingMIL->value  = 0;
      }
      else if (command == "throttle_setting_ab")
      {
         std::string inputName;
         aInput.ReadValue(inputName);

         mThrottleSettingAB         = ut::make_unique<ThrustControlInputValueData>();
         mThrottleSettingAB->handle = 0;
         mThrottleSettingAB->name   = inputName;
         mThrottleSettingAB->value  = 0;
      }
      else if (command == "throttle_setting_reverser")
      {
         std::string inputName;
         aInput.ReadValue(inputName);

         mThrottleSettingReverser         = ut::make_unique<ThrustControlInputValueData>();
         mThrottleSettingReverser->handle = 0;
         mThrottleSettingReverser->name   = inputName;
         mThrottleSettingReverser->value  = 0;
      }
      else if (command == "throttle_setting_yaw")
      {
         std::string inputName;
         aInput.ReadValue(inputName);

         mThrottleSettingYaw         = ut::make_unique<ThrustControlInputValueData>();
         mThrottleSettingYaw->handle = 0;
         mThrottleSettingYaw->name   = inputName;
         mThrottleSettingYaw->value  = 0;

         // Since this is capable of thrust vectoring (TV), enable TV by default
         mThrustVectoringEnabled = true;
      }
      else if (command == "throttle_setting_pitch")
      {
         std::string inputName;
         aInput.ReadValue(inputName);

         mThrottleSettingPitch         = ut::make_unique<ThrustControlInputValueData>();
         mThrottleSettingPitch->handle = 0;
         mThrottleSettingPitch->name   = inputName;
         mThrottleSettingPitch->value  = 0;

         // Since this is capable of thrust vectoring (TV), enable TV by default
         mThrustVectoringEnabled = true;
      }
   }

   if (!mEnginePtr)
   {
      auto out = ut::log::error() << "Incomplete input within RigidBodyThrustProducerObject::ProcessInput().";
      out.AddNote() << "No engine definition found for thrust producer " << mObjName;

      return false;
   }

   return true;
}

void wsf::six_dof::RigidBodyThrustProducerObject::CalcCurrentThrustVectors()
{
   // The assumption is that maximum reverse thrust is deflected through 120 degrees,
   // and with sin(30deg)=0.5 we get 50% of full power deflected forward, which
   // implies a range of normalized thrust from 1.0 (forward) to -0.5 (reverse).
   // This means that the reverser can impart a range of 0.0 to -1.5 on the
   // thrustReverserForwardFactor value.

   // Set thrust reverser effects
   double thrustFactor = 1.0; // default value
   if (mThrustReverserEnabled)
   {
      // Set thrust factor using reverser setting
      thrustFactor = 1.0 - 1.5 * mThrustReverserSetting;
   }

   // Set thrust vectoring effects
   if (mThrustVectoringEnabled)
   {
      // The thrust vector may change with thrust vectoring. Calc the base
      // direction of thrust, using the engine's "installed" angles (YPR)
      UtVec3dX unitXVec(1., 0., 0.);
      UtDCM baseDCM(mThrustInstalledAnglesYPR_rad.X(), mThrustInstalledAnglesYPR_rad.Y(), mThrustInstalledAnglesYPR_rad.Z());

      // Now include the thrust vectoring
      UtDCM yawDCM(mThrustVectorYaw_rad, 0.0, 0.0);
      UtDCM pitchDCM(0.0, mThrustVectorPitch_rad, 0.0);
      UtDCM tempDCM = pitchDCM * yawDCM * baseDCM;

      // Set the current thrust vector that includes any vectoring
      mCurrentThrustVectorDirection = tempDCM.InverseTransform(unitXVec);
   }
   else
   {
      // With no vectoring, the current thrust vector is simply the nominal thrust vector
      mCurrentThrustVectorDirection = mNominalThrustVectorDirection;
   }

   // Set the current thrust factor
   mCurrentThrustFactor = thrustFactor;
}

void wsf::six_dof::RigidBodyThrustProducerObject::SetThrustReferenceOffsetAttitude(const UtVec3dX& aReferenceLocation_ft,
                                                                                   const UtVec3dX& aThrustYPR_rad)
{
   mReferenceLocation_ft         = aReferenceLocation_ft;
   mThrustInstalledAnglesYPR_rad = aThrustYPR_rad;

   // Adjust the thrust location for the engine offset. Calc the offset in
   // body coordinates, taking into account the YPR of the engine
   UtDCM tempDCM(mThrustInstalledAnglesYPR_rad.X(), mThrustInstalledAnglesYPR_rad.Y(), mThrustInstalledAnglesYPR_rad.Z());
   UtVec3dX offsetVec_ft(mThrustOffset_ft, 0., 0.);
   offsetVec_ft = tempDCM.InverseTransform(offsetVec_ft);

   // Calc the direction of thrust
   UtVec3dX unitXVec(1., 0., 0.);
   mNominalThrustVectorDirection = tempDCM.InverseTransform(unitXVec);

   // At start, the current thrust vector is the nominal thrust vector
   mCurrentThrustVectorDirection = mNominalThrustVectorDirection;

   // Add offset to the argument vector
   mThrustLocation_ft = aReferenceLocation_ft + offsetVec_ft;
}

void wsf::six_dof::RigidBodyThrustProducerObject::SetThrustVectoringYaw_deg(double aThrustVectoringYaw_deg)
{
   mThrustVectorYaw_rad = aThrustVectoringYaw_deg * UtMath::cRAD_PER_DEG;
}

void wsf::six_dof::RigidBodyThrustProducerObject::SetThrustVectoringPitch_deg(double aThrustVectoringPitch_deg)
{
   mThrustVectorPitch_rad = aThrustVectoringPitch_deg * UtMath::cRAD_PER_DEG;
}

double wsf::six_dof::RigidBodyThrustProducerObject::GetThrustVectoringYaw_deg() const
{
   return mThrustVectorYaw_rad * UtMath::cDEG_PER_RAD;
}

double wsf::six_dof::RigidBodyThrustProducerObject::GetThrustVectoringPitch_deg() const
{
   return mThrustVectorPitch_rad * UtMath::cDEG_PER_RAD;
}

void wsf::six_dof::RigidBodyThrustProducerObject::SetThrustReverserSetting(double aThrustReverserSetting)
{
   if (aThrustReverserSetting < 0)
   {
      mThrustReverserSetting = 0;
   }
   else if (aThrustReverserSetting > 1.0)
   {
      mThrustReverserSetting = 1.0;
   }
   else
   {
      mThrustReverserSetting = aThrustReverserSetting;
   }
}

void wsf::six_dof::RigidBodyThrustProducerObject::GetCurrentThrustVectorDirection(UtVec3dX& aDirection) const
{
   aDirection = mCurrentThrustVectorDirection;
}

UtVec3dX wsf::six_dof::RigidBodyThrustProducerObject::CalcThrustVectorFromThrust(double aThrust_lbs)
{
   UtVec3dX thrustVector_lbs = mCurrentThrustVectorDirection * aThrust_lbs;
   return thrustVector_lbs;
}

void wsf::six_dof::RigidBodyThrustProducerObject::SetControlInputHandle(const RigidBodyFlightControlSystem* aFlightControls,
                                                                        ThrustControlInputValueData* aControlInput)
{
   if (aControlInput != nullptr)
   {
      size_t handle = aFlightControls->GetControlValueHandle(aControlInput->name);
      if (handle == 0)
      {
         auto out = ut::log::warning() << "Unable to get a control value handle for input.";
         out.AddNote() << "Control Input: " << aControlInput->name;
      }
      aControlInput->handle = handle;
   }
}

void wsf::six_dof::RigidBodyThrustProducerObject::SetControlInputValue(const RigidBodyFlightControlSystem* aFlightControls,
                                                                       ThrustControlInputValueData* aControlInput)
{
   if (aControlInput != nullptr)
   {
      aControlInput->value = aFlightControls->GetControlValue(aControlInput->handle);
   }
}

void wsf::six_dof::RigidBodyThrustProducerObject::CalcThrustProducerFM(double                 aThrustMagnitude_lbs,
                                                                       ForceAndMomentsObject& aForceAndMoment)
{
   // Determine the thrust vector (unit vector in direction of thrust
   // in body coords, thrust location in body coords, and thrust
   // factor (due to any thrust reversing)
   CalcCurrentThrustVectors();

   bool negativeThrustCondition = false;
   if (aThrustMagnitude_lbs < 0.0)
   {
      negativeThrustCondition = true;
   }

   // If the thrust is negative (prior to considering any reverser),
   // this means that there is more drag on the engine than the thrust
   // it is producing. In this case, we do not use the reversing aspect
   // of "currentThrustFactor"
   if (!negativeThrustCondition)
   {
      // Include the effect of any thrust reversers (currentThrustFactor)
      aThrustMagnitude_lbs *= mCurrentThrustFactor;
   }

   // Calculate the thrust force vector
   UtVec3dX thrustForce_lbs = CalcThrustVectorFromThrust(aThrustMagnitude_lbs);

   // Right now, no direct moment is produced.
   // NOTE:  There are two considerations here:
   //  1) We are currently ignoring any moments produced by the engine
   //     such as rotational torques produces by a turbine (jet) engine.
   //  2) This moment is the "direct" moment produced by an
   //     engine -- it is *NOT* an induced (RxF) moment. The
   //     induced moment is calculated in the RigidBodyPropulsionSystem.

   // Set F&M
   aForceAndMoment.MoveRefPoint_ft(mThrustLocation_ft);
   aForceAndMoment.ClearForcesAndMoments();
   aForceAndMoment.AddForceAtReferencePoint(thrustForce_lbs);
}
