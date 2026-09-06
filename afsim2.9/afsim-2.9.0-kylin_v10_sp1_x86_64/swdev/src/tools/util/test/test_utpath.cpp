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

#include "UtPath.hpp"

namespace
{
// Helper function to ease getting string representation of a path
std::string s(const UtPath& aPath)
{
   return aPath.GetSystemPath();
}
// Helper function to ease cross-platform constant path values
std::string c(const std::string& aVal)
{
   return s(UtPath{aVal});
}
} // namespace

TEST(UtPath, Up)
{
   // There is no fundamental difference with an un-Stat()ed string path
   // between file and directory, but test with what appears to be a file path
   // and what appears to be a directory path.
   UtPath test{"foo/bar/example.txt"};
   EXPECT_EQ(s(test), c("foo/bar/example.txt"));
   test.Up();
   EXPECT_EQ(s(test), c("foo/bar"));
   test.Up();
   EXPECT_EQ(s(test), c("foo"));

   UtPath spacesTest{"foo fighters/bar stool/shining example.temp.txt"};
   EXPECT_EQ(s(spacesTest), c("foo fighters/bar stool/shining example.temp.txt"));
   spacesTest.Up();
   EXPECT_EQ(s(spacesTest), c("foo fighters/bar stool"));
   spacesTest.Up();
   EXPECT_EQ(s(spacesTest), c("foo fighters"));
}

TEST(UtPath, UpSingleDot)
{
   UtPath selfdir{"."};
   EXPECT_EQ(s(selfdir), c("."));
   selfdir.Up().Up();
   EXPECT_EQ(s(selfdir), c(".."));
}

TEST(UtPath, UpOnEmpty)
{
   // Test Up on path originally empty
   UtPath empty{};
   EXPECT_EQ(s(empty), c(""));
   empty.Up();
   EXPECT_EQ(s(empty), c(".."));
   empty.Up();
   EXPECT_EQ(s(empty), c("../.."));

   // Test Up on path going through empty
   UtPath transition{"foo"};
   EXPECT_EQ(s(transition), "foo");
   transition.Up();
   EXPECT_EQ(s(transition), "");
   transition.Up();
   EXPECT_EQ(s(transition), "..");
}

// Test effectual usage of both expected modes of Up(), in-place and returning a modified self.
TEST(UtPath, UpUsageVariation)
{
   UtPath inPlaceTest{"foo/bar/some/deep/multi/nested/path/example.txt"};
   inPlaceTest.Up();
   inPlaceTest.Up();
   inPlaceTest.Up();
   inPlaceTest.Up();
   EXPECT_EQ(s(inPlaceTest), c("foo/bar/some/deep"));

   UtPath chainTest{"foo/bar/some/deep/multi/nested/path/example.txt"};
   // Ensure that chained usage also modifies the original, not just the
   // returned value (which may be discarded).
   EXPECT_EQ(s(chainTest.Up()), c("foo/bar/some/deep/multi/nested/path"));
   EXPECT_EQ(s(chainTest.Up()), c("foo/bar/some/deep/multi/nested"));
   EXPECT_EQ(s(chainTest.Up().Up().Up().Up()), c("foo/bar"));
}

TEST(UtPath, GetRelativePath)
{
   {
      // This usage gets the path a/b/c/d/e/f relative to path a/b/c
      UtPath testReferencePath{ "a/b/c" };
      UtPath testPath{ "a/b/c/d/e/f" };
      UtPath testRelativePath{ testReferencePath.GetRelativePath(testPath) };
      EXPECT_EQ(testRelativePath.GetNormalizedPath(), UtPath{ "d/e/f" }.GetNormalizedPath());
   }

   {
      // This usage gets the path a/b/c relative to path a/b/c/d/e/f
      UtPath testReferencePath{ "a/b/c/d/e/f" };
      UtPath testPath{ "a/b/c" };
      UtPath testRelativePath{ testReferencePath.GetRelativePath(testPath) };
      EXPECT_EQ(testRelativePath.GetNormalizedPath(), UtPath{ "../../../" }.GetNormalizedPath());
   }
}
