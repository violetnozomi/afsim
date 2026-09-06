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

#ifndef WSFRICOFFSETTARGETPOINT_HPP
#define WSFRICOFFSETTARGETPOINT_HPP

#include "wsf_space_export.h"

#include <memory>

#include "UtCloneablePtr.hpp"
#include "UtRIC_Frame.hpp"
#include "WsfKinematicOffsetTargetPoint.hpp"

namespace wsf
{
namespace space
{

//! A target point that adapts another to provide an offset in the RIC frame.
class WSF_SPACE_EXPORT RIC_OffsetTargetPoint : public KinematicOffsetTargetPoint
{
public:
   explicit RIC_OffsetTargetPoint(std::unique_ptr<OrbitalTargetPoint> aPointPtr)
      : KinematicOffsetTargetPoint(std::move(aPointPtr))
      , mFrame{UtVec3d{1.0, 0.0, 0.0}, UtVec3d{0.0, 1.0, 0.0}}
   {
   }

   RIC_OffsetTargetPoint(const RIC_OffsetTargetPoint& aOther) = default;
   ~RIC_OffsetTargetPoint() override                          = default;

   void Update(const UtCalendar& aEpoch) override;

   RIC_OffsetTargetPoint* Clone() const override { return new RIC_OffsetTargetPoint{*this}; }
   UtVec3d                GetPositionECI() const override;
   UtVec3d                GetVelocityECI() const override;

private:
   ut::RIC_Frame mFrame;
};

} // namespace space
} // namespace wsf

#endif // WSFRICOFFSETTARGETPOINT_HPP
