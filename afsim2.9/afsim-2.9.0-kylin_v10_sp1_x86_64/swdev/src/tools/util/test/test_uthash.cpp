// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2022 Radiance Technologies. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <gtest/gtest.h>

#include "UtHash.hpp"

TEST(UtHash, HashIsDeterministic) {
   {
      auto hash  = ut::Hash(5);
      auto hash2 = ut::Hash(5);
      EXPECT_EQ(hash, hash2);
   }

   {
      auto hash = ut::Hash(5, 10);
      auto hash2 = ut::Hash(5, 10);
      EXPECT_EQ(hash, hash2);
   }

   {
      auto hash  = ut::Hash(5, 10, 15);
      auto hash2 = ut::Hash(5, 10, 15);
      EXPECT_EQ(hash, hash2);

   }
}

TEST(UtHash, DifferentResults1vs2)
{
   // This might fail if a std::hash implementation causes us to.
   auto hash  = ut::Hash(5);
   auto hash2 = ut::Hash(5, 10);
   EXPECT_NE(hash, hash2);
}

TEST(UtHash, NonSymmetric)
{
   // This might fail if a std::hash implementation causes us to.
   auto hash  = ut::Hash(10, 5);
   auto hash2 = ut::Hash(5, 10);
   EXPECT_NE(hash, hash2);
}

