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
#include "UtVec3.hpp"
#include "WsfSpaceKinematicStateTargetPoint.hpp"

static void CompareVectors(const UtVec3d& aMeasured, const UtVec3d& aExpected, double aTolerance)
{
   EXPECT_NEAR(aMeasured.Get(0), aExpected.Get(0), aTolerance);
   EXPECT_NEAR(aMeasured.Get(1), aExpected.Get(1), aTolerance);
   EXPECT_NEAR(aMeasured.Get(2), aExpected.Get(2), aTolerance);
}

TEST(WsfSpaceKinematicStateTargetPoint, BasicTests)
{
   constexpr double cTOLERANCE = 1.0e-12;

   UtVec3d                               testPosition{1.0e6, 2.0e6, 3.0e6};
   UtVec3d                               testVelocity{100.0, 200.0, 300.0};
   UtCalendar                            defaultEpoch{};
   wsf::space::KinematicStateTargetPoint point{testPosition, testVelocity};

   CompareVectors(point.GetPositionECI(), testPosition, cTOLERANCE);
   CompareVectors(point.GetVelocityECI(), testVelocity, cTOLERANCE);
   auto state = point.GetOrbitalStateVectorInertial();
   CompareVectors(state.GetLocation(), testPosition, cTOLERANCE);
   CompareVectors(state.GetVelocity(), testVelocity, cTOLERANCE);
   EXPECT_EQ(point.GetEpoch(), defaultEpoch);

   // Update the point to a new time, which does not change the position or velocity.
   UtCalendar laterEpoch{};
   laterEpoch.SetDate(2021, 6, 8);
   laterEpoch.SetTime(11, 41, 0.0);
   point.Update(laterEpoch);

   CompareVectors(point.GetPositionECI(), testPosition, cTOLERANCE);
   CompareVectors(point.GetVelocityECI(), testVelocity, cTOLERANCE);
   EXPECT_EQ(point.GetEpoch(), laterEpoch);

   // test get characteristic time
   double r            = testPosition.Magnitude();
   double expectedTime = UtMath::cTWO_PI * std::sqrt(r * r * r / ut::EarthEllipsoidWGS84().GetGravitationalParameter());
   EXPECT_NEAR(point.GetCharacteristicTime(), expectedTime, cTOLERANCE);
}
