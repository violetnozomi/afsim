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
#include "WsfCoverageGridAction.hpp"
#include "WsfCoverageLatLonGrid.hpp"

TEST(WsfCoverageLatLonGrid, Construction)
{
   auto&                     testDataService = ut::Locator<CoverageTestService>::get();
   wsf::coverage::LatLonGrid grid{testDataService.GetScenario()};
   EXPECT_FALSE(grid.IsInitialized());
   EXPECT_EQ(grid.GetSize(), 0u);
}

TEST(WsfCoverageLatLonGrid, ProcessInputBadValue)
{
   auto&                     testDataService = ut::Locator<CoverageTestService>::get();
   wsf::coverage::LatLonGrid grid{testDataService.GetScenario()};

   TestBadValueThrow(grid, "latitude_span 10 m\n");
   TestBadValueThrow(grid, "latitude_span 10:00:00.000n 10 m\n");
   TestBadValueThrow(grid, "longitude_span 10 m\n");
   TestBadValueThrow(grid, "longitude_span 10:00:00.000e deg 10 m\n");
   TestBadValueThrow(grid, "longitude_span 10:00:00.000e deg 00:00:00.00e\n");
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
}

TEST(WsfCoverageLatLonGrid, AcceptableInput)
{
   constexpr double cTOLERANCE{1.0e-12};

   auto&                     testDataService = ut::Locator<CoverageTestService>::get();
   wsf::coverage::LatLonGrid grid{testDataService.GetScenario()};

   std::stringstream inputStream{};
   // clang-format off
   inputStream << "latitude_span 00:00:00.000n 20:00:00.000n\n"
               << "latitude_span 20:00:00.000n 00:00:00.000n\n"
               << "longitude_span 5:00:00.000e 30:00:00.000e\n"
               << "altitude 125 m agl\n"
               << "altitude 125 m msl\n"
               << "latitude_spacing 1 deg\n"
               << "spacing 2 deg\n"
               << "longitude_spacing 1 deg\n"
               << "origin 10:00:00.000n 10:00:00.000e\n";
   // clang-format on
   UtInput input{};
   input.PushInputString(inputStream.str());

   std::string command;
   input.ReadCommand(command);
   EXPECT_TRUE(grid.ProcessInput(input));
   EXPECT_NEAR(grid.GetMinimumLatitudeDeg(), 0.0, cTOLERANCE);
   EXPECT_NEAR(grid.GetMaximumLatitudeDeg(), 20.0, cTOLERANCE);

   input.ReadCommand(command);
   EXPECT_TRUE(grid.ProcessInput(input));
   EXPECT_NEAR(grid.GetMinimumLatitudeDeg(), 0.0, cTOLERANCE);
   EXPECT_NEAR(grid.GetMaximumLatitudeDeg(), 20.0, cTOLERANCE);

   input.ReadCommand(command);
   EXPECT_TRUE(grid.ProcessInput(input));
   EXPECT_NEAR(grid.GetMinimumLongitudeDeg(), 5.0, cTOLERANCE);
   EXPECT_NEAR(grid.GetMaximumLongitudeDeg(), 30.0, cTOLERANCE);

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
}

TEST(WsfCoverageLatLonGrid, FailedInitializationLatSpacing)
{
   auto&                     testDataService = ut::Locator<CoverageTestService>::get();
   wsf::coverage::LatLonGrid grid{testDataService.GetScenario()};
   grid.SetName("LatSpacingTestGrid");

   std::stringstream inputStream{};
   inputStream << "latitude_span 00:00:00.000n 10:00:00.000n\n";
   inputStream << "longitude_span 0:00:00.000e 10:00:00.000e\n";
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

TEST(WsfCoverageLatLonGrid, FailedInitializationLonSpacing)
{
   auto&                     testDataService = ut::Locator<CoverageTestService>::get();
   wsf::coverage::LatLonGrid grid{testDataService.GetScenario()};
   grid.SetName("LonSpacingTestGrid");

   std::stringstream inputStream{};
   inputStream << "latitude_span 00:00:00.000n 10:00:00.000n\n";
   inputStream << "longitude_span 0:00:00.000e 10:00:00.000e\n";
   inputStream << "altitude 125 m agl\n";
   inputStream << "latitude_spacing 1 deg\n";
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

TEST(WsfCoverageLatLonGrid, FailedInitializationLatOrigin)
{
   auto&                     testDataService = ut::Locator<CoverageTestService>::get();
   wsf::coverage::LatLonGrid grid{testDataService.GetScenario()};
   grid.SetName("LonSpacingTestGrid");

   std::stringstream inputStream{};
   inputStream << "latitude_span 00:00:00.000n 10:00:00.000n\n";
   inputStream << "longitude_span 0:00:00.000e 10:00:00.000e\n";
   inputStream << "altitude 125 m agl\n";
   inputStream << "latitude_spacing 1 deg\n";
   inputStream << "longitude_spacing 1 deg\n";
   inputStream << "origin 20:00:00.000n 05:00:00.000e";
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

TEST(WsfCoverageLatLonGrid, FailedInitializationLonOrigin)
{
   auto&                     testDataService = ut::Locator<CoverageTestService>::get();
   wsf::coverage::LatLonGrid grid{testDataService.GetScenario()};
   grid.SetName("LonSpacingTestGrid");

   std::stringstream inputStream{};
   inputStream << "latitude_span 00:00:00.000n 10:00:00.000n\n";
   inputStream << "longitude_span 0:00:00.000e 10:00:00.000e\n";
   inputStream << "altitude 125 m agl\n";
   inputStream << "latitude_spacing 1 deg\n";
   inputStream << "longitude_spacing 1 deg\n";
   inputStream << "origin 05:00:00.000n 20:00:00.000e";
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

TEST(WsfCoverageLatLonGrid, InitializedGrid)
{
   constexpr double cTOLERANCE{1.0e-12};

   auto&                     testDataService = ut::Locator<CoverageTestService>::get();
   wsf::coverage::LatLonGrid grid{testDataService.GetScenario()};

   std::stringstream inputStream{};
   inputStream << "latitude_span 00:00:00.000n 10:00:00.000n\n";
   inputStream << "longitude_span 0:00:00.000e 10:00:00.000e\n";
   inputStream << "altitude 125 m agl\n";
   inputStream << "spacing 1 deg\n";
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

   // Verify setup
   EXPECT_NEAR(grid.GetMinimumLatitudeDeg(), 0.0, cTOLERANCE);
   EXPECT_NEAR(grid.GetMaximumLatitudeDeg(), 10.0, cTOLERANCE);
   EXPECT_NEAR(grid.GetMinimumLongitudeDeg(), 0.0, cTOLERANCE);
   EXPECT_NEAR(grid.GetMaximumLongitudeDeg(), 10.0, cTOLERANCE);
   EXPECT_NEAR(grid.GetLatitudeSpacingDeg(), 1.0, cTOLERANCE);
   EXPECT_NEAR(grid.GetLongitudeSpacingDeg(), 1.0, cTOLERANCE);
   EXPECT_NEAR(grid.GetAltitude(), 125.0, cTOLERANCE);

   // Initialize
   grid.SetUsedFlag();
   EXPECT_TRUE(grid.Initialize());
   EXPECT_TRUE(grid.IsInitialized());

   // Should have an 11x11 grid of points
   EXPECT_EQ(grid.GetSize(), 11u * 11u);

   // Invocation testing
   std::vector<wsf::coverage::PointId> ids{};
   wsf::coverage::PointId              maxId{wsf::coverage::cINVALID_POINT_ID};
   auto                                collectIds = wsf::coverage::CreateGridAction(
      [&ids, &maxId](const wsf::coverage::Grid& aGrid, const wsf::coverage::Point& aPoint)
      {
         ids.push_back(aPoint.GetID());
         if (aPoint.GetID() > maxId)
         {
            maxId = aPoint.GetID();
         }
      });
   grid.Invoke(*collectIds);

   // Get points
   for (auto id : ids)
   {
      auto pointPtr = grid.GetPoint(id);
      EXPECT_TRUE(pointPtr);
      EXPECT_EQ(pointPtr->GetID(), id);
   }
   EXPECT_FALSE(grid.GetPoint(maxId + 20));
}

TEST(WsfCoverageLatLonGrid, InitializedGridOrigin)
{
   constexpr double cTOLERANCE{1.0e-12};

   auto&                     testDataService = ut::Locator<CoverageTestService>::get();
   wsf::coverage::LatLonGrid grid{testDataService.GetScenario()};

   std::stringstream inputStream{};
   inputStream << "latitude_span 00:00:00.000n 10:00:00.000n\n";
   inputStream << "longitude_span 0:00:00.000e 10:00:00.000e\n";
   inputStream << "altitude 125 m agl\n";
   inputStream << "spacing 1 deg\n";
   inputStream << "origin 00:30:00.000n 00:30:00.000e\n";
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

   // Verify setup
   EXPECT_NEAR(grid.GetMinimumLatitudeDeg(), 0.0, cTOLERANCE);
   EXPECT_NEAR(grid.GetMaximumLatitudeDeg(), 10.0, cTOLERANCE);
   EXPECT_NEAR(grid.GetMinimumLongitudeDeg(), 0.0, cTOLERANCE);
   EXPECT_NEAR(grid.GetMaximumLongitudeDeg(), 10.0, cTOLERANCE);
   EXPECT_NEAR(grid.GetLatitudeSpacingDeg(), 1.0, cTOLERANCE);
   EXPECT_NEAR(grid.GetLongitudeSpacingDeg(), 1.0, cTOLERANCE);
   EXPECT_NEAR(grid.GetLatitudeOriginDeg(), 0.5, cTOLERANCE);
   EXPECT_NEAR(grid.GetLongitudeOriginDeg(), 0.5, cTOLERANCE);
   EXPECT_NEAR(grid.GetAltitude(), 125.0, cTOLERANCE);

   // Initialize
   grid.SetUsedFlag();
   EXPECT_TRUE(grid.Initialize());
   EXPECT_TRUE(grid.IsInitialized());

   // Should have a 10x10 grid of points
   EXPECT_EQ(grid.GetSize(), 10u * 10u);

   // Invocation testing
   std::vector<wsf::coverage::PointId> ids{};
   wsf::coverage::PointId              maxId{wsf::coverage::cINVALID_POINT_ID};
   auto                                collectIds = wsf::coverage::CreateGridAction(
      [&ids, &maxId](const wsf::coverage::Grid& aGrid, const wsf::coverage::Point& aPoint)
      {
         ids.push_back(aPoint.GetID());
         if (aPoint.GetID() > maxId)
         {
            maxId = aPoint.GetID();
         }
      });
   grid.Invoke(*collectIds);

   // Get points
   for (auto id : ids)
   {
      auto pointPtr = grid.GetPoint(id);
      EXPECT_TRUE(pointPtr);
      EXPECT_EQ(pointPtr->GetID(), id);
   }
   EXPECT_FALSE(grid.GetPoint(maxId + 20));
}

TEST(WsfCoverageLatLonGrid, InitializedGridOriginAlt)
{
   constexpr double cTOLERANCE{1.0e-12};

   auto&                     testDataService = ut::Locator<CoverageTestService>::get();
   wsf::coverage::LatLonGrid grid{testDataService.GetScenario()};

   std::stringstream inputStream{};
   inputStream << "latitude_span 00:00:00.000n 10:00:00.000n\n";
   inputStream << "longitude_span 0:00:00.000e 10:00:00.000e\n";
   inputStream << "altitude 125 m agl\n";
   inputStream << "spacing 1 deg\n";
   inputStream << "origin 00:00:00.000n 5:00:00.000e\n";
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

   // Verify setup
   EXPECT_NEAR(grid.GetMinimumLatitudeDeg(), 0.0, cTOLERANCE);
   EXPECT_NEAR(grid.GetMaximumLatitudeDeg(), 10.0, cTOLERANCE);
   EXPECT_NEAR(grid.GetMinimumLongitudeDeg(), 0.0, cTOLERANCE);
   EXPECT_NEAR(grid.GetMaximumLongitudeDeg(), 10.0, cTOLERANCE);
   EXPECT_NEAR(grid.GetLatitudeSpacingDeg(), 1.0, cTOLERANCE);
   EXPECT_NEAR(grid.GetLongitudeSpacingDeg(), 1.0, cTOLERANCE);
   EXPECT_NEAR(grid.GetLatitudeOriginDeg(), 0.0, cTOLERANCE);
   EXPECT_NEAR(grid.GetLongitudeOriginDeg(), 5.0, cTOLERANCE);
   EXPECT_NEAR(grid.GetAltitude(), 125.0, cTOLERANCE);

   // Initialize
   grid.SetUsedFlag();
   EXPECT_TRUE(grid.Initialize());
   EXPECT_TRUE(grid.IsInitialized());

   // Should have a 10x10 grid of points
   EXPECT_EQ(grid.GetSize(), 11u * 11u);

   // Invocation testing
   std::vector<wsf::coverage::PointId> ids{};
   wsf::coverage::PointId              maxId{wsf::coverage::cINVALID_POINT_ID};
   auto                                collectIds = wsf::coverage::CreateGridAction(
      [&ids, &maxId](const wsf::coverage::Grid& aGrid, const wsf::coverage::Point& aPoint)
      {
         ids.push_back(aPoint.GetID());
         if (aPoint.GetID() > maxId)
         {
            maxId = aPoint.GetID();
         }
      });
   grid.Invoke(*collectIds);

   // Get points
   for (auto id : ids)
   {
      auto pointPtr = grid.GetPoint(id);
      EXPECT_TRUE(pointPtr);
      EXPECT_EQ(pointPtr->GetID(), id);
   }
   EXPECT_FALSE(grid.GetPoint(maxId + 20));
}

TEST(WsfCoverageLatLonGrid, InitializeAndGetAsset)
{
   auto&                     testDataService = ut::Locator<CoverageTestService>::get();
   wsf::coverage::LatLonGrid grid{testDataService.GetScenario()};
   grid.SetName("grid");

   std::stringstream inputStream{};
   inputStream << "latitude_span 00:00:00.000n 10:00:00.000n\n";
   inputStream << "longitude_span 0:00:00.000e 10:00:00.000e\n";
   inputStream << "altitude 125 m agl\n";
   inputStream << "spacing 1 deg\n";
   inputStream << "origin 00:30:00.000n 00:30:00.000e\n";
   inputStream << "asset typename device\n";
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

   // Initialize
   grid.SetUsedFlag();
   EXPECT_TRUE(grid.Initialize());
   EXPECT_TRUE(grid.IsInitialized());

   std::vector<wsf::coverage::PointId> ids{};
   auto                                collectIds = wsf::coverage::CreateGridAction(
      [&ids](const wsf::coverage::Grid& aGrid, const wsf::coverage::Point& aPoint) { ids.push_back(aPoint.GetID()); });
   grid.Invoke(*collectIds);

   for (auto id : ids)
   {
      auto ga = grid.GetAsset(id);
      EXPECT_TRUE(ga.IsValid());

      std::string platName = grid.GetName() + "_" + std::to_string(id);
      auto        alt      = grid.GetAsset(platName);
      EXPECT_TRUE(alt.IsValid());
      EXPECT_EQ(alt.GetPlatformName(), platName);
      EXPECT_EQ(alt.GetDeviceName(), "device");
      EXPECT_EQ(alt.GetPointId(), id);
   }
}
