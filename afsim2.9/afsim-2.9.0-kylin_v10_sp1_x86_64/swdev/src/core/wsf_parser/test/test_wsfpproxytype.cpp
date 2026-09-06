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

#include <map>
#include <memory>

#include <gtest/gtest.h>

#include "UtMemory.hpp"
#include "WsfPProxyBasicTypes.hpp"
#include "WsfPProxyType.hpp"
#include "WsfPProxyValue.hpp"

namespace
{
struct PProxyTestData
{
   WsfProxy::ValueKind mValueType;
   std::string         mTestInputData;
   const std::string   mTestUnsetData;
   std::string         mTestSetData;
};

struct EnumerationData
{
   WsfPEnumStrMap mStringToInt;
   WsfPEnumIntMap mIntToString;
};
} // namespace

TEST(WsfProxyBasicTypes, IntType)
{
   auto type = ut::make_unique<WsfPProxyIntType>();
   auto data = ut::make_unique<char[]>(type->mDataSize);
   type->Construct(data.get());
   ASSERT_EQ(WsfProxy::cINT_VALUE, type->mTypeKind);
   ASSERT_TRUE(type->IsBasicType());
   ASSERT_TRUE(type->IsUnset(data.get()));
   ASSERT_EQ("Int", type->GetTypeName());
   ASSERT_FALSE(type->IsStruct());
   ASSERT_FALSE(type->IsObjectMap());
   ASSERT_FALSE(type->IsList());
   type->SetValue(data.get(), "42");
   ASSERT_EQ("42", type->ToString(data.get()));

   auto dataCopy = ut::make_unique<char[]>(type->mDataSize);
   type->Copy(dataCopy.get(), data.get(), 0);
   ASSERT_TRUE(type->Equal(data.get(), dataCopy.get()));
   ASSERT_FALSE(type->Less(data.get(), dataCopy.get()));
   type->Destroy(data.get());
}

TEST(WsfProxyBasicTypes, BoolType)
{
   auto type = ut::make_unique<WsfPProxyBoolType>();
   auto data = ut::make_unique<char[]>(type->mDataSize);
   type->Construct(data.get());
   ASSERT_EQ(WsfProxy::cBOOL_VALUE, type->mTypeKind);
   ASSERT_TRUE(type->IsBasicType());
   ASSERT_TRUE(type->IsUnset(data.get()));
   ASSERT_EQ("Bool", type->GetTypeName());
   ASSERT_FALSE(type->IsStruct());
   ASSERT_FALSE(type->IsObjectMap());
   ASSERT_FALSE(type->IsList());
   type->SetValue(data.get(), "true");
   ASSERT_EQ("true", type->ToString(data.get()));
   type->SetValue(data.get(), "false");
   ASSERT_EQ("false", type->ToString(data.get()));

   auto dataCopy = ut::make_unique<char[]>(type->mDataSize);
   type->Copy(dataCopy.get(), data.get(), 0);
   ASSERT_TRUE(type->Equal(data.get(), dataCopy.get()));
   ASSERT_FALSE(type->Less(data.get(), dataCopy.get()));
   ASSERT_EQ("false", type->ToString(dataCopy.get()));
   type->Destroy(data.get());
}

template<typename PROXY_TYPE>
void TestBasicType(const PProxyTestData& aData)
{
   auto type = ut::make_unique<PROXY_TYPE>();
   auto data = ut::make_unique<char[]>(type->mDataSize);
   type->Construct(data.get());
   EXPECT_EQ(type->mTypeKind, aData.mValueType);
   EXPECT_TRUE(type->IsUnset(data.get()));
   EXPECT_EQ(type->ToString(data.get()), aData.mTestUnsetData);
   EXPECT_EQ(type->GetTypeName(), wsf::proxy::ValueKindToString(aData.mValueType));
   EXPECT_TRUE(type->IsBasicType());
   EXPECT_FALSE(type->IsStruct());
   EXPECT_FALSE(type->IsList());
   EXPECT_FALSE(type->IsObjectMap());
   type->SetValue(data.get(), aData.mTestInputData);
   EXPECT_FALSE(type->IsUnset(data.get()));
   EXPECT_EQ(type->ToString(data.get()), aData.mTestSetData);
   auto dataCopy = ut::make_unique<char[]>(type->mDataSize);
   type->Copy(dataCopy.get(), data.get(), 0);
   EXPECT_TRUE(type->Equal(data.get(), dataCopy.get()));
   EXPECT_FALSE(type->Less(data.get(), dataCopy.get()));
   EXPECT_EQ(type->ToString(dataCopy.get()), aData.mTestSetData);
   type->Destroy(data.get());
}

TEST(WsfProxyBasicTypes, BoolType2)
{
   PProxyTestData td{WsfProxy::cBOOL_VALUE, "false", "unset", "false"};
   TestBasicType<WsfPProxyBoolType>(td);
   td.mTestInputData = td.mTestSetData = "true";
   TestBasicType<WsfPProxyBoolType>(td);
   td.mTestInputData = "off";
   td.mTestSetData   = "false";
   TestBasicType<WsfPProxyBoolType>(td);
   td.mTestInputData = "on";
   td.mTestSetData   = "true";
   TestBasicType<WsfPProxyBoolType>(td);
   td.mTestInputData = "no";
   td.mTestSetData   = "false";
   TestBasicType<WsfPProxyBoolType>(td);
   td.mTestInputData = "yes";
   td.mTestSetData   = "true";
   TestBasicType<WsfPProxyBoolType>(td);
   td.mTestInputData = "disabled";
   td.mTestSetData   = "false";
   TestBasicType<WsfPProxyBoolType>(td);
   td.mTestInputData = "enable";
   td.mTestSetData   = "true";
   TestBasicType<WsfPProxyBoolType>(td);
}

void TestEnumType(const PProxyTestData& aData, const EnumerationData& aEnum)
{
   auto type          = ut::make_unique<WsfPProxyEnumType>();
   type->mStringToInt = aEnum.mStringToInt;
   type->mIntToString = aEnum.mIntToString;
   auto data          = ut::make_unique<char[]>(type->mDataSize);
   type->Construct(data.get());
   EXPECT_EQ(type->mTypeKind, aData.mValueType);
   EXPECT_TRUE(type->IsUnset(data.get()));
   EXPECT_EQ(type->ToString(data.get()), aData.mTestUnsetData);
   EXPECT_EQ(type->GetTypeName(), wsf::proxy::ValueKindToString(aData.mValueType));
   EXPECT_TRUE(type->IsBasicType());
   EXPECT_FALSE(type->IsStruct());
   EXPECT_FALSE(type->IsList());
   EXPECT_FALSE(type->IsObjectMap());
   type->SetValue(data.get(), aData.mTestInputData);
   EXPECT_FALSE(type->IsUnset(data.get()));
   EXPECT_EQ(type->ToString(data.get()), aData.mTestSetData);
   auto dataCopy = ut::make_unique<char[]>(type->mDataSize);
   type->Copy(dataCopy.get(), data.get(), 0);
   EXPECT_TRUE(type->Equal(data.get(), dataCopy.get()));
   EXPECT_FALSE(type->Less(data.get(), dataCopy.get()));
   EXPECT_EQ(type->ToString(dataCopy.get()), aData.mTestSetData);
   type->Destroy(data.get());
}

TEST(WsfProxyBasicTypes, AltitudeReferenceType)
{
   PProxyTestData  td{WsfProxy::cENUMERATION_VALUE, "msl", "BadValue", "msl"};
   EnumerationData ed{WsfPEnumStrMap{{"default", 0}, {"msl", 1}, {"agl", 2}},
                      WsfPEnumIntMap{{0, "default"}, {1, "msl"}, {2, "agl"}}};
   td.mTestInputData = td.mTestSetData = "msl";
   TestEnumType(td, ed);
   td.mTestInputData = td.mTestSetData = "agl";
   TestEnumType(td, ed);
}
