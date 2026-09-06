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

#include <gtest/gtest.h>

#include "CoverageTestService.hpp"
#include "TestCoverage.hpp"
#include "TestCoverageGrid.hpp"
#include "TestMeasure.hpp"
#include "TestWriter.hpp"
#include "UtInput.hpp"
#include "UtLocator.hpp"
#include "UtMemory.hpp"
#include "Utils.hpp"
#include "WsfCoverageGridAction.hpp"
#include "WsfCoverageMeasureLatLonStatsOutput.hpp"

TEST(WsfCoverageMeasureLatLonStatsOutput, BadValueInput)
{
   wsf::coverage::MeasureLatLonStatsOutput out{};

   TestBadValueThrow(out, "bin_size 21 m");
   TestBadValueThrow(out, "bin_size -12 deg");
   TestBadValueThrow(out, "bin_size 0 rad");
}

TEST(WsfCoverageMeasureLatLonStatsOutput, ProcessInput)
{
   wsf::coverage::MeasureLatLonStatsOutput out{};

   // First, test default values
   EXPECT_EQ(out.GetProjection(), wsf::coverage::MeasureLatLonStats::Projection::cLONGITUDE);
   EXPECT_NEAR(out.GetBinSizeDeg(), 1.0e-3 / 3600.0, 1.0e-12);

   // Second, test configured values
   std::stringstream stream{};
   stream << "bin_size 1 deg\n";
   stream << "latitude\n";
   stream << "longitude\n";
   stream << "unknown_command\n";
   UtInput input{};
   input.PushInputString(stream.str());

   std::string command;
   input.ReadCommand(command);
   EXPECT_TRUE(out.ProcessInput(input));
   EXPECT_NEAR(out.GetBinSizeDeg(), 1.0, 1.0e-12);

   input.ReadCommand(command);
   EXPECT_TRUE(out.ProcessInput(input));
   EXPECT_EQ(out.GetProjection(), wsf::coverage::MeasureLatLonStats::Projection::cLATITUDE);

   input.ReadCommand(command);
   EXPECT_TRUE(out.ProcessInput(input));
   EXPECT_EQ(out.GetProjection(), wsf::coverage::MeasureLatLonStats::Projection::cLONGITUDE);

   input.ReadCommand(command);
   EXPECT_FALSE(out.ProcessInput(input));
}

namespace // {anonymous}
{

constexpr static const char* cOUTPUT_LON =
   "     Longitude,     Min. Value,     Max. Value,     Mean Value,           Count\n"
   "      0.000000,       0.000000,      45.000000,      22.500000,               2\n"
   "     90.000000,      90.000000,     135.000000,     112.500000,               2\n";
constexpr static const char* cOUTPUT_LAT =
   "      Latitude,     Min. Value,     Max. Value,     Mean Value,           Count\n"
   "      0.000000,       0.000000,      90.000000,      45.000000,               2\n"
   "     45.000000,      45.000000,     135.000000,      90.000000,               2\n";

} // namespace

TEST(WsfCoverageMeasureLatLonStatsOutput, TestLonOutput)
{
   auto&       testDataService = ut::Locator<CoverageTestService>::get();
   TestMeasure moe{testDataService.GetScenario()};
   moe.SetName("moe");
   TestGrid grid{testDataService.GetScenario()};
   grid.SetName("grid");
   grid.Initialize();
   TestCoverage coverage{testDataService.GetScenario(), &grid, "coverage"};

   // Set the values to the sum of the latitude and longitude of the point.
   auto action = wsf::coverage::CreateGridAction(
      [&moe](const wsf::coverage::Grid& aGrid, const wsf::coverage::Point& aPoint)
      {
         auto ga = aGrid.GetAsset(aPoint.GetID());
         moe.SetValue(ga, aPoint.GetLongitude() + aPoint.GetLatitude());
      });
   grid.Invoke(*action);

   auto writerPtr = ut::make_unique<TestWriter>();
   writerPtr->SetWidth(15);
   wsf::coverage::MeasureLatLonStatsOutput out{std::move(writerPtr)};
   out.SetFileNameToDefault(coverage, moe);
   out.Write(coverage, moe);
   EXPECT_EQ(out.GetFileName(), std::string{"coverage_moe_lon_stats.csv"});

   TestWriter& writer = dynamic_cast<TestWriter&>(out.GetWriter());
   EXPECT_EQ(writer.GetStreamContents(), std::string{cOUTPUT_LON});
}

TEST(WsfCoverageMeasureLatLonStatsOutput, TestLatOutput)
{
   auto&       testDataService = ut::Locator<CoverageTestService>::get();
   TestMeasure moe{testDataService.GetScenario()};
   moe.SetName("moe");
   TestGrid grid{testDataService.GetScenario()};
   grid.SetName("grid");
   grid.Initialize();
   TestCoverage coverage{testDataService.GetScenario(), &grid, "coverage"};

   // Set the values to the sum of the latitude and longitude of the point.
   auto action = wsf::coverage::CreateGridAction(
      [&moe](const wsf::coverage::Grid& aGrid, const wsf::coverage::Point& aPoint)
      {
         auto ga = aGrid.GetAsset(aPoint.GetID());
         moe.SetValue(ga, aPoint.GetLongitude() + aPoint.GetLatitude());
      });
   grid.Invoke(*action);

   auto writerPtr = ut::make_unique<TestWriter>();
   writerPtr->SetWidth(15);
   wsf::coverage::MeasureLatLonStatsOutput out{std::move(writerPtr)};
   UtInput                                 input{};
   input.PushInputString("latitude");
   out.ProcessInput(input);
   EXPECT_EQ(out.GetProjection(), wsf::coverage::MeasureLatLonStats::Projection::cLATITUDE);

   out.SetFileNameToDefault(coverage, moe);
   out.Write(coverage, moe);
   EXPECT_EQ(out.GetFileName(), std::string{"coverage_moe_lat_stats.csv"});

   TestWriter& writer = dynamic_cast<TestWriter&>(out.GetWriter());
   EXPECT_EQ(writer.GetStreamContents(), std::string{cOUTPUT_LAT});
}
