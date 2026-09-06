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

#include "UtAlgorithm.hpp"

// Because the ranges version always uses the iterator version,
// no need to test both.
TEST(UtAlgorithm, StartsWith)
{
   using namespace ut::ranges;
   using vi = std::vector<int>;
   vi c0{};
   vi c1{1, 2, 3};
   vi c2{1, 2};
   vi c3{4, 5, 6};
   vi c4{1, 2, 3, 4};
   vi c5{2, 3};

   EXPECT_TRUE(starts_with(c0, c0));
   EXPECT_FALSE(starts_with(c0, c1));
   EXPECT_FALSE(starts_with(c0, c2));
   EXPECT_FALSE(starts_with(c0, c3));
   EXPECT_FALSE(starts_with(c0, c4));
   EXPECT_FALSE(starts_with(c0, c5));

   EXPECT_TRUE(starts_with(c1, c0));
   EXPECT_TRUE(starts_with(c1, c1));
   EXPECT_TRUE(starts_with(c1, c2));
   EXPECT_FALSE(starts_with(c1, c3));
   EXPECT_FALSE(starts_with(c1, c4));
   EXPECT_FALSE(starts_with(c1, c5));

   EXPECT_TRUE(starts_with(c2, c0));
   EXPECT_FALSE(starts_with(c2, c1));
   EXPECT_TRUE(starts_with(c2, c2));
   EXPECT_FALSE(starts_with(c2, c3));
   EXPECT_FALSE(starts_with(c2, c4));
   EXPECT_FALSE(starts_with(c2, c5));

   EXPECT_TRUE(starts_with(c3, c0));
   EXPECT_FALSE(starts_with(c3, c1));
   EXPECT_FALSE(starts_with(c3, c2));
   EXPECT_TRUE(starts_with(c3, c3));
   EXPECT_FALSE(starts_with(c3, c4));
   EXPECT_FALSE(starts_with(c3, c5));

   EXPECT_TRUE(starts_with(c4, c0));
   EXPECT_TRUE(starts_with(c4, c1));
   EXPECT_TRUE(starts_with(c4, c2));
   EXPECT_FALSE(starts_with(c4, c3));
   EXPECT_TRUE(starts_with(c4, c4));
   EXPECT_FALSE(starts_with(c4, c5));

   EXPECT_TRUE(starts_with(c5, c0));
   EXPECT_FALSE(starts_with(c5, c1));
   EXPECT_FALSE(starts_with(c5, c2));
   EXPECT_FALSE(starts_with(c5, c3));
   EXPECT_FALSE(starts_with(c5, c4));
   EXPECT_TRUE(starts_with(c5, c5));
}

TEST(UtAlgorithm, StringStartsWith)
{
   const char         cstr[] = "Hello, World!";
   const std::string& str    = "Hello, World!";

   EXPECT_TRUE(ut::string_starts_with(cstr, "Hello"));
   EXPECT_TRUE(ut::string_starts_with(cstr, ""));
   EXPECT_TRUE(ut::string_starts_with(cstr, str));
   EXPECT_FALSE(ut::string_starts_with(cstr, "hello"));

   EXPECT_TRUE(ut::string_starts_with(str, "Hello"));
   EXPECT_TRUE(ut::string_starts_with(str, ""));
   EXPECT_TRUE(ut::string_starts_with(str, cstr));
   EXPECT_FALSE(ut::string_starts_with(str, "hello"));

   EXPECT_TRUE(ut::string_starts_with("", ""));
   EXPECT_TRUE(ut::string_starts_with("Hello, World!", "Hello, World!"));
   EXPECT_TRUE(ut::string_starts_with("Hello, World!", "Hello"));
   EXPECT_FALSE(ut::string_starts_with("Hello, World!", "hello"));
}

// Because the ranges version always uses the iterator version,
// no need to test both.
TEST(UtAlgorithm, EndsWith)
{
   using namespace ut::ranges;
   using vi = std::vector<int>;
   vi c0{};
   vi c1{1, 2, 3};
   vi c2{1, 2};
   vi c3{4, 5, 6};
   vi c4{1, 2, 3, 4};
   vi c5{2, 3};

   EXPECT_TRUE(ends_with(c0, c0));
   EXPECT_FALSE(ends_with(c0, c1));
   EXPECT_FALSE(ends_with(c0, c2));
   EXPECT_FALSE(ends_with(c0, c3));
   EXPECT_FALSE(ends_with(c0, c4));
   EXPECT_FALSE(ends_with(c0, c5));

   EXPECT_TRUE(ends_with(c1, c0));
   EXPECT_TRUE(ends_with(c1, c1));
   EXPECT_FALSE(ends_with(c1, c2));
   EXPECT_FALSE(ends_with(c1, c3));
   EXPECT_FALSE(ends_with(c1, c4));
   EXPECT_TRUE(ends_with(c1, c5));

   EXPECT_TRUE(ends_with(c2, c0));
   EXPECT_FALSE(ends_with(c2, c1));
   EXPECT_TRUE(ends_with(c2, c2));
   EXPECT_FALSE(ends_with(c2, c3));
   EXPECT_FALSE(ends_with(c2, c4));
   EXPECT_FALSE(ends_with(c2, c5));

   EXPECT_TRUE(ends_with(c3, c0));
   EXPECT_FALSE(ends_with(c3, c1));
   EXPECT_FALSE(ends_with(c3, c2));
   EXPECT_TRUE(ends_with(c3, c3));
   EXPECT_FALSE(ends_with(c3, c4));
   EXPECT_FALSE(ends_with(c3, c5));

   EXPECT_TRUE(ends_with(c4, c0));
   EXPECT_FALSE(ends_with(c4, c1));
   EXPECT_FALSE(ends_with(c4, c2));
   EXPECT_FALSE(ends_with(c4, c3));
   EXPECT_TRUE(ends_with(c4, c4));
   EXPECT_FALSE(ends_with(c4, c5));

   EXPECT_TRUE(ends_with(c5, c0));
   EXPECT_FALSE(ends_with(c5, c1));
   EXPECT_FALSE(ends_with(c5, c2));
   EXPECT_FALSE(ends_with(c5, c3));
   EXPECT_FALSE(ends_with(c5, c4));
   EXPECT_TRUE(ends_with(c5, c5));
}

TEST(UtAlgorithm, StringEndsWith)
{
   const char         cstr[] = "Hello, World!";
   const std::string& str    = "Hello, World!";

   EXPECT_TRUE(ut::string_ends_with(cstr, "World!"));
   EXPECT_TRUE(ut::string_ends_with(cstr, ""));
   EXPECT_TRUE(ut::string_ends_with(cstr, str));
   EXPECT_FALSE(ut::string_ends_with(cstr, "world!"));

   EXPECT_TRUE(ut::string_ends_with(str, "World!"));
   EXPECT_TRUE(ut::string_ends_with(str, ""));
   EXPECT_TRUE(ut::string_ends_with(str, cstr));
   EXPECT_FALSE(ut::string_ends_with(str, "world!"));

   EXPECT_TRUE(ut::string_ends_with("", ""));
   EXPECT_TRUE(ut::string_ends_with("Hello, World!", "Hello, World!"));
   EXPECT_TRUE(ut::string_ends_with("Hello, World!", "World!"));
   EXPECT_FALSE(ut::string_ends_with("Hello, World!", "world!"));
}
