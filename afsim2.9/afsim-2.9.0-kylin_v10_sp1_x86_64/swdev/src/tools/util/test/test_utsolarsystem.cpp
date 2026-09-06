// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <gtest/gtest.h>

#include "UtCalendar.hpp"
#include "UtMath.hpp"
#include "UtMoon.hpp"
#include "UtOrbitalElements.hpp"
#include "UtSolarSystem.hpp"
#include "UtSphericalEarth.hpp"
#include "UtVec3.hpp"

TEST(UtSolarSystem, MOON_PLANETS_LOCATION_TEST)
{
   UtCalendar cal;
   cal.SetDate(1992, 4, 12);
   cal.SetTime(00, 00);
   double  ra, dec;
   UtVec3d locECI;
   UtMoon::GetLocationECI(cal, locECI);
   UtSphericalEarth::ConvertECIToRA_Dec(locECI.GetData(), ra, dec);
   ra *= UtMath::cDEG_PER_RAD;
   dec *= UtMath::cDEG_PER_RAD;
   EXPECT_NEAR(ra, 134.685, 0.01);
   EXPECT_NEAR(dec, 13.765, 0.01);

   UtSolarSystem solarSystem;

   // same as above but use the moon object.
   // Test case from Meeus' Astronomical Algorithms 2nd ed. p. 342-343:
   // Compare vs. lat, lon, r: 133.162655 -3.229126 368409.7

   UtSolarSystem::Body* moon = solarSystem.GetBody("Moon");
   UtCalendar           test2;
   test2.SetDate(1992, 4, 12); // April 12 1992
   test2.SetTime(0, 0, 0.0);
   moon->Update(test2);
   const ut::OrbitalState& orbitalState = moon->GetOrbitalState();
   EXPECT_TRUE(orbitalState.GetCoordinateSystem() == ut::OrbitalState::CoordinateSystem::cECLIPTIC);
   UtVec3d locationEcliptic = orbitalState.GetOrbitalStateVector().GetLocation();
   double  distance         = locationEcliptic.Magnitude();
   double  lonEcl           = atan2(locationEcliptic[1], locationEcliptic[0]);
   double  latEcl           = asin(locationEcliptic[2] / distance);
   lonEcl *= UtMath::NormalizeAngle0_360(UtMath::cDEG_PER_RAD);
   latEcl *= UtMath::cDEG_PER_RAD;
   distance /= 1000.0;

   EXPECT_NEAR(lonEcl, 133.162655, 0.02);
   EXPECT_NEAR(latEcl, -3.229126, 0.02);
   EXPECT_NEAR(distance, 368409.7, 1.0);

   int year = 2004, month = 1, day = 1;

   UtCalendar test;
   test.SetDate(year, month, day);
   test.SetTime(0, 0, 0);

   double expectedRA[UtSolarSystem::Planet::cNUM_PLANETS] = {268.693, 316.189, 280.710, 8.335, 170.120, 100.256, 333.148, 313.525};
   double expectedDec[UtSolarSystem::Planet::cNUM_PLANETS] =
      {-20.296, -18.614, -23.074, +3.660, +5.567, +22.420, -11.868, -17.459};
   double expectedDist[UtSolarSystem::Planet::cNUM_PLANETS] = {0.70403, 1.3061, 0.98331, 1.1115, 4.9716, 8.0443, 20.654, 30.973};
   for (int planetNum = UtSolarSystem::Planet::cMERCURY; planetNum < UtSolarSystem::Planet::cNUM_PLANETS; ++planetNum)
   {
      if (planetNum != UtSolarSystem::Planet::cEARTH)
      {
         solarSystem.GetLocationECI(solarSystem.GetPlanetName(planetNum), test, locECI);
      }
      else
      {
         solarSystem.GetSunLocationECI(test, locECI);
      }

      UtSphericalEarth::ConvertECIToRA_Dec(locECI.GetData(), ra, dec);
      ra *= UtMath::cDEG_PER_RAD;
      dec *= UtMath::cDEG_PER_RAD;
      double distAu = UtVec3d::Magnitude(locECI.GetData()) / 1.496E+11;
      EXPECT_NEAR(ra, expectedRA[planetNum], 0.3);
      EXPECT_NEAR(dec, expectedDec[planetNum], 0.1);
      EXPECT_NEAR(distAu, expectedDist[planetNum], 0.01);
   }

   // Test updated vs. initialized earth location
   UtSolarSystem::Body* earth = solarSystem.GetBody("Earth");
   earth->Initialize(cal);
   UtCalendar updatedTime = cal;
   updatedTime.AdvanceTimeBy(100.0);
   UtSolarSystem::Body* earth2 = solarSystem.GetBody("Earth");
   earth2->Initialize(updatedTime);

   UtVec3d earthLoc;
   earth->GetLocationEcliptic(updatedTime, earthLoc);

   UtVec3d earth2Loc = earth2->GetOrbitalState().GetOrbitalStateVector().GetLocation();

   EXPECT_NEAR(earthLoc[0], earth2Loc[0], 1.0e-12);
   EXPECT_NEAR(earthLoc[1], earth2Loc[1], 1.0e-12);
   EXPECT_NEAR(earthLoc[2], earth2Loc[2], 1.0e-12);

   UtVec3d earth3Loc;
   UtVec3d earthLocECI;
   UtSun::GetSunLocationECI(updatedTime, earthLocECI);
   UtSolarSystem::ConvertEquatorialToEcliptic(updatedTime, earthLocECI, earth3Loc);
   earth3Loc.Multiply(-1.0);

   EXPECT_NEAR(earthLoc[0], earth3Loc[0], 1.0e-12);
   EXPECT_NEAR(earthLoc[1], earth3Loc[1], 1.0e-4);
   EXPECT_NEAR(earthLoc[2], earth3Loc[2], 1.0e-12);
}

namespace
{
void TestCentralBodyRotationRate(const ut::CentralBody& aCentralBody, double aDaysPerRev, double aTolerance)
{
   UtCalendar epoch;
   double     beginAngle = aCentralBody.ComputeSiderealAngle(epoch);
   epoch.AdvanceTimeBy((aDaysPerRev * UtCalendar::cSEC_IN_DAY) / 2.0); // 1/2 revolution
   double midAngle   = aCentralBody.ComputeSiderealAngle(epoch);
   double deltaAngle = fabs(midAngle - beginAngle);
   EXPECT_NEAR(deltaAngle, UtMath::cPI, aTolerance);
   epoch.AdvanceTimeBy((aDaysPerRev * UtCalendar::cSEC_IN_DAY) / 2.0); // 1/2 revolution
   double endAngle = aCentralBody.ComputeSiderealAngle(epoch);
   deltaAngle      = fabs(endAngle - midAngle);
   EXPECT_NEAR(deltaAngle, UtMath::cPI, aTolerance);
   EXPECT_NEAR(fabs(endAngle - beginAngle), 0.0, aTolerance);
}
} // namespace

TEST(UtSolarSystem, RotationRateTest)
{
   // earth is covered in other tests
   TestCentralBodyRotationRate(ut::Moon{}, 27.321582, 1.0e-6);
   TestCentralBodyRotationRate(ut::Sun{}, 25.38, 1.0e-4);
}
