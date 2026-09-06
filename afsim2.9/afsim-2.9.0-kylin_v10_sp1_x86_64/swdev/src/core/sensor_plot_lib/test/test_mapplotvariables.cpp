// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <gtest/gtest.h>

#include "MapPlotVariables.hpp"
#include "UtInput.hpp"

TEST(MapPlotVariables, ProcessInput)
{
   MapPlotVariables mapPlotVariables; // Performs unit test of RegisterMapPlotVariable()
   UtInput          input;
   input.PushInputString("radar_signature");
   EXPECT_FALSE(mapPlotVariables.ProcessInput(input));
}

TEST(MapPlotVariables, ProcessScriptVariableInput)
{
   MapPlotVariables mapPlotVariables;

   EXPECT_TRUE(mapPlotVariables.ProcessScriptVariableInput("test_script_variable", 0));

   size_t labelCountPre = mapPlotVariables.Count(); // Performs unit test of Count()
   mapPlotVariables.ProcessScriptVariableInput("test_script_variable", 0);
   size_t labelCountPost = mapPlotVariables.Count();
   EXPECT_EQ(labelCountPre, labelCountPost);
}

TEST(MapPlotVariables, AddVariable)
{
   MapPlotVariables mapPlotVariables;

   EXPECT_THROW(mapPlotVariables.AddVariable("test_variable"), std::invalid_argument);
   EXPECT_NO_THROW(mapPlotVariables.AddVariable("radar_signature"));
   EXPECT_EQ(1, mapPlotVariables.Count());
}
