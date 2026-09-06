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

#include "WsfCommNetworkManager.hpp"

#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "WsfComm.hpp"
#include "WsfCommNetworkLayer.hpp"
#include "WsfCommNetworkMesh.hpp"
#include "WsfCommNetworkTypes.hpp"
#include "WsfCommObserver.hpp"
#include "WsfCommRouter.hpp"
#include "WsfCommRouterProtocolInterface.hpp"
#include "WsfCommUtil.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformObserver.hpp"
#include "WsfSimulation.hpp"


namespace wsf
{
namespace comm
{

NetworkManager* NetworkManager::Find(const WsfSimulation& aSimulation)
{
   return static_cast<NetworkManager*>(aSimulation.FindExtension("comm_network_manager"));
}

// =================================================================================================
NetworkManager::NetworkManager(WsfSimulation* aSimPtr)
   : mSimulationPtr(aSimPtr)
   , mAddressToCommMap()
   , mCommToAddressMap()
   , mAddressToNetworkMap()
   , mNetworkMap()
   , mNetworkAddressSet()
   , mGraph()
   , mCallbacks()
   , mReservedAddresses()
   , mMulticastMap()
   , mComms()
   , mRouters()
{
}

// =================================================================================================
bool NetworkManager::Initialize()
{
   Clear();

   // NOTE: The PlatformInitialized callback is not subscribed to until PendingStart(). PlatformAdded is intended for
   // use by the network manager ONLY for platforms being added during simulation runtime.
   mCallbacks.Add(WsfObserver::PlatformDeleted(mSimulationPtr).Connect(&wsf::comm::NetworkManager::PlatformDeleted, this));

   //! Initialize reserved addressing
   ReservedAddressing::UniqueReserved reservedBase = ut::make_unique<ReservedAddressBase>();
   mReservedAddresses.Register(std::move(reservedBase));
   mReservedAddresses.Initialize();

   // Create the default network
   auto    reservedAddressDefault = mReservedAddresses.QueryAddressByType(reserved::cDEFAULT_INITIAL);
   Address defaultAddress(reservedAddressDefault.GetAddress(), 16);
   auto    networkLegacy = ut::make_unique<NetworkMeshLegacy>("default", defaultAddress);
   AddNetwork(GetSimulation()->GetSimTime(), std::move(networkLegacy), true);

   return true;
}

// =================================================================================================
// This override of the simulation extension method is called after platforms are available via the simulation
// object, and all platforms (and the objects they contain, most importantly comms in this application) have
// undergone initialization, but have not yet undergone calls to Initialize2(). Processes for setting up
// addressing and initial network state are driven from here to explicitly make this functionality is available
// within code and to the user interface during Initialize2, such that communications better conforms and
// provides the functionality expected to be available in some format and at some point during
// the initialization process.
bool NetworkManager::PlatformsInitialized()
{
   // All comm based initialization occurs at this time, when every platform (and subsequently,
   // every comm component) is ready and exists in the simulation just prior to platform/component
   // calls to their Initialize2 methods. The network manager then drives the following operations:
   // 1. The "default" network is instantiated
   // 2. Any user defined networks via scenario input are instantiated. Comms specified for
   //    membership in this network are modified to specify the network they should belong to
   //    (as they would revert to the "default" network, otherwise)
   // 3. All comms are iterated through, using the AddComm() method. This does the following:
   //    A. Adds the comm to the network manager for control in the sim.
   //    B. If the comm specifies a new network, it is created (default mesh topology only)
   //    C. The comm is added to an existing network, or the one just created in step 3B.
   //    D. The network itself, depending on type, creates linkage between members when a
   //       new comm interface is added.
   //    E. Any user defined linkage is established for the comm.
   //
   // NOTE: Any comm added at runtime goes through the same procedure in step 3, except this is
   // driven by the "PlatformInitialized" callback.

   // A temporary map of named comms to network membership. These are provided by the user defined
   // network membership lists. These are stored when the network is created so that when the comm is
   // added, the correct network is specified. Faster than attempting to find the comm object in a vector
   // and modifying it directly as a separate step.
   std::unordered_map<Network::NamedComm, std::string> networkMemberMap;

   // Step 1 - Create the default network
   // DEPRECATED - This can be conducted prior to this point, and is now already accomplished
   // in the simulation extension override of the Initialization method.

   // Step 2 - Create User Defined Network Types
   std::vector<Network*> userNetworkPtrs;
   NetworkTypes::Get(GetSimulation()->GetScenario()).GetCurrentUserTypes(userNetworkPtrs);
   for (auto networkPtr : userNetworkPtrs)
   {
      auto addNetworkPtr = ut::clone(networkPtr);
      if (!AddNetwork(GetSimulation()->GetSimTime(), std::move(addNetworkPtr)))
      {
         throw UtException("Unable to add user defined network: " + networkPtr->GetName());
      }

      // If the user network has members defined by name, save those now.
      auto namedList = networkPtr->GetNamedMemberList();
      for (const auto& namedPair : namedList)
      {
         networkMemberMap.insert(std::make_pair(namedPair, networkPtr->GetType()));
      }
   }

   // Step 3 - Add comms to network manager
   // Note: This is where initialization can be performance bound for comms due to the required
   // multiple iterations. However, this is required, as we cannot establish linkage until
   // addressing is resolved for all comms, and we do not want to subscribe to callbacks until
   // linkage is complete. The time is nearly the same even if these operations were possible
   // to complete in a single loop. This also orders output for consistency.
   for (auto commPtr : mComms)
   {
      // First check if this comm is specified for a user defined network.
      Network::NamedComm namedComm;
      namedComm.mPlatformName = commPtr->GetPlatform()->GetNameId();
      namedComm.mCommName     = commPtr->GetNameId();
      auto it                 = networkMemberMap.find(namedComm);

      const auto& userAddress        = commPtr->GetInputAddress();
      auto        userNetwork        = commPtr->GetInputNetwork();
      const auto& userNetworkAddress = commPtr->GetInputNetworkAddress();

      if (it != std::end(networkMemberMap))
      {
         if (!userNetwork.empty() && (it->second != userNetwork))
         {
            auto out = ut::log::warning() << "Comm has conflicting network assignment.";
            out.AddNote() << "Comm: " << commPtr->GetFullName();
            out.AddNote() << "Assigned Network: " << commPtr->GetNetwork();
            out.AddNote() << "Found in Network: " << it->second;
         }

         userNetwork = it->second;
      }

      // This call uses the correct AddComm method depending on user supplied input.
      // It adds the comms to the network manager, the correct network, and dynamically
      // creates any links associated with the network type (depending on implementation).
      ManageComm(0.0, commPtr, userNetwork, userAddress, userNetworkAddress);

      //! Set the router for this comm
      auto routerPtr = commPtr->GetPlatform()->GetComponent<router::Router>(commPtr->GetRouterName());
      if (!routerPtr)
      {
         auto out = ut::log::error() << "Assigned router not found on platform.";
         out.AddNote() << "Comm: " << commPtr->GetFullName();
         out.AddNote() << "Router: " << commPtr->GetRouterName();
         out.AddNote() << "Ensure the specified router name is correct.";
         throw UtException("Specified router not found in NetworkManager::PlatformsInitialized().");
      }

      commPtr->SetRouter(routerPtr);

      // We have to also add the interface to the router at this time, since we are not subscribed
      // yet to PlatformInitialized()
      routerPtr->AddInterface(0.0, commPtr);
   }

   // Now that all comms have addresses, allow networks to create their linkage. Some networks
   // do this dynamically when they add members. The usage of this is based on the network
   // implementation. (E.g. Mesh networks create all linkage when members are added, so this call
   // does nothing, but a Generic network has ALL links set up in this manner)
   for (auto& networkMapEntry : mNetworkMap)
   {
      networkMapEntry.second->InitializeLinkage(*GetSimulation());
   }

   // Once all comms are added and have addresses assigned, we can now add user defined linkage.
   // This has to happen in a separate iteration of the managed comms, as these links rely on every
   // interface having an address. This also creates the linkage for routers amongst their interfaces,
   // assuming the user has selected this option.
   for (auto commPtr : mComms)
   {
      InitializeUserLinks(0.0, commPtr);
   }

   // Iterate through routers to hook up their callbacks. We don't want
   // this happening until this point to ensure callbacks aren't triggered until the sim starts.
   for (auto routerPtr : mRouters)
   {
      routerPtr->InitializeCallbacks();
      routerPtr->Setup();
   }

   // Allow each comm to now call Setup() on any components using this method.
   for (auto commPtr : mComms)
   {
      commPtr->Setup();
   }

   return true;
}

// =================================================================================================
void NetworkManager::ManageComm(double             aSimTime,
                                Comm*              aCommPtr,
                                const std::string& aNetworkName,
                                const Address&     aAddress,
                                const Address&     aNetworkAddress)
{
   if (aAddress.IsNull() && aNetworkAddress.IsNull() && aNetworkName.empty())
   {
      //! The user didn't specify any addressing or network usage parameters.
      //! This comm device will be automatically assigned an address within the "default" network.
      std::string defaultNetwork = "default";
      auto        address        = AddComm(aSimTime, aCommPtr, defaultNetwork);

      if (address.IsNull() || (defaultNetwork != "default"))
      {
         //! Note: We should never reach this branch during normal operating conditions. If this occurs, we need
         //! to re-evaluate the number of default comm objects allowed in AFSIM, and perhaps allow it to be defined.
         ut::log::error() << "Maximum number of comm objects defined on default network.";
         throw UtException("Default comm object network limit reached.");
      }
   }
   else if ((!aNetworkName.empty()) && aAddress.IsNull() && aNetworkAddress.IsNull())
   {
      //! LEGACY SUPPORT
      //! Check if the network name is specified to local:master or local:slave and update
      //! the value accordingly.

      auto tempNetworkName = aNetworkName;

      if (aNetworkName == "<local:master>")
      {
         tempNetworkName = util::LocalMasterNetworkName(*aCommPtr);
      }
      else if (aNetworkName == "<local:slave>")
      {
         tempNetworkName = util::LocalSlaveNetworkName(*aCommPtr);
      }

      //! The network name (string) was set. Provide appropriate addressing based on network
      //! information.
      auto address = AddComm(aSimTime, aCommPtr, tempNetworkName);

      if (address.IsNull())
      {
         auto out = ut::log::error() << "Error assigning comm to network.";
         out.AddNote() << "Network: " << tempNetworkName;
         out.AddNote() << "Comm: " << aCommPtr->GetName();
         out.AddNote() << "Ensure the network can accommodate the required number of hosts.";
         out.AddNote()
            << "All networks not defined by the user have a limit of 255 members except the default network.";

         throw UtException("Error in comm network assignment.");
      }
   }
   else if ((!aNetworkAddress.IsNull()) && (aNetworkName.empty()) && aAddress.IsNull())
   {
      //! The network address was set. Create or join a network with that address and assign
      //! an appropriate address.
      auto networkAddressPtr = GetManagingNetworkAddress(aNetworkAddress);
      if (!networkAddressPtr)
      {
         //! The network associated with this address doesn't exist. Attempt to create it.
         std::unique_ptr<Network> networkMesh =
            ut::make_unique<NetworkMeshLegacy>(aCommPtr->GetFullName(), aNetworkAddress);
         auto result = AddNetwork(aSimTime, std::move(networkMesh));
         if (!result)
         {
            //! Network creation failed.
            auto out = ut::log::error() << "Cannot create network of same name.";
            out.AddNote() << "Address: " << aNetworkAddress;
            out.AddNote() << "Comm: " << aCommPtr->GetFullName();
            throw UtException("Error in comm network creation.");
         }

         std::string networkName = aCommPtr->GetFullName();
         auto        commAddress = AddComm(aSimTime, aCommPtr, networkName);
      }
      else
      {
         //! Network exists. Join it and assign an address.
         auto networkName = GetNetworkNameFromAddress(aNetworkAddress);
         auto address     = AddComm(aSimTime, aCommPtr, networkName);
         if (address.IsNull())
         {
            auto out = ut::log::error() << "Error assigning comm to network.";
            out.AddNote() << "Network: " << networkName;
            out.AddNote() << "Comm: " << aCommPtr->GetName();
            out.AddNote() << "Ensure the network can accommodate the required number of hosts.";
            out.AddNote()
               << "All networks not defined by the user have a limit of 255 members except the default network.";
            throw UtException("Error in comm network assignment.");
         };
      }
   }
   else if ((!aAddress.IsNull()) && (aNetworkName.empty()) && aNetworkAddress.IsNull())
   {
      //! Address was set. Attempt to assign the comm device to that address, with the resulting
      //! network inclusion. If no network exists, one will be made using the address values.
      auto networkName = AddComm(aSimTime, aCommPtr, aAddress);
      if (networkName.empty())
      {
         auto out = ut::log::error() << "Error assigning comm to network address.";
         out.AddNote() << "Address: " << aAddress;
         out.AddNote() << "Comm: " << aCommPtr->GetName();
         out.AddNote() << "Ensure the network can accommodate the required number of hosts.";
         out.AddNote()
            << "All networks not defined by the user have a limit of 255 members except the default network.";
         throw UtException("Error in comm network assignment.");
      }
   }
   else
   {
      //! The user defined multiple values for network and/or network assignment, resulting in
      //! ambiguous usage. Inform the user and return failure.
      auto out = ut::log::error() << "Too many inputs for comm.";
      if (!aAddress.IsNull())
      {
         out.AddNote() << "Address: " << aAddress;
      }
      if (!aNetworkAddress.IsNull())
      {
         out.AddNote() << "Network Address: " << aNetworkAddress;
      }
      if (!aNetworkName.empty())
      {
         out.AddNote() << "Network Name: " << aNetworkName;
      }
      out.AddNote() << "Can only specify one.";

      throw UtException("Ambiguous comm object address assignment.");
   }
}

// =================================================================================================
void NetworkManager::PendingStart()
{
   for (auto routerPtr : mRouters)
   {
      routerPtr->PendingStart();
   }

   // Allow each comm to now call PendingStart() on any components using this method.
   for (auto commPtr : mComms)
   {
      commPtr->PendingStart();
   }

   // Finally, subscribe to the PlatformInitialized callback for runtime comm management. It's safe to do
   // so now as the simulation has already added all of the platforms indicated for sim start.
   mCallbacks.Add(
      WsfObserver::PlatformInitialized(mSimulationPtr).Connect(&wsf::comm::NetworkManager::PlatformInitialized, this));

   //! Notify callbacks
   CommFrameworkPendingStart();
}

// =================================================================================================
void NetworkManager::InitializeUserLinks(double aSimTime, Comm* aCommPtr)
{
   auto                                    routerPtr = aCommPtr->GetRouter();
   std::vector<router::ProtocolInterface*> protocols;

   if (routerPtr)
   {
      protocols = routerPtr->GetSortedProtocols();
   }
   else
   {
      auto out = ut::log::error() << "Missing router assignment for comm.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Comm: " << aCommPtr->GetFullName();
      out.AddNote() << "Ensure the specified 'router_name' references an existing router on this platform.";
      throw UtException("Missing required comm/router association in NetworkManager.");
   }

   for (const auto& linkAddress : aCommPtr->GetLinkAddresses())
   {
      //! If the link being added is between members of the same network,
      //! then the network will determine if the link is valid. Otherwise, this
      //! is a link between networks, and can be established directly.
      //! Note: Currently, this behavior affects all protocols. This could be
      //! changed to a protocol by protocol basis in the future, if the need exists.
      auto commPtr = GetComm(linkAddress);
      if (commPtr)
      {
         if (aCommPtr->GetNetwork() == commPtr->GetNetwork())
         {
            auto addedLink =
               GetNetwork(aCommPtr->GetNetwork())->AddLink(aCommPtr->GetAddress(), commPtr->GetAddress(), *GetSimulation());

            if (routerPtr && addedLink)
            {
               for (auto protocolPtr : protocols)
               {
                  //! Truth will be propagated via callback. Only push in the non-truth usage case, as
                  //! these links are user directed.
                  if (!protocolPtr->ShouldPropagateTruth() && protocolPtr->GetGraph())
                  {
                     routerPtr->AddLink(aSimTime, aCommPtr->GetAddress(), linkAddress, *protocolPtr);
                  }
               }
            }
            else
            {
               ut::log::warning() << "Failed to add connection to comm network manager.";
            }
         }
         else
         {
            auto addedLink = AddConnection(aSimTime, aCommPtr, GetComm(linkAddress), false);

            if (routerPtr && addedLink)
            {
               for (auto protocolPtr : protocols)
               {
                  //! Truth will be propagated via callback. Only push in the non-truth usage case, as
                  //! these links are user directed.
                  if (!protocolPtr->ShouldPropagateTruth() && protocolPtr->GetGraph())
                  {
                     routerPtr->AddLink(aSimTime, aCommPtr->GetAddress(), linkAddress, *protocolPtr);
                  }
               }
            }
            else
            {
               ut::log::warning() << "Failed to add connection to comm network manager.";
            }
         }
      }
   }

   //! We can use the network manager to find the correct address of the provided
   //! platform name + comm name pairs, as this data was already propagated to the network
   //! manager during Initialize() calls by the various WsfComm objects.
   for (auto& link : aCommPtr->GetLinkPairs())
   {
      auto platformName = link.first;
      auto commName     = link.second;

      //! If the platform name is a null string, then this is a local connection.
      //! The name of the platform relevant to this link is the current platform owning
      //! this layer's comm.
      if (platformName.empty())
      {
         platformName = aCommPtr->GetPlatform()->GetName();
      }

      auto platformPtr = GetSimulation()->GetPlatformByName(platformName);

      if (!platformPtr)
      {
         auto out = ut::log::error() << "Platform not found during external link creation in network manager.";
         out.AddNote() << "Missing Platform: " << platformName;
         out.AddNote() << "Comm: " << aCommPtr->GetFullName();
         throw UtException("Platform not found during external link creation in network manager.");
      }

      auto commPtr = platformPtr->GetComponent<Comm>(commName);
      if (!commPtr)
      {
         auto out = ut::log::error() << "Comm not found during external link creation in network manager.";
         out.AddNote() << "Missing Comm: " << platformName << "." << commName;
         out.AddNote() << "Comm: " << aCommPtr->GetFullName();
         throw UtException("Comm not found during external link creation in network manager.");
      }

      const auto& destinationAddress = commPtr->GetAddress();
      if (destinationAddress.IsNull())
      {
         auto out = ut::log::error() << "Comm address not found during external link creation in network manager.";
         out.AddNote() << "Comm: " << platformName << "." << commName;
         throw UtException("Comm address not found during external link creation in network manager.");
      }

      //! If the link being added is between members of the same network,
      //! then the network will determine if the link is valid. Otherwise, this
      //! is a link between network members, and can be established directly.
      if (aCommPtr->GetNetwork() == commPtr->GetNetwork())
      {
         auto addedLink =
            GetNetwork(aCommPtr->GetNetwork())->AddLink(aCommPtr->GetAddress(), destinationAddress, *GetSimulation());

         if (routerPtr && addedLink)
         {
            for (auto protocolPtr : protocols)
            {
               //! Truth will be propagated via callback. Only push in the non-truth usage case, as
               //! these links are user directed.
               if (!protocolPtr->ShouldPropagateTruth() && protocolPtr->GetGraph())
               {
                  routerPtr->AddLink(aSimTime, aCommPtr->GetAddress(), destinationAddress, *protocolPtr);
               }
            }
         }
         else
         {
            ut::log::warning() << "Failed to add connection to comm network manager.";
         }
      }
      else
      {
         auto addedLink = AddConnection(aSimTime, aCommPtr, commPtr, false);

         if (routerPtr && addedLink)
         {
            for (auto protocolPtr : protocols)
            {
               //! Truth will be propagated via callback. Only push in the non-truth usage case, as
               //! these links are user directed.
               if (!protocolPtr->ShouldPropagateTruth() && protocolPtr->GetGraph())
               {
                  routerPtr->AddLink(aSimTime, aCommPtr->GetAddress(), destinationAddress, *protocolPtr);
               }
            }
         }
         else
         {
            ut::log::warning() << "Failed to add connection to comm network manager.";
         }
      }
   } // for
}

// =================================================================================================
const Address* NetworkManager::GetAddress(Comm* aCommPtr) const
{
   auto it = mCommToAddressMap.find(aCommPtr);
   if (it != mCommToAddressMap.end())
   {
      return &(it->second);
   }
   return nullptr;
}

// =================================================================================================
Comm* NetworkManager::GetComm(const Address& aAddress) const
{
   auto it = mAddressToCommMap.find(aAddress);
   if (it != mAddressToCommMap.end())
   {
      return it->second;
   }
   return nullptr;
}

// =================================================================================================
Network* NetworkManager::GetNetwork(const std::string& aNetworkName) const
{
   auto it = mNetworkMap.find(aNetworkName);
   if (it != mNetworkMap.end())
   {
      return it->second.get();
   }
   return nullptr;
}

// =================================================================================================
bool NetworkManager::IsCommManaged(Comm* aCommPtr) const
{
   auto it = mCommToAddressMap.find(aCommPtr);
   if (it != mCommToAddressMap.end())
   {
      return true;
   }
   return false;
}

// =================================================================================================
bool NetworkManager::IsAddressManaged(const Address& aAddress) const
{
   auto it = mAddressToCommMap.find(aAddress);
   if (it != mAddressToCommMap.end())
   {
      return true;
   }
   return false;
}

// =================================================================================================
bool NetworkManager::IsNetworkManaged(const std::string& aNetworkName) const
{
   if (GetNetwork(aNetworkName))
   {
      return true;
   }
   return false;
}

// =================================================================================================
std::vector<std::string> NetworkManager::GetManagedNetworks() const
{
   std::vector<std::string> networks;
   for (auto& networkPair : mNetworkMap)
   {
      networks.push_back(networkPair.first);
   }

   return networks;
}

// =================================================================================================
bool NetworkManager::IsNetworkAddressManaged(const Address& aAddress) const
{
   auto addressPtr = GetManagingNetworkAddress(aAddress);
   if (addressPtr)
   {
      return true;
   }
   return false;
}

const Address* NetworkManager::GetManagingNetworkAddress(const Address& aAddress) const
{
   if (mNetworkAddressSet.empty())
   {
      return nullptr;
   }

   auto it = mNetworkAddressSet.lower_bound(aAddress);

   if (it == mNetworkAddressSet.end())
   {
      // Exact address not found, and at end of set. Decrement iterator, and check with previous
      // value to ensure range does not overlap. If they overlap, this Address is managed.
      --it;
      if (Address::CheckInAddressRange(*it, aAddress))
      {
         return &*it;
      }

      return nullptr;
   }
   else
   {
      // Lower bound found the exact address, or the next "higher" address. Check the surrounding
      // addresses for overlap. If none exists, this address is not managed.
      if (*it == aAddress)
      {
         return &*it;
      }

      auto previousIt = it;
      --previousIt;

      if (Address::CheckInAddressRange(*it, aAddress))
      {
         return &*it;
      }
      else if (previousIt != mNetworkAddressSet.end() && Address::CheckInAddressRange(*previousIt, aAddress))
      {
         return &*previousIt;
      }
      return nullptr;
   }
}

// =================================================================================================
std::string NetworkManager::GetNetworkNameFromAddress(const Address& aAddress) const
{
   auto it = mAddressToNetworkMap.find(aAddress);
   if (it != mAddressToNetworkMap.end())
   {
      return it->second;
   }
   return "";
}

// =================================================================================================
NetworkManager::AddressSet NetworkManager::GetAddressesInNetwork(const std::string& aNetworkName) const
{
   AddressSet addressList;
   auto       networkPtr = GetNetwork(aNetworkName);
   if (!networkPtr)
   {
      return addressList;
   }

   const Address& routing_prefix = networkPtr->GetAddress();

   for (auto& it : mAddressToCommMap)
   {
      if (it.first.GetRoutingPrefix() == routing_prefix.GetRoutingPrefix())
      {
         addressList.insert(it.first);
      }
   }

   return addressList;
}

// =================================================================================================
graph::Graph& NetworkManager::GetGraph()
{
   return mGraph;
}

// =================================================================================================
void NetworkManager::Clear()
{
   mAddressToCommMap.clear();
   mCommToAddressMap.clear();
   mAddressToNetworkMap.clear();
   mNetworkMap.clear();
   mNetworkAddressSet.clear();
   mGraph.Clear();
}

// =================================================================================================
Address NetworkManager::AddComm(double       aSimTime,
                                Comm*        aCommPtr,
                                std::string& aNetworkName,
                                bool         aNotifyObserver) // = true
{
   Address returnAddress;

   bool   skipNetworkCreation = false;
   size_t cidr_value          = 24;

   if (!aCommPtr)
   {
      ut::log::warning() << "A null referenced comm object cannot be managed by the network manager.";
      return returnAddress;
   }
   if (IsCommManaged(aCommPtr))
   {
      auto out = ut::log::warning() << "Comm is already managed by the network manager.";
      out.AddNote() << "Comm: " << aCommPtr->GetFullName();
      return returnAddress;
   }

   std::string    commName   = aCommPtr->GetFullName();
   const Network* curNetwork = nullptr;

   //! If the network name is not provided, set a network name using the device name.
   //! The network is defined with the comm name with a dot and an integer value, as
   //! it is possible that the defining network comm device may leave a network and
   //! re-establish a new one during the simulation, in which a network using that
   //! name would already be established. e.g. If the comm is named "platform.blue_comm",
   //! the initial typical network name would be "platform.blue_comm.1". If that comm device
   //! left that network, but later re-established a new network with an auto-generated
   //! name, that new network would be "platform.blue_comm.2". As such, names of networks should
   //! never assume inclusion of a comm device of the same name, and are only identifiers
   //! used to identify a particular network.
   if (aNetworkName.empty())
   {
      const std::string& networkName = commName;
      unsigned int       count       = 1;
      while (IsNetworkManaged(networkName + "." + std::to_string(count)))
      {
         ++count;
      }

      aNetworkName = networkName + "." + std::to_string(count);
   }
   else if (IsNetworkManaged(aNetworkName))
   {
      //! The user defined a network name. If this network already exists, set a flag
      //! so we don't create that network later. Also match the CIDR value for address
      //! creation to any existing network specification.
      skipNetworkCreation = true;
      curNetwork          = GetNetwork(aNetworkName);
      cidr_value          = curNetwork->GetAddress().GetSubnetMaskBitLength();
   }

   //! If the above conditional determined that a network needed to be created,
   //! do so now.
   if (!skipNetworkCreation)
   {
      Address networkAddress;
      networkAddress.GenerateAddress(aNetworkName);

      //! Get an address that isn't already maintained (used) and that isn't reserved.
      while (IsNetworkAddressManaged(networkAddress) || mReservedAddresses.IsReserved(networkAddress))
      {
         networkAddress.GenerateAddress(aNetworkName);
      }

      //! Add a default network object.
      //! Note that all default networks in AFSIM are mesh based.
      std::unique_ptr<Network> meshNetwork = ut::make_unique<NetworkMeshLegacy>(aNetworkName, networkAddress);
      auto                     result      = AddNetwork(aSimTime, std::move(meshNetwork));
      if (!result)
      {
         auto out = ut::log::warning() << "Comm cannot be managed by the network manager due to rejection of comm "
                                          "object addition to the specified network.";
         out.AddNote() << "Network: " << aNetworkName;
         out.AddNote() << "Comm: " << aCommPtr->GetFullName();
         return returnAddress;
      }
   }

   //! Now, regardless of case above, we have a network to add this comm device to.
   //! Use that network to generate an appropriate address for this device.
   auto        network        = GetNetwork(aNetworkName);
   const auto& networkAddress = network->GetAddress();

   //! Create an address that is only the base routing prefix of the network address.
   //! This represents the lowest available address assignable to this particular domain.
   Address commAddress(networkAddress.GetRoutingPrefix(), cidr_value);

   //! The initial address in an invalid *.0 address. Increment to start at the first available value,
   //! a *.001 address.
   ++commAddress;

   //! Check the address for current assignment in the simulation, incrementing the address until an
   //! available address is found. If we exhaust all available host addresses, return a failure.
   for (size_t i = 0; i < network->GetHostsMax(); ++i)
   {
      if (IsAddressManaged(commAddress))
      {
         //! Address exists. Increment address;
         ++commAddress;
      }
      else
      {
         //! Address does not exist in manager, use it for this device.
         mAddressToCommMap.insert(std::make_pair(commAddress, aCommPtr));
         mCommToAddressMap.insert(std::make_pair(aCommPtr, commAddress));
         mGraph.CreateNode(commAddress);

         //! Set the comm's address and network name
         aCommPtr->SetAddress(commAddress);
         aCommPtr->SetNetwork(aNetworkName);

         //! Add the comm to the network object. This may result in linkage.
         if (!network->AddMember(commAddress, *GetSimulation()))
         {
            //! We can't undo this earlier, due to the network needing these objects populated for
            //! for potential linking on success, so we have to undo them here on failure.
            mAddressToCommMap.erase(commAddress);
            mCommToAddressMap.erase(aCommPtr);
            mGraph.RemoveNode(commAddress);
            commAddress = Address();

            aCommPtr->SetAddress(commAddress);
            aCommPtr->SetNetwork(std::string());

            //! If we created a network for this failed add, remove it too.
            if (!skipNetworkCreation)
            {
               RemoveNetwork(aSimTime, aNetworkName);
            }
         }
         else
         {
            //! Notify the observer that a node was added to the manager
            if (aNotifyObserver)
            {
               mSimulationPtr->GetCommObserver().CommAddedToManager(aSimTime, aCommPtr);
            }

            //! Notify callbacks
            CommAdded(aSimTime, aCommPtr);
         }

         return commAddress;
      }
   }

   //! If we reach this point, it means that the network no longer had any assignable
   //! addresses within its subnet. Return the non-assigned address of 0.0.0.0 to
   //! indicate inability to assign this comm device to this network.
   return Address{};
}

// =================================================================================================
std::string NetworkManager::AddComm(double         aSimTime,
                                    Comm*          aCommPtr,
                                    const Address& aAddress,
                                    bool           aNotifyObserver) // =true
{
   std::string returnNetwork = "";

   if (!aCommPtr || aAddress.IsNull() || IsAddressManaged(aAddress) || IsCommManaged(aCommPtr))
   {
      return returnNetwork;
   }

   //! Find the network the user provided address belongs to, if it exists.
   auto networkAddress = GetManagingNetworkAddress(aAddress);
   if (networkAddress && networkAddress->GetSubnetMaskBitLength() == aAddress.GetSubnetMaskBitLength())
   {
      //! A managing network was found for the user address. Add the comm device to this network.
      returnNetwork = GetNetworkNameFromAddress(*networkAddress);
      mAddressToCommMap.insert(std::make_pair(aAddress, aCommPtr));
      mCommToAddressMap.insert(std::make_pair(aCommPtr, aAddress));
      mGraph.CreateNode(aAddress);

      //! Set the comm's address and network name
      aCommPtr->SetAddress(aAddress);
      aCommPtr->SetNetwork(returnNetwork);

      if (!GetNetwork(returnNetwork)->AddMember(aAddress, *GetSimulation()))
      {
         //! We can't undo this earlier, due to the network needing these objects populated for
         //! for potential linking on success, so we have to undo them here on failure.
         mAddressToCommMap.erase(aAddress);
         mCommToAddressMap.erase(aCommPtr);
         mGraph.RemoveNode(aAddress);
         returnNetwork = std::string();

         aCommPtr->SetAddress(Address());
         aCommPtr->SetNetwork(returnNetwork);
      }
      else
      {
         //! Notify the observer that a node was added to the manager
         if (aNotifyObserver)
         {
            mSimulationPtr->GetCommObserver().CommAddedToManager(aSimTime, aCommPtr);
         }

         //! Notify callback
         CommAdded(aSimTime, aCommPtr);
      }

      return returnNetwork;
   }
   else
   {
      //! No current managing network for this address. Create one for this address.
      std::string networkName = aCommPtr->GetFullName();
      size_t      counter     = 1;
      bool        foundName   = false;
      while (!foundName)
      {
         if (!IsNetworkManaged(networkName + "." + std::to_string(counter)))
         {
            foundName = true;
         }
         else
         {
            ++counter;
         }
      }

      networkName = networkName + "." + std::to_string(counter);
      Address networkAddress(aAddress.GetRoutingPrefix(), aAddress.GetSubnetMaskBitLength());

      //! The default network type is a mesh network.
      std::unique_ptr<Network> networkMesh = ut::make_unique<NetworkMeshLegacy>(networkName, networkAddress);
      if (!AddNetwork(aSimTime, std::move(networkMesh)))
      {
         return returnNetwork;
      }

      //! New network created. Add this address to indicate its being managed.
      returnNetwork = networkName;
      mAddressToCommMap.insert(std::make_pair(aAddress, aCommPtr));
      mCommToAddressMap.insert(std::make_pair(aCommPtr, aAddress));
      mGraph.CreateNode(aAddress);

      aCommPtr->SetAddress(aAddress);
      aCommPtr->SetNetwork(returnNetwork);

      if (!GetNetwork(networkName)->AddMember(aAddress, *GetSimulation()))
      {
         //! We can't undo this earlier, due to the network needing these objects populated for
         //! for potential linking on success, so we have to undo them here on failure.
         mAddressToCommMap.erase(aAddress);
         mCommToAddressMap.erase(aCommPtr);
         mGraph.RemoveNode(aAddress);
         RemoveNetwork(aSimTime, networkName);
         returnNetwork = std::string();

         aCommPtr->SetAddress(Address());
         aCommPtr->SetNetwork(returnNetwork);
      }
      else
      {
         //! Notify the observer that a node was added to the manager
         if (aNotifyObserver)
         {
            mSimulationPtr->GetCommObserver().CommAddedToManager(aSimTime, aCommPtr);
         }

         //! Notify callback
         CommAdded(aSimTime, aCommPtr);
      }

      return returnNetwork;
   }
}

// =================================================================================================
void NetworkManager::RemoveComm(double aSimTime, Comm* aCommPtr,
                                bool aNotifyObserver) // =true
{
   auto address = GetAddress(aCommPtr);
   RemoveComm(aSimTime, *address, aNotifyObserver);
}

// =================================================================================================
void NetworkManager::RemoveComm(double aSimTime, const Address& aAddress,
                                bool aNotifyObserver) // =true
{
   auto commPtr = GetComm(aAddress);
   if (!commPtr)
   {
      return;
   }

   std::string networkName = commPtr->GetNetwork();
   Address     address     = commPtr->GetAddress();

   // Get all the edges corresponding with the pending removal comm.
   // Even though these are implicitly removed with the removal of the comm/node,
   // we want observers/callbacks to be notified of their removal.
   auto allEdges = mGraph.GetAllNodeEdges(aAddress);
   for (auto edgePtr : allEdges)
   {
      RemoveConnection(aSimTime, edgePtr->GetSourceAddress(), edgePtr->GetDestinationAddress());
   }

   auto result = mGraph.RemoveNode(aAddress);
   if (result)
   {
      //! Notify the observer that a node is being removed from the manager
      if (aNotifyObserver)
      {
         mSimulationPtr->GetCommObserver().CommRemovedFromManager(aSimTime, commPtr);
      }

      //! Notify callback
      CommRemoved(aSimTime, commPtr);

      mAddressToCommMap.erase(aAddress);
      mCommToAddressMap.erase(commPtr);
      mAddressToNetworkMap.erase(aAddress);

      //! Update the affected comm.
      commPtr->SetAddress(Address());
      commPtr->SetNetwork(std::string());

      //! Notify the network losing the comm for internal management.
      GetNetwork(networkName)->RemoveMember(address, *GetSimulation());
   }
}

// =================================================================================================
bool NetworkManager::AddNetwork(double aSimTime, std::unique_ptr<Network> aNetwork, bool aSuppressReservedCheck /*= false*/)
{
   //! If a network with this name already exists, return false for failure to add network.
   if (IsNetworkManaged(aNetwork->GetType()))
   {
      return false;
   }

   //! Need copies of these, as std::move will invalidate aNetwork during processing.
   Address     networkAddress = aNetwork->GetAddress();
   std::string networkName    = aNetwork->GetType();

   //! Check for a user supplied address that is not the default
   if (aNetwork->GetAddress().IsNull())
   {
      //! Specific address not provided. Generate one, but use current CIDR
      //! value in case the user specified one.
      while (true)
      {
         networkAddress.GenerateAddress(aNetwork->GetType(), networkAddress.GetSubnetMaskBitLength());
         if (!IsNetworkAddressManaged(networkAddress) && !mReservedAddresses.IsReserved(networkAddress))
         {
            aNetwork->SetAddress(networkAddress);
            break;
         }
      }
   }
   else if (IsNetworkAddressManaged(networkAddress) || mReservedAddresses.IsReserved(networkAddress))
   {
      if (!aSuppressReservedCheck)
      {
         return false;
      }
   }

   //! Add this network.
   auto result1 = mAddressToNetworkMap.insert(std::make_pair(networkAddress, networkName));
   if (!result1.second)
   {
      return false;
   }

   auto result2 = mNetworkMap.insert(std::make_pair(networkName, std::move(aNetwork)));
   if (!result2.second)
   {
      mAddressToNetworkMap.erase(networkAddress);
      return false;
   }

   auto result3 = mNetworkAddressSet.insert(networkAddress);
   if (!result3.second)
   {
      mAddressToNetworkMap.erase(networkAddress);
      mNetworkMap.erase(networkName);
      return false;
   }

   //! Initialize the network
   auto networkPtr = GetNetwork(networkName);
   networkPtr->Initialize(*GetSimulation());
   networkPtr->SetIsManaged(true);

   //! Notify the observer of the new network.
   mSimulationPtr->GetCommObserver().NetworkAdded(aSimTime, GetNetwork(networkName));

   //! Notify callback
   NetworkAdded(aSimTime, result2.first->second.get());

   return true;
}

// =================================================================================================
void NetworkManager::RemoveNetwork(double aSimTime, const std::string& aNetworkName)
{
   auto networkPtr = GetNetwork(aNetworkName);
   if (!networkPtr)
   {
      return;
   }

   //! Notify the observer of the removed network.
   mSimulationPtr->GetCommObserver().NetworkRemoved(aSimTime, networkPtr);

   //! Notify the callback.
   NetworkRemoved(aSimTime, networkPtr);

   auto netAddress  = networkPtr->GetAddress();
   auto addressList = GetAddressesInNetwork(aNetworkName);
   for (const auto& it : addressList)
   {
      RemoveComm(aSimTime, it);
   }

   networkPtr->SetIsManaged(false);
   mAddressToNetworkMap.erase(netAddress);
   mNetworkMap.erase(aNetworkName);
   mNetworkAddressSet.erase(netAddress);
}

// =================================================================================================
bool NetworkManager::EnableComm(double aSimTime, Comm* aCommPtr)
{
   auto addressPtr = GetAddress(aCommPtr);
   return EnableComm(aSimTime, *addressPtr);
}

// =================================================================================================
bool NetworkManager::EnableComm(double aSimTime, const Address& aAddress)
{
   auto nodePtr = mGraph.FindNode(aAddress);
   if (!nodePtr)
   {
      return false;
   }

   nodePtr->SetEnabled();

   //! Notify callback
   CommEnabled(aSimTime, GetComm(aAddress));

   return true;
}

// =================================================================================================
bool NetworkManager::DisableComm(double aSimTime, Comm* aCommPtr)
{
   auto addressPtr = GetAddress(aCommPtr);
   return DisableComm(aSimTime, *addressPtr);
}

// =================================================================================================
bool NetworkManager::DisableComm(double aSimTime, const Address& aAddress)
{
   auto nodePtr = mGraph.FindNode(aAddress);
   if (!nodePtr)
   {
      return false;
   }

   nodePtr->SetDisabled();

   //! Notify Callback
   CommDisabled(aSimTime, GetComm(aAddress));

   return true;
}

// =================================================================================================
bool NetworkManager::AddConnection(double aSimTime,
                                   Comm*  aSourceCommPtr,
                                   Comm*  aDestinationCommPtr,
                                   bool   aNotifyObserver) // =true
{
   auto srcAddress  = GetAddress(aSourceCommPtr);
   auto destAddress = GetAddress(aDestinationCommPtr);

   if (srcAddress && destAddress)
   {
      return AddConnection(aSimTime, *srcAddress, *destAddress, aNotifyObserver);
   }
   else
   {
      return false;
   }
}

// =================================================================================================
bool NetworkManager::AddConnection(double         aSimTime,
                                   const Address& aSourceAddress,
                                   const Address& aDestinationAddress,
                                   bool           aNotifyObserver) // =true
{
   auto existingEdge = mGraph.FindEdge(aSourceAddress, aDestinationAddress);
   if (existingEdge)
   {
      return true;
   }

   //! Links to self are not allowed.
   if (aSourceAddress == aDestinationAddress)
   {
      return true;
   }

   auto edgePtr = mGraph.InsertEdge(aSourceAddress, aDestinationAddress, true);
   if (edgePtr)
   {
      if (aNotifyObserver)
      {
         mSimulationPtr->GetCommObserver().LinkAddedToManager(aSimTime,
                                                              GetComm(aSourceAddress),
                                                              GetComm(aDestinationAddress));
      }

      //! Notify callback
      ConnectionAdded(aSimTime, aSourceAddress, aDestinationAddress);

      return true;
   }
   return false;
}

// =================================================================================================
bool NetworkManager::RemoveConnection(double aSimTime,
                                      Comm*  aSourceCommPtr,
                                      Comm*  aDestinationCommPtr,
                                      bool   aNotifyObserver) // =true
{
   auto srcAddress  = GetAddress(aSourceCommPtr);
   auto destAddress = GetAddress(aDestinationCommPtr);
   return RemoveConnection(aSimTime, *srcAddress, *destAddress, aNotifyObserver);
}

// =================================================================================================
bool NetworkManager::RemoveConnection(double         aSimTime,
                                      const Address& aSourceAddress,
                                      const Address& aDestinationAddress,
                                      bool           aNotifyObserver) // =true
{
   bool  result = true;
   auto* edge   = mGraph.FindEdge(aSourceAddress, aDestinationAddress);
   if (edge == nullptr)
   {
      return result;
   }

   // create local copies of src/dest addresses, incase the references
   // will be invalidated upon edge removal.
   const Address sourceAddress      = aSourceAddress;
   const Address destinationAddress = aDestinationAddress;

   do
   {
      result &= mGraph.EraseEdge(sourceAddress, destinationAddress);
      if (result)
      {
         if (aNotifyObserver)
         {
            mSimulationPtr->GetCommObserver().LinkRemovedFromManager(aSimTime,
                                                                     GetComm(sourceAddress),
                                                                     GetComm(destinationAddress));
         }

         // Notify callback
         ConnectionRemoved(aSimTime, sourceAddress, destinationAddress);
      }
   } while (mGraph.FindEdge(sourceAddress, destinationAddress));

   return result;
}

// =================================================================================================
bool NetworkManager::EnableConnection(double aSimTime,
                                      Comm*  aSourceCommPtr,
                                      Comm*  aDestinationCommPtr,
                                      bool   aNotifyObserver) // = true
{
   auto srcAddress  = GetAddress(aSourceCommPtr);
   auto destAddress = GetAddress(aDestinationCommPtr);
   return EnableConnection(aSimTime, *srcAddress, *destAddress, aNotifyObserver);
}

// =================================================================================================
bool NetworkManager::EnableConnection(double         aSimTime,
                                      const Address& aSourceAddress,
                                      const Address& aDestinationAddress,
                                      bool           aNotifyObserver) // = true
{
   auto edgePtr = mGraph.FindEdge(aSourceAddress, aDestinationAddress);
   if (edgePtr)
   {
      edgePtr->SetEnabled();

      if (aNotifyObserver)
      {
         mSimulationPtr->GetCommObserver().LinkEnabledOnManager(aSimTime,
                                                                GetComm(aSourceAddress),
                                                                GetComm(aDestinationAddress));
      }

      // Notify callback
      ConnectionEnabled(aSimTime, aSourceAddress, aDestinationAddress);

      return true;
   }
   return false;
}

// =================================================================================================
bool NetworkManager::DisableConnection(double aSimTime,
                                       Comm*  aSourceCommPtr,
                                       Comm*  aDestinationCommPtr,
                                       bool   aNotifyObserver) // = true
{
   auto srcAddress  = GetAddress(aSourceCommPtr);
   auto destAddress = GetAddress(aDestinationCommPtr);
   return DisableConnection(aSimTime, *srcAddress, *destAddress, aNotifyObserver);
}

// =================================================================================================
bool NetworkManager::DisableConnection(double         aSimTime,
                                       const Address& aSourceAddress,
                                       const Address& aDestinationAddress,
                                       bool           aNotifyObserver) // = true
{
   auto edgePtr = mGraph.FindEdge(aSourceAddress, aDestinationAddress);
   if (edgePtr)
   {
      edgePtr->SetDisabled();

      if (aNotifyObserver)
      {
         mSimulationPtr->GetCommObserver().LinkDisabledOnManager(aSimTime,
                                                                 GetComm(aSourceAddress),
                                                                 GetComm(aDestinationAddress));
      }

      //! Notify callback
      ConnectionDisabled(aSimTime, aSourceAddress, aDestinationAddress);

      return true;
   }
   return false;
}

// =================================================================================================
bool NetworkManager::RemoveNetworkConnections(double             aSimTime,
                                              const std::string& aNetworkName,
                                              bool               aNotifyObserver) // =true
{
   if (!IsNetworkManaged(aNetworkName))
   {
      return false;
   }

   bool ok         = true;
   auto addressSet = GetAddressesInNetwork(aNetworkName);
   for (const auto& addressIt : addressSet)
   {
      auto edgeList = mGraph.GetOutgoingNodeEdges(addressIt);
      for (auto& edge : edgeList)
      {
         ok &= mGraph.EraseEdge(addressIt, edge->GetDestinationAddress());
         if (ok && aNotifyObserver)
         {
            mSimulationPtr->GetCommObserver().LinkRemovedFromManager(aSimTime,
                                                                     GetComm(addressIt),
                                                                     GetComm(edge->GetDestinationAddress()));
         }
      }
   }

   return ok;
}

// =================================================================================================
void NetworkManager::PlatformDeleted(double aSimTime, WsfPlatform* aPlatform)
{
   //! Notify the observer BEFORE any action is taken.
   CommFrameworkPlatformDeleted(aSimTime, aPlatform);

   for (WsfComponentList::RoleIterator<Comm> iter{aPlatform->GetComponents()}; !iter.AtEnd(); ++iter)
   {
      if (!iter->GetAddress().IsNull())
      {
         auto nodePtr = mGraph.FindNode(iter->GetAddress());
         if (nodePtr)
         {
            WsfObserver::CommRemovedFromManager(mSimulationPtr)(aSimTime, *iter);

            //! Inform the router of the interface removal
            if (iter->GetRouter())
            {
               iter->GetRouter()->RemoveInterface(aSimTime, *iter);
            }

            //! Find the owning network, and remove the node, if possible.
            auto networkPtr = GetNetwork(iter->GetNetwork());
            if (!networkPtr)
            {
               mGraph.RemoveNodeEdges(nodePtr);
               mGraph.RemoveNode(iter->GetAddress());
            }
            else
            {
               networkPtr->RemoveMember(iter->GetAddress(), *mSimulationPtr);
            }
         }

         mAddressToCommMap.erase(iter->GetAddress());
         mCommToAddressMap.erase(*iter);
      }
   }
}

// =================================================================================================
void NetworkManager::PlatformInitialized(double aSimTime, WsfPlatform* aPlatform)
{
   for (WsfComponentList::RoleIterator<Comm> iter{aPlatform->GetComponents()}; !iter.AtEnd(); ++iter)
   {
      auto commPtr = *iter;
      ManageComm(aSimTime, commPtr, commPtr->GetInputNetwork(), commPtr->GetInputAddress(), commPtr->GetInputNetworkAddress());

      // Set the router for this interface
      auto routerPtr = aPlatform->GetComponent<router::Router>(iter->GetRouterName());
      if (!routerPtr)
      {
         auto out = ut::log::error() << "Missing router on platform.";
         out.AddNote() << "Platform: " << aPlatform->GetName();
         out.AddNote() << "Router: " << iter->GetRouterName();
         throw UtException("Invalid router specification in NetworkManager::PlatformInitialized()");
      }
      routerPtr->AddInterface(aSimTime, *iter);

      InitializeUserLinks(aSimTime, commPtr);
   }

   for (WsfComponentList::RoleIterator<router::Router> iter{aPlatform->GetComponents()}; !iter.AtEnd(); ++iter)
   {
      iter->InitializeCallbacks();
   }

   //! Notify callbacks
   CommFrameworkPlatformInitialized(aSimTime, aPlatform);
}

// =================================================================================================
bool NetworkManager::HasMulticastMember(const Address& aMulticastAddress, const std::string& aNetworkName) const
{
   bool hasMember = false;

   auto it = mMulticastMap.find(aMulticastAddress);
   if (it != std::end(mMulticastMap))
   {
      auto addressSet = GetAddressesInNetwork(aNetworkName);
      if (!addressSet.empty())
      {
         for (auto& memberAddress : it->second)
         {
            auto foundIt = addressSet.find(memberAddress);
            if (foundIt != std::end(addressSet))
            {
               return true;
            }
         }
      }
   }

   return hasMember;
}

// =================================================================================================
bool NetworkManager::AddMulticastMember(double aSimTime, const Address& aMulticastAddress, const Address& aJoiningAddress)
{
   bool added = false;

   if (aMulticastAddress.IsNull() || aJoiningAddress.IsNull())
   {
      return added;
   }

   auto it = mMulticastMap.find(aMulticastAddress);
   if (it == std::end(mMulticastMap))
   {
      AddressVector members;
      members.push_back(aJoiningAddress);
      auto result = mMulticastMap.insert(std::make_pair(aMulticastAddress, members));
      added       = result.second;
   }
   else
   {
      auto found = std::find(std::begin(it->second), std::end(it->second), aJoiningAddress);
      if (found == std::end(it->second))
      {
         it->second.push_back(aJoiningAddress);
         added = true;
      }
   }

   return added;
}

// =================================================================================================
bool NetworkManager::RemoveMulticastMember(double aSimTime, const Address& aMulticastAddress, const Address& aLeavingAddress)
{
   bool removed = false;

   if (aMulticastAddress.IsNull() || aLeavingAddress.IsNull())
   {
      return removed;
   }

   auto it = mMulticastMap.find(aMulticastAddress);
   if (it != std::end(mMulticastMap))
   {
      auto found = std::find(std::begin(it->second), std::end(it->second), aLeavingAddress);
      if (found != std::end(it->second))
      {
         it->second.erase(found);
         removed = true;
      }
   }

   return removed;
}

// =================================================================================================
NetworkManager::AddressSet NetworkManager::GetMulticastMembers(const Address&     aMulticastAddress,
                                                               const std::string& aNetworkName) const
{
   AddressSet members;

   auto it = mMulticastMap.find(aMulticastAddress);
   if (it != std::end(mMulticastMap))
   {
      auto addressSet = GetAddressesInNetwork(aNetworkName);
      if (!addressSet.empty())
      {
         for (auto& memberAddress : it->second)
         {
            auto foundIt = addressSet.find(memberAddress);
            if (foundIt != std::end(addressSet))
            {
               members.insert(memberAddress);
            }
         }
      }
   }

   return members;
}

// =================================================================================================
NetworkManager::AddressSet NetworkManager::GetMulticastMembers(const Address& aMulticastAddress) const
{
   AddressSet members;

   auto it = mMulticastMap.find(aMulticastAddress);
   if (it != std::end(mMulticastMap))
   {
      for (auto& memberAddress : it->second)
      {
         members.insert(memberAddress);
      }
   }

   return members;
}

// =================================================================================================
bool NetworkManager::IsMulticastManaged(const Address& aMulticastAddress) const
{
   auto it = mMulticastMap.find(aMulticastAddress);
   return (it != std::end(mMulticastMap));
}

// =================================================================================================
NetworkManager::AddressSet NetworkManager::GetComms() const
{
   AddressSet returnSet;
   for (auto commPair : mAddressToCommMap)
   {
      returnSet.insert(commPair.first);
   }

   return returnSet;
}

// =================================================================================================
bool NetworkManager::PathExists(const Address& aSourceAddress, const Address& aDestinationAddress) const
{
   double cost = 0.0;
   return mGraph.FindAnyPath(aSourceAddress, aDestinationAddress, nullptr, cost);
}

} // namespace comm
} // namespace wsf
