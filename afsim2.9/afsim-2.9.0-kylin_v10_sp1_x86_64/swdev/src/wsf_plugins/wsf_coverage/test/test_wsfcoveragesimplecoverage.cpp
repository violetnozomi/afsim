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
#include "UtInput.hpp"
#include "UtLocator.hpp"
#include "WsfSimpleCoverage.hpp"

TEST(WsfSimpleCoverage, ManualIntervals)
{
   auto& testDataService = ut::Locator<CoverageTestService>::get();

   TestGrid grid{testDataService.GetScenario()};
   grid.SetName("test_grid");
   grid.Initialize();

   TestCoverage cov{testDataService.GetScenario(), &grid, "cov"};
   UtInput      input{};
   input.PushInputString("start_time 0.0 s\nend_time 100.0 s\n");
   std::string command;
   input.ReadCommand(command);
   EXPECT_TRUE(cov.ProcessInput(input));
   input.ReadCommand(command);
   EXPECT_TRUE(cov.ProcessInput(input));

   wsf::coverage::SimpleCoverage moe{testDataService.GetScenario()};
   moe.Initialize(cov);

   auto                     ll = grid.GetAsset(1);
   auto                     lr = grid.GetAsset(2);
   auto                     ur = grid.GetAsset(3);
   auto                     ul = grid.GetAsset(4);
   wsf::coverage::FreeAsset faOne{"test", "sensor"};

   cov.InjectInterval(ll, faOne, 1.0, 4.0);
   cov.InjectInterval(lr, faOne, 2.0, 12.0);

   EXPECT_THROW(cov.InjectIntervalEnd(ul, faOne, 20.0), std::runtime_error);

   // Should get a 1 from ll, lr, and a 0 from ul,ans ur
   EXPECT_NEAR(moe.GetMeasuredValue(ll), 1.0, 1.0e-12);
   EXPECT_NEAR(moe.GetMeasuredValue(lr), 1.0, 1.0e-12);
   EXPECT_NEAR(moe.GetMeasuredValue(ul), 0.0, 1.0e-12);
   EXPECT_NEAR(moe.GetMeasuredValue(ur), 0.0, 1.0e-12);
}
