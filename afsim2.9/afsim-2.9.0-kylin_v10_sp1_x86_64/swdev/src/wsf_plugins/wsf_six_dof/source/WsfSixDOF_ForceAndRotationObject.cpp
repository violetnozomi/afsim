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

#include "WsfSixDOF_ForceAndRotationObject.hpp"

#include "UtDCM.hpp"

wsf::six_dof::ForceAndRotationObject* wsf::six_dof::ForceAndRotationObject::Clone() const
{
   return new ForceAndRotationObject(*this);
}

wsf::six_dof::ForceAndRotationObject& wsf::six_dof::ForceAndRotationObject::operator+=(const ForceAndRotationObject& aSrc)
{
   AddForce(aSrc.mForceVec_lbs);
   AddMaximumAngularAcceleration_rps2(aSrc.mMaximumOmegaDot_rps2);
   AddStabilizingFrequency_rps(aSrc.mStabilizingFrequency_rps);

   return *this;
}

void wsf::six_dof::ForceAndRotationObject::ClearForcesAndRotation()
{
   mForceVec_lbs.Set(0.0);
   mMaximumOmegaDot_rps2.Set(0.0);
   mStabilizingFrequency_rps.Set(0.0);
}

void wsf::six_dof::ForceAndRotationObject::ReorientForce_ft(const UtVec3dX& aAngRelToParentYPR_rad)
{
   // Transform F&M vectors
   UtDCM    dcm(aAngRelToParentYPR_rad.X(), aAngRelToParentYPR_rad.Y(), aAngRelToParentYPR_rad.Z());
   UtVec3dX forceVec_lbs = dcm.InverseTransform(mForceVec_lbs);

   mForceVec_lbs = forceVec_lbs;
}

void wsf::six_dof::ForceAndRotationObject::AddForce(const UtVec3dX& aForceVec_lbs)
{
   // Add the additional force to the current force
   mForceVec_lbs += aForceVec_lbs;
}

double wsf::six_dof::ForceAndRotationObject::GetForceMagnitude_lbs() const
{
   return mForceVec_lbs.Magnitude();
}

void wsf::six_dof::ForceAndRotationObject::LimitMaxForceMagnitude_lbs(double aMaxForce_lbs)
{
   double currentForceMagnitude_lbs = mForceVec_lbs.Magnitude();

   if (currentForceMagnitude_lbs > aMaxForce_lbs)
   {
      double factor = aMaxForce_lbs / currentForceMagnitude_lbs;

      mForceVec_lbs *= factor;
   }
}

UtVec3dX wsf::six_dof::ForceAndRotationObject::GetForce_lbs() const
{
   return mForceVec_lbs;
}

wsf::six_dof::ForceAndRotationObject
wsf::six_dof::ForceAndRotationObject::CalcAverageWith(const ForceAndRotationObject& aOtherFM) const
{
   // Copy "this" to the output F&M
   ForceAndRotationObject outputFM = *this;

   // Add the "other" F&M to the output
   outputFM += aOtherFM;

   // Divide by 2 to get average value
   outputFM.mForceVec_lbs *= 0.5;
   outputFM.mMaximumOmegaDot_rps2 *= 0.5;
   outputFM.mStabilizingFrequency_rps *= 0.5;

   return outputFM;
}
