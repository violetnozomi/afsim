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

#include <type_traits>

#include <gtest/gtest.h>

#include "WsfPProxyBasicValues.hpp"

namespace
{
struct ProxyBasicValuesTestData
{
   std::string mDataTests;
   // lhs != rhs && lhs < rhs
   std::string mInequalityLhs;
   std::string mInequalityRhs;
};
} // namespace

TEST(WsfProxyBasicValues, BasicValueValueFlags)
{
   ASSERT_EQ(WsfProxy::BasicValue::cVALUE_INHERITED, WsfProxy::cVALUE_INHERITED);
   ASSERT_EQ(WsfProxy::BasicValue::cVALUE_UNSET, WsfProxy::cVALUE_UNSET);
   ASSERT_EQ(WsfProxy::BasicValue::cVALUE_BITS, WsfProxy::BasicValue::cVALUE_UNSET);
   ASSERT_EQ(WsfProxy::BasicValue::cCOPY_BITS, WsfProxy::BasicValue::cVALUE_INHERITED | WsfProxy::BasicValue::cVALUE_UNSET);
}

TEST(WsfProxyBasicValues, BasicValue)
{
   WsfProxy::BasicValue test;
   EXPECT_TRUE(test.IsUnset());
   test.ClearUnset();
   EXPECT_FALSE(test.IsUnset());
   test.SetUnset();
   EXPECT_TRUE(test.IsUnset());
   test.SetInherited(true);
   EXPECT_TRUE(test.IsInherited());
   test.SetInherited(false);
   EXPECT_FALSE(test.IsInherited());
}

#include <iostream>

template<typename PROXY_VALUE>
void TestBasicValue(const ProxyBasicValuesTestData& aData)
{
   PROXY_VALUE testDefaultConstructor;
   EXPECT_TRUE(testDefaultConstructor.IsUnset());
   EXPECT_TRUE(testDefaultConstructor.ToString() == "unset" || testDefaultConstructor.ToString() == "");
   PROXY_VALUE testFromString;
   testFromString.SetFromString(aData.mDataTests);
   EXPECT_FALSE(testFromString.IsUnset());
   auto testToString = testFromString.ToString();
   EXPECT_EQ(testToString, aData.mDataTests);
   PROXY_VALUE testCopyValue;
   testCopyValue.CopyValue(testFromString);
   EXPECT_EQ(testCopyValue, testFromString);
   PROXY_VALUE testInequalityLhs, testInequalityRhs;
   testInequalityLhs.SetFromString(aData.mInequalityLhs);
   testInequalityRhs.SetFromString(aData.mInequalityRhs);
   EXPECT_FALSE(testInequalityLhs == testInequalityRhs);
   EXPECT_TRUE(testInequalityLhs.Less(testInequalityRhs));
}

TEST(WsfProxyBasicValues, IntValue)
{
   ProxyBasicValuesTestData td{"0", "0", "1"};
   TestBasicValue<WsfProxy::Int>(td);
}
