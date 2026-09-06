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

#ifndef WSFORBITALTARGETPOINT_HPP
#define WSFORBITALTARGETPOINT_HPP

#include "UtCalendar.hpp"
#include "UtOrbitalState.hpp"
#include "UtVec3.hpp"

//! OrbitalTargetPoint represents a target for orbital maneuvers.
//! Target points give the kinematics of a target of interest. The target so
//! specified is almost always a function of time, so to get the current
//! kinematics clients of OrbitalTargetPoint must call Update with the
//! epoch of interest.
namespace wsf
{
namespace space
{

class OrbitalTargetPoint
{
public:
   OrbitalTargetPoint()          = default;
   virtual ~OrbitalTargetPoint() = default;

   //! Create a copy of this target point.
   virtual OrbitalTargetPoint* Clone() const = 0;

   //! Update the point to the given @p aEpoch.
   virtual void Update(const UtCalendar& aEpoch) = 0;

   //! Return the combined inertial position and velocity of this point at this object's current epoch.
   virtual ut::OrbitalStateVector GetOrbitalStateVectorInertial() const = 0;

   //! Return the ECI position of this point at this object's current epoch.
   virtual UtVec3d GetPositionECI() const = 0;

   //! Return the ECI velocity of this point at this object's current epoch.
   virtual UtVec3d GetVelocityECI() const = 0;

   //! Return the current epoch of this point.
   virtual UtCalendar GetEpoch() const = 0;

   //! Return a characteristic time for this point.
   //!
   //! This is used in the Target maneuver to set a maximum search time when
   //! there is no user input for this value. This method should return a
   //! reasonable time that likely includes the targeting solution. For
   //! example, a propagator based OrbitalTargetPoint might return the
   //! orbital period.
   //!
   //! \returns - A characteristic time (in seconds) for the motion of the underlying point.
   virtual double GetCharacteristicTime() const = 0;
};

} // namespace space
} // namespace wsf

#endif // WSFORBITALTARGETPOINT_HPP
