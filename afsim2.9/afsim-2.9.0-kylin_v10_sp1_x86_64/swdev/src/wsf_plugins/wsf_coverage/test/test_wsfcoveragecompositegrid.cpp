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
#include "WsfCoverageCompositeGrid.hpp"

TEST(WsfCoverageCompositeGrid, ProcessInputBadValue)
{
   auto&                        testDataService = ut::Locator<CoverageTestService>::get();
   wsf::coverage::CompositeGrid grid{testDataService.GetScenario()};

   TestBadValueThrow(grid, "subgrid end_subgrid");
   TestBadValueThrow(grid, "subgrid name end_subgrid");
   TestBadValueThrow(grid, "subgrid name UNKNOWN_GRID_TYPE end_subgrid");
}

TEST(WsfCoverageCompositeGrid, RepeatSubgridName)
{
   auto&                        testDataService = ut::Locator<CoverageTestService>::get();
   wsf::coverage::CompositeGrid grid{testDataService.GetScenario()};

   std::stringstream inputStream{};
   inputStream << "subgrid name WSF_LAT_LON_GRID end_subgrid\n";
   inputStream << "subgrid name WSF_LAT_LON_GRID end_subgrid\n";
   UtInput input{};
   input.PushInputString(inputStream.str());

   std::string command;
   input.ReadCommand(command);
   EXPECT_TRUE(grid.ProcessInput(input));

   input.ReadCommand(command);
   EXPECT_THROW(grid.ProcessInput(input), UtInput::BadValue);
}
