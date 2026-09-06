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

#ifndef WSFCOMM_HPP
#define WSFCOMM_HPP

#include "wsf_export.h"

class UtInput;
#include "WsfArticulatedPart.hpp"
#include "WsfCommAddress.hpp"
#include "WsfCommComponent.hpp"
#include "WsfCommLayerMessage.hpp"
#include "WsfCommMediumContainer.hpp"
#include "WsfCommProtocolStack.hpp"
#include "WsfComponentList.hpp"
#include "WsfStringId.hpp"

namespace wsf
{
namespace comm
{

class Message;
class ProtocolInterface;

namespace router
{
class Router;
}

//! A base class for communications systems.
//! Comm provides a common interface among all communication devices.
class WSF_EXPORT Comm : public WsfArticulatedPart
{
   friend class Event;

public:
   //! Vectors indicating linkage to this interface, in different user input formats
   //! for dynamic and static specification.
   using LinkAddress = std::vector<wsf::comm::Address>;
   using LinkPair    = std::vector<std::pair<std::string, std::string>>;

   enum CommType
   {
      cCT_XMT_ONLY = 0x0001,                     //!< Transmit only
      cCT_RCV_ONLY = 0x0002,                     //!< Receive only
      cCT_XMT_RCV  = cCT_XMT_ONLY | cCT_RCV_ONLY //!< Transmit and receive
   };

   //! Enum for multicast conformance level. Based on the presence
   //! (or lack thereof) of multicasting protocols on the comm and/or
   //! the associated router. As such, the value used to describe any
   //! particular comm implementation is not set until the comm framework
   //! has been processed, starting with Initialize2()
   enum class MulticastConformanceLevel : size_t
   {
      cLEVEL_0, // no support for multicasting
      cLEVEL_1, // only supports sending of multicast, receive disabled
      cLEVEL_2  // full multicast support
   };

   //! The gateway definition for an individual comm interface. Dictates
   //! where this interface should forward a message in the absence of
   //! routing. This is necessary, as interfaces may have multiple connections,
   //! and this allows the specification of a single connection for forwarding.
   struct Gateway
   {
      Address     mAddress;
      std::string mPlatformName;
      std::string mCommName;
   };

   Comm(const WsfScenario& aScenario, CommType aCommType = cCT_XMT_RCV);
   ~Comm() override = default;
   Comm& operator=(const Comm& aRhs) = delete;

   //! @name Component list support.
   //! A comm contains a list of components.
   //@{
   using ComponentList = WsfComponentListT<WsfComponentT<Comm>>;
   ComponentList&       GetComponents() { return mComponents; }
   const ComponentList& GetComponents() const { return mComponents; }
   //@}

   //! @name Component infrastructure methods not provided by WsfPlatformPart.
   //@{
   WsfComponent* CloneComponent() const override { return Clone(); }
   const int*    GetComponentRoles() const override;
   void*         QueryInterface(int aRole) override;
   int           GetComponentInitializationOrder() const override { return cWSF_INITIALIZE_ORDER_COMM; }
   //@}

   //! @name Common infrastructure methods.
   //@{
   const char*  GetScriptClassName() const override;
   Comm*        Clone() const override;
   bool         PreInitialize(double aSimTime) override;
   bool         Initialize(double aSimTime) override;
   bool         Initialize2(double aSimTime) override;
   void         PreInput() override;
   bool         ProcessInput(UtInput& aInput) override;
   void         Update(double aSimTime) override;
   virtual void Setup();
   virtual void PendingStart();
   //@}

   //! @name Status methods.
   //@{
   void PartBroken(double aSimTime) override;
   void SetNonOperational(double aSimTime) override;
   void SetOperational(double aSimTime) override;
   void TurnOff(double aSimTime) override;
   void TurnOn(double aSimTime) override;
   //@}

   //! @name Link Methods
   //! Specifies linkage to this interface.
   //@{
   virtual void AddLinkAddress(const Address& aAddress) { mLinkAddresses.push_back(aAddress); }
   virtual void AddLink(const std::string& aPlatformName, const std::string& aCommName)
   {
      mLinks.emplace_back(aPlatformName, aCommName);
   }
   //@}

   //! @name Gateway Methods
   //@{
   const Address&     GetGateway() const { return mGateway.mAddress; }
   const std::string& GetGatewayRemotePlatform() const { return mGateway.mPlatformName; }
   const std::string& GetGatewayRemoteComm() const { return mGateway.mCommName; }
   void               SetGateway(const Address& aAddress);
   //@}

   //! @name CanSendTo
   //! The CanSendTo method provides an interface for use with all
   //! comm objects as a unified entry point to determine whether or not
   //! another comm device can receive a transmission from this comm object.
   //! The determination of this may rely on many factors based on the actual
   //! implementation of each comm device. For example, the base case may only
   //! check that the receiving comm is on and is able to receive. More advanced
   //! versions may rely on the existence of specific hardware, or even EM
   //! interactions.
   virtual bool CanSendTo(double aSimTime, Comm* aRcvrPtr, Message* aMessagePtr = nullptr);

   //! @name Send/Receive methods
   //! @note The send method does not utilize the comm message object at this time
   //! for legacy support purposes. It is expected that AFSIM 3.0 will require the usage
   //! of the WsfCommMessage for send purposes to mirror the receive call for a multitude
   //! of benefits. In addition, it is likely the explicit comm pointer reference will
   //! be replaced with an address, such that both send and receive methods will have the
   //! same signature.
   //@{
   virtual bool Receive(double aSimTime, Comm* aXmtrPtr, Message& aMessage);
   virtual bool Send(double aSimTime, std::unique_ptr<WsfMessage> aMessagePtr, const Address& aAddress);
   //! Deprecated in favor of passing a unique_ptr to Send so that
   //! send doesn't have to perform a deepcopy of the message.
   WSF_DEPRECATED
   virtual bool Send(double aSimTime, const WsfMessage& aMessage, const Address& aAddress) final;
   //@}

   //! @name Comm event methods.
   //@{
   virtual bool ProcessLayerMessage(double aSimTime, size_t aLayerIndex, layer::Message aLayerMessage, Message* aCommMessagePtr);
   //@}

   //! Returns the comm device's true type as was intended on construction.
   CommType GetCommType() const { return mCommTrueType; }

   //! Get the class ID associated with the object (poor mans RTTI).
   //! @return Returns the associated class ID.
   WsfStringId GetClassId() const { return mClassId; }

   //! Returns the comm device's unique address.
   //! @return Returns the comm device's unique address.
   const Address& GetAddress() const { return mAddress; }

   //! Returns the user indicated interface address.
   const Address& GetInputAddress() const { return mInputAddress; }

   //! Returns the comm device's network membership.
   //! @return Returns the comm device's network name.
   const std::string& GetNetwork() const { return mNetwork; }

   //! Returns the number of channels this comm can
   //! support (hardware definition).
   size_t GetNumChannels() const { return mNumChannels; }

   //! Returns the user indicated network membership.
   const std::string& GetInputNetwork() const { return mInputNetwork; }

   //! Returns the user indicated network membership in address format.
   const Address& GetInputNetworkAddress() const { return mInputNetworkAddress; }

   //! Returns the user suppled interface linkage in address format.
   const LinkAddress& GetLinkAddresses() const { return mLinkAddresses; }

   //! Returns the user supplied interface linkage in platform/comm name format.
   const LinkPair& GetLinkPairs() const { return mLinks; }

   //! Sets the comm devices' unique address.
   //! @param aAddress The unique address.
   void SetAddress(const Address& aAddress) { mAddress = aAddress; }
   void SetNetwork(const std::string& aNetwork) { mNetwork = aNetwork; }

   //! Returns comm device name as a string
   virtual std::string GetFullName() const;

   //! Returns whether the comm device can currently transmit or receive.
   //! This determination can be based on may factors and
   //! can be modified by derived classes.
   virtual bool CanReceive() const { return mReceiveEnabled; }
   virtual bool CanSend() const { return mTransmitEnabled; }

   //! Sets the current state of the device to receive or transmit.
   //! @param aIsEnabled   True if the comm device is enabled.
   void SetReceiveEnabled(bool aIsEnabled) { mReceiveEnabled = aIsEnabled; }
   void SetTransmitEnabled(bool aIsEnabled) { mTransmitEnabled = aIsEnabled; }

   //! Gets/sets the router this comm is assigned to.
   //! @note Setting the router interface should only be done by
   //! the router itself under normal circumstances. The router pointer
   //! is provided as a convenience method, and can be modified by
   //! adding/removing the comm via the router methods.
   //@{
   router::Router*    GetRouter() const { return mRouterPtr; }
   void               SetRouter(router::Router* aRouterPtr);
   const std::string& GetRouterName() const { return mRouterName; }
   void               SetRouterName(const std::string& aRouterName) { mRouterName = aRouterName; }
   //@}

   //! This method call handles the query of protocols associated with this
   //! comm object (via the component list) and provides a list to these
   //! protocols in their order of precedence of operation.
   std::vector<ProtocolInterface*> GetSortedProtocols() const;

   //! @name Multicast Methods
   //@{
   MulticastConformanceLevel GetMulticastLevel() const { return mMulticastLevel; }

   //! The set method for multicast level is determined by the availability of
   //! particular protocols. It should be called to set the multicast conformance
   //! level if any protocols change or the defined router for this interface
   //! is modified.
   virtual void SetMulticastLevel();
   //@}

   //! Accessor/mutator for protocol stack
   //@{
   ProtocolStack&       GetProtocolStack() { return mProtocolStack; }
   const ProtocolStack& GetProtocolStack() const { return mProtocolStack; }
   //@}

   //! Send/Receive method hooks


   //! @name Send/Receive Setup Calls
   //! Implementation defined methods for collecting common actions taken
   //! for message transmission/reception.
   //! In the base implementation, the send method takes a newly created
   //! comm::Message from a standard WsfMessage and performs necessary
   //! setup for comm framework utilization prior/during the public Send
   //! call.
   //! In the receive setup method, the base implementation simply checks
   //! that the comm is on and prints debug information, if applicable.
   //@{
   virtual bool SendSetup(double aSimTime, Message& aMessage, const Address& aAddress);

   virtual bool ReceiveSetup(double aSimTime, Comm* aXmtrPtr, Message& aMessage);
   //@}

   //! @name Send/Receive Action Call
   //! Implementation defined methods for collecting common actions taken
   //! for message transmission/reception upon determination of the success
   //! or failure of a transmission/reception via Send/Receive call functionality.
   //@{
   virtual bool SendActions(double aSimTime, bool aSendStatus, Message& aMessage);

   virtual bool ReceiveActions(double aSimTime, bool aReceiveStatus, Comm* aXmtrPtr, Message& aMessage);

protected:
   //! Displays debug info associated with message reception.
   virtual void PrintDebugReception(double aSimTime, const Message& aMessage) const;

   // Copy constructor
   Comm(const Comm& aSrc);

   // Comm by default has no update interval
   // Override these methods to get regular Update() calls
   void   SetUpdateInterval(double aInterval) override {}
   double GetUpdateInterval() const override { return 0.0; }

   //! Sets the class ID associated with this object (poor mans RTTI).
   //! @param aClassId Sets the associated class ID.
   void SetClassId(WsfStringId aClassId) { mClassId = aClassId; }

   //! Helper method for ensuring/setting valid gateways.
   virtual bool ValidateGateway();

   //! Protocol stack used by this comm device.
   ProtocolStack mProtocolStack;

   //! Addressing user or implementation variables
   //! These are the variables used by the network manager to drive addressing and network
   //! membership. These are typically provided by user input, or may be modified by a
   //! comm derivation to drive how the comm is ultimately addressed in the simulation.
   //@{
   //! A user supplied address indicated for use with this interface. May be null if not specified.
   Address mInputAddress;

   //! A user supplied network, for indicating which network this interface should join.
   std::string mInputNetwork;

   //! A user supplied network address, for indicating which network this interface should join.
   Address mInputNetworkAddress;
   //@}

   //! The number of channels, or simultaneous transmissions,
   //! that this comm model is capable of supporting. Note that
   //! this value may limit or be limited by the actual medium
   //! being used by this comm, which is determined at runtime.
   size_t mNumChannels;


private:
   //! Type of communication device - Transmit, receive, or both
   //! This should be used as a truth state.
   CommType mCommTrueType;

   // Comm device's current ability to transmit or receive.
   // Initialized to the truth type but can be changed later
   // to reflect the current state of the device.
   bool mReceiveEnabled;  // 'true' if receive enabled
   bool mTransmitEnabled; // 'true' if transmit enabled

   //! List of components attached to this device.
   ComponentList mComponents;

   //! Addressing member variables
   //! These variables are set by the network manager, utilizing the
   //! protected variables related to addressing and network membership.
   //@{
   //! The address of the comm interface, assigned via the network manager.
   Address mAddress;

   //! The network this interface belongs to. Assigned by the network manager.
   std::string mNetwork;
   //@}

   WsfStringId mClassId; //!< Poor man's RTTI

   //! The following member variables specify linkage with this comm interface.
   //@{

   //! Links specified by address (static address usage).
   LinkAddress mLinkAddresses;

   //! Links specified by platform and comm name (dynamic address usage).
   LinkPair mLinks;

   //! @name Router Assignments
   //! Comms can be assigned to a specific router. Unless the user dictates
   //! a different assignment, comms are assigned to the default router.
   //! A comm may only be assigned to a single router.
   //@{

   //! A name of the current router this comm is an interface to.
   std::string mRouterName;

   //! A pointer to the current router this comm is an interface to.
   router::Router* mRouterPtr;
   //@}

   //! The gateway for this comm interface, if provided.
   Gateway mGateway;

   //! A description of this comm's multicast conformance level.
   //! For non-IP based comm implementations, this value should
   //! always be level 0.
   MulticastConformanceLevel mMulticastLevel;
};

} // namespace comm
} // namespace wsf

UT_MAP_CLASS_TO_SCRIPT_NAME(wsf::comm::Comm, "WsfComm")

WSF_DECLARE_COMPONENT_ROLE_TYPE(wsf::comm::Comm, cWSF_COMPONENT_COMM)
WSF_DECLARE_COMPONENT_ROLE_TYPE(wsf::comm::medium::ContainerComponent<wsf::comm::Comm>,
                                cWSF_COMPONENT_COMM_MEDIUM_CONTAINER_COMM);

#endif
