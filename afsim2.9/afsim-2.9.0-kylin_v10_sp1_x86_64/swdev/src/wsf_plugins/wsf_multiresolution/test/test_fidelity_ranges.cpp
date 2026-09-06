// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright (C) 2021 Stellar Science; U.S. Government has Unlimited Rights.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <gtest/gtest.h>

#include "FidelityRange.hpp"

namespace
{
TEST(FidelityRange, Contains)
{
   constexpr wsf::multiresolution::FidelityRange range{0.125, 0.25};
   EXPECT_TRUE(range.Contains(0.2));
   EXPECT_TRUE(range.Contains(0.125));
   EXPECT_FALSE(range.Contains(0.25)); // Upper bound is non-inclusive if < 1
   EXPECT_FALSE(range.Contains(0.1));
   EXPECT_FALSE(range.Contains(0.5));

   // Check handling of an upper bound of 1.
   // It's expected to be included in range, contrary to upper bounds < 1.
   constexpr wsf::multiresolution::FidelityRange defaultRange; // [0, 1]
   EXPECT_TRUE(defaultRange.Contains(1.0));
}

TEST(FidelityRange, Overlap)
{
   constexpr wsf::multiresolution::FidelityRange range1{0.125, 0.25};
   // Overlaps self
   EXPECT_TRUE(range1.Overlaps(range1));

   // Doesn't overlap when end points are equal
   constexpr wsf::multiresolution::FidelityRange range2{0.25, 0.5};
   EXPECT_FALSE(range1.Overlaps(range2));
   EXPECT_FALSE(range2.Overlaps(range1));

   // Partial overlap
   constexpr wsf::multiresolution::FidelityRange range3{0.0, 0.2};
   EXPECT_TRUE(range3.Overlaps(range1));
   EXPECT_TRUE(range1.Overlaps(range3));
   // Gap between ranges
   EXPECT_FALSE(range3.Overlaps(range2));
   EXPECT_FALSE(range2.Overlaps(range3));

   // Complete covering
   constexpr wsf::multiresolution::FidelityRange range4; // [0,1]
   EXPECT_TRUE(range4.Overlaps(range1));
   EXPECT_TRUE(range1.Overlaps(range4));
   EXPECT_TRUE(range4.Overlaps(range2));
   EXPECT_TRUE(range2.Overlaps(range4));
   EXPECT_TRUE(range4.Overlaps(range3));
   EXPECT_TRUE(range3.Overlaps(range4));
}

TEST(FidelityRange, Disjoint)
{
   // Same tests as for Overlap, but expect opposite result
   constexpr wsf::multiresolution::FidelityRange range1{0.125, 0.25};
   // Not disjoint with self
   EXPECT_FALSE(range1.IsDisjoint(range1));

   // Is disjoint when end points are equal
   constexpr wsf::multiresolution::FidelityRange range2{0.25, 0.5};
   EXPECT_TRUE(range1.IsDisjoint(range2));
   EXPECT_TRUE(range2.IsDisjoint(range1));

   // Partial overlap
   constexpr wsf::multiresolution::FidelityRange range3{0.0, 0.2};
   EXPECT_FALSE(range3.IsDisjoint(range1));
   EXPECT_FALSE(range1.IsDisjoint(range3));
   // Gap between ranges
   EXPECT_TRUE(range3.IsDisjoint(range2));
   EXPECT_TRUE(range2.IsDisjoint(range3));

   // Complete covering
   constexpr wsf::multiresolution::FidelityRange range4; // [0,1]
   EXPECT_FALSE(range4.IsDisjoint(range1));
   EXPECT_FALSE(range1.IsDisjoint(range4));
   EXPECT_FALSE(range4.IsDisjoint(range2));
   EXPECT_FALSE(range2.IsDisjoint(range4));
   EXPECT_FALSE(range4.IsDisjoint(range3));
   EXPECT_FALSE(range3.IsDisjoint(range4));
}
} // namespace
