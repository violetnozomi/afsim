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

#ifndef WSFLIBRATIONTARGETPOINT_HPP
#define WSFLIBRATIONTARGETPOINT_HPP

#include "wsf_space_export.h"

#include "WsfLibrationPoint.hpp"
#include "WsfOrbitalTargetPoint.hpp"

namespace wsf
{
namespace space
{

//! This orbital target point is used for targeting libration points.
class WSF_SPACE_EXPORT LibrationTargetPoint : public OrbitalTargetPoint
{
public:
   LibrationTargetPoint(LibrationPoint::System aSystem, LibrationPoint::Point aPoint);
   ~LibrationTargetPoint() override = default;

   LibrationTargetPoint* Clone() const override { return new LibrationTargetPoint{*this}; }

   void                   Update(const UtCalendar& aEpoch) override;
   ut::OrbitalStateVector GetOrbitalStateVectorInertial() const override;
   UtVec3d                GetPositionECI() const override;
   UtVec3d                GetVelocityECI() const override;
   UtCalendar             GetEpoch() const override { return mEpoch; }
   double                 GetCharacteristicTime() const override;

private:
   LibrationPoint        mPoint;
   LibrationPoint::Point mSelectedPoint;
   UtCalendar            mEpoch;
};

} // namespace space
} // namespace wsf

#endif // WSFLIBRATIONTARGETPOINT_HPP
