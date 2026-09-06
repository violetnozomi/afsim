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

#include "CoverageTestService.hpp"
#include "TestCoverage.hpp"
#include "TestCoverageGrid.hpp"
#include "TestMeasure.hpp"
#include "TestWriter.hpp"
#include "UtLocator.hpp"
#include "UtMemory.hpp"
#include "Utils.hpp"
#include "WsfCoverageGridAction.hpp"
#include "WsfCoverageMeasureCSV_Output.hpp"

// NOTE: wsf::coverage::MeasureCSV_Output does not define any inputs beyond the common
//       ones in wsf::coverage::MeasureTextOutput and wsf::coverage::MeasureOutput.
//       Those are tested elsewhere, and so repeat tests are not performed here.

namespace // {anonymous}
{

constexpr static const char* cMEASURE_CSV_OUTPUT =
   "    Lat. [deg],     Lon. [deg],       Alt. [m],       Point ID,      Test Value\n"
   "      0.000000,       0.000000,       0.000000,              1,        0.000000\n"
   "      0.000000,      90.000000,       0.000000,              2,       90.000000\n"
   "     45.000000,      90.000000,       0.000000,              3,      135.000000\n"
   "     45.000000,       0.000000,       0.000000,              4,       45.000000\n";

} // namespace

TEST(WsfCoverageMeasureCSV_Output, TestOutput)
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
   wsf::coverage::MeasureCSV_Output out{std::move(writerPtr)};
   out.SetFileNameToDefault(coverage, moe);
   out.Write(coverage, moe);
   EXPECT_EQ(out.GetFileName(), std::string{"coverage_moe_data.csv"});

   TestWriter& writer = dynamic_cast<TestWriter&>(out.GetWriter());
   EXPECT_EQ(writer.GetStreamContents(), std::string{cMEASURE_CSV_OUTPUT});
}
