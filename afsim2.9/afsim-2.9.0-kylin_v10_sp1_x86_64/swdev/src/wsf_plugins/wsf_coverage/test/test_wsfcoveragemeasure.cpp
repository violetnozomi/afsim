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

#include <sstream>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "CoverageTestService.hpp"
#include "TestCoverage.hpp"
#include "TestCoverageGrid.hpp"
#include "TestMeasure.hpp"
#include "UtInput.hpp"
#include "UtLocator.hpp"
#include "Utils.hpp"
#include "WsfCoverageAsset.hpp"
#include "WsfCoverageGrid.hpp"
#include "WsfCoverageGridAction.hpp"
#include "WsfCoverageMeasureGridStats.hpp"
#include "WsfCoverageMeasureLatLonStats.hpp"

TEST(WsfCoverageMeasure, ProcessInputFailures)
{
   auto&       testDataService = ut::Locator<CoverageTestService>::get();
   TestMeasure moe{testDataService.GetScenario()};

   TestBadValueThrow(moe, "output unknown_type end_output");
   TestBadValueThrow(moe, "output end_output");
   TestEndOfDataThrow(moe, "output");
}

TEST(WsfCoverageMeasure, OutputObjectsCreated)
{
   std::stringstream stream{};
   stream << "output data end_output\n";
   stream << "output grid_stats end_output\n";
   stream << "output lat_lon_stats end_output\n";
   UtInput input{};
   input.PushInputString(stream.str());

   auto&       testDataService = ut::Locator<CoverageTestService>::get();
   TestMeasure moe{testDataService.GetScenario()};
   try
   {
      std::string command;
      while (true)
      {
         input.ReadCommand(command);
         moe.ProcessInput(input);
      }
   }
   catch (UtInput::EndOfData&)
   {
      // Expected exception; do nothing.
   }

   EXPECT_EQ(moe.NumOutputsOfType("data"), 1);
   EXPECT_EQ(moe.NumOutputsOfType("grid_stats"), 1);
   EXPECT_EQ(moe.NumOutputsOfType("lat_lon_stats"), 1);
}

TEST(WsfCoverageMeasure, GetSetOfTestClass)
{
   auto& testDataService = ut::Locator<CoverageTestService>::get();

   TestMeasure moe{testDataService.GetScenario()};
   TestGrid    grid{testDataService.GetScenario()};
   grid.SetName("grid");
   grid.Initialize();
   TestCoverage coverage{testDataService.GetScenario(), &grid, "coverage"};

   // Do an action to set the values
   std::vector<wsf::coverage::GridAsset> assets{};
   auto                                  action = wsf::coverage::CreateGridAction(
      [&moe, &assets](const wsf::coverage::Grid& aGrid, const wsf::coverage::Point& aPoint)
      {
         auto ga = aGrid.GetAsset(aPoint.GetID());
         moe.SetValue(ga, static_cast<double>(aPoint.GetID()));
         assets.push_back(ga);
      });
   grid.Invoke(*action);

   // Now test all the values
   for (const auto& asset : assets)
   {
      EXPECT_NEAR(moe.GetMeasuredValue(asset), static_cast<double>(asset.GetPointId()), 1.0e-12);
   }
}

TEST(WsfCoverageMeasure, GridStats)
{
   auto& testDataService = ut::Locator<CoverageTestService>::get();

   TestMeasure moe{testDataService.GetScenario()};
   TestGrid    grid{testDataService.GetScenario()};
   grid.SetName("grid");
   grid.Initialize();
   TestCoverage coverage{testDataService.GetScenario(), &grid, "coverage"};

   // Set the values to (1, 2, 3, 4)
   auto action = wsf::coverage::CreateGridAction(
      [&moe](const wsf::coverage::Grid& aGrid, const wsf::coverage::Point& aPoint)
      {
         auto ga = aGrid.GetAsset(aPoint.GetID());
         moe.SetValue(ga, static_cast<double>(aPoint.GetID()));
      });
   grid.Invoke(*action);

   constexpr double cTOLERANCE{1.0e-12};
   constexpr double cMIN_EXPECTED{1.0};  //  min(1, 2, 3, 4) = 1
   constexpr double cMAX_EXPECTED{4.0};  //  max(1, 2, 3, 4) = 4
   constexpr double cMEAN_EXPECTED{2.5}; // mean(1, 2, 3, 4) = 2.5
   constexpr int    cCOUNT_EXPECTED{4};

   wsf::coverage::MeasureGridStats stats{coverage, moe};
   EXPECT_NEAR(stats.GetMinValue(), cMIN_EXPECTED, cTOLERANCE);
   EXPECT_NEAR(stats.GetMaxValue(), cMAX_EXPECTED, cTOLERANCE);
   EXPECT_NEAR(stats.GetMeanValue(), cMEAN_EXPECTED, cTOLERANCE);
   EXPECT_EQ(stats.GetCount(), cCOUNT_EXPECTED);
}

TEST(WsfCoverageMeasure, LatProjectionStats)
{
   auto&       testDataService = ut::Locator<CoverageTestService>::get();
   TestMeasure moe{testDataService.GetScenario()};
   TestGrid    grid{testDataService.GetScenario()};
   grid.SetName("grid");
   grid.Initialize();
   TestCoverage coverage{testDataService.GetScenario(), &grid, "coverage"};

   // Set values equal to the sum of latitude and longitude of the point.
   auto action = wsf::coverage::CreateGridAction(
      [&moe](const wsf::coverage::Grid& aGrid, const wsf::coverage::Point& aPoint)
      {
         auto ga = aGrid.GetAsset(aPoint.GetID());
         moe.SetValue(ga, aPoint.GetLongitude() + aPoint.GetLatitude());
      });
   grid.Invoke(*action);

   constexpr double cMINIMUM_BIN_SIZE = 1.0 / 3600.0; // 1 arc second
   constexpr double cTOLERANCE{1.0e-12};

   wsf::coverage::MeasureLatLonStats stats{coverage,
                                           moe,
                                           wsf::coverage::MeasureLatLonStats::Projection::cLATITUDE,
                                           cMINIMUM_BIN_SIZE};
   EXPECT_EQ(stats.GetProjection(), wsf::coverage::MeasureLatLonStats::Projection::cLATITUDE);
   EXPECT_EQ(stats.GetProjectionDescription(), std::string{"Latitude"});
   EXPECT_EQ(stats.GetSize(), 2u);

   EXPECT_NEAR(stats.GetVariable(0), grid.GetMinLat(), cTOLERANCE);
   EXPECT_EQ(stats.GetCount(0), 2);
   EXPECT_NEAR(stats.GetMinValue(0), grid.GetMinLon() + grid.GetMinLat(), cTOLERANCE);
   EXPECT_NEAR(stats.GetMaxValue(0), grid.GetMaxLon() + grid.GetMinLat(), cTOLERANCE);
   EXPECT_NEAR(stats.GetMeanValue(0), grid.GetMeanLon() + grid.GetMinLat(), cTOLERANCE);

   EXPECT_NEAR(stats.GetVariable(1), grid.GetMaxLat(), cTOLERANCE);
   EXPECT_EQ(stats.GetCount(1), 2);
   EXPECT_NEAR(stats.GetMinValue(1), grid.GetMinLon() + grid.GetMaxLat(), cTOLERANCE);
   EXPECT_NEAR(stats.GetMaxValue(1), grid.GetMaxLon() + grid.GetMaxLat(), cTOLERANCE);
   EXPECT_NEAR(stats.GetMeanValue(1), grid.GetMeanLon() + grid.GetMaxLat(), cTOLERANCE);
}

TEST(WsfCoverageMeasure, LonProjectionStats)
{
   auto&       testDataService = ut::Locator<CoverageTestService>::get();
   TestMeasure moe{testDataService.GetScenario()};
   TestGrid    grid{testDataService.GetScenario()};
   grid.SetName("grid");
   grid.Initialize();
   TestCoverage coverage{testDataService.GetScenario(), &grid, "coverage"};

   // Set values equal to the sum of latitude and longitude of the point.
   auto action = wsf::coverage::CreateGridAction(
      [&moe](const wsf::coverage::Grid& aGrid, const wsf::coverage::Point& aPoint)
      {
         auto ga = aGrid.GetAsset(aPoint.GetID());
         moe.SetValue(ga, aPoint.GetLongitude() + aPoint.GetLatitude());
      });
   grid.Invoke(*action);

   constexpr double cMINIMUM_BIN_SIZE = 1.0 / 3600.0; // 1 arc second
   constexpr double cTOLERANCE{1.0e-12};

   wsf::coverage::MeasureLatLonStats stats{coverage,
                                           moe,
                                           wsf::coverage::MeasureLatLonStats::Projection::cLONGITUDE,
                                           cMINIMUM_BIN_SIZE};
   EXPECT_EQ(stats.GetProjection(), wsf::coverage::MeasureLatLonStats::Projection::cLONGITUDE);
   EXPECT_EQ(stats.GetProjectionDescription(), std::string{"Longitude"});
   EXPECT_EQ(stats.GetSize(), 2u);

   EXPECT_NEAR(stats.GetVariable(0), grid.GetMinLon(), cTOLERANCE);
   EXPECT_EQ(stats.GetCount(0), 2);
   EXPECT_NEAR(stats.GetMinValue(0), grid.GetMinLat() + grid.GetMinLon(), cTOLERANCE);
   EXPECT_NEAR(stats.GetMaxValue(0), grid.GetMaxLat() + grid.GetMinLon(), cTOLERANCE);
   EXPECT_NEAR(stats.GetMeanValue(0), grid.GetMeanLat() + grid.GetMinLon(), cTOLERANCE);

   EXPECT_NEAR(stats.GetVariable(1), grid.GetMaxLon(), cTOLERANCE);
   EXPECT_EQ(stats.GetCount(1), 2);
   EXPECT_NEAR(stats.GetMinValue(1), grid.GetMinLat() + grid.GetMaxLon(), cTOLERANCE);
   EXPECT_NEAR(stats.GetMaxValue(1), grid.GetMaxLat() + grid.GetMaxLon(), cTOLERANCE);
   EXPECT_NEAR(stats.GetMeanValue(1), grid.GetMeanLat() + grid.GetMaxLon(), cTOLERANCE);
}

TEST(WsfCoverageMeasure, ProjectionBinning)
{
   auto&                             testDataService = ut::Locator<CoverageTestService>::get();
   TestMeasure                       moe{testDataService.GetScenario()};
   std::vector<wsf::coverage::Point> points{};
   points.emplace_back(1, 0.0, 0.0, 0.0);
   points.emplace_back(2, 0.0, 0.25, 0.0);
   points.emplace_back(3, 0.0, 1.0, 0.0);
   points.emplace_back(4, 0.0, 1.25, 0.0);
   points.emplace_back(5, 0.0, 2.0, 0.0);
   points.emplace_back(6, 0.0, 2.25, 0.0);
   TestGrid grid{testDataService.GetScenario(), points};
   grid.SetName("grid");
   grid.Initialize();
   TestCoverage coverage{testDataService.GetScenario(), &grid, "coverage"};

   // Set values equal to 1.
   auto action =
      wsf::coverage::CreateGridAction([&moe](const wsf::coverage::Grid& aGrid, const wsf::coverage::Point& aPoint)
                                      { moe.SetValue(aGrid.GetAsset(aPoint.GetID()), 1.0); });
   grid.Invoke(*action);

   wsf::coverage::MeasureLatLonStats narrow{coverage, moe, wsf::coverage::MeasureLatLonStats::Projection::cLONGITUDE, 0.01};
   EXPECT_EQ(narrow.GetSize(), 6u);
   for (std::size_t i = 0; i < narrow.GetSize(); ++i)
   {
      EXPECT_EQ(narrow.GetCount(i), 1);
   }

   wsf::coverage::MeasureLatLonStats wide{coverage, moe, wsf::coverage::MeasureLatLonStats::Projection::cLONGITUDE, 1.0};
   EXPECT_EQ(wide.GetSize(), 3u);
   for (std::size_t i = 0; i < wide.GetSize(); ++i)
   {
      EXPECT_EQ(wide.GetCount(i), 2);
      EXPECT_NEAR(wide.GetVariable(i), static_cast<double>(i), 1.0e-12);
   }
}
