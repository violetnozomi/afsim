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

#ifndef WSFSPACEKINEMATICSTATETARGETPOINT_HPP
#define WSFSPACEKINEMATICSTATETARGETPOINT_HPP

#include "wsf_space_export.h"

#include "UtVec3.hpp"
#include "WsfOrbitalTargetPoint.hpp"

namespace wsf
{
namespace space
{

//! A target point at a fixed inertial position and velocity.
class WSF_SPACE_EXPORT KinematicStateTargetPoint : public OrbitalTargetPoint
{
public:
   KinematicStateTargetPoint(const UtVec3d& aPositionECI, const UtVec3d& aVelocityECI)
      : OrbitalTargetPoint{}
      , mInertialState(aPositionECI, aVelocityECI)
   {
   }
   KinematicStateTargetPoint(const KinematicStateTargetPoint& aOther) = default;
   ~KinematicStateTargetPoint() override                              = default;

   KinematicStateTargetPoint* Clone() const override { return new KinematicStateTargetPoint{*this}; }
   void                       Update(const UtCalendar& aEpoch) override { mEpoch = aEpoch; }
   ut::OrbitalStateVector     GetOrbitalStateVectorInertial() const override { return mInertialState; }
   UtVec3d                    GetPositionECI() const override { return mInertialState.GetLocation(); }
   UtVec3d                    GetVelocityECI() const override { return mInertialState.GetVelocity(); }
   UtCalendar                 GetEpoch() const override { return mEpoch; }

   double GetCharacteristicTime() const override;

private:
   ut::OrbitalStateVector mInertialState;
   UtCalendar             mEpoch{};
};

} // namespace space
} // namespace wsf

#endif // WSFSPACEKINEMATICSTATETARGETPOINT_HPP
