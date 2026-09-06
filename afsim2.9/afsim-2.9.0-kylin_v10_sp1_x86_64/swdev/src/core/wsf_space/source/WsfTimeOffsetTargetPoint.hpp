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

#ifndef WSFTIMEOFFSETTARGETPOINT_HPP
#define WSFTIMEOFFSETTARGETPOINT_HPP

#include "wsf_space_export.h"

#include "UtCloneablePtr.hpp"
#include "UtOrbitalPropagatorBase.hpp"
#include "WsfOrbitalTargetPoint.hpp"

namespace wsf
{
namespace space
{

//! A target point that predicts backward from a given target point.
//!
//! This target point will predict backward (or forward) from another target point,
//! called the base target, using a provided propagator. A space platform with
//! a propagator like the one used during this objects construction, and that has
//! achieved the kinematics of this target point, will reach the kinematics of the
//! base target at a fixed time in the future.
class WSF_SPACE_EXPORT TimeOffsetTargetPoint : public OrbitalTargetPoint
{
public:
   TimeOffsetTargetPoint(std::unique_ptr<OrbitalTargetPoint>      aTargetPoint,
                         std::unique_ptr<UtOrbitalPropagatorBase> aPropagator,
                         double                                   aTimeOffset);

   ~TimeOffsetTargetPoint() override = default;

   TimeOffsetTargetPoint* Clone() const override { return new TimeOffsetTargetPoint{*this}; }

   void                   Update(const UtCalendar& aEpoch) override;
   ut::OrbitalStateVector GetOrbitalStateVectorInertial() const override;
   UtVec3d                GetPositionECI() const override;
   UtVec3d                GetVelocityECI() const override;
   UtCalendar             GetEpoch() const override;
   double                 GetCharacteristicTime() const override;

   double GetTimeOffset() const { return mTimeOffset; }
   void   SetTimeOffset(double aTimeOffset) { mTimeOffset = aTimeOffset; }

private:
   void Retrodict();

   ut::CloneablePtr<OrbitalTargetPoint>      mTargetPoint; //! The base target point
   ut::CloneablePtr<UtOrbitalPropagatorBase> mPropagator;  //! Propagator used to predict motion
   double                                    mTimeOffset;  //! Offset time
};

} // namespace space
} // namespace wsf

#endif // WSFTIMEOFFSETTARGETPOINT_HPP
