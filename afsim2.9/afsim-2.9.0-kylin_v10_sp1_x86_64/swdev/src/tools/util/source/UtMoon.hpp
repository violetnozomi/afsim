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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef UTMOON_HPP
#define UTMOON_HPP

#include "ut_export.h"

class UtCalendar;
#include "UtCentralBody.hpp"
#include "UtVec3.hpp"

namespace ut
{
//! Physical parameters of Earth's moon.
//! References:
//! Gravitational Parameter:
//!   Konopliv AS, Asmar SW, Yuan DN (2001) Recent gravity models as a result of the Lunar Prospector mission.
//!   Icarus 150:1-18
//! SMA/Mean Radius:
//!   Smith, Zuber, Neumann, et. al, 1997, Topography of the Moon from the Clementine lidar,
//!   Journal of Geophysical Research, vol 102, No. E1, pp. 1591-1611
//! Rotation Rate:
//!   Yoder CF (1995a) Astrometric and geodetic properties of Earth and the solar system. In: Global Earth Physics: A
//!   Handbook of Physical Constants (AGU reference shelf 1). Ahrens TJ (ed) American Geophysical Union, p 1-31
class UT_EXPORT MoonEllipsoid : public ut::CentralBodyEllipsoid
{
public:
   static constexpr const char* GetNameId() { return "moon"; }
   const char*                  GetName() const override { return GetNameId(); }
   double GetGravitationalParameter() const override { return 4.902801076E+12; } // m^3/s^2 +- 8.1e-8
   double GetRotationRate() const override { return 2.66170722e-6; }             // rad/s (27.321582 days/rev)
   double GetSemiMajorAxis() const override { return 1.737103E+6; }              // m, +- 15, using the mean radius

   CentralBodyEllipsoid* Clone() const override { return new MoonEllipsoid(); }
};

//! The central body representation of the Moon.
//! Orientation parameters (north pole RA, DEC; sidereal offset angle):
//!   Report of the IAU Working Group on Cartographic Coordinates and Rotational Elements: 2009,
//!   Archinal et. al., Celest Mech Dyn Astr. Special Report (2009).
class UT_EXPORT Moon : public CentralBody
{
public:
   Moon()
      : CentralBody(MoonEllipsoid())
   {
   }

   ut::CentralBody*             Clone() const override { return new Moon(*this); }
   static constexpr const char* GetNameId() { return MoonEllipsoid::GetNameId(); }
   UtVec3d                      GetLocationECI(const UtCalendar& aEpoch) const override;
   double                       GetSiderealOffsetAngle(const UtCalendar&) const override { return 0.6688; } // radians
   double                       GetNorthPoleRA(const UtCalendar&) const override
   {
      return 4.642;
   } // radians; average to 3 significant figures
   double GetNorthPoleDec(const UtCalendar&) const override
   {
      return 1.15;
   } // radians; average to 2 significant figures
};
} // namespace ut

//! Methods to get the Moon's location, phase, and Illumination.
//! The Moon's mean radius is also provided as a constant value.
namespace UtMoon
{
//! The mean radius of the Moon.
constexpr double cMEAN_RADIUS = 1.737103E+6; // meters

//! The semi-major axis of the Moon's orbit about the Earth.
constexpr double cA = 384399000.0; // meters

//! The gravitational parameter (G*M) of the Moon.
constexpr double cGRAVITATIONAL_PARAMETER = 4.902801076E+12; // m^3 / s^2

UT_EXPORT void GetVecWCS(const UtCalendar& aTime, UtVec3d& aVecWCS);
UT_EXPORT void GetLocationWCS(const UtCalendar& aTime, UtVec3d& aVecWCS);
UT_EXPORT void GetLocationECI(const UtCalendar& aEpoch, UtVec3d& aLocationECI);
UT_EXPORT void GetVelocityECI(const UtCalendar& aEpoch, UtVec3d& aVelocityECI);
UT_EXPORT void GetAccelerationECI(const UtCalendar& aEpoch, UtVec3d& aVelocityECI);
UT_EXPORT void GetLocationRA_Dec(const UtCalendar& aEpoch, double& aRA, double& aDec);

UT_EXPORT double GetPhase(const UtCalendar& aEpoch);

UT_EXPORT double GetIllumination(const UtCalendar& aEpoch);

//!@name Deprecated methods using double xxx[3].
//! These are planned to be removed once it is replaced by UtVec3 in other classes.
//!@{
UT_EXPORT void GetLocationECI(const UtCalendar& aEpoch, double aLocationECI[3]);
UT_EXPORT void GetVelocityECI(const UtCalendar& aEpoch, double aVelocityECI[3]);
UT_EXPORT void GetAccelerationECI(const UtCalendar& aEpoch, double aVelocityECI[3]);
//!@}
} // namespace UtMoon

#endif
