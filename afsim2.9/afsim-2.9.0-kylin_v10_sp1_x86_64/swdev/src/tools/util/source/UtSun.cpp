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

#include "UtSun.hpp"

#include "UtCalendar.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtEntity.hpp"
#include "UtMath.hpp"
#include "UtOrbitalElements.hpp"
#include "UtSolarSystem.hpp"
#include "UtSphericalEarth.hpp"
#include "UtVec3.hpp"

UtVec3d ut::Sun::GetLocationECI(const UtCalendar& aEpoch) const
{
   UtVec3d locECI{};
   UtSun::GetSunLocationECI(aEpoch, locECI);
   return locECI;
}

//-----------------------------------------------------------------------------
/** Return the obliquity of the ecliptic (declination angle of the sun at the summer solstice).
   This calculation does not take nutation into account.
   From Capitaine et al. (2003), Astronomy and Astrophysics 412, 567-586.
   @param aTime The time at which the obliquity is desired.
*/
namespace UtSun
{

double GetMeanObliquity(const UtCalendar& aTime)
{
   double t = aTime.GetJ2000TT_Date() / 36525.0;
   double epsilon =
      ((((-0.0000000434 * t - 0.000000576) * t + 0.00200340) * t - 0.0001831) * t - 46.836769) * t + 84381.406;
   return (epsilon / 3600.0 * UtMath::cRAD_PER_DEG);
}

/** Return the obliquity of the ecliptic (declination angle of the sun at the summer solstice).
   This calculation includes a nutational correction that is accurate to within 2 arcseconds.
   @note The formula is referenced in Astronomical Algorithms, p.148, originally from the
   Astronomical Almanac (1983), p.S26.
   @param aTime The time at which the obliquity is desired.
*/
double GetObliquity(const UtCalendar& aTime)
{
   double T         = aTime.GetJ2000TT_Date() / 36525.0; // Centuries since J2000
   double T2        = T * T;
   double T3        = T2 * T;
   double obliquity = 23.439291 - 0.013004 * T - 0.00000017 * T2 + 0.000005036 * T3; // in degrees
   double Om        = UtMath::cRAD_PER_DEG * (UtMath::NormalizeAngle0_360(125.04452 - 1934.136261 * T + 0.0020708 * T2 +
                                                                   T3 / 450000.0)); // longitude of ascending node of Sun
   double Ls        = UtMath::cRAD_PER_DEG * UtMath::NormalizeAngle0_360(280.4665 + 36000.7698 * T);
   double Lm        = UtMath::cRAD_PER_DEG * UtMath::NormalizeAngle0_360(218.3165 + 481267.8813 * T);
   double deltaObliquity = 0.0025 * cos(Om) + 0.00015 * cos(2 * Ls) + 0.000027 * cos(2 * Lm) - 0.000025 * cos(2 * Om);

   return ((obliquity + deltaObliquity) * UtMath::cRAD_PER_DEG);
}

/** Return the sun's right ascension and declination at a given time.
   @param aTime The time of interest.
   @param aSunRA  (output) The sun's right ascension, or angle in radians,
      measured counter-clockwise along the ecliptic from the vernal equinox.
   @param aSunDec (output) The sun's declination, or angle in radians,
      measured perpendicular to the ecliptic.
*/
void GetSunRA_Dec(const UtCalendar& aTime, double& aSunRA, double& aSunDec)
{
   /*
      double obliquity = GetObliquity(aTime);
      //solve for alpha (right ascension) of the sun.
      double excess = GetApparentSunExcess(aTime);
      //excess, due to the equation of time, subtracts from the r.a., and adds to the hour angle.
      aSunRA = GetSunLongitude(aTime) - excess;
      if (aSunRA < -UtMath::cTWO_PI)
      {
         aSunRA += UtMath::cTWO_PI;
      }
      else if (aSunRA > UtMath::cTWO_PI)
      {
         aSunRA -= UtMath::cTWO_PI;
      }
      aSunDec = asin(sin(aSunRA)*sin(obliquity));
   */
   UtVec3d sunVecECI;
   GetSunVecECI(aTime, sunVecECI);
   UtSphericalEarth::ConvertECIToRA_Dec(sunVecECI.GetData(), aSunRA, aSunDec);
}

/** Return the sun's azimuth and elevation for a given latitude, longitude, and time.
   @note This method does not compute refraction effects; therefore it is inaccurate within
   ~5 degrees of the horizon.  Consequently it should not be tested or used in this regime or tested
   with any of the sun rise/set calculators (which do include this effect).  It should agree with results
   of the az/el calculator: http://aa.usno.navy.mil/ to within a solar arc radius (~15 minutes).
   @param aLat The latitude of interest.
   @param aLon The longitude of interest.
   @param aTime The time of interest.
   @param aSunAz (output) The sun's azimuth in local coordinates, measured clockwise from local north.
   @param aSunEl (output) The sun's elevation in local coordinates, measured from the local horizon.
*/
void GetSunAzEl(double aLat, double aLon, const UtCalendar& aTime, double& aSunAz, double& aSunEl)
{
   UtVec3d locNED;
   GetSunVecNED(aLat, aLon, aTime, locNED);
   aSunAz = UtMath::NormalizeAngleMinusPi_Pi(atan2(locNED[1], locNED[0]));
   aSunEl = -asin(locNED[2]);
}

/** Return the sun's azimuth and elevation for a given latitude, longitude, and time.
   @note This method does not compute refraction effects; therefore it is inaccurate within
   ~5 degrees of the horizon.  Consequently it should not be tested or used in this regime or tested
   with any of the sun rise/set calculators (which do include this effect).  It should agree with results
   of the az/el calculator: http://aa.usno.navy.mil/ to within a solar arc radius (~15 minutes).
   @param aLat The latitude of interest.
   @param aLon The longitude of interest.
   @param aTime The time of interest.
   @param aVecNED The unit vector to the sun in local coordinates.
*/
void GetSunVecNED(double aLat, double aLon, const UtCalendar& aTime, UtVec3d& aVecNED)
{
   UtVec3d sunVecWCS;
   GetSunVecWCS(aTime, sunVecWCS);
   double transECEF[3][3];
   double refLocWCS[3];
   UtEllipsoidalEarth::ComputeNEDTransform(aLat,
                                           aLon,
                                           0.0, // altitude is irrelevant to a good approximation
                                           transECEF,
                                           refLocWCS);
   UtEllipsoidalEarth::ConvertECEFToLocal(transECEF, sunVecWCS.GetData(), aVecNED.GetData());
}

/** Return the sun's azimuth and elevation for a geocentric coordinate system at a given time.
@note This method does not compute refraction effects; therefore it is inaccurate within
~5 degrees of the horizon.  Consequently it should not be tested or used in this regime or tested
with any of the sun rise/set calculators (which do include this effect).  It should agree with results
of the az/el calculator: http://aa.usno.navy.mil/ to within a solar arc radius (~15 minutes).
@param aTime The time of interest.
@param aSunAz (output) The sun's azimuth in local coordinates, measured clockwise from local north.
@param aSunEl (output) The sun's elevation in local coordinates, measured from the local horizon.
*/
void GetSunAzEl(const UtCalendar& aTime, double& aSunAz, double& aSunEl)
{
   UtVec3d locWCS;
   GetSunVecWCS(aTime, locWCS);
   aSunAz = UtMath::NormalizeAngleMinusPi_Pi(UtMath::cTWO_PI - atan2(locWCS[1], locWCS[0]));
   aSunEl = asin(locWCS[2]);
}

//! Given a valid calendar time, Return the unit WCS vector pointing to the sun.
//! @note This method does not take parallax into account.
void GetSunVecWCS(const UtCalendar& aTime, UtVec3d& aSunVecWCS)
{
   GetSunLocationWCS(aTime, aSunVecWCS);
   aSunVecWCS.Normalize();
}

//! Given a valid calendar time, Return the unit ECI vector pointing to the sun.
//! @note This method does not take parallax into account.
void GetSunVecECI(const UtCalendar& aTime, UtVec3d& aSunVecECI)
{
   GetSunLocationECI(aTime, aSunVecECI);
   aSunVecECI.Normalize();
}

//! Given a valid calendar time, Return the WCS location vector of the sun.
//! @note This method does not take parallax into account.
void GetSunLocationWCS(const UtCalendar& aTime, UtVec3d& aSunVecWCS)
{
   UtVec3d locECI;
   GetSunLocationECI(aTime, locECI);
   UtEllipsoidalEarth::ConvertTODToECEF(aTime.GetEarthAngleApprox(), locECI.GetData(), aSunVecWCS.GetData());
}

//! Given a valid calendar time, Return the ECI location vector of the sun.
//! @note This method does not take parallax into account.
void GetSunLocationECI(const UtCalendar& aTime, UtVec3d& aSunVecECI)
{
   UtSolarSystem::GetSunLocationECI(aTime, aSunVecECI);
}

//! (Deprecated)
void GetSunVecWCS(const UtCalendar& aTime, double aSunVecWCS[3])
{
   GetSunLocationWCS(aTime, aSunVecWCS);
   UtVec3d::Normalize(aSunVecWCS);
}

//! (Deprecated)
void GetSunVecECI(const UtCalendar& aTime, double aSunVecECI[3])
{
   GetSunLocationECI(aTime, aSunVecECI);
   UtVec3d::Normalize(aSunVecECI);
}

//! (Deprecated)
void GetSunLocationWCS(const UtCalendar& aTime, double aSunVecWCS[3])
{
   double locECI[3];
   GetSunLocationECI(aTime, locECI);
   UtEllipsoidalEarth::ConvertTODToECEF(aTime.GetEarthAngleApprox(), locECI, aSunVecWCS);
}

//! (Deprecated)
void GetSunLocationECI(const UtCalendar& aTime, double aSunVecECI[3])
{
   UtVec3d sunLocationECI;
   UtSolarSystem::GetSunLocationECI(aTime, sunLocationECI);
   UtVec3d::Set(aSunVecECI, sunLocationECI.GetData());
}

/** Get the ecliptic longitude of the mean sun, referenced to the vernal equinox.
   @param aTime The time at which the longitude is desired.
*/
double GetSunLongitude(const UtCalendar& aTime)
{
   // equations from http://aa.usno.navy.mil/faq/docs/GAST.html
   // D = JD - 2451545.0
   // L = 280.47 + 0.98565 D.
   // static const double jdOfJan1_2000   = 2451545.0;
   static const double longOnJan1_2000 = 280.47;
   // following is the angle per day traversed by the mean sun, in degrees (=360/364.24)
   static const double anglePerDay = 0.98565;
   // double jdDiff = aTime.GetJulianDate() - jdOfJan1_2000;
   double jdDiff       = aTime.GetJ2000_Date();
   double sunLongitude = longOnJan1_2000 + anglePerDay * jdDiff;
   sunLongitude        = fmod(sunLongitude, 360.0) * UtMath::cRAD_PER_DEG;
   return sunLongitude;
}

/** Return the approximate result from the equation of time, which is the difference between the mean sun and
   the true sun, in seconds.
   @param aTime The time at which the equation of time is desired.
*/
double GetEquationOfTime(const UtCalendar& aTime)
{
   // The following curve fit to the equation of time from the Wikipedia (wikipedia.com).
   double doy = aTime.GetEpochDayOfYear();
   double B   = UtMath::cTWO_PI * (doy - 81.0) / 364.0;
   double E   = 9.87 * sin(2.0 * B) - 7.53 * cos(B) - 1.5 * sin(B);
   return (E * UtCalendar::cSEC_IN_MIN);
}

/** Return the approximate result from the equation of time, which is the difference between the mean sun and
   the true sun, in radians.
   @param aTime The time at which the excess is desired.
*/
double GetApparentSunExcess(const UtCalendar& aTime)
{
   return ((GetEquationOfTime(aTime) / UtCalendar::cSEC_IN_HOUR) * UtCalendar::cDEGREES_IN_HOUR * UtMath::cRAD_PER_DEG);
}

//! Return the right ascension and declination of the sun.
//! @param aEpoch The date and time at which the coordinates are desired.
//! @param aRA The right ascension of the sun.
//! @param aDec The declination of the sun.
void GetSunLocationRA_Dec(const UtCalendar& aEpoch, double& aRA, double& aDec)
{
   UtVec3d locationECI;
   GetSunLocationECI(aEpoch, locationECI);
   UtSphericalEarth::ConvertECIToRA_Dec(locationECI.GetData(), aRA, aDec);
}

} // namespace UtSun
