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

#include <chrono>

#include <gtest/gtest.h>

#include "UtCalendar.hpp"
#include "UtMath.hpp"
#include "UtSun.hpp"

TEST(UtSun, VernalEquinox)
{
   // Get some values for the Vernal Equinox at Greenwich
   // (table from http://scienceworld.wolfram.com/astronomy/VernalEquinox.html)
   UtCalendar cal;
   cal.SetDate(2005, 3, 20);
   cal.SetTime(12, 30);
   double ra, dec;
   // ra shouble -9 at v.e.
   double angDia = 1.0 / 57.2; // -0.5 degree
   UtSun::GetSunRA_Dec(cal, ra, dec);
   EXPECT_NEAR(ra, 0.0, angDia);

   // The difference between the mean and the true sun is ~ 8 min 12 sec at this time.
   // We must advance the mean time forward to account for this effect.
   // As a sanity check, see http://aa.usno.navy.mil/, and enter appropriate values
   // (e.g. 1 minute intervals for 90 deg west, 45 deg north (-6 hours UT).
   cal.SetTime(12, 8, 12);

   // sunrise should be at 0 el for lat=obliquity at v.e.
   double lat = 45.0; // deg
   double lon = -90.0;
   double az, el;
   UtSun::GetSunAzEl(lat, lon, cal, az, el);

   EXPECT_NEAR(0.0, el, angDia);

   // same for sunset.
   lon = 90.0;
   UtSun::GetSunAzEl(lat, lon, cal, az, el);

   EXPECT_NEAR(0.0, el, angDia);

   // zenith
   lon = -15.0 * (0.0);
   UtSun::GetSunAzEl(lat, lon, cal, az, el);

   EXPECT_NEAR(UtMath::cPI, std::abs(az), angDia);
   EXPECT_NEAR(UtMath::cPI_OVER_4, el, angDia);

   // nadir
   lon = -15.0 * (12);
   UtSun::GetSunAzEl(lat, lon, cal, az, el);

   EXPECT_NEAR(0.0, az, angDia);
   //    EXPECT_NEAR(UtMath::cTWO_PI, az, angDia);
   EXPECT_NEAR(-UtMath::cPI_OVER_4, el, angDia);

   lon = 0.0;
   UtSun::GetSunAzEl(lat, lon, cal, az, el);

   lon = -15.0 * (-6);
   cal.SetTime(6, 8, 12.0);
   UtSun::GetSunAzEl(lat, lon, cal, az, el);

   EXPECT_NEAR(UtMath::cPI, std::abs(az), angDia);
   EXPECT_NEAR(UtMath::cPI_OVER_4, el, angDia);
}
