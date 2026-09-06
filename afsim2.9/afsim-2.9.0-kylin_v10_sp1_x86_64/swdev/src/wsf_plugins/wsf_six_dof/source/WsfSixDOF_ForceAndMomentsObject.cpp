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

#include "WsfSixDOF_ForceAndMomentsObject.hpp"

#include "UtDCM.hpp"

wsf::six_dof::ForceAndMomentsObject* wsf::six_dof::ForceAndMomentsObject::Clone() const
{
   return new ForceAndMomentsObject(*this);
}

wsf::six_dof::ForceAndMomentsObject& wsf::six_dof::ForceAndMomentsObject::operator+=(const ForceAndMomentsObject& aSrc)
{
   UtVec3dX forceVec_lbs      = aSrc.mForceVec_lbs;
   UtVec3dX momentVec_ftlbs   = aSrc.mMomentVec_ftlbs;
   UtVec3dX specifiedPoint_ft = aSrc.mRefPoint_ft;

   AddForceAndMomentAtSpecifiedPoint(forceVec_lbs, momentVec_ftlbs, specifiedPoint_ft);

   return *this;
}

void wsf::six_dof::ForceAndMomentsObject::ClearForcesAndMoments()
{
   mForceVec_lbs.Set(0.0, 0.0, 0.0);
   mMomentVec_ftlbs.Set(0.0, 0.0, 0.0);
}

void wsf::six_dof::ForceAndMomentsObject::ClearReferenceAndForceAndMoment()
{
   mForceVec_lbs.Set(0.0, 0.0, 0.0);
   mMomentVec_ftlbs.Set(0.0, 0.0, 0.0);
   mRefPoint_ft.Set(0.0, 0.0, 0.0);
}

void wsf::six_dof::ForceAndMomentsObject::MoveRefPoint_ft(const UtVec3dX& aSpecifiedPoint_ft)
{
   // Calculate the relative vector
   UtVec3dX relativeVec_ft = mRefPoint_ft - aSpecifiedPoint_ft;

   // Calculate the induced moment (which is RxF)
   UtVec3dX inducedMomentVec_ftlbs(0.0, 0.0, 0.0);
   inducedMomentVec_ftlbs = relativeVec_ft.Cross(mForceVec_lbs);

   // Add the induced moment to the current moment
   mMomentVec_ftlbs += inducedMomentVec_ftlbs;

   // The force is unchanged

   // Update the reference point
   mRefPoint_ft = aSpecifiedPoint_ft;
}

void wsf::six_dof::ForceAndMomentsObject::RelocateForce_ft(const UtVec3dX& aSpecifiedPoint_ft,
                                                           const UtVec3dX& aAngRelToParentYPR_rad)
{
   // Transform F&M vectors
   UtDCM    dcm(aAngRelToParentYPR_rad.X(), aAngRelToParentYPR_rad.Y(), aAngRelToParentYPR_rad.Z());
   UtVec3dX forceVec_lbs    = dcm.InverseTransform(mForceVec_lbs);
   UtVec3dX momentVec_ftlbs = dcm.InverseTransform(mMomentVec_ftlbs);

   mForceVec_lbs    = forceVec_lbs;
   mMomentVec_ftlbs = momentVec_ftlbs;

   // Update the reference point
   mRefPoint_ft = aSpecifiedPoint_ft;
}

void wsf::six_dof::ForceAndMomentsObject::AddForceAndMomentAtSpecifiedPoint(const UtVec3dX& aForceVec_lbs,
                                                                            const UtVec3dX& aMomentVec_ftlbs,
                                                                            const UtVec3dX& aSpecifiedPoint_ft)
{
   AddForceAtSpecifiedPoint(aForceVec_lbs, aSpecifiedPoint_ft);
   AddMoment(aMomentVec_ftlbs);
}

void wsf::six_dof::ForceAndMomentsObject::AddForceAndMomentAtReferencePoint(const UtVec3dX& aForceVec_lbs,
                                                                            const UtVec3dX& aMomentVec_ftlbs)
{
   mForceVec_lbs += aForceVec_lbs;
   mMomentVec_ftlbs += aMomentVec_ftlbs;
}

void wsf::six_dof::ForceAndMomentsObject::AddForceAtSpecifiedPoint(const UtVec3dX& aForceVec_lbs,
                                                                   const UtVec3dX& aSpecifiedPoint_ft)
{
   // Calculate the relative vector
   UtVec3dX relativeVec_ft = aSpecifiedPoint_ft - mRefPoint_ft;

   // Calculate the induced moment (which is RxF)
   UtVec3dX inducedMomentVec_ftlbs(0.0, 0.0, 0.0);
   inducedMomentVec_ftlbs = relativeVec_ft.Cross(aForceVec_lbs);

   // Add the induced moment to the current moment
   mMomentVec_ftlbs += inducedMomentVec_ftlbs;

   // Add the additional force to the current force
   mForceVec_lbs += aForceVec_lbs;

   // The reference point is unchanged
}

void wsf::six_dof::ForceAndMomentsObject::AddForceAtReferencePoint(const UtVec3dX& aForceVec_lbs)
{
   mForceVec_lbs += aForceVec_lbs;
}

void wsf::six_dof::ForceAndMomentsObject::AddMoment(const UtVec3dX& aMomentVec_ftlbs)
{
   mMomentVec_ftlbs += aMomentVec_ftlbs;
}

void wsf::six_dof::ForceAndMomentsObject::GetForceAndMomentAtSpecifiedPoint(UtVec3dX&       aForceVec_lbs,
                                                                            UtVec3dX&       aMomentVec_ftlbs,
                                                                            const UtVec3dX& aSpecifiedPoint_ft) const
{
   // Calculate the relative vector
   UtVec3dX relativeVec_ft = mRefPoint_ft - aSpecifiedPoint_ft;

   // Calculate the induced moment (which is RxF)
   UtVec3dX inducedMomentVec_ftlbs(0.0, 0.0, 0.0);
   inducedMomentVec_ftlbs = relativeVec_ft.Cross(aForceVec_lbs);

   // The force is the same, regardless of the reference point
   aForceVec_lbs = mForceVec_lbs;

   // The moment is the current moment plus the induced moment
   aMomentVec_ftlbs = mMomentVec_ftlbs + inducedMomentVec_ftlbs;
}

void wsf::six_dof::ForceAndMomentsObject::GetForceAndMomentAtCurrentRefPoint(UtVec3dX& aForceVec_lbs,
                                                                             UtVec3dX& aMomentVec_ftlbs) const
{
   aForceVec_lbs    = mForceVec_lbs;
   aMomentVec_ftlbs = mMomentVec_ftlbs;
}

double wsf::six_dof::ForceAndMomentsObject::GetForceMagnitude_lbs() const
{
   return mForceVec_lbs.Magnitude();
}

void wsf::six_dof::ForceAndMomentsObject::LimitMaxForceMagnitude_lbs(double aMaxForce_lbs)
{
   double currentForceMagnitude_lbs = mForceVec_lbs.Magnitude();

   if (currentForceMagnitude_lbs > aMaxForce_lbs)
   {
      double factor = aMaxForce_lbs / currentForceMagnitude_lbs;

      mForceVec_lbs *= factor;
   }
}

void wsf::six_dof::ForceAndMomentsObject::LimitMomentMagnitude_ftlbs(double aMaxMoment_ftlbs)
{
   double currentMomentMagnitude_ftlbs = mMomentVec_ftlbs.Magnitude();

   if (currentMomentMagnitude_ftlbs > aMaxMoment_ftlbs)
   {
      double factor = aMaxMoment_ftlbs / currentMomentMagnitude_ftlbs;

      mMomentVec_ftlbs *= factor;
   }
}

UtVec3dX wsf::six_dof::ForceAndMomentsObject::GetForceAtRefPoint_lbs() const
{
   return mForceVec_lbs;
}

UtVec3dX wsf::six_dof::ForceAndMomentsObject::GetMomentAtRefPoint_ftlbs() const
{
   return mMomentVec_ftlbs;
}

wsf::six_dof::ForceAndMomentsObject wsf::six_dof::ForceAndMomentsObject::CalcAverageWith(const ForceAndMomentsObject& aOtherFM) const
{
   // Copy "this" to the output F&M
   ForceAndMomentsObject outputFM = *this;

   // Add the "other" F&M to the output
   outputFM += aOtherFM;

   // Divide by 2 to get average value
   outputFM.mForceVec_lbs *= 0.5;
   outputFM.mMomentVec_ftlbs *= 0.5;

   return outputFM;
}

UtVec3dX wsf::six_dof::ForceAndMomentsObject::GetReferencePoint_ft() const
{
   return mRefPoint_ft;
}
