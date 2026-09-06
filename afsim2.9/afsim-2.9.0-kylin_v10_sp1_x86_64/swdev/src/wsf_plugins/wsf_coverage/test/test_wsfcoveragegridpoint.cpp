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

#include "WsfCoverageGridPoint.hpp"

TEST(WsfCoverageGridPoint, Point)
{
   constexpr wsf::coverage::PointId cID{1};
   constexpr double                 cLAT{20.0};
   constexpr double                 cLON{50.0};
   constexpr double                 cALT{100.0};
   constexpr double                 cTOLERANCE{1.0e-12};

   wsf::coverage::Point point{cID, cLAT, cLON, cALT};
   EXPECT_EQ(point.GetID(), cID);
   EXPECT_NEAR(point.GetLatitude(), cLAT, cTOLERANCE);
   EXPECT_NEAR(point.GetLongitude(), cLON, cTOLERANCE);
   EXPECT_NEAR(point.GetAltitude(), cALT, cTOLERANCE);

   point.SetLLA(UtLLAPos{});
   EXPECT_EQ(point.GetLLA(), UtLLAPos{});
   EXPECT_NEAR(point.GetLatitude(), 0.0, cTOLERANCE);
   EXPECT_NEAR(point.GetLongitude(), 0.0, cTOLERANCE);
   EXPECT_NEAR(point.GetAltitude(), 0.0, cTOLERANCE);

   point.SetLLA(cLAT, cLON, cALT);
   EXPECT_NEAR(point.GetLatitude(), cLAT, cTOLERANCE);
   EXPECT_NEAR(point.GetLongitude(), cLON, cTOLERANCE);
   EXPECT_NEAR(point.GetAltitude(), cALT, cTOLERANCE);
}
