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
#include "WsfCoverageAccessDuration.hpp"

namespace
{

constexpr double cACCESS_DURATION_TOLERANCE{1.0e-9};

}

TEST(WsfCoverageAccessDuration, BadInput)
{
   auto&                         testDataService = ut::Locator<CoverageTestService>::get();
   wsf::coverage::AccessDuration moe{testDataService.GetScenario()};

   TestBadValueThrow(moe, "subtype unknown");
   TestEndOfDataThrow(moe, "subtype percent_above");
   TestBadValueThrow(moe, "subtype percent_above -10.0");
   TestBadValueThrow(moe, "subtype percent_above 110.0");
   TestFalseReturn(moe, "unknown");
}

TEST(WsfCoverageAccessDuration, ProcessInputSubTypes)
{
   std::stringstream inputStream{};
   inputStream << "subtype mean\n";
   inputStream << "subtype maximum\n";
   inputStream << "subtype minimum\n";
   inputStream << "subtype standard_deviation\n";
   inputStream << "subtype percent_above 50.0\n";
   inputStream << "subtype sum\n";
   UtInput input{};
   input.PushInputString(inputStream.str());

   auto&                         testDataService = ut::Locator<CoverageTestService>::get();
   wsf::coverage::AccessDuration moe{testDataService.GetScenario()};

   std::string command;
   input.ReadCommand(command);
   EXPECT_TRUE(moe.ProcessInput(input));
   EXPECT_EQ(moe.GetSubType(), wsf::coverage::AccessDuration::SubType::cMEAN);

   input.ReadCommand(command);
   EXPECT_TRUE(moe.ProcessInput(input));
   EXPECT_EQ(moe.GetSubType(), wsf::coverage::AccessDuration::SubType::cMAX);

   input.ReadCommand(command);
   EXPECT_TRUE(moe.ProcessInput(input));
   EXPECT_EQ(moe.GetSubType(), wsf::coverage::AccessDuration::SubType::cMIN);

   input.ReadCommand(command);
   EXPECT_TRUE(moe.ProcessInput(input));
   EXPECT_EQ(moe.GetSubType(), wsf::coverage::AccessDuration::SubType::cSTDDEV);

   input.ReadCommand(command);
   EXPECT_TRUE(moe.ProcessInput(input));
   EXPECT_EQ(moe.GetSubType(), wsf::coverage::AccessDuration::SubType::cPERCENT_ABOVE);
   EXPECT_NEAR(moe.GetParameter(), 50.0, cACCESS_DURATION_TOLERANCE);

   input.ReadCommand(command);
   EXPECT_TRUE(moe.ProcessInput(input));
   EXPECT_EQ(moe.GetSubType(), wsf::coverage::AccessDuration::SubType::cSUM);
}

void SetupIntervals(TestGrid&                      aGrid,
                    TestCoverage&                  aCoverage,
                    wsf::coverage::AccessDuration& aMoe,
                    const WsfScenario&             aScenario,
                    const std::string&             aMoeInput)
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

   auto someGaps = aGrid.GetAsset(2);
   // Lengths:
   // 50, 40, 30, 20, 10
   // Total coverage:
   // 90
   aCoverage.InjectInterval(someGaps, fa, 0.0, 50.0);
   aCoverage.InjectInterval(someGaps, fa, 20.0, 60.0);
   aCoverage.InjectInterval(someGaps, fa, 40.0, 70.0);
   aCoverage.InjectInterval(someGaps, fa, 60.0, 80.0);
   aCoverage.InjectInterval(someGaps, fa, 80.0, 90.0);

   aMoe.CollectionCompleting(aCoverage, 100.0);
}

TEST(WsfCoverageAccessDuration, minimumimum)
{
   auto&                         testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                      grid{testDataService.GetScenario()};
   TestCoverage                  cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::AccessDuration moe{testDataService.GetScenario()};
   SetupIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype minimum\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cACCESS_DURATION_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 10.0, cACCESS_DURATION_TOLERANCE);
}

TEST(WsfCoverageAccessDuration, maximumimum)
{
   auto&                         testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                      grid{testDataService.GetScenario()};
   TestCoverage                  cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::AccessDuration moe{testDataService.GetScenario()};
   SetupIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype maximum\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cACCESS_DURATION_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 50.0, cACCESS_DURATION_TOLERANCE);
}

TEST(WsfCoverageAccessDuration, Mean)
{
   auto&                         testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                      grid{testDataService.GetScenario()};
   TestCoverage                  cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::AccessDuration moe{testDataService.GetScenario()};
   SetupIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype mean\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cACCESS_DURATION_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 30.0, cACCESS_DURATION_TOLERANCE);
}

TEST(WsfCoverageAccessDuration, standard_deviation)
{
   auto&                         testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                      grid{testDataService.GetScenario()};
   TestCoverage                  cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::AccessDuration moe{testDataService.GetScenario()};
   SetupIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype standard_deviation\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cACCESS_DURATION_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 14.142135623730951, cACCESS_DURATION_TOLERANCE);
}

TEST(WsfCoverageAccessDuration, PercentAbove)
{
   auto&                         testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                      grid{testDataService.GetScenario()};
   TestCoverage                  cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::AccessDuration moe{testDataService.GetScenario()};
   SetupIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype percent_above 80.0\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cACCESS_DURATION_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 20.0, cACCESS_DURATION_TOLERANCE);
}

TEST(WsfCoverageAccessDuration, PercentBelowGapsOnly)
{
   auto&                         testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                      grid{testDataService.GetScenario()};
   TestCoverage                  cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::AccessDuration moe{testDataService.GetScenario()};
   SetupIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype sum\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cACCESS_DURATION_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 150.0, cACCESS_DURATION_TOLERANCE);
}
