// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfExternalLinks.hpp"
#include "gtest/gtest.h"

namespace wsf
{

//! Note: Command chain and WsfGroup usage is not tested here. These
//! objects are too tightly coupled with the simulation and will be
//! tested via a standard scenario input file.

TEST(ExternalLinks, AddRecipient)
{
   ExternalLinks links;
   comm::Address address;
   address.SetAddress("192.168.1.1");
   std::string xmtrName     = "xmtr";
   std::string rcvrName     = "rcvr";
   std::string platformName = "rcvrPlatform";

   auto ok = links.AddRecipient(xmtrName, address);
   ok &= links.AddRecipient(xmtrName, std::make_pair(platformName, rcvrName));

   EXPECT_TRUE(ok);
   EXPECT_EQ(links.GetRecipientCount(), 2);
   EXPECT_EQ(links.GetRecipientCount(xmtrName), 2);

   xmtrName = "another_xmtr";
   ok &= links.AddRecipient(xmtrName, address);
   ok &= links.AddRecipient(xmtrName, std::make_pair(platformName, rcvrName));

   EXPECT_TRUE(ok);
   EXPECT_EQ(links.GetRecipientCount(), 4);
   EXPECT_EQ(links.GetRecipientCount(xmtrName), 2);

   EXPECT_FALSE(links.AddRecipient(xmtrName, address));

   EXPECT_FALSE(links.AddRecipient(xmtrName, std::make_pair(platformName, rcvrName)));

   EXPECT_EQ(links.GetRecipientCount(), 4);
   EXPECT_EQ(links.GetRecipientCount(xmtrName), 2);
}

TEST(ExternalLinks, RemoveRecipient)
{
   ExternalLinks links;
   comm::Address address;
   address.SetAddress("192.168.1.1");
   std::string xmtrName     = "xmtr";
   std::string rcvrName     = "rcvr";
   std::string platformName = "rcvrPlatform";

   links.AddRecipient(xmtrName, address);
   links.AddRecipient(xmtrName, std::make_pair(platformName, rcvrName));
   std::string otherXmtrName = "another_xmtr";
   links.AddRecipient(otherXmtrName, address);
   links.AddRecipient(otherXmtrName, std::make_pair(platformName, rcvrName));

   std::string   falseXmtrName     = "not_a_xmtr";
   std::string   falseRcvrName     = "not_a_rcvr";
   std::string   falsePlatformName = "not_my_platform";
   comm::Address falseAddress;
   falseAddress.SetAddress("2.2.2.2");

   EXPECT_FALSE(links.RemoveRecipient(falseXmtrName, address));
   EXPECT_FALSE(links.RemoveRecipient(xmtrName, falseAddress));
   EXPECT_FALSE(links.RemoveRecipient(falseXmtrName, std::make_pair(platformName, rcvrName)));
   EXPECT_FALSE(links.RemoveRecipient(xmtrName, std::make_pair(falsePlatformName, rcvrName)));
   EXPECT_FALSE(links.RemoveRecipient(xmtrName, std::make_pair(platformName, falseRcvrName)));

   EXPECT_TRUE(links.RemoveRecipient(xmtrName, address));
   EXPECT_EQ(links.GetRecipientCount(), 3);
   EXPECT_EQ(links.GetRecipientCount(xmtrName), 1);
   EXPECT_EQ(links.GetRecipientCount(otherXmtrName), 2);

   EXPECT_TRUE(links.RemoveRecipient(xmtrName, std::make_pair(platformName, rcvrName)));
   EXPECT_EQ(links.GetRecipientCount(), 2);
   EXPECT_EQ(links.GetRecipientCount(xmtrName), 0);
   EXPECT_EQ(links.GetRecipientCount(otherXmtrName), 2);

   EXPECT_TRUE(links.RemoveRecipient(otherXmtrName, address));
   EXPECT_EQ(links.GetRecipientCount(), 1);
   EXPECT_EQ(links.GetRecipientCount(xmtrName), 0);
   EXPECT_EQ(links.GetRecipientCount(otherXmtrName), 1);

   EXPECT_TRUE(links.RemoveRecipient(otherXmtrName, std::make_pair(platformName, rcvrName)));
   EXPECT_EQ(links.GetRecipientCount(), 0);
   EXPECT_EQ(links.GetRecipientCount(xmtrName), 0);
   EXPECT_EQ(links.GetRecipientCount(otherXmtrName), 0);

   EXPECT_FALSE(links.HasLinks());
}

} // namespace wsf
