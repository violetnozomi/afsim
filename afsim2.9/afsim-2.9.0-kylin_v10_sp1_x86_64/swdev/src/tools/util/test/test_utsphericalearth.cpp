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

#include "UtEarth.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"

namespace
{
constexpr double latDegFrom0p8Rad = 0.8 * UtMath::cDEG_PER_RAD;
constexpr double latDegFrom0p6Rad = 0.6 * UtMath::cDEG_PER_RAD;
} // namespace

static double RawSToELat(double aLat)
{
   return std::atan(std::tan(aLat * UtMath::cRAD_PER_DEG) / (1 - ut::EarthEllipsoidWGS84().GetFirstEccentricitySquared())) *
          UtMath::cDEG_PER_RAD;
}

TEST(UtSphericalEarth, SphericalToEllipsoidalLLA)
{
   for (double lat = -89; lat < 90; lat += 1)
   {
      for (double lon = -179; lon < 180; lon += 1)
      {
         double eLat, eLon, eAlt;
         UtSphericalEarth::ConvertSphericalToEllipsoidalLLA(lat, lon, 0, eLat, eLon, eAlt);
         // I adjusted the error terms until the tests passed.
         // Using ASSERT instead of EXPECT to reduce console spam.
         ASSERT_NEAR(eLat, RawSToELat(lat), 2E-4);
         ASSERT_NEAR(eLon, lon, 1E-13);
      }
   }
}

// Test calculation of declination from azimuth/elevation.
TEST(UtSphericalEarth, DeclinationFromAzEl)
{
   // Testing at north pole, where elevation equals declination
   const double northPoleLatitudeDeg = 90;
   const double azPoles1             = 1.;
   const double elPoles1             = 0.;
   const double azPoles2             = 2.;
   const double elPoles2             = UtMath::cPI_OVER_4;
   const double azPoles3             = 5.4;
   const double elPoles3             = 1.34;

   EXPECT_NEAR(elPoles1, UtSphericalEarth::ConvertAzElToDeclination(azPoles1, elPoles1, northPoleLatitudeDeg), 1E-6);
   EXPECT_NEAR(elPoles2, UtSphericalEarth::ConvertAzElToDeclination(azPoles2, elPoles2, northPoleLatitudeDeg), 1E-6);
   EXPECT_NEAR(elPoles3, UtSphericalEarth::ConvertAzElToDeclination(azPoles3, elPoles3, northPoleLatitudeDeg), 1E-6);

   // Testing at south pole, where elevation equals negative declination
   const double southPoleLatitudeDeg = -90;
   EXPECT_NEAR(-elPoles1, UtSphericalEarth::ConvertAzElToDeclination(azPoles1, elPoles1, southPoleLatitudeDeg), 1E-6);
   EXPECT_NEAR(-elPoles2, UtSphericalEarth::ConvertAzElToDeclination(azPoles2, elPoles2, southPoleLatitudeDeg), 1E-6);
   EXPECT_NEAR(-elPoles3, UtSphericalEarth::ConvertAzElToDeclination(azPoles3, elPoles3, southPoleLatitudeDeg), 1E-6);

   // Testing at equator, where along azimuths of pi/2 and 3 pi/2, declination should be zero.
   EXPECT_NEAR(0, UtSphericalEarth::ConvertAzElToDeclination(UtMath::cPI_OVER_2, 1.342423, 0), 1E-6);
   EXPECT_NEAR(0, UtSphericalEarth::ConvertAzElToDeclination(3 * UtMath::cPI_OVER_2, 0.023, 0), 1E-6);

   // Testing at equator, where along azimuths of zero and pi, declination should be 90 degrees - elevation (plus a
   // negative sign for south pointing)
   const double elEquator3 = 1.2;
   const double elEquator4 = 0.342;
   EXPECT_NEAR(-(UtMath::cPI_OVER_2 - elEquator3),
               UtSphericalEarth::ConvertAzElToDeclination(UtMath::cPI, elEquator3, 0),
               1E-6);
   EXPECT_NEAR(UtMath::cPI_OVER_2 - elEquator4, UtSphericalEarth::ConvertAzElToDeclination(0, elEquator4, 0), 1E-6);

   // Using azimuth as declination (from equator)
   const double az1 = 1.;
   EXPECT_NEAR(UtMath::cPI_OVER_2 - az1, UtSphericalEarth::ConvertAzElToDeclination(az1, 0., 0.), 1E-6);

   const double az2 = 2.782839;
   EXPECT_NEAR(UtMath::cPI_OVER_2 - az2, UtSphericalEarth::ConvertAzElToDeclination(az2, 0., 0.), 1E-6);

   EXPECT_NEAR(0, UtSphericalEarth::ConvertAzElToDeclination(0.82374237, UtMath::cPI_OVER_2, 0), 1E-6);

   // Comparison values from astropy
   EXPECT_NEAR(25 * UtMath::cRAD_PER_DEG,
               UtSphericalEarth::ConvertAzElToDeclination(287.64254335 * UtMath::cRAD_PER_DEG,
                                                          19.97480854 * UtMath::cRAD_PER_DEG,
                                                          32),
               1E-4);

   EXPECT_NEAR(-79 * UtMath::cRAD_PER_DEG,
               UtSphericalEarth::ConvertAzElToDeclination(190.86689268 * UtMath::cRAD_PER_DEG,
                                                          -0.29115028 * UtMath::cRAD_PER_DEG,
                                                          2),
               1E-4);

   EXPECT_NEAR(-67.3 * UtMath::cRAD_PER_DEG,
               UtSphericalEarth::ConvertAzElToDeclination(225.36128948 * UtMath::cRAD_PER_DEG,
                                                          57.59453106 * UtMath::cRAD_PER_DEG,
                                                          -62.3),
               1E-4);

   EXPECT_NEAR(20.2 * UtMath::cRAD_PER_DEG,
               UtSphericalEarth::ConvertAzElToDeclination(291.55391096 * UtMath::cRAD_PER_DEG,
                                                          4.31044816 * UtMath::cRAD_PER_DEG,
                                                          -11),
               1E-4);
}

// Test calculation of hour angle from azimuth/elevation.
TEST(UtSphericalEarth, HourAngle)
{
   const double latDegFrom1Rad = UtMath::cDEG_PER_RAD;

   // Test along local meridian
   EXPECT_NEAR(0., UtSphericalEarth::ConvertAzElToHourAngle(0, UtMath::cPI_OVER_2, latDegFrom1Rad), 1E-6);
   EXPECT_NEAR(0., UtSphericalEarth::ConvertAzElToHourAngle(UtMath::cPI, UtMath::cPI_OVER_2, latDegFrom1Rad), 1E-6);
   EXPECT_NEAR(0., UtSphericalEarth::ConvertAzElToHourAngle(UtMath::cPI, 1, latDegFrom1Rad), 1E-6);
   EXPECT_NEAR(0., UtSphericalEarth::ConvertAzElToHourAngle(UtMath::cPI, 0, latDegFrom1Rad), 1E-6);
   EXPECT_NEAR(0., UtSphericalEarth::ConvertAzElToHourAngle(0, UtMath::cPI_OVER_2, -latDegFrom1Rad), 1E-6);
   EXPECT_NEAR(0., UtSphericalEarth::ConvertAzElToHourAngle(UtMath::cPI, UtMath::cPI_OVER_2, -latDegFrom1Rad), 1E-6);
   EXPECT_NEAR(UtMath::cPI,
               UtMath::NormalizeAngle0_TwoPi(UtSphericalEarth::ConvertAzElToHourAngle(0, 0, latDegFrom1Rad)),
               1E-6);
   EXPECT_NEAR(UtMath::cPI,
               UtMath::NormalizeAngle0_TwoPi(UtSphericalEarth::ConvertAzElToHourAngle(UtMath::cPI, 0, -latDegFrom1Rad)),
               1E-6);
   // Special meridian case: test pointing directly at north/south pole, handled specially by the function.
   const double latElValue = 1.1;
   EXPECT_EQ(0, UtSphericalEarth::ConvertAzElToHourAngle(0, latElValue, latElValue * UtMath::cDEG_PER_RAD));
   EXPECT_EQ(0, UtSphericalEarth::ConvertAzElToHourAngle(UtMath::cPI, -latElValue, latElValue * UtMath::cDEG_PER_RAD));
   EXPECT_EQ(0, UtSphericalEarth::ConvertAzElToHourAngle(UtMath::cPI, latElValue, -latElValue * UtMath::cDEG_PER_RAD));
   EXPECT_EQ(0, UtSphericalEarth::ConvertAzElToHourAngle(0, -latElValue, -latElValue * UtMath::cDEG_PER_RAD));

   // Test along direct east/west
   EXPECT_NEAR(0, UtSphericalEarth::ConvertAzElToHourAngle(UtMath::cPI_OVER_2, UtMath::cPI_OVER_2, 0), 1E-6);
   EXPECT_NEAR(-UtMath::cPI_OVER_2, UtSphericalEarth::ConvertAzElToHourAngle(UtMath::cPI_OVER_2, 0, 0), 1E-6);
   EXPECT_NEAR(0, UtSphericalEarth::ConvertAzElToHourAngle(3 * UtMath::cPI_OVER_2, UtMath::cPI_OVER_2, 0), 1E-6);
   EXPECT_NEAR(UtMath::cPI_OVER_2, UtSphericalEarth::ConvertAzElToHourAngle(3 * UtMath::cPI_OVER_2, 0, 0), 1E-6);

   // Telescope located at the north pole, where azimuth and hour angle are directly related and elevation doesn't matter
   EXPECT_NEAR(UtMath::cPI, UtMath::NormalizeAngle0_TwoPi(UtSphericalEarth::ConvertAzElToHourAngle(0, 0, 90)), 1E-6);
   const std::vector<double> azArr1{0.0001, 1.3423234, 2.05, UtMath::cPI, 4.002342, 5.98};
   const std::vector<double> elArr1{0, 0.34, 0.59, 1.0, 1.55};
   for (const double az : azArr1)
   {
      for (const double el : elArr1)
      {
         EXPECT_NEAR(az - UtMath::cPI, UtSphericalEarth::ConvertAzElToHourAngle(az, el, 90), 1E-6);
      }
   }

   // The equator, where hour angle is (positive, negative) elevation when azimuth is directly east or west.
   const std::vector<double> azArr2{UtMath::cPI_OVER_2, 3 * UtMath::cPI_OVER_2};
   const std::vector<double> elArr2{0, 0.34, UtMath::cPI_OVER_4, 1.1, 1.55, UtMath::cPI_OVER_2};
   for (const double az : azArr2)
   {
      for (const double el : elArr2)
      {
         const double hourAngle = copysign(UtMath::cPI_OVER_2 - el, -sin(az));
         EXPECT_NEAR(hourAngle, UtSphericalEarth::ConvertAzElToHourAngle(az, el, 0), 1E-5);
      }
   }

   // Make sure the local meridian crossing is hour angle zero.
   const std::vector<double> azArr3{0.,
                                    0.1,
                                    UtMath::cPI_OVER_4,
                                    1.2,
                                    UtMath::cPI_OVER_2,
                                    2.0,
                                    UtMath::cPI,
                                    4.,
                                    3 * UtMath::cPI_OVER_2,
                                    7 * UtMath::cPI_OVER_4,
                                    6.2};
   const std::vector<double> latArr3{-90 * 0.99999, -60, -10, 0, 0.001, 22, 75, 90 * 0.99999};
   for (const double az : azArr3)
   {
      for (const double lat : latArr3)
      {
         EXPECT_NEAR(0, UtSphericalEarth::ConvertAzElToHourAngle(0., UtMath::cPI_OVER_2, lat), 1E-6);
      }
   }

   // The following test data were calculated by astropy.
   // The delta values (1E-4) are fairly large of necessity, this is likely due to the indirect way we of necessity
   // calculate this in astropy, which requires using RA and declination values, and in turn are converted to
   // azimuth/elevation and hour angle using an equinox epoch and earth orientation parameters.  We tried to control for
   // these as much as possible to match the calculation we use but some residuals appear to still exist.
   const double ha1 = 1.4170396299105008;
   const double ha2 = 1.3995863373905577;
   const double ha3 = -2.59721764968;

   // Double number of tests, and test on both east and west sides, by negating the azimuth and the hour angle.
   for (int sign : {-1, 1})
   {
      EXPECT_NEAR(sign * ha1, UtSphericalEarth::ConvertAzElToHourAngle(sign * 5.06877, 0.144179, 0), 1E-4);
      EXPECT_NEAR(sign * ha1,
                  UtSphericalEarth::ConvertAzElToHourAngle(sign * 4.70289, 0.383172, 1.2 * UtMath::cDEG_PER_RAD),
                  1E-4);
      EXPECT_NEAR(sign * ha1,
                  UtSphericalEarth::ConvertAzElToHourAngle(sign * 5.08913, 0.0722854, -0.2 * UtMath::cDEG_PER_RAD),
                  1E-4);
      EXPECT_NEAR(sign * ha2, UtSphericalEarth::ConvertAzElToHourAngle(sign * 4.71235, 0.171148, 0), 1E-4);
      EXPECT_NEAR(sign * ha2, UtSphericalEarth::ConvertAzElToHourAngle(sign * 4.58901, 0.118913, latDegFrom0p8Rad), 1E-4);
      EXPECT_NEAR(sign * ha2, UtSphericalEarth::ConvertAzElToHourAngle(sign * 4.80964, 0.141054, -latDegFrom0p6Rad), 1E-4);
      EXPECT_NEAR(sign * ha2, UtSphericalEarth::ConvertAzElToHourAngle(sign * 5.15435, 0.154984, 0), 1E-4);
      EXPECT_NEAR(sign * ha2, UtSphericalEarth::ConvertAzElToHourAngle(sign * 4.91524, 0.423212, latDegFrom0p8Rad), 1E-4);
      EXPECT_NEAR(sign * ha2, UtSphericalEarth::ConvertAzElToHourAngle(sign * 5.16649, -0.111439, -latDegFrom0p6Rad), 1E-4);
      EXPECT_NEAR(sign * ha2, UtSphericalEarth::ConvertAzElToHourAngle(sign * 3.46522, 0.0548982, 0), 1E-4);
      EXPECT_NEAR(sign * ha2, UtSphericalEarth::ConvertAzElToHourAngle(sign * 3.56803, -0.695624, latDegFrom0p8Rad), 1E-4);
      EXPECT_NEAR(sign * ha2, UtSphericalEarth::ConvertAzElToHourAngle(sign * 3.54192, 0.618498, -latDegFrom0p6Rad), 1E-4);
      EXPECT_NEAR(sign * ha3, UtSphericalEarth::ConvertAzElToHourAngle(sign * 1.57074, -1.02636, 0), 5E-4);
      EXPECT_NEAR(sign * ha3, UtSphericalEarth::ConvertAzElToHourAngle(sign * 0.701007, -0.638452, latDegFrom0p8Rad), 5E-4);
      EXPECT_NEAR(sign * ha3, UtSphericalEarth::ConvertAzElToHourAngle(sign * 2.32128, -0.78387, -latDegFrom0p6Rad), 5E-4);
      EXPECT_NEAR(sign * ha3, UtSphericalEarth::ConvertAzElToHourAngle(sign * 0.837759, -0.887135, 0), 5E-4);
      EXPECT_NEAR(sign * ha3, UtSphericalEarth::ConvertAzElToHourAngle(sign * 0.504255, -0.23922, latDegFrom0p8Rad), 5E-4);
      EXPECT_NEAR(sign * ha3, UtSphericalEarth::ConvertAzElToHourAngle(sign * 1.75802, -1.07271, -latDegFrom0p6Rad), 5E-4);
      EXPECT_NEAR(sign * ha3, UtSphericalEarth::ConvertAzElToHourAngle(sign * 2.96703, -0.279305, 0), 5E-4);
      EXPECT_NEAR(sign * ha3, UtSphericalEarth::ConvertAzElToHourAngle(sign * 2.79467, -1.05754, latDegFrom0p8Rad), 5E-4);
      EXPECT_NEAR(sign * ha3, UtSphericalEarth::ConvertAzElToHourAngle(sign * 2.96526, 0.312016, -latDegFrom0p6Rad), 5E-4);
      EXPECT_NEAR(sign * 3.0751024193051144, UtSphericalEarth::ConvertAzElToHourAngle(sign * 3.28297, -1.12978, 0), 1E-4);
      EXPECT_NEAR(sign * -3.03354996268, UtSphericalEarth::ConvertAzElToHourAngle(sign * 2.91417, -1.12212, 0), 1E-4);
      EXPECT_NEAR(sign * -3.12081642527, UtSphericalEarth::ConvertAzElToHourAngle(sign * 3.0969, -1.13402, 0), 1E-4);
   }
}

// Test calculation of parallactic angle from azimuth/elevation.
TEST(UtSphericalEarth, ParallacticAngle)
{
   // Test north and south azimuth pointings
   const double azPointSouth1 = UtMath::cPI;
   const double elPointSouth1 = 0.;
   const double azPointSouth2 = UtMath::cPI;
   const double elPointSouth2 = 1.28372;
   const double azPointNorth1 = 0;
   const double elPointNorth1 = 0;
   const double azPointNorth2 = 0;
   const double elPointNorth2 = 0.783;

   const double latDegFrom1p213Rad = 1.213 * UtMath::cDEG_PER_RAD;
   const double latDegFrom0p823Rad = 0.823 * UtMath::cDEG_PER_RAD;

   EXPECT_NEAR(0., UtSphericalEarth::ConvertAzElToParallacticAngle(azPointSouth1, elPointSouth1, 0.), 1E-6);
   EXPECT_NEAR(0., UtSphericalEarth::ConvertAzElToParallacticAngle(azPointSouth2, elPointSouth2, 0.), 1E-6);
   EXPECT_NEAR(UtMath::cPI,
               UtMath::NormalizeAngle0_TwoPi(
                  UtSphericalEarth::ConvertAzElToParallacticAngle(azPointNorth1, elPointNorth1, 0.)),
               1E-6);
   EXPECT_NEAR(
      UtMath::cPI,
      UtMath::NormalizeAngle0_TwoPi(
         UtSphericalEarth::ConvertAzElToParallacticAngle(azPointNorth1, elPointNorth1, -0.000001 * UtMath::cDEG_PER_RAD)),
      1E-6);
   EXPECT_NEAR(UtMath::cPI,
               UtMath::NormalizeAngle0_TwoPi(
                  UtSphericalEarth::ConvertAzElToParallacticAngle(azPointNorth2, elPointNorth2, 0.)),
               1E-6);
   EXPECT_NEAR(UtMath::cPI,
               UtMath::NormalizeAngle0_TwoPi(
                  UtSphericalEarth::ConvertAzElToParallacticAngle(azPointNorth2, elPointNorth2, 0.)),
               1E-6);
   EXPECT_NEAR(0.,
               UtMath::NormalizeAngleMinusPi_Pi(
                  UtSphericalEarth::ConvertAzElToParallacticAngle(azPointSouth1, elPointSouth1, latDegFrom1p213Rad)),
               1E-6);
   EXPECT_NEAR(0.,
               UtMath::NormalizeAngleMinusPi_Pi(
                  UtSphericalEarth::ConvertAzElToParallacticAngle(azPointSouth2, elPointSouth2, latDegFrom1p213Rad)),
               1E-6);
   EXPECT_NEAR(0.,
               UtMath::NormalizeAngleMinusPi_Pi(
                  UtSphericalEarth::ConvertAzElToParallacticAngle(azPointSouth2, elPointSouth2, 1.29 * UtMath::cDEG_PER_RAD)),
               1E-6);
   EXPECT_NEAR(0.,
               UtMath::NormalizeAngleMinusPi_Pi(
                  UtSphericalEarth::ConvertAzElToParallacticAngle(azPointNorth1, elPointNorth1, latDegFrom1p213Rad)),
               1E-6);
   EXPECT_NEAR(0.,
               UtMath::NormalizeAngleMinusPi_Pi(
                  UtSphericalEarth::ConvertAzElToParallacticAngle(azPointNorth2, elPointNorth2, latDegFrom1p213Rad)),
               1E-6);
   EXPECT_NEAR(UtMath::cPI,
               UtMath::NormalizeAngle0_TwoPi(
                  UtSphericalEarth::ConvertAzElToParallacticAngle(azPointNorth2, elPointNorth2, 0.77 * UtMath::cDEG_PER_RAD)),
               1E-6);
   EXPECT_NEAR(UtMath::cPI,
               UtMath::NormalizeAngle0_TwoPi(
                  UtSphericalEarth::ConvertAzElToParallacticAngle(azPointSouth1, elPointSouth1, -latDegFrom0p823Rad)),
               1E-6);
   EXPECT_NEAR(0.,
               UtMath::NormalizeAngleMinusPi_Pi(
                  UtSphericalEarth::ConvertAzElToParallacticAngle(azPointSouth2, elPointSouth2, -latDegFrom0p823Rad)),
               1E-6);
   EXPECT_NEAR(UtMath::cPI,
               UtMath::NormalizeAngle0_TwoPi(
                  UtSphericalEarth::ConvertAzElToParallacticAngle(azPointSouth2, elPointSouth2, -1.3 * UtMath::cDEG_PER_RAD)),
               1E-6);
   EXPECT_NEAR(UtMath::cPI,
               UtMath::NormalizeAngle0_TwoPi(
                  UtSphericalEarth::ConvertAzElToParallacticAngle(azPointNorth1, elPointNorth1, -latDegFrom0p823Rad)),
               1E-6);
   EXPECT_NEAR(UtMath::cPI,
               UtMath::NormalizeAngle0_TwoPi(
                  UtSphericalEarth::ConvertAzElToParallacticAngle(azPointNorth2, elPointNorth2, -latDegFrom0p823Rad)),
               1E-6);
   EXPECT_NEAR(UtMath::cPI,
               UtMath::NormalizeAngle0_TwoPi(
                  UtSphericalEarth::ConvertAzElToParallacticAngle(UtMath::cPI, 0.00, -latDegFrom0p823Rad)),
               1E-6);
   EXPECT_NEAR(0,
               UtMath::NormalizeAngle0_TwoPi(UtSphericalEarth::ConvertAzElToParallacticAngle(0., 0.001, latDegFrom0p823Rad)),
               1E-6);

   // Test some direct east/west pointings from the equator.
   EXPECT_NEAR(-UtMath::cPI_OVER_2, UtSphericalEarth::ConvertAzElToParallacticAngle(UtMath::cPI_OVER_2, 0, 0), 1E-6);
   EXPECT_NEAR(-UtMath::cPI_OVER_2, UtSphericalEarth::ConvertAzElToParallacticAngle(UtMath::cPI_OVER_2, 1, 0), 1E-6);
   EXPECT_NEAR(-UtMath::cPI_OVER_2,
               UtSphericalEarth::ConvertAzElToParallacticAngle(UtMath::cPI_OVER_2, UtMath::cPI_OVER_2, 0),
               1E-6);
   EXPECT_NEAR(UtMath::cPI_OVER_2,
               UtSphericalEarth::ConvertAzElToParallacticAngle(3 * UtMath::cPI_OVER_2, UtMath::cPI_OVER_2, 0),
               1E-6);
   EXPECT_NEAR(UtMath::cPI_OVER_2, UtSphericalEarth::ConvertAzElToParallacticAngle(3 * UtMath::cPI_OVER_2, 1, 0), 1E-6);
   EXPECT_NEAR(UtMath::cPI_OVER_2, UtSphericalEarth::ConvertAzElToParallacticAngle(3 * UtMath::cPI_OVER_2, 0, 0), 1E-6);

   // The following test data were calculated by astropy.
   // The delta values (typically 2E-3) are fairly large of necessity, this is likely due to the indirect way we of
   // necessity calculate this in astropy, which requires using RA and declination values, and in turn are converted to
   // azimuth/elevation and parallactic angle using an equinox epoch and earth orientation parameters.  We tried to
   // control for these as much as possible to match the calculation we use but some residuals appear to still exist.

   // Double number of tests, and test on both east and west sides, by negating the azimuth and the parallactic angle.
   for (int sign : {-1, 1})
   {
      EXPECT_NEAR(sign * 1.5707963267948966,
                  UtSphericalEarth::ConvertAzElToParallacticAngle(sign * 4.71235, 0.171148, 0),
                  2E-3);
      EXPECT_NEAR(sign * 0.7634423664436059,
                  UtSphericalEarth::ConvertAzElToParallacticAngle(sign * 4.58901, 0.118913, latDegFrom0p8Rad),
                  2E-3);
      EXPECT_NEAR(sign * 2.1776732555414258,
                  UtSphericalEarth::ConvertAzElToParallacticAngle(sign * 4.80964, 0.141054, -latDegFrom0p6Rad),
                  2E-3);
      EXPECT_NEAR(sign * 1.6437117102212973,
                  UtSphericalEarth::ConvertAzElToParallacticAngle(sign * 5.15435, 0.154984, 0),
                  2E-3);
      EXPECT_NEAR(sign * 0.8525554240046139,
                  UtSphericalEarth::ConvertAzElToParallacticAngle(sign * 4.91524, 0.423212, latDegFrom0p8Rad),
                  2E-3);
      EXPECT_NEAR(sign * 2.1830485372206825,
                  UtSphericalEarth::ConvertAzElToParallacticAngle(sign * 5.16649, -0.111439, -latDegFrom0p6Rad),
                  2E-3);
      EXPECT_NEAR(sign * 1.4086155450123092,
                  UtSphericalEarth::ConvertAzElToParallacticAngle(sign * 3.46522, 0.0548982, 0),
                  2E-3);
      EXPECT_NEAR(sign * 1.1068649360595504,
                  UtSphericalEarth::ConvertAzElToParallacticAngle(sign * 3.56803, -0.695624, latDegFrom0p8Rad),
                  4E-3);
      EXPECT_NEAR(sign * 1.6308489341040313,
                  UtSphericalEarth::ConvertAzElToParallacticAngle(sign * 3.54192, 0.618498, -latDegFrom0p6Rad),
                  2E-3);
      EXPECT_NEAR(sign * -1.5707963267948966,
                  UtSphericalEarth::ConvertAzElToParallacticAngle(sign * 1.57074, -1.02636, 0),
                  3E-3);
      EXPECT_NEAR(sign * -0.4660666995517275,
                  UtSphericalEarth::ConvertAzElToParallacticAngle(sign * 0.701007, -0.638452, latDegFrom0p8Rad),
                  2E-3);
      EXPECT_NEAR(sign * -2.4935847847444155,
                  UtSphericalEarth::ConvertAzElToParallacticAngle(sign * 2.32128, -0.78387, -latDegFrom0p6Rad),
                  2E-3);
      EXPECT_NEAR(sign * -0.9614190945511756,
                  UtSphericalEarth::ConvertAzElToParallacticAngle(sign * 0.837759, -0.887135, 0),
                  3E-3);
      EXPECT_NEAR(sign * -0.3805479793659598,
                  UtSphericalEarth::ConvertAzElToParallacticAngle(sign * 0.504255, -0.23922, latDegFrom0p8Rad),
                  2E-3);
      EXPECT_NEAR(sign * -2.03375564129644,
                  UtSphericalEarth::ConvertAzElToParallacticAngle(sign * 1.75802, -1.07271, -latDegFrom0p6Rad),
                  3E-3);
      EXPECT_NEAR(sign * -2.5725631955165404,
                  UtSphericalEarth::ConvertAzElToParallacticAngle(sign * 2.96703, -0.279305, 0),
                  5E-3);
      EXPECT_NEAR(sign * -2.316079586534727,
                  UtSphericalEarth::ConvertAzElToParallacticAngle(sign * 2.79467, -1.05754, latDegFrom0p8Rad),
                  1E-2);
      EXPECT_NEAR(sign * -2.6757664295610635,
                  UtSphericalEarth::ConvertAzElToParallacticAngle(sign * 2.96526, 0.312016, -latDegFrom0p6Rad),
                  4E-3);
   }
}
