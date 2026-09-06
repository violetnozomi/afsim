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

#include "CoverageTestService.hpp"
#include "TestCoverageGrid.hpp"
#include "UtInput.hpp"
#include "UtLocator.hpp"

void TestPoint(const wsf::coverage::Point* aPointPtr, wsf::coverage::PointId aID, double aLat, double aLon, double aAlt)
{
   constexpr double cTOLERANCE{1.0e-12};

   EXPECT_TRUE(aPointPtr);
   EXPECT_EQ(aPointPtr->GetID(), aID);
   EXPECT_NEAR(aPointPtr->GetLatitude(), aLat, cTOLERANCE);
   EXPECT_NEAR(aPointPtr->GetLongitude(), aLon, cTOLERANCE);
   EXPECT_NEAR(aPointPtr->GetAltitude(), aAlt, cTOLERANCE);
}

TEST(WsfCoverageGrid, TestGrid)
{
   auto&    testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid grid{testDataService.GetScenario()};
   grid.SetName("test_grid");
   EXPECT_FALSE(grid.IsInitialized());
   EXPECT_EQ(std::string{grid.GetCentralBody().GetName()}, "earth_wgs84");
   EXPECT_EQ(grid.GetSize(), 0u);

   grid.Initialize();
   EXPECT_TRUE(grid.IsInitialized());
   EXPECT_EQ(grid.GetSize(), 4u);

   auto pointPtr = grid.GetPoint(1);
   TestPoint(pointPtr, 1, 0.0, 0.0, 0.0);
   pointPtr = grid.GetPoint(2);
   TestPoint(pointPtr, 2, 0.0, 90.0, 0.0);
   pointPtr = grid.GetPoint(3);
   TestPoint(pointPtr, 3, 45.0, 90.0, 0.0);
   pointPtr = grid.GetPoint(4);
   TestPoint(pointPtr, 4, 45.0, 0.0, 0.0);
   pointPtr = grid.GetPoint(100);
   EXPECT_FALSE(pointPtr);

   auto ga = grid.GetAsset(1);
   EXPECT_TRUE(ga.IsValid());
   EXPECT_EQ(ga.GetPlatformName(), "test_grid1");
   EXPECT_EQ(ga.GetDeviceName(), "none");
   EXPECT_EQ(ga.GetPointId(), 1u);

   ga = grid.GetAsset(100);
   EXPECT_FALSE(ga.IsValid());

   ga = grid.GetAsset("test_grid3");
   EXPECT_TRUE(ga.IsValid());
   EXPECT_EQ(ga.GetPlatformName(), "test_grid3");
   EXPECT_EQ(ga.GetDeviceName(), "none");
   EXPECT_EQ(ga.GetPointId(), 3u);

   ga = grid.GetAsset("unknown");
   EXPECT_FALSE(ga.IsValid());
}

TEST(WsfCoverageGrid, TestGridProcessInput)
{
   auto&    testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid grid{testDataService.GetScenario()};

   UtInput input{};
   input.PushInputString("central_body earth_egm96\nend_central_body\ncentral_body "
                         "unknown\nend_central_body\nno_such_command\ncentral_body moon end_central_body\n");

   // The first line should set the central body
   std::string command;
   input.ReadCommand(command);
   EXPECT_TRUE(grid.ProcessInput(input));
   EXPECT_EQ(std::string{grid.GetCentralBody().GetName()}, "earth_egm96");

   // The next should throw UtInput::BadValue
   input.ReadCommand(command);
   EXPECT_THROW(grid.ProcessInput(input), UtInput::BadValue);

   // The next should return false
   input.ReadCommand(command);
   EXPECT_FALSE(grid.ProcessInput(input));

   // The last should throw UtInput::BadValue
   input.ReadCommand(command);
   EXPECT_THROW(grid.ProcessInput(input), UtInput::BadValue);
}
