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
#include "WsfCoverageNumberOfAccesses.hpp"

namespace
{

constexpr double cNUMBER_OF_ACCESSES_TOLERANCE{1.0e-9};

}

TEST(WsfCoverageNumberOfAccesses, BadInput)
{
   auto&                           testDataService = ut::Locator<CoverageTestService>::get();
   wsf::coverage::NumberOfAccesses moe{testDataService.GetScenario()};

   TestBadValueThrow(moe, "subtype unknown");
   TestEndOfDataThrow(moe, "subtype in_span");
   TestEndOfDataThrow(moe, "subtype in_span 10.0");
   TestBadValueThrow(moe, "subtype in_span -10.0 10.0");
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

TEST(WsfCoverageNumberOfAccesses, ProcessInputSubTypes)
{
   std::stringstream inputStream{};
   inputStream << "subtype in_span 10 s 20 s\n";
   inputStream << "subtype in_span_per 10 s 20 s 1 hour\n";
   inputStream << "subtype maximum_per 1 hour\n";
   inputStream << "subtype mean_per 1 hour\n";
   inputStream << "subtype minimum_per 1 hour\n";
   inputStream << "subtype total\n";
   UtInput input{};
   input.PushInputString(inputStream.str());

   auto&                           testDataService = ut::Locator<CoverageTestService>::get();
   wsf::coverage::NumberOfAccesses moe{testDataService.GetScenario()};

   std::string command;
   input.ReadCommand(command);
   EXPECT_TRUE(moe.ProcessInput(input));
   EXPECT_EQ(moe.GetSubType(), wsf::coverage::NumberOfAccesses::SubType::cIN_SPAN);
   EXPECT_NEAR(moe.GetMinParameter(), 10.0, cNUMBER_OF_ACCESSES_TOLERANCE);
   EXPECT_NEAR(moe.GetMaxParameter(), 20.0, cNUMBER_OF_ACCESSES_TOLERANCE);

   input.ReadCommand(command);
   EXPECT_TRUE(moe.ProcessInput(input));
   EXPECT_EQ(moe.GetSubType(), wsf::coverage::NumberOfAccesses::SubType::cIN_SPAN_PER_TIME);
   EXPECT_NEAR(moe.GetMinParameter(), 10.0, cNUMBER_OF_ACCESSES_TOLERANCE);
   EXPECT_NEAR(moe.GetMaxParameter(), 20.0, cNUMBER_OF_ACCESSES_TOLERANCE);
   EXPECT_NEAR(moe.GetSampleSpan(), 3600.0, cNUMBER_OF_ACCESSES_TOLERANCE);

   input.ReadCommand(command);
   EXPECT_TRUE(moe.ProcessInput(input));
   EXPECT_EQ(moe.GetSubType(), wsf::coverage::NumberOfAccesses::SubType::cMAX_PER_TIME);
   EXPECT_NEAR(moe.GetSampleSpan(), 3600.0, cNUMBER_OF_ACCESSES_TOLERANCE);

   input.ReadCommand(command);
   EXPECT_TRUE(moe.ProcessInput(input));
   EXPECT_EQ(moe.GetSubType(), wsf::coverage::NumberOfAccesses::SubType::cMEAN_PER_TIME);
   EXPECT_NEAR(moe.GetSampleSpan(), 3600.0, cNUMBER_OF_ACCESSES_TOLERANCE);

   input.ReadCommand(command);
   EXPECT_TRUE(moe.ProcessInput(input));
   EXPECT_EQ(moe.GetSubType(), wsf::coverage::NumberOfAccesses::SubType::cMIN_PER_TIME);
   EXPECT_NEAR(moe.GetSampleSpan(), 3600.0, cNUMBER_OF_ACCESSES_TOLERANCE);

   input.ReadCommand(command);
   EXPECT_TRUE(moe.ProcessInput(input));
   EXPECT_EQ(moe.GetSubType(), wsf::coverage::NumberOfAccesses::SubType::cTOTAL);
}

void SetupIntervals(TestGrid&                        aGrid,
                    TestCoverage&                    aCoverage,
                    wsf::coverage::NumberOfAccesses& aMoe,
                    const WsfScenario&               aScenario,
                    const std::string&               aMoeInput)
{
   aGrid.SetName("test_grid");
   aGrid.Initialize();
   {
      UtInput input{};
      input.PushInputString("start_time 0.0 s\nend_time 3.0 days\n");
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

   auto             someIntervals = aGrid.GetAsset(2);
   constexpr double threeDays{259200.0};
   constexpr double hour{3600.0};
   for (double i = 0.0; i < threeDays; i += hour * 6.0)
   {
      aCoverage.InjectInterval(someIntervals, fa, i, i + hour * 3.0);
      aCoverage.InjectInterval(someIntervals, fa, i + hour * 4.0, i + hour * 5.0);
   }

   aMoe.CollectionCompleting(aCoverage, threeDays);
}

TEST(WsfCoverageNumberOfAccesses, InSpan)
{
   auto&                           testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                        grid{testDataService.GetScenario()};
   TestCoverage                    cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::NumberOfAccesses moe{testDataService.GetScenario()};
   SetupIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype in_span 2 hours 4 hours\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cNUMBER_OF_ACCESSES_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 12.0, cNUMBER_OF_ACCESSES_TOLERANCE);
}

TEST(WsfCoverageNumberOfAccesses, PerDayInSpan)
{
   auto&                           testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                        grid{testDataService.GetScenario()};
   TestCoverage                    cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::NumberOfAccesses moe{testDataService.GetScenario()};
   SetupIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype in_span_per 2 hours 4 hours 1 day\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cNUMBER_OF_ACCESSES_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 4.0, cNUMBER_OF_ACCESSES_TOLERANCE);
}

TEST(WsfCoverageNumberOfAccesses, PerHalfDayInSpan)
{
   auto&                           testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                        grid{testDataService.GetScenario()};
   TestCoverage                    cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::NumberOfAccesses moe{testDataService.GetScenario()};
   SetupIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype in_span_per 2 hours 4 hours 12 hours\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cNUMBER_OF_ACCESSES_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 2.0, cNUMBER_OF_ACCESSES_TOLERANCE);
}

TEST(WsfCoverageNumberOfAccesses, PerDayMax)
{
   auto&                           testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                        grid{testDataService.GetScenario()};
   TestCoverage                    cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::NumberOfAccesses moe{testDataService.GetScenario()};
   SetupIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype maximum_per 1 day\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cNUMBER_OF_ACCESSES_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 8.0, cNUMBER_OF_ACCESSES_TOLERANCE);
}

TEST(WsfCoverageNumberOfAccesses, PerHalfDayMax)
{
   auto&                           testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                        grid{testDataService.GetScenario()};
   TestCoverage                    cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::NumberOfAccesses moe{testDataService.GetScenario()};
   SetupIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype maximum_per 12 hours\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cNUMBER_OF_ACCESSES_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 4.0, cNUMBER_OF_ACCESSES_TOLERANCE);
}

TEST(WsfCoverageNumberOfAccesses, PerDayMin)
{
   auto&                           testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                        grid{testDataService.GetScenario()};
   TestCoverage                    cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::NumberOfAccesses moe{testDataService.GetScenario()};
   SetupIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype minimum_per 1 day\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cNUMBER_OF_ACCESSES_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 4.0, cNUMBER_OF_ACCESSES_TOLERANCE);
}

TEST(WsfCoverageNumberOfAccesses, PerHalfDayMin)
{
   auto&                           testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                        grid{testDataService.GetScenario()};
   TestCoverage                    cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::NumberOfAccesses moe{testDataService.GetScenario()};
   SetupIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype minimum_per 12 hours\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cNUMBER_OF_ACCESSES_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 4.0, cNUMBER_OF_ACCESSES_TOLERANCE);
}

TEST(WsfCoverageNumberOfAccesses, PerDayMean)
{
   auto&                           testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                        grid{testDataService.GetScenario()};
   TestCoverage                    cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::NumberOfAccesses moe{testDataService.GetScenario()};
   SetupIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype mean_per 1 day\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cNUMBER_OF_ACCESSES_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 8.0, cNUMBER_OF_ACCESSES_TOLERANCE);
}

TEST(WsfCoverageNumberOfAccesses, PerHalfDayMean)
{
   auto&                           testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                        grid{testDataService.GetScenario()};
   TestCoverage                    cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::NumberOfAccesses moe{testDataService.GetScenario()};
   SetupIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype mean_per 12 hours\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cNUMBER_OF_ACCESSES_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 4.0, cNUMBER_OF_ACCESSES_TOLERANCE);
}

TEST(WsfCoverageNumberOfAccesses, total)
{
   auto&                           testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                        grid{testDataService.GetScenario()};
   TestCoverage                    cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::NumberOfAccesses moe{testDataService.GetScenario()};
   SetupIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype total\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cNUMBER_OF_ACCESSES_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 24.0, cNUMBER_OF_ACCESSES_TOLERANCE);
}
