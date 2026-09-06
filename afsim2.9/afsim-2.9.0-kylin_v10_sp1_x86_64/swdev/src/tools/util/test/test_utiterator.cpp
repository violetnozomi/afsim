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

#include <algorithm>
#include <iterator>
#include <vector>

#include <gtest/gtest.h>

#include "UtIterator.hpp"

TEST(UtIterator, MakeReverseIterator)
{
   std::vector<int> v_f{1, 3, 10, 8, 22};
   std::vector<int> v_r;

   auto rev_begin = ut::make_reverse_iterator(v_f.end());
   auto rev_end   = ut::make_reverse_iterator(v_f.begin());

   for (auto it = rev_begin; it != rev_end; ++it)
   {
      v_r.push_back(*it);
   }

   std::reverse(std::begin(v_f), std::end(v_f));

   EXPECT_EQ(v_f, v_r);
}