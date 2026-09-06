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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "UtInput.hpp"
#include "UtLexicalCast.hpp"
#include "WsfAttributeContainer.hpp"
#include "WsfCommProtocolRIPv2.hpp"
#include "WsfComponentRoles.hpp"

namespace wsf
{
namespace comm
{
namespace router
{
namespace rip
{

TEST(RipV2, isFullTableRequest)
{
   Message msg;
   EXPECT_FALSE(isFullTableRequest(msg));
   msg.entries.emplace_back();
   EXPECT_FALSE(isFullTableRequest(msg));
   msg.entries.front().addressFamilyIdentifier = 0;
   EXPECT_FALSE(isFullTableRequest(msg));
   msg.entries.front().metric = 16;
   EXPECT_TRUE(isFullTableRequest(msg));
}

TEST(RipV2, CreateFullTableRequest)
{
   auto request = CreateFullTableRequest();
   EXPECT_TRUE(isFullTableRequest(request));
}

TEST(IPv4, masking)
{
   IPv4 addr{192, 168, 1, 10};
   IPv4 mask{255, 255, 255, 0};

   auto expected = IPv4{192, 168, 1, 0};
   ASSERT_EQ(expected, addr & mask);

   mask     = IPv4{255, 255, 0, 0};
   expected = IPv4{192, 168, 0, 0};
   ASSERT_EQ(expected, addr & mask);
}

TEST(IPv4, to_string)
{
   IPv4        addr(192, 168, 1, 2);
   std::string expected{"192.168.1.2"};
   ASSERT_EQ(expected, addr.to_string());
}

TEST(IPv4, stream_operator)
{
   std::stringstream str;
   std::string       expected = "192.168.1.10";
   str << expected;
   IPv4 ip;
   str >> ip;
   ASSERT_FALSE(str.fail());
   ASSERT_EQ(expected, ip.to_string());
}

TEST(IPv4, string_cast)
{
   std::string expected = "192.168.1.10";
   auto        ip       = ut::lexical_cast<IPv4>(expected);
   ASSERT_EQ(expected, ip.to_string());
}

TEST(IPv4, FromCommAddress)
{
   std::string expected = "192.168.1.10";
   Address     address(expected);
   auto        ip = ut::lexical_cast<IPv4>(address.GetAddress());
   ASSERT_EQ(expected, ip.to_string());
}

TEST(WsfCommProtocolRIPv2, QueryInterface)
{
   RIPv2 ripProtocol;
   ASSERT_NE(ripProtocol.QueryInterface(cWSF_COMPONENT_COMM_ROUTER_PROTOCOL_RIPv2), nullptr);
   ASSERT_NE(ripProtocol.QueryInterface(cWSF_COMPONENT_COMM_ROUTER_PROTOCOL), nullptr);
   ASSERT_NE(ripProtocol.QueryInterface(cWSF_COMPONENT_NULL), nullptr);
   ASSERT_EQ(ripProtocol.QueryInterface(cWSF_COMPONENT_COMM_ROUTER_PROTOCOL_RIPv2), &ripProtocol);
   ASSERT_EQ(ripProtocol.QueryInterface(cWSF_COMPONENT_COMM_ROUTER_PROTOCOL), &ripProtocol);
   ASSERT_EQ(ripProtocol.QueryInterface(cWSF_COMPONENT_NULL), &ripProtocol);
}

TEST(WsfCommProtocolRIPv2, DISABLED_ProcessInput)
{
   RIPv2   rip;
   UtInput input;
   input.PushInputString("update_interval 25 sec\n"
                         "timeoute_to_drop 120 sec\n"
                         "poisoned_reverse on\n"
                         "not_a_valid_command");
   ASSERT_TRUE(rip.ProcessInput(input));
   ASSERT_TRUE(rip.ProcessInput(input));
   ASSERT_TRUE(rip.ProcessInput(input));
   ASSERT_FALSE(rip.ProcessInput(input));
}

} // namespace rip
} // namespace router
} // namespace comm
} // namespace wsf
