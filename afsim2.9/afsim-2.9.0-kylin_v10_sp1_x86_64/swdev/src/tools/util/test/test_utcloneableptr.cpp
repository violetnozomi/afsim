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

#include <sstream>

#include <gtest/gtest.h>

#include "UtCloneablePtr.hpp"

namespace
{
class Cloneable
{
public:
   Cloneable() { oss << "dc"; }                 // default construct
   Cloneable(const Cloneable&) { oss << "cc"; } // copy construct
   ~Cloneable() { oss << "~c"; }                // destruct cloneable

   std::unique_ptr<Cloneable> Clone() const
   {
      oss << "cl"; // clone
      return ut::make_unique<Cloneable>(*this);
   }

   static std::string str() noexcept { return oss.str(); }

   static void Clear() { oss.str(std::string()); }

private:
   static std::ostringstream oss;
};

class DerivedCloneable : public Cloneable
{
};

std::ostringstream Cloneable::oss;
} // namespace

using CloneablePtr        = ut::CloneablePtr<Cloneable>;
using DerivedCloneablePtr = ut::CloneablePtr<DerivedCloneable>;

// *************
// Constructors
// *************

TEST(UtCloneablePtr, DefaultConstruct)
{
   CloneablePtr cp;
   EXPECT_FALSE(cp);
   EXPECT_EQ(Cloneable::str(), "");
}

TEST(UtCloneablePtr, NullptrConstruct)
{
   CloneablePtr cp{nullptr};
   EXPECT_FALSE(cp);
   EXPECT_EQ(Cloneable::str(), "");
}

TEST(UtCloneablePtr, RawPointerConstruct)
{
   Cloneable::Clear();
   {
      CloneablePtr cp{new Cloneable};
      EXPECT_TRUE(cp);
   }
   EXPECT_EQ(Cloneable::str(), "dc~c");
}

TEST(UtCloneablePtr, UniquePtrConstruct)
{
   Cloneable::Clear();
   {
      auto         up = ut::make_unique<Cloneable>();
      CloneablePtr cp{std::move(up)};
      EXPECT_TRUE(cp);
      EXPECT_FALSE(up);
   }

   {
      auto         dup = ut::make_unique<DerivedCloneable>();
      CloneablePtr cp{std::move(dup)};
      EXPECT_TRUE(cp);
      EXPECT_FALSE(dup);
   }
   EXPECT_EQ(Cloneable::str(), "dc~cdc~c");
}

TEST(UtCloneablePtr, CopyConstruct)
{
   Cloneable::Clear();
   // Copy construct from null CloneablePtr
   {
      CloneablePtr cp;
      CloneablePtr cpy{cp};
      EXPECT_FALSE(cpy);
   }
   EXPECT_EQ(Cloneable::str(), "");

   // Copy construct from CloneablePtr
   {
      CloneablePtr cp{ut::make_unique<Cloneable>()};
      CloneablePtr cpy{cp};
      EXPECT_TRUE(cp);
      EXPECT_TRUE(cpy);
   }
   EXPECT_EQ(Cloneable::str(), "dcclcc~c~c");

   Cloneable::Clear();
   // Copy construct from DerivedCloneablePtr
   {
      DerivedCloneablePtr dcp{ut::make_unique<DerivedCloneable>()};
      CloneablePtr        cpy{dcp};
      EXPECT_TRUE(dcp);
      EXPECT_TRUE(cpy);
   }
   EXPECT_EQ(Cloneable::str(), "dcclcc~c~c");
}

TEST(UtCloneablePtr, MoveConstruct)
{
   Cloneable::Clear();
   // Move construct from null CloneablePtr
   {
      CloneablePtr cp;
      CloneablePtr mov{std::move(cp)};
      EXPECT_FALSE(mov);
   }
   EXPECT_EQ(Cloneable::str(), "");

   // Move construct from CloneablePtr
   {
      CloneablePtr cp{ut::make_unique<Cloneable>()};
      CloneablePtr mov{std::move(cp)};
      EXPECT_TRUE(mov);
      EXPECT_FALSE(cp);
   }

   // Move construct from DerivedCloneablePtr
   {
      DerivedCloneablePtr dcp{ut::make_unique<DerivedCloneable>()};
      CloneablePtr        mov{std::move(dcp)};
      EXPECT_TRUE(mov);
      EXPECT_FALSE(dcp);
   }
   EXPECT_EQ(Cloneable::str(), "dc~cdc~c");
}

// ***********
// Assignment
// ***********

TEST(UtCloneablePtr, CopyAssignment)
{
   Cloneable::Clear();
   // Copy assign from null CloneablePtr...
   {
      // ... to null CloneablePtr
      CloneablePtr cp;
      CloneablePtr cpy;
      cpy = cp;
      EXPECT_FALSE(cpy);
   }
   EXPECT_EQ(Cloneable::str(), "");

   {
      // ... to non-null CloneablePtr
      CloneablePtr cp;
      CloneablePtr cpy{ut::make_unique<Cloneable>()};
      cpy = cp;
      EXPECT_FALSE(cpy);
   }
   EXPECT_EQ(Cloneable::str(), "dc~c");

   Cloneable::Clear();
   // Copy assign from CloneablePtr...
   {
      // ... to null CloneablePtr
      CloneablePtr cp{ut::make_unique<Cloneable>()};
      CloneablePtr cpy;
      cpy = cp;
      EXPECT_TRUE(cp);
      EXPECT_TRUE(cpy);
   }
   EXPECT_EQ(Cloneable::str(), "dcclcc~c~c");

   Cloneable::Clear();
   {
      // ... to non-null CloneablePtr
      CloneablePtr cp{ut::make_unique<Cloneable>()};
      CloneablePtr cpy{ut::make_unique<Cloneable>()};
      cpy = cp;
      EXPECT_TRUE(cp);
      EXPECT_TRUE(cpy);
   }
   EXPECT_EQ(Cloneable::str(), "dcdcclcc~c~c~c");

   Cloneable::Clear();
   // Copy assign from DerivedCloneablePtr
   {
      DerivedCloneablePtr dcp{ut::make_unique<DerivedCloneable>()};
      CloneablePtr        cpy;
      cpy = dcp;
      EXPECT_TRUE(dcp);
      EXPECT_TRUE(cpy);
   }
   EXPECT_EQ(Cloneable::str(), "dcclcc~c~c");
}

TEST(UtCloneablePtr, MoveAssignment)
{
   Cloneable::Clear();
   // Move assign from null CloneablePtr...
   {
      // ... to null CloneablePtr
      CloneablePtr cp;
      CloneablePtr mov;
      mov = std::move(cp);
      EXPECT_FALSE(mov);
   }
   EXPECT_EQ(Cloneable::str(), "");

   {
      // ... to non-null CloneablePtr
      CloneablePtr cp;
      CloneablePtr mov{ut::make_unique<Cloneable>()};
      mov = std::move(cp);
      EXPECT_FALSE(mov);
   }
   EXPECT_EQ(Cloneable::str(), "dc~c");

   Cloneable::Clear();
   // Move assign from CloneablePtr...
   {
      // ... to null CloneablePtr
      CloneablePtr cp{ut::make_unique<Cloneable>()};
      CloneablePtr mov;
      mov = std::move(cp);
      EXPECT_TRUE(mov);
      EXPECT_FALSE(cp);
   }
   EXPECT_EQ(Cloneable::str(), "dc~c");

   Cloneable::Clear();
   {
      // ... to non-null CloneablePtr
      CloneablePtr cp{ut::make_unique<Cloneable>()};
      CloneablePtr mov{ut::make_unique<Cloneable>()};
      mov = std::move(cp);
      EXPECT_TRUE(mov);
      EXPECT_FALSE(cp);
   }
   EXPECT_EQ(Cloneable::str(), "dcdc~c~c");

   Cloneable::Clear();
   // Move assign from DerivedCloneablePtr
   {
      DerivedCloneablePtr dcp{ut::make_unique<DerivedCloneable>()};
      CloneablePtr        mov;
      mov = std::move(dcp);
      EXPECT_TRUE(mov);
      EXPECT_FALSE(dcp);
   }
   EXPECT_EQ(Cloneable::str(), "dc~c");

   Cloneable::Clear();
   // Move assign from unique_ptr
   {
      auto         up = ut::make_unique<Cloneable>();
      CloneablePtr mov;
      mov = std::move(up);
      EXPECT_TRUE(mov);
      EXPECT_FALSE(up);
   }
   EXPECT_EQ(Cloneable::str(), "dc~c");
}

TEST(UtCloneablePtr, AssignFromRawPointer)
{
   Cloneable::Clear();
   {
      CloneablePtr cp;
      cp = new Cloneable;
      EXPECT_TRUE(cp);
   }
   EXPECT_EQ(Cloneable::str(), "dc~c");
}

TEST(UtCloneablePtr, AssignFromNullptr)
{
   Cloneable::Clear();
   {
      CloneablePtr cp;
      cp = nullptr;
      EXPECT_FALSE(cp);
   }
   EXPECT_EQ(Cloneable::str(), "");
}

// **********
// Modifiers
// **********

TEST(UtCloneablePtr, Release)
{
   Cloneable::Clear();
   {
      auto                       p = new Cloneable;
      CloneablePtr               cp{p};
      std::unique_ptr<Cloneable> up{cp.release()};
      EXPECT_FALSE(cp);
      EXPECT_EQ(p, up.get());
   }
   EXPECT_EQ(Cloneable::str(), "dc~c");
}

TEST(UtCloneablePtr, Reset)
{
   Cloneable::Clear();
   {
      CloneablePtr cp{ut::make_unique<Cloneable>()};
      // reset with nullptr
      cp.reset();
      EXPECT_FALSE(cp);
      // reset with raw pointer to Cloneable
      auto p = new Cloneable;
      cp.reset(p);
      EXPECT_TRUE(cp);
      EXPECT_EQ(cp.get(), p);
   }
   EXPECT_EQ(Cloneable::str(), "dc~cdc~c");
}

TEST(UtCloneablePtr, Swap)
{
   Cloneable::Clear();
   {
      CloneablePtr cp1;
      CloneablePtr cp2{ut::make_unique<Cloneable>()};
      cp1.swap(cp2);
      EXPECT_TRUE(cp1);
      EXPECT_FALSE(cp2);
      std::swap(cp1, cp2);
      EXPECT_TRUE(cp2);
      EXPECT_FALSE(cp1);
   }
   EXPECT_EQ(Cloneable::str(), "dc~c");
}

TEST(UtCloneablePtr, ConversionToUniquePtr)
{
   Cloneable::Clear();
   {
      DerivedCloneablePtr        dcp{ut::make_unique<DerivedCloneable>()};
      auto                       p  = dcp.get();
      std::unique_ptr<Cloneable> up = std::move(dcp);
      EXPECT_TRUE(up);
      EXPECT_FALSE(dcp);
      EXPECT_EQ(p, up.get());
   }
   EXPECT_EQ(Cloneable::str(), "dc~c");
}
