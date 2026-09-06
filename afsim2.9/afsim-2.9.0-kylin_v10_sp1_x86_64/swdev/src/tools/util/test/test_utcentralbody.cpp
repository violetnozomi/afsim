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

#include <gtest/gtest.h>

#include "UtCalendar.hpp"
#include "UtCentralBody.hpp"
#include "UtEarth.hpp"
#include "UtException.hpp"
#include "UtMoon.hpp"
#include "UtOrbitalElements.hpp"
#include "UtSolarSystem.hpp"
#include "UtVec3.hpp"

TEST(CentralBody, moon_pole_test)
{
   // Test that the x-axis in the rotated inertial coordinates points to the earth.
   UtCalendar cal;
   cal.SetDate(2020, 2, 20);
   ut::OrbitalState osMoonX(ut::Moon{},
                            ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                            ut::OrbitalState::ReferenceFrame::cTRUE_OF_DATE);
   osMoonX.Set(cal, ut::OrbitalState::Vector({10000000.0, 0.0, 0.0}, {0.0, 0.0, 0.0}));
   osMoonX.TransformTo(ut::EarthEGM96{});
   UtVec3d moonLocECI;
   UtMoon::GetLocationECI(cal, moonLocECI);
   UtVec3d osMoonLocX = osMoonX.GetOrbitalStateVector().GetLocation();
   double  cosAngle   = UtVec3d::DotProduct(osMoonLocX, moonLocECI) / moonLocECI.Magnitude() / osMoonLocX.Magnitude();
   EXPECT_NEAR(cosAngle, 1.0, 0.01);

   ut::OrbitalState osMoonZ(ut::Moon{},
                            ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                            ut::OrbitalState::ReferenceFrame::cTRUE_OF_DATE);
   osMoonZ.Set(cal, ut::OrbitalState::Vector({0.0, 0.0, 10000000.0}, {0.0, 0.0, 0.0}));
   osMoonX.TransformTo(ut::EarthEGM96{});
   UtVec3d osMoonLocZ = osMoonX.GetOrbitalStateVector().GetLocation();
   double  angle = acos(UtVec3d::DotProduct(osMoonLocZ, moonLocECI) / moonLocECI.Magnitude() / osMoonLocZ.Magnitude());
   double  expectedAngle = atan2(10000000.0, moonLocECI.Magnitude());
   EXPECT_NEAR(angle, expectedAngle, 0.01);
}

// Note: The following test may become obsolete if UtSolarSystem is refactored to include central body objects.
TEST(CentralBody, moon_velocity_test)
{
   UtSolarSystem ss{};
   UtCalendar    epoch;
   epoch.SetDate(2020, 3, 20);
   static constexpr double tolerance = 5.0e-4;

   // Moon
   ut::Moon moon{};
   auto     moonVel = moon.GetVelocityECI(epoch);
   UtVec3d  moonVelComp{};
   UtVec3d  moonLocComp{};
   ss.GetMoonLocationVelocityECI(epoch, moonLocComp, moonVelComp);
   EXPECT_NEAR(moonVel[0], moonVelComp[0], tolerance);
   EXPECT_NEAR(moonVel[1], moonVelComp[1], tolerance);
   EXPECT_NEAR(moonVel[2], moonVelComp[2], tolerance);
}

class TestCentralOffsetPoint : public ut::CentralPoint
{
public:
   TestCentralOffsetPoint* Clone() const override { return new TestCentralOffsetPoint(); }
   const char*             GetName() const override { return "test"; }
   UtVec3d GetLocationECI(const UtCalendar& aEpoch) const override { return UtVec3d(10000.0, 0.0, 0.0); }
   UtMat3d ComputeECI_ToCentralPointInertialTransform(const UtCalendar& aEpoch) const override { return UtMat3d(); }
};

TEST(CentralPoint, exceptions)
{
   TestCentralOffsetPoint tcop;
   EXPECT_THROW(tcop.GetAsCentralBody(), UtException);
   ut::EarthEGM96 cb;
   EXPECT_NO_THROW(cb.GetAsCentralBody());
   UtCalendar       epoch;
   UtECI_Conversion convCentralPoint(epoch, tcop);
   EXPECT_THROW(convCentralPoint.GetCentralBody(), UtException);
   UtECI_Conversion convCentralBody(epoch, cb);
   EXPECT_NO_THROW(convCentralBody.GetCentralBody());
   ut::OrbitalState osCentralPoint(tcop,
                                   ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                                   ut::OrbitalState::ReferenceFrame::cECI);
   EXPECT_THROW(osCentralPoint.GetCentralBody(), UtException);
   ut::OrbitalState osCentralBody(cb, ut::OrbitalState::CoordinateSystem::cEQUATORIAL, ut::OrbitalState::ReferenceFrame::cECI);
   EXPECT_NO_THROW(osCentralBody.GetCentralBody());
}

TEST(CentralBody, line_of_sight_test)
{
   // Check true when in LOS
   UtVec3d loc1(1000, 0, 0);
   UtVec3d loc2(0, 1000, 0);
   UtVec3d bodyLoc(0, 0, 0);
   double  radius = 100;
   EXPECT_TRUE(ut::CentralBody::LineOfSight(loc1, loc2, bodyLoc, radius));

   // Check false when not in LOS
   loc1    = UtVec3d(0, 0, 0);
   loc2    = UtVec3d(1000, 2000, 3000);
   bodyLoc = UtVec3d(100, 200, 300);
   radius  = 100;
   EXPECT_FALSE(ut::CentralBody::LineOfSight(loc1, loc2, bodyLoc, radius));

   // Check Edge Case depending on slight radius change
   loc1               = UtVec3d(500, 500, 0);
   loc2               = UtVec3d(0, -500, 0);
   bodyLoc            = UtVec3d(0, 0, 0);
   double exactRadius = 500 / sqrt(5);
   EXPECT_TRUE(ut::CentralBody::LineOfSight(loc1, loc2, bodyLoc, exactRadius - 1));
   EXPECT_FALSE(ut::CentralBody::LineOfSight(loc1, loc2, bodyLoc, exactRadius + 1));

   // Check Edge Case where location is within body
   loc1    = UtVec3d(1000, 0, 0);
   loc2    = UtVec3d(1500, 0, 0);
   bodyLoc = UtVec3d(950, 0, 0);
   radius  = 100;

   // Expect line of sight when location is within body but also on correct side of body
   EXPECT_TRUE(ut::CentralBody::LineOfSight(loc1, loc2, bodyLoc, radius));

   // Expect no line of sight when location is within body and on wrong side
   bodyLoc = UtVec3d(1050, 0, 0);
   EXPECT_FALSE(ut::CentralBody::LineOfSight(loc1, loc2, bodyLoc, radius));
}
