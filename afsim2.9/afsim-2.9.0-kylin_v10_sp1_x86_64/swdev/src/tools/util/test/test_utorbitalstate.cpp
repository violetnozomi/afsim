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

#include <string>

#include <gtest/gtest.h>

#include "UtCalendar.hpp"
#include "UtCentralBody.hpp"
#include "UtECI_Conversion.hpp"
#include "UtEarth.hpp"
#include "UtEntity.hpp"
#include "UtInput.hpp"
#include "UtMoon.hpp"
#include "UtOrbitalElements.hpp"
#include "UtOrbitalState.hpp"
#include "UtTwoLineElement.hpp"
#include "UtVec2.hpp"
#include "UtVec3.hpp"

//! Define a spherical earth for test purposes.
class TestEllipsoid : public ut::EarthEllipsoidWGS84
{
public:
   static constexpr const char* GetNameId() { return "test"; }
   const char*                  GetName() const override { return GetNameId(); }
   CentralBodyEllipsoid*        Clone() const override { return new TestEllipsoid(*this); }
   double                       GetSemiMajorAxis() const override { return 6378137.0; }
   double                       GetFlatteningReciprocal() const override { return 1.0e12; }
   double                       GetSemiMinorAxis() const override { return 6378137.0; }
   double                       GetFirstEccentricitySquared() const override { return 0.0; }
   double                       GetSecondEccentricitySquared() const override { return 0.0; }
};

class TestCentralBody : public ut::CentralBody
{
public:
   TestCentralBody()
      : CentralBody(TestEllipsoid())
   {
   }
   TestCentralBody(const TestCentralBody&) = default;
   CentralBody*                 Clone() const override { return new TestCentralBody(*this); }
   static constexpr const char* GetNameId() { return TestEllipsoid::GetNameId(); }
   double                       GetNorthPoleRA(const UtCalendar& aEpoch) const override { return mNorthPoleRA; }
   double                       GetNorthPoleDec(const UtCalendar& aEpoch) const override { return mNorthPoleDec; }
   double                       GetSiderealOffsetAngle(const UtCalendar& aEpoch) const override { return mOffsetAngle; }
   UtVec3d                      GetLocationECI(const UtCalendar& aEpoch) const override
   {
      return UtVec3d();
   } //! Earth location ECI is always the origin.
   UtVec3d GetVelocityECI(const UtCalendar& aEpoch) const override
   {
      return UtVec3d();
   } //! Earth velocity ECI is always zero.

   void SetNorthPoleRA(double aNPRA) { mNorthPoleRA = aNPRA; }
   void SetNorthPoleDec(double aNPDec) { mNorthPoleDec = aNPDec; }
   void SetOffsetAngle(double aOOA) { mOffsetAngle = aOOA; }

   double mNorthPoleRA{0.0};
   double mNorthPoleDec{UtMath::cPI_OVER_2};
   double mOffsetAngle{0.0};
};

ut::OrbitalState TestTransformToEarth(double aPoleAngleRA, double aPoleAngleDec, double aInclination, double aRAAN = 0.0)
{
   TestCentralBody tcb;
   tcb.mNorthPoleRA  = aPoleAngleRA;
   tcb.mNorthPoleDec = aPoleAngleDec;

   UtOrbitalElements tcbElements(tcb,
                                 8000000.0,    // semi-major axis
                                 0.0,          // eccentricity
                                 0.0,          // mean anomaly
                                 aInclination, // inclination
                                 aRAAN,        // right ascension of the ascending node
                                 0.0);         // argument of periapsis

   ut::OrbitalState tcbState(ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                             ut::OrbitalState::ReferenceFrame::cECI,
                             tcbElements);
   tcbState.TransformTo(ut::EarthWGS84());
   return tcbState;
}

ut::OrbitalState TestTransformFromEarth(double aPoleAngleRA, double aPoleAngleDec, double aInclination, double aRAAN = 0.0)
{
   TestCentralBody tcb;
   tcb.mNorthPoleRA  = aPoleAngleRA;
   tcb.mNorthPoleDec = aPoleAngleDec;

   ut::EarthWGS84    earth;
   UtOrbitalElements elements(earth,
                              8000000.0,    // semi-major axis
                              0.0,          // eccentricity
                              0.0,          // mean anomaly
                              aInclination, // inclination
                              aRAAN,        // right ascension of the ascending node
                              0.0);         // argument of periapsis

   ut::OrbitalState tcbState(ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                             ut::OrbitalState::ReferenceFrame::cECI,
                             elements);
   tcbState.TransformTo(tcb);
   return tcbState;
}

TEST(UtOrbitalState, AnomalyTest)
{
   UtOrbitalElements testObj{ut::EarthEGM96{}};

   for (double index = 0; index < 8.0 * UtMath::cPI_OVER_4; index += UtMath::cPI_OVER_4)
   {
      testObj.SetTrueAnomaly(index + UtMath::cTWO_PI);
      EXPECT_NEAR(testObj.GetTrueAnomaly(), index, 1.0e-12);

      testObj.SetTrueAnomaly(index - UtMath::cTWO_PI);
      EXPECT_NEAR(testObj.GetTrueAnomaly(), index, 1.0e-12);

      testObj.SetMeanAnomaly(index + UtMath::cTWO_PI);
      EXPECT_NEAR(testObj.GetMeanAnomaly(), index, 1.0e-12);

      testObj.SetMeanAnomaly(index - UtMath::cTWO_PI);
      EXPECT_NEAR(testObj.GetMeanAnomaly(), index, 1.0e-12);
   }
}

TEST(UtOrbitalState, TransformToAngles)
{
   UtCalendar epoch;
   epoch.SetEpoch(2020000.0);

   ut::EarthWGS84    earth;
   UtOrbitalElements elements(epoch,
                              earth,
                              8000000.0, // semi-major axis
                              0.0,       // eccentricity
                              0.0,       // mean anomaly
                              0.0,       // inclination
                              0.0,       // right ascension of the ascending node
                              0.0);      // argument of periapsis

   ut::OrbitalState state(ut::OrbitalState::CoordinateSystem::cEQUATORIAL, ut::OrbitalState::ReferenceFrame::cECI, elements);
   TestCentralBody  tcb;
   ut::OrbitalState tcbState = state;
   tcbState.TransformTo(tcb);

   EXPECT_NEAR(tcbState.GetOrbitalElements().GetInclination(), 0.0, 1.0e-6);
   EXPECT_NEAR(tcbState.GetOrbitalElements().GetSemiMajorAxis(), 8000000.0, 1.0e-3);

   // 90 degree declination of other body.
   ut::OrbitalState state2 = TestTransformToEarth(0.0, 0.0, 0.0);
   EXPECT_NEAR(state2.GetOrbitalElements().GetInclination(), UtMath::cPI_OVER_2, 1.0e-6);
   double raan2 = state2.GetOrbitalElements().GetRAAN();

   // 90 degree RA, 90 degree dec of other body.
   ut::OrbitalState state3 = TestTransformToEarth(UtMath::cPI, 0.0, 0.0);
   EXPECT_NEAR(state3.GetOrbitalElements().GetInclination(), UtMath::cPI_OVER_2, 1.0e-6);
   double raan3 = state3.GetOrbitalElements().GetRAAN();

   EXPECT_NEAR(fabs(raan3 - raan2), UtMath::cPI, 1.0e-6);

   // 90 degree dec. of other body.
   ut::OrbitalState state4 = TestTransformToEarth(0.0, 0.0, UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
   EXPECT_NEAR(state4.GetOrbitalElements().GetInclination(), UtMath::cPI_OVER_2, 1.0e-6);
   EXPECT_NEAR(state4.GetOrbitalElements().GetRAAN(), UtMath::cPI, 1.0e-6);

   // No transform case.
   ut::OrbitalState state5 = TestTransformFromEarth(0.0, UtMath::cPI_OVER_2, 0.0, 0.0);
   EXPECT_NEAR(state5.GetOrbitalElements().GetInclination(), 0.0, 1.0e-6);

   ut::OrbitalState state6 = TestTransformFromEarth(0.0, 0.0, 0.0, 0.0);
   EXPECT_NEAR(state6.GetOrbitalElements().GetInclination(), UtMath::cPI_OVER_2, 1.0e-6);
}

TEST(UtOrbitalState, EclipticTransform)
{
   using ut::OrbitalState;

   // Test case from Vallado, Fundamentals of Astrodynamics and Applications, 4th ed., pp.273-275
   UtCalendar epoch;
   epoch.SetDate(1994, 5, 14);
   epoch.SetTime(13, 11, 20.59856);
   UtVec3d      locECI{1752246215000, -3759563433000, -1577568105000};
   UtVec3d      velECI{-18323, 18332, 7777};
   OrbitalState equatorialState(epoch,
                                ut::EarthWGS84{},
                                OrbitalState::CoordinateSystem::cEQUATORIAL,
                                OrbitalState::ReferenceFrame::cECI,
                                OrbitalState::Vector(locECI, velECI));
   OrbitalState eclipticState(ut::EarthWGS84{}, OrbitalState::CoordinateSystem::cECLIPTIC, OrbitalState::ReferenceFrame::cECI);
   eclipticState.Set(equatorialState);
   OrbitalState::Vector eclipticOSV = eclipticState.GetOrbitalStateVector();
   double               r           = eclipticOSV.GetLocation().Magnitude();
   EXPECT_NEAR(r, 4437725220273, 1);
   double l = 360 + atan2(eclipticOSV.GetLocation()[1], eclipticOSV.GetLocation()[0]) * UtMath::cDEG_PER_RAD;
   double b = asin(eclipticOSV.GetLocation()[2] / r) * UtMath::cDEG_PER_RAD;
   EXPECT_NEAR(l, 293.258210, 0.00001);
   EXPECT_NEAR(b, 0.62075, 0.00002);

   // round trip
   equatorialState.Set(eclipticState);
   UtVec3d compLocECI = equatorialState.GetOrbitalStateVector().GetLocation();
   UtVec3d compVelECI = equatorialState.GetOrbitalStateVector().GetVelocity();

   EXPECT_NEAR(compLocECI[0], locECI[0], 1);
   EXPECT_NEAR(compLocECI[1], locECI[1], 1);
   EXPECT_NEAR(compLocECI[2], locECI[2], 1);

   EXPECT_NEAR(compVelECI[0], velECI[0], 1);
   EXPECT_NEAR(compVelECI[1], velECI[1], 1);
   EXPECT_NEAR(compVelECI[2], velECI[2], 1);

   // Now let's try the same thing with a TOD referenced state
   OrbitalState osTOD(ut::EarthWGS84{},
                      OrbitalState::CoordinateSystem::cEQUATORIAL,
                      OrbitalState::ReferenceFrame::cTRUE_OF_DATE);
   osTOD.Set(equatorialState);
   OrbitalState::Vector osvi = osTOD.GetOrbitalStateVectorInertial();
   compLocECI                = osvi.GetLocation();
   compVelECI                = osvi.GetVelocity();
   EXPECT_NEAR(compLocECI[0], locECI[0], 1);
   EXPECT_NEAR(compLocECI[1], locECI[1], 1);
   EXPECT_NEAR(compLocECI[2], locECI[2], 1);

   EXPECT_NEAR(compVelECI[0], velECI[0], 1);
   EXPECT_NEAR(compVelECI[1], velECI[1], 1);
   EXPECT_NEAR(compVelECI[2], velECI[2], 1);

   eclipticState.Set(osTOD);
   eclipticOSV = eclipticState.GetOrbitalStateVector();
   r           = eclipticOSV.GetLocation().Magnitude();
   EXPECT_NEAR(r, 4437725220273, 1);
   l = 360 + atan2(eclipticOSV.GetLocation()[1], eclipticOSV.GetLocation()[0]) * UtMath::cDEG_PER_RAD;
   b = asin(eclipticOSV.GetLocation()[2] / r) * UtMath::cDEG_PER_RAD;
   EXPECT_NEAR(l, 293.258210, 0.00001);
   EXPECT_NEAR(b, 0.62075, 0.00002);
}

TEST(UtOrbitalState, Invalid)
{
   ut::OrbitalState state;
   EXPECT_FALSE(state.IsValid());
   EXPECT_ANY_THROW(state.GetOrbitalElementsTOD());
   EXPECT_ANY_THROW(state.GetOrbitalStateVectorInertial());
   EXPECT_ANY_THROW(state.GetOrbitalStateVectorTEME());
   EXPECT_ANY_THROW(state.GetOrbitalStateVectorTOD());
   EXPECT_ANY_THROW(state.GetOrbitalStateVector(ut::OrbitalState::ReferenceFrame::cTRUE_OF_DATE));
}

TEST(UtOrbitalState, TLE_Input)
{
   // Read a TLE from input and construct an orbital state
   UtTwoLineElement tle;
   std::string      stringTLE = "STARFIRE-198\n"
                                "1 45364U 20019E   20099.55369514 -.00381893  00000-0 -24582-2 0  9999\n"
                                "2 45364  53.0003 150.1807 0006224 265.2999  94.7309 15.76471253  3986\n";
   UtInput          input;
   input.PushInputString(stringTLE);
   tle.ReadOrbitalData(input);
   ut::OrbitalState state(ut::OrbitalState::CoordinateSystem::cEQUATORIAL, ut::OrbitalState::ReferenceFrame::cECI, tle);
   const UtTwoLineElement& tleComp = state.GetTwoLineElement();
   EXPECT_NEAR(tleComp.GetBstarDrag(), -0.0024582000000000002, 1.0e-6);
   EXPECT_NEAR(tleComp.GetMeanMotionFirstDerivative(), -1.1571684444997751e-08, 1.0e-8);
   EXPECT_EQ(tleComp.GetSatelliteNumber(), 45364);
   EXPECT_EQ(tleComp.GetRevolutionNumber(), 398);
   EXPECT_EQ(tleComp.GetElementNumber(), 999);

   // Now do the same thing with the set method.
   ut::OrbitalState state2(ut::EarthEGM96{},
                           ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                           ut::OrbitalState::ReferenceFrame::cECI);
   state2.Set(tle);
   const UtTwoLineElement& tleComp2 = state2.GetTwoLineElement();
   EXPECT_NEAR(tleComp2.GetBstarDrag(), -0.0024582000000000002, 1.0e-6);
   EXPECT_NEAR(tleComp2.GetMeanMotionFirstDerivative(), -1.1571684444997751e-08, 1.0e-8);
   EXPECT_EQ(tleComp2.GetSatelliteNumber(), 45364);
   EXPECT_EQ(tleComp2.GetRevolutionNumber(), 398);
   EXPECT_EQ(tleComp2.GetElementNumber(), 999);
}

TEST(UtOrbitalState, ChangeCentralBody)
{
   // Earth-Moon Round Trip.
   UtVec3d          earthLoc{5000000.0, 6000000.0, 1000000.0};
   UtVec3d          earthVel{4000.0, 3000.0, 500.0};
   auto             epoch = UtCalendar();
   ut::OrbitalState earthState(epoch,
                               ut::EarthEGM96{},
                               ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                               ut::OrbitalState::ReferenceFrame::cECI,
                               ut::OrbitalState::Vector(earthLoc, earthVel));
   earthState.TransformTo(ut::Moon());
   earthState.TransformTo(ut::EarthEGM96());
   auto osv = earthState.GetOrbitalStateVector();
   EXPECT_NEAR(osv.GetLocation()[0], earthLoc[0], 1.0e-3);
   EXPECT_NEAR(osv.GetLocation()[1], earthLoc[1], 1.0e-3);
   EXPECT_NEAR(osv.GetLocation()[2], earthLoc[2], 1.0e-3);
   EXPECT_NEAR(osv.GetVelocity()[0], earthVel[0], 1.0e-3);
   EXPECT_NEAR(osv.GetVelocity()[1], earthVel[1], 1.0e-3);
   EXPECT_NEAR(osv.GetVelocity()[2], earthVel[2], 1.0e-3);

   //! Location, velocity magnitude test
   ut::OrbitalState moonCenter(epoch,
                               ut::EarthEGM96{},
                               ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                               ut::OrbitalState::ReferenceFrame::cECI,
                               ut::OrbitalState::Vector(UtVec3d{}, UtVec3d{}));
   moonCenter.TransformTo(ut::Moon());
   ut::Moon moon{};
   auto     moonLoc = moon.GetLocationECI(epoch);
   auto     moonVel = moon.GetVelocityECI(epoch);
   osv              = moonCenter.GetOrbitalStateVectorInertial();
   EXPECT_NEAR(osv.GetLocation().Magnitude(), moonLoc.Magnitude(), 1.0e-3);
   EXPECT_NEAR(osv.GetVelocity().Magnitude(), moonVel.Magnitude(), 1.0e-3);
}

TEST(UtOrbitalState, ChangeReferenceFrame)
{
   UtCalendar       epoch;
   ut::OrbitalState oeState(ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                            ut::OrbitalState::ReferenceFrame::cTRUE_OF_DATE,
                            UtOrbitalElements(epoch, ut::EarthEGM96(), 9e+6, 0.0, 0.0, 0.0, 0.523, 0.0));
   EXPECT_TRUE(oeState.GetOrbitalElements().OrbitIsEquatorial());
   oeState.SetReferenceFrame(ut::OrbitalState::ReferenceFrame::cECI);
   EXPECT_FALSE(oeState.GetOrbitalElements().OrbitIsEquatorial());
   oeState.SetReferenceFrame(ut::OrbitalState::ReferenceFrame::cTRUE_OF_DATE);
   EXPECT_TRUE(oeState.GetOrbitalElements().OrbitIsEquatorial());
}

TEST(UtOrbitalState, ChangeCentralBodyLLA)
{
   // North pole of test body is rotated 10 degrees, leading to 10 degree rotation of an ECI y vector (10 deg. lat).
   TestCentralBody tcb{};
   tcb.SetNorthPoleRA(-90.0 * UtMath::cRAD_PER_DEG);
   tcb.SetNorthPoleDec(80.0 * UtMath::cRAD_PER_DEG);
   tcb.SetOffsetAngle(0.0 * UtMath::cRAD_PER_DEG);
   UtCalendar epoch;
   UtEntity   testEntity1(tcb);
   testEntity1.SetECI_ReferenceEpoch(epoch);
   double locTOD[3]{0.0, 10000000.0, 0.0};
   testEntity1.SetLocationTOD(locTOD);
   double lat, lon, alt;
   testEntity1.GetLocationLLA(lat, lon, alt);
   EXPECT_NEAR(lat, 0.0, 0.0001);
   TestCentralBody  ecb{};
   ut::OrbitalState state1(epoch,
                           tcb,
                           ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                           ut::OrbitalState::ReferenceFrame::cTRUE_OF_DATE,
                           ut::OrbitalState::Vector(locTOD, UtVec3d{}));
   state1.TransformTo(ecb);
   testEntity1.SetLocationTOD(state1.GetOrbitalStateVectorTOD().GetLocation().GetData());
   testEntity1.SetVelocityTOD(state1.GetOrbitalStateVectorTOD().GetVelocity().GetData());
   testEntity1.GetLocationLLA(lat, lon, alt);
   EXPECT_NEAR(lat, 10.0, 0.0001);
}

bool TestMalformedTLE(const std::string& aTLE)
{
   UtInput input;
   input.PushInputString(aTLE);
   bool             ok = true;
   ut::OrbitalState initialState(ut::EarthWGS84(),
                                 ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                                 ut::OrbitalState::ReferenceFrame::cTEME);
   while (true)
   {
      try
      {
         std::string command;
         input.ReadCommand(command);
         ok = initialState.ProcessInput(input);
      }
      catch (UtInput::BadValue&)
      {
         ok = false;
         break;
      }
      catch (std::exception&)
      {
         break;
      }
   };
   return ok;
}

TEST(UtOrbitalState, UtInputWithMalformedTLE)
{
   std::string tle = "orbit\n";
   tle += "1 38953U 12059A   19115.41660 -.00000189 +00000-0 +00000-0 0 09990\n"; // bad format on first line
   tle += "2 38953 001.7600 083.1917 0004773 235.1941 128.7092 01.00274739023882\n";
   tle += "end_orbit\n";

   bool ok = TestMalformedTLE(tle);
   EXPECT_FALSE(ok);

   tle = "orbit\n";
   tle += "1 32384U 07062A   14243.07000179  .00000003  00000-0  00000+0 0  3630\n"; // correct formatting
   tle += "2 32384  55.6007  84.1910 0013624 307.7892 230.5742  2.00567038 49164\n";
   tle += "end_orbit\n";

   ok = TestMalformedTLE(tle);
   EXPECT_TRUE(ok);

   tle = "orbit\n";
   tle += "This is the descriptor\n";
   tle +=
      "1 32384U 07062A   14243.07000179  .00000003  00000-0  00000+0 0  3630\n"; // correct formatting w/descriptor line
   tle += "2 32384  55.6007  84.1910 0013624 307.7892 230.5742  2.00567038 49164\n";
   tle += "end_orbit\n";

   ok = TestMalformedTLE(tle);
   EXPECT_TRUE(ok);

   tle = "orbit\n";
   tle += "1 24905U 97043C   14243.37051886  .00000260  00000-0  85887-4 0  9757\n";
   tle += "2 24905  86.3999 322.6117 0002416 110.6549 249.4905 14.342185318915167\n"; // second line too long
   tle += "end_orbit\n";

   // 1 24905U 97043C   14243.37051886  .00000260  00000-0  85887-4 0  9757
   // 2 24905  86.3999 322.6117 0002416 110.6549 249.4905 14.34218531891516

   ok = TestMalformedTLE(tle);
   EXPECT_FALSE(ok);

   tle = "orbit\n";
   tle += "1 24905U 97043C  14243.37051886 .00000260  00000-0  85887-4 0  9757\n"; // first line too short
   tle += "2 24905  86.3999 322.6117 0002416 110.6549 249.4905 14.34218531891516\n";
   tle += "end_orbit\n";

   ok = TestMalformedTLE(tle);
   EXPECT_FALSE(ok);

   tle = "orbit\n";
   tle += "24905U 97043C  14243.37051886  .00000260  00000-0  85887-4 0  9757\n"; // missing initial number
   tle += "2 24905  86.3999 322.6117 0002416 110.6549 249.4905 14.34218531891516\n";
   tle += "end_orbit\n";

   ok = TestMalformedTLE(tle);
   EXPECT_FALSE(ok);

   tle = "orbit\n";
   tle += "1 24905U 97043C  14243.37051886  .00000260  00000-0  85887-4 0  9757\n"; // missing line
   tle += "end_orbit\n";

   ok = TestMalformedTLE(tle);
   EXPECT_FALSE(ok);

   // Special case of nonstandard TLE as found in Spacetrack Report #3 deep space test case:
   tle = "orbit\n";
   tle += "1 11801U          80230.29629788  .01431103  00000-0  14311-1\n";   // missing line
   tle += "2 11801  46.7916 230.4354 7318036  47.4722  10.4117  2.28537848\n"; // missing line
   tle += "end_orbit\n";

   ok = TestMalformedTLE(tle);
   EXPECT_TRUE(ok);
}

TEST(UtOrbitalState, RoundTrip)
{
   ut::OrbitalState  state(ut::EarthWGS84(),
                          ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                          ut::OrbitalState::ReferenceFrame::cTRUE_OF_DATE);
   UtOrbitalElements initEle(ut::EarthWGS84(),
                             800000.0, // sma
                             0.1,      // ecc
                             0.2,      // ma
                             0.3,      // inc
                             0.4,      // raan
                             0.5);     // argp
   state.Set(initEle);
   ut::OrbitalState testState = state;

   // Simple round trip
   ut::OrbitalState::Vector vecTOD = state.GetOrbitalStateVectorTOD();
   auto&                    locTOD = vecTOD.GetLocation();
   auto&                    velTOD = vecTOD.GetVelocity();
   UtCalendar               epoch  = testState.GetEpoch();
   testState.Set(epoch, ut::OrbitalState::Vector(locTOD, velTOD));
   static constexpr double linearTolerance  = 0.0001;
   static constexpr double angularTolerance = 1.0e-6;
   UtOrbitalElements       testEle          = testState.GetOrbitalElements();
   UtOrbitalElements       ele              = state.GetOrbitalElements();
   EXPECT_NEAR(testEle.GetSemiMajorAxis(), ele.GetSemiMajorAxis(), linearTolerance);
   EXPECT_NEAR(testEle.GetInclination(), ele.GetInclination(), angularTolerance);
   EXPECT_NEAR(testEle.GetEccentricity(), ele.GetEccentricity(), 0.0001);
   EXPECT_NEAR(testEle.GetMeanAnomaly(), ele.GetMeanAnomaly(), angularTolerance);
   EXPECT_NEAR(testEle.GetArgumentOfPeriapsis(), ele.GetArgumentOfPeriapsis(), angularTolerance);
   EXPECT_NEAR(testEle.GetRAAN(), ele.GetRAAN(), angularTolerance);
}

class TestCentralOriginPoint : public ut::CentralPoint
{
public:
   TestCentralOriginPoint* Clone() const override { return new TestCentralOriginPoint(); }
   const char*             GetName() const override { return "test"; }
   UtVec3d                 GetLocationECI(const UtCalendar& aEpoch) const override { return UtVec3d(); }
   UtMat3d ComputeECI_ToCentralPointInertialTransform(const UtCalendar& aEpoch) const override { return UtMat3d(); }
};

TEST(UtOrbitalState, CentralPoint)
{
   UtVec3d          loc{5000000.0, 6000000.0, 1000000.0};
   UtVec3d          vel{4000.0, 3000.0, 500.0};
   UtCalendar       epoch;
   ut::OrbitalState state(epoch,
                          TestCentralOriginPoint(),
                          ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                          ut::OrbitalState::ReferenceFrame::cECI,
                          ut::OrbitalState::Vector(loc, vel));

   // orbital elements should be not-computable due to use of TestCentralOriginPoint as non-gravitating.
   EXPECT_THROW(state.GetOrbitalElements(), UtException);
   EXPECT_THROW(state.GetOrbitalElementsTOD(), UtException);
}

TEST(UtOrbitalState, Initialize)
{
   ut::OrbitalState state;
   UtCalendar       epoch;
   EXPECT_FALSE(state.Initialize(epoch)); // Not enough information.
   UtVec3d position(8000000.0, 0.0, 0.0);
   UtVec3d velocity(4000000, 0.0, 0.0);
   state.SetOrbitalStateVector(ut::OrbitalState::Vector(position, velocity));
   EXPECT_FALSE(state.Initialize(epoch)); // Ref frame and coordinate system not set.
   state.SetCoordinateSystem(ut::OrbitalState::CoordinateSystem::cEQUATORIAL);
   state.SetReferenceFrame(ut::OrbitalState::ReferenceFrame::cECI);
   state.SetEpoch(epoch);
   EXPECT_TRUE(state.Initialize(epoch));
   EXPECT_EQ(state.GetEpoch().GetJ2000_Date(), epoch.GetJ2000_Date());

   ut::OrbitalState state2(ut::EarthEGM96(),
                           ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                           ut::OrbitalState::ReferenceFrame::cECI);
   EXPECT_FALSE(state2.Initialize(epoch));
   state2.SetOrbitalStateVector(ut::OrbitalState::Vector(position, velocity));
   EXPECT_TRUE(state2.Initialize(epoch));
   state2.SetEpoch(epoch);
   EXPECT_TRUE(state2.Initialize(epoch));
}
