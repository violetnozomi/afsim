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

#ifndef WSFNTWOFFSETTARGETPOINT_HPP
#define WSFNTWOFFSETTARGETPOINT_HPP

#include "wsf_space_export.h"

#include <memory>

#include "UtOrbitalState.hpp"
#include "WsfKinematicOffsetTargetPoint.hpp"

namespace wsf
{
namespace space
{

//! A target point that adapts another to provide an offset in the NTW frame.
class WSF_SPACE_EXPORT NTW_OffsetTargetPoint : public KinematicOffsetTargetPoint
{
public:
   explicit NTW_OffsetTargetPoint(std::unique_ptr<OrbitalTargetPoint> aPointPtr)
      : KinematicOffsetTargetPoint(std::move(aPointPtr))
   {
   }

   NTW_OffsetTargetPoint(const NTW_OffsetTargetPoint& aOther) = default;
   ~NTW_OffsetTargetPoint() override                          = default;

   NTW_OffsetTargetPoint* Clone() const override { return new NTW_OffsetTargetPoint{*this}; }
   UtVec3d                GetPositionECI() const override;
   UtVec3d                GetVelocityECI() const override;

private:
   static UtVec3d TransformVectorFromNTW(const ut::OrbitalStateVector& aOriginState, const UtVec3d& aNTW_FrameVector);
};

} // namespace space
} // namespace wsf

#endif // WSFNTWOFFSETTARGETPOINT_HPP
