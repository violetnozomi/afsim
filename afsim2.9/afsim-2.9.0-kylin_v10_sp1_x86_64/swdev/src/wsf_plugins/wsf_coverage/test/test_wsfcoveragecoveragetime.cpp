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

#include <gtest/gtest.h>

#include "CoverageTestService.hpp"
#include "TestCoverage.hpp"
#include "TestCoverageGrid.hpp"
#include "UtInput.hpp"
#include "UtLocator.hpp"
#include "Utils.hpp"
#include "WsfCoverageCoverageTime.hpp"

namespace
{

constexpr double cCOVERAGE_TIME_TOLERANCE{1.0e-9};

}

TEST(WsfCoverageCoverageTime, BadInput)
{
   auto&                       testDataService = ut::Locator<CoverageTestService>::get();
   wsf::coverage::CoverageTime moe{testDataService.GetScenario()};

   TestBadValueThrow(moe, "subtype unknown");
   TestEndOfDataThrow(moe, "subtype percent_time_above");
   TestBadValueThrow(moe, "subtype percent_time_above -1");
   TestEndOfDataThrow(moe, "subtype total_time_above");
   TestBadValueThrow(moe, "subtype total_time_above -1");
   TestEndOfDataThrow(moe, "subtype minimum_per");
   TestBadValueThrow(moe, "subtype minimum_per -1 day");
   TestBadValueThrow(moe, "subtype minimum_per 1 meter");
   TestBadValueThrow(moe, "subtype minimum_per 5 hours");
   TestEndOfDataThrow(moe, "subtype minimum_percent_per");
   TestBadValueThrow(moe, "subtype minimum_percent_per -1 day");
   TestBadValueThrow(moe, "subtype minimum_percent_per 1 meter");
   TestBadValueThrow(moe, "subtype minimum_percent_per 5 hours");
   TestEndOfDataThrow(moe, "subtype maximum_per");
   TestBadValueThrow(moe, "subtype maximum_per -1 day");
   TestBadValueThrow(moe, "subtype maximum_per 1 meter");
   TestBadValueThrow(moe, "subtype maximum_per 5 hours");
   TestEndOfDataThrow(moe, "subtype maximum_percent_per");
   TestBadValueThrow(moe, "subtype maximum_percent_per -1 day");
   TestBadValueThrow(moe, "subtype maximum_percent_per 1 meter");
   TestBadValueThrow(moe, "subtype maximum_percent_per 5 hours");
   TestEndOfDataThrow(moe, "subtype standard_deviation_per");
   TestBadValueThrow(moe, "subtype standard_deviation_per -1 day");
   TestBadValueThrow(moe, "subtype standard_deviation_per 1 meter");
   TestBadValueThrow(moe, "subtype standard_deviation_per 5 hours");
   TestEndOfDataThrow(moe, "subtype standard_deviation_percent_per");
   TestEndOfDataThrow(moe, "subtype standard_deviation_percent_per");
   TestBadValueThrow(moe, "subtype standard_deviation_percent_per -1 day");
   TestBadValueThrow(moe, "subtype standard_deviation_percent_per 1 meter");
   TestBadValueThrow(moe, "subtype standard_deviation_percent_per 5 hours");
   TestEndOfDataThrow(moe, "subtype mean_per");
   TestBadValueThrow(moe, "subtype mean_per -1 day");
   TestBadValueThrow(moe, "subtype mean_per 1 meter");
   TestEndOfDataThrow(moe, "subtype mean_percent_per");
   TestBadValueThrow(moe, "subtype mean_percent_per -1 day");
   TestBadValueThrow(moe, "subtype mean_percent_per 1 meter");
   TestFalseReturn(moe, "unknown");
}

TEST(WsfCoverageCoverageTime, ProcessInputSubTypes)
{
   std::stringstream inputStream{};
   // clang-format off
   inputStream << "subtype total\n"
               << "subtype percent\n"
               << "subtype total_time_above 2\n"
               << "subtype percent_time_above 2\n"
               << "subtype maximum_per 1 day\n"
               << "subtype maximum_percent_per 1 day\n"
               << "subtype minimum_per 1 day\n"
               << "subtype minimum_percent_per 1 day\n"
               << "subtype mean_per 1 day\n"
               << "subtype mean_percent_per 1 day\n"
               << "subtype standard_deviation_per 1 day\n"
               << "subtype standard_deviation_percent_per 1 day\n";
   // clang-format on
   UtInput input{};
   input.PushInputString(inputStream.str());

   auto&                       testDataService = ut::Locator<CoverageTestService>::get();
   wsf::coverage::CoverageTime moe{testDataService.GetScenario()};

   std::string command;
   input.ReadCommand(command);
   EXPECT_TRUE(moe.ProcessInput(input));
   EXPECT_EQ(moe.GetSubType(), wsf::coverage::CoverageTime::SubType::cTOTAL);

   input.ReadCommand(command);
   EXPECT_TRUE(moe.ProcessInput(input));
   EXPECT_EQ(moe.GetSubType(), wsf::coverage::CoverageTime::SubType::cPERCENT);

   input.ReadCommand(command);
   EXPECT_TRUE(moe.ProcessInput(input));
   EXPECT_EQ(moe.GetSubType(), wsf::coverage::CoverageTime::SubType::cTOTAL_TIME_ABOVE);
   EXPECT_EQ(moe.GetParameter(), 2);

   input.ReadCommand(command);
   EXPECT_TRUE(moe.ProcessInput(input));
   EXPECT_EQ(moe.GetSubType(), wsf::coverage::CoverageTime::SubType::cPERCENT_TIME_ABOVE);
   EXPECT_EQ(moe.GetParameter(), 2);

   input.ReadCommand(command);
   EXPECT_TRUE(moe.ProcessInput(input));
   EXPECT_EQ(moe.GetSubType(), wsf::coverage::CoverageTime::SubType::cMAX_PER_TIME);
   EXPECT_EQ(moe.GetSampleSpan(), 3600 * 24);

   input.ReadCommand(command);
   EXPECT_TRUE(moe.ProcessInput(input));
   EXPECT_EQ(moe.GetSubType(), wsf::coverage::CoverageTime::SubType::cMAX_PERCENT_PER_TIME);
   EXPECT_EQ(moe.GetSampleSpan(), 3600 * 24);

   input.ReadCommand(command);
   EXPECT_TRUE(moe.ProcessInput(input));
   EXPECT_EQ(moe.GetSubType(), wsf::coverage::CoverageTime::SubType::cMIN_PER_TIME);
   EXPECT_EQ(moe.GetSampleSpan(), 3600 * 24);

   input.ReadCommand(command);
   EXPECT_TRUE(moe.ProcessInput(input));
   EXPECT_EQ(moe.GetSubType(), wsf::coverage::CoverageTime::SubType::cMIN_PERCENT_PER_TIME);
   EXPECT_EQ(moe.GetSampleSpan(), 3600 * 24);

   input.ReadCommand(command);
   EXPECT_TRUE(moe.ProcessInput(input));
   EXPECT_EQ(moe.GetSubType(), wsf::coverage::CoverageTime::SubType::cMEAN_PER_TIME);
   EXPECT_EQ(moe.GetSampleSpan(), 3600 * 24);

   input.ReadCommand(command);
   EXPECT_TRUE(moe.ProcessInput(input));
   EXPECT_EQ(moe.GetSubType(), wsf::coverage::CoverageTime::SubType::cMEAN_PERCENT_PER_TIME);
   EXPECT_EQ(moe.GetSampleSpan(), 3600 * 24);

   input.ReadCommand(command);
   EXPECT_TRUE(moe.ProcessInput(input));
   EXPECT_EQ(moe.GetSubType(), wsf::coverage::CoverageTime::SubType::cSTD_DEV_PER_TIME);
   EXPECT_EQ(moe.GetSampleSpan(), 3600 * 24);

   input.ReadCommand(command);
   EXPECT_TRUE(moe.ProcessInput(input));
   EXPECT_EQ(moe.GetSubType(), wsf::coverage::CoverageTime::SubType::cSTD_DEV_PERCENT_PER_TIME);
   EXPECT_EQ(moe.GetSampleSpan(), 3600 * 24);
}

void SetupIntervals(TestGrid&                    aGrid,
                    TestCoverage&                aCoverage,
                    wsf::coverage::CoverageTime& aMoe,
                    const WsfScenario&           aScenario,
                    const std::string&           aMoeInput)
{
   aGrid.SetName("test_grid");
   aGrid.Initialize();
   {
      UtInput input{};
      input.PushInputString("start_time 0.0 s\nend_time 100.0 s\n");
      std::string command;
      input.ReadCommand(command);
      EXPECT_TRUE(aCoverage.ProcessInput(input));
      input.ReadCommand(command);
      EXPECT_TRUE(aCoverage.ProcessInput(input));
   }

   {
      UtInput input{};
      input.PushInputString(aMoeInput);
      std::string command;
      input.ReadCommand(command);
      EXPECT_TRUE(aMoe.ProcessInput(input));
   }

   aMoe.Initialize(aCoverage);
   aMoe.CollectionStarting(aCoverage, 0.0);

   // Manually set some intervals on coverage
   wsf::coverage::FreeAsset fa{"test", "sensor"};

   auto someCoverage = aGrid.GetAsset(2);
   aCoverage.InjectInterval(someCoverage, fa, 3.0, 6.0);   // 3
   aCoverage.InjectInterval(someCoverage, fa, 12.0, 13.0); // 1
   aCoverage.InjectInterval(someCoverage, fa, 17.0, 27.0); // 10
   aCoverage.InjectInterval(someCoverage, fa, 35.0, 41.0); // 5
   aCoverage.InjectInterval(someCoverage, fa, 40.0, 55.0); // 15
   aCoverage.InjectInterval(someCoverage, fa, 66.0, 72.0); // 6
   aCoverage.InjectInterval(someCoverage, fa, 70.0, 83.0); // 13
   aCoverage.InjectInterval(someCoverage, fa, 68.0, 88.0); // 20
   // 30-second samples:
   // 14
   // 20
   // 22
   // 0
   // Total:
   // 56

   aMoe.CollectionCompleting(aCoverage, 100.0);
}

// This is the same concept as SetupIntervals, except that these are counted in days.
void SetupLongIntervals(TestGrid&                    aGrid,
                        TestCoverage&                aCoverage,
                        wsf::coverage::CoverageTime& aMoe,
                        const WsfScenario&           aScenario,
                        const std::string&           aMoeInput)
{
   aGrid.SetName("test_grid");
   aGrid.Initialize();
   {
      UtInput input{};
      input.PushInputString("start_time 0.0 s\nend_time 4.0 days\n");
      std::string command;
      input.ReadCommand(command);
      EXPECT_TRUE(aCoverage.ProcessInput(input));
      input.ReadCommand(command);
      EXPECT_TRUE(aCoverage.ProcessInput(input));
   }

   {
      UtInput input{};
      input.PushInputString(aMoeInput);
      std::string command;
      input.ReadCommand(command);
      EXPECT_TRUE(aMoe.ProcessInput(input));
   }

   aMoe.Initialize(aCoverage);
   aMoe.CollectionStarting(aCoverage, 0.0);

   // Manually set some intervals on coverage
   wsf::coverage::FreeAsset fa{"test", "sensor"};

   auto someCoverage = aGrid.GetAsset(2);
   aCoverage.InjectInterval(someCoverage, fa, 0.0, 86400.0);       // a whole day
   aCoverage.InjectInterval(someCoverage, fa, 172800.0, 259200.0); // day 2 to day 3
   aCoverage.InjectInterval(someCoverage, fa, 129600, 302400.0);   // day 1.5 to day 3.5

   aMoe.CollectionCompleting(aCoverage, 345600.0); // completes after 4 days
}

TEST(WsfCoverageCoverageTime, Total)
{
   auto&                       testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                    grid{testDataService.GetScenario()};
   TestCoverage                cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::CoverageTime moe{testDataService.GetScenario()};
   SetupIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype total\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cCOVERAGE_TIME_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 56.0, cCOVERAGE_TIME_TOLERANCE);
}

TEST(WsfCoverageCoverageTime, Percent)
{
   auto&                       testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                    grid{testDataService.GetScenario()};
   TestCoverage                cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::CoverageTime moe{testDataService.GetScenario()};
   SetupIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype percent\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cCOVERAGE_TIME_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 56.0, cCOVERAGE_TIME_TOLERANCE);
}

TEST(WsfCoverageCoverageTime, TotalTimeAbove)
{
   auto&                       testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                    grid{testDataService.GetScenario()};
   TestCoverage                cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::CoverageTime moe{testDataService.GetScenario()};
   SetupIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype total_time_above 2\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cCOVERAGE_TIME_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 16.0, cCOVERAGE_TIME_TOLERANCE);
}

TEST(WsfCoverageCoverageTime, PercentTimeAbove)
{
   auto&                       testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                    grid{testDataService.GetScenario()};
   TestCoverage                cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::CoverageTime moe{testDataService.GetScenario()};
   SetupIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype percent_time_above 2\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cCOVERAGE_TIME_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 16.0, cCOVERAGE_TIME_TOLERANCE);
}

TEST(WsfCoverageCoverageTime, PerDayMax)
{
   auto&                       testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                    grid{testDataService.GetScenario()};
   TestCoverage                cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::CoverageTime moe{testDataService.GetScenario()};
   SetupLongIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype maximum_per 1 day\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cCOVERAGE_TIME_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 86400.0, cCOVERAGE_TIME_TOLERANCE);
}

TEST(WsfCoverageCoverageTime, PerSecondsMax)
{
   auto&                       testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                    grid{testDataService.GetScenario()};
   TestCoverage                cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::CoverageTime moe{testDataService.GetScenario()};
   SetupIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype maximum_per 30 seconds\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cCOVERAGE_TIME_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 22.0, cCOVERAGE_TIME_TOLERANCE);
}

TEST(WsfCoverageCoverageTime, PerDayMaxPercent)
{
   auto&                       testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                    grid{testDataService.GetScenario()};
   TestCoverage                cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::CoverageTime moe{testDataService.GetScenario()};
   SetupLongIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype maximum_percent_per 1 day\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cCOVERAGE_TIME_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 100.0, cCOVERAGE_TIME_TOLERANCE);
}

TEST(WsfCoverageCoverageTime, PerSecondsMaxPercent)
{
   auto&                       testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                    grid{testDataService.GetScenario()};
   TestCoverage                cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::CoverageTime moe{testDataService.GetScenario()};
   SetupIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype maximum_percent_per 30 seconds\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cCOVERAGE_TIME_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 100.0 * 22.0 / 30.0, cCOVERAGE_TIME_TOLERANCE);
}

TEST(WsfCoverageCoverageTime, PerDayMin)
{
   auto&                       testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                    grid{testDataService.GetScenario()};
   TestCoverage                cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::CoverageTime moe{testDataService.GetScenario()};
   SetupLongIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype minimum_per 1 day\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cCOVERAGE_TIME_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 0.0, cCOVERAGE_TIME_TOLERANCE);
}

TEST(WsfCoverageCoverageTime, PerSecondsMin)
{
   auto&                       testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                    grid{testDataService.GetScenario()};
   TestCoverage                cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::CoverageTime moe{testDataService.GetScenario()};
   SetupIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype minimum_per 30 seconds\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cCOVERAGE_TIME_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 0.0, cCOVERAGE_TIME_TOLERANCE);
}

TEST(WsfCoverageCoverageTime, PerDayMinPercent)
{
   auto&                       testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                    grid{testDataService.GetScenario()};
   TestCoverage                cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::CoverageTime moe{testDataService.GetScenario()};
   SetupLongIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype minimum_percent_per 1 day\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cCOVERAGE_TIME_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 0.0, cCOVERAGE_TIME_TOLERANCE);
}

TEST(WsfCoverageCoverageTime, PerSecondsMinPercent)
{
   auto&                       testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                    grid{testDataService.GetScenario()};
   TestCoverage                cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::CoverageTime moe{testDataService.GetScenario()};
   SetupIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype minimum_percent_per 30 seconds\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cCOVERAGE_TIME_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 0.0, cCOVERAGE_TIME_TOLERANCE);
}

TEST(WsfCoverageCoverageTime, PerDayMean)
{
   auto&                       testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                    grid{testDataService.GetScenario()};
   TestCoverage                cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::CoverageTime moe{testDataService.GetScenario()};
   SetupLongIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype mean_per 1 day\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cCOVERAGE_TIME_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 64800.0, cCOVERAGE_TIME_TOLERANCE);
}

TEST(WsfCoverageCoverageTime, PerSecondsMean)
{
   auto&                       testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                    grid{testDataService.GetScenario()};
   TestCoverage                cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::CoverageTime moe{testDataService.GetScenario()};
   SetupIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype mean_per 30 seconds\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cCOVERAGE_TIME_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 56.0 / (10.0 / 3.0), cCOVERAGE_TIME_TOLERANCE);
}

TEST(WsfCoverageCoverageTime, PerDayMeanPercent)
{
   auto&                       testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                    grid{testDataService.GetScenario()};
   TestCoverage                cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::CoverageTime moe{testDataService.GetScenario()};
   SetupLongIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype mean_percent_per 1 day\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cCOVERAGE_TIME_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 18.75, cCOVERAGE_TIME_TOLERANCE);
}

TEST(WsfCoverageCoverageTime, PerSecondsMeanPercent)
{
   auto&                       testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                    grid{testDataService.GetScenario()};
   TestCoverage                cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::CoverageTime moe{testDataService.GetScenario()};
   SetupIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype mean_percent_per 30 seconds\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cCOVERAGE_TIME_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 56.0 / (10.0 / 3.0), cCOVERAGE_TIME_TOLERANCE);
}

TEST(WsfCoverageCoverageTime, PerDaystandard_deviation)
{
   auto&                       testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                    grid{testDataService.GetScenario()};
   TestCoverage                cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::CoverageTime moe{testDataService.GetScenario()};
   SetupLongIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype standard_deviation_per 1 day\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cCOVERAGE_TIME_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 32327.919821726853, cCOVERAGE_TIME_TOLERANCE);
}

TEST(WsfCoverageCoverageTime, PerSecondsstandard_deviation)
{
   auto&                       testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                    grid{testDataService.GetScenario()};
   TestCoverage                cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::CoverageTime moe{testDataService.GetScenario()};
   SetupIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype standard_deviation_per 30 seconds\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cCOVERAGE_TIME_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 8.602325267042627, cCOVERAGE_TIME_TOLERANCE);
}

TEST(WsfCoverageCoverageTime, PerDaystandard_deviationPercent)
{
   auto&                       testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                    grid{testDataService.GetScenario()};
   TestCoverage                cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::CoverageTime moe{testDataService.GetScenario()};
   SetupLongIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype standard_deviation_percent_per 1 day\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cCOVERAGE_TIME_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 37.41657386773941, cCOVERAGE_TIME_TOLERANCE);
}

TEST(WsfCoverageCoverageTime, PerSecondsstandard_deviationPercent)
{
   auto&                       testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                    grid{testDataService.GetScenario()};
   TestCoverage                cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::CoverageTime moe{testDataService.GetScenario()};
   SetupIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype standard_deviation_percent_per 30 seconds\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cCOVERAGE_TIME_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 28.67441755680875, cCOVERAGE_TIME_TOLERANCE);
}
