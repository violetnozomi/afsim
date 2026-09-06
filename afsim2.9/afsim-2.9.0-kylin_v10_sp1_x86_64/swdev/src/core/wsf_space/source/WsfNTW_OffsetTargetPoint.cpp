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

#include "WsfNTW_OffsetTargetPoint.hpp"

#include "WsfSpaceOrientation.hpp"

namespace wsf
{
namespace space
{

UtVec3d NTW_OffsetTargetPoint::GetPositionECI() const
{
   auto state     = GetParentPoint().GetOrbitalStateVectorInertial();
   auto baseState = GetBasePoint().GetOrbitalStateVectorInertial();
   return state.GetLocation() + TransformVectorFromNTW(baseState, GetPositionOffset());
}

UtVec3d NTW_OffsetTargetPoint::GetVelocityECI() const
{
   auto state     = GetParentPoint().GetOrbitalStateVectorInertial();
   auto baseState = GetBasePoint().GetOrbitalStateVectorInertial();
   return state.GetVelocity() + TransformVectorFromNTW(baseState, GetVelocityOffset());
}

UtVec3d NTW_OffsetTargetPoint::TransformVectorFromNTW(const ut::OrbitalStateVector& aOriginState,
                                                      const UtVec3d&                aNTW_FrameVector)
{
   UtVec3d xP;
   UtVec3d yP;
   UtVec3d zP;
   UtVec3d nadir = -1.0 * aOriginState.GetLocation();

   // Vallado defines the so-called "NTW" coordinate system, where x/T is in the direction of the
   // target's motion and the z/(-N) axis is constrained to the nadir
   // (Fundamentals of Astrodynamics and Applications, 4th ed., p.389).  This is the system
   // we use for our offsets as well (x==T, y==-W, z==-N).
   // For the default orientation in AFSIM, this will also be the target's ECS coordinate system.
   Orientation::CalculatePCS_DirectionCosines(aOriginState.GetVelocity(), nadir, xP, yP, zP);
   double  Tntw_eci[3][3] = {{xP[0], yP[0], zP[0]}, {xP[1], yP[1], zP[1]}, {xP[2], yP[2], zP[2]}};
   UtVec3d eciVec;
   UtMat3d::Transform(eciVec.GetData(), Tntw_eci, aNTW_FrameVector.GetData());

   return eciVec;
}

} // namespace space
} // namespace wsf
