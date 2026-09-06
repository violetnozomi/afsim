// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "gtest/gtest.h"


namespace wsf
{

TEST(PlaceholderTest, Test_Commands)
{
   // Fatal tests
   ASSERT_TRUE(true);
   ASSERT_FALSE(false);

   // Non-Fatal Testing
   EXPECT_TRUE(true);
   EXPECT_FALSE(false);

   ASSERT_EQ(1, 1);
   EXPECT_EQ(1, 1);
   ASSERT_NE(1, 0);
   EXPECT_NE(1, 0);
   ASSERT_LT(0, 1);
   EXPECT_LT(0, 1);
   ASSERT_LE(0, 0);
   EXPECT_LE(0, 0);
   ASSERT_GT(1, 0);
   EXPECT_GT(1, 0);
   ASSERT_GE(0, 0);
   EXPECT_GE(0, 0);
}

} // namespace wsf