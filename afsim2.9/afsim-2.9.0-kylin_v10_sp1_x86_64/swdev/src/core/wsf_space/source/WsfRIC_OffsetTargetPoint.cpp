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

#include "WsfRIC_OffsetTargetPoint.hpp"

namespace wsf
{
namespace space
{

void RIC_OffsetTargetPoint::Update(const UtCalendar& aEpoch)
{
   KinematicOffsetTargetPoint::Update(aEpoch);
   auto baseState = GetBasePoint().GetOrbitalStateVectorInertial();
   mFrame.Update(baseState.GetLocation(), baseState.GetVelocity());
}

UtVec3d RIC_OffsetTargetPoint::GetPositionECI() const
{
   auto state = GetParentPoint().GetOrbitalStateVectorInertial();
   return state.GetLocation() + mFrame.TransformFromRIC(GetPositionOffset());
}

UtVec3d RIC_OffsetTargetPoint::GetVelocityECI() const
{
   UtVec3d ricpos = mFrame.PositionToRIC(GetPositionECI());
   return mFrame.VelocityFromRIC(ricpos, GetVelocityOffset());
}

} // namespace space
} // namespace wsf
