// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfSixDOF_ManeuverTurnCircle.hpp"

#include <limits>

namespace
{
constexpr double cTURN_CIRCLE_ACC_LIMIT = 1.0e-6;
}

wsf::six_dof::ManeuverTurnCircle::ManeuverTurnCircle(const UtVec3d& aLocationWCS,
                                                     const UtVec3d& aVelocityWCS,
                                                     const UtVec3d& aAccelerationWCS)
   : mCenter{0.0, 0.0, 0.0}
   , mX_Hat{0.0, 0.0, 0.0}
   , mY_Hat{0.0, 0.0, 0.0}
   , mRadius_m{std::numeric_limits<double>::max()}
   , mSpeed_mps{0.0}
   , mAngSpeed_radps{0.0}
{
   // By construction, the velocity vector will be along the y hat direction.
   mY_Hat = aVelocityWCS.GetNormal();

   // 1. Get the radius
   double  accParallelMag = UtVec3d::DotProduct(mY_Hat, aAccelerationWCS);
   UtVec3d accParallel    = mY_Hat * accParallelMag;
   UtVec3d accRadial      = aAccelerationWCS - accParallel;
   double  accRadialMag   = accRadial.Magnitude();

   // If the radial acceleration is too small, we don't continue to avoid
   // division by a small quantity. If this branch is not entered, this object
   // will return false from any subsequent calls to IsValid.
   if (accRadialMag > cTURN_CIRCLE_ACC_LIMIT)
   {
      mRadius_m       = aVelocityWCS.MagnitudeSquared() / accRadialMag;
      mSpeed_mps      = aVelocityWCS.Magnitude();
      mAngSpeed_radps = mSpeed_mps / mRadius_m;

      // 2. Get the center
      UtVec3d rHat = accRadial.GetNormal();
      mCenter      = aLocationWCS + rHat * mRadius_m;

      // 3. Finish the frame
      mX_Hat = -rHat;
   }
}

//! Return the location on the circle a given phase away from the reference point.
//!
//! Return the WCS location of the point on the turn circle that is a @p aPhase
//! advanced from the reference point of the circle. The given @p aPhase can be of
//! either sign. To return the reference point, call this method with a @p aPhase
//! of zero.
//!
//! \param aPhase - An angle in radians advanced from the reference point at which to get the location.
//! \return       - The WCS location of the requested point if this object is valid; a default vector otherwise.
UtVec3d wsf::six_dof::ManeuverTurnCircle::GetLocationOnCircle(double aPhase) const
{
   UtVec3d retval{};
   if (IsValid())
   {
      retval = mCenter + mX_Hat * cos(aPhase) * mRadius_m + mY_Hat * sin(aPhase) * mRadius_m;
   }
   return retval;
}

//! Return the location on the circle a given time offset from the reference point.
//!
//! Return the WCS location of the point on the turn circle that is @p aDeltaT advanced
//! from the reference point of the circle at the angular rate implied by the kinematics
//! used to construct this object. To return the reference point, call this method with
//! a @p aDeltaT of zero.
//!
//! \param aDeltaT - The time ahead (or behind) the reference point at which to get the location.
//! \return        - The WCS location of the requested point if this object is valid; a default vector otherwise.
UtVec3d wsf::six_dof::ManeuverTurnCircle::GetLocationAfterTime(double aDeltaT) const
{
   return GetLocationOnCircle(mAngSpeed_radps * aDeltaT);
}

//! Return the angular velocity of the reference point.
//!
//! Returns the angular velocity as a vector in the WCS frame.
//!
//! \return - The WCS angular velocity for the turn circle.
UtVec3d wsf::six_dof::ManeuverTurnCircle::GetAngularVelocity() const
{
   UtVec3d zHat{};
   zHat.CrossProduct(mX_Hat, mY_Hat);
   zHat *= mAngSpeed_radps;
   return zHat;
}

//! Return the velocity on the circle a given phase away from the reference point.
//!
//! Return the WCS velocity that would result from uniform circular motion of the reference
//! point around the turn circle. The given @p aPhase can be of either sign.
//!
//! \param aPhase - An angle in radians advanced from the reference point at which to get the velocity.
//! \return       - The WCS velocity at the requested point.
UtVec3d wsf::six_dof::ManeuverTurnCircle::GetVelocityOnCircle(double aPhase) const
{
   UtVec3d retval{};
   if (IsValid())
   {
      retval = -mX_Hat * sin(aPhase) * mSpeed_mps + mY_Hat * cos(aPhase) * mSpeed_mps;
   }
   return retval;
}

//! Return if the turn circle is valid.
//!
//! A turn circle will fail to be valid after construction if the radial acceleration is
//! small.
bool wsf::six_dof::ManeuverTurnCircle::IsValid() const
{
   return mRadius_m < std::numeric_limits<double>::max();
}
