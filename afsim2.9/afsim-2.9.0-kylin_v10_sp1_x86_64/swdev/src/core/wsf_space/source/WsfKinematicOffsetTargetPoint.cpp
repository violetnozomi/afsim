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

#include "WsfKinematicOffsetTargetPoint.hpp"

#include <stdexcept>

namespace wsf
{
namespace space
{
KinematicOffsetTargetPoint::KinematicOffsetTargetPoint(std::unique_ptr<OrbitalTargetPoint> aPointPtr)
   : mParentPtr{std::move(aPointPtr)}
{
   if (!mParentPtr)
   {
      throw std::runtime_error{"Attempt to form a position offset target from a null target"};
   }

   UpdateBase();
}

KinematicOffsetTargetPoint::KinematicOffsetTargetPoint(const KinematicOffsetTargetPoint& aOther)
   : mParentPtr{aOther.mParentPtr}
   , mPositionOffset{aOther.mPositionOffset}
   , mVelocityOffset{aOther.mVelocityOffset}
{
   UpdateBase();
}

void KinematicOffsetTargetPoint::Update(const UtCalendar& aEpoch)
{
   mParentPtr->Update(aEpoch);
}

ut::OrbitalStateVector KinematicOffsetTargetPoint::GetOrbitalStateVectorInertial() const
{
   return ut::OrbitalStateVector{GetPositionECI(), GetVelocityECI()};
}

UtCalendar KinematicOffsetTargetPoint::GetEpoch() const
{
   return mParentPtr->GetEpoch();
}

double KinematicOffsetTargetPoint::GetCharacteristicTime() const
{
   return mBasePtr->GetCharacteristicTime();
}

void KinematicOffsetTargetPoint::UpdateBase()
{
   auto parentKinematicOffsetPtr = dynamic_cast<const KinematicOffsetTargetPoint*>(mParentPtr.get());
   if (parentKinematicOffsetPtr != nullptr)
   {
      // If the parent is also a KinematicOffsetTargetPoint, use the parent's base as base here.
      mBasePtr = parentKinematicOffsetPtr->mBasePtr;
   }
   else
   {
      // Otherwise, use the parent as the base.
      mBasePtr = mParentPtr.get();
   }
}

} // namespace space
} // namespace wsf
