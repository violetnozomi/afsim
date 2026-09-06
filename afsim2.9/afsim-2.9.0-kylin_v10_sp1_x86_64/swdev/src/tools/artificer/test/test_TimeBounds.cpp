// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright (C) 2022 Stellar Science; U.S. Government has Unlimited Rights.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include <gtest/gtest.h>

#include "TimeBounds.hpp"


TEST(Bounds, Basic)
{
   using Bound = artificer::TimeBounds;
   Bound singleValue;
   EXPECT_FALSE(singleValue.HasValues());
   singleValue.Union(10);
   EXPECT_TRUE(singleValue.HasValues());

   Bound time(1, 2);
   EXPECT_TRUE(time.HasValues());
   EXPECT_EQ(time.min, 1);
   EXPECT_EQ(time.max, 2);
   EXPECT_EQ(time.Delta(), 1);

   time.Union(Bound(2, 3));
   time.Union(Bound(2, 3));

   EXPECT_EQ(time.min, 1);
   EXPECT_EQ(time.max, 3);
   EXPECT_EQ(time.Delta(), 2);

   Bound time2(2, 4);

   auto result(time);
   result.Intersection(time2);

   EXPECT_EQ(result.min, 2);
   EXPECT_EQ(result.max, 3);

   Bound   range(1, 10);
   int64_t subtractResult = *range.DeltaExcluding({{1, 3}, {6, 7}, {11, 12}});
   EXPECT_EQ(subtractResult, 6);
}

TEST(Bounds, Booleans)
{
   using Bound                                = artificer::TimeBounds;
   const std::vector<Bound> independentRanges = {{1, 10}, {21, 30}, {11, 20}};

   Bound unitedRange;
   for (auto outer = independentRanges.begin(); outer != independentRanges.end(); ++outer)
   {
      unitedRange.Union(*outer);
      for (auto inner = std::next(outer); inner != independentRanges.end(); ++inner)
      {
         EXPECT_FALSE(outer->Intersects(*inner));
         EXPECT_EQ(Bound(*outer).Intersection(*inner), Bound());
      }
   }

   const Bound expectedUnited(1, 30);
   EXPECT_EQ(unitedRange, expectedUnited);
   EXPECT_EQ(expectedUnited.DeltaExcluding(independentRanges), 2.0); // space between 10-11, 20-21
}

TEST(Bounds, ConstructionOrder)
{
   artificer::TimeBounds a(7, 3), b(3, 7);
   EXPECT_EQ(a, b);
   EXPECT_TRUE(a.HasValues());
   EXPECT_EQ(a.Union(b), b);
   EXPECT_EQ(a.Intersection(b), b);
   EXPECT_TRUE(a.Intersects(b));
}
