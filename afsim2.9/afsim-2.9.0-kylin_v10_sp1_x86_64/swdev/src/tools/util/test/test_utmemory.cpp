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

#include "UtMemory.hpp"

namespace
{
class SomeCloneable
{
public:
   SomeCloneable* Clone() const { return new SomeCloneable(*this); }
   bool           operator==(const SomeCloneable& rhs) const { return this == &rhs; }
   bool           operator!=(const SomeCloneable& rhs) const { return !(*this == rhs); }
};
} // namespace


TEST(Clone, RawPointer)
{
   SomeCloneable  original;
   SomeCloneable* originalPtr = &original;
   auto           cloned      = ut::clone(originalPtr);
   ASSERT_NE(original, *cloned);
}

TEST(Clone, Reference)
{
   SomeCloneable original;
   auto          cloned = ut::clone(original);
   ASSERT_NE(original, *cloned);
}

TEST(Clone, UniquePtr)
{
   auto original = ut::make_unique<SomeCloneable>();
   auto cloned   = ut::clone(original);
   ASSERT_NE(*original, *cloned);
}

TEST(Clone, SharedPtr)
{
   auto original = std::make_shared<SomeCloneable>();
   auto cloned   = ut::clone(original);
   ASSERT_NE(*original, *cloned);
}
