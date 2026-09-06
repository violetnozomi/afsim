// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright (C) 2022 Stellar Science; U.S. Government has Unlimited Rights.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <gtest/gtest.h>

#include "mapUtil.hpp"


TEST(MapUtil, MapOrderedBy)
{
   const std::map<int, int>                     originalData   = {{1, 10}, {2, 9}, {3, 8}, {4, 7}, {5, 6}};
   const std::vector<std::pair<const int, int>> expectedResult = {{5, 6}, {4, 7}, {3, 8}, {2, 9}, {1, 10}};
   auto secondAscending = [](const std::map<int, int>::value_type* a, const std::map<int, int>::value_type* b)
   { return a->second < b->second; };

   const std::vector<const std::pair<const int, int>*> result = artificer::MapOrderedBy(originalData, secondAscending);
   auto                                                expected = expectedResult.begin();
   for (const auto& actual : result)
   {
      EXPECT_EQ(*expected, *actual);
      ++expected;
   }
}

TEST(MapUtil, IncrementCount)
{
   std::map<int, uint64_t> countOfValues;
   artificer::IncrementCount(countOfValues, 1);
   artificer::IncrementCount(countOfValues, 1);
   artificer::IncrementCount(countOfValues, 2);
   artificer::IncrementCount(countOfValues, 1);

   const std::map<int, uint64_t> expectedResult = {{1, 3}, {2, 1}};

   EXPECT_EQ(expectedResult, countOfValues);
}

TEST(MapUtil, ParseNameValuePairs)
{
   struct TestCase
   {
      std::string                        input;
      std::map<std::string, std::string> expectedResult;
   };

   const TestCase cases[] = {{"", {}},
                             {";;;;", {}},
                             {"single=pair", {{"single", "pair"}}},
                             {"single=pair;", {{"single", "pair"}}},
                             {"single=pair=containing=more=equals", {{"single", "pair=containing=more=equals"}}},
                             {"string without formatting", {{"string without formatting", ""}}},
                             {"a=1;b=2;c;d;e=5", {{"a", "1"}, {"b", "2"}, {"c", ""}, {"d", ""}, {"e", "5"}}},
                             {"first=value1;second=value2;novalue",
                              {{"first", "value1"}, {"second", "value2"}, {"novalue", ""}}}};

   for (const auto& curCase : cases)
   {
      EXPECT_EQ(artificer::ParseNameValuePairs(curCase.input), curCase.expectedResult);
   }
}
