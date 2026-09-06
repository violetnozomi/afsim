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

#include <vector>

#include <gtest/gtest.h>

#include "UtEllipsoidalEarth.hpp"
#include "UtMath.hpp"
#include "UtMeasurementUtil.hpp"
#include "UtRandom.hpp"
#include "UtSphericalEarth.hpp"

TEST(UtMeasurementUtil, GroundRange)
{
   ut::Random random;

   // Random point above the surface of the Earth.
   // This is the control point that will be used for all tests
   double controlEcef[3] = {0.0, 0.0, 0.0};
   double controlLat     = random.Uniform(-90.0, 90.0);
   double controlLon     = random.Uniform(-180.0, 180.0);
   double controlAlt     = random.Uniform(0.0, 1.0e5);
   UtEllipsoidalEarth::ConvertLLAToECEF(controlLat, controlLon, controlAlt, controlEcef);

   // Test locations: Edge cases plus a random point on the Earth.
   std::vector<double> testLatitudes  = {-90.0, 0.0, 90.0, random.Uniform(-90.0, 90.0)};
   std::vector<double> testLongitudes = {-180.0, -90.0, 0.0, 90.0, 180.0, random.Uniform(-180.0, 180.0)};
   std::vector<double> testAltitudes  = {1e3, 1e4, 1e5, 1e6, 1e7}; // 1 km to 10000 km

   for (double latitude : testLatitudes)
   {
      for (double longitude : testLongitudes)
      {
         double testEcef0[3] = {0.0, 0.0, 0.0};
         UtEllipsoidalEarth::ConvertLLAToECEF(latitude, longitude, 0.0, testEcef0);

         // Ground Range from control point to point on Earth's surface
         double groundRange0 = UtMeasurementUtil::GroundRange(controlEcef, testEcef0);

         for (double altitude : testAltitudes)
         {
            double testEcef1[3] = {0.0, 0.0, 0.0};
            UtEllipsoidalEarth::ConvertLLAToECEF(latitude, longitude, altitude, testEcef1);

            // Ground Range from control point to point above the Earth's surface
            double groundRange1 = UtMeasurementUtil::GroundRange(controlEcef, testEcef1);

            EXPECT_NEAR(groundRange0, groundRange1, 1e-8 * altitude);
         }
      }
   }

   // Edge cases:
   // [0n 0e] to [0n 180e] should be half the spherical earth's circumference
   // [0n 0e] to [90n 0e] should be one quarter the spherical earth's circumference
   // [0n 180e] to [90n 0e] should be one quarter the spherical earth's circumference
   double testEcef2[3] = {0.0, 0.0, 0.0};
   UtEllipsoidalEarth::ConvertLLAToECEF(0.0, 0.0, 0.0, testEcef2);
   double testEcef3[3] = {0.0, 0.0, 0.0};
   UtEllipsoidalEarth::ConvertLLAToECEF(0.0, 180.0, 0.0, testEcef3);
   double testEcef4[3] = {0.0, 0.0, 0.0};
   UtEllipsoidalEarth::ConvertLLAToECEF(90.0, 0.0, 0.0, testEcef4);

   EXPECT_NEAR(UtMeasurementUtil::GroundRange(testEcef2, testEcef3), UtSphericalEarth::cEARTH_RADIUS * UtMath::cPI, 1e-8);
   EXPECT_NEAR(UtMeasurementUtil::GroundRange(testEcef2, testEcef4),
               0.5 * UtSphericalEarth::cEARTH_RADIUS * UtMath::cPI,
               1e-8);
   EXPECT_NEAR(UtMeasurementUtil::GroundRange(testEcef3, testEcef4),
               0.5 * UtSphericalEarth::cEARTH_RADIUS * UtMath::cPI,
               1e-8);
}

TEST(UtMeasurementUtil, SlantRange)
{
   struct LatLonCoords
   {
      double mLat;
      double mLon;
      LatLonCoords(double lat, double lon)
         : mLat(lat)
         , mLon(lon){};
   };

   // building four 45-45-90 triangles, short sides are the earth's radius
   // these points are the points at the right angles of the triangles
   std::vector<LatLonCoords> testPointsLatLon = {LatLonCoords(45.0, 0.0),
                                                 LatLonCoords(0.0, 45.0),
                                                 LatLonCoords(-45.0, 0.0),
                                                 LatLonCoords(0.0, -45.0)};

   // the ecef point is one of the 45-deg points, shared by all triangles
   // the other is earth's center, also shared
   // the slant range between the two points should = the earth's radius, due to nature of 45-45-90
   double controlSlantRange = UtSphericalEarth::cEARTH_RADIUS;
   double controlAlt        = controlSlantRange * std::sqrt(2) - controlSlantRange;
   double controlEcef[3]    = {0.0, 0.0, 0.0};
   UtEllipsoidalEarth::ConvertLLAToECEF(0.0, 0.0, controlAlt, controlEcef);

   for (const auto& ptLatLon : testPointsLatLon)
   {
      double testEcef[3] = {0.0, 0.0, 0.0};
      UtEllipsoidalEarth::ConvertLLAToECEF(ptLatLon.mLat, ptLatLon.mLon, 0.0, testEcef);
      double testSlantRange = UtMeasurementUtil::SlantRange(controlEcef, testEcef);

      EXPECT_NEAR(testSlantRange, controlSlantRange, 1e-7);
   }
}
