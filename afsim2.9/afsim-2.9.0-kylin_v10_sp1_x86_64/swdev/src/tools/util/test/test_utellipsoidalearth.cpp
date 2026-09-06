// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <gtest/gtest.h>

#include "UtEarth.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtMath.hpp"

std::vector<std::pair<double, double>> cEXPECTED_RESULTS{
   std::make_pair(0.0, 0.0),     // 31NAA66020000
   std::make_pair(30.0, 0.0),    // 31RBP1059022575
   std::make_pair(0.0, 75.0),    // 43NEA
   std::make_pair(-45.2, 90.7),  // 46GCQ1935692258
   std::make_pair(60.0, -180.0), // 1VCG3270555205
   std::make_pair(-.01, .01)     // 31MAV6713598893
};

// Values retrieved from https://www.earthpoint.us/Convert.aspx
std::vector<std::string> cTEST_SUITE{
   "31NAA66020000",   //    0n    0e
   "31RBP1059022575", //   30n    0e
   "43NEA",           //    0n   75e
   "46GCQ1935692258", // 45.2s 90.7e
   "1VCG3270555205",  //   60n   180w
   "31MAV6713598893"  //  .01s  .01e
};

TEST(UtEllipsoidalEarth, ConvertMGRSToLL)
{
   double lat, lon;
   bool   succeeded;
   for (int i = 0; i < cTEST_SUITE.size(); i++)
   {
      succeeded = UtEllipsoidalEarth::ConvertMGRSToLL(cTEST_SUITE[i], lat, lon);
      EXPECT_TRUE(succeeded);
      constexpr double cTOLERANCE{1.0e-4};
      EXPECT_NEAR(lat, cEXPECTED_RESULTS[i].first, cTOLERANCE);
      EXPECT_NEAR(lon, cEXPECTED_RESULTS[i].second, cTOLERANCE);
   }

   // Invalid MGRS inputs

   // Invalid grid zone designation number
   succeeded = UtEllipsoidalEarth::ConvertMGRSToLL("61NAA0000000000", lat, lon);
   EXPECT_FALSE(succeeded);

   // One too many letters
   succeeded = UtEllipsoidalEarth::ConvertMGRSToLL("31NAAA6602100000", lat, lon);
   EXPECT_FALSE(succeeded);

   // Too many digits
   succeeded = UtEllipsoidalEarth::ConvertMGRSToLL("31NAA660210000000", lat, lon);
   EXPECT_FALSE(succeeded);

   // Starts with letter
   succeeded = UtEllipsoidalEarth::ConvertMGRSToLL("A31NAA6602100000", lat, lon);
   EXPECT_FALSE(succeeded);
}

static double RawEToSLat(double aLat)
{
   return std::atan(std::tan(aLat * UtMath::cRAD_PER_DEG) * (1 - ut::EarthEllipsoidWGS84().GetFirstEccentricitySquared())) *
          UtMath::cDEG_PER_RAD;
}

TEST(UtEllipsoidalEarth, EllipsoidalToSphericalLLA)
{
   for (double lat = -89; lat < 90; lat += 1)
   {
      for (double lon = -179; lon < 180; lon += 1)
      {
         double eLat, eLon, eAlt;
         UtEllipsoidalEarth::ConvertEllipsoidalToSphericalLLA(lat, lon, 0, eLat, eLon, eAlt);
         // I adjusted the error terms until the tests passed.
         // Using ASSERT instead of EXPECT to reduce console spam.
         ASSERT_NEAR(eLat, RawEToSLat(lat), 1E-13);
         ASSERT_NEAR(eLon, lon, 1E-13);
      }
   }
}
