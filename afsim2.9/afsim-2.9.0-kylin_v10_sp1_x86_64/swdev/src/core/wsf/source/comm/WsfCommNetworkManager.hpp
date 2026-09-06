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

#ifndef WSFCOMMNETWORKMANAGER_HPP
#define WSFCOMMNETWORKMANAGER_HPP

#include "wsf_export.h"

#include <map>
#include <memory>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "UtCallback.hpp"
#include "UtCallbackHolder.hpp"
#include "UtMemory.hpp"
#include "WsfComm.hpp"
#include "WsfCommAddress.hpp"
#include "WsfCommGraph.hpp"
#include "WsfCommNetwork.hpp"
#include "WsfCommReservedAddressing.hpp"
class WsfPlatform;
class WsfSimulation;
#include "WsfScenarioExtension.hpp"
#include "WsfSimulation.hpp"
#include "WsfSimulationExtension.hpp"

namespace wsf
{
namespace comm
{

class WSF_EXPORT NetworkManager : public WsfSimulationExtension
{
public:
   enum class NetworkTopology
   {
      cPOINT_TO_POINT,
      cMESH,
      cSTAR,
      cRING,
      cDIRECTED_RING
   };

   struct CommHash
   {
      size_t operator()(Comm* aCommPtr) const noexcept { return std::hash<unsigned int>()(aCommPtr->GetUniqueId()); }
   };

   //! Definitions of commonly used container types
   using CommMap           = std::unordered_map<Address, Comm*>;           // Address -> Comm
   using AddressMap        = std::unordered_map<Comm*, Address, CommHash>; // Comm.GetUniqueId -> Address
   using AddressSet        = std::unordered_set<Address>;
   using NetworkSet        = std::set<Address>;
   using AddressNetworkMap = std::unordered_map<Address, std::string>;                  // Address -> Network Name
   using NetworkMap        = std::unordered_map<std::string, std::unique_ptr<Network>>; // Network name -> Network
   using AddressVector     = std::vector<Address>;                                      // A vector of Addresses
   using MultiAddressMap = std::unordered_map<Address, AddressVector>; // A mapping of an address to multiple addresses
   using CommVector      = std::vector<Comm*>;
   using RouterVector    = std::vector<router::Router*>;

   static NetworkManager* Find(const WsfSimulation& aSimulation);

   //! Constructors and Operator Overloads
   //@{
   NetworkManager(WsfSimulation* aSimPtr);
   ~NetworkManager() override                 = default;
   NetworkManager(const NetworkManager& aSrc) = delete;
   NetworkManager& operator=(const NetworkManager& aSrc) = delete;
   NetworkManager(NetworkManager&& aSrc)                 = delete;
   NetworkManager& operator=(NetworkManager&& aSrc) = delete;
   //@}

   //! @name Notifications from the simulation.
   //! These are called in the order they are listed.
   //@{
   bool Initialize() override;
   bool PlatformsInitialized() override;
   void PendingStart() override;
   //@}

   //! Accessors
   //@{

   //! Provides Address for a Comm object
   const Address* GetAddress(Comm* aCommPtr) const;

   //! Provides Comm object for an Address
   Comm* GetComm(const Address& aAddress) const;

   //! Provides Network object for a network name
   Network* GetNetwork(const std::string& aNetworkName) const;

   //! True if Comm object present
   bool IsCommManaged(Comm* aCommPtr) const;

   //! True if Address object present
   bool IsAddressManaged(const Address& aAddress) const;

   //! True if network with name provided present
   bool IsNetworkManaged(const std::string& aNetworkName) const;

   //! Returns the names of all managed networks.
   std::vector<std::string> GetManagedNetworks() const;

   //! Returns the address of the network that the provided address belongs to
   const Address* GetManagingNetworkAddress(const Address& aAddress) const;

   //! True if network address provided overlaps with a currently assigned network address
   bool IsNetworkAddressManaged(const Address& aAddress) const;

   //! Get name of network associated with an Address
   std::string GetNetworkNameFromAddress(const Address& aAddress) const;

   //! Get a list of all addresses associated with a network
   AddressSet GetAddressesInNetwork(const std::string& aNetworkName) const;

   //! Returns all registered comms in the simulation
   AddressSet GetComms() const;

   //! Get the graph representation of all comm networks in AFSIM (perfect knowledge)
   graph::Graph& GetGraph();

   //! Simple check of pathing from source to destination
   bool PathExists(const Address& aSourceAddress, const Address& aDestinationAddress) const;

   //! Returns true if the network has a comm subscribed to the provided multicast address
   bool HasMulticastMember(const Address& aMulticastAddress, const std::string& aNetworkName) const;

   //! Returns the members of a network subscribed to a multicast address
   AddressSet GetMulticastMembers(const Address& aMulticastAddress, const std::string& aNetworkName) const;

   //! Returns ALL members of a multicast address.
   AddressSet GetMulticastMembers(const Address& aMulticastAddress) const;

   //! True if a multicast address being utilized/managed.
   bool IsMulticastManaged(const Address& aMulticastAddress) const;
   //@}

   //! @name AddManagedComm
   //! Adds a comm (during its initialization routine) to a container of interfaces
   //! that are processed during the PlatformsInitialized() call. Required for ALL comm
   //! implementations being managed by the network manager via scenario input.
   void AddManagedComm(Comm* aCommPtr) { mComms.push_back(aCommPtr); }

   //! @name AddManagedRouter
   //! Adds a router (during its initialization routine) to a container
   //! that is processed during the PlatformsInitialized() call. Required for ALL
   //! router implementations being managed by the network manager.
   void AddManagedRouter(router::Router* aRouterPtr) { mRouters.push_back(aRouterPtr); }

   //! @name AddComm
   //! Adds a comm device to the network manager.
   //! The first version of this method will look for an existing network, if
   //! the value was provided, and add the comm device to that network with an
   //! appropriate address, if possible. If no network was specified, a new
   //! address will be assigned, with a new network created containing only
   //! this comm device. The assigned address of the comm device is returned,
   //! with an address of 0.0.0.0 provided upon failure to add the device.
   //! The second version checks the user provided address to see if it belongs
   //! to an existing network. If so, the device is added to that network with
   //! the user specified address. If the address does not belong to a current
   //! network assignment, a new network is created to which this device is
   //! assigned. The second version returns the name of the network that the
   //! comm was assigned to, or a empty string ("") on failure.
   //@{
   Address AddComm(double aSimTime, Comm* aCommPtr, std::string& aNetworkName, bool aNotifyObserver = true);

   std::string AddComm(double aSimTime, Comm* aCommPtr, const Address& aAddress, bool aNotifyObserver = true);
   //@}

   //! @name RemoveComm
   //! Removes the comm device and all applicable mappings from the network manager.
   //! Can be performed by providing either a pointer to the comm device, or an address.
   //@{
   void RemoveComm(double aSimTIme, Comm* aCommPtr, bool aNotifyObserver = true);
   void RemoveComm(double aSimTime, const Address& aAddress, bool aNotifyObserver = true);
   //@}

   //! @name AddNetwork
   //! Adds a network to the network manager.
   //! The passed boolean value indicates whether reserved addressing is
   //! enforced, as this needs to be disabled during initial setup.
   //@{
   bool AddNetwork(double aSimTime, std::unique_ptr<Network> aNetwork, bool aSuppressReservedCheck = false);
   //@}

   //! @name RemoveNetwork
   //! Removes a network from the network manager.
   //! @note Removal of the network also removes the management of any associated
   //! comms and their addresses from network management.
   //! @{
   void RemoveNetwork(double aSimTime, const std::string& aNetworkName);

   //! @name EnableComm
   //! Given a comm device or an address, enables the object (node) within the graph.
   //@{
   bool EnableComm(double aSimTime, Comm* aCommPtr);
   bool EnableComm(double aSimTime, const Address& aAddress);
   //@}

   //! @name DisableComm
   //! Given a comm device or an address, disables the object (node) within the graph.
   //@{
   bool DisableComm(double aSimTime, Comm* aCommPtr);
   bool DisableComm(double aSimTime, const Address& aAddress);
   //@}

   //! @name AddConnection
   //! Given two comm devices, create a connection (edge) on the graph.
   //@{
   bool AddConnection(double aSimTime, Comm* aSourceCommPtr, Comm* aDestinationCommPtr, bool aNotifyObserver = true);
   bool AddConnection(double         aSimTime,
                      const Address& aSourceAddress,
                      const Address& aDestinationAddress,
                      bool           aNotifyObserver = true);
   //@}

   //! @name RemoveConnection
   //! Given two comm devices, remove a connection (edge) on the graph.
   //@{
   bool RemoveConnection(double aSimTime, Comm* aSourceCommPtr, Comm* aDestinationCommPtr, bool aNotifyObserver = true);
   bool RemoveConnection(double         aSimTime,
                         const Address& aSourceAddress,
                         const Address& aDestinationAddress,
                         bool           aNotifyObserver = true);
   //@}

   //! @name EnableConnection
   //! Given two comm devices, enable a connection (edge) on the graph.
   //@{
   bool EnableConnection(double aSimTime, Comm* aSourceCommPtr, Comm* aDestinationCommPtr, bool aNotifyObserver = true);
   bool EnableConnection(double         aSimTime,
                         const Address& aSourceAddress,
                         const Address& aDestinationAddress,
                         bool           aNotifyObserver = true);
   //@}

   //! @name DisableConnection
   //! Given two comm devices, disable a connection (edge) on the graph.
   //@{
   bool DisableConnection(double aSimTime, Comm* aSourceCommPtr, Comm* aDestinaationCommPtr, bool aNotifyObserver = true);
   bool DisableConnection(double         aSimTime,
                          const Address& aSourceAddress,
                          const Address& aDestinationAddress,
                          bool           aNotifyObserver = true);

   bool AddMulticastMember(double aSimTime, const Address& aMulticastAddress, const Address& aJoiningMember);

   bool RemoveMulticastMember(double aSimTime, const Address& aMulticastAddress, const Address& aLeavingMember);
   //@}

   //! @name RemoveNetworkConnections
   //! Given a network name, remove all connections between members of the network.
   bool RemoveNetworkConnections(double aSimTime, const std::string& aNetworkName, bool aNotifyObserver = true);

   //! @name ManageComm
   //! Based on input to the comm interface (or lack thereof), this method determines
   //! the appropriate call to AddComm to correctly assigned addressing to the interface.
   //! and then assigned the Comm to its network and assign linkage (via AddComm)
   void ManageComm(double             aSimTime,
                   Comm*              aCommPtr,
                   const std::string& aNetworkName,
                   const Address&     aAddress,
                   const Address&     aNetworkAddress);

   //! @name InitializeUserLinks
   //! For any passed comm object, creates linkage based on the user input for that
   //! comm. Cannot be called until all comms in the simulation have been assigned
   //! addresses and assigned to networks due to reliance on some linkage being
   //! address based.
   void InitializeUserLinks(double aSimTime, Comm* aCommPtr);

   //! @name Clear
   //! Removes all mappings and empties the graph object in the network manager.
   void Clear();

   //! @name GetSimulation()
   //! Provides an alternate point of entry for the simulation object for objects
   //! that may initially only have access to the Network Manager
   WsfSimulation* GetSimulation() const { return mSimulationPtr; }

   //! @name Network Manager Callbacks
   //@{
   void PlatformDeleted(double aSimTime, WsfPlatform* aPlatform);
   void PlatformInitialized(double aSimTime, WsfPlatform* aPlatform);
   //@}

   //! Accessor for reserved addressing object
   const ReservedAddressing& GetReservedAddresses() const { return mReservedAddresses; }

   //! @name Callback lists.
   //! Other objects may register callbacks that are to be invoked during network manager
   //! network graph state changes.
   //! NOTE: Be aware that these objects are called as processed by the network manager.
   //! It should not be assumed that they are called in a logical order (such as adding
   //! a comm before enabling a link to it), so any use of these must be robust enough
   //! to accommodate potential information that has not yet propagated.
   //@{
   UtCallbackListN<void(double, Comm*)>                          CommAdded{};
   UtCallbackListN<void(double, Comm*)>                          CommRemoved{};
   UtCallbackListN<void(double, Network*)>                       NetworkAdded{};
   UtCallbackListN<void(double, Network*)>                       NetworkRemoved{};
   UtCallbackListN<void(double, Comm*)>                          CommEnabled{};
   UtCallbackListN<void(double, Comm*)>                          CommDisabled{};
   UtCallbackListN<void(double, const Address&, const Address&)> ConnectionAdded{};
   UtCallbackListN<void(double, const Address&, const Address&)> ConnectionRemoved{};
   UtCallbackListN<void(double, const Address&, const Address&)> ConnectionEnabled{};
   UtCallbackListN<void(double, const Address&, const Address&)> ConnectionDisabled{};

   //! Allows indication that the comm framework is fully set up.
   UtCallbackListN<void()> CommFrameworkPendingStart{};

   //! Filtered PlatformInitialized event, called after the comm framework has processed this platform.
   UtCallbackListN<void(double, WsfPlatform*)> CommFrameworkPlatformInitialized{};

   //! Filtered PlatformDeleted event, called prior to the comm framework removing this platform.
   UtCallbackListN<void(double, WsfPlatform*)> CommFrameworkPlatformDeleted{};
   //@}

private:
   //! Pointer to the simulation object that owns the network manager.
   WsfSimulation* mSimulationPtr;

   //! Maps (in both directions) for 1 to 1 assignment of a comm object to an address.
   CommMap    mAddressToCommMap;
   AddressMap mCommToAddressMap;

   //! Provides a map of managed addresses to their owning network name.
   AddressNetworkMap mAddressToNetworkMap;

   //! A map of networks being maintained by the network manger to their names.
   NetworkMap mNetworkMap;

   //! A list of addresses corresponding to networks. Only the routing prefix is relevant.
   NetworkSet mNetworkAddressSet;

   //! Graph representation of all comm devices and their connections in AFSIM.
   //! This is a directed graph, which is weighted based on algorithm selection
   //! at the time of evaluation. This object belongs solely to the network manager
   //! class, although individual comm devices may maintain references to it when
   //! perfect knowledge of the network environment is desired. Otherwise, those
   //! individual comm objects may maintain their own graphs.
   graph::Graph mGraph;

   //! Callback list. The network manager represents truth, so when a platform and
   //! any of its associated comm devices are added or removed from the simulation,
   //! the network manager needs to be informed.
   UtCallbackHolder mCallbacks;

   //! The reserved addressing object. Maintains the reserved addressing objects that
   //! together define which addresses are not available for assignment to comm objects,
   //! and may have special use cases via hardware, protocols, etc.
   ReservedAddressing mReservedAddresses;

   //! The multicast group subscription object. Tracks which comm objects are
   //! "subscribed" to a particular address. Allows for tracking truth of such
   //! assignments, and avoids collisions of address assignment.
   MultiAddressMap mMulticastMap;

   //! A list of comms, supplied via simulation initialization, that is processed by the
   //! network manager during the PendingStart() call.
   CommVector mComms;

   //! A list of routers for proper notifications/processing
   RouterVector mRouters;
};

class NetworkManagerExtension : public WsfScenarioExtension
{
public:
   void SimulationCreated(WsfSimulation& aSimulation) override
   {
      aSimulation.RegisterExtension(GetExtensionName(), ut::make_unique<NetworkManager>(&aSimulation));
   }
};

} // namespace comm
} // namespace wsf

#endif
