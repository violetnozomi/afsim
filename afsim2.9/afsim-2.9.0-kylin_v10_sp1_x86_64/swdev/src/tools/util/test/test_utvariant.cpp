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
#include <vector>

#include <gtest/gtest.h>

#include "UtVariant.hpp"

TEST(UtVariant, MultipleTypeAssignment)
{
   ut::Variant var;
   EXPECT_EQ(ut::Variant::Type::cUNSET, var.GetType());

   var = 1;
   EXPECT_EQ(ut::Variant::Type::cINT, var.GetType());
   EXPECT_EQ(1, var.GetInt());

   var = 1.00012321;
   EXPECT_EQ(ut::Variant::Type::cDOUBLE, var.GetType());
   EXPECT_DOUBLE_EQ(1.00012321, var.GetDouble());

   var = "some string";
   EXPECT_EQ(ut::Variant::Type::cSTRING, var.GetType());
   EXPECT_EQ(std::string("some string"), var.GetString());

   var = false;
   EXPECT_EQ(ut::Variant::Type::cBOOL, var.GetType());
   EXPECT_FALSE(var.GetBool());
}

TEST(UtVariant, MoveConstructionInVector)
{
   // This test highlighted an issue where the move ctor for
   // ut::Variant would have a double delete when moving heap
   // allocated types.
   std::vector<ut::Variant> vec;
   EXPECT_NO_THROW({
      for (int i = 0; i < 512; ++i)
      {
         vec.emplace_back(std::to_string(i));
      }
   });
}
