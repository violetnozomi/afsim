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
#include "WsfCoverageNumberOfGaps.hpp"

namespace
{

constexpr double cNUMBER_OF_GAPS_TOLERANCE{1.0e-9};

}

TEST(WsfCoverageNumberOfGaps, BadInput)
{
   auto&                       testDataService = ut::Locator<CoverageTestService>::get();
   wsf::coverage::NumberOfGaps moe{testDataService.GetScenario()};

   TestBadValueThrow(moe, "subtype unknown");
   TestEndOfDataThrow(moe, "subtype in_span");
   TestEndOfDataThrow(moe, "subtype in_span 10.0 s");
   TestBadValueThrow(moe, "subtype in_span -10.0 s 100.0 s");
   TestBadValueThrow(moe, "subtype in_span 100.0 s 10.0 s");
   TestEndOfDataThrow(moe, "subtype in_span_per");
   TestEndOfDataThrow(moe, "subtype in_span_per 1 day");
   TestEndOfDataThrow(moe, "subtype in_span_per 10.0 s 1 day");
   TestBadValueThrow(moe, "subtype in_span_per -10.0 s 100.0 s 1 day");
   TestBadValueThrow(moe, "subtype in_span_per 100.0 s 10.0 s 1 day");
   TestEndOfDataThrow(moe, "subtype maximum_per");
   TestBadValueThrow(moe, "subtype maximum_per -1 day");
   TestBadValueThrow(moe, "subtype maximum_per 5 hours");
   TestBadValueThrow(moe, "subtype maximum_per 2 days");
   TestEndOfDataThrow(moe, "subtype minimum_per");
   TestBadValueThrow(moe, "subtype minimum_per -1 day");
   TestBadValueThrow(moe, "subtype minimum_per 5 hours");
   TestBadValueThrow(moe, "subtype maximum_per 2 days");
   TestEndOfDataThrow(moe, "subtype mean_per");
   TestBadValueThrow(moe, "subtype mean_per -1 day");
   TestFalseReturn(moe, "unknown");
}

TEST(WsfCoverageNumberOfGaps, ProcessInputSubTypes)
{
   std::stringstream inputStream{};
   inputStream << "subtype total\n";
   inputStream << "subtype in_span 10.0 s 100.0 s\n";
   inputStream << "subtype maximum_per 1 day\n";
   inputStream << "subtype minimum_per 1 day\n";
   inputStream << "subtype mean_per 1 day\n";
   inputStream << "subtype in_span_per 10.0 s 100.0 s 1 day\n";
   UtInput input{};
   input.PushInputString(inputStream.str());

   auto&                       testDataService = ut::Locator<CoverageTestService>::get();
   wsf::coverage::NumberOfGaps moe{testDataService.GetScenario()};

   std::string command;
   input.ReadCommand(command);
   EXPECT_TRUE(moe.ProcessInput(input));
   EXPECT_EQ(moe.GetSubType(), wsf::coverage::NumberOfGaps::SubType::cTOTAL);

   input.ReadCommand(command);
   EXPECT_TRUE(moe.ProcessInput(input));
   EXPECT_EQ(moe.GetSubType(), wsf::coverage::NumberOfGaps::SubType::cIN_SPAN);
   EXPECT_NEAR(moe.GetMinParameter(), 10.0, cNUMBER_OF_GAPS_TOLERANCE);
   EXPECT_NEAR(moe.GetMaxParameter(), 100.0, cNUMBER_OF_GAPS_TOLERANCE);

   input.ReadCommand(command);
   EXPECT_TRUE(moe.ProcessInput(input));
   EXPECT_EQ(moe.GetSubType(), wsf::coverage::NumberOfGaps::SubType::cMAXIMUM_PER_TIME);
   EXPECT_NEAR(moe.GetSampleSpan(), 3600.0 * 24.0, cNUMBER_OF_GAPS_TOLERANCE);

   input.ReadCommand(command);
   EXPECT_TRUE(moe.ProcessInput(input));
   EXPECT_EQ(moe.GetSubType(), wsf::coverage::NumberOfGaps::SubType::cMINIMUM_PER_TIME);
   EXPECT_NEAR(moe.GetSampleSpan(), 3600.0 * 24.0, cNUMBER_OF_GAPS_TOLERANCE);

   input.ReadCommand(command);
   EXPECT_TRUE(moe.ProcessInput(input));
   EXPECT_EQ(moe.GetSubType(), wsf::coverage::NumberOfGaps::SubType::cMEAN_PER_TIME);
   EXPECT_NEAR(moe.GetSampleSpan(), 3600.0 * 24.0, cNUMBER_OF_GAPS_TOLERANCE);

   input.ReadCommand(command);
   EXPECT_TRUE(moe.ProcessInput(input));
   EXPECT_EQ(moe.GetSubType(), wsf::coverage::NumberOfGaps::SubType::cIN_SPAN_PER_TIME);
   EXPECT_NEAR(moe.GetMinParameter(), 10.0, cNUMBER_OF_GAPS_TOLERANCE);
   EXPECT_NEAR(moe.GetMaxParameter(), 100.0, cNUMBER_OF_GAPS_TOLERANCE);
   EXPECT_NEAR(moe.GetSampleSpan(), 3600.0 * 24.0, cNUMBER_OF_GAPS_TOLERANCE);
}

void SetupIntervals(TestGrid&                    aGrid,
                    TestCoverage&                aCoverage,
                    wsf::coverage::NumberOfGaps& aMoe,
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

   auto noGaps = aGrid.GetAsset(1);
   aCoverage.InjectInterval(noGaps, fa, 0.0, 100.0);

   // These intervals are chosen so as to produce gaps of
   // duration (in order), 3, 6, 4, 8, 1, 2, 7, 9, 5.
   // Sorted these are: 1, 2, 3, 4, 5, 6, 7, 8, 9, for which
   // it is easy to compute expected values.
   auto someGaps = aGrid.GetAsset(2);
   aCoverage.InjectInterval(someGaps, fa, 3.0, 6.0);
   aCoverage.InjectInterval(someGaps, fa, 12.0, 13.0);
   aCoverage.InjectInterval(someGaps, fa, 17.0, 27.0);
   aCoverage.InjectInterval(someGaps, fa, 35.0, 40.0);
   aCoverage.InjectInterval(someGaps, fa, 41.0, 55.0);
   aCoverage.InjectInterval(someGaps, fa, 57.0, 63.0);
   aCoverage.InjectInterval(someGaps, fa, 70.0, 81.0);
   aCoverage.InjectInterval(someGaps, fa, 90.0, 95.0);

   aMoe.CollectionCompleting(aCoverage, 100.0);
}

// This is the same concept as SetupIntervals, except that these are counted in days.
void SetupLongIntervals(TestGrid&                    aGrid,
                        TestCoverage&                aCoverage,
                        wsf::coverage::NumberOfGaps& aMoe,
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
   wsf::coverage::FreeAsset fa1{"test", "sensor"};
   wsf::coverage::FreeAsset fa2{"test", "sensor"};

   auto noGaps = aGrid.GetAsset(1);
   aCoverage.InjectInterval(noGaps, fa1, 0.0, 345600.0);

   auto someCoverage = aGrid.GetAsset(2);
   aCoverage.InjectInterval(someCoverage, fa1, 0.0, 86400.0);       // day 0.5 to day 1.5
   aCoverage.InjectInterval(someCoverage, fa1, 95040.0, 112320.0);  // day 1.6 to day 1.8
   aCoverage.InjectInterval(someCoverage, fa2, 103680.0, 129600.0); // day 1.7 to day 2.0
   aCoverage.InjectInterval(someCoverage, fa1, 138240.0, 146880.0); // day 2.1 to day 2.2
   aCoverage.InjectInterval(someCoverage, fa1, 155520.0, 181440.0); // day 2.3 to day 2.6
   aCoverage.InjectInterval(someCoverage, fa2, 172800.0, 216000.0); // day 2.5 to day 3.0
   aCoverage.InjectInterval(someCoverage, fa1, 233280.0, 259200.0); // day 3.2 to day 3.5
   aCoverage.InjectInterval(someCoverage, fa1, 267840.0, 276480.0); // day 3.6 to day 3.7
   aCoverage.InjectInterval(someCoverage, fa1, 285120.0, 293760.0); // day 3.8 to day 3.9
   aCoverage.InjectInterval(someCoverage, fa1, 302400.0, 311040.0); // day 4.0 to day 4.1
   aCoverage.InjectInterval(someCoverage, fa1, 328320.0, 345600.0); // day 4.3 to day 4.5

   aMoe.CollectionCompleting(aCoverage, 345600.0); // completes after 4 days
}

TEST(WsfCoverageNumberOfGaps, Total)
{
   auto&                       testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                    grid{testDataService.GetScenario()};
   TestCoverage                cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::NumberOfGaps moe{testDataService.GetScenario()};
   SetupIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype total\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cNUMBER_OF_GAPS_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 9.0, cNUMBER_OF_GAPS_TOLERANCE);
}

TEST(WsfCoverageNumberOfGaps, InSpan)
{
   auto&                       testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                    grid{testDataService.GetScenario()};
   TestCoverage                cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::NumberOfGaps moe{testDataService.GetScenario()};
   SetupIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype in_span 3.0 s 7.0 s\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cNUMBER_OF_GAPS_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 5.0, cNUMBER_OF_GAPS_TOLERANCE);
}

TEST(WsfCoverageNumberOfGaps, PerDayMax)
{
   auto&                       testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                    grid{testDataService.GetScenario()};
   TestCoverage                cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::NumberOfGaps moe{testDataService.GetScenario()};
   SetupLongIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype maximum_per 1 day\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cNUMBER_OF_GAPS_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 4.0, cNUMBER_OF_GAPS_TOLERANCE);
}

TEST(WsfCoverageNumberOfGaps, PerDayMin)
{
   auto&                       testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                    grid{testDataService.GetScenario()};
   TestCoverage                cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::NumberOfGaps moe{testDataService.GetScenario()};
   SetupLongIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype minimum_per 1 day\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cNUMBER_OF_GAPS_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 0.0, cNUMBER_OF_GAPS_TOLERANCE);
}

TEST(WsfCoverageNumberOfGaps, PerDayMax_Short)
{
   auto&                       testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                    grid{testDataService.GetScenario()};
   TestCoverage                cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::NumberOfGaps moe{testDataService.GetScenario()};
   SetupIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype maximum_per 1 day\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cNUMBER_OF_GAPS_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 9.0, cNUMBER_OF_GAPS_TOLERANCE);
}

TEST(WsfCoverageNumberOfGaps, PerDayMin_Short)
{
   auto&                       testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                    grid{testDataService.GetScenario()};
   TestCoverage                cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::NumberOfGaps moe{testDataService.GetScenario()};
   SetupIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype minimum_per 1 day\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cNUMBER_OF_GAPS_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 9.0, cNUMBER_OF_GAPS_TOLERANCE);
}

TEST(WsfCoverageNumberOfGaps, PerSecondsMax)
{
   auto&                       testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                    grid{testDataService.GetScenario()};
   TestCoverage                cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::NumberOfGaps moe{testDataService.GetScenario()};
   SetupIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype maximum_per 30 seconds\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cNUMBER_OF_GAPS_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 4.0, cNUMBER_OF_GAPS_TOLERANCE);
}

TEST(WsfCoverageNumberOfGaps, PerSecondsMin)
{
   auto&                       testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                    grid{testDataService.GetScenario()};
   TestCoverage                cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::NumberOfGaps moe{testDataService.GetScenario()};
   SetupIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype minimum_per 30 seconds\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cNUMBER_OF_GAPS_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 1.0, cNUMBER_OF_GAPS_TOLERANCE);
}

TEST(WsfCoverageNumberOfGaps, PerDayMean)
{
   auto&                       testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                    grid{testDataService.GetScenario()};
   TestCoverage                cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::NumberOfGaps moe{testDataService.GetScenario()};
   SetupLongIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype mean_per 1 day\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cNUMBER_OF_GAPS_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 2.0, cNUMBER_OF_GAPS_TOLERANCE);
}

TEST(WsfCoverageNumberOfGaps, PerDayInSpan)
{
   auto&                       testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                    grid{testDataService.GetScenario()};
   TestCoverage                cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::NumberOfGaps moe{testDataService.GetScenario()};
   SetupLongIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype in_span_per 17279.0 s 25920.0 s 1 day\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cNUMBER_OF_GAPS_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 0.5, cNUMBER_OF_GAPS_TOLERANCE);
}
