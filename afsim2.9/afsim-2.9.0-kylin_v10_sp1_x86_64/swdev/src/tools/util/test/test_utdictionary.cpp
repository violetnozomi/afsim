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

#include "UtDictionary.hpp"

// Test that a global dictionary can be self-created.
TEST(UtDictionary, GlobalDictionarySelfCreate)
{
   UtDictionary::Destroy(); // Ensure the global dictionary does not exist
   EXPECT_FALSE(UtDictionary::HasGlobalDictionary());
   UtDictionary::GetDictionary(); // So it can be self-created
   EXPECT_TRUE(UtDictionary::HasGlobalDictionary());
   UtDictionary::Destroy();
   EXPECT_FALSE(UtDictionary::HasGlobalDictionary());
}

// Test that a local heap-allocated dictionary can be set as the global dictionary.
TEST(UtDictionary, HasGlobalDictionary)
{
   UtDictionary::Destroy(); // Ensure the global dictionary does not exist
   EXPECT_FALSE(UtDictionary::HasGlobalDictionary());
   UtDictionary::SetDictionary(new UtDictionary); // So it can be set
   EXPECT_TRUE(UtDictionary::HasGlobalDictionary());
   UtDictionary::Destroy();
   EXPECT_FALSE(UtDictionary::HasGlobalDictionary());
}

// Test that a string ID does not exist in the global dictionary.
TEST(UtDictionary, GlobalDictionaryUnknownString)
{
   UtDictionary::SetDictionary(new UtDictionary);
   EXPECT_EQ(UtDictionary::GetDictionary().GetString(-1), "<unknown>");
   EXPECT_EQ(UtDictionary::GetDictionary().GetString(std::numeric_limits<int>::max()), "<unknown>");
   UtDictionary::Destroy();
}

// Test that a new global dictionary has its default values.
TEST(UtDictionary, DefaultGlobalDictionary)
{
   UtDictionary::SetDictionary(new UtDictionary);
   EXPECT_EQ(UtDictionary::GetDictionary().GetString(0), "");
   EXPECT_EQ(UtDictionary::GetDictionary().GetCount(), 1);
   UtDictionary::Destroy();
}

// Test that a string does not exist after resetting the global dictionary.
TEST(UtDictionary, ResetGlobalDictionary)
{
   UtDictionary::SetDictionary(new UtDictionary);
   UtDictionary::GetDictionary().GetNumber("some-string");
   EXPECT_TRUE(UtDictionary::GetDictionary().Exists("some-string"));
   UtDictionary::SetDictionary(new UtDictionary);
   EXPECT_FALSE(UtDictionary::GetDictionary().Exists("some-string"));
   UtDictionary::Destroy();
}

// Test inserting a string into a new global dictionary.
TEST(UtDictionary, GetGlobalDictionaryStringId)
{
   UtDictionary::SetDictionary(new UtDictionary);
   EXPECT_EQ(UtDictionary::GetDictionary().GetNumber("some-string"), 1);
   EXPECT_TRUE(UtDictionary::GetDictionary().Exists("some-string"));
   EXPECT_EQ(UtDictionary::GetDictionary().GetCount(), 2);
   UtDictionary::Destroy();
}

// Test that a non-empty local dictionary can replace the global dictionary.
TEST(UtDictionary, ReplaceGlobalDictionary)
{
   UtDictionary::GetDictionary().GetNumber("some-string");
   UtDictionary* dictionary = new UtDictionary;
   dictionary->GetNumber("another-string");
   UtDictionary::SetDictionary(dictionary);
   EXPECT_TRUE(UtDictionary::GetDictionary().Exists("another-string"));
   EXPECT_FALSE(UtDictionary::GetDictionary().Exists("some-string"));
   EXPECT_EQ(UtDictionary::GetDictionary().GetCount(), 2);
   UtDictionary::Destroy();
   dictionary = nullptr;
}
