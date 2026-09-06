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

#include <cmath>

#include <gtest/gtest.h>

#include "UtCalendar.hpp"
#include "UtEarth.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtOrbitalPropagator.hpp"
#include "WsfPropagatorTargetPoint.hpp"

TEST(WsfPropagatorTargetPoint, ClassicalOrbitTargetPoint)
{
   // Set the initial epoch
   UtCalendar initialEpoch{};
   initialEpoch.SetDate(2020, 4, 22);
   initialEpoch.SetTime(12, 0, 0.0);

   // Some parameters of the orbit
   constexpr double cTOLERANCE      = 1.0e-3; // mm and mm/s accuracy for the tests
   constexpr double cECCENTRICITY   = 0.2;
   constexpr double cORBITAL_PERIOD = 4.0 * 3600.0; // 4 hr orbit time
   auto             earth           = ut::EarthWGS84{};
   double           mu              = earth.GetGravitationalParameter();
   double           semiMajorAxis   = pow(mu * pow(cORBITAL_PERIOD / UtMath::cTWO_PI, 2.0), 1.0 / 3.0);
   double           rPeriapsis      = semiMajorAxis * (1.0 - cECCENTRICITY);
   double           rApoapsis       = semiMajorAxis * (1.0 + cECCENTRICITY);
   double           vPeriapsis      = sqrt(2.0 * mu / rPeriapsis - mu / semiMajorAxis);
   double           vApoapsis       = sqrt(2.0 * mu / rApoapsis - mu / semiMajorAxis);

   // We model an equatorial orbit, with elements so that the initial position is entirely
   // along the x axis.
   UtVec3d initialPos{rPeriapsis, 0.0, 0.0};
   UtVec3d initialVel{0.0, vPeriapsis, 0.0};
   UtVec3d semiOrbitPos{-rApoapsis, 0.0, 0.0};
   UtVec3d semiOrbitVel{0.0, -vApoapsis, 0.0};

   // Create a propagator with the given orbit
   ut::OrbitalState::Vector initialStateVector{initialPos, initialVel};
   auto                     initialStatePtr = ut::make_unique<ut::OrbitalState>(initialEpoch,
                                                            earth,
                                                            ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                                                            ut::OrbitalState::ReferenceFrame::cECI,
                                                            initialStateVector);
   auto                     propPtr         = ut::make_unique<UtOrbitalPropagator>(std::move(initialStatePtr));
   propPtr->Initialize(initialEpoch);

   // Create the target point
   wsf::space::PropagatorTargetPoint target{std::move(propPtr)};

   UtVec3d testVec{}; // general output vector from the various tests

   // Test that it gives out the correct ICs
   target.Update(initialEpoch);
   testVec = target.GetPositionECI();
   EXPECT_NEAR(testVec[0], initialPos[0], cTOLERANCE);
   EXPECT_NEAR(testVec[1], initialPos[1], cTOLERANCE);
   EXPECT_NEAR(testVec[2], initialPos[2], cTOLERANCE);
   testVec = target.GetVelocityECI();
   EXPECT_NEAR(testVec[0], initialVel[0], cTOLERANCE);
   EXPECT_NEAR(testVec[1], initialVel[1], cTOLERANCE);
   EXPECT_NEAR(testVec[2], initialVel[2], cTOLERANCE);
   EXPECT_EQ(target.GetEpoch(), initialEpoch);

   // Test that is gives out the correct location after a half orbit
   UtCalendar halfOrbit = initialEpoch;
   halfOrbit.AdvanceTimeBy(cORBITAL_PERIOD / 2.0);
   target.Update(halfOrbit);
   testVec = target.GetPositionECI();
   EXPECT_NEAR(testVec[0], semiOrbitPos[0], cTOLERANCE);
   EXPECT_NEAR(testVec[1], semiOrbitPos[1], cTOLERANCE);
   EXPECT_NEAR(testVec[2], semiOrbitPos[2], cTOLERANCE);
   testVec = target.GetVelocityECI();
   EXPECT_NEAR(testVec[0], semiOrbitVel[0], cTOLERANCE);
   EXPECT_NEAR(testVec[1], semiOrbitVel[1], cTOLERANCE);
   EXPECT_NEAR(testVec[2], semiOrbitVel[2], cTOLERANCE);
   EXPECT_EQ(target.GetEpoch(), halfOrbit);

   // Test that is gives out the correct location after a full orbit
   UtCalendar fullOrbit = initialEpoch;
   fullOrbit.AdvanceTimeBy(cORBITAL_PERIOD);
   target.Update(fullOrbit);
   testVec = target.GetPositionECI();
   EXPECT_NEAR(testVec[0], initialPos[0], cTOLERANCE);
   EXPECT_NEAR(testVec[1], initialPos[1], cTOLERANCE);
   EXPECT_NEAR(testVec[2], initialPos[2], cTOLERANCE);
   testVec = target.GetVelocityECI();
   EXPECT_NEAR(testVec[0], initialVel[0], cTOLERANCE);
   EXPECT_NEAR(testVec[1], initialVel[1], cTOLERANCE);
   EXPECT_NEAR(testVec[2], initialVel[2], cTOLERANCE);
   EXPECT_EQ(target.GetEpoch(), fullOrbit);
}
