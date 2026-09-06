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

#include <cmath>

#include <gtest/gtest.h>

#include "UtCalendar.hpp"
#include "UtEarth.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtOrbitalPropagator.hpp"
#include "WsfNTW_OffsetTargetPoint.hpp"
#include "WsfPropagatorTargetPoint.hpp"
#include "WsfRIC_OffsetTargetPoint.hpp"

TEST(WsfKinematicOffsetTargetPoint, ClassicalOrbitTest)
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
   // These ICs will give a RIC frame with a basis in ECI of
   // (1, 0, 0)      -- radial      (R)
   // (0, 1, 0)      -- in-track    (I)
   // (0, 0, 1)      -- cross-track (C)
   UtVec3d initialPos{cRADIUS, 0.0, 0.0};
   UtVec3d initialVel{cVELOCITY, cVELOCITY, 0.0};
   UtVec3d ntwFrameX{1.0 / sqrt(2.0), 1.0 / sqrt(2.0), 0.0};
   UtVec3d ntwFrameY{0.0, 0.0, -1.0};
   UtVec3d ntwFrameZ{-1.0 / sqrt(2.0), 1.0 / sqrt(2.0), 0.0};
   UtVec3d ricFrameX{1.0, 0.0, 0.0};
   UtVec3d ricFrameY{0.0, 1.0, 0.0};
   UtVec3d ricFrameZ{0.0, 0.0, 1.0};

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

   // Create the NTW position offset
   auto    ntwOffsetPtr = ut::make_unique<wsf::space::NTW_OffsetTargetPoint>(std::move(propTargetPtr));
   UtVec3d positionOffset{100.0, 0.0, 0.0};
   ntwOffsetPtr->SetPositionOffset(positionOffset);

   // Create the RIC position and velocity offset
   auto    offsetTargetPtr = ut::make_unique<wsf::space::RIC_OffsetTargetPoint>(std::move(ntwOffsetPtr));
   UtVec3d ricPosOffset{0.0, 100.0, 0.0};
   UtVec3d velocityOffset{100.0, 0.0, 0.0};
   offsetTargetPtr->SetPositionOffset(ricPosOffset);
   offsetTargetPtr->SetVelocityOffset(velocityOffset);

   // Compute expected values
   UtVec3d offsetPosition = positionOffset[0] * ntwFrameX + ricPosOffset[0] * ricFrameX + positionOffset[1] * ntwFrameY +
                            ricPosOffset[1] * ricFrameY + positionOffset[2] * ntwFrameZ + ricPosOffset[2] * ricFrameZ;
   UtVec3d expectedPosition = initialPos + offsetPosition;
   UtVec3d omega;
   omega.CrossProduct(initialPos, initialVel);
   omega.Multiply(1.0 / initialPos.MagnitudeSquared());
   UtVec3d nonInertial;
   nonInertial.CrossProduct(omega, offsetPosition);
   UtVec3d expectedVelocity = initialVel + velocityOffset[0] * ricFrameX + velocityOffset[1] * ricFrameY +
                              velocityOffset[2] * ricFrameZ + nonInertial;

   offsetTargetPtr->Update(initialEpoch);

   // Vector for storing test inputs
   UtVec3d testVec{};

   // Test position offset
   testVec = offsetTargetPtr->GetPositionECI();
   EXPECT_NEAR(testVec[0], expectedPosition[0], cTOLERANCE);
   EXPECT_NEAR(testVec[1], expectedPosition[1], cTOLERANCE);
   EXPECT_NEAR(testVec[2], expectedPosition[2], cTOLERANCE);

   // Test velocity offset
   testVec = offsetTargetPtr->GetVelocityECI();
   EXPECT_NEAR(testVec[0], expectedVelocity[0], cTOLERANCE);
   EXPECT_NEAR(testVec[1], expectedVelocity[1], cTOLERANCE);
   EXPECT_NEAR(testVec[2], expectedVelocity[2], cTOLERANCE);

   // Test base pointer kinematics
   auto state = offsetTargetPtr->GetBasePoint().GetOrbitalStateVectorInertial();
   EXPECT_NEAR(state.GetLocation()[0], initialPos[0], cTOLERANCE);
   EXPECT_NEAR(state.GetLocation()[1], initialPos[1], cTOLERANCE);
   EXPECT_NEAR(state.GetLocation()[2], initialPos[2], cTOLERANCE);
   EXPECT_NEAR(state.GetVelocity()[0], initialVel[0], cTOLERANCE);
   EXPECT_NEAR(state.GetVelocity()[1], initialVel[1], cTOLERANCE);
   EXPECT_NEAR(state.GetVelocity()[2], initialVel[2], cTOLERANCE);
}
