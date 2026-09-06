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

#include "UtAzElLookup.hpp"
#include "UtAzElTable.hpp"

TEST(UtAzElLookup, InterpolationFlags)
{
   using ut::azel::InterpolationType;
   InterpolationType itype = InterpolationType::cLinear;
   UtAzElTable       table;
   UtAzElLookup      lookup;

   // test 1 - interpolate is true
   lookup.SetInterpolate(true);
   EXPECT_TRUE(lookup.Interpolate());

   // test 2 - interpolate is false
   lookup.SetInterpolate(false);
   EXPECT_FALSE(lookup.Interpolate());

   // create a constant table and rerun the tests
   table.CreateConstantTable(20.0);

   // test 3 - interpolate is true
   table.SetInterpolationFlag(true);
   table.GetContext(lookup);
   EXPECT_TRUE(table.GetInterpolationFlag());
   EXPECT_TRUE(lookup.Interpolate());

   // test 4 - interpolate is false
   table.SetInterpolationFlag(false);
   table.GetContext(lookup);
   EXPECT_FALSE(table.GetInterpolationFlag());
   EXPECT_FALSE(lookup.Interpolate());
}
