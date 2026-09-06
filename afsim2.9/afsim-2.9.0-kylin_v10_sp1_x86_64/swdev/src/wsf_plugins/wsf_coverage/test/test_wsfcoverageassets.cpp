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

#include <string>

#include <gtest/gtest.h>

#include "WsfCoverageAsset.hpp"
#include "WsfCoverageGridPoint.hpp"

TEST(WsfCoverageAssets, GridAsset)
{
   std::string            devName{"device"};
   wsf::coverage::PointId id{12};
   std::string            platName{"grid_12"};

   wsf::coverage::GridAsset ga{platName, id, devName};
   EXPECT_TRUE(ga.IsValid());
   EXPECT_EQ(ga.GetPointId(), id);
   EXPECT_EQ(ga.GetDeviceName(), devName);
   EXPECT_EQ(ga.GetPlatformName(), platName);

   ga.SetPlatformName("a_new_name");
   EXPECT_EQ(ga.GetPlatformName(), "a_new_name");

   ga.SetDeviceName("dev");
   EXPECT_EQ(ga.GetDeviceName(), "dev");

   ga.SetPointId(1000);
   EXPECT_EQ(ga.GetPointId(), 1000u);

   ga.SetPointId(wsf::coverage::cINVALID_POINT_ID);
   EXPECT_FALSE(ga.IsValid());
}

TEST(WsfCoverageAssets, FreeAsset)
{
   std::string platName{"platform"};
   std::string devName{"device"};

   wsf::coverage::FreeAsset fa{platName, devName};
   EXPECT_EQ(fa.GetPlatformName(), platName);
   EXPECT_EQ(fa.GetDeviceName(), devName);

   fa.SetPlatformName("plat");
   EXPECT_EQ(fa.GetPlatformName(), "plat");

   fa.SetDeviceName("dev");
   EXPECT_EQ(fa.GetDeviceName(), "dev");
}
