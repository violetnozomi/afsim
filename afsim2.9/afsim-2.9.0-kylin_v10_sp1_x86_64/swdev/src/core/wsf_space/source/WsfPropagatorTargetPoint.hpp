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

#ifndef WSFPROPAGATORTARGETPOINT_HPP
#define WSFPROPAGATORTARGETPOINT_HPP

#include "wsf_space_export.h"

#include <memory>

#include "UtCloneablePtr.hpp"
#include "UtOrbitalPropagatorBase.hpp"
#include "UtOrbitalState.hpp"
#include "WsfOrbitalTargetPoint.hpp"

namespace wsf
{
namespace space
{

//! A target point with kinematics given by a propagator.
class WSF_SPACE_EXPORT PropagatorTargetPoint : public OrbitalTargetPoint
{
public:
   explicit PropagatorTargetPoint(std::unique_ptr<UtOrbitalPropagatorBase> aPropPtr)
      : mPropPtr{std::move(aPropPtr)}
   {
   }

   PropagatorTargetPoint(const PropagatorTargetPoint& aOther);

   PropagatorTargetPoint& operator=(const PropagatorTargetPoint& aOther) = delete;

   ~PropagatorTargetPoint() override = default;

   PropagatorTargetPoint* Clone() const override { return new PropagatorTargetPoint{*this}; }
   void                   Update(const UtCalendar& aEpoch) override;
   ut::OrbitalStateVector GetOrbitalStateVectorInertial() const override;
   UtVec3d                GetPositionECI() const override;
   UtVec3d                GetVelocityECI() const override;
   UtCalendar             GetEpoch() const override;
   double                 GetCharacteristicTime() const override;

private:
   ut::CloneablePtr<UtOrbitalPropagatorBase> mPropPtr;
};

} // namespace space
} // namespace wsf

#endif // WSFPROPAGATORTARGETPOINT_HPP
