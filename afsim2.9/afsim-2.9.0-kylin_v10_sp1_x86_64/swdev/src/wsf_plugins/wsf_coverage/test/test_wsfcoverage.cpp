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
#include "TestMeasure.hpp"
#include "UtCalendar.hpp"
#include "UtInput.hpp"
#include "UtLocator.hpp"
#include "Utils.hpp"

TEST(WsfCoverage, ProcessInputFailures)
{
   auto&        testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid     grid{testDataService.GetScenario()};
   TestCoverage cov{testDataService.GetScenario(), &grid, "cov"};

   TestEndOfDataThrow(cov, "output_dir");
   TestEndOfDataThrow(cov, "raw_data_file");
   TestEndOfDataThrow(cov, "start_time");
   TestBadValueThrow(cov, "start_time 1 deg");
   TestEndOfDataThrow(cov, "start_epoch");
   TestEndOfDataThrow(cov, "start_epoch jan");
   TestBadValueThrow(cov, "start_epoch abc");
   TestEndOfDataThrow(cov, "start_epoch jan 1");
   TestEndOfDataThrow(cov, "start_epoch jan 1 2020");
   TestBadValueThrow(cov, "start_epoch jan 1 2020 notatime");
   TestBadValueThrow(cov, "start_epoch jan 50 2020 12:00:00.000");
   TestBadValueThrow(cov, "start_epoch jan 1 2020 25:00:00.000");
   TestEndOfDataThrow(cov, "end_time");
   TestBadValueThrow(cov, "end_time 1 deg");
   TestEndOfDataThrow(cov, "end_epoch");
   TestEndOfDataThrow(cov, "end_epoch jan");
   TestBadValueThrow(cov, "end_epoch abc");
   TestEndOfDataThrow(cov, "end_epoch jan 1");
   TestEndOfDataThrow(cov, "end_epoch jan 1 2020");
   TestBadValueThrow(cov, "end_epoch jan 1 2020 notatime");
   TestBadValueThrow(cov, "end_epoch jan 50 2020 12:00:00.000");
   TestBadValueThrow(cov, "end_epoch jan 1 2020 25:00:00.000");
   TestEndOfDataThrow(cov, "grid");
   TestFalseReturn(cov, "unknown");
}

TEST(WsfCoverage, ProcessInput)
{
   std::stringstream stream{};
   stream << "output_dir test_dir\n";
   stream << "start_time 100.0 s\n";
   stream << "end_time 300.0 s\n";
   stream << "start_epoch jan 1 2020 12:01:02.000\n";
   stream << "end_epoch feb 2 2021 13:02:04.000\n";
   UtInput input{};
   input.PushInputString(stream.str());

   auto&        testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid     grid{testDataService.GetScenario()};
   TestCoverage cov{testDataService.GetScenario(), &grid, "cov"};
   EXPECT_TRUE(cov.GetStartTime() < 0.0);
   EXPECT_TRUE(cov.GetEndTime() < 0.0);
   EXPECT_FALSE(cov.IsStartEpochDefined());
   EXPECT_FALSE(cov.IsEndEpochDefined());

   std::string command;
   input.ReadCommand(command);
   EXPECT_TRUE(cov.ProcessInput(input));
   EXPECT_EQ(cov.GetOutputDir(), "test_dir");

   input.ReadCommand(command);
   EXPECT_TRUE(cov.ProcessInput(input));
   EXPECT_NEAR(cov.GetStartTime(), 100.0, 1.0e-12);

   input.ReadCommand(command);
   EXPECT_TRUE(cov.ProcessInput(input));
   EXPECT_NEAR(cov.GetEndTime(), 300.0, 1.0e-12);

   input.ReadCommand(command);
   EXPECT_TRUE(cov.ProcessInput(input));
   UtCalendar testStart{};
   testStart.SetDate(2020, 1, 1);
   testStart.SetTime(12, 1, 2.0);
   EXPECT_EQ(cov.GetStartEpoch(), testStart);
   EXPECT_TRUE(cov.IsStartEpochDefined());

   input.ReadCommand(command);
   EXPECT_TRUE(cov.ProcessInput(input));
   UtCalendar testEnd{};
   testEnd.SetDate(2021, 2, 2);
   testEnd.SetTime(13, 2, 4.0);
   EXPECT_EQ(cov.GetEndEpoch(), testEnd);
   EXPECT_TRUE(cov.IsEndEpochDefined());
}

void TestMOE_Counts(const TestMeasure&              aMeasure,
                    const wsf::coverage::GridAsset& aGridAsset,
                    const wsf::coverage::FreeAsset& aFreeAsset,
                    int                             aStarts,
                    int                             aEnds,
                    int                             aCompletions)
{
   EXPECT_EQ(aMeasure.GetNumIntervalStarts(aGridAsset, aFreeAsset), aStarts);
   EXPECT_EQ(aMeasure.GetNumIntervalEnds(aGridAsset, aFreeAsset), aEnds);
   EXPECT_EQ(aMeasure.GetNumIntervalCompletions(aGridAsset, aFreeAsset), aCompletions);
}

TEST(WsfCoverage, ManualIntervals)
{
   auto& testDataService = ut::Locator<CoverageTestService>::get();

   TestGrid grid{testDataService.GetScenario()};
   grid.SetName("grid");
   grid.Initialize();

   TestCoverage cov{testDataService.GetScenario(), &grid, "cov"};
   UtInput      input{};
   input.PushInputString("start_time 0.0 s\nend_time 100.0 s\n");
   std::string command;
   input.ReadCommand(command);
   EXPECT_TRUE(cov.ProcessInput(input));
   input.ReadCommand(command);
   EXPECT_TRUE(cov.ProcessInput(input));

   TestMeasure moe{testDataService.GetScenario()};
   moe.Initialize(cov);

   // The grid assets and free assets
   auto                     ll = grid.GetAsset(1);
   auto                     lr = grid.GetAsset(2);
   auto                     ur = grid.GetAsset(3);
   auto                     ul = grid.GetAsset(4);
   wsf::coverage::FreeAsset faOne{"test", "sensor"};
   wsf::coverage::FreeAsset faTwo{"plat", "sensor"};

   cov.InjectIntervalStart(ll, faOne, 1.0);
   cov.InjectIntervalEnd(ll, faOne, 2.0);

   EXPECT_THROW(cov.InjectIntervalEnd(lr, faTwo, 5.0), std::runtime_error);

   cov.InjectIntervalStart(ll, faOne, 10.0);
   cov.InjectIntervalEnd(ll, faOne, 20.0);

   cov.InjectIntervalStart(ur, faOne, 1.0);
   cov.InjectIntervalEnd(ur, faOne, 2.0);

   cov.InjectIntervalStart(ul, faTwo, 50.0);

   TestMOE_Counts(moe, ll, faOne, 2, 2, 2);
   TestMOE_Counts(moe, ll, faTwo, 0, 0, 0);
   TestMOE_Counts(moe, lr, faOne, 0, 0, 0);
   TestMOE_Counts(moe, lr, faTwo, 0, 0, 0);
   TestMOE_Counts(moe, ul, faOne, 0, 0, 0);
   TestMOE_Counts(moe, ul, faTwo, 1, 0, 0);
   TestMOE_Counts(moe, ur, faOne, 1, 1, 1);
   TestMOE_Counts(moe, ur, faTwo, 0, 0, 0);

   cov.Finalize(100.0, UtCalendar{});

   TestMOE_Counts(moe, ul, faTwo, 1, 1, 1);
}

TEST(WsfCoverage, InvertedInterval)
{
   auto& testDataService = ut::Locator<CoverageTestService>::get();

   TestGrid grid{testDataService.GetScenario()};
   grid.SetName("grid");
   grid.Initialize();

   TestCoverage cov{testDataService.GetScenario(), &grid, "cov"};
   UtInput      input{};
   input.PushInputString("start_time 0.0 s\nend_time 100.0 s\n");
   std::string command;
   input.ReadCommand(command);
   EXPECT_TRUE(cov.ProcessInput(input));
   input.ReadCommand(command);
   EXPECT_TRUE(cov.ProcessInput(input));

   TestMeasure moe{testDataService.GetScenario()};
   moe.Initialize(cov);

   auto                     ga = grid.GetAsset(1);
   wsf::coverage::FreeAsset fa{"test", "sensor"};

   cov.InjectIntervalStart(ga, fa, 10.0);
   EXPECT_THROW(cov.InjectIntervalEnd(ga, fa, 5.0), std::runtime_error);
}
