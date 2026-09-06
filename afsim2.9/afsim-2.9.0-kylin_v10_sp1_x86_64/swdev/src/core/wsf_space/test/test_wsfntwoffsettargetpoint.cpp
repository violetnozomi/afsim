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
#include "WsfNTW_OffsetTargetPoint.hpp"
#include "WsfPropagatorTargetPoint.hpp"

TEST(WsfNTW_OffsetTargetPoint, ClassicalOrbitTest)
{
   // Set the initial epoch
   UtCalendar initialEpoch{};
   initialEpoch.SetDate(2020, 4, 22);
   initialEpoch.SetTime(12, 0, 0.0);

   // Some parameters of the orbit
   constexpr double cTOLERANCE = 1.0e-6; // mm and mm/s accuracy for the tests
   constexpr double cRADIUS    = 1.0e6;
   constexpr double cVELOCITY  = 1.0e3;
   auto             earth      = ut::EarthWGS84();

   // These ICs will give an NTW frame with a basis in ECI of
   // (1 / sqrt(2),  1 / sqrt(2),  0)
   // (0,            0,           -1)
   // (-1 / sqrt(2), 1 / sqrt(2),  0)
   UtVec3d initialPos{cRADIUS, 0.0, 0.0};
   UtVec3d initialVel{cVELOCITY, cVELOCITY, 0.0};
   UtVec3d frameX{1.0 / sqrt(2.0), 1.0 / sqrt(2.0), 0.0};
   UtVec3d frameY{0.0, 0.0, -1.0};
   UtVec3d frameZ{-1.0 / sqrt(2.0), 1.0 / sqrt(2.0), 0.0};

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
   auto propTargetPtr = ut::make_unique<wsf::space::PropagatorTargetPoint>(std::move(propPtr));
   wsf::space::NTW_OffsetTargetPoint offsetTarget{std::move(propTargetPtr)};
   offsetTarget.Update(initialEpoch);

   // Vector for storing test inputs
   UtVec3d testVec{};

   // Test zero offset
   EXPECT_EQ(offsetTarget.GetEpoch(), initialEpoch);
   testVec = offsetTarget.GetPositionECI();
   EXPECT_NEAR(testVec[0], initialPos[0], cTOLERANCE);
   EXPECT_NEAR(testVec[1], initialPos[1], cTOLERANCE);
   EXPECT_NEAR(testVec[2], initialPos[2], cTOLERANCE);
   testVec = offsetTarget.GetVelocityECI();
   EXPECT_NEAR(testVec[0], initialVel[0], cTOLERANCE);
   EXPECT_NEAR(testVec[1], initialVel[1], cTOLERANCE);
   EXPECT_NEAR(testVec[2], initialVel[2], cTOLERANCE);

   // Try 'X' position offset
   UtVec3d offset{100.0, 0.0, 0.0};
   offsetTarget.SetPositionOffset(offset);
   UtVec3d expectedPosition = initialPos + offset[0] * frameX + offset[1] * frameY + offset[2] * frameZ;
   testVec                  = offsetTarget.GetPositionECI();
   EXPECT_NEAR(testVec[0], expectedPosition[0], cTOLERANCE);
   EXPECT_NEAR(testVec[1], expectedPosition[1], cTOLERANCE);
   EXPECT_NEAR(testVec[2], expectedPosition[2], cTOLERANCE);

   // Try 'Y' position offset
   offset = UtVec3d{0.0, 100.0, 0.0};
   offsetTarget.SetPositionOffset(offset);
   expectedPosition = initialPos + offset[0] * frameX + offset[1] * frameY + offset[2] * frameZ;
   testVec          = offsetTarget.GetPositionECI();
   EXPECT_NEAR(testVec[0], expectedPosition[0], cTOLERANCE);
   EXPECT_NEAR(testVec[1], expectedPosition[1], cTOLERANCE);
   EXPECT_NEAR(testVec[2], expectedPosition[2], cTOLERANCE);

   // Try 'Z' position offset
   offset = UtVec3d{0.0, 0.0, 100.0};
   offsetTarget.SetPositionOffset(offset);
   expectedPosition = initialPos + offset[0] * frameX + offset[1] * frameY + offset[2] * frameZ;
   testVec          = offsetTarget.GetPositionECI();
   EXPECT_NEAR(testVec[0], expectedPosition[0], cTOLERANCE);
   EXPECT_NEAR(testVec[1], expectedPosition[1], cTOLERANCE);
   EXPECT_NEAR(testVec[2], expectedPosition[2], cTOLERANCE);

   // Try 'X' velocity offset
   offset = UtVec3d{100.0, 0.0, 0.0};
   offsetTarget.SetVelocityOffset(offset);
   UtVec3d expectedVelocity = initialVel + offset[0] * frameX + offset[1] * frameY + offset[2] * frameZ;
   testVec                  = offsetTarget.GetVelocityECI();
   EXPECT_NEAR(testVec[0], expectedVelocity[0], cTOLERANCE);
   EXPECT_NEAR(testVec[1], expectedVelocity[1], cTOLERANCE);
   EXPECT_NEAR(testVec[2], expectedVelocity[2], cTOLERANCE);

   // Try 'Y' velocity offset
   offset = UtVec3d{0.0, 100.0, 0.0};
   offsetTarget.SetVelocityOffset(offset);
   expectedVelocity = initialVel + offset[0] * frameX + offset[1] * frameY + offset[2] * frameZ;
   testVec          = offsetTarget.GetVelocityECI();
   EXPECT_NEAR(testVec[0], expectedVelocity[0], cTOLERANCE);
   EXPECT_NEAR(testVec[1], expectedVelocity[1], cTOLERANCE);
   EXPECT_NEAR(testVec[2], expectedVelocity[2], cTOLERANCE);

   // Try 'Z' velocity offset
   offset = UtVec3d{0.0, 0.0, 100.0};
   offsetTarget.SetVelocityOffset(offset);
   expectedVelocity = initialVel + offset[0] * frameX + offset[1] * frameY + offset[2] * frameZ;
   testVec          = offsetTarget.GetVelocityECI();
   EXPECT_NEAR(testVec[0], expectedVelocity[0], cTOLERANCE);
   EXPECT_NEAR(testVec[1], expectedVelocity[1], cTOLERANCE);
   EXPECT_NEAR(testVec[2], expectedVelocity[2], cTOLERANCE);
}

TEST(WsfNTW_OffsetTargetPoint, ThrowWhenConstructFromNull)
{
   EXPECT_THROW({ wsf::space::NTW_OffsetTargetPoint offsetTarget{nullptr}; }, std::runtime_error);
}
