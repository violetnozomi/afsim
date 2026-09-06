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
#include "WsfCoverageTimeAverageGap.hpp"

TEST(WsfCoverageTimeAverageGap, BadInput)
{
   auto&                         testDataService = ut::Locator<CoverageTestService>::get();
   wsf::coverage::TimeAverageGap moe{testDataService.GetScenario()};

   TestFalseReturn(moe, "unknown");
}

void SetupIntervals(TestGrid& aGrid, TestCoverage& aCoverage, wsf::coverage::TimeAverageGap& aMoe, const WsfScenario& aScenario)
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

TEST(WsfCoverageTimeAverageGap, Minimum)
{
   auto&                         testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid                      grid{testDataService.GetScenario()};
   TestCoverage                  cov{testDataService.GetScenario(), &grid, "cov"};
   wsf::coverage::TimeAverageGap moe{testDataService.GetScenario()};
   SetupIntervals(grid, cov, moe, testDataService.GetScenario());

   constexpr double cTOLERANCE{1.0e-9};
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(1)), 0.0, cTOLERANCE);
   EXPECT_NEAR(moe.GetMeasuredValue(grid.GetAsset(2)), 2.85, cTOLERANCE);
}
