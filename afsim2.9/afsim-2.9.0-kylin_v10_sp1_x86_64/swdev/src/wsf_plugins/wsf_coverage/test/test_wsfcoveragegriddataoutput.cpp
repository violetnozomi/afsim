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

#include <string>

#include <gtest/gtest.h>

#include "CoverageTestService.hpp"
#include "TestCoverageGrid.hpp"
#include "TestWriter.hpp"
#include "UtInput.hpp"
#include "UtLocator.hpp"
#include "UtMemory.hpp"

namespace
{

constexpr static const char* cEXPECTED_CONTENTS =
   "       Point ID,  Latitude [deg], Longitude [deg],  Alt. (MSL) [m],   Alt. (AGL) [m]\n"
   "              1,        0.000000,        0.000000,        0.000000,         0.000000\n"
   "              2,        0.000000,       90.000000,        0.000000,         0.000000\n"
   "              3,       45.000000,       90.000000,        0.000000,         0.000000\n"
   "              4,       45.000000,        0.000000,        0.000000,         0.000000\n";

}

TEST(WsfCoverageGridDataOutput, TestOutput)
{
   auto&    testDataService = ut::Locator<CoverageTestService>::get();
   TestGrid grid{testDataService.GetScenario()};
   grid.SetName("test_grid");

   UtInput input{};
   input.PushInputString("grid_data_file test_file_name.csv");

   EXPECT_TRUE(grid.ProcessInput(input));
   EXPECT_EQ(grid.GetGridDataFileName(), "test_file_name.csv");

   grid.Initialize();
   TestWriter writer{};
   grid.WriteGridDataFile(writer);

   EXPECT_EQ(writer.GetStreamContents(), std::string(cEXPECTED_CONTENTS));
}
