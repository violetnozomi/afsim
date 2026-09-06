// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <gtest/gtest.h>

#include "UtInput.hpp"
#include "Utils.hpp"
#include "WsfOrbitalTargetingCost.hpp"

TEST(BlendedCost, BasicUsage)
{
   static constexpr double cDBL_TOLERANCE{1.0e-12};

   // Defalt construction should be equivalent to delta-v.
   wsf::space::BlendedCost cost{};
   EXPECT_NEAR(cost.GetA_Value(), 0.0, cDBL_TOLERANCE);
   EXPECT_NEAR(cost.GetB_Value(), 1.0, cDBL_TOLERANCE);
   EXPECT_NEAR(cost.GetC_Value(), 0.0, cDBL_TOLERANCE);

   static constexpr double cA_VAL{1.0};
   static constexpr double cB_VAL{2.0};
   static constexpr double cC_VAL{10.0};
   cost.SetA_Value(cA_VAL);
   cost.SetB_Value(cB_VAL);
   cost.SetC_Value(cC_VAL);
   EXPECT_NEAR(cost.GetA_Value(), cA_VAL, cDBL_TOLERANCE);
   EXPECT_NEAR(cost.GetB_Value(), cB_VAL, cDBL_TOLERANCE);
   EXPECT_NEAR(cost.GetC_Value(), cC_VAL, cDBL_TOLERANCE);

   // Test functional form
   EXPECT_NEAR(cost(1.0, 0.0), cA_VAL, cDBL_TOLERANCE);
   EXPECT_NEAR(cost(0.0, 1.0), cB_VAL, cDBL_TOLERANCE);
   EXPECT_NEAR(cost(1.0, 1.0), cA_VAL + cB_VAL + cC_VAL, cDBL_TOLERANCE);
   EXPECT_NEAR(cost(1.0, -1.0), cA_VAL - cB_VAL - cC_VAL, cDBL_TOLERANCE);
}

TEST(BlendedCost, ProcessInput)
{
   static constexpr double cDBL_TOLERANCE{1.0e-12};

   wsf::space::BlendedCost cost{};

   TestEndOfDataThrow(cost, "5.0");
   TestEndOfDataThrow(cost, "5.0 1.0");

   UtInput input{};
   input.PushInputString("1.0 2.0 3.0");
   EXPECT_TRUE(cost.ProcessInput(input));

   EXPECT_NEAR(cost.GetA_Value(), 1.0, cDBL_TOLERANCE);
   EXPECT_NEAR(cost.GetB_Value(), 2.0, cDBL_TOLERANCE);
   EXPECT_NEAR(cost.GetC_Value(), 3.0, cDBL_TOLERANCE);
}
