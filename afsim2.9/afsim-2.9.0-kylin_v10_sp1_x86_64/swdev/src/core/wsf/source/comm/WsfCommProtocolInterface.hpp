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

#ifndef WSFCOMMPROTOCOLINTERFACE_HPP
#define WSFCOMMPROTOCOLINTERFACE_HPP

#include "wsf_export.h"

#include "UtScriptTypes.hpp"
#include "WsfCommComponent.hpp"
#include "WsfCommProtocolPriority.hpp"
#include "WsfObject.hpp"
#include "WsfSimpleComponent.hpp"

namespace wsf
{
namespace comm
{

//! The ProtocolInterface class is an interface for any implementation of
//! of protocols for use with comms in AFSIM. Any comm object may use
//! multiple protocols, which are called during typical layer operations.
//! It is the responsibility of the user implementing these protocols to
//! ensure compatibility, especially with the basic protocols pre-defined
//! within the AFSIM comms framework. Protocols are components, which are
//! added directly to comm objects, and are subject to addition, modification,
//! and removal at any time during a simulation.
class WSF_EXPORT ProtocolInterface : public Component, public WsfObject
{
public:
   using ProcessResult = std::pair<bool, bool>;

   ProtocolInterface()           = default;
   ~ProtocolInterface() override = default;
   const char* GetScriptClassName() const override { return "WsfCommProtocol"; }

   //! @name Required interface from WsfComponent.
   //@{
   WsfComponent* CloneComponent() const override = 0;
   WsfStringId   GetComponentName() const final { return GetNameId(); }
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

   //! Required query for protocol priority.
   //! Protocols are evaluated in order based on
   //! their assigned priorities. A lower value
   //! indicates a higher priority.
   virtual ProtocolPriority GetPriority() const = 0;

   //! @name Layer Processing Methods
   //! These methods are the default logic implementations
   //! for the protocol. During layer processing,
   //! the appropriate referenced method will be
   //! called to allow the protocol to do its work.
   //! The message/packet being worked is passed, to
   //! allow header modifications and data passage.
   //!
   //! A pair of boolean values is returned from these
   //! methods when processing in a layer.
   //! The first value indicates whether or not the layer
   //! should stop processing other protocols. Some protocols
   //! take ownership of message processing, while others
   //! work in cooperation, and this allows the protocol
   //! to define that behavior to avoid conflicts (along with
   //! the explicit ordering of protocol priority level).
   //! The second value indicates whether or not the protocol
   //! processed the message successfully, and generally
   //! indicates that the message should notify the next
   //! layer.
   //@{
   //! @name Application Layer Send/Receive
   //@{
   virtual ProcessResult ApplicationSend(double aSimTime, size_t aLayerIndex, Message& aMessage)
   {
      return std::make_pair(false, true);
   }
   virtual ProcessResult ApplicationReceive(double aSimTime, size_t aLayerIndex, Message& aMessage)
   {
      return std::make_pair(false, true);
   }
   //@}
   //! @name Presentation Layer Send/Receive
   virtual ProcessResult PresentationSend(double aSimTime, size_t aLayerIndex, Message& aMessage)
   {
      return std::make_pair(false, true);
   }
   virtual ProcessResult PresentationReceive(double aSimTime, size_t aLayerIndex, Message& aMessage)
   {
      return std::make_pair(false, true);
   }
   //@}
   //! @name Session Layer Send/Receive
   virtual ProcessResult SessionSend(double aSimTime, size_t aLayerIndex, Message& aMessage)
   {
      return std::make_pair(false, true);
   }
   virtual ProcessResult SessionReceive(double aSimTime, size_t aLayerIndex, Message& aMessage)
   {
      return std::make_pair(false, true);
   }
   //@}
   //! @name Transport Layer Send/Receive
   virtual ProcessResult TransportSend(double aSimTime, size_t aLayerIndex, Message& aMessage)
   {
      return std::make_pair(false, true);
   }
   virtual ProcessResult TransportReceive(double aSimTime, size_t aLayerIndex, Message& aMessage)
   {
      return std::make_pair(false, true);
   }
   //@}
   //! @name Network Layer Send/Receive
   virtual ProcessResult NetworkSend(double aSimTime, size_t aLayerIndex, Message& aMessage)
   {
      return std::make_pair(false, true);
   }
   virtual ProcessResult NetworkReceive(double aSimTime, size_t aLayerIndex, Message& aMessage)
   {
      return std::make_pair(false, true);
   }
   //@}
   //! @name Datalink Layer Send/Receive
   virtual ProcessResult DatalinkSend(double aSimTime, size_t aLayerIndex, Message& aMessage)
   {
      return std::make_pair(false, true);
   }
   virtual ProcessResult DatalinkReceive(double aSimTime, size_t aLayerIndex, Message& aMessage)
   {
      return std::make_pair(false, true);
   }
   //@}
   //! @name Physical Layer Send/Receive
   virtual ProcessResult PhysicalSend(double aSimTime, size_t aLayerIndex, Message& aMessage)
   {
      return std::make_pair(false, true);
   }
   virtual ProcessResult PhysicalReceive(double aSimTime, size_t aLayerIndex, Message& aMessage)
   {
      return std::make_pair(false, true);
   }
   //@}
   //@}
};

//! Base interface script class.
class WSF_EXPORT ScriptProtocolClass : public UtScriptClass
{
public:
   ScriptProtocolClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   ~ScriptProtocolClass() override = default;
};

} // namespace comm
} // namespace wsf

WSF_DECLARE_COMPONENT_ROLE_TYPE(wsf::comm::ProtocolInterface, cWSF_COMPONENT_COMM_PROTOCOL);

#endif
