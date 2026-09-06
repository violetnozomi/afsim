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

#include "WsfLibrationTargetPoint.hpp"

namespace wsf
{
namespace space
{

LibrationTargetPoint::LibrationTargetPoint(LibrationPoint::System aSystem, LibrationPoint::Point aPoint)
   : mPoint{aSystem}
   , mSelectedPoint{aPoint}
   , mEpoch{}
{
   mEpoch.SetJulianDate(0.0);
}

void LibrationTargetPoint::Update(const UtCalendar& aEpoch)
{
   mEpoch = aEpoch;
}

ut::OrbitalStateVector LibrationTargetPoint::GetOrbitalStateVectorInertial() const
{
   return ut::OrbitalStateVector{mPoint.GetPositionECI(mSelectedPoint, mEpoch),
                                 mPoint.GetVelocityECI(mSelectedPoint, mEpoch)};
}

UtVec3d LibrationTargetPoint::GetPositionECI() const
{
   return mPoint.GetPositionECI(mSelectedPoint, mEpoch);
}

UtVec3d LibrationTargetPoint::GetVelocityECI() const
{
   return mPoint.GetVelocityECI(mSelectedPoint, mEpoch);
}

double LibrationTargetPoint::GetCharacteristicTime() const
{
   return mPoint.GetSystemPeriod();
}

} // namespace space
} // namespace wsf
