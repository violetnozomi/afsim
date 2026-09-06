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

#ifndef WSFCOMMPROTOCOLOSPF_HPP
#define WSFCOMMPROTOCOLOSPF_HPP

#include "wsf_export.h"

#include <memory>

#include "UtCallbackHolder.hpp"
#include "WsfCommRouterProtocolInterface.hpp"
#include "WsfEvent.hpp"
#include "WsfRandomVariable.hpp"

namespace wsf
{
namespace comm
{
namespace router
{

//! Forward declare for ProtocolOSPF usage.
class OSPF_Area;

//! This is a generic implementation of some of
//! the features commonly associated with the OSPF routing
//! protocol. Usage assumes several operating conditions
//! that may be otherwise configurable in a full implementation
//! to avoid issues with complexity and performance in a mission
//! level simulation. Note that this protocol requires multicasting
//! capabilities, currently provided by the basic IGMP multicast
//! protocol, pre-defined and included in AFSIM, in order to
//! function correctly. Removing the IGMP protocol when using
//! this protocol will result in a lack of state sharing between
//! routers with OSPF.
//!
//! In general, this protocol provides some general
//! capabilities commonly associated with OSPF in a generalized
//! and highly abstract nature. These include:
//! 1. Using "routing areas" to define AS (autonomous systems).
//! 2. Limited sharing and updating states between routers to avoid
//!    excessive communications. (Still is performance heavy)
//! 3. Immediate distribution of network updates on update.
//! 4. Polling intervals for nodes (heartbeat for down nodes).
//! 5. Discovery and inclusion of new nodes (comms), assuming
//!    they are correctly configured.
//! 6. Re-election of BDR in the case of DR/BDR loss (we always
//!    promote the BDR to DR upon DR loss)
//!
//! What this implementation does NOT do:
//! 1. Attempt a full implementation of OSPF.
//! 2. Implement full OSPF data containers, link state messages, etc.
//! 3. Actually distribute routing data to every router. This
//!    implementation uses a generalization of routing data
//!    that is only contained on DR/BDRs, and other routers use
//!    this data for routing (instead of being shared via type 2 LSAs)
//! 4. Allow non DR/BDR networking types, such as point to point or
//!    most non-broadcasting network types.
//! 5. Neighbor relationships are not fully modeled. General assumptions
//!    are used to simplify the process based on connected interfaces.
//! 6. This model does not enforce timers to be the same between members.
//!    However, be aware that an excessive hello timer time beyond any DR/BDRs
//!    hold interval will cause that router to be dropped from OSPF.
class WSF_EXPORT ProtocolOSPF : public ProtocolInterface
{
public:
   //! ProtocolVector provides a vector of OSPF protocols
   using ProtocolVector = std::vector<ProtocolOSPF*>;

   //! A list of areas associated with this router/protocol. These are
   //! shared pointers, as the OSPF_Area lifetime is shared between any routers
   //! sharing the OSPF area, and the area objects are common across routers
   //! who are members.
   using AreaVector = std::vector<std::shared_ptr<OSPF_Area>>;

   //! A mapping of an area with the corresponding graph/routing table.
   using AreaGraphMap = std::map<Address, std::unique_ptr<graph::Graph>>;

   //! @name Common OSPF message identifiers.
   //@{
   static constexpr const char* cOSPF_ALL_HELLO   = "OSPF_ALL_HELLO";
   static constexpr const char* cOSPF_ALL_DROP    = "OSPF_ALL_DROP";
   static constexpr const char* cOSPF_ALL_DR_DROP = "OSPF_ALL_DR_DROP";
   static constexpr const char* cOSPF_ALL_DR_ADD  = "OSPF_ALL_DR_ADD";
   //@}

   enum class RouterAreaType : size_t
   {
      cNONE,   //! Router non-functional - host specification
      cNORMAL, //! No special area type - internal area router
      cABR,    //! Autonomous Border Router - connected to 2 or more OSPF areas
      cASBR    //! Autonomous System Boundary Router - connected to 2 or more areas,
               //! of which one or more do not use OSPF
   };

   static ProtocolOSPF* Find(const Router& aParent);
   static ProtocolOSPF* FindOrCreate(Router& aParent);

   ProtocolOSPF()           = default;
   ~ProtocolOSPF() override = default;
   ProtocolOSPF(const ProtocolOSPF& aSrc);

   //! @name Required interface from WsfComponent.
   //@{
   WsfComponent* CloneComponent() const override;
   const int*    GetComponentRoles() const override;
   void*         QueryInterface(int aRole) override;
   int           GetPriority() const override;
   ProtocolOSPF* Clone() const override;
   const char*   GetScriptClassName() const override { return "WsfCommProtocolOSPF"; }
   //@}

   //! @name Optional interface usage
   //@{
   void Setup() override;
   bool ProcessInput(UtInput& aInput) override;
   bool Initialize(double aSimTime) override;
   //@}

   //! @name ProtocolProcess methods
   //! This method implements the OSPF logic for reception of
   //! OSPF messages in the network layer. It "grabs" the message
   //! and checks if it is an OSPF type message (via Aux data)
   //! and acts accordingly to that data.
   //@{
   bool Send(double aSimTime, Router::SendData& aData) override;

   bool Receive(double aSimTime, const Address& aReceivingInterface, Message& aMessage, bool& aOverrideForward) override;

   std::vector<Address> Routing(double         aSimTime,
                                const Address& aSendingInterface,
                                size_t&        aHopNumber,
                                double&        aCost,
                                const Message& aMessage) override;
   //@}

   //! @name Graph/Table Methods
   //@{
   //! This graph is truth via the network manager. It shouldn't be used
   //! internally. It is provided for multicast functionality.
   graph::Graph* GetGraph() const override;
   graph::Graph* GetGraph(const Address& aArea);
   bool          ShouldPropagateTruth() const override { return false; }
   void          SetGraph(const Address& aArea, std::unique_ptr<graph::Graph> aGraph);
   //@}

   AreaVector&                 GetAreas() { return mAreas; }
   OSPF_Area*                  GetArea(const Address& aAreaAddress);
   const std::vector<Address>& GetInputAreas() const { return mInputAreas; }
   const Address&              GetInputBackbone() const { return mInputBackbone; }
   bool                        IsSetup() const { return mSetup; }
   void                        SetSetup(bool aSetup) { mSetup = aSetup; }
   RouterAreaType              GetAreaType() const { return mAreaType; }
   void                        SetAreaType(RouterAreaType aType) { mAreaType = aType; }
   size_t                      GetPriorityDR() const { return mPriorityDR; }

   //! Given a network name, returns the interface available to this router/protocol
   //! with the highest priority (the highest binary value address) belonging to that network.
   //! Returns a nullptr if such an interface could not be found.
   Comm* GetPriorityInterface(const std::string aNetwork) const;

   //! Callback methods to handle addition of routers during runtime.
   void PlatformInitialized(double aSimTime, WsfPlatform* aPlatformPtr);

   //! A simple struct used to combine data used by OSPF on all routers.
   //! Every router periodically attempts to communicate with other routers
   //! within its area. Routers respond to this, validating two-way communications.
   //! Since areas can encompass multiple networks, this allows an area to know
   //! which networks (and their interfaces) that can be sent to directly within
   //! the same area, as well as track when such data should be removed when
   //! communications fail (after a timeout).
   struct LinkStateData
   {
      Address     mLocalInterface{};
      Address     mRemoteInterface{};
      std::string mLocalNetwork{};
      std::string mRemoteNetwork{};
      double      mLastUpdateTime{};
   };

   //! A LinkStateMap provides a mapping between an area and the link state data in that area.
   //! Only ABRs and ASBRs will have more than one map entry (one for each area).
   using LinkStateMap = std::map<Address, std::vector<LinkStateData>>;
   using LinkStateIt  = LinkStateMap::iterator;

protected:
   //! Convenience method for querying the Network Manager and getting
   //! all of the OSPF enabled comm objects at the time of the call.
   ProtocolVector GetAllOSPF();

   //! Convenience method for runtime acquisition of all existing areas.
   std::set<std::shared_ptr<OSPF_Area>> GetAllAreas();

   //! A method that determines and sets the OSPF router type.
   void SetRouterRoleType(ProtocolOSPF& aProtocol);

   //! A method that provides verification that each area is directly
   //! connected to the backbone, and throw an exception if this condition
   //! does not exist due to improper user configuration.
   void VerifyBackboneConnections(const AreaVector& aAreas);

   //! A method for initializing link state data for OSPF routers.
   //! Used only for sim initialization for network convergence.
   //! The data is maintained organically during runtime, or with
   //! the entry of new routers/interfaces. Uses the provided
   //! graph to initialize the data.
   void InitializeLinkState(const AreaVector& aAreas, graph::Graph& aGraph);

   //! Instead of using summary tables, we cheat by peeking at other area
   //! link state data to find the membership of a router with an interface
   //! in our connected OSPF areas. Returns the
   //! address of the area, which is null if not found.
   Address InterfaceInContiguousOSPF(const Address&     aDestinationInterface,
                                     ProtocolOSPF&      aProtocol,
                                     std::set<Address>& aCheckedAreas);

   //! Provides all of the ASBRs in contiguous OSPF areas.
   std::set<ProtocolOSPF*> GetAllASBR(ProtocolOSPF& aProtocol, std::set<Address>& aCheckedAreas);

   //! @name Routing Methods
   //! Routing in OSPF is based on the location of the sender in respect
   //! to the backbone, and whether the destination is in the same area,
   //! a different area, or even external to a connected OSPF area.
   //! These methods standardize the routing procedures based on
   //! discovery of these conditions during normal routing operation.
   //@{

   //! Finds a route to the backbone. Assumes knowledge that the destination
   //! is in a connected OSPF area, and the current handling router
   //! is not in the backbone.
   std::vector<Address> RouteToBackbone(ProtocolOSPF& aProtocol, const Address& aSendingAddress, double& aCost, size_t& aHops);

   //! Find a route to the correct area from the backbone. Assumes
   //! knowledge that the destination is in a connected OSPF area
   //! that isn't the backbone, and the current handling router is
   //! in the backbone.
   std::vector<Address> RouteFromBackbone(ProtocolOSPF&  aProtocol,
                                          const Address& aSendingAddress,
                                          const Address& aDestinationAddress,
                                          double&        aCost,
                                          size_t&        aHops);

   //! Using all of the connected ASBRs, find the optimal external route
   //! to the destination using a non-OSPF routing protocol. This is not
   //! guaranteed to succeed. Removes the need for ASBR's to send summary
   //! data across OSPF by directly polling. Does not provide a path
   //! from the current sender to the ASBR itself, just the best external
   //! path available and identification of the router that provides it.
   std::pair<std::vector<Address>, Comm*> RouteExternallyASBR(const Message& aMessage);
   //@}

   //! Determines if a message is for us. OSPF multicast messages
   //! provide the area, so we ensure this matches the area we
   //! belong to. Returns the matching addresses of the areas shared.
   std::vector<Address> MyMessage(Message& aMessage) const;


   //! Schedules an event. Doesn't fire if the scheduler
   //! is removed from the sim.
   void ScheduleEvent(std::unique_ptr<WsfEvent> aEvent) const;

   //! OSPF messages
   //! These messages are generic link state messages to
   //! implement general OSPF behavior.
   //@{
   void SendHello(double aSimTime);
   void ReceiveHello(double                      aSimTime,
                     const Address&              aReceiverAddress,
                     const std::vector<Address>& aValidAreas,
                     Message&                    aMessage);

   void SendDR_GraphAdd(double aSimTime, const Address& aReceivingInterface, const std::vector<Address>& aTraceRoute);
   void ReceiveDR_GraphAdd(double                      aSimTime,
                           const Address&              aReceivingInterface,
                           const std::vector<Address>& aValidAreas,
                           Message&                    aMessage);
   void SendDR_GraphDrop(double             aSimTime,
                         const Address&     aReceivingInterface,
                         const Address&     aDroppedInterface,
                         const std::string& aDroppedInterfaceNetwork);
   void ReceiveDR_GraphDrop(double                      aSimTime,
                            const Address&              aReceivingInterface,
                            const std::vector<Address>& aValidAreas,
                            Message&                    aMessage);
   //@}


   //! Updates the local graph to reflect the data contained
   //! in a received OSPF HELLO message, Only valid for
   //! routers acting as the DR/BDR.
   void DR_GraphAdd(graph::Graph& aGraph, const Address& aReceivingInterface, const std::vector<Address>& aTraceRoute);

   //! Updates the local graph to reflect a dropped interface
   //! due to timeout of non-reception of OSPF HELLO. Only valid for
   //! routers acting as the DR/BDR.
   void DR_GraphDrop(graph::Graph&      aGraph,
                     const Address&     aArea,
                     const Address&     aDroppedInterface,
                     const std::string& aDroppedNetwork);


   //! Handles the update of the link state list for checks on dropped interfaces via timeout.
   void TimeoutUpdate(double aSimTime, bool aCalledFromEvent);


   void           AddLinkState(const Address& aAreaAddress, const LinkStateData& aData);
   LinkStateData* FindLinkState(const Address& aAreaAddress, const Address& aLocalAddress, const Address& aRemoteAddress);

   //! Finds a link state regardless of local interface. Returns all link states
   //! that match the destination in the provided area.
   std::vector<LinkStateData> FindLinkState(const Address& aAreaAddress, const Address& aRemoteAddress);
   bool RemoveLinkState(const Address& aAreaAddress, const Address& aLocalAddress, const Address& aRemoteAddress);

   const LinkStateMap& GetLinkStates() const { return mLinkStates; }
   LinkStateMap&       GetLinkStates() { return mLinkStates; }


private:
   Address              mInputBackbone{};
   std::vector<Address> mInputAreas{};
   AreaVector           mAreas{};
   size_t               mPriorityDR{std::numeric_limits<size_t>::max()};
   RouterAreaType       mAreaType{RouterAreaType::cNORMAL};

   //! 10.0 s default hello interval
   WsfRandomVariable mHelloInterval{10.0, WsfRandomVariable::cNON_NEGATIVE};

   //! 40.0 s default hold timer
   WsfRandomVariable mHoldTimer{40.0, WsfRandomVariable::cNON_NEGATIVE};

   //! A random draw between 0 and 0.1 to add some slop to the above timers.
   //! This avoids having them all fire at exactly the same time in the simulation.
   //! This value is only drawn once, so that once established, the delay is always the
   //! same, and the ordering of messaging related to the hello and hold intervals are
   //! consistent.
   WsfRandomVariable mRandomInterval{0.0, 0.1, WsfRandomVariable::cNON_NEGATIVE};

   //! Flag for indicating whether the OSPF initialization routine for all OSPF members
   //! has fired, and ensures it only occurs once. Avoids usage of static.
   bool mSetup{false};

   //! The graph used by this protocol/router for OSPF. Unless this router is a DR
   //! or BDR, this graph will be empty. Since a router/protocol can be the DR/BDR
   //! for multiple areas when an ABR/ASBR, multiple graphs are stored corresponding
   //! to each area membership.
   AreaGraphMap mGraph{};

   //! Link state data maintained by all OSPF routers. Actual routing data is only held
   //! by the DRs/BDRs.
   LinkStateMap mLinkStates{};

   //! For wsf::comm::GenericEvent
   std::shared_ptr<int> mContext{nullptr};

   //! Callback for router instantiation at runtime.
   UtCallbackHolder mCallbacks{};
};

//! The OSPF protocol defines an area as a logical grouping of routers and their interfaces.
//! What makes OSPF useful in large networks is the ability to divide these networks into
//! multiple areas that typically only retain information and conduct protocol communication within
//! that area to avoid flooding the network and streamline communications using assumptions held
//! constant across all OSPF enabled areas.
//!
//! This object is a representation of an OSPF area. Each area has a unique ID set by user input
//! (we simply use the existing address schema to provide this). Every network within the area
//! has a single router that collects the network state knowledge for the area, the designated router (DR).
//! It may have a secondary router with a copy of this data (to account for loss of routers) known as a
//! backup designated router (BDR). Since an area may represent a single network or many networks,
//! each area should have at least one DR, but may have many DR's, and potentially as many BDRs given
//! at least two routers for each network in an area.
//!
//! All areas must connect to a specially designated area, referred to as the backbone area.
//! Loss of connectivity with the backbone typically limits the range of communications to within the
//! the same area.
//!
//! Note that routers may exist in multiple areas. Such routers are referred to as an area border
//! router (ABR), and such routers will be members of the areas in which they border. Thus, developers
//! should be aware that a router may be repeated in multiple areas, and may have the same or different
//! roles in each of these areas (e.g. router 'foo' may be a DR in two areas, a BDR in another area,
//! and have no specific roles in other areas - each area manages itself).
class WSF_EXPORT OSPF_Area
{
public:
   //! ProtocolVector provides a vector of OSPF protocols
   using ProtocolVector = std::vector<ProtocolOSPF*>;

   //! ProtocolMapDR provides a mapping of networks to their DR/BDR
   using ProtocolMapDR = std::map<std::string, ProtocolOSPF*>;

   //! RemovedNetworkInterfaces provides a mapping of networks to removed interface addresses.
   //! This aids book-keeping by allowing the removal of protocol pointers during runtime,
   //! but delaying the actual removal of the associated OSPF constructs with their
   //! associated timeouts. Only applies to DR/BDRs.
   using RemovedNetworkInterfaces = std::map<std::string, std::vector<Address>>;

   OSPF_Area()          = default;
   virtual ~OSPF_Area() = default;

   void                      SetAddress(const Address& aAddress) { mArea = aAddress; }
   void                      SetBackbone(bool aIsBackbone) { mIsBackbone = aIsBackbone; }
   const Address&            GetAddress() const { return mArea; }
   bool                      IsBackbone() const { return mIsBackbone; }
   RemovedNetworkInterfaces& GetRemovedInterfacesDR() { return mRemovedInterfacesDR; }
   void                      AddRemovedInterfaceDR(Comm* aInterfacePtr);
   RemovedNetworkInterfaces& GetRemovedInterfacesBDR() { return mRemovedInterfacesBDR; }
   void                      AddRemovedInterfaceBDR(Comm* aInterfacePtr);

   const ProtocolMapDR& GetDRs() const { return mMapDR; }
   const ProtocolMapDR& GetBDRs() const { return mMapBDR; }

   ProtocolVector& GetProtocols() { return mProtocols; }

   //! This version of 'GetProtocols' only returns area protocols/routers with an
   //! interface in the specified network.
   ProtocolVector GetProtocols(const std::string& aNetwork);

   std::set<std::string>& GetNetworks() { return mNetworks; }

   //! Sets the DRs and BDRs for this area. Used during framework initialization only.
   void SetDRs();

   //! Handles the removal of a platform during runtime. Ensures cleanup of
   //! dangling pointers that may be stored, and also provides indication
   //! of loss of DR/BDR.
   void PlatformDeleted(double aSimTime, WsfPlatform* aPlatformPtr);

   //! Sets initial network knowledge graph data for this area.
   //! Based off of the provided graph knowledge. Only used for
   //! OSPF protocol areas defined via user input and initialized at
   //! the start of the simulation. All other areas and members have
   //! their knowledge collected organically via typical OSPF mechanisms.
   //!
   //! Assumes DR/BDR election has already been completed.
   void InitializeGraphData(graph::Graph& aGraph);

   //! Returns all the interfaces in this area with functional routers (i.e. turned on).
   //! The OSPF protocol associated with the interface is also provided for convenience.
   std::vector<std::pair<Comm*, ProtocolOSPF*>> GetInterfaces();

   //! The election event is the process of 'electing' DRs. This occurs initially
   //! at simulation start, then again whenever there is a change affecting
   //! the area such that a new election is required to ensure each network within
   //! the area has a DR and, if possible, a BDR.
   //!
   //! This method is intended for usage during runtime. The 'SetDRs' method calls
   //! this method for each network in the area during initialization.
   //!
   //! The boolean 'aElectionDR' indicates if this is a DR/BDR election if true,
   //! or just a BDR election if false. Regardless, this method will update the
   //! appropriate map entries for DRs and BDRs.
   void ElectionDR(double aSimTime, const std::string aNetwork, const Address& aArea, bool aElectionDR);

   void InitializeCallbacks(WsfSimulation& aSim);

   //! The set networks method should be called anytime there is a change to
   //! any router or interface within the area to ensure the listed networks this
   //! area represents is correct.
   void SetNetworks();

private:
   ProtocolMapDR            mMapDR{};
   ProtocolMapDR            mMapBDR{};
   Address                  mArea{};
   bool                     mIsBackbone{false};
   ProtocolVector           mProtocols{};
   std::set<std::string>    mNetworks{};
   RemovedNetworkInterfaces mRemovedInterfacesDR{};
   RemovedNetworkInterfaces mRemovedInterfacesBDR{};

   //! Callback for router removal at runtime.
   UtCallbackHolder mCallbacks{};
};

} // namespace router
} // namespace comm
} // namespace wsf

WSF_DECLARE_COMPONENT_ROLE_TYPE(wsf::comm::router::ProtocolOSPF, cWSF_COMPONENT_COMM_ROUTER_PROTOCOL_OSPF);

#endif
