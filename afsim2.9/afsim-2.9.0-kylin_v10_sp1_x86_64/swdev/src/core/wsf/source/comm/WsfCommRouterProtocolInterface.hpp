// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCOMMROUTERPROTOCOLINTERFACE_HPP
#define WSFCOMMROUTERPROTOCOLINTERFACE_HPP

#include "wsf_export.h"

#include "UtScriptTypes.hpp"
#include "WsfCommGraph.hpp"
#include "WsfCommMessage.hpp"
#include "WsfCommProtocolPriority.hpp"
#include "WsfCommRouter.hpp"
#include "WsfCommRouterComponent.hpp"
#include "WsfObject.hpp"

namespace wsf
{
namespace comm
{
namespace router
{

class WSF_EXPORT ProtocolInterface : public RouterComponent, public WsfObject
{
public:
   ProtocolInterface()           = default;
   ~ProtocolInterface() override = default;
   const char* GetScriptClassName() const override { return "WsfCommRouterProtocol"; }

   //! @name Required interface from WsfComponent.
   //@{
   WsfComponent* CloneComponent() const override = 0;
   WsfStringId   GetComponentName() const override final { return GetNameId(); }
   const int*    GetComponentRoles() const override = 0;
   void*         QueryInterface(int aRole) override = 0;
   //@}

   //! @name Required interface from WsfObject
   //@{
   ProtocolInterface* Clone() const override = 0;
   //@}

   //! @name Interface from WsfCommComponent.
   //@{
   bool Initialize(double aSimTime) override { return true; }
   bool ProcessInput(UtInput& aInput) override { return false; }
   //@}

   //! @name Operator Overloads
   //@{
   bool operator<(const ProtocolInterface& aRhs) const { return GetPriority() < aRhs.GetPriority(); }
   //@}

   //! @name Graph methods
   //! If a protocol uses a graph representation, this
   //! method should return it for operations. Not all
   //! protocols have this functionality, and should
   //! return a nullptr on the GetGraph function
   //! if that is the case. The ShouldPropagateTruth
   //! function provides whether or not the router should
   //! push updates from the truth representation (the network manager)
   //! to this protocol during updates (otherwise it is up to the
   //! protocol or direct user input do accomplish this).
   //@{
   virtual bool          ShouldPropagateTruth() const = 0;
   virtual graph::Graph* GetGraph() const             = 0;
   //@}

   //! Required query for protocol priority.
   //! Protocols are evaluated in order based on
   //! their assigned priorities. A lower value
   //! indicates a higher priority.
   virtual int GetPriority() const = 0;

   //! The routing method is used by the router
   //! to determine the route based on the protocol defined
   //! routing algorithm. A return of an empty vector
   //! indicates that no path was found. The router
   //! will utilize any of its available interfaces
   //! to find a valid path.
   virtual std::vector<Address> Routing(double         aSimTime,
                                        const Address& aSendingInterface,
                                        size_t&        aHopNumber,
                                        double&        aCost,
                                        const Message& aMessage) = 0;

   //! @name Send/Receive methods
   //! The send and receive methods for the routing protocol
   //! provide hooks into querying each protocol for router
   //! message processing.
   //!
   //! The send method is queried, allowing the protocol to process the
   //! the message prior to a transmission. By default, this
   //! method returns true with no additional message processing.
   //! Returning false indicates an issue with this protocol not
   //! being able to process this message transmission, upon which
   //! the router will attempt the same process for other available
   //! protocols (if applicable). This method also allows the
   //! protocol to conduct internal bookkeeping when messages are sent.
   //!
   //! The receive method is called when an interface receives a message.
   //! If the destination address of the message isn't natively mapped
   //! to the receiving interface, a protocol may use a different addressing
   //! schema (such as in multicast groups). This method also allows the
   //! protocol to conduct internal bookkeeping when messages are received.
   //! This method returns true to indicate the message is intended for the
   //! current receiving interface, false if not. Any given protocol that returns
   //! false will allow other, lower priority protocols, an attempt to
   //! process this message after returning false.
   //@{
   virtual bool Send(double aSimTime, Router::SendData& aData) { return false; }

   virtual bool Receive(double aSimTime, const Address& aReceivingInterface, Message& aMessage, bool& aOverrideForward)
   {
      return false;
   }
   //@}

   //! @name State Notifications
   //! Some protocols may wish to take implementation defined actions
   //! based on truth state updates in the simulation via the Network Manager.
   //! These methods may be overridden, and are called during the applicable
   //! event from the router.
   //@{
   virtual void PerfectCommAdded(double aSimTime, Comm* aCommPtr) {}
   virtual void PerfectCommRemoved(double aSimTime, Comm* aCommPtr) {}
   virtual void PerfectNetworkAdded(double aSimTime, Network* aNetworkPtr) {}
   virtual void PerfectNetworkRemoved(double aSimTime, Network* aNetworkPtr) {}
   virtual void PerfectCommEnabled(double aSimTime, Comm* aCommPtr) {}
   virtual void PerfectCommDisabled(double aSimTime, Comm* aCommPtr) {}
   virtual void PerfectConnectionAdded(double aSimTime, const Address& aSourceAddress, const Address& aDestinationAddress)
   {
   }
   virtual void PerfectConnectionRemoved(double aSimTime, const Address& aSourceAddress, const Address& aDestinationAddress)
   {
   }
   virtual void PerfectConnectionEnabled(double aSimTime, const Address& aSourceAddress, const Address& aDestinationAddress)
   {
   }
   virtual void PerfectConnectionDisabled(double aSimTime, const Address& aSourceAddress, const Address& aDestinationAddress)
   {
   }
   //@}
};

//! Base interface script class.
class WSF_EXPORT ScriptRouterProtocolClass : public UtScriptClass
{
public:
   ScriptRouterProtocolClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   ~ScriptRouterProtocolClass() override = default;
};

} // namespace router
} // namespace comm
} // namespace wsf

WSF_DECLARE_COMPONENT_ROLE_TYPE(wsf::comm::router::ProtocolInterface, cWSF_COMPONENT_COMM_ROUTER_PROTOCOL);

#endif
