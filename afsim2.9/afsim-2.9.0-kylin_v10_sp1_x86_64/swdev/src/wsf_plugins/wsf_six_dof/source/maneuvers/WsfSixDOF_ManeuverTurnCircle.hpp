// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSF_SIXDOF_MANEUVERTURNCIRCLE_HPP
#define WSF_SIXDOF_MANEUVERTURNCIRCLE_HPP

#include "UtVec3.hpp"

namespace wsf
{
namespace six_dof
{
//! This class represents the geometry of a turn circle for an aircraft.
//!
//! Though this is intended to be used with reference to a specific platform, there is
//! nothing in the implementation of this class that relies on that connection. Instead,
//! the basic kinematic data are used to compute what would be the turn circle for an
//! entity with the given kinematics. The location of the entity provided to the constructor
//! is taken to be the reference point on the circle. This class can also compute other
//! locations on the turn circle, either as a phase angle relative to the reference point,
//! or as a time relative to the time at which the turn circle was created.
class ManeuverTurnCircle
{
public:
   ManeuverTurnCircle(const UtVec3d& aLocationWCS, const UtVec3d& aVelocityWCS, const UtVec3d& aAccelerationWCS);

   UtVec3d        GetLocationOnCircle(double aPhase) const;
   UtVec3d        GetLocationAfterTime(double aDeltaT) const;
   UtVec3d        GetVelocityOnCircle(double aPhase) const;
   const UtVec3d& GetCenter() const { return mCenter; }
   double         GetRadiusMeters() const { return mRadius_m; }
   double         GetSpeedMetersPerSecond() const { return mSpeed_mps; }
   double         GetAngularSpeedRadiansPerSecond() const { return mAngSpeed_radps; }
   UtVec3d        GetAngularVelocity() const;

   bool IsValid() const;

private:
   UtVec3d mCenter;         //! The WCS position of the center of the circle
   UtVec3d mX_Hat;          //! A unit vector pointing from the center to the turning object's position.
   UtVec3d mY_Hat;          //! A unit vector 90 degrees advanced from mX_Hat in the direction of the object's motion.
   double  mRadius_m;       //! The radius of the circle [m].
   double  mSpeed_mps;      //! The speed of traversal of the circle [m/s].
   double  mAngSpeed_radps; //! The angular speed of the object along the turn circle [rad/s].
};
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_MANEUVERTURNCIRCLE_HPP
