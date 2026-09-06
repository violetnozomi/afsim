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

#include "WsfPointMassSixDOF_ThrustProducerObject.hpp"

#include "UtDCM.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtInputFile.hpp"
#include "UtMath.hpp"
#include "WsfPointMassSixDOF_PropulsionSystem.hpp"
#include "WsfSixDOF_FuelTank.hpp"
#include "WsfSixDOF_TypeManager.hpp"

wsf::six_dof::PointMassThrustProducerObject::PointMassThrustProducerObject(const PointMassThrustProducerObject& aSrc)
   : ThrustProducerObject(aSrc)
   , mCurrentThrustFactor(aSrc.mCurrentThrustFactor)
   , mThrottleSettingMIL(aSrc.mThrottleSettingMIL)
   , mThrottleSettingAB(aSrc.mThrottleSettingAB)
   , mOmegaDotIncrementSlope_rps2_lbs(aSrc.mOmegaDotIncrementSlope_rps2_lbs)
{
}

void wsf::six_dof::PointMassThrustProducerObject::DeriveFrom(const ThrustProducerObject* aSrc)
{
   ThrustProducerObject::DeriveFrom(aSrc);

   auto pointMassThrustProducer = dynamic_cast<const PointMassThrustProducerObject*>(aSrc);
   if (pointMassThrustProducer == nullptr)
   {
      // Unknown type -- error since we can't derive from an unknown type
      auto out = ut::log::error() << "PointMassThrustProducerObject is trying to derive from a different class.";
      out.AddNote() << "Base Type: " << mBaseName;
      return;
   }

   mCurrentThrustFactor             = pointMassThrustProducer->mCurrentThrustFactor;
   mThrottleSettingMIL              = pointMassThrustProducer->mThrottleSettingMIL;
   mThrottleSettingAB               = pointMassThrustProducer->mThrottleSettingAB;
   mOmegaDotIncrementSlope_rps2_lbs = pointMassThrustProducer->mOmegaDotIncrementSlope_rps2_lbs;
}

wsf::six_dof::ThrustProducerObject* wsf::six_dof::PointMassThrustProducerObject::Clone() const
{
   return new PointMassThrustProducerObject(*this);
}

bool wsf::six_dof::PointMassThrustProducerObject::ProcessInput(UtInput& aInput, TypeManager* aTypeManager)
{
   UtInputBlock block(aInput);

   ProcessInputType(aInput, aTypeManager);

   while (block.ReadCommand())
   {
      if (ThrustProducerObject::ProcessInput(aInput, aTypeManager))
      {
      }
      else
      {
         auto out = ut::log::error() << "Unrecognized command within PointMassThrustProducerObject::ProcessInput().";
         out.AddNote() << "Location: " << aInput.GetLocation();
         throw UtInput::UnknownCommand(aInput);
      }
   }

   if (!mEnginePtr)
   {
      auto out = ut::log::error() << "Incomplete input within PointMassThrustProducerObject::ProcessInput().";
      out.AddNote() << "No engine definition found for thrust producer " << mObjName;

      return false;
   }

   return true;
}

bool wsf::six_dof::PointMassThrustProducerObject::ProcessTableInput(UtInput& aInput)
{
   const std::string& command = aInput.GetCommand();

   if (command == "maximum_roll_acceleration_at_thrust")
   {
      double omegaDot_rps2 = 0.0;
      double thrust_Nt     = 0.0;

      aInput.ReadValueOfType(omegaDot_rps2, UtInput::cANGULAR_ACCELERATION);
      aInput.ReadValueOfType(thrust_Nt, UtInput::cFORCE);
      aInput.ValueGreater(thrust_Nt, 0.0);

      double thrust_lbs = thrust_Nt * UtMath::cLB_PER_NT;

      mOmegaDotIncrementSlope_rps2_lbs.Set(0, omegaDot_rps2 / thrust_lbs);
   }
   else if (command == "maximum_pitch_acceleration_at_thrust")
   {
      double omegaDot_rps2 = 0.0;
      double thrust_Nt     = 0.0;

      aInput.ReadValueOfType(omegaDot_rps2, UtInput::cANGULAR_ACCELERATION);
      aInput.ReadValueOfType(thrust_Nt, UtInput::cFORCE);
      aInput.ValueGreater(thrust_Nt, 0.0);

      double thrust_lbs = thrust_Nt * UtMath::cLB_PER_NT;

      mOmegaDotIncrementSlope_rps2_lbs.Set(1, omegaDot_rps2 / thrust_lbs);
   }
   else if (command == "maximum_yaw_acceleration_at_thrust")
   {
      double omegaDot_rps2 = 0.0;
      double thrust_Nt     = 0.0;

      aInput.ReadValueOfType(omegaDot_rps2, UtInput::cANGULAR_ACCELERATION);
      aInput.ReadValueOfType(thrust_Nt, UtInput::cFORCE);
      aInput.ValueGreater(thrust_Nt, 0.0);

      double thrust_lbs = thrust_Nt * UtMath::cLB_PER_NT;

      mOmegaDotIncrementSlope_rps2_lbs.Set(2, omegaDot_rps2 / thrust_lbs);
   }
   else
   {
      return false;
   }

   return true;
}

void wsf::six_dof::PointMassThrustProducerObject::CalcCurrentThrustVectors()
{
   // Since this is exclusively an air-domain mover (no taxiing), we do not consider
   // effects of thrust reversers. Since this mover models thrust vectoring effects,
   // the thrust vectors are held static.
}

UtVec3dX wsf::six_dof::PointMassThrustProducerObject::CalcOmegaDotIncrementFromThrust(double aThrust_lbs)
{
   return mOmegaDotIncrementSlope_rps2_lbs * aThrust_lbs;
}

void wsf::six_dof::PointMassThrustProducerObject::SetThrustReferenceOffsetAttitude(const UtVec3dX& aReferenceLocation_ft,
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

   // Add offset to the argument vector
   mThrustLocation_ft = aReferenceLocation_ft + offsetVec_ft;
}

void wsf::six_dof::PointMassThrustProducerObject::GetCurrentThrustVectorDirection(UtVec3dX& aDirection) const
{
   aDirection = mNominalThrustVectorDirection;
}

UtVec3dX wsf::six_dof::PointMassThrustProducerObject::CalcThrustVectorFromThrust(double aThrust_lbs)
{
   UtVec3dX thrustVector_lbs = mNominalThrustVectorDirection * aThrust_lbs;
   return thrustVector_lbs;
}

void wsf::six_dof::PointMassThrustProducerObject::CalcThrustProducerFM(double                  aThrustMagnitude_lbs,
                                                                       ForceAndRotationObject& aForceAndMoment)
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

   // Calculate the increment to body rotation limits
   UtVec3dX maximumOmegaDot_rps2 = CalcOmegaDotIncrementFromThrust(aThrustMagnitude_lbs);

   // Set F&M
   aForceAndMoment.ClearForcesAndRotation();
   aForceAndMoment.AddForce(thrustForce_lbs);
   aForceAndMoment.AddMaximumAngularAcceleration_rps2(maximumOmegaDot_rps2);
}
