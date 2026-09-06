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

#include "WsfPropagatorTargetPoint.hpp"

#include "UtMath.hpp"
#include "UtOrbitalPropagatorBase.hpp"

namespace wsf
{
namespace space
{

PropagatorTargetPoint::PropagatorTargetPoint(const PropagatorTargetPoint& aOther)
   : mPropPtr{aOther.mPropPtr}
{
   mPropPtr->Initialize(aOther.mPropPtr->GetInitialOrbitalState().GetEpoch());
}

void PropagatorTargetPoint::Update(const UtCalendar& aEpoch)
{
   mPropPtr->Update(aEpoch);
}

ut::OrbitalStateVector PropagatorTargetPoint::GetOrbitalStateVectorInertial() const
{
   return mPropPtr->GetOrbitalState().GetOrbitalStateVectorInertial();
}

UtVec3d PropagatorTargetPoint::GetPositionECI() const
{
   return GetOrbitalStateVectorInertial().GetLocation();
}

UtVec3d PropagatorTargetPoint::GetVelocityECI() const
{
   return GetOrbitalStateVectorInertial().GetVelocity();
}

UtCalendar PropagatorTargetPoint::GetEpoch() const
{
   return mPropPtr->GetCurrentTime();
}

double PropagatorTargetPoint::GetCharacteristicTime() const
{
   return UtMath::cTWO_PI / mPropPtr->GetOrbitalState().GetOrbitalElements().GetMeanMotion();
}

} // namespace space
} // namespace wsf
