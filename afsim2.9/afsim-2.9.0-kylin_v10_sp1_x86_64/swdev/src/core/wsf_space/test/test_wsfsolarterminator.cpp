// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <cmath>

#include <gtest/gtest.h>

#include "UtCalendar.hpp"
#include "UtEarth.hpp"
#include "UtMath.hpp"
#include "UtSun.hpp"
#include "WsfSolarTerminator.hpp"

TEST(WsfSolarTerminator, PhaseOfDayFixedLocation)
{
   // Vernal Equinox 2019 : March 20, 21:58 UTC.
   // At that time, the Sun is over 00:00:09.0n 147:38:14.8w
   UtCalendar equinox2019{};
   equinox2019.SetDate(2019, 3, 20);
   equinox2019.SetTime(21, 58, 0.0);

   // The following points (A, B, C, D, E) should have the following
   // phase for the various twilight definitions (Civil, Nautical, Astronomical)
   //
   //   | C | N | A |
   // ---------------
   // A | D   D   D
   // B | T   T   T
   // C | N   T   T
   // D | N   N   T
   // E | N   N   N
   //
   // All points on the equator, but with the following longitude:
   // A: 57:38:14.8 w
   // B: 52:38:14.8 w
   // C: 47:38:14.8 w
   // D: 42:38:14.8 w
   // E: 37:38:14.8 w

   // Point A tests
   double lat = 0.0;
   double lon = -(57.0 + 38.0 / 60.0 + 14.8 / 3600.0);
   EXPECT_EQ(WsfSolarTerminator::PhaseOfDay::cDAY,
             WsfSolarTerminator::GetPhaseOfDay(lat, lon, equinox2019, WsfSolarTerminator::cTWILIGHT_CIVIL));
   EXPECT_EQ(WsfSolarTerminator::PhaseOfDay::cDAY,
             WsfSolarTerminator::GetPhaseOfDay(lat, lon, equinox2019, WsfSolarTerminator::cTWILIGHT_NAUTICAL));
   EXPECT_EQ(WsfSolarTerminator::PhaseOfDay::cDAY,
             WsfSolarTerminator::GetPhaseOfDay(lat, lon, equinox2019, WsfSolarTerminator::cTWILIGHT_ASTRONOMICAL));

   // Point B tests
   lon += 5.0;
   EXPECT_EQ(WsfSolarTerminator::PhaseOfDay::cTWILIGHT,
             WsfSolarTerminator::GetPhaseOfDay(lat, lon, equinox2019, WsfSolarTerminator::cTWILIGHT_CIVIL));
   EXPECT_EQ(WsfSolarTerminator::PhaseOfDay::cTWILIGHT,
             WsfSolarTerminator::GetPhaseOfDay(lat, lon, equinox2019, WsfSolarTerminator::cTWILIGHT_NAUTICAL));
   EXPECT_EQ(WsfSolarTerminator::PhaseOfDay::cTWILIGHT,
             WsfSolarTerminator::GetPhaseOfDay(lat, lon, equinox2019, WsfSolarTerminator::cTWILIGHT_ASTRONOMICAL));

   // Point C tests
   lon += 5.0;
   EXPECT_EQ(WsfSolarTerminator::PhaseOfDay::cNIGHT,
             WsfSolarTerminator::GetPhaseOfDay(lat, lon, equinox2019, WsfSolarTerminator::cTWILIGHT_CIVIL));
   EXPECT_EQ(WsfSolarTerminator::PhaseOfDay::cTWILIGHT,
             WsfSolarTerminator::GetPhaseOfDay(lat, lon, equinox2019, WsfSolarTerminator::cTWILIGHT_NAUTICAL));
   EXPECT_EQ(WsfSolarTerminator::PhaseOfDay::cTWILIGHT,
             WsfSolarTerminator::GetPhaseOfDay(lat, lon, equinox2019, WsfSolarTerminator::cTWILIGHT_ASTRONOMICAL));

   // Point D tests
   lon += 5.0;
   EXPECT_EQ(WsfSolarTerminator::PhaseOfDay::cNIGHT,
             WsfSolarTerminator::GetPhaseOfDay(lat, lon, equinox2019, WsfSolarTerminator::cTWILIGHT_CIVIL));
   EXPECT_EQ(WsfSolarTerminator::PhaseOfDay::cNIGHT,
             WsfSolarTerminator::GetPhaseOfDay(lat, lon, equinox2019, WsfSolarTerminator::cTWILIGHT_NAUTICAL));
   EXPECT_EQ(WsfSolarTerminator::PhaseOfDay::cTWILIGHT,
             WsfSolarTerminator::GetPhaseOfDay(lat, lon, equinox2019, WsfSolarTerminator::cTWILIGHT_ASTRONOMICAL));

   // Point E tests
   lon += 5.0;
   EXPECT_EQ(WsfSolarTerminator::PhaseOfDay::cNIGHT,
             WsfSolarTerminator::GetPhaseOfDay(lat, lon, equinox2019, WsfSolarTerminator::cTWILIGHT_CIVIL));
   EXPECT_EQ(WsfSolarTerminator::PhaseOfDay::cNIGHT,
             WsfSolarTerminator::GetPhaseOfDay(lat, lon, equinox2019, WsfSolarTerminator::cTWILIGHT_NAUTICAL));
   EXPECT_EQ(WsfSolarTerminator::PhaseOfDay::cNIGHT,
             WsfSolarTerminator::GetPhaseOfDay(lat, lon, equinox2019, WsfSolarTerminator::cTWILIGHT_ASTRONOMICAL));
}

TEST(WsfSolarTerminator, PhaseOfDayInsideArctic)
{
   const int cNUM_TEST_POINTS = 36;

   // At the winter solstice, test a series of points in the arctic.
   {
      UtCalendar winter2018{};
      winter2018.SetDate(2018, 12, 21);
      winter2018.SetTime(21, 32, 0.0);

      int  numberOfDaysWinter{0};
      auto winterSum = [&numberOfDaysWinter, &winter2018](double aLat, double aLon)
      {
         if (WsfSolarTerminator::PhaseOfDay::cDAY == WsfSolarTerminator::GetPhaseOfDay(aLat, aLon, winter2018))
         {
            ++numberOfDaysWinter;
         }
      };
      for (int i = 0; i < cNUM_TEST_POINTS; ++i)
      {
         winterSum(70.0, i * 360.0 / cNUM_TEST_POINTS);
      }

      // They should all be beyond the terminator.
      EXPECT_EQ(numberOfDaysWinter, 0);
   }

   // At the summer solstice, test those same points.
   {
      UtCalendar summer2019{};
      summer2019.SetDate(2019, 6, 21);
      summer2019.SetTime(15, 54, 0.0);

      int  numberOfDaysSummer{0};
      auto summerSum = [&numberOfDaysSummer, &summer2019](double aLat, double aLon)
      {
         if (WsfSolarTerminator::PhaseOfDay::cDAY == WsfSolarTerminator::GetPhaseOfDay(aLat, aLon, summer2019))
         {
            ++numberOfDaysSummer;
         }
      };
      for (int i = 0; i < cNUM_TEST_POINTS; ++i)
      {
         summerSum(70.0, i * 360.0 / cNUM_TEST_POINTS);
      }

      // They should all be before the terminator.
      EXPECT_EQ(numberOfDaysSummer, cNUM_TEST_POINTS);
   }

   // At an equinox, test those same points.
   {
      UtCalendar equinox2019{};
      equinox2019.SetDate(2019, 3, 20);
      equinox2019.SetTime(21, 58, 0.0);

      int  numberOfDaysSpring{0};
      auto springSum = [&numberOfDaysSpring, &equinox2019](double aLat, double aLon)
      {
         if (WsfSolarTerminator::PhaseOfDay::cDAY == WsfSolarTerminator::GetPhaseOfDay(aLat, aLon, equinox2019))
         {
            ++numberOfDaysSpring;
         }
      };
      for (int i = 0; i < cNUM_TEST_POINTS; ++i)
      {
         springSum(70.0, i * 360.0 / cNUM_TEST_POINTS);
      }

      // Just about half should be beyond the terminator.
      EXPECT_LE(numberOfDaysSpring - cNUM_TEST_POINTS / 2, 1);
   }
}

TEST(WsfSolarTerminator, MaskedByHorizonTest)
{
   // Test locations
   double locationA[3]{UtEarth::cA + 1.0, 0.0, 0.0};
   double locationB[3]{UtEarth::cA - 0.5, 0.0, 0.0};
   double locationC[3]{2.0 * UtEarth::cA, 0.0, 0.0};
   double locationD[3]{-2.0 * UtEarth::cA, 0.0, 0.0};
   double locationE[3]{0.0, 0.0, 0.0};
   double locationF[3]{UtEarth::cA, UtEarth::cA, 0.0};

   EXPECT_TRUE(WsfSolarTerminator::MaskedByHorizon(locationE, locationC));
   EXPECT_TRUE(WsfSolarTerminator::MaskedByHorizon(locationC, locationE));

   EXPECT_FALSE(WsfSolarTerminator::MaskedByHorizon(locationA, locationC));
   EXPECT_FALSE(WsfSolarTerminator::MaskedByHorizon(locationC, locationA));

   EXPECT_FALSE(WsfSolarTerminator::MaskedByHorizon(locationA, locationF));
   EXPECT_FALSE(WsfSolarTerminator::MaskedByHorizon(locationF, locationA));

   EXPECT_TRUE(WsfSolarTerminator::MaskedByHorizon(locationB, locationC));
   EXPECT_TRUE(WsfSolarTerminator::MaskedByHorizon(locationC, locationB));

   EXPECT_TRUE(WsfSolarTerminator::MaskedByHorizon(locationB, locationF));
   EXPECT_TRUE(WsfSolarTerminator::MaskedByHorizon(locationF, locationB));

   EXPECT_TRUE(WsfSolarTerminator::MaskedByHorizon(locationB, locationD));
   EXPECT_TRUE(WsfSolarTerminator::MaskedByHorizon(locationD, locationB));
}

TEST(WsfSolarTerminator, EquatorToAndFromSun)
{
   double sunLoc[3];
   UtSun::GetSunLocationWCS(UtCalendar{}, sunLoc);

   double testLoc[3];
   for (double lon = 0.0; lon < 360.0; lon += 10.0)
   {
      testLoc[0] = UtEarth::cA * std::cos(lon * UtMath::cRAD_PER_DEG);
      testLoc[1] = UtEarth::cA * std::sin(lon * UtMath::cRAD_PER_DEG);
      testLoc[2] = 0.0;

      double dot      = testLoc[0] * sunLoc[0] + testLoc[1] * sunLoc[1] + testLoc[2] * sunLoc[2];
      bool   expected = dot <= 0.0;

      EXPECT_EQ(WsfSolarTerminator::MaskedByHorizon(sunLoc, testLoc), expected);
      EXPECT_EQ(WsfSolarTerminator::MaskedByHorizon(testLoc, sunLoc), expected);
   }
}
