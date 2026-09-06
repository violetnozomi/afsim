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

#ifndef UTSUN_HPP
#define UTSUN_HPP

#include "ut_export.h"

class UtCalendar;
#include "UtCentralBody.hpp"
#include "UtVec3.hpp"

namespace ut
{
//! Physical parameters for the Sun:
//! Gravitational Parameter:
//! Astrodynamic Constants (Jet Propulsion Laboratory, derived from 1994 IAU data):
//!   https://ssd.jpl.nasa.gov/?constants
//! Semi-major axis:
//!  Emilio, Marcelo; Kuhn, Jeff R.; Bush, Rock I.; Scholl, Isabelle F. (2012),
//!  "Measuring the Solar Radius from Space during the 2003 and 2006 Mercury Transits",
//!  The Astrophysical Journal, 750 (2): 135.
//! Rotation Rate:
//!   Williams, D.R. (1 July 2013). "Sun Fact Sheet".
//!   NASA Goddard Space Flight Center. Archived from the original on 15 July 2010. Retrieved 12 August 2013.
class UT_EXPORT SunEllipsoid : public ut::CentralBodyEllipsoid
{
public:
   static constexpr const char* GetNameId() { return "sun"; }
   const char*                  GetName() const override { return GetNameId(); }
   double GetGravitationalParameter() const override { return 1.32712440018E+20; } // m^3/s^2 +-8E9
   double GetRotationRate() const override { return 2.8653E-6; }                   // rad/s, equatorial (25.38 days/rev)
   double GetSemiMajorAxis() const override { return 6.963420E+8; }                // m, +-65000, using the mean radius

   CentralBodyEllipsoid* Clone() const override { return new SunEllipsoid(); }
};

//! The central body representation of the Sun.
//! Orientation parameters (north pole RA, DEC; sidereal offset angle):
//!   Report of the IAU Working Group on Cartographic Coordinates and Rotational Elements: 2009,
//!   Archinal et. al., Celest Mech Dyn Astr. Special Report (2009).
class UT_EXPORT Sun : public CentralBody
{
public:
   Sun()
      : CentralBody(SunEllipsoid())
   {
   }

   ut::CentralBody*             Clone() const override { return new Sun(*this); }
   static constexpr const char* GetNameId() { return SunEllipsoid::GetNameId(); }
   UtVec3d                      GetLocationECI(const UtCalendar& aEpoch) const override;
   double                       GetNorthPoleRA(const UtCalendar&) const override { return 4.9939; }
   double                       GetNorthPoleDec(const UtCalendar&) const override { return 1.1147; }
   double                       GetSiderealOffsetAngle(const UtCalendar&) const override { return 1.4691; }
};
} // namespace ut

/** A set of utilities that provide information about the sun,
  especially its geocentric direction.  These primarily make use
  of UtSolarSystem::GetSunLocationECI.
*/
namespace UtSun
{
constexpr double cGRAVITATIONAL_PARAMETER = 1.32712440018E+20;
constexpr double cMEAN_RADIUS             = 6.963420E+8; // m

UT_EXPORT void GetSunAzEl(const UtCalendar& aTime, double& aSunAz, double& aSunEl);

UT_EXPORT void GetSunVecWCS(const UtCalendar& aTime, UtVec3d& aSunVecWCS);

UT_EXPORT void GetSunVecECI(const UtCalendar& aTime, UtVec3d& aSunVecECI);

UT_EXPORT void GetSunLocationWCS(const UtCalendar& aTime, UtVec3d& aSunVecWCS);

UT_EXPORT void GetSunLocationECI(const UtCalendar& aTime, UtVec3d& aSunVecECI);

UT_EXPORT void GetSunLocationRA_Dec(const UtCalendar& aEpoch, double& aRA, double& aDec);

UT_EXPORT void GetSunAzEl(double aLat, double aLon, const UtCalendar& aTime, double& aSunAz, double& aSunEl);

UT_EXPORT void GetSunVecNED(double aLat, double aLon, const UtCalendar& aTime, UtVec3d& aVecNED);

UT_EXPORT void GetSunRA_Dec(const UtCalendar& aTime, double& aSunRA, double& aSunDec);

UT_EXPORT double GetSunLongitude(const UtCalendar& aTime);

UT_EXPORT double GetApparentSunExcess(const UtCalendar& aTime);

UT_EXPORT double GetEquationOfTime(const UtCalendar& aTime);

UT_EXPORT double GetMeanObliquity(const UtCalendar& aTime);

UT_EXPORT double GetObliquity(const UtCalendar& aTime);

//!@name Deprecated methods using double xxx[3].
//! These are planned to be removed once it is replaced by UtVec3 in other classes.
//!@{
UT_EXPORT void GetSunVecWCS(const UtCalendar& aTime, double aSunVecWCS[3]);

UT_EXPORT void GetSunVecECI(const UtCalendar& aTime, double aSunVecECI[3]);

UT_EXPORT void GetSunLocationWCS(const UtCalendar& aTime, double aSunVecWCS[3]);

UT_EXPORT void GetSunLocationECI(const UtCalendar& aTime, double aSunVecECI[3]);
//!@}
} // namespace UtSun

#endif
