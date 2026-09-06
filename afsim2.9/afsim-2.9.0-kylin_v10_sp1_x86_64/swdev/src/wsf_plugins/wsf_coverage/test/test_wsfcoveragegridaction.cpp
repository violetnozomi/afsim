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
#include "UtLocator.hpp"
#include "WsfCoverageGridAction.hpp"

TEST(WsfCoverageGridAction, GridActionCall)
{
   constexpr wsf::coverage::PointId cPOINTID{1};
   constexpr double                 cLATITUDE{20.0};
   constexpr double                 cLONGITUDE{30.0};
   constexpr double                 cALTITUDE{40.0};
   const std::string                cGRIDNAME{"test_grid"};
   constexpr double                 cTOLERANCE{1.0e-12};

   auto&    testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid grid{testDataService.GetScenario()};
   grid.SetName("test_grid");
   wsf::coverage::Point point{cPOINTID, cLATITUDE, cLONGITUDE, cALTITUDE};

   auto action = wsf::coverage::CreateGridAction(
      [cPOINTID, cLATITUDE, cLONGITUDE, cALTITUDE, cGRIDNAME, cTOLERANCE](const wsf::coverage::Grid&  aGrid,
                                                                          const wsf::coverage::Point& aPoint)
      {
         EXPECT_EQ(aGrid.GetName(), cGRIDNAME);
         EXPECT_EQ(aPoint.GetID(), cPOINTID);
         EXPECT_NEAR(aPoint.GetLatitude(), cLATITUDE, cTOLERANCE);
         EXPECT_NEAR(aPoint.GetLongitude(), cLONGITUDE, cTOLERANCE);
         EXPECT_NEAR(aPoint.GetAltitude(), cALTITUDE, cTOLERANCE);
      });

   action->Call(grid, point);
}

TEST(WsfCoverageGridAction, GridActionInvoke)
{
   auto&       testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid    grid{testDataService.GetScenario()};
   std::size_t counter{0};

   auto action = wsf::coverage::CreateGridAction(
      [&counter](const wsf::coverage::Grid& aGrid, const wsf::coverage::Point& aPoint) { ++counter; });

   grid.Invoke(*action);

   EXPECT_EQ(counter, grid.GetSize());
}
