// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include <gtest/gtest.h>

#include "UtVec2.hpp"

TEST(UtVec2, Addition)
{
   UtVec2<int> v1(4);
   UtVec2<int> v2(6);
   auto        v3 = v1 + v2;
   EXPECT_EQ(10, v3[0]);
   EXPECT_EQ(10, v3[1]);
   v3.Add(v1);
   EXPECT_EQ(14, v3[0]);
   EXPECT_EQ(14, v3[1]);
}

TEST(UtVec2, Equality)
{
   UtVec2<int> v1(6);
   UtVec2<int> v2(11);
   UtVec2<int> v3(6);
   EXPECT_FALSE(v1.Equals(v2));
   EXPECT_TRUE(v1.Equals(v3));
   EXPECT_FALSE(v1.Equals(11));
   EXPECT_TRUE(v1.Equals(6));
   EXPECT_TRUE(v1.Equals(v1));
   EXPECT_TRUE(v1 == v1);
   EXPECT_TRUE(v1 == v3);
   EXPECT_FALSE(v1 == v2);
   EXPECT_TRUE(v1 == 6);
   EXPECT_FALSE(v1 == 11);
}

TEST(UtVec2, DoubleConversion)
{
   UtVec2<double> src(3.1415, 2.1111);
   UtVec2<int>    target(src);
   EXPECT_EQ(3, target[0]);
   EXPECT_EQ(2, target[1]);
}
