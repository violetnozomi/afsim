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
#include <map>

#include <gtest/gtest.h>

#include "UtLog.hpp"
#include "UtLogSubscriber.hpp"
#include "UtScriptData.hpp"

// Regression test for the following issue:
//
// Insertions into a std::map with UtScriptData keys
// were being unexpectedly dropped.  This was caused by
// UtScriptData::operator< not correctly handling the case
// where one of its arguments contained a string,
// and the other argument contained some other type
// (other than cPOINTER, which is out of scope for this issue).
// As a result, equivalence(!(x < y) && !(y < x)) and equality
// did not mean the same thing for UtScriptData.
//
// While we're doing this, make sure that no error messages were logged.
// It's not supposed to be an error to compare string < int, or int < string.
TEST(UtScriptDataClass, StringIntEquivalenceInMap)
{
   ut::log::Subscriber catchErrorMessages(ut::log::Message::Error());
   { // original reproducer
      std::map<UtScriptData, UtScriptData> values1;
      values1[UtScriptData(std::string("forty-two"))] = UtScriptData(42);
      values1[UtScriptData(42.1f)]                    = UtScriptData(42);
      EXPECT_EQ(values1.size(), std::size_t(2));
   }
   { // map size should not depend on insertion order
      std::map<UtScriptData, UtScriptData> values1;
      values1[UtScriptData(42.1f)]                    = UtScriptData(42);
      values1[UtScriptData(std::string("forty-two"))] = UtScriptData(42);
      EXPECT_EQ(values1.size(), std::size_t(2));
   }
   std::queue<ut::log::Message> messageQueue = catchErrorMessages.TakeQueue();
   ASSERT_TRUE(messageQueue.empty());
}

// Test that fixing the above issue did not change the original behavior
// that string < not-pointer-not-string is false.
// While doing that, make sure that not-pointer-not-string < string is true,
// and that neither case logged error messages.
TEST(UtScriptDataClass, StringNotLessThanIntBoolOrDouble)
{
   ut::log::Subscriber catchErrorMessages(ut::log::Message::Error());
   UtScriptData        s418(std::string("four hundred and eighteen"));
   {
      UtScriptData i418(418);
      EXPECT_FALSE(s418 < i418);
      EXPECT_TRUE(i418 < s418);
   }
   for (bool boolValue : {false, true})
   {
      UtScriptData b(boolValue);
      EXPECT_FALSE(s418 < b);
      EXPECT_TRUE(b < s418);
   }
   {
      UtScriptData d418(418.0);
      EXPECT_FALSE(s418 < d418);
      EXPECT_TRUE(d418 < s418);
   }
   std::queue<ut::log::Message> messageQueue = catchErrorMessages.TakeQueue();
   ASSERT_TRUE(messageQueue.empty());
}

// Test that Data::operator<(T) and Data::operator<(Data)
// produce consistent results, when *this contains a string.
TEST(UtScriptDataClass, ConsistentOperatorLess)
{
   ut::log::Subscriber catchErrorMessages(ut::log::Message::Error());
   UtScriptData        s418(std::string("four hundred and eighteen"));
   {
      constexpr int intValue = 418;
      UtScriptData  i418(intValue);
      EXPECT_EQ(s418 < i418, s418 < intValue);
      // There is no overloaded nonmember operator<(int, Data).
      EXPECT_EQ(s418 < i418, !(i418 < s418));
   }
   for (bool boolValue : {false, true})
   {
      UtScriptData b(boolValue);
      EXPECT_EQ(s418 < b, s418 < boolValue);
      // There is no overloaded nonmember operator<(bool, Data).
      EXPECT_EQ(s418 < b, !(b < s418));
   }
   {
      constexpr double dValue = 418.0;
      UtScriptData     d418(dValue);
      EXPECT_EQ(s418 < d418, s418 < dValue);
      // There is no overloaded nonmember operator<(double, Data).
      EXPECT_EQ(s418 < d418, !(d418 < s418));
   }
   std::queue<ut::log::Message> messageQueue = catchErrorMessages.TakeQueue();
   ASSERT_TRUE(messageQueue.empty());
}

// Regression test for the following issue:
//
// Constructing Data with a const char* was resolving to
// the Data(bool) constructor, not to the expected Data(std::string).
// This caused an access violation for e.g., Data("").
// It also caused the unexpected behavior of Data("raw string")
// claiming that it contains a bool, rather than a string.
TEST(UtScriptDataClass, Constructor)
{
   {
      UtScriptData strData(std::string("one hundred and eleven"));
      UtScriptData cstrData("one hundred and eleven");
      EXPECT_EQ(strData.GetType(), UtScriptData::Type::cSTRING);
      EXPECT_EQ(cstrData.GetType(), UtScriptData::Type::cSTRING);
      EXPECT_EQ(strData, cstrData);

      // Data also has operator= overloads for const char* and std::string.
      // Make sure that our fix for the constructor didn't affect those.
      // Create the variables with a type not a string,
      // so that we know that the assignment leaves them with the right type.
      UtScriptData cstrData2(111);
      cstrData2 = "four hundred and eighteen";
      UtScriptData strData2(111);
      strData2 = "seven hundred seventy-seven";
      EXPECT_EQ(strData2.GetType(), UtScriptData::Type::cSTRING);
      EXPECT_EQ(cstrData2.GetType(), UtScriptData::Type::cSTRING);
      EXPECT_EQ(cstrData2, UtScriptData("four hundred and eighteen"));
      EXPECT_EQ(strData2, UtScriptData("seven hundred seventy-seven"));
   }

   for (bool boolValue : {false, true})
   {
      UtScriptData boolData(boolValue);
      EXPECT_EQ(boolData.GetType(), UtScriptData::Type::cBOOL);
   }

   for (double dblValue : {-111.0, 0.0, 418.0})
   {
      UtScriptData dblData(dblValue);
      EXPECT_EQ(dblData.GetType(), UtScriptData::Type::cDOUBLE);
   }

   // We don't want to constrain this to have type cDOUBLE,
   // in case future developers want to add a new enum for float.
   UtScriptData fltData(418.0f);
   EXPECT_NE(fltData.GetType(), UtScriptData::Type::cBOOL);
   EXPECT_NE(fltData.GetType(), UtScriptData::Type::cINT);
   EXPECT_NE(fltData.GetType(), UtScriptData::Type::cSTRING);
}

// The empty string is a special case in Data.
// Test that the empty string, just like any other string,
// is not less than any other not-pointer-not-string.
TEST(UtScriptDataClass, EmptyStringNotLessThan)
{
   for (auto&& empty : {UtScriptData(std::string("")), UtScriptData("")})
   {
      for (bool boolValue : {false, true})
      {
         UtScriptData boolData(boolValue);
         EXPECT_FALSE(empty < boolData);
         EXPECT_TRUE(boolData < empty);
      }
      for (double dblValue : {-111.0, 0.0, 418.0})
      {
         UtScriptData dblData(dblValue);
         EXPECT_FALSE(empty < dblData);
         EXPECT_TRUE(dblData < empty);
      }
      for (int intValue : {-777, 0, 777})
      {
         UtScriptData intData(intValue);
         EXPECT_FALSE(empty < intData);
         EXPECT_TRUE(intData < empty);
      }
   }
}
