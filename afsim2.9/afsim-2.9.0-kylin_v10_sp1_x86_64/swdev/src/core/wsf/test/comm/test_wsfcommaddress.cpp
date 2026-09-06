// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <string>

#include "WsfCommAddress.hpp"
#include "gtest/gtest.h"

namespace wsf
{
namespace comm
{
TEST(CommAddress, Constructor_1)
{
   Address addressObj;
   auto    address    = addressObj.GetAddress();
   auto    prefix     = addressObj.GetRoutingPrefix();
   auto    subnet     = addressObj.GetSubnet();
   auto    subnetSize = addressObj.GetSubnetMaskBitLength();

   EXPECT_EQ(address, "0.0.0.0");
   EXPECT_EQ(prefix, "");
   EXPECT_EQ(subnet, "");
   EXPECT_EQ(subnetSize, 24);
}

TEST(CommAddress, Constructor_2)
{
   Address     addressObj("192.122.87.2", 24);
   std::string address    = addressObj.GetAddress();
   std::string prefix     = addressObj.GetRoutingPrefix();
   std::string subnet     = addressObj.GetSubnet();
   size_t      subnetSize = addressObj.GetSubnetMaskBitLength();

   EXPECT_EQ(address, "192.122.87.2");
   EXPECT_EQ(prefix, "192.122.87.0");
   EXPECT_EQ(subnet, "255.255.255.0");
   EXPECT_EQ(subnetSize, 24);
}

TEST(CommAddress, Constructor_3)
{
   Address     addressObj("192.122.87.2", 16);
   std::string address    = addressObj.GetAddress();
   std::string prefix     = addressObj.GetRoutingPrefix();
   std::string subnet     = addressObj.GetSubnet();
   size_t      subnetSize = addressObj.GetSubnetMaskBitLength();

   EXPECT_EQ(address, "192.122.87.2");
   EXPECT_EQ(prefix, "192.122.0.0");
   EXPECT_EQ(subnet, "255.255.0.0");
   EXPECT_EQ(subnetSize, 16);
}

TEST(CommAddress, Constructor_4)
{
   Address     addressObj("192.122.87.2", 8);
   std::string address    = addressObj.GetAddress();
   std::string prefix     = addressObj.GetRoutingPrefix();
   std::string subnet     = addressObj.GetSubnet();
   size_t      subnetSize = addressObj.GetSubnetMaskBitLength();

   EXPECT_EQ(address, "192.122.87.2");
   EXPECT_EQ(prefix, "192.0.0.0");
   EXPECT_EQ(subnet, "255.0.0.0");
   EXPECT_EQ(subnetSize, 8);
}

TEST(CommAddress, Constructor_5)
{
   Address     addressObj("192.122.87.2", 32);
   std::string address    = addressObj.GetAddress();
   std::string prefix     = addressObj.GetRoutingPrefix();
   std::string subnet     = addressObj.GetSubnet();
   size_t      subnetSize = addressObj.GetSubnetMaskBitLength();

   EXPECT_EQ(address, "192.122.87.2");
   EXPECT_EQ(prefix, "192.122.87.2");
   EXPECT_EQ(subnet, "255.255.255.255");
   EXPECT_EQ(subnetSize, 32);
}

TEST(CommAddress, Constructor_6)
{
   Address     addressObj("192.122.87.2", 1);
   std::string address    = addressObj.GetAddress();
   std::string prefix     = addressObj.GetRoutingPrefix();
   std::string subnet     = addressObj.GetSubnet();
   size_t      subnetSize = addressObj.GetSubnetMaskBitLength();

   EXPECT_EQ(address, "192.122.87.2");
   EXPECT_EQ(prefix, "128.0.0.0");
   EXPECT_EQ(subnet, "128.0.0.0");
   EXPECT_EQ(subnetSize, 1);
}

TEST(CommAddress, Constructor_7)
{
   Address     addressObj("192.122.87.2", 4);
   std::string address    = addressObj.GetAddress();
   std::string prefix     = addressObj.GetRoutingPrefix();
   std::string subnet     = addressObj.GetSubnet();
   size_t      subnetSize = addressObj.GetSubnetMaskBitLength();

   EXPECT_EQ(address, "192.122.87.2");
   EXPECT_EQ(prefix, "192.0.0.0");
   EXPECT_EQ(subnet, "240.0.0.0");
   EXPECT_EQ(subnetSize, 4);
}

TEST(CommAddress, Constructor_8)
{
   Address     addressObj("192.122.87.2", 13);
   std::string address    = addressObj.GetAddress();
   std::string prefix     = addressObj.GetRoutingPrefix();
   std::string subnet     = addressObj.GetSubnet();
   size_t      subnetSize = addressObj.GetSubnetMaskBitLength();

   EXPECT_EQ(address, "192.122.87.2");
   EXPECT_EQ(prefix, "192.120.0.0");
   EXPECT_EQ(subnet, "255.248.0.0");
   EXPECT_EQ(subnetSize, 13);
}

TEST(CommAddress, Constructor_9)
{
   Address     addressObj("192.122.87.2", 22);
   std::string address    = addressObj.GetAddress();
   std::string prefix     = addressObj.GetRoutingPrefix();
   std::string subnet     = addressObj.GetSubnet();
   size_t      subnetSize = addressObj.GetSubnetMaskBitLength();

   EXPECT_EQ(address, "192.122.87.2");
   EXPECT_EQ(prefix, "192.122.84.0");
   EXPECT_EQ(subnet, "255.255.252.0");
   EXPECT_EQ(subnetSize, 22);
}

TEST(CommAddress, CopyConstructor)
{
   Address addressObj("192.122.87.2", 22);
   Address addressObjCopy(addressObj);

   EXPECT_EQ(addressObj.GetAddress(), addressObjCopy.GetAddress());
   EXPECT_EQ(addressObj.GetRoutingPrefix(), addressObjCopy.GetRoutingPrefix());
   EXPECT_EQ(addressObj.GetSubnet(), addressObjCopy.GetSubnet());
   EXPECT_EQ(addressObj.GetSubnetMaskBitLength(), addressObjCopy.GetSubnetMaskBitLength());

   EXPECT_TRUE(addressObj == addressObjCopy);
}

TEST(CommAddress, Operator)
{
   Address addressObj1("192.45.222.116");
   Address addressObj2("192.45.222.116");
   Address addressObj3("192.45.222.115");

   EXPECT_TRUE(addressObj1 == addressObj2);
   EXPECT_FALSE(addressObj1 == addressObj3);
   EXPECT_FALSE(addressObj1 != addressObj2);
   EXPECT_TRUE(addressObj2 != addressObj3);
   EXPECT_TRUE(addressObj3 < addressObj2);
   EXPECT_FALSE(addressObj1 < addressObj3);
}

TEST(CommAddress, StreamOperator)
{
   Address addressObj("192.45.222.116");

   std::stringstream ss;
   ss << addressObj;
   EXPECT_EQ(ss.str(), "192.45.222.116");
}

TEST(CommAddress, HostNumbers)
{
   Address addressObj1("192.45.222.116");
   Address addressObj2("192.45.222.116", 16);
   Address addressObj3("22.10.16.128");
   Address addressObj4("22.10.16.128", 8);

   size_t val1 = addressObj1.GetNumHostAddresses();
   size_t val2 = addressObj2.GetNumHostAddresses();
   size_t val3 = addressObj3.GetNumHostAddresses();
   size_t val4 = addressObj4.GetNumHostAddresses();

   EXPECT_EQ(val1, 256);
   EXPECT_EQ(val2, 65536);
   EXPECT_EQ(val3, 256);
   EXPECT_EQ(val4, 16777216);
}

TEST(CommAddress, PreFixIncrementOperator)
{
   //! Note: These tests also stress and confirm the validity
   //! of the Address class "PopulateAddressFromBytes()" method.

   Address addressObj1("192.45.222.116");
   Address addressObj2("192.45.222.116", 16);
   Address addressObj3("0.0.0.0");
   Address addressObj4("254.255.255.255");
   Address addressObj5("255.255.255.255");

   Address addressCompare1("192.45.222.117");
   Address addressCompare2("192.45.222.117", 16);
   Address addressCompare3("0.0.0.1");
   Address addressCompare4("255.0.0.0");
   Address addressCompare5("0.0.0.0");

   ++addressObj1;
   ++addressObj2;
   ++addressObj3;
   ++addressObj4;
   ++addressObj5;

   EXPECT_EQ(addressObj1, addressCompare1);
   EXPECT_EQ(addressObj2, addressCompare2);
   EXPECT_EQ(addressObj3, addressCompare3);
   EXPECT_EQ(addressObj4, addressCompare4);
   EXPECT_EQ(addressObj5, addressCompare5);
}

TEST(CommAddress, PostFixIncrementOperator)
{
   //! Note: These tests also stress and confirm the validity
   //! of the Address class "PopulateAddressFromBytes()" method.

   Address addressObj1("192.45.222.116");
   Address addressObj2("192.45.222.116", 16);
   Address addressObj3("0.0.0.0");
   Address addressObj4("254.255.255.255");
   Address addressObj5("255.255.255.255");

   Address addressCompare1("192.45.222.117");
   Address addressCompare2("192.45.222.117", 16);
   Address addressCompare3("0.0.0.1");
   Address addressCompare4("255.0.0.0");
   Address addressCompare5("0.0.0.0");

   addressObj1++;
   addressObj2++;
   addressObj3++;
   addressObj4++;
   addressObj5++;

   EXPECT_EQ(addressObj1, addressCompare1);
   EXPECT_EQ(addressObj2, addressCompare2);
   EXPECT_EQ(addressObj3, addressCompare3);
   EXPECT_EQ(addressObj4, addressCompare4);
   EXPECT_EQ(addressObj5, addressCompare5);
}

TEST(CommAddress, GenerateAddress)
{
   Address addressObj1("192.45.222.116");
   Address addressObj2("0.0.0.0");
   Address addressObj3;

   addressObj1.GenerateAddress("<local:master>");
   addressObj2.GenerateAddress("blue");
   addressObj3.GenerateAddress("red", 16);

   auto address1 = addressObj1.GetAddress();
   auto address2 = addressObj2.GetAddress();
   auto address3 = addressObj3.GetAddress();

   auto cidr1 = addressObj1.GetSubnetMaskBitLength();
   auto cidr2 = addressObj2.GetSubnetMaskBitLength();
   auto cidr3 = addressObj3.GetSubnetMaskBitLength();

   EXPECT_EQ(address1, "38.63.47.62");
   EXPECT_EQ(address2, "0.41.40.23");
   EXPECT_EQ(address3, "0.40.213.222");
   EXPECT_EQ(cidr1, 24);
   EXPECT_EQ(cidr2, 24);
   EXPECT_EQ(cidr3, 16);

   addressObj3.GenerateAddress("blue");
   address3 = addressObj3.GetAddress();

   EXPECT_EQ(address3, "216.187.220.73");

   addressObj3.GenerateAddress("blue");
   address3 = addressObj3.GetAddress();

   EXPECT_EQ(address3, "59.23.37.88");

   addressObj3.GenerateAddress("not_blue");
   address3 = addressObj3.GetAddress();

   EXPECT_EQ(address3, "233.17.115.203");
}

TEST(CommAddress, CheckInAddressRange)
{
   Address addressObj1("192.45.222.116");
   Address addressObj2("192.45.222.0");
   Address addressObj3("192.45.222.000");
   Address addressObj4("192.45.222.255");
   Address addressObj5("192.45.223.000");

   Address addressObj6("192.45.222.116", 22);
   Address addressObj7("192.45.219.255");
   Address addressObj8("192.45.220.0");
   Address addressObj9("192.45.223.255");
   Address addressObj10("192.45.224.0");

   EXPECT_TRUE(Address::CheckInAddressRange(addressObj1, addressObj2));
   EXPECT_TRUE(Address::CheckInAddressRange(addressObj1, addressObj3));
   EXPECT_TRUE(Address::CheckInAddressRange(addressObj1, addressObj4));
   EXPECT_FALSE(Address::CheckInAddressRange(addressObj1, addressObj5));

   EXPECT_FALSE(Address::CheckInAddressRange(addressObj6, addressObj7));
   EXPECT_TRUE(Address::CheckInAddressRange(addressObj6, addressObj8));
   EXPECT_TRUE(Address::CheckInAddressRange(addressObj6, addressObj9));
   EXPECT_FALSE(Address::CheckInAddressRange(addressObj6, addressObj10));
}

TEST(CommAddress, GetBroadcastAddress)
{
   Address addressObj1("65.114.155.95", 22);

   std::string subnet_mask       = "255.255.252.0";
   std::string broadcast_address = "65.114.155.255";

   EXPECT_TRUE(addressObj1.GetSubnet() == subnet_mask);
   EXPECT_TRUE(addressObj1.GetBroadcastAddress() == broadcast_address);
}
} // namespace comm
} // namespace wsf
