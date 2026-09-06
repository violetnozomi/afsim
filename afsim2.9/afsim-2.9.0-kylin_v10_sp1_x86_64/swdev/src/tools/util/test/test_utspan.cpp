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

#include <array>
#include <vector>

#include <gtest/gtest.h>

#include "UtSpan.hpp"

TEST(UtSpan, FromVector)
{
   std::vector<size_t> v = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
   ut::span<size_t>    testSpan(v);
   ASSERT_EQ(testSpan.begin(), &v[0]);
   ASSERT_EQ(testSpan.end(), &v[0] + v.size());
}

TEST(UtSpan, FromArray)
{
   std::array<int, 10> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
   ut::span<int>       testSpan(a);
   ASSERT_EQ(testSpan.begin(), &a[0]);
   ASSERT_EQ(testSpan.end(), &a[0] + a.size());
}

TEST(UtSpan, FromCArray)
{
   size_t           testArray[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
   ut::span<size_t> testSpan(testArray);
   ASSERT_EQ(testArray, testSpan.begin());
   ASSERT_EQ(testArray + 10, testSpan.end());
}

TEST(UtSpan, First)
{
   std::vector<size_t> v = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
   ut::span<size_t>    testSpan(v);
   auto                first = testSpan.first(3);
   ASSERT_EQ(first[0], 0);
   ASSERT_EQ(first[1], 1);
   ASSERT_EQ(first[3], 3);
}

TEST(UtSpan, FirstTemplate)
{
   std::vector<size_t> v = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
   ut::span<size_t>    testSpan(v);
   auto                first = testSpan.first<4>();
   for (size_t i = 0; i < 4; ++i)
   {
      ASSERT_EQ(first[i], i);
   }
}

TEST(UtSpan, Last)
{
   std::vector<size_t> v = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
   ut::span<size_t>    testSpan(v);
   auto                last = testSpan.last(5);
   for (size_t i = 0; i < 5; ++i)
   {
      ASSERT_EQ(v[i + 5], last[i]);
   }
}

TEST(UtSpan, LastTemplate)
{
   std::vector<size_t> v = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
   ut::span<size_t>    testSpan(v);
   auto                last = testSpan.last<5>();
   for (size_t i = 0; i < 5; ++i)
   {
      ASSERT_EQ(v[i + 5], last[i]);
   }
}

TEST(UtSpan, SubSpan)
{
   std::vector<size_t> v = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
   ut::span<size_t>    testSpan(v);
   auto                subSpan = testSpan.subspan(2, 4);
   for (size_t i = 0; i < 4; ++i)
   {
      ASSERT_EQ(i + 2, subSpan[i]);
   }
   ASSERT_EQ(4, subSpan.size());
}

TEST(UtSpan, SubSpanTemplate)
{
   std::vector<size_t> v = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
   ut::span<size_t>    testSpan(v);
   auto                subSpan = testSpan.subspan<2, 4>();
   for (size_t i = 0; i < 4; ++i)
   {
      ASSERT_EQ(i + 2, subSpan[i]);
   }
   ASSERT_EQ(4, subSpan.size());
}

TEST(UtSpan, SubCArray)
{
   int           carray[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
   ut::span<int> subSpan(carray, carray + 4);
   ASSERT_EQ(4, subSpan.size());
   for (size_t i = 0; i < subSpan.size(); ++i)
   {
      ASSERT_EQ(i, subSpan[i]);
   }
}

TEST(UtSpan, Get)
{
   std::array<size_t, 10> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
   ut::span<size_t, 10>   testSpan(a);
   ASSERT_EQ(ut::get<0>(testSpan), 0);
   ASSERT_EQ(ut::get<1>(testSpan), 1);
   ASSERT_EQ(ut::get<2>(testSpan), 2);
   ASSERT_EQ(ut::get<3>(testSpan), 3);
   ASSERT_EQ(ut::get<4>(testSpan), 4);
   ASSERT_EQ(ut::get<5>(testSpan), 5);
   ASSERT_EQ(ut::get<6>(testSpan), 6);
   ASSERT_EQ(ut::get<7>(testSpan), 7);
   ASSERT_EQ(ut::get<8>(testSpan), 8);
   ASSERT_EQ(ut::get<9>(testSpan), 9);
   // Fails to compile: static_assert, index is not within the range of the span
   // ASSERT_EQ(ut::get<10>(testSpan), 10);
}

TEST(UtSpan, Iteration)
{
   std::array<size_t, 10> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
   ut::span<size_t>       testSpan(a);
   for (auto& t : testSpan)
   {
      t *= 2;
   }
   for (size_t i = 0; i < 10; ++i)
   {
      ASSERT_EQ(i * 2, a[i]);
   }
}

TEST(UtSpan, ImplicitConversion)
{
   auto lambda = [](ut::span<size_t> aSpan)
   {
      for (auto& s : aSpan)
      {
         s *= 4;
      }
   };

   std::vector<size_t> v          = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
   size_t              carray[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
   lambda(v);
   lambda(carray);
   for (size_t i = 0; i < 10; ++i)
   {
      ASSERT_EQ(i * 4, v[i]);
      ASSERT_EQ(i * 4, carray[i]);
   }
}
