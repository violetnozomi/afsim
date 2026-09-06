// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <gtest/gtest.h>

#include "UtCast.hpp"
#include "WsfPProxyKey.hpp"

TEST(TestWsfPProxyKey, DefaultConstruction)
{
   WsfPProxyKey testKey;
   ASSERT_EQ(ut::npos, testKey.GetIndex());
   ASSERT_TRUE(testKey.GetMapKey().empty());
   ASSERT_FALSE(testKey.IsIndex());
   ASSERT_TRUE(testKey.IsString());
}

TEST(TestWsfPProxyKey, IntegerConstruction)
{
   WsfPProxyKey testKey(1);
   ASSERT_TRUE(testKey.IsIndex());
   ASSERT_FALSE(testKey.IsString());
   ASSERT_TRUE(testKey.GetMapKey().empty());
   ASSERT_EQ(size_t{1}, testKey.GetIndex());
}

TEST(TestWsfPProxyKey, StringConstruction)
{
   WsfPProxyKey testKey("testString");
   ASSERT_FALSE(testKey.IsIndex());
   ASSERT_TRUE(testKey.IsString());
   ASSERT_FALSE(testKey.GetMapKey().empty());
   ASSERT_EQ("testString", testKey.GetMapKey());
}

TEST(TestWsfPProxyKey, StringToString)
{
   WsfPProxyKey testKey("testString");
   ASSERT_EQ("testString", testKey.ToString());
}

TEST(TestWsfPProxyKey, IntToString)
{
   WsfPProxyKey testKey(42);
   ASSERT_EQ("42", testKey.ToString());
}

TEST(TestWsfPProxyKey, IntIntEquality)
{
   WsfPProxyKey test1(42);
   WsfPProxyKey test2(42);
   ASSERT_EQ(test1, test2);
   ASSERT_FALSE(test1 < test2);
   ASSERT_FALSE(test2 < test1);
}

TEST(TestWsfPProxyKey, IntIntLess)
{
   WsfPProxyKey test1(1024);
   WsfPProxyKey test2(512);
   ASSERT_FALSE(test1 == test2);
   ASSERT_FALSE(test1 < test2);
   ASSERT_TRUE(test2 < test1);
}

TEST(TestWsfPProxyKey, IntStringEqual)
{
   WsfPProxyKey testInt(1024);
   WsfPProxyKey testString("testString");
   ASSERT_FALSE(testInt == testString);
}

TEST(TestWsfPProxyKey, IntStringLess)
{
   WsfPProxyKey testInt(1024);
   WsfPProxyKey testString("testString");
   ASSERT_FALSE(testInt < testString);
   ASSERT_TRUE(testString < testInt);
}

TEST(TestWsfPProxyKey, StringStringEquality)
{
   WsfPProxyKey test1("testString");
   WsfPProxyKey test2("testString");
   ASSERT_EQ(test1, test2);
   ASSERT_FALSE(test1 < test2);
   ASSERT_FALSE(test2 < test1);
}

TEST(TestWsfPProxyKey, StringStringLess)
{
   WsfPProxyKey test1("test");
   WsfPProxyKey test2("testString");
   ASSERT_FALSE(test1 == test2);
   ASSERT_LT(test1, test2);
   ASSERT_FALSE(test2 < test1);
}
