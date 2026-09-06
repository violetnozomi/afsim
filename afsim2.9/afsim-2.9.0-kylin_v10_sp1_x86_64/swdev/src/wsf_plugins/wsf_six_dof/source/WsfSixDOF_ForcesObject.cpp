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

#include "WsfSixDOF_ForcesObject.hpp"

#include "UtDCM.hpp"

wsf::six_dof::ForcesObject* wsf::six_dof::ForcesObject::Clone() const
{
   return new ForcesObject(*this);
}

wsf::six_dof::ForcesObject& wsf::six_dof::ForcesObject::operator+=(const ForcesObject& aSrc)
{
   UtVec3dX forceVec_lbs = aSrc.mForceVec_lbs;

   AddForce(forceVec_lbs);

   return *this;
}

void wsf::six_dof::ForcesObject::ClearForces()
{
   mForceVec_lbs.Set(0.0, 0.0, 0.0);
}

void wsf::six_dof::ForcesObject::MoveRefPoint_ft(const UtVec3dX& aAngRelToParentYPR_rad)
{
   // Transform F&M vectors
   UtDCM    dcm(aAngRelToParentYPR_rad.X(), aAngRelToParentYPR_rad.Y(), aAngRelToParentYPR_rad.Z());
   UtVec3dX newForceVec_lbs = dcm.InverseTransform(mForceVec_lbs);

   // Set the new force
   mForceVec_lbs = newForceVec_lbs;
}

void wsf::six_dof::ForcesObject::AddForce(const UtVec3dX& aForceVec_lbs)
{
   mForceVec_lbs += aForceVec_lbs;
}

void wsf::six_dof::ForcesObject::GetForce(UtVec3dX& aForceVec_lbs) const
{
   aForceVec_lbs = mForceVec_lbs;
}

double wsf::six_dof::ForcesObject::GetForceMagnitude_lbs() const
{
   return mForceVec_lbs.Magnitude();
}

void wsf::six_dof::ForcesObject::LimitMaxForceMagnitude_lbs(double aMaxForce_lbs)
{
   double currentForceMagnitude_lbs = mForceVec_lbs.Magnitude();

   if (currentForceMagnitude_lbs > aMaxForce_lbs)
   {
      double factor = aMaxForce_lbs / currentForceMagnitude_lbs;

      mForceVec_lbs *= factor;
   }
}

UtVec3dX wsf::six_dof::ForcesObject::GetForce_lbs() const
{
   return mForceVec_lbs;
}

wsf::six_dof::ForcesObject wsf::six_dof::ForcesObject::CalcAverageWith(const ForcesObject& aOtherFM) const
{
   // Copy "this" to the output F&M
   ForcesObject outputFM = *this;

   // Add the "other" F&M to the output
   outputFM += aOtherFM;

   // Divide by 2 to get average value
   outputFM.mForceVec_lbs *= 0.5;

   return outputFM;
}
