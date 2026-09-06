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
#include "WsfCoverageN_AssetCoverage.hpp"

namespace
{

constexpr double cASSET_COVERAGE_TOLERANCE{1.0e-9};

}

TEST(WsfCoverageN_AssetCoverage, BadInput)
{
   auto&                          testDataService = ut::Locator<CoverageTestService>::get();
   wsf::coverage::N_AssetCoverage moe{testDataService.GetScenario()};

   TestBadValueThrow(moe, "subtype unknown");
   TestEndOfDataThrow(moe, "subtype percent_above");
   TestBadValueThrow(moe, "subtype percent_above -10.0");
   TestBadValueThrow(moe, "subtype percent_above 110.0");
   TestFalseReturn(moe, "unknown");
}

TEST(WsfCoverageN_AssetCoverage, ProcessInputSubTypes)
{
   std::stringstream inputStream{};
   inputStream << "subtype mean\n";
   inputStream << "subtype maximum\n";
   inputStream << "subtype minimum\n";
   inputStream << "subtype percent_above 60.0\n";
   inputStream << "subtype unique\n";
   UtInput input{};
   input.PushInputString(inputStream.str());

   auto&                          testDataService = ut::Locator<CoverageTestService>::get();
   wsf::coverage::N_AssetCoverage moe{testDataService.GetScenario()};

   std::string command;
   input.ReadCommand(command);
   EXPECT_TRUE(moe.ProcessInput(input));
   EXPECT_EQ(moe.GetSubType(), wsf::coverage::N_AssetCoverage::SubType::cMEAN);

   input.ReadCommand(command);
   EXPECT_TRUE(moe.ProcessInput(input));
   EXPECT_EQ(moe.GetSubType(), wsf::coverage::N_AssetCoverage::SubType::cMAX);

   input.ReadCommand(command);
   EXPECT_TRUE(moe.ProcessInput(input));
   EXPECT_EQ(moe.GetSubType(), wsf::coverage::N_AssetCoverage::SubType::cMIN);

   input.ReadCommand(command);
   EXPECT_TRUE(moe.ProcessInput(input));
   EXPECT_EQ(moe.GetSubType(), wsf::coverage::N_AssetCoverage::SubType::cPERCENT_ABOVE);
   EXPECT_NEAR(moe.GetParameter(), 60.0, cASSET_COVERAGE_TOLERANCE);

   input.ReadCommand(command);
   EXPECT_TRUE(moe.ProcessInput(input));
   EXPECT_EQ(moe.GetSubType(), wsf::coverage::N_AssetCoverage::SubType::cUNIQUE);
}

void SetupIntervals(TestGrid&                       aGrid,
                    TestCoverage&                   aCoverage,
                    wsf::coverage::N_AssetCoverage& aMoe,
                    const WsfScenario&              aScenario,
                    const std::string&              aMoeInput)
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

   // Manually set some intervals on coverage         0123456789
   wsf::coverage::FreeAsset fa0{"test0", "sensor"}; // #####    #
   wsf::coverage::FreeAsset fa1{"test1", "sensor"}; //     ##
   wsf::coverage::FreeAsset fa2{"test2", "sensor"}; //   #####
   wsf::coverage::FreeAsset fa3{"test3", "sensor"}; //       ##
   wsf::coverage::FreeAsset fa4{"test4", "sensor"}; //         #

   auto someAccess = aGrid.GetAsset(2);
   aCoverage.InjectInterval(someAccess, fa0, 0.0, 50.0);
   aCoverage.InjectInterval(someAccess, fa1, 40.0, 60.0);
   aCoverage.InjectInterval(someAccess, fa2, 20.0, 70.0);
   aCoverage.InjectInterval(someAccess, fa3, 60.0, 80.0);
   aCoverage.InjectInterval(someAccess, fa4, 80.0, 90.0);
   aCoverage.InjectInterval(someAccess, fa0, 90.0, 100.0);

   aMoe.CollectionCompleting(aCoverage, 100.0);
}

TEST(WsfCoverageN_AssetCoverage, Minimum)
{
   auto&                          testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                       grid{testDataService.GetScenario()};
   TestCoverage                   cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::N_AssetCoverage moe{testDataService.GetScenario()};
   SetupIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype minimum\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cASSET_COVERAGE_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 1.0, cASSET_COVERAGE_TOLERANCE);
}

TEST(WsfCoverageN_AssetCoverage, Maximum)
{
   auto&                          testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                       grid{testDataService.GetScenario()};
   TestCoverage                   cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::N_AssetCoverage moe{testDataService.GetScenario()};
   SetupIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype maximum\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cASSET_COVERAGE_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 3.0, cASSET_COVERAGE_TOLERANCE);
}

TEST(WsfCoverageN_AssetCoverage, Mean)
{
   auto&                          testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                       grid{testDataService.GetScenario()};
   TestCoverage                   cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::N_AssetCoverage moe{testDataService.GetScenario()};
   SetupIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype mean\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cASSET_COVERAGE_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 1.6, cASSET_COVERAGE_TOLERANCE);
}

TEST(WsfCoverageN_AssetCoverage, PercentAbove)
{
   auto&                          testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                       grid{testDataService.GetScenario()};
   TestCoverage                   cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::N_AssetCoverage moe{testDataService.GetScenario()};
   SetupIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype percent_above 50.0\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cASSET_COVERAGE_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 2.0, cASSET_COVERAGE_TOLERANCE);
}

TEST(WsfCoverageN_AssetCoverage, Unique)
{
   auto&                          testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                       grid{testDataService.GetScenario()};
   TestCoverage                   cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::N_AssetCoverage moe{testDataService.GetScenario()};
   SetupIntervals(grid, cov, moe, testDataService.GetScenario(), "subtype unique\n");

   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cASSET_COVERAGE_TOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 5.0, cASSET_COVERAGE_TOLERANCE);
}
