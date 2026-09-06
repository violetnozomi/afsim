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
#include "WsfCoverageZoneBasedGrid.hpp"

TEST(WsfCoverageZoneBasedGrid, Construction)
{
   wsf::coverage::ZoneBasedGrid grid{ut::Locator<CoverageTestService>::get().GetScenario()};
   EXPECT_FALSE(grid.IsInitialized());
   EXPECT_EQ(grid.GetSize(), 0u);
}

TEST(WsfCoverageZoneBasedGrid, ProcessInputBadValue)
{
   wsf::coverage::ZoneBasedGrid grid{ut::Locator<CoverageTestService>::get().GetScenario()};

   TestFalseReturn(grid, "latitude_span\n");
   TestFalseReturn(grid, "longitude_span\n");
   TestBadValueThrow(grid, "altitude 10 s\n");
   TestEndOfDataThrow(grid, "altitude 10 m\n");
   TestBadValueThrow(grid, "altitude 10 m unknown\n");
   TestBadValueThrow(grid, "latitude_spacing 10 m\n");
   TestBadValueThrow(grid, "latitude_spacing -1 deg\n");
   TestBadValueThrow(grid, "longitude_spacing 10 m\n");
   TestBadValueThrow(grid, "longitude_spacing -1 deg\n");
   TestBadValueThrow(grid, "spacing 1 s\n");
   TestBadValueThrow(grid, "spacing -10 deg\n");
   TestBadValueThrow(grid, "origin 10 m\n");
   TestBadValueThrow(grid, "origin 10:00:00.000n 10 m\n");
   TestEndOfDataThrow(grid, "asset\n");
   TestEndOfDataThrow(grid, "asset typename\n");
   TestEndOfDataThrow(grid, "zone\n");
}

TEST(WsfCoverageZoneBasedGrid, AcceptableInput)
{
   constexpr double cTOLERANCE{1.0e-12};

   wsf::coverage::ZoneBasedGrid grid{ut::Locator<CoverageTestService>::get().GetScenario()};

   std::stringstream inputStream{};
   inputStream << "altitude 125 m agl\n";
   inputStream << "altitude 125 m msl\n";
   inputStream << "latitude_spacing 1 deg\n";
   inputStream << "spacing 2 deg\n";
   inputStream << "longitude_spacing 1 deg\n";
   inputStream << "origin 10:00:00.000n 10:00:00.000e\n";
   inputStream << "zone twilight\n";
   UtInput input{};
   input.PushInputString(inputStream.str());

   std::string command;
   input.ReadCommand(command);
   EXPECT_TRUE(grid.ProcessInput(input));
   EXPECT_NEAR(grid.GetAltitude(), 125.0, cTOLERANCE);

   input.ReadCommand(command);
   EXPECT_TRUE(grid.ProcessInput(input));
   EXPECT_NEAR(grid.GetAltitude(), 125.0, cTOLERANCE);

   input.ReadCommand(command);
   EXPECT_TRUE(grid.ProcessInput(input));
   EXPECT_NEAR(grid.GetLatitudeSpacingDeg(), 1.0, cTOLERANCE);
   EXPECT_TRUE(grid.GetLongitudeSpacingDeg() < 0.0);

   input.ReadCommand(command);
   EXPECT_TRUE(grid.ProcessInput(input));
   EXPECT_NEAR(grid.GetLatitudeSpacingDeg(), 1.0, cTOLERANCE);
   EXPECT_NEAR(grid.GetLongitudeSpacingDeg(), 2.0, cTOLERANCE);

   input.ReadCommand(command);
   EXPECT_TRUE(grid.ProcessInput(input));
   EXPECT_NEAR(grid.GetLatitudeSpacingDeg(), 1.0, cTOLERANCE);
   EXPECT_NEAR(grid.GetLongitudeSpacingDeg(), 1.0, cTOLERANCE);

   input.ReadCommand(command);
   EXPECT_TRUE(grid.ProcessInput(input));
   EXPECT_NEAR(grid.GetLatitudeOriginDeg(), 10.0, cTOLERANCE);
   EXPECT_NEAR(grid.GetLongitudeOriginDeg(), 10.0, cTOLERANCE);

   input.ReadCommand(command);
   EXPECT_TRUE(grid.ProcessInput(input));
   EXPECT_EQ(grid.GetZoneName(), "twilight");
}

TEST(WsfCoverageZoneBasedGrid, FailedInitializationZoneSpecified)
{
   wsf::coverage::LatLonGrid grid{ut::Locator<CoverageTestService>::get().GetScenario()};
   grid.SetName("ZoneBasedTestGrid");

   std::stringstream inputStream{};
   inputStream << "altitude 125 m agl\n";
   inputStream << "longitude_spacing 1 deg\n";
   UtInput input{};
   input.PushInputString(inputStream.str());

   try
   {
      std::string command;
      while (true)
      {
         input.ReadCommand(command);
         grid.ProcessInput(input);
      }
   }
   catch (UtInput::EndOfData&)
   {
      // This was expected; absorb the exception.
   }

   grid.SetUsedFlag();
   EXPECT_FALSE(grid.Initialize());
}

TEST(WsfCoverageZoneBasedGrid, FailedInitializationZoneNotExist)
{
   wsf::coverage::LatLonGrid grid{ut::Locator<CoverageTestService>::get().GetScenario()};
   grid.SetName("ZoneBasedTestGrid");

   std::stringstream inputStream{};
   inputStream << "altitude 125 m agl\n";
   inputStream << "longitude_spacing 1 deg\n";
   inputStream << "zone unknown\n";
   UtInput input{};
   input.PushInputString(inputStream.str());

   try
   {
      std::string command;
      while (true)
      {
         input.ReadCommand(command);
         grid.ProcessInput(input);
      }
   }
   catch (UtInput::EndOfData&)
   {
      // This was expected; absorb the exception.
   }

   grid.SetUsedFlag();
   EXPECT_FALSE(grid.Initialize());
}
