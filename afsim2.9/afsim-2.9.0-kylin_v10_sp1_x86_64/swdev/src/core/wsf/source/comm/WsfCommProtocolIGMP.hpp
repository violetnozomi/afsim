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

#ifndef WSFCOMMPROTOCOLIGMP_HPP
#define WSFCOMMPROTOCOLIGMP_HPP

#include "wsf_export.h"

#include <unordered_set>

#include "UtScriptBasicTypes.hpp"
#include "WsfCommAddress.hpp"
#include "WsfCommGraph.hpp"
#include "WsfCommProtocolInterface.hpp"
#include "WsfEvent.hpp"
#include "WsfRandomVariable.hpp"

namespace wsf
{
namespace comm
{

//! This protocol is a lightweight implementation of the functionality
//! of the Internet Group Management Protocol (IGMP). Typically, IGMP
//! is a protocol that defines how comm interfaces and routers
//! communicate with each other and determine how to propagate
//! messages to relay joining and leaving multicast groups. This allows
//! routers to have the prerequisite knowledge necessary to know how
//! to route multicast messages.
//!
//! This implementation uses perfect knowledge, and does not actually generate
//! protocol messaging. It 'cheats' by using multicast
//! data stored in the network manager, and is only intended to
//! resolve addressing. This protocol is provided in this manner
//! to reduce performance overhead and not clutter the simulation
//! with message events when users strictly want multicast messaging
//! without the potential pitfalls of a full, high fidelity implementation.
//!
//! This protocol does not inherently provide routing capability, and
//! is reliant on the inherent PIM router protocol to provide that service.
//! As such, if PIM is removed from the router, multicast messaging that
//! passes through such a router will fail without an analogue capability.
class WSF_EXPORT ProtocolIGMP : public ProtocolInterface
{
public:
   static ProtocolIGMP* Find(const Comm& aParent);
   static ProtocolIGMP* FindOrCreate(Comm& aParent);

   ProtocolIGMP()           = default;
   ~ProtocolIGMP() override = default;

   //! @name Required ProtocolInterface interface.
   //@{
   WsfComponent*    CloneComponent() const override;
   const int*       GetComponentRoles() const override;
   void*            QueryInterface(int aRole) override;
   ProtocolPriority GetPriority() const override;
   ProtocolIGMP*    Clone() const override;
   void             Setup() override;
   const char*      GetScriptClassName() const override { return "WsfCommProtocolIGMP"; }
   //@}

   bool Initialize(double aSimTime) override;
   bool ProcessInput(UtInput& aInput) override;

   //! @name Explicit Protocol Emulation Methods
   //! @note We don't explicitly model all the network
   //! communication for IGMP. These change on the presence
   //! of other protocols, the protocol version, etc.
   //! We only explicitly send join or leave requests with an
   //! associated delay for modelling purposes, as a full
   //! implementation would be far more taxing on performance
   //! and require a myriad of user defined settings.
   //@{
   Address      GetQuerier() const;
   virtual void LeaveGroup(double aSimTime, const Address& aAddress);
   virtual void JoinGroup(double aSimTime, const Address& aAddress);
   //@}

   //! @name ProtocolProcess methods
   //! These methods implement the IGMP logic in the interface layers.
   //@{
   ProcessResult NetworkSend(double aSimTime, size_t aLayerIndex, Message& aMessage) override;
   ProcessResult NetworkReceive(double aSimTime, size_t aLayerIndex, Message& aMessage) override;
   ProcessResult PhysicalReceive(double aSimTime, size_t aLayerIndex, Message& aMessage) override;
   //@}

   std::vector<Address> GetGroups() const { return mJoinGroups; }

protected:
   //! A simple data structure to hold routing data.
   struct RouteData
   {
      Address                     mNextHop;
      size_t                      mHops;
      double                      mCost;
      graph::AddressList          mFullPath;
      std::unordered_set<Address> mDestinationSet;
   };

   Address GetMessageAddressFromAux(Message& aMessage);

   //! @name Leave/Join Group protected methods
   //! These methods are only intended for scenario input processing, prior to
   //! initialization. Joining and leaving groups during runtime requires
   //! different behaviors.
   //@{
   void JoinGroup(const Address& aAddress) { mJoinGroups.push_back(aAddress); }
   void LeaveGroup(const Address& aAddress);
   //@}

private:
   std::vector<Address> mJoinGroups{};

   //! A general abstraction of delays in joining a group. These may be caused
   //! by querier polling time, host response time, and other associated delays
   //! that a user wishes to incorporate.
   WsfRandomVariable mJoinDelay{0.0, WsfRandomVariable::cNON_NEGATIVE};

   //! A general abstraction of delays in leaving a group. These may be caused
   //! by querier polling time, last member response times, and other associated
   //! delays that a user wishes to incorporate.
   WsfRandomVariable mLeaveDelay{0.0, WsfRandomVariable::cNON_NEGATIVE};
};

//! An event class for handling join/leave multicast delays.
class WSF_EXPORT IGMP_SubscriptionEvent : public WsfEvent
{
public:
   enum class EventType : size_t
   {
      cJOIN,
      cLEAVE
   };

   IGMP_SubscriptionEvent(double         aSimTime,
                          EventType      aEventType,
                          const Address& aCommAddress,
                          const Address& aMulticastGroupAddress);

   ~IGMP_SubscriptionEvent() override = default;

   EventDisposition Execute() override;

private:
   EventType mEventType;
   Address   mCommAddress;
   Address   mMulticastAddress;
};

//! Script class for this protocol.
class WSF_EXPORT ScriptProtocolClassIGMP : public ScriptProtocolClass
{
public:
   ScriptProtocolClassIGMP(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   ~ScriptProtocolClassIGMP() override = default;

   UT_DECLARE_SCRIPT_METHOD(JoinMulticastGroup);
   UT_DECLARE_SCRIPT_METHOD(LeaveMulticastGroup);
};

} // namespace comm
} // namespace wsf

WSF_DECLARE_COMPONENT_ROLE_TYPE(wsf::comm::ProtocolIGMP, cWSF_COMPONENT_COMM_PROTOCOL_IGMP);

#endif
