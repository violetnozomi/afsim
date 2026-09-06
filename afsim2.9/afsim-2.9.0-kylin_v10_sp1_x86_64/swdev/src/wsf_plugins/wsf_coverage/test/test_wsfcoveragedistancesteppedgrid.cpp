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
#include "UtInput.hpp"
#include "UtLocator.hpp"
#include "Utils.hpp"
#include "WsfCoverageDistanceSteppedGrid.hpp"
#include "WsfCoverageGridAction.hpp"

TEST(WsfCoverageDistanceSteppedGrid, Construction)
{
   auto&                              testDataService = ut::Locator<CoverageTestService>::get();
   wsf::coverage::DistanceSteppedGrid grid{testDataService.GetScenario()};
   EXPECT_FALSE(grid.IsInitialized());
   EXPECT_EQ(grid.GetSize(), 0u);
}

TEST(WsfCoverageDistanceSteppedGrid, ProcessInputErrors)
{
   auto&                              testDataService = ut::Locator<CoverageTestService>::get();
   wsf::coverage::DistanceSteppedGrid grid{testDataService.GetScenario()};

   TestEndOfDataThrow(grid, "step_distance 10");
   TestBadValueThrow(grid, "step_distance 10 s");
   TestBadValueThrow(grid, "step_distance -10 m");
   TestEndOfDataThrow(grid, "step_distance 10 m");
   TestEndOfDataThrow(grid, "step_distance 10 m 12");
   TestBadValueThrow(grid, "step_distance 10 m 12 s");
   TestBadValueThrow(grid, "step_distance 10 m -12 m");
   TestEndOfDataThrow(grid, "size");
   TestEndOfDataThrow(grid, "size 12");
   TestBadValueThrow(grid, "size -10 10");
   TestBadValueThrow(grid, "size 10 -10");
   TestEndOfDataThrow(grid, "origin_index");
   TestEndOfDataThrow(grid, "origin_index 12");
   TestFalseReturn(grid, "unknown");
   TestBadValueThrow(grid, "altitude 10 s");
   TestEndOfDataThrow(grid, "altitude 10 m");
   TestBadValueThrow(grid, "altitude 10 m unk");
   TestBadValueThrow(grid, "origin 10 m");
   TestBadValueThrow(grid, "origin 10:00:00.000n 10 m");
   TestEndOfDataThrow(grid, "asset");
   TestEndOfDataThrow(grid, "asset typename");
}

TEST(WsfCoverageDistanceSteppedGrid, AcceptableInput)
{
   auto&                              testDataService = ut::Locator<CoverageTestService>::get();
   wsf::coverage::DistanceSteppedGrid grid{testDataService.GetScenario()};

   std::stringstream inputStream{};
   inputStream << "step_distance 100 km 120 km\n";
   inputStream << "size 10 12\n";
   inputStream << "origin_index 4 5\n";
   inputStream << "altitude 100 m agl\n";
   inputStream << "altitude 120 m msl\n";
   inputStream << "asset TYPE device\n";
   inputStream << "origin 10n 20e\n";
   UtInput input{};
   input.PushInputString(inputStream.str());

   std::string command;
   input.ReadCommand(command);
   EXPECT_TRUE(grid.ProcessInput(input));
   EXPECT_NEAR(grid.GetLatStepDistance(), 100.0e3, 1.0e-6);
   EXPECT_NEAR(grid.GetLonStepDistance(), 120.0e3, 1.0e-6);

   input.ReadCommand(command);
   EXPECT_TRUE(grid.ProcessInput(input));
   EXPECT_EQ(grid.GetNumLatSteps(), 10);
   EXPECT_EQ(grid.GetNumLonSteps(), 12);

   input.ReadCommand(command);
   EXPECT_TRUE(grid.ProcessInput(input));
   EXPECT_EQ(grid.GetLatOriginIndex(), 4);
   EXPECT_EQ(grid.GetLonOriginIndex(), 5);

   input.ReadCommand(command);
   EXPECT_TRUE(grid.ProcessInput(input));
   EXPECT_NEAR(grid.GetAltitude(), 100.0, 1.0e-6);
   EXPECT_TRUE(grid.IsAltitudeAGL());

   input.ReadCommand(command);
   EXPECT_TRUE(grid.ProcessInput(input));
   EXPECT_NEAR(grid.GetAltitude(), 120.0, 1.0e-6);
   EXPECT_FALSE(grid.IsAltitudeAGL());

   input.ReadCommand(command);
   EXPECT_TRUE(grid.ProcessInput(input));
   EXPECT_EQ(grid.GetAssetPlatformType(), "TYPE");
   EXPECT_EQ(grid.GetAssetDeviceName(), "device");

   input.ReadCommand(command);
   EXPECT_TRUE(grid.ProcessInput(input));
   EXPECT_NEAR(grid.GetLatitudeOriginDeg(), 10.0, 1.0e-6);
   EXPECT_NEAR(grid.GetLongitudeOriginDeg(), 20.0, 1.0e-6);
}

void TestGridInitialize(const std::string& aInputString, bool aExpectSuccess)
{
   auto&                              testDataService = ut::Locator<CoverageTestService>::get();
   wsf::coverage::DistanceSteppedGrid grid{testDataService.GetScenario()};
   grid.SetName("DistanceSteppedTestGrid");

   UtInput input{};
   input.PushInputString(aInputString);

   try
   {
      std::string command;
      while (true)
      {
         input.ReadCommand(command);
         EXPECT_TRUE(grid.ProcessInput(input));
      }
   }
   catch (UtInput::EndOfData&)
   {
      // This was expected; absorb the exception.
   }

   grid.SetUsedFlag();
   EXPECT_EQ(grid.Initialize(), aExpectSuccess);
}

TEST(WsfCoverageDistanceSteppedGrid, FailedInitializationStepDistance)
{
   TestGridInitialize("", false);
}

TEST(WsfCoverageDistanceSteppedGrid, FailedInitializationStepCount)
{
   TestGridInitialize("step_distance 100 km 100 km", false);
}

TEST(WsfCoverageDistanceSteppedGrid, FailedInitializationOriginIndex)
{
   TestGridInitialize("step_distance 100 km 100 km\nsize 10 10", false);
   TestGridInitialize("step_distance 100 km 100 km\nsize 10 10\norigin_index 0 20", false);
   TestGridInitialize("step_distance 100 km 100 km\nsize 10 10\norigin_index 20 0", false);
   TestGridInitialize("step_distance 100 km 100 km\nsize 10 10\norigin_index -10 2", false);
   TestGridInitialize("step_distance 100 km 100 km\nsize 10 10\norigin_index 2 -20", false);
}

TEST(WsfCoverageDistanceSteppedGrid, FailedInitializationAssetSpecification)
{
   TestGridInitialize("step_distance 100 km 100 km\nsize 10 10\norigin_index 1 2", false);
}

TEST(WsfCoverageDistanceSteppedGrid, FailedInitializationOrigin)
{
   TestGridInitialize("step_distance 100 km 100 km\nsize 10 10\n"
                      "origin_index 1 2\nasset TYPE device",
                      false);
}

TEST(WsfCoverageDistanceSteppedGrid, InitializationSuccess)
{
   TestGridInitialize("step_distance 100 km 100 km\nsize 10 10\n"
                      "origin_index 1 2\nasset TYPE device\norigin 10n 20w",
                      true);
}
