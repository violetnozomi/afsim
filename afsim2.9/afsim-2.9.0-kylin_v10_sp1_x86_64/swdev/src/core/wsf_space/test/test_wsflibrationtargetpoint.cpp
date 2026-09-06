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
#include "WsfLibrationPoint.hpp"
#include "WsfLibrationTargetPoint.hpp"

TEST(WsfLibrationTargetPoint, EarthMoon)
{
   constexpr double cTOLERANCE = 1.0e-12;

   // Pick an epoch
   UtCalendar initialEpoch{};
   initialEpoch.SetDate(2020, 4, 1);
   initialEpoch.SetTime(12, 0, 0.0);

   wsf::space::LibrationTargetPoint target{wsf::space::LibrationPoint::System::cEARTH_MOON,
                                           wsf::space::LibrationPoint::Point::cL4};
   target.Update(initialEpoch);

   wsf::space::LibrationPoint emPoint{wsf::space::LibrationPoint::System::cEARTH_MOON};
   UtVec3d                    testVec{};
   UtVec3d                    expectedVec{};

   testVec     = target.GetPositionECI();
   expectedVec = emPoint.GetPositionECI(wsf::space::LibrationPoint::Point::cL4, initialEpoch);
   EXPECT_NEAR(testVec[0], expectedVec[0], cTOLERANCE);
   EXPECT_NEAR(testVec[1], expectedVec[1], cTOLERANCE);
   EXPECT_NEAR(testVec[2], expectedVec[2], cTOLERANCE);

   testVec     = target.GetVelocityECI();
   expectedVec = emPoint.GetVelocityECI(wsf::space::LibrationPoint::Point::cL4, initialEpoch);
   EXPECT_NEAR(testVec[0], expectedVec[0], cTOLERANCE);
   EXPECT_NEAR(testVec[1], expectedVec[1], cTOLERANCE);
   EXPECT_NEAR(testVec[2], expectedVec[2], cTOLERANCE);
}
