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

#include <sstream>
#include <string>

#include <gtest/gtest.h>

#include "UtInput.hpp"
#include "Utils.hpp"
#include "WsfCoverageAssetSpecification.hpp"

TEST(WsfCoverageAssetSpecification, SetAssetClass)
{
   wsf::coverage::AssetSpecification::Asset asset{};
   EXPECT_EQ(asset.mType, wsf::coverage::AssetSpecification::AssetClass::cUNKNOWN);
   EXPECT_TRUE(asset.SetAssetClass("platform"));
   EXPECT_EQ(asset.mType, wsf::coverage::AssetSpecification::AssetClass::cPLATFORM);
   EXPECT_TRUE(asset.SetAssetClass("platform_type"));
   EXPECT_EQ(asset.mType, wsf::coverage::AssetSpecification::AssetClass::cPLATFORM_TYPE);
   EXPECT_TRUE(asset.SetAssetClass("category"));
   EXPECT_EQ(asset.mType, wsf::coverage::AssetSpecification::AssetClass::cCATEGORY);
   EXPECT_TRUE(asset.SetAssetClass("group"));
   EXPECT_EQ(asset.mType, wsf::coverage::AssetSpecification::AssetClass::cGROUP);
   EXPECT_FALSE(asset.SetAssetClass("unknown"));
   EXPECT_EQ(asset.mType, wsf::coverage::AssetSpecification::AssetClass::cUNKNOWN);
}

TEST(WsfCoverageAssetSpecification, Construction)
{
   wsf::coverage::AssetSpecification spec{};
   EXPECT_TRUE(spec.GetAssets().empty());
}

TEST(WsfCoverageAssetSpecification, BadInputThrows)
{
   wsf::coverage::AssetSpecification spec{};

   TestFalseReturn(spec, "unknown name device");
   TestEndOfDataThrow(spec, "platform name");
   TestEndOfDataThrow(spec, "platform");
}

TEST(WsfCoverageAssetSpecification, FreeAssetInput)
{
   wsf::coverage::AssetSpecification spec{};

   std::stringstream inputStream{};
   inputStream << "platform platform_name device_one\n";
   inputStream << "platform_type platform_type_name device_two\n";
   inputStream << "category category_name device_three\n";
   inputStream << "group group_name device_four\n";
   UtInput input{};
   input.PushInputString(inputStream.str());

   std::string command;
   input.ReadCommand(command);
   EXPECT_TRUE(spec.ProcessInput(input));
   EXPECT_EQ(spec.GetAssets().size(), 1u);

   input.ReadCommand(command);
   EXPECT_TRUE(spec.ProcessInput(input));
   EXPECT_EQ(spec.GetAssets().size(), 2u);

   input.ReadCommand(command);
   EXPECT_TRUE(spec.ProcessInput(input));
   EXPECT_EQ(spec.GetAssets().size(), 3u);

   input.ReadCommand(command);
   EXPECT_TRUE(spec.ProcessInput(input));
   EXPECT_EQ(spec.GetAssets().size(), 4u);

   // Now test the contents
   auto& assets = spec.GetAssets();
   EXPECT_EQ(assets[0].mType, wsf::coverage::AssetSpecification::AssetClass::cPLATFORM);
   EXPECT_EQ(assets[0].mName, "platform_name");
   EXPECT_EQ(assets[0].mDeviceName, "device_one");
   EXPECT_EQ(assets[1].mType, wsf::coverage::AssetSpecification::AssetClass::cPLATFORM_TYPE);
   EXPECT_EQ(assets[1].mName, "platform_type_name");
   EXPECT_EQ(assets[1].mDeviceName, "device_two");
   EXPECT_EQ(assets[2].mType, wsf::coverage::AssetSpecification::AssetClass::cCATEGORY);
   EXPECT_EQ(assets[2].mName, "category_name");
   EXPECT_EQ(assets[2].mDeviceName, "device_three");
   EXPECT_EQ(assets[3].mType, wsf::coverage::AssetSpecification::AssetClass::cGROUP);
   EXPECT_EQ(assets[3].mName, "group_name");
   EXPECT_EQ(assets[3].mDeviceName, "device_four");
}
