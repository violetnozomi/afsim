// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfTimeOffsetTargetPoint.hpp"

#include <stdexcept>

namespace wsf
{
namespace space
{

TimeOffsetTargetPoint::TimeOffsetTargetPoint(std::unique_ptr<OrbitalTargetPoint>      aTargetPoint,
                                             std::unique_ptr<UtOrbitalPropagatorBase> aPropagator,
                                             double                                   aTimeOffset)
   : mTargetPoint{std::move(aTargetPoint)}
   , mPropagator{std::move(aPropagator)}
   , mTimeOffset{aTimeOffset}
{
   if (mTargetPoint == nullptr)
   {
      throw std::runtime_error{"Null target point in TimeOffsetTargetPoint"};
   }
   if (mPropagator == nullptr)
   {
      throw std::runtime_error{"Null propagator in TimeOffsetTargetPoint"};
   }
}

void TimeOffsetTargetPoint::Update(const UtCalendar& aEpoch)
{
   mTargetPoint->Update(aEpoch);
   Retrodict();
}

ut::OrbitalStateVector TimeOffsetTargetPoint::GetOrbitalStateVectorInertial() const
{
   return mPropagator->GetOrbitalState().GetOrbitalStateVectorInertial();
}

UtVec3d TimeOffsetTargetPoint::GetPositionECI() const
{
   return GetOrbitalStateVectorInertial().GetLocation();
}

UtVec3d TimeOffsetTargetPoint::GetVelocityECI() const
{
   return GetOrbitalStateVectorInertial().GetVelocity();
}

UtCalendar TimeOffsetTargetPoint::GetEpoch() const
{
   return mTargetPoint->GetEpoch();
}

double TimeOffsetTargetPoint::GetCharacteristicTime() const
{
   return mTargetPoint->GetCharacteristicTime();
}

void TimeOffsetTargetPoint::Retrodict()
{
   // Initialize propagator to current epoch and conditions
   UtCalendar       epoch = GetEpoch();
   ut::OrbitalState state = mPropagator->GetOrbitalState();
   state.SetOrbitalStateVector(mTargetPoint->GetOrbitalStateVectorInertial());
   state.SetEpoch(epoch);
   mPropagator->SetInitialOrbitalState(state);
   mPropagator->Initialize(epoch);

   // Propagate backward by mTimeOffset seconds
   epoch.AdvanceTimeBy(-mTimeOffset);
   mPropagator->Update(epoch);
}

} // namespace space
} // namespace wsf
