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
#include "WsfCoverageRevisitTime.hpp"

namespace
{

constexpr double cVISIT_TIME_TOLERANCE{1.0e-9};

}

TEST(WsfCoverageRevisitTime, BadInput)
{
   auto&                      testDataService = ut::Locator<CoverageTestService>::get();
   wsf::coverage::RevisitTime moe{testDataService.GetScenario()};

   TestBadValueThrow(moe, "subtype unknown");
   TestEndOfDataThrow(moe, "subtype number_percent_below");
   TestBadValueThrow(moe, "subtype number_percent_below -10.0");
   TestBadValueThrow(moe, "subtype number_percent_below 110.0");
   TestEndOfDataThrow(moe, "subtype percent_below");
   TestBadValueThrow(moe, "subtype percent_below -10.0");
   TestBadValueThrow(moe, "subtype percent_below 110.0");
   TestEndOfDataThrow(moe, "subtype percent_below_gaps_only");
   TestBadValueThrow(moe, "subtype percent_below_gaps_only -10.0");
   TestBadValueThrow(moe, "subtype percent_below_gaps_only 110.0");
   TestFalseReturn(moe, "unknown");
}

TEST(WsfCoverageRevisitTime, ProcessInputSubTypes)
{
   std::stringstream inputStream{};
   inputStream << "subtype mean\n";
   inputStream << "subtype maximum\n";
   inputStream << "subtype minimum\n";
   inputStream << "subtype standard_deviation\n";
   inputStream << "subtype number_percent_below 50.0\n";
   inputStream << "subtype percent_below 60.0\n";
   inputStream << "subtype percent_below_gaps_only 70.0\n";
   UtInput input{};
   input.PushInputString(inputStream.str());

   auto&                      testDataService = ut::Locator<CoverageTestService>::get();
   wsf::coverage::RevisitTime moe{testDataService.GetScenario()};

   std::string command;
   input.ReadCommand(command);
   EXPECT_TRUE(moe.ProcessInput(input));
   EXPECT_EQ(moe.GetSubType(), wsf::coverage::RevisitTime::SubType::cMEAN);

   input.ReadCommand(command);
   EXPECT_TRUE(moe.ProcessInput(input));
   EXPECT_EQ(moe.GetSubType(), wsf::coverage::RevisitTime::SubType::cMAXIMUM);

   input.ReadCommand(command);
   EXPECT_TRUE(moe.ProcessInput(input));
   EXPECT_EQ(moe.GetSubType(), wsf::coverage::RevisitTime::SubType::cMINIMUM);

   input.ReadCommand(command);
   EXPECT_TRUE(moe.ProcessInput(input));
   EXPECT_EQ(moe.GetSubType(), wsf::coverage::RevisitTime::SubType::cSTD_DEV);

   input.ReadCommand(command);
   EXPECT_TRUE(moe.ProcessInput(input));
   EXPECT_EQ(moe.GetSubType(), wsf::coverage::RevisitTime::SubType::cNUM_PERCENT_BELOW);
   EXPECT_NEAR(moe.GetParameter(), 50.0, cVISIT_TIME_TOLERANCE);

   input.ReadCommand(command);
   EXPECT_TRUE(moe.ProcessInput(input));
   EXPECT_EQ(moe.GetSubType(), wsf::coverage::RevisitTime::SubType::cPERCENT_BELOW);
   EXPECT_NEAR(moe.GetParameter(), 60.0, cVISIT_TIME_TOLERANCE);

   input.ReadCommand(command);
   EXPECT_TRUE(moe.ProcessInput(input));
   EXPECT_EQ(moe.GetSubType(), wsf::coverage::RevisitTime::SubType::cPERCENT_BELOW_GAPS_ONLY);
   EXPECT_NEAR(moe.GetParameter(), 70.0, cVISIT_TIME_TOLERANCE);
}

void SetupIntervals(TestGrid&                   aGrid,
                    TestCoverage&               aCoverage,
                    wsf::coverage::RevisitTime& aMoe,
                    const WsfScenario&          aScenario,
                    const std::string&          aMoeInput)
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

TEST(WsfCoverageRevisitTime, Minimum)
{
   auto&                      testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                   grid{testDataService.GetScenario()};
   TestCoverage               cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::RevisitTime moe{testDataService.GetScenario()};
   SetupIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype minimum\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cVISIT_TIME_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 1.0, cVISIT_TIME_TOLERANCE);
}

TEST(WsfCoverageRevisitTime, Maximum)
{
   auto&                      testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                   grid{testDataService.GetScenario()};
   TestCoverage               cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::RevisitTime moe{testDataService.GetScenario()};
   SetupIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype maximum\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cVISIT_TIME_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 9.0, cVISIT_TIME_TOLERANCE);
}

TEST(WsfCoverageRevisitTime, Mean)
{
   auto&                      testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                   grid{testDataService.GetScenario()};
   TestCoverage               cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::RevisitTime moe{testDataService.GetScenario()};
   SetupIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype mean\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cVISIT_TIME_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 5.0, cVISIT_TIME_TOLERANCE);
}

TEST(WsfCoverageRevisitTime, StdDev)
{
   auto&                      testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                   grid{testDataService.GetScenario()};
   TestCoverage               cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::RevisitTime moe{testDataService.GetScenario()};
   SetupIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype standard_deviation\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cVISIT_TIME_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 2.5819888974, cVISIT_TIME_TOLERANCE);
}

TEST(WsfCoverageRevisitTime, NumPercentBelow)
{
   auto&                      testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                   grid{testDataService.GetScenario()};
   TestCoverage               cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::RevisitTime moe{testDataService.GetScenario()};
   SetupIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype number_percent_below 56.0\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cVISIT_TIME_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 6.0, cVISIT_TIME_TOLERANCE);
}

TEST(WsfCoverageRevisitTime, PercentBelow)
{
   auto&                      testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                   grid{testDataService.GetScenario()};
   TestCoverage               cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::RevisitTime moe{testDataService.GetScenario()};
   SetupIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype percent_below 80.0\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cVISIT_TIME_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 6.0, cVISIT_TIME_TOLERANCE);
}

TEST(WsfCoverageRevisitTime, PercentBelowGapsOnly)
{
   auto&                      testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                   grid{testDataService.GetScenario()};
   TestCoverage               cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::RevisitTime moe{testDataService.GetScenario()};
   SetupIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype percent_below_gaps_only 40.0\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cVISIT_TIME_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 8.0, cVISIT_TIME_TOLERANCE);
}
