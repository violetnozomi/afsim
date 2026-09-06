// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <vector>

#include <gtest/gtest.h>

#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "WsfEquatorialFieldOfView.hpp"

namespace
{
// Check whether the given distance on the sensor (in ECS) is within the field of view.
bool CheckDistance(const double                    aDistance,
                   const double                    aAz,
                   const double                    aEl,
                   const double                    aLatDeg,
                   const double                    aRotationAngle,
                   const WsfEquatorialFieldOfView& aFov)
{
   const double y      = aDistance * cos(aRotationAngle);
   const double z      = aDistance * sin(aRotationAngle);
   const double x      = 1; // x = sqrt(y * y + z * z) / aDistance, since aDistance is tan(theta)
   const double edgeAz = atan2(y, x);
   const double edgeEl = -atan(z / sqrt(x * x + y * y));

   return aFov.WithinFieldOfView(edgeAz, edgeEl, aAz, aEl, aLatDeg);
}

// Check whether the given angle on the sensor is within the field of view, based on this angle being the field of view
// size directly to the field of view edge.
bool CheckEdge(const double                    aAngleSize,
               const double                    aAz,
               const double                    aEl,
               const double                    aLatDeg,
               const double                    aRotationAngle,
               const WsfEquatorialFieldOfView& aFov)
{
   return CheckDistance(tan(aAngleSize), aAz, aEl, aLatDeg, aRotationAngle, aFov);
}

// Check whether the given angle on the sensor is within the field of view, based on this angle being the field of view
// size to the corner.
bool CheckCorner(const double                    aAngleSize,
                 const double                    aAz,
                 const double                    aEl,
                 const double                    aLatDeg,
                 const double                    aRotationAngle,
                 const WsfEquatorialFieldOfView& aFov)
{
   // sqrt(2) to convert field of view size to distance to field of view corner
   return CheckDistance(sqrt(2) * tan(aAngleSize), aAz, aEl, aLatDeg, aRotationAngle, aFov);
}
} // namespace

// Check equatorial field of view at the north pole, where horizon az/el matches equatorial azimuth/polar, so it's
// easier to figure out what tests should look like.
TEST(WsfEquatorialFieldOfView, WithinFieldOfViewAtNorthPole)
{
   // Asymmetric field of view sizes
   const double             eqFovPositiveRad    = 0.02;
   const double             eqFovNegativeRad    = -0.01;
   const double             polarFovPositiveRad = 0.015;
   const double             polarFovNegativeRad = -0.012;
   const double             sensorLatDeg        = 90;
   WsfEquatorialFieldOfView equatorialFov;
   equatorialFov.SetEquatorialFieldOfView(eqFovNegativeRad, eqFovPositiveRad);
   equatorialFov.SetPolarFieldOfView(polarFovNegativeRad, polarFovPositiveRad);

   const double eqPositivePlusEpsilon     = eqFovPositiveRad + 1E-6;
   const double eqPositiveMinusEpsilon    = eqFovPositiveRad - 1E-6;
   const double eqNegativePlusEpsilon     = eqFovNegativeRad + 1E-6;
   const double eqNegativeMinusEpsilon    = eqFovNegativeRad - 1E-6;
   const double polarPositivePlusEpsilon  = polarFovPositiveRad + 1E-6;
   const double polarPositiveMinusEpsilon = polarFovPositiveRad - 1E-6;
   const double polarNegativePlusEpsilon  = polarFovNegativeRad + 1E-6;
   const double polarNegativeMinusEpsilon = polarFovNegativeRad - 1E-6;

   const double cuedAz1 = 4;
   const double cuedEl1 = 0;
   EXPECT_TRUE(equatorialFov.WithinFieldOfView(0, 0, cuedAz1, cuedEl1, sensorLatDeg));
   EXPECT_TRUE(equatorialFov.WithinFieldOfView(eqFovPositiveRad, 0, cuedAz1, cuedEl1, sensorLatDeg));

   EXPECT_TRUE(equatorialFov.WithinFieldOfView(eqPositiveMinusEpsilon, 0, cuedAz1, cuedEl1, sensorLatDeg));
   EXPECT_FALSE(equatorialFov.WithinFieldOfView(eqPositivePlusEpsilon, 0, cuedAz1, cuedEl1, sensorLatDeg));
   EXPECT_TRUE(equatorialFov.WithinFieldOfView(eqNegativePlusEpsilon, 0, cuedAz1, cuedEl1, sensorLatDeg));
   EXPECT_FALSE(equatorialFov.WithinFieldOfView(eqNegativeMinusEpsilon, 0, cuedAz1, cuedEl1, sensorLatDeg));
   EXPECT_TRUE(equatorialFov.WithinFieldOfView(0, polarPositiveMinusEpsilon, cuedAz1, cuedEl1, sensorLatDeg));
   EXPECT_FALSE(equatorialFov.WithinFieldOfView(0, polarPositivePlusEpsilon, cuedAz1, cuedEl1, sensorLatDeg));
   EXPECT_TRUE(equatorialFov.WithinFieldOfView(0, polarNegativePlusEpsilon, cuedAz1, cuedEl1, sensorLatDeg));
   EXPECT_FALSE(equatorialFov.WithinFieldOfView(0, polarNegativeMinusEpsilon, cuedAz1, cuedEl1, sensorLatDeg));

   const double cuedAz2 = UtMath::cPI;
   const double cuedEl2 = 0.5;
   EXPECT_TRUE(equatorialFov.WithinFieldOfView(0, 0, cuedAz2, cuedEl2, sensorLatDeg));
   EXPECT_TRUE(equatorialFov.WithinFieldOfView(eqFovPositiveRad - 1E-6, 0, cuedAz2, cuedEl2, sensorLatDeg));
   EXPECT_FALSE(equatorialFov.WithinFieldOfView(eqFovPositiveRad + 1E-6, 0, cuedAz2, cuedEl2, sensorLatDeg));
   EXPECT_TRUE(equatorialFov.WithinFieldOfView(eqFovNegativeRad + 1E-6, 0, cuedAz2, cuedEl2, sensorLatDeg));
   EXPECT_FALSE(equatorialFov.WithinFieldOfView(eqFovNegativeRad - 1E-6, 0, cuedAz2, cuedEl2, sensorLatDeg));
   EXPECT_TRUE(equatorialFov.WithinFieldOfView(0, polarPositiveMinusEpsilon, cuedAz2, cuedEl2, sensorLatDeg));
   EXPECT_FALSE(equatorialFov.WithinFieldOfView(0, polarPositivePlusEpsilon, cuedAz2, cuedEl2, sensorLatDeg));
   EXPECT_TRUE(equatorialFov.WithinFieldOfView(0, polarNegativePlusEpsilon, cuedAz2, cuedEl2, sensorLatDeg));
   EXPECT_FALSE(equatorialFov.WithinFieldOfView(0, polarNegativeMinusEpsilon, cuedAz2, cuedEl2, sensorLatDeg));

   // Symmetric field of view, test all sides and corners.
   const double deltaX = 0.15;
   const double deltaY = 0.27;
   equatorialFov.SetEquatorialFieldOfView(-deltaX, deltaX);
   equatorialFov.SetPolarFieldOfView(-deltaY, deltaY);
   std::vector<std::pair<int, int>> signsVec = {{1, 1}, {1, -1}, {-1, -1}, {-1, 1}};
   for (const auto& signs : signsVec)
   {
      // Corner closest to origin
      EXPECT_TRUE(equatorialFov.WithinFieldOfView(-signs.first * (deltaX - 1E-6),
                                                  -signs.second * (deltaY - 1E-6),
                                                  signs.first * deltaX,
                                                  signs.second * deltaY,
                                                  sensorLatDeg));
      EXPECT_FALSE(equatorialFov.WithinFieldOfView(-signs.first * (deltaX + 1E-6),
                                                   -signs.second * (deltaY + 1E-6),
                                                   signs.first * deltaX,
                                                   signs.second * deltaY,
                                                   sensorLatDeg));
      EXPECT_FALSE(equatorialFov.WithinFieldOfView(-signs.first * deltaX,
                                                   -signs.second * (deltaY + 1E-6),
                                                   signs.first * deltaX,
                                                   signs.second * deltaY,
                                                   sensorLatDeg));
      EXPECT_FALSE(equatorialFov.WithinFieldOfView(-signs.first * (deltaX + 1E-6),
                                                   -signs.second * deltaY,
                                                   signs.first * deltaX,
                                                   signs.second * deltaY,
                                                   sensorLatDeg));

      // Corner furthest from origin
      EXPECT_TRUE(equatorialFov.WithinFieldOfView(signs.first * (deltaX - 1E-6),
                                                  signs.second * (deltaY - 1E-6),
                                                  signs.first * deltaX,
                                                  signs.second * deltaY,
                                                  sensorLatDeg));
      EXPECT_FALSE(equatorialFov.WithinFieldOfView(signs.first * (deltaX + 1E-6),
                                                   signs.second * (deltaY + 1E-6),
                                                   signs.first * deltaX,
                                                   signs.second * deltaY,
                                                   sensorLatDeg));
      EXPECT_FALSE(equatorialFov.WithinFieldOfView(signs.first * deltaX,
                                                   signs.second * (deltaY + 1E-6),
                                                   signs.first * deltaX,
                                                   signs.second * deltaY,
                                                   sensorLatDeg));
      EXPECT_FALSE(equatorialFov.WithinFieldOfView(signs.first * (deltaX + 1E-6),
                                                   signs.second * deltaY,
                                                   signs.first * deltaX,
                                                   signs.second * deltaY,
                                                   sensorLatDeg));

      // Other two corners
      EXPECT_TRUE(equatorialFov.WithinFieldOfView(signs.first * (deltaX - 1E-6),
                                                  -signs.second * (deltaY - 1E-6),
                                                  signs.first * deltaX,
                                                  signs.second * deltaY,
                                                  sensorLatDeg));
      EXPECT_FALSE(equatorialFov.WithinFieldOfView(signs.first * (deltaX + 1E-6),
                                                   -signs.second * (deltaY + 1E-6),
                                                   signs.first * deltaX,
                                                   signs.second * deltaY,
                                                   sensorLatDeg));
      EXPECT_FALSE(equatorialFov.WithinFieldOfView(signs.first * deltaX,
                                                   -signs.second * (deltaY + 1E-6),
                                                   signs.first * deltaX,
                                                   signs.second * deltaY,
                                                   sensorLatDeg));
      EXPECT_FALSE(equatorialFov.WithinFieldOfView(signs.first * (deltaX + 1E-6),
                                                   -signs.second * deltaY,
                                                   signs.first * deltaX,
                                                   signs.second * deltaY,
                                                   sensorLatDeg));
      EXPECT_TRUE(equatorialFov.WithinFieldOfView(-signs.first * (deltaX - 1E-6),
                                                  signs.second * (deltaY - 1E-6),
                                                  signs.first * deltaX,
                                                  signs.second * deltaY,
                                                  sensorLatDeg));
      EXPECT_FALSE(equatorialFov.WithinFieldOfView(-signs.first * (deltaX + 1E-6),
                                                   signs.second * (deltaY + 1E-6),
                                                   signs.first * deltaX,
                                                   signs.second * deltaY,
                                                   sensorLatDeg));
      EXPECT_FALSE(equatorialFov.WithinFieldOfView(-signs.first * deltaX,
                                                   signs.second * (deltaY + 1E-6),
                                                   signs.first * deltaX,
                                                   signs.second * deltaY,
                                                   sensorLatDeg));
      EXPECT_FALSE(equatorialFov.WithinFieldOfView(-signs.first * (deltaX + 1E-6),
                                                   signs.second * deltaY,
                                                   signs.first * deltaX,
                                                   signs.second * deltaY,
                                                   sensorLatDeg));
   }
}

// Test equatorial field of view with a 45 degree parallactic angle (special case with easily calculated known output).
TEST(WsfEquatorialFieldOfView, WithinFieldOfView45DegParallacticAngle)
{
   const double latitudeDeg    = 0.35 * UtMath::cDEG_PER_RAD;
   const double fovAngularSize = 0.1;

   // This azimuth and elevation, with the latitude above, will produce a parallactic angle of 45 degrees.
   const double az45 = 3.9894733306044294;
   const double el45 = 1.1896887638451656;

   WsfEquatorialFieldOfView equatorialFov;

   equatorialFov.SetEquatorialFieldOfView(-fovAngularSize, fovAngularSize);
   equatorialFov.SetPolarFieldOfView(-fovAngularSize, fovAngularSize);

   const double fovDistance         = tan(fovAngularSize);
   const double cornerDistance      = sqrt(2 * fovDistance * fovDistance);
   const double cornerDistanceAngle = atan(cornerDistance);
   const double epsilon             = 1E-9;

   EXPECT_TRUE(equatorialFov.WithinFieldOfView(0, 0, az45, el45, latitudeDeg));

   // Corners
   std::vector<int> signVec = {1, -1};
   for (auto sign : signVec)
   {
      const double cornerDistanceAngleMinus = cornerDistanceAngle - epsilon;
      const double cornerDistanceAnglePlus  = cornerDistanceAngle + epsilon;
      EXPECT_TRUE(equatorialFov.WithinFieldOfView(sign * cornerDistanceAngleMinus, 0, az45, el45, latitudeDeg));
      EXPECT_FALSE(equatorialFov.WithinFieldOfView(sign * cornerDistanceAnglePlus, 0, az45, el45, latitudeDeg));
      EXPECT_TRUE(equatorialFov.WithinFieldOfView(0, sign * cornerDistanceAngleMinus, az45, el45, latitudeDeg));
      EXPECT_FALSE(equatorialFov.WithinFieldOfView(0, sign * cornerDistanceAnglePlus, az45, el45, latitudeDeg));
   }

   // Edges
   for (int i = 0; i < 4; ++i)
   {
      // The pi/4 in the rotation angle points to the equatorial field of view edge (45 degree parallactic angle), and
      // the iteration over the i*pi/2 factor tests all four edges.
      EXPECT_TRUE(
         CheckEdge(fovAngularSize - epsilon, az45, el45, latitudeDeg, UtMath::cPI_OVER_4 + i * UtMath::cPI_OVER_2, equatorialFov));
      EXPECT_FALSE(
         CheckEdge(fovAngularSize + epsilon, az45, el45, latitudeDeg, UtMath::cPI_OVER_4 + i * UtMath::cPI_OVER_2, equatorialFov));
   }
}

// Test equatorial field of view with arbitrary pointings.
TEST(WsfEquatorialFieldOfView, WithinFieldOfViewArbitraryAngle)
{
   const double              latitudeDeg    = -0.2;
   const double              fovAngularSize = 0.16;
   const std::vector<double> azVec          = {0.02, 2.5, 5.7};
   const std::vector<double> elVec          = {0.03, 0.8, 1.35};

   WsfEquatorialFieldOfView equatorialFov;

   equatorialFov.SetEquatorialFieldOfView(-fovAngularSize, fovAngularSize);
   equatorialFov.SetPolarFieldOfView(-fovAngularSize, fovAngularSize);

   const double epsilon = 1E-9;

   for (const double az : azVec)
   {
      for (const double el : elVec)
      {
         EXPECT_TRUE(equatorialFov.WithinFieldOfView(0, 0, az, el, latitudeDeg));
         const double parallacticAngle = UtSphericalEarth::ConvertAzElToParallacticAngle(az, el, latitudeDeg);
         // Corners
         const double cornerAnglePlusRotation =
            parallacticAngle + UtMath::cPI_OVER_4; // Add pi/4 to get set on the first corner.
         for (int i = 0; i < 4; ++i)
         {
            // The iteration over the i*pi/2 factor tests all four corners.
            EXPECT_TRUE(CheckCorner(fovAngularSize - epsilon,
                                    az,
                                    el,
                                    latitudeDeg,
                                    cornerAnglePlusRotation + i * UtMath::cPI_OVER_2,
                                    equatorialFov));
            EXPECT_FALSE(CheckCorner(fovAngularSize + epsilon,
                                     az,
                                     el,
                                     latitudeDeg,
                                     cornerAnglePlusRotation + i * UtMath::cPI_OVER_2,
                                     equatorialFov));
         }

         // Edges
         for (int i = 0; i < 4; ++i)
         {
            // The iteration over the i*pi/2 factor tests all four edges.
            EXPECT_TRUE(
               CheckEdge(fovAngularSize - epsilon, az, el, latitudeDeg, parallacticAngle + i * UtMath::cPI_OVER_2, equatorialFov));
            EXPECT_FALSE(
               CheckEdge(fovAngularSize + epsilon, az, el, latitudeDeg, parallacticAngle + i * UtMath::cPI_OVER_2, equatorialFov));
         }
      }
   }
}
