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

#include "UtMoon.hpp"

#include <iostream>

#include "UtCalendar.hpp"
#include "UtEarth.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtMath.hpp"
#include "UtOrbitalElements.hpp"
#include "UtSolarSystem.hpp"
#include "UtSphericalEarth.hpp"
#include "UtSun.hpp"
#include "UtVec3.hpp"

UtVec3d ut::Moon::GetLocationECI(const UtCalendar& aEpoch) const
{
   UtVec3d locECI;
   UtMoon::GetLocationECI(aEpoch, locECI);
   return locECI;
}

//-----------------------------------------------------------------------------
namespace UtMoon
{
//! Given a valid calendar time, Return the unit WCS vector pointing to the moon.
//! @note This method does not take parallax into account.
void GetVecWCS(const UtCalendar& aTime, UtVec3d& aVecWCS)
{
   GetLocationWCS(aTime, aVecWCS);
   aVecWCS.Normalize();
}

//! Given a valid calendar time, Return the WCS location vector of the moon.
//! @note This method does not take parallax into account.
void GetLocationWCS(const UtCalendar& aTime, UtVec3d& aVecWCS)
{
   UtVec3d locECI;
   GetLocationECI(aTime, locECI);
   UtEllipsoidalEarth::ConvertTODToECEF(aTime.GetEarthAngleApprox(), locECI.GetData(), aVecWCS.GetData());
}

//! Return the location of the moon in ECI coordinates at the given time.
//! @param aEpoch The date and time at which the location is valid.
//! @param aLocationECI The returned ECI location.
void GetLocationECI(const UtCalendar& aEpoch, UtVec3d& aLocationECI)
{
   UtSolarSystem::GetMoonLocationECI(aEpoch, aLocationECI);
}

//! Return the velocity of the moon in ECI coordinates at the given time.
//! @param aEpoch The date and time at which the velocity is valid.
//! @param aVelocityECI The returned ECI velocity.
void GetVelocityECI(const UtCalendar& aEpoch, UtVec3d& aVelocityECI)
{
   UtVec3d locationECI;
   UtSolarSystem::GetMoonLocationVelocityECI(aEpoch, locationECI, aVelocityECI);
}

//! Return the acceleration of the moon in ECI coordinates at the given time.
//! @param aEpoch The date and time at which the acceleration is valid.
//! @param aAccelerationECI The returned ECI acceleration.
void GetAccelerationECI(const UtCalendar& aEpoch, UtVec3d& aAccelerationECI)
{
   // Use the passed in variable to hold the location vector (changed later).
   GetLocationECI(aEpoch, aAccelerationECI);
   double distance = aAccelerationECI.Magnitude();
   aAccelerationECI.Multiply(-UtEarth::cGRAVITATIONAL_PARAMETER / pow(distance, 3)); // GM * r / r^3
}

//! Return the right ascension and declination of the moon.
//! @param aEpoch The date and time at which the coordinates are desired.
//! @param aRA The right ascension of the moon.
//! @param aDec The declination of the moon.
void GetLocationRA_Dec(const UtCalendar& aEpoch, double& aRA, double& aDec)
{
   UtVec3d locationECI;
   GetLocationECI(aEpoch, locationECI);
   UtSphericalEarth::ConvertECIToRA_Dec(locationECI.GetData(), aRA, aDec);
}

//! Get the phase of the moon for the give date and time.
//! @param aEpoch The date and time for which the phase is desired.
//! @return the phase of the moon in radians, where 0 is a new moon, pi is full, and the range is 0 - 2pi.
double GetPhase(const UtCalendar& aEpoch)
{
   UtVec3d moonLocECI;
   GetLocationECI(aEpoch, moonLocECI);
   UtVec3d sunLocECI;
   UtSolarSystem::GetSunLocationECI(aEpoch, sunLocECI);
   UtVec3d moonSunVecECI;
   moonSunVecECI.Subtract(sunLocECI, moonLocECI);
   moonSunVecECI.Normalize();
   moonLocECI.Normalize();
   double cosPhase = UtVec3d::DotProduct(moonLocECI, moonSunVecECI);
   double phase    = acos(cosPhase);

   // By knowing the cosine of the phase, we still have a quadrant ambiguity.
   // I resolve this by taking the cross product and looking at the z-value.
   // If the value is negative, the moon is waxing; if positive, it is waning.
   UtVec3d crossProduct;
   crossProduct.CrossProduct(moonLocECI, moonSunVecECI);
   if (crossProduct[2] > 0.0) // waning
   {
      phase = UtMath::cTWO_PI - phase;
   }

   return phase;
}

//! Get the fraction of the moon illuminated at the given date and time.
//! @param aEpoch The date and time for which the illumination is desired.
//! @return The illuminated fraction, in the range (0.0 - 1.0).
double GetIllumination(const UtCalendar& aEpoch)
{
   UtVec3d moonLocECI;
   GetLocationECI(aEpoch, moonLocECI);
   UtVec3d sunLocECI;
   UtSolarSystem::GetSunLocationECI(aEpoch, sunLocECI);
   UtVec3d moonSunVecECI;
   moonSunVecECI.Subtract(sunLocECI, moonLocECI);
   moonSunVecECI.Normalize();
   ;
   moonLocECI.Normalize();
   double cosPhase = UtVec3d::DotProduct(moonLocECI, moonSunVecECI);
   return fabs((cosPhase - 1) / 2.0);
}

//! (Deprecated)
void GetVelocityECI(const UtCalendar& aEpoch, double aVelocityECI[3])
{
   UtVec3d locationECI;
   UtVec3d velocityECI;
   UtSolarSystem::GetMoonLocationVelocityECI(aEpoch, locationECI, velocityECI);
   UtVec3d::Set(aVelocityECI, velocityECI.GetData());
}

//! (Deprecated)
void GetAccelerationECI(const UtCalendar& aEpoch, double aAccelerationECI[3])
{
   // Use the passed in variable to hold the location vector (changed later).
   GetLocationECI(aEpoch, aAccelerationECI);
   double distance = UtVec3d::Magnitude(aAccelerationECI);
   UtVec3d::Multiply(aAccelerationECI, -UtEarth::cGRAVITATIONAL_PARAMETER / pow(distance, 3)); // GM * r / r^3
}

//! (Deprecated)
void GetLocationECI(const UtCalendar& aEpoch, double aLocationECI[3])
{
   UtVec3d locationECI;
   UtSolarSystem::GetMoonLocationECI(aEpoch, locationECI);
   UtVec3d::Set(aLocationECI, locationECI.GetData());
}

} // namespace UtMoon
