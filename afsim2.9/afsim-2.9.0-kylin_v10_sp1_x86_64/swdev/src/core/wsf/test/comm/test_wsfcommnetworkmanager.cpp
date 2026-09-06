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

#include "WsfComm.hpp"
#include "WsfCommGraph.hpp"
#include "WsfCommNetwork.hpp"
#include "WsfCommNetworkManager.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformTypes.hpp"
#include "gtest/gtest.h"
//#include "WsfTestEnvironment.hpp"

namespace wsf
{
namespace comm
{
////! Some strings that are necessary to instantiate scenario objects as input.
// const std::string cLITERAL_COMM_1 = R"(
//    comm comm1 WSF_COMM_TRANSCEIVER
//    end_comm
//)";

// const std::string cLITERAL_COMM_2 = R"(
//    comm comm2 WSF_COMM_TRANSCEIVER
//    end_comm
//)";

// const std::string cLITERAL_COMM_3 = R"(
//    comm comm3 WSF_COMM_TRANSCEIVER
//    end_comm
//)";

// const std::string cLITERAL_COMM_4 = R"(
//    comm comm4 WSF_COMM_TRANSCEIVER
//    end_comm
//)";

////! Set up the mocked AFSIM simulation and feed it some comm devices as input.
////! This will provide us with some actual comm devices from the simulation typelist
////! that we can use for unit testing. This is necessary due to the reliance of WsfComm
////! on a scenario object as part of its construction.
// static auto testEnvPtr = TestEnvironment::GetTestEnvironment(cLITERAL_COMM_1 +
//                                                              cLITERAL_COMM_2 +
//                                                              cLITERAL_COMM_3 +
//                                                              cLITERAL_COMM_4);

// static auto objPtr1 = (testEnvPtr->GetScenario())->FindType("comm", "comm1");
// static auto objPtr2 = (testEnvPtr->GetScenario())->FindType("comm", "comm2");
// static auto objPtr3 = (testEnvPtr->GetScenario())->FindType("comm", "comm3");
// static auto objPtr4 = (testEnvPtr->GetScenario())->FindType("comm", "comm4");

// static auto commPtr1 = dynamic_cast<Comm*>(objPtr1);
// static auto commPtr2 = dynamic_cast<Comm*>(objPtr2);
// static auto commPtr3 = dynamic_cast<Comm*>(objPtr3);
// static auto commPtr4 = dynamic_cast<Comm*>(objPtr4);

// TEST(CommNetworkManager, Initialize)
//{
//    NetworkManager manager(nullptr);
//    EXPECT_TRUE(manager.Initialize());
// }

// TEST(CommNetworkManger, AddNetwork)
//{
//    Address network1("192.168.1.1");
//    Address network2("222.23.1.34", 16);
//    Address network3("192.168.1.222");

//   std::string networkName1 = "default";
//   std::string networkName2 = "red";
//   std::string networkName3 = "failure";
//   std::string networkName4 = "automatic";
//   Address network4;

//   NetworkManager manager(nullptr);
//   EXPECT_TRUE(manager.AddNetwork(0.0, networkName1, network1));
//   EXPECT_TRUE(manager.AddNetwork(0.0, networkName2, network2));
//   EXPECT_FALSE(manager.AddNetwork(0.0, networkName3, network3));
//   EXPECT_FALSE(manager.AddNetwork(0.0, networkName1, network3));
//   EXPECT_TRUE(manager.AddNetwork(0.0, networkName4, network4));

//   //! Test the return address for the auto-generated network address for network4.
//   //! This is expected repeatable for every simulation run, so it should be consistent.
//   EXPECT_EQ(network4.GetAddress().GetString(), "12.52.213.5");
//   EXPECT_EQ(network4.GetSubnetMaskBitLength(), 24);
//}

// TEST(CommNetworkManger, AddRemoveComm)
//{
//    Address network1("192.168.1.1");
//    Address network2("222.23.1.34", 16);
//    Address network3;
//    Address commAddress1("192.168.1.1");
//    Address commAddress2("192.168.1.255");
//    Address commAddress3("192.168.2.0");
//    Address commAddress4("2.2.2.2");

//   std::string networkName1 = "default";
//   std::string networkName2 = "red";
//   std::string networkName3 = "automatic";

//   NetworkManager manager(nullptr);

//   ASSERT_TRUE(manager.AddNetwork(0.0, networkName1, network1));
//   ASSERT_TRUE(manager.AddNetwork(0.0, networkName2, network2));
//   ASSERT_TRUE(manager.AddNetwork(0.0, networkName3, network3));

//   EXPECT_TRUE(manager.GetNetwork("default"));
//   EXPECT_TRUE(manager.GetNetwork("red"));
//   EXPECT_TRUE(manager.GetNetwork("automatic"));
//
//   //! Add a comm device with a specified address. That address, by definition,
//   //! belongs to the "default" network by nature of its value. It should be
//   //! added to the default network automatically.
//   auto joinedNetworkName = manager.AddComm(0.0, commPtr1, commAddress1);
//   EXPECT_TRUE(networkName1 == joinedNetworkName);

//   //! Attempt to add this device a second time. This should obviously fail,
//   //! with an null string returned indicating no addition with no network joined.
//   joinedNetworkName = manager.AddComm(0.0, commPtr1, commAddress1);
//   EXPECT_FALSE(networkName1 == joinedNetworkName);
//   EXPECT_TRUE("" == joinedNetworkName);

//   //! Attempt to add a comm deviced with a specific address. However, unlike
//   //! the previous test, this address does not currently belong to any available
//   //! networks. It should be added, along with a new network based off of the comm
//   //! name.
//   joinedNetworkName = manager.AddComm(0.0, commPtr2, commAddress4);
//   //! NOTE: The name looks strange because its from a scenario typelist. There isn't
//   //! a platform to get a name from, and the part doesn't have a name either.
//   //! This isn't typical usage, but it gets the job done.
//   EXPECT_TRUE("NULL..1" == joinedNetworkName);

//   //! Attempt to add a comm device, without an address, to a specified network.
//   //! Adding this device to network "default" should result in assignment to
//   //! address 192.168.1.0. The subnet on this network begins at this address,
//   //! and the only other comm device in this network is using address 192.168.1.1.
//   auto returnAddress = manager.AddComm(0.0, commPtr3, networkName1);
//   EXPECT_TRUE(returnAddress == Address("192.168.1.0"));

//   //! Adding another comm device to the "default" network should result in address
//   //! 192.168.1.2
//   returnAddress = manager.AddComm(0.0, commPtr4, networkName1);
//   EXPECT_TRUE(returnAddress == Address("192.168.1.2"));

//   //! Remove commPtr4.
//   manager.RemoveComm(0.0, returnAddress);
//   auto addressList = manager.GetAddressesInNetwork(networkName1);
//   EXPECT_TRUE(addressList.size() == 2);
//   auto it = addressList.find(returnAddress);
//   EXPECT_TRUE(it == addressList.end());

//   //! Re-add commPtr4, this time without a specific network. It should be placed in its
//   //! own network assignment, with a new address. The new network should be an increment of
//   //! the last auto-assigned network, namely "NULL..2"
//   std::string newNetworkName = "";
//   returnAddress = manager.AddComm(0.0, commPtr4, newNetworkName);
//   EXPECT_TRUE(returnAddress == Address("0.108.41.0"));
//   EXPECT_TRUE(newNetworkName == "NULL..2");
//}

// TEST(CommNetworkManger, GetNetwork)
//{
//    Address network1("192.168.1.1");
//    Address network2("222.23.1.34", 16);
//    Address network3;

//   std::string networkName1 = "default";
//   std::string networkName2 = "red";
//   std::string networkName3 = "automatic";

//   NetworkManager manager(nullptr);

//   ASSERT_TRUE(manager.AddNetwork(0.0, networkName1, network1));
//   ASSERT_TRUE(manager.AddNetwork(0.0, networkName2, network2));
//   ASSERT_TRUE(manager.AddNetwork(0.0, networkName3, network3));

//   EXPECT_TRUE(manager.GetNetwork("default"));
//   EXPECT_TRUE(manager.GetNetwork("red"));
//   EXPECT_TRUE(manager.GetNetwork("automatic"));
//   EXPECT_FALSE(manager.GetNetwork("doesNotExist"));
//}

// TEST(CommNetworkManger, RemoveNetwork)
//{
//    Address network1("192.168.1.1");
//    Address network2("222.23.1.34", 16);
//    Address network3;

//   std::string networkName1 = "default";
//   std::string networkName2 = "red";
//   std::string networkName3 = "automatic";

//   NetworkManager manager(nullptr);

//   ASSERT_TRUE(manager.AddNetwork(0.0, networkName1, network1));
//   ASSERT_TRUE(manager.AddNetwork(0.0, networkName2, network2));
//   ASSERT_TRUE(manager.AddNetwork(0.0, networkName3, network3));

//   EXPECT_TRUE(manager.GetNetwork("default"));
//   EXPECT_TRUE(manager.GetNetwork("red"));
//   EXPECT_TRUE(manager.GetNetwork("automatic"));

//   //! Add some comm devices to the "default" network, and then remove the network.
//   //! The associated comm devices should also be removed and available for reassignment.
//   auto address1 = manager.AddComm(0.0, commPtr1, networkName1);
//   auto address2 = manager.AddComm(0.0, commPtr2, networkName1);
//   EXPECT_TRUE(address1 != Address());
//   EXPECT_TRUE(address2 != Address());

//   manager.RemoveNetwork(0.0, networkName1);
//   EXPECT_FALSE(manager.GetNetwork(networkName1));
//   EXPECT_FALSE(manager.GetComm(address1));
//   EXPECT_FALSE(manager.GetComm(address2));
//   EXPECT_FALSE(manager.GetIsCommManaged(commPtr1));
//   EXPECT_FALSE(manager.GetIsCommManaged(commPtr2));
//}

// TEST(CommNetworkManger, EnableDisableComm)
//{
//    Address address1("192.168.1.0");
//    Address address2("192.168.1.1");
//    Address address3("192.168.1.2");
//    Address address4("192.168.1.3");

//   std::string networkName = "test_network";

//   NetworkManager manager(nullptr);

//   manager.AddNetwork(0.0, networkName, address1);
//   ASSERT_TRUE(networkName == manager.AddComm(0.0, commPtr1, address1));
//   ASSERT_TRUE(networkName == manager.AddComm(0.0, commPtr2, address2));
//   ASSERT_TRUE(networkName == manager.AddComm(0.0, commPtr3, address3));
//   ASSERT_TRUE(networkName == manager.AddComm(0.0, commPtr4, address4));

//   EXPECT_TRUE(manager.AddConnection(0.0, address1, address2));
//   EXPECT_TRUE(manager.AddConnection(0.0, address2, address3));
//   EXPECT_TRUE(manager.AddConnection(0.0, address3, address4));
//   EXPECT_TRUE(manager.AddConnection(0.0, address1, address4));

//   graph::AddressList addressList;
//   double cost = 0.0;
//   router::LeastHopsAlgorithm leastHops;
//   EXPECT_TRUE(manager.GetGraph()->FindPath(address1, address4, addressList, cost, &leastHops));
//   EXPECT_TRUE(addressList.size() == 2);

//   EXPECT_TRUE(manager.DisableComm(0.0, address3));
//   EXPECT_TRUE(manager.GetGraph()->FindPath(address1, address4, addressList, cost, &leastHops));
//   EXPECT_TRUE(addressList.size() == 2);
//   EXPECT_FALSE(manager.GetGraph()->FindPath(address1, address3, addressList, cost, &leastHops));

//   EXPECT_TRUE(manager.EnableComm(0.0, address3));
//   EXPECT_TRUE(manager.GetGraph()->FindPath(address1, address4, addressList, cost, &leastHops));
//   EXPECT_TRUE(addressList.size() == 2);

//   EXPECT_TRUE(manager.DisableComm(0.0, address4));
//   EXPECT_FALSE(manager.GetGraph()->FindPath(address1, address4, addressList, cost, &leastHops));

//   EXPECT_TRUE(manager.EnableComm(0.0, address4));
//   EXPECT_TRUE(manager.GetGraph()->FindPath(address1, address4, addressList, cost, &leastHops));
//   EXPECT_TRUE(addressList.size() == 2);
//}

// TEST(CommNetworkManager, AddRemoveConnection)
//{
//    Address address1("192.168.1.0");
//    Address address2("192.168.1.1");
//    Address address3("192.168.1.2");
//    Address address4("192.168.1.3");

//   std::string networkName = "test_network";

//   NetworkManager manager(nullptr);

//   manager.AddNetwork(0.0, networkName, address1);
//   ASSERT_TRUE(networkName == manager.AddComm(0.0, commPtr1, address1));
//   ASSERT_TRUE(networkName == manager.AddComm(0.0, commPtr2, address2));
//   ASSERT_TRUE(networkName == manager.AddComm(0.0, commPtr3, address3));
//   ASSERT_TRUE(networkName == manager.AddComm(0.0, commPtr4, address4));

//   EXPECT_TRUE(manager.AddConnection(0.0, address1, address2));
//   EXPECT_TRUE(manager.AddConnection(0.0, address2, address3));
//   EXPECT_TRUE(manager.AddConnection(0.0, address3, address4));
//   EXPECT_TRUE(manager.AddConnection(0.0, address1, address4));

//   graph::AddressList addressList;
//   double cost = 0.0;
//   router::LeastHopsAlgorithm leastHops;
//   EXPECT_TRUE(manager.GetGraph()->FindPath(address1, address4, addressList, cost, &leastHops));
//   EXPECT_TRUE(addressList.size() == 2);

//   manager.RemoveConnection(0.0, address1, address4);
//   EXPECT_TRUE(manager.GetGraph()->FindPath(address1, address4, addressList, cost, &leastHops));
//   EXPECT_TRUE(addressList.size() == 4);

//   EXPECT_TRUE(manager.AddConnection(0.0, address1, address4));
//   EXPECT_TRUE(manager.GetGraph()->FindPath(address1, address4, addressList, cost, &leastHops));
//   EXPECT_TRUE(addressList.size() == 2);
//}

// TEST(CommNetworkManager, EnableDisableConnection)
//{
//    Address address1("192.168.1.0");
//    Address address2("192.168.1.1");
//    Address address3("192.168.1.2");
//    Address address4("192.168.1.3");

//   std::string networkName = "test_network";

//   NetworkManager manager(nullptr);

//   manager.AddNetwork(0.0, networkName, address1);
//   ASSERT_TRUE(networkName == manager.AddComm(0.0, commPtr1, address1));
//   ASSERT_TRUE(networkName == manager.AddComm(0.0, commPtr2, address2));
//   ASSERT_TRUE(networkName == manager.AddComm(0.0, commPtr3, address3));
//   ASSERT_TRUE(networkName == manager.AddComm(0.0, commPtr4, address4));

//   EXPECT_TRUE(manager.AddConnection(0.0, address1, address2));
//   EXPECT_TRUE(manager.AddConnection(0.0, address2, address3));
//   EXPECT_TRUE(manager.AddConnection(0.0, address3, address4));
//   EXPECT_TRUE(manager.AddConnection(0.0, address1, address4));

//   graph::AddressList addressList;
//   double cost = 0.0;
//   router::LeastHopsAlgorithm leastHops;
//   EXPECT_TRUE(manager.GetGraph()->FindPath(address1, address4, addressList, cost, &leastHops));
//   EXPECT_TRUE(addressList.size() == 2);

//   EXPECT_TRUE(manager.DisableConnection(0.0, address1, address4));
//   EXPECT_TRUE(manager.GetGraph()->FindPath(address1, address4, addressList, cost, &leastHops));
//   EXPECT_TRUE(addressList.size() == 4);

//   EXPECT_TRUE(manager.EnableConnection(0.0, address1, address4));
//   EXPECT_TRUE(manager.GetGraph()->FindPath(address1, address4, addressList, cost, &leastHops));
//   EXPECT_TRUE(addressList.size() == 2);
//}

// TEST(CommNetworkManager, CreateNetworkConnections)
//{
//    Address address1("192.168.1.0");
//    Address address2("192.168.1.1");
//    Address address3("192.168.1.2");
//    Address address4("192.168.1.3");

//   std::vector<Address> addressVec = { address1, address2, address3, address4 };

//   std::string networkName = "test_network";
//   router::LeastHopsAlgorithm leastHops;

//   NetworkManager manager(nullptr);

//   //! TEST POINT-TO-POINT
//   manager.AddNetwork(0.0, networkName, address1);
//   ASSERT_TRUE(networkName == manager.AddComm(0.0, commPtr1, address1));
//   ASSERT_TRUE(networkName == manager.AddComm(0.0, commPtr2, address2));

//   EXPECT_TRUE(manager.CreateNetworkConnections(0.0,
//                                                networkName,
//                                                NetworkManager::NetworkTopology::cPOINT_TO_POINT));

//   graph::AddressList addressList;
//   double cost = 0.0;
//   EXPECT_TRUE(manager.GetGraph()->FindPath(address1, address2, addressList, cost, &leastHops));
//   EXPECT_TRUE(addressList.size() == 2);
//   EXPECT_TRUE(manager.GetGraph()->FindPath(address2, address1, addressList, cost, &leastHops));
//   EXPECT_TRUE(addressList.size() == 2);
//   manager.Clear();

//   //! TEST MESH
//   manager.AddNetwork(0.0, networkName, address1);
//   ASSERT_TRUE(networkName == manager.AddComm(0.0, commPtr1, address1));
//   ASSERT_TRUE(networkName == manager.AddComm(0.0, commPtr2, address2));
//   ASSERT_TRUE(networkName == manager.AddComm(0.0, commPtr3, address3));
//   ASSERT_TRUE(networkName == manager.AddComm(0.0, commPtr4, address4));

//   EXPECT_TRUE(manager.CreateNetworkConnections(0.0,
//                                          networkName,
//                                          NetworkManager::NetworkTopology::cMESH));

//   for (size_t i = 0; i < 4; ++i)
//   {
//      for (size_t j = 0; j < 4; ++j)
//      {
//         if (i != j)
//         {
//            EXPECT_TRUE(manager.GetGraph()->FindPath(addressVec[i], addressVec[j], addressList, cost, &leastHops));
//            EXPECT_TRUE(addressList.size() == 2);
//         }
//      }
//   }
//   manager.Clear();

//   //! TEST STAR
//   manager.AddNetwork(0.0, networkName, address1);
//   ASSERT_TRUE(networkName == manager.AddComm(0.0, commPtr1, address1));
//   ASSERT_TRUE(networkName == manager.AddComm(0.0, commPtr2, address2));
//   ASSERT_TRUE(networkName == manager.AddComm(0.0, commPtr3, address3));
//   ASSERT_TRUE(networkName == manager.AddComm(0.0, commPtr4, address4));

//   EXPECT_TRUE(manager.CreateNetworkConnections(0.0,
//                                                networkName,
//                                                NetworkManager::NetworkTopology::cSTAR,
//                                                false,
//                                                addressVec));
//
//   //! Hub to spoke
//   EXPECT_TRUE(manager.GetGraph()->FindPath(address1, address2, addressList, cost, &leastHops));
//   EXPECT_TRUE(addressList.size() == 2);
//   EXPECT_TRUE(manager.GetGraph()->FindPath(address1, address3, addressList, cost, &leastHops));
//   EXPECT_TRUE(addressList.size() == 2);
//   EXPECT_TRUE(manager.GetGraph()->FindPath(address1, address4, addressList, cost, &leastHops));
//   EXPECT_TRUE(addressList.size() == 2);

//   //! Spoke to hub
//   EXPECT_TRUE(manager.GetGraph()->FindPath(address2, address1, addressList, cost, &leastHops));
//   EXPECT_TRUE(addressList.size() == 2);
//   EXPECT_TRUE(manager.GetGraph()->FindPath(address3, address1, addressList, cost, &leastHops));
//   EXPECT_TRUE(addressList.size() == 2);
//   EXPECT_TRUE(manager.GetGraph()->FindPath(address4, address1, addressList, cost, &leastHops));
//   EXPECT_TRUE(addressList.size() == 2);

//   //! Spoke to spoke
//   EXPECT_TRUE(manager.GetGraph()->FindPath(address2, address3, addressList, cost, &leastHops));
//   EXPECT_TRUE(addressList.size() == 3);
//   EXPECT_TRUE(manager.GetGraph()->FindPath(address3, address2, addressList, cost, &leastHops));
//   EXPECT_TRUE(addressList.size() == 3);
//   EXPECT_TRUE(manager.GetGraph()->FindPath(address3, address4, addressList, cost, &leastHops));
//   EXPECT_TRUE(addressList.size() == 3);
//   EXPECT_TRUE(manager.GetGraph()->FindPath(address4, address3, addressList, cost, &leastHops));
//   EXPECT_TRUE(addressList.size() == 3);
//   EXPECT_TRUE(manager.GetGraph()->FindPath(address2, address4, addressList, cost, &leastHops));
//   EXPECT_TRUE(addressList.size() == 3);
//   EXPECT_TRUE(manager.GetGraph()->FindPath(address4, address2, addressList, cost, &leastHops));
//   EXPECT_TRUE(addressList.size() == 3);
//   manager.Clear();

//   //! TEST RING
//   manager.AddNetwork(0.0, networkName, address1);
//   ASSERT_TRUE(networkName == manager.AddComm(0.0, commPtr1, address1));
//   ASSERT_TRUE(networkName == manager.AddComm(0.0, commPtr2, address2));
//   ASSERT_TRUE(networkName == manager.AddComm(0.0, commPtr3, address3));
//   ASSERT_TRUE(networkName == manager.AddComm(0.0, commPtr4, address4));

//   EXPECT_TRUE((manager.CreateNetworkConnections(0.0,
//                                                networkName,
//                                                NetworkManager::NetworkTopology::cRING,
//                                                false,
//                                                addressVec)));

//   for (size_t i = 0; i < 4; ++i)
//   {
//      for (size_t j = 0; j < 4; ++j)
//      {
//         if (i != j)
//         {
//            EXPECT_TRUE(manager.GetGraph()->FindPath(addressVec[i], addressVec[j], addressList, cost, &leastHops));
//            if ((i == 0 && j == 3) || (i == 3 && j == 0))
//            {
//               EXPECT_TRUE(addressList.size() == 2);
//            }
//            else
//            {
//               EXPECT_TRUE(addressList.size() == (size_t)((std::abs((int)i - (int)j) + 1)));
//            }
//         }
//      }
//   }
//   manager.Clear();

//   //! TEST DIRECTED RING
//   manager.AddNetwork(0.0, networkName, address1);
//   ASSERT_TRUE(networkName == manager.AddComm(0.0, commPtr1, address1));
//   ASSERT_TRUE(networkName == manager.AddComm(0.0, commPtr2, address2));
//   ASSERT_TRUE(networkName == manager.AddComm(0.0, commPtr3, address3));
//   ASSERT_TRUE(networkName == manager.AddComm(0.0, commPtr4, address4));

//   EXPECT_TRUE(manager.CreateNetworkConnections(0.0,
//                                                networkName,
//                                                NetworkManager::NetworkTopology::cDIRECTED_RING,
//                                                false,
//                                                addressVec));

//   EXPECT_TRUE(manager.GetGraph()->FindPath(address1, address2, addressList, cost, &leastHops));
//   EXPECT_TRUE(addressList.size() == 2);
//   EXPECT_TRUE(manager.GetGraph()->FindPath(address1, address3, addressList, cost, &leastHops));
//   EXPECT_TRUE(addressList.size() == 3);
//   EXPECT_TRUE(manager.GetGraph()->FindPath(address1, address4, addressList, cost, &leastHops));
//   EXPECT_TRUE(addressList.size() == 4);
//   EXPECT_TRUE(manager.GetGraph()->FindPath(address2, address1, addressList, cost, &leastHops));
//   EXPECT_TRUE(addressList.size() == 4);
//   EXPECT_TRUE(manager.GetGraph()->FindPath(address2, address3, addressList, cost, &leastHops));
//   EXPECT_TRUE(addressList.size() == 2);
//   EXPECT_TRUE(manager.GetGraph()->FindPath(address2, address4, addressList, cost, &leastHops));
//   EXPECT_TRUE(addressList.size() == 3);
//   EXPECT_TRUE(manager.GetGraph()->FindPath(address3, address1, addressList, cost, &leastHops));
//   EXPECT_TRUE(addressList.size() == 3);
//   EXPECT_TRUE(manager.GetGraph()->FindPath(address3, address2, addressList, cost, &leastHops));
//   EXPECT_TRUE(addressList.size() == 4);
//   EXPECT_TRUE(manager.GetGraph()->FindPath(address3, address4, addressList, cost, &leastHops));
//   EXPECT_TRUE(addressList.size() == 2);
//   EXPECT_TRUE(manager.GetGraph()->FindPath(address4, address1, addressList, cost, &leastHops));
//   EXPECT_TRUE(addressList.size() == 2);
//   EXPECT_TRUE(manager.GetGraph()->FindPath(address4, address2, addressList, cost, &leastHops));
//   EXPECT_TRUE(addressList.size() == 3);
//   EXPECT_TRUE(manager.GetGraph()->FindPath(address4, address3, addressList, cost, &leastHops));
//   EXPECT_TRUE(addressList.size() == 4);
//}

} // namespace comm
} // namespace wsf
