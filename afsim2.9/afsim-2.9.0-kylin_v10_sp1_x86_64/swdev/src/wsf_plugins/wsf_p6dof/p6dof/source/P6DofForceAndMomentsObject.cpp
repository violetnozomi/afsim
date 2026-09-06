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

#include "P6DofForceAndMomentsObject.hpp"

#include "UtDCM.hpp"

P6DofForceAndMomentsObject::P6DofForceAndMomentsObject()
   : mForceVec_lbs(0.0, 0.0, 0.0)
   , mMomentVec_ftlbs(0.0, 0.0, 0.0)
   , mRefPoint_ft(0.0, 0.0, 0.0)
{
}

P6DofForceAndMomentsObject* P6DofForceAndMomentsObject::Clone() const
{
   return new P6DofForceAndMomentsObject(*this);
}

P6DofForceAndMomentsObject& P6DofForceAndMomentsObject::operator+=(const P6DofForceAndMomentsObject& aSrc)
{
   UtVec3dX forceVec_lbs      = aSrc.mForceVec_lbs;
   UtVec3dX momentVec_ftlbs   = aSrc.mMomentVec_ftlbs;
   UtVec3dX specifiedPoint_ft = aSrc.mRefPoint_ft;

   AddForceAndMomentAtSpecifiedPoint(forceVec_lbs, momentVec_ftlbs, specifiedPoint_ft);

   return *this;
}

void P6DofForceAndMomentsObject::ClearForcesAndMoments()
{
   mForceVec_lbs.Set(0.0, 0.0, 0.0);
   mMomentVec_ftlbs.Set(0.0, 0.0, 0.0);
}

void P6DofForceAndMomentsObject::ClearReferenceAndForceAndMoment()
{
   mForceVec_lbs.Set(0.0, 0.0, 0.0);
   mMomentVec_ftlbs.Set(0.0, 0.0, 0.0);
   mRefPoint_ft.Set(0.0, 0.0, 0.0);
}

void P6DofForceAndMomentsObject::MoveRefPoint_ft(const UtVec3dX& aSpecifiedPoint_ft)
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

void P6DofForceAndMomentsObject::RelocateForce_ft(const UtVec3dX& aSpecifiedPoint_ft, const UtVec3dX& aAngRelToParentYPR_rad)
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


void P6DofForceAndMomentsObject::AddForceAndMomentAtSpecifiedPoint(UtVec3dX& aForceVec_lbs,
                                                                   UtVec3dX& aMomentVec_ftlbs,
                                                                   UtVec3dX& aSpecifiedPoint_ft)
{
   AddForceAtSpecifiedPoint(aForceVec_lbs, aSpecifiedPoint_ft);
   AddMoment(aMomentVec_ftlbs);
}

void P6DofForceAndMomentsObject::AddForceAndMomentAtReferencePoint(const UtVec3dX& aForceVec_lbs,
                                                                   const UtVec3dX& aMomentVec_ftlbs)
{
   mForceVec_lbs += aForceVec_lbs;
   mMomentVec_ftlbs += aMomentVec_ftlbs;
}

void P6DofForceAndMomentsObject::AddForceAtSpecifiedPoint(UtVec3dX& aForceVec_lbs, UtVec3dX& aSpecifiedPoint_ft)
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

void P6DofForceAndMomentsObject::AddForceAtReferencePoint(UtVec3dX& aForceVec_lbs)
{
   mForceVec_lbs += aForceVec_lbs;
}

void P6DofForceAndMomentsObject::AddMoment(UtVec3dX& aMomentVec_ftlbs)
{
   mMomentVec_ftlbs += aMomentVec_ftlbs;
}

void P6DofForceAndMomentsObject::GetForceAndMomentAtSpecifiedPoint(UtVec3dX& aForceVec_lbs,
                                                                   UtVec3dX& aMomentVec_ftlbs,
                                                                   UtVec3dX& aSpecifiedPoint_ft) const
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

void P6DofForceAndMomentsObject::GetForceAndMomentAtCurrentRefPoint(UtVec3dX& aForceVec_lbs, UtVec3dX& aMomentVec_ftlbs) const
{
   aForceVec_lbs    = mForceVec_lbs;
   aMomentVec_ftlbs = mMomentVec_ftlbs;
}

double P6DofForceAndMomentsObject::GetForceMagnitude_lbs() const
{
   return mForceVec_lbs.Magnitude();
}

void P6DofForceAndMomentsObject::LimitMaxForceMagnitude_lbs(double aMaxForce_lbs)
{
   double currentForceMagnitude_lbs = mForceVec_lbs.Magnitude();

   if (currentForceMagnitude_lbs > aMaxForce_lbs)
   {
      double factor = aMaxForce_lbs / currentForceMagnitude_lbs;

      mForceVec_lbs *= factor;
   }
}

void P6DofForceAndMomentsObject::LimitMomentMagnitude_ftlbs(double aMaxMoment_ftlbs)
{
   double currentMomentMagnitude_ftlbs = mMomentVec_ftlbs.Magnitude();

   if (currentMomentMagnitude_ftlbs > aMaxMoment_ftlbs)
   {
      double factor = aMaxMoment_ftlbs / currentMomentMagnitude_ftlbs;

      mMomentVec_ftlbs *= factor;
   }
}

UtVec3dX P6DofForceAndMomentsObject::GetForceAtRefPoint_lbs() const
{
   return mForceVec_lbs;
}

UtVec3dX P6DofForceAndMomentsObject::GetMomentAtRefPoint_ftlbs() const
{
   return mMomentVec_ftlbs;
}

P6DofForceAndMomentsObject P6DofForceAndMomentsObject::CalcAverageWith(P6DofForceAndMomentsObject& aOtherFM) const
{
   // Copy "this" to the output F&M
   P6DofForceAndMomentsObject outputFM = *this;

   // Add the "other" F&M to the output
   outputFM += aOtherFM;

   // Divide by 2 to get average value
   outputFM.mForceVec_lbs *= 0.5;
   outputFM.mMomentVec_ftlbs *= 0.5;

   return outputFM;
}

UtVec3dX P6DofForceAndMomentsObject::GetReferencePoint_ft() const
{
   return mRefPoint_ft;
}
