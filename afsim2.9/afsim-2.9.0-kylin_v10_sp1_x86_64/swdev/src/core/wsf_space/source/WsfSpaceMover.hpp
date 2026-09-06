// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSPACEMOVER_HPP
#define WSFSPACEMOVER_HPP

#include "wsf_space_export.h"

#include <memory>

#include "UtCloneablePtr.hpp"
class UtInput;
#include "UtOrbitalState.hpp"
class UtTwoLineElement;
class WsfConjunctionInput;
class WsfDateTime;
class WsfMover;
class WsfScenario;
#include "WsfSpaceMoverBase.hpp"
#include "WsfWaypoint.hpp"

//! WsfSpaceMover is a two-body orbital propagator, used as an WSF mover.
//!
//! The motion of the satellite is described by a standard orbital elements.
//! These may be provided explicitly, in the form of an initial waypoint, or in
//! the form of a 'two-line element' (TLE) (the TLE form captures the classical
//! orbital elements along with some other terms useful to the NORAD propagators.
//!
//! This mover reproduces two body motion, either assuming spherical earth or oblate earth.
//! The default is to use spherical earth. For oblate earth, the right ascension of the ascending node
//! and the argument of the periapsis are precessed at a constant rate using a first-order approximation
//! in the Legendre polynomial expansion of the potential (i.e., the "J2 Term").
//!
//! This mover is also useful for simulating satellite maneuvering.  A general method is provided
//! for impulsive maneuvering, "AddDeltaV"; for maneuvers parallel to the existing velocity vector
//! of the orbiting body, the method "Boost" is also provided.
class WSF_SPACE_EXPORT WsfSpaceMover : public WsfSpaceMoverBase
{
public:
   explicit WsfSpaceMover(const WsfScenario& aScenario);
   WsfSpaceMover(const WsfSpaceMover& aSrc) = default;
   WsfSpaceMover& operator=(const WsfSpaceMover&) = delete;
   ~WsfSpaceMover() override                      = default;

   bool SetInitialPoint(double lat, double lon, double alt, double speed, double heading) override;

   bool Initialize(double aSimTime) override;

   WsfMover* Clone() const override { return new WsfSpaceMover(*this); }

   bool ProcessInput(UtInput& aInput) override;

   const WsfDateTime& GetDateTime() const;

protected:
   void ShowStatus(double aSimTime);

private:
   void InitializeFromWaypoint(const UtCalendar& aEpoch);

   bool mDebugShowStatus;

   // Initial conditions to be converted to orbital elements
   ut::CloneablePtr<WsfWaypoint> mInitialPointPtr;
};

#endif
