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
#include "WsfCoverageMeasureGridStatsOutput.hpp"

TEST(WsfCoverageMeasureGridStatsOutput, BadValueInput)
{
   wsf::coverage::MeasureGridStatsOutput out{};

   TestBadValueThrow(out, "precision 3");
   TestBadValueThrow(out, "precision 40");
   TestBadValueThrow(out, "width 3");
   TestBadValueThrow(out, "width 40");
   TestBadValueThrow(out, "justify center");
   TestBadValueThrow(out, "format default");
}

TEST(WsfCoverageMeasureGridStatsOutput, ProcessInput)
{
   wsf::coverage::MeasureGridStatsOutput out{};

   // First, test default values
   EXPECT_EQ(out.GetWriter().GetPrecision(), 6);
   EXPECT_EQ(out.GetWriter().GetWidth(), 15);
   EXPECT_TRUE(out.GetWriter().IsRightJustified());
   EXPECT_TRUE(out.GetWriter().IsFixedFormat());
   EXPECT_TRUE(out.GetFileName().empty());

   // Second, test configured values
   std::stringstream stream{};
   stream << "precision 10\n";
   stream << "width 16\n";
   stream << "justify left\n";
   stream << "justify right\n";
   stream << "format scientific\n";
   stream << "format fixed\n";
   stream << "file test_file_name.txt\n";
   UtInput input{};
   input.PushInputString(stream.str());

   std::string command;
   input.ReadCommand(command);
   EXPECT_TRUE(out.ProcessInput(input));
   EXPECT_EQ(out.GetWriter().GetPrecision(), 10);

   input.ReadCommand(command);
   EXPECT_TRUE(out.ProcessInput(input));
   EXPECT_EQ(out.GetWriter().GetWidth(), 16);

   input.ReadCommand(command);
   EXPECT_TRUE(out.ProcessInput(input));
   EXPECT_FALSE(out.GetWriter().IsRightJustified());

   input.ReadCommand(command);
   EXPECT_TRUE(out.ProcessInput(input));
   EXPECT_TRUE(out.GetWriter().IsRightJustified());

   input.ReadCommand(command);
   EXPECT_TRUE(out.ProcessInput(input));
   EXPECT_FALSE(out.GetWriter().IsFixedFormat());

   input.ReadCommand(command);
   EXPECT_TRUE(out.ProcessInput(input));
   EXPECT_TRUE(out.GetWriter().IsFixedFormat());

   input.ReadCommand(command);
   EXPECT_TRUE(out.ProcessInput(input));
   EXPECT_EQ(out.GetFileName(), std::string{"test_file_name.txt"});
}

namespace // {anonymous}
{

constexpr static const char* cOUTPUT_RIGHT_FIXED = "    Min. Value,     Max. Value,     Mean Value,           Count\n"
                                                   "      1.000000,       4.000000,       2.500000,               4\n";
constexpr static const char* cOUTPUT_LEFT_FIXED  = "Min. Value,     Max. Value,     Mean Value,     Count          \n"
                                                   "1.000000,       4.000000,       2.500000,       4              \n";
constexpr static const char* cOUTPUT_RIGHT_SCIENTIFIC =
   "    Min. Value,     Max. Value,     Mean Value,           Count\n"
   "  1.000000e+00,   4.000000e+00,   2.500000e+00,               4\n";
constexpr static const char* cOUTPUT_LEFT_SCIENTIFIC =
   "Min. Value,     Max. Value,     Mean Value,     Count          \n"
   "1.000000e+00,   4.000000e+00,   2.500000e+00,   4              \n";

} // namespace

void TestJustifyAndFormat(const TestCoverage& aCoverage,
                          const TestMeasure&  aMOE,
                          bool                aRightJustify,
                          bool                aFixedFormat,
                          const char*         aExpectedOutput)
{
   auto writerPtr = ut::make_unique<TestWriter>();
   writerPtr->SetRightJustified(aRightJustify);
   writerPtr->SetFixedFormat(aFixedFormat);
   wsf::coverage::MeasureGridStatsOutput out{std::move(writerPtr)};
   out.SetFileNameToDefault(aCoverage, aMOE);
   out.Write(aCoverage, aMOE);
   EXPECT_EQ(out.GetFileName(), std::string{"coverage_moe_grid_stats.csv"});

   TestWriter& writer = dynamic_cast<TestWriter&>(out.GetWriter());
   EXPECT_EQ(writer.GetStreamContents(), std::string{aExpectedOutput});
}

TEST(WsfCoverageMeasureGridStatsOutput, TestOutput)
{
   auto&       testDataService = ut::Locator<CoverageTestService>::get();
   TestMeasure moe{testDataService.GetScenario()};
   moe.SetName("moe");
   TestGrid grid{testDataService.GetScenario()};
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

   TestJustifyAndFormat(coverage, moe, true, true, cOUTPUT_RIGHT_FIXED);
   TestJustifyAndFormat(coverage, moe, false, true, cOUTPUT_LEFT_FIXED);
   TestJustifyAndFormat(coverage, moe, true, false, cOUTPUT_RIGHT_SCIENTIFIC);
   TestJustifyAndFormat(coverage, moe, false, false, cOUTPUT_LEFT_SCIENTIFIC);
}
