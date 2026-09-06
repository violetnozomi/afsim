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
#include "UtInput.hpp"
#include "UtLocator.hpp"
#include "Utils.hpp"
#include "WsfCoverageExistingPlatformGrid.hpp"

TEST(WsfCoverageExistingPlatformGrid, Construction)
{
   auto&                               testDataService = ut::Locator<CoverageTestService>::get();
   wsf::coverage::ExistingPlatformGrid grid{testDataService.GetScenario()};
   EXPECT_FALSE(grid.IsInitialized());
   EXPECT_EQ(grid.GetSize(), 0u);
}

TEST(WsfCoverageExistingPlatformGrid, ProcessInputBadValue)
{
   auto&                               testDataService = ut::Locator<CoverageTestService>::get();
   wsf::coverage::ExistingPlatformGrid grid{testDataService.GetScenario()};

   TestEndOfDataThrow(grid, "platform");
   TestEndOfDataThrow(grid, "platform name");
   TestFalseReturn(grid, "unknown");
}

TEST(WsfCoverageExistingPlatformGrid, InitializeFailure)
{
   auto&                               testDataService = ut::Locator<CoverageTestService>::get();
   wsf::coverage::ExistingPlatformGrid grid{testDataService.GetScenario()};
   UtInput                             input{};
   input.PushInputString("platform test device");

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
      // Expected; absorb the exception.
   }

   grid.SetUsedFlag();
   EXPECT_FALSE(grid.Initialize());
}
