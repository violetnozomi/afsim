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

#ifndef WSFCOMMPROTOCOLMULTICAST_HPP
#define WSFCOMMPROTOCOLMULTICAST_HPP

#include "wsf_export.h"

#include <unordered_set>

#include "WsfCommAddress.hpp" // for Address
#include "WsfCommGraph.hpp"
#include "WsfCommRouter.hpp"
#include "WsfCommRouterProtocolInterface.hpp"
#include "WsfComponentRoles.hpp" // for WSF_DECLARE_COMPONENT_ROLE_TYPE
class UtScriptTypes;
class WsfComponent;

namespace wsf
{
namespace comm
{

class Message;

namespace router
{

//! This protocol provides a generic multicasting routing capability
//! in AFSIM. It is based on perfect knowledge, and is only intended
//! to provide emulated multicast routing behavior such that forwarded
//! multicast messages are only duplicated and propagated when necessary
//! to deliver to recipients along unique paths.
//!
//! This routing protocol does not provide the functionality to join or leave
//! multicast groups, nor does it maintain any particular network knowledge.
//! This protocol is intended to be provided by default as a baseline multicasting
//! routing protocol in the absence of, or need for, higher fidelity multicast
//! capabilities.
class WSF_EXPORT ProtocolMulticast : public ProtocolInterface
{
public:
   static ProtocolMulticast* Find(const Router& aParent);
   static ProtocolMulticast* FindOrCreate(Router& aParent);

   ProtocolMulticast()           = default;
   ~ProtocolMulticast() override = default;

   //! @name Required ProtocolInterface interface.
   //@{
   WsfComponent*      CloneComponent() const override;
   const int*         GetComponentRoles() const override;
   void*              QueryInterface(int aRole) override;
   int                GetPriority() const override;
   ProtocolMulticast* Clone() const override;

   std::vector<Address> Routing(double         aSimTime,
                                const Address& aSendingInterface,
                                size_t&        aHopNumber,
                                double&        aCost,
                                const Message& aMessage) override;

   const char* GetScriptClassName() const override { return "WsfCommProtocolMulticast"; }
   //@}

   void Setup() override;

   //! @name Graph methods
   //@{
   bool          ShouldPropagateTruth() const override { return false; }
   graph::Graph* GetGraph() const override { return mGraph; }
   //@}

   bool Send(double aSimTime, Router::SendData& aData) override;

   bool Receive(double aSimTime, const Address& aReceivingInterface, Message& aMessage, bool& aOverrideForward) override;

   bool Initialize(double aSimTime) override;

protected:
   //! A simple data structure to hold routing data.
   struct RouteData
   {
      Address                     mNextHop;
      size_t                      mHops{};
      double                      mCost{};
      graph::AddressList          mFullPath{};
      std::unordered_set<Address> mDestinationSet{};
   };

   //! A convenience method for message processing. This method
   //! indicates whether a message received on a particular interface
   //! will need forwarding to other interfaces.
   virtual bool ForwardMessage(double aSimTime, const Address& aReceivingInterface, Message& aMessage);

private:
   graph::Graph* mGraph{nullptr};
};

//! Script class for this protocol.
class WSF_EXPORT ScriptProtocolClassMulticast : public ScriptRouterProtocolClass
{
public:
   ScriptProtocolClassMulticast(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   ~ScriptProtocolClassMulticast() override = default;
};

} // namespace router
} // namespace comm
} // namespace wsf

WSF_DECLARE_COMPONENT_ROLE_TYPE(wsf::comm::router::ProtocolMulticast, cWSF_COMPONENT_COMM_ROUTER_PROTOCOL_MULTICAST);

#endif
