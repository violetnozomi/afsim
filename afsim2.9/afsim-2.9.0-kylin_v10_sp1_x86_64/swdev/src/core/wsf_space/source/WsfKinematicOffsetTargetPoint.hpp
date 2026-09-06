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

#ifndef WSFPOSITIONOFFSETTARGETPOINT_HPP
#define WSFPOSITIONOFFSETTARGETPOINT_HPP

#include "wsf_space_export.h"

#include <memory>

#include "UtCloneablePtr.hpp"
#include "UtOrbitalState.hpp"
#include "WsfOrbitalTargetPoint.hpp"

namespace wsf
{
namespace space
{
//! A target point that adapts another to provide an offset in a particular frame.
class WSF_SPACE_EXPORT KinematicOffsetTargetPoint : public OrbitalTargetPoint
{
public:
   explicit KinematicOffsetTargetPoint(std::unique_ptr<OrbitalTargetPoint> aPointPtr);
   KinematicOffsetTargetPoint(const KinematicOffsetTargetPoint& aOther);
   KinematicOffsetTargetPoint& operator=(const KinematicOffsetTargetPoint& aOther) = delete;
   ~KinematicOffsetTargetPoint() override                                          = default;

   void                   Update(const UtCalendar& aEpoch) override;
   ut::OrbitalStateVector GetOrbitalStateVectorInertial() const override;
   UtCalendar             GetEpoch() const override;
   double                 GetCharacteristicTime() const override;

   //! Set the position offset in meters.
   void SetPositionOffset(const UtVec3d& aPositionOffset) { mPositionOffset = aPositionOffset; }

   //! Get the position offset in meters.
   const UtVec3d& GetPositionOffset() const { return mPositionOffset; }

   //! Set the velocity offset in meters per second.
   void SetVelocityOffset(const UtVec3d& aVelocityOffset) { mVelocityOffset = aVelocityOffset; }

   //! Get the velocity offset in meters per second.
   const UtVec3d& GetVelocityOffset() const { return mVelocityOffset; }

   //! Get the point from to which the offset is added.
   const OrbitalTargetPoint& GetParentPoint() const { return *mParentPtr; }

   //! Get the point from which the reference frame is computed.
   const OrbitalTargetPoint& GetBasePoint() const { return *mBasePtr; }

private:
   void UpdateBase();

   // The parent pointer is OrbitalTargetPoint that provides the position and velocity
   // off of which this kinematic offset is computed, but the mBasePtr provides the
   // kinematics of the object for computing the frame. This split allows for multiple
   // kinematic offsets to be added onto a single target that use frames consistent with
   // the original target.

   ut::CloneablePtr<OrbitalTargetPoint> mParentPtr{nullptr};
   const OrbitalTargetPoint*            mBasePtr{nullptr};
   UtVec3d                              mPositionOffset{};
   UtVec3d                              mVelocityOffset{};
};

} // namespace space
} // namespace wsf

#endif // WSFPOSITIONOFFSETTARGETPOINT_HPP
