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

#include "UtStacktrace.hpp"

#ifndef _WIN32 // If running Linux

#define RUN_UT_STACKTRACE_TEST
#define NO_INLINE_ATTRIBUTE __attribute__((noinline))
constexpr const char* cFOO_NAME = "foo(unsigned long, unsigned long)";
constexpr const char* cBAR_NAME = "bar(unsigned long, unsigned long)";
constexpr const char* cZAP_NAME = "zap(unsigned long, unsigned long)";

#elif defined(_DEBUG) // Else if running Windows debug (also works with RelWithDebInfo)

#define RUN_UT_STACKTRACE_TEST
#define NO_INLINE_ATTRIBUTE __declspec(noinline)
constexpr const char* cFOO_NAME = "foo";
constexpr const char* cBAR_NAME = "bar";
constexpr const char* cZAP_NAME = "zap";

#endif

#ifdef RUN_UT_STACKTRACE_TEST

#define NO_INLINE \
   volatile std::vector<int> vi { 1, 2, 3, 4, 5 }

static ut::stacktrace sTrace;

void NO_INLINE_ATTRIBUTE foo(std::size_t aSkip, std::size_t aLimit)
{
   NO_INLINE;
   sTrace = ut::stacktrace::current(aSkip, aLimit);
}

void NO_INLINE_ATTRIBUTE bar(std::size_t aSkip, std::size_t aLimit)
{
   NO_INLINE;
   foo(aSkip, aLimit);
}

void NO_INLINE_ATTRIBUTE zap(std::size_t aSkip, std::size_t aLimit)
{
   NO_INLINE;
   bar(aSkip, aLimit);
}

TEST(UtStacktrace, Current)
{
   sTrace = ut::stacktrace();

   EXPECT_TRUE(sTrace.empty());
   EXPECT_EQ(sTrace.size(), 0);
   EXPECT_THROW(sTrace.at(0), std::out_of_range);

   zap(0, 1000);

   EXPECT_FALSE(sTrace.empty());
   EXPECT_GE(sTrace.size(), 4);
   EXPECT_THROW(sTrace.at(sTrace.size()), std::out_of_range);

   EXPECT_TRUE(sTrace[0]);
   EXPECT_EQ(sTrace[0].description(), cFOO_NAME);
   EXPECT_TRUE(sTrace[1]);
   EXPECT_EQ(sTrace[1].description(), cBAR_NAME);
   EXPECT_TRUE(sTrace[2]);
   EXPECT_EQ(sTrace[2].description(), cZAP_NAME);
   EXPECT_TRUE(sTrace[3]);

   auto st = ut::stacktrace::current();
   EXPECT_EQ(sTrace.size(), st.size() + 3);
}

TEST(UtStacktrace, Current2)
{
   sTrace = ut::stacktrace();

   EXPECT_TRUE(sTrace.empty());
   EXPECT_EQ(sTrace.size(), 0);
   EXPECT_THROW(sTrace.at(0), std::out_of_range);

   zap(1, 1000);

   EXPECT_FALSE(sTrace.empty());
   EXPECT_GE(sTrace.size(), 3);
   EXPECT_THROW(sTrace.at(sTrace.size()), std::out_of_range);

   EXPECT_TRUE(sTrace[0]);
   EXPECT_EQ(sTrace[0].description(), cBAR_NAME);
   EXPECT_TRUE(sTrace[1]);
   EXPECT_EQ(sTrace[1].description(), cZAP_NAME);
   EXPECT_TRUE(sTrace[2]);
}

TEST(UtStacktrace, Current3)
{
   sTrace = ut::stacktrace();

   EXPECT_TRUE(sTrace.empty());
   EXPECT_EQ(sTrace.size(), 0);
   EXPECT_THROW(sTrace.at(0), std::out_of_range);

   zap(1, 2);

   EXPECT_FALSE(sTrace.empty());
   EXPECT_EQ(sTrace.size(), 2);
   EXPECT_THROW(sTrace.at(sTrace.size()), std::out_of_range);

   EXPECT_TRUE(sTrace[0]);
   EXPECT_EQ(sTrace[0].description(), cBAR_NAME);
   EXPECT_TRUE(sTrace[1]);
   EXPECT_EQ(sTrace[1].description(), cZAP_NAME);
}

#endif
