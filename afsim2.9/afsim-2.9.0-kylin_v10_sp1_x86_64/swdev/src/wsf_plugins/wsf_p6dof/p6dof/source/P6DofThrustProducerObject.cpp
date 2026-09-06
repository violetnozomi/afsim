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

#include "P6DofThrustProducerObject.hpp"

#include "P6DofFuelTank.hpp"
#include "P6DofPropulsionSystem.hpp"
#include "P6DofScenario.hpp"
#include "P6DofTypeManager.hpp"
#include "UtDCM.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtInputFile.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"

P6DofThrustProducerObject::P6DofThrustProducerObject(P6DofScenario* aScenario)
   : P6DofObject(aScenario)
   , mParentPropulsionSystemPtr(nullptr)
   , mTypeOfThrustProducer()
   , mNominalThrustVectorDirection(1., 0., 0.)
   , mCurrentThrustVectorDirection(1., 0., 0.)
   , mReferenceLocation_ft(0., 0., 0.)
   , mThrustLocation_ft(0., 0., 0.)
   , mCurrentThrustFM_lbs_ftlbs()
   , mThrustInstalledAnglesYPR_rad(0., 0., 0.)
   , mThrustOffset_ft(0)
   , mThrustVectoringEnabled(false)
   , mThrustVectorYaw_rad(0)
   , mThrustVectorPitch_rad(0)
   , mThrustReverserEnabled(0)
   , mThrustReverserSetting(0)
   , mShutdownInProgress(false)
   , mShutdownFraction_nanosec(0)
   , mIgniteTimeInFrame_nanosec(0)
   , mInOpRefAreaValid(false)
   , mInOpRefArea_ft2(0)
   , mMassPropertiesPtr(nullptr)
   , mCurrentThrustFactor(1)
   , mAfterburnerPresent(false)
   , mEngineOperating(false)
   , mEngineSmoking(false)
   , mAfterburnerOn(false)
   , mContrailing(false)
   , mProducingSmokeTrail(false)
   , mEngineDamageSmokeActivated(false)
{
}

P6DofThrustProducerObject::P6DofThrustProducerObject(const P6DofThrustProducerObject& aSrc)
   : P6DofObject(aSrc)
   , mParentPropulsionSystemPtr(aSrc.mParentPropulsionSystemPtr)
   , mTypeOfThrustProducer(aSrc.mTypeOfThrustProducer)
   , mNominalThrustVectorDirection(aSrc.mNominalThrustVectorDirection)
   , mCurrentThrustVectorDirection(aSrc.mCurrentThrustVectorDirection)
   , mReferenceLocation_ft(aSrc.mReferenceLocation_ft)
   , mThrustLocation_ft(aSrc.mThrustLocation_ft)
   , mCurrentThrustFM_lbs_ftlbs(aSrc.mCurrentThrustFM_lbs_ftlbs)
   , mThrustInstalledAnglesYPR_rad(aSrc.mThrustInstalledAnglesYPR_rad)
   , mThrustOffset_ft(aSrc.mThrustOffset_ft)
   , mThrustVectoringEnabled(aSrc.mThrustVectoringEnabled)
   , mThrustVectorYaw_rad(aSrc.mThrustVectorYaw_rad)
   , mThrustVectorPitch_rad(aSrc.mThrustVectorPitch_rad)
   , mThrustReverserEnabled(aSrc.mThrustReverserEnabled)
   , mThrustReverserSetting(aSrc.mThrustReverserSetting)
   , mShutdownInProgress(aSrc.mShutdownInProgress)
   , mShutdownFraction_nanosec(aSrc.mShutdownFraction_nanosec)
   , mIgniteTimeInFrame_nanosec(aSrc.mIgniteTimeInFrame_nanosec)
   , mInOpRefAreaValid(aSrc.mInOpRefAreaValid)
   , mInOpRefArea_ft2(aSrc.mInOpRefArea_ft2)
   , mMassPropertiesPtr(nullptr)
   , mCurrentThrustFactor(aSrc.mCurrentThrustFactor)
   , mAfterburnerPresent(aSrc.mAfterburnerPresent)
   , mEngineOperating(aSrc.mEngineOperating)
   , mEngineSmoking(aSrc.mEngineSmoking)
   , mAfterburnerOn(aSrc.mAfterburnerOn)
   , mContrailing(aSrc.mContrailing)
   , mProducingSmokeTrail(aSrc.mProducingSmokeTrail)
   , mEngineDamageSmokeActivated(aSrc.mEngineDamageSmokeActivated)
{
   if (aSrc.mMassPropertiesPtr != nullptr)
   {
      mMassPropertiesPtr = new P6DofMassProperties(*aSrc.mMassPropertiesPtr);
   }
}

P6DofThrustProducerObject::~P6DofThrustProducerObject()
{
   if (mMassPropertiesPtr != nullptr)
   {
      delete mMassPropertiesPtr;
   }
}

void P6DofThrustProducerObject::DeriveFrom(const P6DofThrustProducerObject* aSrc)
{
   if (aSrc != nullptr)
   {
      mTypeOfThrustProducer = aSrc->mTypeOfThrustProducer;
   }
}

void P6DofThrustProducerObject::SetParentPropulsionSystem(P6DofPropulsionSystem* aParent)
{
   mParentPropulsionSystemPtr = aParent;
}

double P6DofThrustProducerObject::GetThrust_lbs() const
{
   return mCurrentThrustFM_lbs_ftlbs.GetForceMagnitude_lbs();
}

double P6DofThrustProducerObject::GetForwardThrust_lbs() const
{
   return mCurrentThrustFM_lbs_ftlbs.GetForceAtRefPoint_lbs().X();
}

void P6DofThrustProducerObject::CalcCurrentThrustVectors()
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

const P6DofForceAndMomentsObject& P6DofThrustProducerObject::GetThrustForceAndAndMoment() const
{
   return mCurrentThrustFM_lbs_ftlbs;
}

void P6DofThrustProducerObject::SetThrustReferenceOffsetAttitude(const UtVec3dX& aReferenceLocation_ft,
                                                                 UtVec3dX&       aThrustYPR_rad)
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

void P6DofThrustProducerObject::SetThrustVectoringYaw_deg(double aThrustVectoringYaw_deg)
{
   mThrustVectorYaw_rad = aThrustVectoringYaw_deg * UtMath::cRAD_PER_DEG;
}

void P6DofThrustProducerObject::SetThrustVectoringPitch_deg(double aThrustVectoringPitch_deg)
{
   mThrustVectorPitch_rad = aThrustVectoringPitch_deg * UtMath::cRAD_PER_DEG;
}

double P6DofThrustProducerObject::GetThrustVectoringYaw_deg() const
{
   return mThrustVectorYaw_rad * UtMath::cDEG_PER_RAD;
}

double P6DofThrustProducerObject::GetThrustVectoringPitch_deg() const
{
   return mThrustVectorPitch_rad * UtMath::cDEG_PER_RAD;
}

void P6DofThrustProducerObject::SetThrustReverserSetting(double aThrustReverserSetting)
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

void P6DofThrustProducerObject::GetThrustLocation(UtVec3dX& aThrustLocation_ft) const
{
   aThrustLocation_ft = mThrustLocation_ft;
}

void P6DofThrustProducerObject::GetThrustInstalledAnglesYPR(UtVec3dX& aThrustYPR_rad) const
{
   aThrustYPR_rad = mThrustInstalledAnglesYPR_rad;
}

void P6DofThrustProducerObject::GetCurrentThrustVectorDirection(UtVec3dX& aDirection) const
{
   aDirection = mCurrentThrustVectorDirection;
}

UtVec3dX P6DofThrustProducerObject::CalcThrustVectorFromThrust(double aThrust_lbs)
{
   UtVec3dX thrustVector_lbs = mCurrentThrustVectorDirection * aThrust_lbs;
   return thrustVector_lbs;
}


void P6DofThrustProducerObject::SetControlInputHandle(const P6DofFlightControlSystem* aFlightControls,
                                                      ThrustControlInputValueData*    aControlInput)
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

void P6DofThrustProducerObject::SetControlInputValue(const P6DofFlightControlSystem* aFlightControls,
                                                     ThrustControlInputValueData*    aControlInput)
{
   if (aControlInput != nullptr)
   {
      aControlInput->value = aFlightControls->GetControlValue(aControlInput->handle);
   }
}

void P6DofThrustProducerObject::CalcThrustProducerFM(double aThrustMagnitude_lbs, P6DofForceAndMomentsObject& aForceAndMoment)
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
   //     induced moment is calculated in the P6DofPropulsionSystem.

   // Set F&M
   aForceAndMoment.MoveRefPoint_ft(mThrustLocation_ft);
   aForceAndMoment.ClearForcesAndMoments();
   aForceAndMoment.AddForceAtReferencePoint(thrustForce_lbs);
}

void P6DofThrustProducerObject::MakeEngineSmoke(bool aSmoking)
{
   mEngineDamageSmokeActivated = aSmoking;
}
