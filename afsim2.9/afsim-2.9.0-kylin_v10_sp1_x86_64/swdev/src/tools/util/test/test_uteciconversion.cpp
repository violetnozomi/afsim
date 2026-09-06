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
#include "UtECI_Conversion.hpp"
#include "UtEarth.hpp"
#include "UtOrbitalPropagator.hpp"
#include "UtOrbitalState.hpp"
#include "UtVec3.hpp"

namespace
{
void TestECI_LocVel(UtECI_Conversion& aConv, const UtVec3d& aRefLocECI, const UtVec3d& aRefVelECI)
{
   static constexpr double tolerance = 1.0e-8;

   UtVec3d locECI = aConv.GetLocationECI();
   UtVec3d velECI = aConv.GetVelocityECI();

   EXPECT_NEAR(aRefLocECI[0], locECI[0], tolerance);
   EXPECT_NEAR(aRefLocECI[1], locECI[1], tolerance);
   EXPECT_NEAR(aRefLocECI[2], locECI[2], tolerance);

   EXPECT_NEAR(aRefVelECI[0], velECI[0], tolerance);
   EXPECT_NEAR(aRefVelECI[1], velECI[1], tolerance);
   EXPECT_NEAR(aRefVelECI[2], velECI[2], tolerance);
}
} // namespace

TEST(UtECI_Conversion, RoundTrip)
{
   // arbitrary date and time
   UtCalendar cal;
   cal.SetDate(2019, 2, 25);
   cal.SetTime(14, 0, 0.0);
   double           r = 800000.0; // 800 km
   UtVec3d          locECI(r, 0.0, 0.0);
   UtVec3d          velECI(0.0, sqrt(UtEarth::cGRAVITATIONAL_PARAMETER_WGS84 / r), 0.0);
   UtECI_Conversion conv(cal, ut::EarthWGS84());
   conv.SetLocationECI(locECI);
   conv.SetVelocityECI(velECI);

   UtVec3d locTOD = conv.GetLocationTOD();
   UtVec3d velTOD = conv.GetVelocityTOD();
   conv.SetLocationTOD(locTOD);
   conv.SetVelocityTOD(velTOD);

   TestECI_LocVel(conv, locECI, velECI);

   // reset coordinates and continue
   conv.SetLocationTOD(locTOD);
   conv.SetVelocityTOD(velTOD);

   UtVec3d locWCS = conv.GetLocationWCS();
   UtVec3d velWCS = conv.GetVelocityWCS();

   conv.SetLocationWCS(locWCS);
   conv.SetVelocityWCS(velWCS);

   TestECI_LocVel(conv, locECI, velECI);

   conv.SetLocationWCS(locWCS);
   conv.SetVelocityWCS(velWCS);

   UtVec3d locTEME = conv.GetLocationTEME();
   UtVec3d velTEME = conv.GetVelocityTEME();

   conv.SetLocationTEME(locTEME);
   conv.SetVelocityTEME(velTEME);

   TestECI_LocVel(conv, locECI, velECI);
}

namespace
{
// =================================================================================================
//! Convert acceleration from TOD to ECEF.
//! This is different from converting a vector because the length is not conserved.
// static
void ConvertAccelerationTODToECEF(double aEarthAngle, const double aVelTOD[3], const double aAccelTOD[3], double aAccelECEF[3])
{
   double sinPhi = sin(aEarthAngle);
   double cosPhi = cos(aEarthAngle);

   aAccelECEF[0] = (cosPhi * (aAccelTOD[0] + 2.0 * UtEarth::cOMEGA * aVelTOD[1])) +
                   (sinPhi * (aAccelTOD[1] - 2.0 * UtEarth::cOMEGA * aVelTOD[0]));

   aAccelECEF[1] = (cosPhi * (aAccelTOD[1] - 2.0 * UtEarth::cOMEGA * aVelTOD[0])) -
                   (sinPhi * (aAccelTOD[0] + 2.0 * UtEarth::cOMEGA * aVelTOD[1]));

   aAccelECEF[2] = aAccelTOD[2];
}
} // namespace

TEST(UtECI_Conversion, Epoch)
{
   // Example 3-14, p. 220 from Vallado, Fundamentals of Astrodynamics and Applications, 4th ed.

   int              deltaAT = 32;                                        // s, on the above date.
   constexpr double xp      = -0.140682 * UtMath::cRAD_PER_DEG / 3600.0; // arcseconds -> rad
   constexpr double yp      = 0.333309 * UtMath::cRAD_PER_DEG / 3600.0;  // arcseconds -> rad
   double           delut1  = -0.439961;                                 // s
   double           delta_t = 32.184 + deltaAT - delut1;

   UtCalendar ut1(delut1, deltaAT);
   ut1.SetDate(2004, 4, 06);
   ut1.SetTime(07, 51, 28.386009);
   UtVec3d r_itrf(-1033.479383, 7901.2952754, 6380.3565958); // km
   r_itrf.Multiply(1000.0);                                  // m

   // Initial rotations may be off a little because we check against a FK76 result.
   double tolerance = 1.0; // m

   UtVec3d v_itrf(-3.225636520, -2.872451450, 5.531924446); // km/s
   v_itrf.Multiply(1000.0);                                 // m/s

   UtECI_Conversion eciConv(ut1, ut::EarthWGS84());
   eciConv.SetPolarOffsetAngles(xp, yp);
   eciConv.SetLocationWCS(r_itrf);
   eciConv.SetVelocityWCS(v_itrf);
   eciConv.SetTime(0.0);

   UtVec3d r_pef = eciConv.GetLocationPEF();
   UtVec3d r_pef_expected(-1033.4750313, 7901.3055856, 6380.3445328); // km
   r_pef_expected.Multiply(1000.0);                                   // m
   EXPECT_NEAR(r_pef[0], r_pef_expected[0], tolerance);
   EXPECT_NEAR(r_pef[1], r_pef_expected[1], tolerance);
   EXPECT_NEAR(r_pef[2], r_pef_expected[2], tolerance);

   UtVec3d r_tod = eciConv.GetLocationTOD();
   UtVec3d r_tod_expected(5094.5147804, 6127.3664612, 6380.3445328); // km
   r_tod_expected.Multiply(1000.0);                                  // m
   EXPECT_NEAR(r_tod[0], r_tod_expected[0], tolerance);
   EXPECT_NEAR(r_tod[1], r_tod_expected[1], tolerance);
   EXPECT_NEAR(r_tod[2], r_tod_expected[2], tolerance);

   UtVec3d r_mod = eciConv.GetLocationMOD();
   UtVec3d r_mod_expected(5094.0283745, 6127.8708164, 6380.248516); // km
   r_mod_expected.Multiply(1000.0);                                 // m
   EXPECT_NEAR(r_mod[0], r_mod_expected[0], tolerance);
   EXPECT_NEAR(r_mod[1], r_mod_expected[1], tolerance);
   EXPECT_NEAR(r_mod[2], r_mod_expected[2], tolerance);

   UtVec3d r_j2000 = eciConv.GetLocationJ2000();
   UtVec3d r_j2000_expected(5102.5096, 6123.01152, 6378.1363); // km
   r_j2000_expected.Multiply(1000.0);                          // m
   EXPECT_NEAR(r_j2000[0], r_j2000_expected[0], tolerance);
   EXPECT_NEAR(r_j2000[1], r_j2000_expected[1], tolerance);
   EXPECT_NEAR(r_j2000[2], r_j2000_expected[2], tolerance);

   // the final result should match expected gcrf almost exactly.
   tolerance = 0.02;

   UtVec3d r_eci = eciConv.GetLocationECI();
   UtVec3d r_eci_expected(5102.508958, 6123.011401, 6378.136928); // km
   r_eci_expected.Multiply(1000.0);                               // m
   EXPECT_NEAR(r_eci[0], r_eci_expected[0], tolerance);
   EXPECT_NEAR(r_eci[1], r_eci_expected[1], tolerance);
   EXPECT_NEAR(r_eci[2], r_eci_expected[2], tolerance);

   eciConv.SetLocationECI(r_eci_expected);
   UtVec3d r_wcs = eciConv.GetLocationWCS();
   EXPECT_NEAR(r_wcs[0], r_itrf[0], tolerance);
   EXPECT_NEAR(r_wcs[1], r_itrf[1], tolerance);
   EXPECT_NEAR(r_wcs[2], r_itrf[2], tolerance);

   UtVec3d v_eci = eciConv.GetVelocityECI();
   UtVec3d v_eci_expected(-4.74322016, 0.79053650, 5.53375528);
   v_eci_expected.Multiply(1000.0); // m
   EXPECT_NEAR(v_eci[0], v_eci_expected[0], tolerance);
   EXPECT_NEAR(v_eci[1], v_eci_expected[1], tolerance);
   EXPECT_NEAR(v_eci[2], v_eci_expected[2], tolerance);

   // test acceleration; ballpark estimate from existing utility.
   UtVec3d a_eci(r_eci);
   a_eci.Normalize();
   a_eci.Multiply(-UtEarth::cGRAVITATIONAL_PARAMETER_WGS84 / r_eci.MagnitudeSquared());
   eciConv.SetAccelerationECI(a_eci);
   UtVec3d a_wcs_expected;

   // The legacy transform only takes sidereal motion and Coriolis acceleration into account.
   ConvertAccelerationTODToECEF(ut1.GetEarthAngle(), v_eci.GetData(), a_eci.GetData(), a_wcs_expected.GetData());
   UtVec3d a_wcs = eciConv.GetAccelerationWCS();

   tolerance = 0.2;

   EXPECT_NEAR(fabs((a_wcs[0] - a_wcs_expected[0]) / a_wcs_expected[0]), 0, tolerance);
   EXPECT_NEAR(fabs((a_wcs[1] - a_wcs_expected[1]) / a_wcs_expected[1]), 0, tolerance);
   EXPECT_NEAR(fabs((a_wcs[2] - a_wcs_expected[2]) / a_wcs_expected[2]), 0, tolerance);

   eciConv.SetAccelerationWCS(a_wcs);
   UtVec3d a_eci_conv = eciConv.GetAccelerationECI();

   // Round Trip test
   tolerance = 1.0e-10;

   EXPECT_NEAR(a_eci[0], a_eci_conv[0], tolerance);
   EXPECT_NEAR(a_eci[1], a_eci_conv[1], tolerance);
   EXPECT_NEAR(a_eci[2], a_eci_conv[2], tolerance);

   // Dv/Dt test
   // Calculate the WCS acceleration as deltaV/deltaT and compare with the directly converted value.
   UtOrbitalPropagator prop(ut::make_unique<ut::OrbitalState>(ut1,
                                                              ut::EarthWGS84{},
                                                              ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                                                              ut::OrbitalState::ReferenceFrame::cECI,
                                                              ut::OrbitalState::Vector(r_eci, v_eci)));
   prop.Initialize(ut1);
   UtVec3d vel1 = prop.GetOrbitalState().GetOrbitalStateVectorWCS().GetVelocity();
   ut1.AdvanceTimeBy(1.0e-5);
   prop.Update(ut1);
   UtVec3d vel2       = prop.GetOrbitalState().GetOrbitalStateVectorWCS().GetVelocity();
   UtVec3d accWCS     = eciConv.GetAccelerationWCS();
   UtVec3d accCompWCS = vel2 - vel1;
   accCompWCS *= 1.0e5;

   tolerance = 5.0e-5;

   EXPECT_NEAR(accWCS[0], accCompWCS[0], tolerance);
   EXPECT_NEAR(accWCS[1], accCompWCS[1], tolerance);
   EXPECT_NEAR(accWCS[2], accCompWCS[2], tolerance);
}

TEST(UtECI_Conversion, AdvanceTimeToEpoch)
{
   // Example 3-14, p. 220 from Vallado, Fundamentals of Astrodynamics and Applications, 4th ed.
   int        deltaAT  = 32;        // s
   double     deltaUT1 = -0.439961; // s
   UtCalendar ut1(deltaUT1, deltaAT);
   // ut1.SetDate(2004, 4, 6);
   ut1.SetDate(2004, 3, 9); // 28 days before the test epoch.
   ut1.SetTime(07, 51, 28.386009);
   UtVec3d r_itrf(-1033.479383, 7901.2952754, 6380.3565958); // km
   r_itrf.Multiply(1000.0);                                  // m

   constexpr double xp = -0.140682 * UtMath::cRAD_PER_DEG / 3600.0; // arcseconds -> rad
   constexpr double yp = 0.333309 * UtMath::cRAD_PER_DEG / 3600.0;  // arcseconds -> rad

   constexpr double tolerance = 1.0; // m

   UtVec3d v_itrf(-3.225636520, -2.872451450, 5.531924446); // km/s
   v_itrf.Multiply(1000.0);                                 // m/s

   UtECI_Conversion eciConv(ut1, ut::EarthWGS84());
   eciConv.SetPolarOffsetAngles(xp, yp);
   eciConv.SetLocationWCS(r_itrf);
   eciConv.SetVelocityWCS(v_itrf);

   // Now advance the time
   double deltaT = 28 * 86400.0; // advance the 28 days to the test epoch.
   eciConv.SetTime(deltaT);

   UtVec3d r_pef = eciConv.GetLocationPEF();
   UtVec3d r_pef_expected(-1033.4750313, 7901.3055856, 6380.3445328); // km
   r_pef_expected.Multiply(1000.0);                                   // m
   EXPECT_NEAR(r_pef[0], r_pef_expected[0], tolerance);
   EXPECT_NEAR(r_pef[1], r_pef_expected[1], tolerance);
   EXPECT_NEAR(r_pef[2], r_pef_expected[2], tolerance);

   UtVec3d r_tod = eciConv.GetLocationTOD();
   UtVec3d r_tod_expected(5094.5147804, 6127.3664612, 6380.3445328); // km
   r_tod_expected.Multiply(1000.0);                                  // m
   EXPECT_NEAR(r_tod[0], r_tod_expected[0], tolerance);
   EXPECT_NEAR(r_tod[1], r_tod_expected[1], tolerance);
   EXPECT_NEAR(r_tod[2], r_tod_expected[2], tolerance);

   UtVec3d r_mod = eciConv.GetLocationMOD();
   UtVec3d r_mod_expected(5094.0283745, 6127.8708164, 6380.248516); // km
   r_mod_expected.Multiply(1000.0);                                 // m
   EXPECT_NEAR(r_mod[0], r_mod_expected[0], tolerance);
   EXPECT_NEAR(r_mod[1], r_mod_expected[1], tolerance);
   EXPECT_NEAR(r_mod[2], r_mod_expected[2], tolerance);

   UtVec3d r_eci = eciConv.GetLocationECI();
   UtVec3d r_eci_expected(5102.508958, 6123.011401, 6378.136928); // km
   r_eci_expected.Multiply(1000.0);                               // m
   EXPECT_NEAR(r_eci[0], r_eci_expected[0], tolerance);
   EXPECT_NEAR(r_eci[1], r_eci_expected[1], tolerance);
   EXPECT_NEAR(r_eci[2], r_eci_expected[2], tolerance);

   UtVec3d v_eci = eciConv.GetVelocityECI();
   UtVec3d v_eci_expected(-4.74322016, 0.79053650, 5.53375528);
   v_eci_expected.Multiply(1000.0); // m
   EXPECT_NEAR(v_eci[0], v_eci_expected[0], tolerance);
   EXPECT_NEAR(v_eci[1], v_eci_expected[1], tolerance);
   EXPECT_NEAR(v_eci[2], v_eci_expected[2], tolerance);
}

TEST(UtECI_Conversion, TOD_WCS_RoundtripWithPolarAngles)
{
   const double inputLocationTOD[3] = {-12948.334185109e3, -28232.189416087e3, 4955.936921923e3};
   const double inputVelocityTOD[3] = {1.108214345e3, -1.988931585e3, -0.889876451e3};
   UtCalendar   epoch(-0.1536230, 37);
   epoch.SetDate(2019, 9, 1);
   epoch.SetTime(1 * 3600.0 + 14 * 60.0 + 24.123);
   // Polar angles and DUT1 provided by Celestrak:
   // https://celestrak.com/SpaceData/EOP-Last5Years.txt
   UtECI_Conversion conversion(epoch, ut::EarthWGS84());
   conversion.SetPolarOffsetAngles(0.214251 * UtMath::cRAD_PER_DEG / 3600.0, 0.351024 * UtMath::cRAD_PER_DEG / 3600.0);
   conversion.SetLocationTOD(inputLocationTOD);
   conversion.SetVelocityTOD(inputVelocityTOD);
   const UtVec3d outputLocationWCS = conversion.GetLocationWCS();
   const UtVec3d outputVelocityWCS = conversion.GetVelocityWCS();
   conversion.SetLocationWCS(outputLocationWCS);
   conversion.SetVelocityWCS(outputVelocityWCS);
   const UtVec3d outputLocationTOD = conversion.GetLocationTOD();
   const UtVec3d outputVelocityTOD = conversion.GetVelocityTOD();

   const double tolerance = 0.001; // m
   EXPECT_NEAR(outputLocationTOD[0], inputLocationTOD[0], tolerance);
   EXPECT_NEAR(outputLocationTOD[1], inputLocationTOD[1], tolerance);
   EXPECT_NEAR(outputLocationTOD[2], inputLocationTOD[2], tolerance);
   EXPECT_NEAR(outputVelocityTOD[0], inputVelocityTOD[0], tolerance);
   EXPECT_NEAR(outputVelocityTOD[1], inputVelocityTOD[1], tolerance);
   EXPECT_NEAR(outputVelocityTOD[2], inputVelocityTOD[2], tolerance);
}

TEST(UtECI_Conversion, AccelerationConversions)
{
   UtCalendar       epoch;
   UtECI_Conversion conv(epoch, ut::EarthEGM96());
   auto             earth = ut::EarthEGM96();
   UtVec3d          loc(earth.GetMeanRadius() / sqrt(2.0), 0.0, earth.GetMeanRadius() / sqrt(2.0));
   conv.SetLocationWCS(loc);
   UtVec3d vel(0.0, 0.0, 100.0);
   conv.SetVelocityWCS(vel);

   UtVec3d acc(-7.0, 0.0, -7.0);
   UtVec3d zeroAcc;

   // WCS->ECI->WCS
   conv.SetAccelerationECI(zeroAcc);
   conv.SetAccelerationWCS(acc);
   UtVec3d accECI = conv.GetAccelerationECI();
   EXPECT_NE(accECI, zeroAcc); // ECI acceleration was updated from WCS

   // zero out the wcs acceleration and test update from inertial
   conv.SetAccelerationWCS(zeroAcc);
   conv.SetAccelerationECI(accECI);
   auto newAcc = conv.GetAccelerationWCS();
   EXPECT_NEAR(acc[0], newAcc[0], 1.0e-8);
   EXPECT_NEAR(acc[1], newAcc[1], 1.0e-8);
   EXPECT_NEAR(acc[2], newAcc[2], 1.0e-8);

   // WCS->TOD->WCS
   conv.SetAccelerationTOD(zeroAcc);
   conv.SetAccelerationWCS(acc);
   auto accTOD = conv.GetAccelerationTOD();
   EXPECT_NE(accTOD, zeroAcc); // TOD acceleration was updated from WCS

   // zero out the wcs acceleration and test update from tod
   conv.SetAccelerationWCS(zeroAcc);
   conv.SetAccelerationTOD(accTOD);
   newAcc = conv.GetAccelerationWCS();
   EXPECT_NEAR(acc[0], newAcc[0], 1.0e-8);
   EXPECT_NEAR(acc[1], newAcc[1], 1.0e-8);
   EXPECT_NEAR(acc[2], newAcc[2], 1.0e-8);

   // TEME->WCS->TEME
   conv.SetAccelerationTEME(zeroAcc);
   conv.SetAccelerationWCS(acc);
   auto accTEME = conv.GetAccelerationTEME();
   EXPECT_NE(accTEME, zeroAcc); // TEME acceleration was updated from WCS

   // zero out the wcs acceleration and test update from TEME
   conv.SetAccelerationWCS(zeroAcc);
   conv.SetAccelerationTEME(accTEME);
   newAcc = conv.GetAccelerationWCS();
   EXPECT_NEAR(acc[0], newAcc[0], 1.0e-8);
   EXPECT_NEAR(acc[1], newAcc[1], 1.0e-8);
   EXPECT_NEAR(acc[2], newAcc[2], 1.0e-8);
}

TEST(UtECI_Conversion, AccelerationNotProvided)
{
   UtCalendar       epoch;
   UtECI_Conversion conv(epoch, ut::EarthEGM96());
   auto             earth = ut::EarthEGM96();
   UtVec3d          loc(earth.GetMeanRadius() / sqrt(2.0), 0.0, earth.GetMeanRadius() / sqrt(2.0));
   conv.SetLocationWCS(loc);
   UtVec3d vel(0.0, 0.0, 100.0);
   conv.SetVelocityWCS(vel);

   // No acceleration set
   EXPECT_DEBUG_DEATH(conv.GetAccelerationECI(), "");
   EXPECT_DEBUG_DEATH(conv.GetAccelerationTOD(), "");
   EXPECT_DEBUG_DEATH(conv.GetAccelerationTEME(), "");
   EXPECT_DEBUG_DEATH(conv.GetAccelerationWCS(), "");
}
