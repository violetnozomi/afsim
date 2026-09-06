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

#include "UtMemory.hpp"
#include "WsfPProxyBasicTypes.hpp"
#include "WsfPProxyType.hpp"
#include "WsfPProxyValue.hpp"

TEST(ProxyValueTest, EmptyContruction)
{
   auto emptyValue = WsfPProxyValue::ConstructNew(nullptr);
   ASSERT_FALSE(emptyValue);
   ASSERT_TRUE(emptyValue.IsUnset());
   ASSERT_FALSE(emptyValue.IsValid());
}

TEST(ProxyValueTest, IntegerValue)
{
   auto typePtr = ut::make_unique<WsfPProxyIntType>();
   auto value   = WsfPProxyValue::ConstructNew(typePtr.get());
   ASSERT_TRUE(value.IsUnset());
   ASSERT_FALSE(value.IsInherited());
   ASSERT_TRUE(value.IsBasicType());
   ASSERT_EQ("", value.ValueToString());
   ASSERT_EQ(nullptr, value.GetList());
   ASSERT_EQ(nullptr, value.GetObjectMap());
   ASSERT_EQ(typePtr.get(), value.GetType());
   value.SetValue("256");
   ASSERT_FALSE(value.IsUnset());
   ASSERT_EQ("256", value.ValueToString());
   int tmp;
   ASSERT_TRUE(value.GetIntegerValue(tmp));
   ASSERT_EQ(256, tmp);
   value.SetIntegerValue(1024);
   ASSERT_EQ("1024", value.ValueToString());
   ASSERT_TRUE(value.GetIntegerValue(tmp));
   ASSERT_EQ(1024, tmp);
   value.Delete();
}
