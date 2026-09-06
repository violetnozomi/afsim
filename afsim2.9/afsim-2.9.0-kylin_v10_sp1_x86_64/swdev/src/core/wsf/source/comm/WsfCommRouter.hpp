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

#ifndef WSFCOMMROUTER_HPP
#define WSFCOMMROUTER_HPP

#include "wsf_export.h"

#include <memory>

#include "UtCallbackHolder.hpp"
class UtInput;
class UtInputBlock;
#include "WsfCommAddress.hpp"
#include "WsfCommMediumContainer.hpp"
#include "WsfCommMessage.hpp"
#include "WsfCommRouterComponent.hpp"
#include "WsfComponentList.hpp"
#include "WsfEvent.hpp"
#include "WsfPlatformPart.hpp"
class WsfScenario;
class WsfSimulation;

namespace wsf
{
namespace comm
{

class Comm;
class Network;
class NetworkManager;

namespace router
{

class ProtocolInterface;

//! The router class object for AFSIM communications.
//! This object is held on every comm object in AFSIM to aid in determination of
//! the path a message is transmitted. It is applicable to both traditional wired
//! networks as well as EM based systems, such as radio. Knowledge of the communications
//! network depends on the protocols used with this router. The default "legacy"
//! protocol simply uses the simulation truth for routing decisions. Other protocols
//! that are based on potentially incomplete, stale, or incorrect network states
//! maintain their own graph that is used for routing.
class WSF_EXPORT Router : public WsfPlatformPart
{
public:
   //! The SendData class is used for marshaling data between the calling comm interface
   //! layer and the router protocols.
   class SendData
   {
   public:
      SendData()          = default;
      virtual ~SendData() = default;

      Comm*                 GetXmtr() const { return mXmtrPtr; }
      void                  SetXmtr(Comm* aXmtrPtr) { mXmtrPtr = aXmtrPtr; }
      std::vector<Message>& GetMessages() { return mMessages; }
      bool                  GetAbortProcessing() const { return mAbortProcessing; }
      void                  SetAbortProcessing(bool aAbort) { mAbortProcessing = aAbort; }

   private:
      Comm*                mXmtrPtr{nullptr};
      std::vector<Message> mMessages{};
      bool                 mAbortProcessing{false};
   };

   struct Gateway;
   using CommVector = std::vector<Comm*>;

   //! @name Component List support
   //@{
   using ComponentList = WsfComponentListT<WsfComponentT<Router>>;
   ComponentList&       GetComponents() { return mComponents; }
   const ComponentList& GetComponents() const { return mComponents; }

   //! Wrapper for adding components directly to the router. Used by the
   //! various component typelist factories to pass created objects to
   //! the router.
   bool AddComponent(RouterComponent* aComponentPtr);
   bool RemoveComponent(RouterComponent* aComponentPtr);
   //@{

   //! The router virtual send/receive methods. The router
   //! handles layer processing (typically the network layer)
   //! of send/receive methods to determine where a message
   //! should be sent (the next hop) to reach its destination.
   //!
   //! The send method also contains an optional vector of messages.
   //! The functionality of some protocols require sending multiple
   //! messages to satisfy a send request. In such a case, any
   //! additional messages will be fully populated with necessary
   //! data members and returned with the send method for handling
   //! by the calling comm interface layer.
   //!
   //! The return of the receive method indicates whether
   //! or not forwarding of the message is required, while allowing
   //! message inspection by the protocols. This return is typically
   //! overridden by a check in addressing against the receiving interface,
   //! such that an address that doesn't map to the interface will
   //! attempt forwarding regardless of the receive return call.
   //! The reference passed boolean value in the receive call indicates
   //! to override this behavior, as some protocols utilize unique
   //! addressing that circumvents typical processing (such as multicast).
   //@{
   virtual bool Receive(double aSimTime, Comm* aXmtrPtr, Comm* aRcvrPtr, Message& aMessage, bool& aOverrideForward);

   virtual bool Send(double aSimTime, SendData& aData);
   //@}

   Router(const WsfScenario& aScenario);
   Router* Clone() const override;
   Router& operator=(const Router& aRhs) = delete;
   Router(Router&& aSrc)                 = delete;
   Router& operator=(Router&& aRhs) = delete;

   ~Router() override = default;

   //! @name Component infrastructure methods
   //@{
   WsfComponent* CloneComponent() const override { return Clone(); }
   void*         QueryInterface(int aRole) override;
   const int*    GetComponentRoles() const override;
   WsfStringId   GetComponentName() const override { return GetNameId(); }
   int           GetComponentInitializationOrder() const override { return cWSF_INITIALIZE_ORDER_ROUTER; }
   //@}

   //! @name Common infrastructure methods
   //@{
   bool         PreInitialize(double aSimTime) override;
   bool         Initialize(double aSimTime) override;
   bool         Initialize2(double aSimTime) override;
   void         PreInput() override;
   virtual void Setup();
   virtual void PendingStart();
   bool         ProcessInput(UtInput& aInput) override;
   virtual bool ProcessTypeAddEditDelete(UtInput& aInput);
   const char*  GetScriptClassName() const override;
   //@}

   //! @name Status methods.
   //@{
   void PartBroken(double aSimTime) override;
   void SetNonOperational(double aSimTime) override;
   void SetOperational(double aSimTime) override;
   void TurnOff(double aSimTime) override;
   void TurnOn(double aSimTime) override;
   //@}

   // Router by default has no update interval
   // Override these methods to get regular Update() calls
   void   SetUpdateInterval(double aInterval) override {}
   double GetUpdateInterval() const override { return 0.0; }

   //! @name Interface/Comm Member methods
   //! @note Prefer usage of the comm pointer methods
   //! when applicable, as they are better performing.
   //@{
   virtual void      AddInterface(double aSimTime, Comm* aCommPtr);
   void              AddInterface(double aSimTime, const Address& aAddress);
   virtual void      RemoveInterface(double aSimTime, Comm* aCommPtr);
   void              RemoveInterface(double aSimTime, const Address& aAddress);
   bool              HasInterface(Comm* aCommPtr) const;
   bool              HasInterface(const Address& aAddress) const;
   void              ClearInterfaces() { mComms.clear(); }
   const CommVector& GetInterfaces() const { return mComms; }
   //@}

   //! The really just returns if the router is turned on, but
   //! we need to wrap it since WsfPlatformPart::IsTurnedOn()
   //! returns false before the simulation starts.
   bool IsActive() const;

   //! Setup the callbacks for this router.
   virtual void InitializeCallbacks();

   //! Router graph manipulation methods.
   //! These methods add/remove nodes/links on the graph, and/or
   //! enables/disables connections. These methods alter what information
   //! is available to the router to make decisions. If the router is set
   //! to perfect routing, these calls will be initiated by the router
   //! "listening" to the callbacks from the network manager.
   //! If the router is using dynamic routing capabilities provided by
   //! an associated protocol, then these calls are made when appropriate
   //! and may result in a network representation locally that differs from
   //! "truth" (a.k.a. the network manager).
   //@{
   virtual bool AddLink(double             aSimTime,
                        const Address&     aSender,
                        const Address&     aDestination,
                        ProtocolInterface& aProtocol,
                        bool               aNotifyObserver = true);

   virtual bool RemoveLink(double             aSimTime,
                           const Address&     aSender,
                           const Address&     aDestination,
                           ProtocolInterface& aProtocol,
                           bool               aNotifyObserver = true);

   virtual bool AddNode(double aSimTime, const Address& aNodeAddress, ProtocolInterface& aProtocol, bool aNotifyObserver = true);

   virtual bool
   RemoveNode(double aSimTime, const Address& aNodeAddress, ProtocolInterface& aProtocol, bool aNotifyObserver = true);

   virtual bool EnableLink(double             aSimTime,
                           const Address&     aSender,
                           const Address&     aDestination,
                           ProtocolInterface& aProtocol,
                           bool               aNotifyObserver = true);

   virtual bool DisableLink(double             aSimTime,
                            const Address&     aSender,
                            const Address&     aDestination,
                            ProtocolInterface& aProtocol,
                            bool               aNotifyObserver = true);

   virtual bool EnableNode(double aSimTime, const Address& aNodeAddress, ProtocolInterface& aProtocol);

   virtual bool DisableNode(double aSimTime, const Address& aNodeAddress, ProtocolInterface& aProtocol);
   //@}

   //! GetGateway returns the gateway address. It must only be called during runtime,
   //! as gateways specified by platform and comm name can only be reconciled after
   //! simulation initialization. If the platform/comm name was used to specify the
   //! gateway, the gateway validation method fills the missing address.
   const Address&  GetGateway() const { return mGateway.mAddress; }
   void            SetGateway(const Address& aAddress);
   size_t          GetHopLimit() const { return mHopLimit; }
   NetworkManager* GetNetworkManager() const { return mNetworkManager; }

   void SetHopLimit(size_t aHopLimit) { mHopLimit = aHopLimit; }

   //! Simple struct containing gateway information. Only the address is ultimately needed,
   //! but such information may not be resolvable before initialization. Note that this
   //! gateway definition is different from the comm interface gateway, such that the
   //! router gateway specifies which interface to use, which is local to the platform.
   //! The comm interface gateway itself determines the destination.
   struct Gateway
   {
      Address     mAddress;
      std::string mGatewayCommName;

      Gateway()                    = default;
      Gateway(const Gateway& aSrc) = default;
   };

   //! This method enables basic broadcasting functionality.
   //! Be warned that this message isn't true broadcasting, as it simply sends
   //! multiple messages to each member of the network. This functionality requires
   //! a more robust modeling of interface connections, which is currently lacking.
   //! When and if that capability is developed, this method can be deprecated.
   //! Returns true if successful, prompting appropriate usage in the RouterSend
   //! usage of this method.
   virtual bool Broadcast(double aSimTime, SendData& aData);

   //! This method call handles the query of protocols associated with this
   //! router object (via the component list) and provides a list of these
   //! protocols in their order of precedence of operation.
   std::vector<ProtocolInterface*> GetSortedProtocols() const;

protected:
   Router(const Router& aSrc);

   //! Helper method for determining if user set values indicate a valid gateway.
   //! Returns true if valid, or if the gateway was not set.
   virtual bool ValidateGateway();

   //! Helper method for creating linkage between comm interfaces.
   //! This also queries the network type
   //! for correct add/removal of linkage, to prevent the addition
   //! of links that violate the specified network rules of topology,
   //! if applicable.
   //@{
   virtual bool AutomateAddedLinkage(double aSimTime, Comm* aCommPtr);
   //@}

   //! Network Manager Callbacks
   //! Handles the local graph actions on notification from the network manager when
   //! this router is using perfect routing.
   //@{
   virtual void PerfectCommAdded(double aSimTime, Comm* aCommPtr);
   virtual void PerfectCommRemoved(double aSimTime, Comm* aCommPtr);
   virtual void PerfectNetworkAdded(double aSimTime, Network* aNetworkPtr);
   virtual void PerfectNetworkRemoved(double aSimTime, Network* aNetworkPtr);
   virtual void PerfectCommEnabled(double aSimTime, Comm* aCommPtr);
   virtual void PerfectCommDisabled(double aSimTime, Comm* aCommPtr);
   virtual void PerfectConnectionAdded(double aSimTime, const Address& aSourceAddress, const Address& aDestinationAddress);
   virtual void PerfectConnectionRemoved(double aSimTime, const Address& aSourceAddress, const Address& aDestinationAddress);
   virtual void PerfectConnectionEnabled(double aSimTime, const Address& aSourceAddress, const Address& aDestinationAddress);
   virtual void PerfectConnectionDisabled(double aSimTime, const Address& aSourceAddress, const Address& aDestinationAddress);
   //@}

private:
   NetworkManager* mNetworkManager;

   //! The gateway address to forward messages to in the case of unresolved
   //! pathing to a destination. If missing, any pathing failure due to
   //! perceived knowledge at this router will result in inability to send a message.
   Gateway mGateway;

   //! The local hop limit for this router. If this router is queried to find a path,
   //! and the hop limit / time to live value has been exceeded, then a failure will result.
   //! This is needed even during non wired/RF routing to avoid circular routing and
   //! stack overflow.
   size_t mHopLimit;

   //! Component list holds objects that the router needs to do its job
   //! and allows extensibility in core command processing. This will always
   //! contain a RoutingAlgorithm, and possibly a RoutingProtocol, along with
   //! possible user provided objects.
   ComponentList mComponents;

   const WsfScenario& mScenario;
   UtCallbackHolder   mCallbacks;

   //! The comm "interfaces" assigned and available to this router.
   CommVector mComms;

   //! If true, comm interfaces added and removed from a router will have
   //! required linkage between interfaces created and removed when those
   //! interfaces are added or removed from a router. If false, the user
   //! can manage these directly.
   bool mAutomatedInterfaceLinking;

   //! Indicates whether or not to use the legacy routing protocol.
   bool mUseLegacyProtocol;

   //! Indicates whether or not to use the default multicast routing protocol.
   bool mUseMulticastProtocol;

   //! Note - Future Capability
   //! Data transfer rate and other factors to router message/packet processing and
   //! provision to comm interfaces will be supported in a future update where
   //! mediums are modeled and supported on a link-by-link basis. Currently,
   //! messages will be propagated between comms connected by a router as has been
   //! done previously - using the values set on each sending comm interface.
};

} // namespace router
} // namespace comm
} // namespace wsf

WSF_DECLARE_COMPONENT_ROLE_TYPE(wsf::comm::router::Router, cWSF_COMPONENT_COMM_ROUTER)
WSF_DECLARE_COMPONENT_ROLE_TYPE(wsf::comm::medium::ContainerComponent<wsf::comm::router::Router>,
                                cWSF_COMPONENT_COMM_MEDIUM_CONTAINER_ROUTER);

#endif
