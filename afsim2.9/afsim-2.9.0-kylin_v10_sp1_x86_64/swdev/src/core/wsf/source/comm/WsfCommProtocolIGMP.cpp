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

#include "WsfCommProtocolIGMP.hpp"

#include "UtLog.hpp"
#include "UtStringIdLiteral.hpp"
#include "WsfAttributeContainer.hpp"
#include "WsfComm.hpp"
#include "WsfCommMessage.hpp"
#include "WsfCommNetworkManager.hpp"
#include "WsfCommProtocolPriority.hpp"
#include "WsfCommRouter.hpp"
#include "WsfCommRoutingAlgorithmLibrary.hpp"
#include "WsfSimulation.hpp"
#include "script/WsfScriptContext.hpp"

namespace wsf
{
namespace comm
{

static const Address& cMulticastRange{"224.0.0.0", 4u};

// ============================================================================
ProtocolIGMP* ProtocolIGMP::Find(const Comm& aParent) // static
{
   auto protocolPtr = aParent.GetComponents().GetComponent<ProtocolIGMP>();
   return protocolPtr;
}

// ============================================================================
ProtocolIGMP* ProtocolIGMP::FindOrCreate(Comm& aParent) // static
{
   auto protocolPtr = Find(aParent);
   if (!protocolPtr)
   {
      protocolPtr = new ProtocolIGMP;
      aParent.GetComponents().AddComponent(protocolPtr);
   }

   return protocolPtr;
}

// ============================================================================
WsfComponent* ProtocolIGMP::CloneComponent() const
{
   return new ProtocolIGMP(*this);
}

// ============================================================================
const int* ProtocolIGMP::GetComponentRoles() const
{
   static const int roles[] = {cWSF_COMPONENT_COMM_PROTOCOL_IGMP,
                               cWSF_COMPONENT_COMM_PROTOCOL,
                               cWSF_COMPONENT_COMM,
                               cWSF_COMPONENT_NULL};

   return roles;
}

// ============================================================================
ProtocolIGMP* ProtocolIGMP::Clone() const
{
   return new ProtocolIGMP(*this);
}

// ============================================================================
void* ProtocolIGMP::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_COMM_PROTOCOL_IGMP)
   {
      return this;
   }
   if (aRole == cWSF_COMPONENT_COMM_PROTOCOL)
   {
      return dynamic_cast<ProtocolInterface*>(this);
   }
   if (aRole == cWSF_COMPONENT_COMM)
   {
      return dynamic_cast<Component*>(this);
   }

   return nullptr;
}

// ============================================================================
ProtocolPriority ProtocolIGMP::GetPriority() const
{
   return ProtocolPriority::cIGMP;
}

// ============================================================================
bool ProtocolIGMP::Initialize(double aSimTime)
{
   bool ok = true;

   auto&  componentList = GetComm()->GetComponents();
   size_t protocolCount = 0;
   for (auto component : componentList)
   {
      if (component->ComponentHasRole(cWSF_COMPONENT_COMM_PROTOCOL_IGMP))
      {
         ++protocolCount;
         if (protocolCount > 1)
         {
            ut::log::warning() << "Only a single IGMP protocol allowed per comm.";
            return false;
         }
      }
   }

   return ok;
}

// ============================================================================
void ProtocolIGMP::Setup()
{
   auto networkManagerPtr = GetComm()->GetSimulation()->GetCommNetworkManager();
   for (auto& multicastGroupAddress : mJoinGroups)
   {
      networkManagerPtr->AddMulticastMember(0.0, multicastGroupAddress, GetComm()->GetAddress());
   }
}

// ============================================================================
bool ProtocolIGMP::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();

   if (command == "join_multicast_group")
   {
      Address group = Address::ProcessInput(aInput);
      JoinGroup(group);
   }
   else if (command == "leave_multicast_group")
   {
      Address group = Address::ProcessInput(aInput);
      LeaveGroup(group);
   }
   else if (command == "join_delay")
   {
      WsfRandomVariable interval;
      interval.ProcessInput(aInput, UtInput::cTIME);
      mJoinDelay = interval;
   }
   else if (command == "leave_delay")
   {
      WsfRandomVariable interval;
      interval.ProcessInput(aInput, UtInput::cTIME);
      mLeaveDelay = interval;
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

// ============================================================================
void ProtocolIGMP::LeaveGroup(const Address& aAddress)
{
   auto it = std::find(std::begin(mJoinGroups), std::end(mJoinGroups), aAddress);
   if (it != std::end(mJoinGroups))
   {
      mJoinGroups.erase(it);
   }
}

// ============================================================================
ProtocolIGMP::ProcessResult ProtocolIGMP::NetworkSend(double aSimTime, size_t aLayerIndex, Message& aMessage)
{
   //! Determine if this is a multicast address destination.
   const Address& destinationAddress = aMessage.SourceMessage()->GetDstAddr();

   if (Address::CheckInAddressRange(cMulticastRange, destinationAddress))
   {
      //! If this comm device does has level 0 support, we can't send it.
      if (GetComm()->GetMulticastLevel() == Comm::MulticastConformanceLevel::cLEVEL_0)
      {
         //! Set the message destination to null, to avoid any further
         //! handling of the message.
         aMessage.SourceMessage()->SetDstAddr(Address());

         //! Halt processing in the layer, don't send to next layer.
         return std::make_pair(true, false);
      }
      else
      {
         //! Halt protocol processing, continue to other layers
         return std::make_pair(true, true);
      }
   }

   //! Don't stop protocol processing, continue to other layers.
   return std::make_pair(false, true);
}

// ============================================================================
ProtocolIGMP::ProcessResult ProtocolIGMP::NetworkReceive(double aSimTime, size_t aLayerIndex, Message& aMessage)
{
   const Address& destinationAddress = aMessage.SourceMessage()->GetDstAddr();
   // This protocol needs to inspect and process IGMP messages for
   // correct protocol functionality. Note that messages to join/leave groups
   // made to the querier are using standard addressing.

   //! If we're the querier, handle the messages.
   if (GetQuerier() == GetComm()->GetAddress())
   {
      auto requestAddress = GetMessageAddressFromAux(aMessage);
      if (!requestAddress.IsNull())
      {
         if (aMessage.SourceMessage()->GetType() == UtStringIdLiteral("IGMP_JOIN_MESSAGE"))
         {
            auto                                    delay = mJoinDelay.Draw();
            std::unique_ptr<IGMP_SubscriptionEvent> eventPtr =
               ut::make_unique<IGMP_SubscriptionEvent>(aSimTime + delay,
                                                       IGMP_SubscriptionEvent::EventType::cJOIN,
                                                       aMessage.SourceMessage()->GetSrcAddr(),
                                                       requestAddress);

            GetComm()->GetSimulation()->AddEvent(std::move(eventPtr));
         }
         else if (aMessage.SourceMessage()->GetType() == UtStringIdLiteral("IGMP_LEAVE_MESSAGE"))
         {
            auto                                    delay = mLeaveDelay.Draw();
            std::unique_ptr<IGMP_SubscriptionEvent> eventPtr =
               ut::make_unique<IGMP_SubscriptionEvent>(aSimTime + delay,
                                                       IGMP_SubscriptionEvent::EventType::cLEAVE,
                                                       aMessage.SourceMessage()->GetSrcAddr(),
                                                       requestAddress);

            GetComm()->GetSimulation()->AddEvent(std::move(eventPtr));
         }
         else
         {
            throw UtException("Corrupt IGMP message received.");
         }

         // This was an IGMP protocol message. It belongs to this interface, and no other
         // protocols should handle this message.
         return std::make_pair(true, true);
      }
   }

   // If this is a multicast message, the physical layer passed along the message
   // because it is intended for this interface, or needs forwarding. Check
   // for these cases to determine the return for correct network layer processing.
   if (Address::CheckInAddressRange(cMulticastRange, destinationAddress))
   {
      for (auto& groupAddress : mJoinGroups)
      {
         if (groupAddress == destinationAddress)
         {
            // We're in the group this message is addressed to.
            // Indicate our message, but not to stop processing
            // as other protocols may want to inspect the message
            return std::make_pair(false, true);
         }
         else
         {
            // Not our message.
            return std::make_pair(false, false);
         }
      }
   }

   // Default return, not our message, don't halt.
   return std::make_pair(false, false);
}

// ============================================================================
ProtocolIGMP::ProcessResult ProtocolIGMP::PhysicalReceive(double aSimTime, size_t aLayerIndex, Message& aMessage)
{
   //! With IGMP, multicast messages not valid for the owning comm are ignored at the
   //! physical layer. We listen for those we want, and ignore the rest. If
   //! the owning comm is doing routing, then we also must check for
   //! dependent hosts who may want this message as well.
   const Address& destinationAddress = aMessage.SourceMessage()->GetDstAddr();
   if (Address::CheckInAddressRange(cMulticastRange, destinationAddress))
   {
      //! This is a multicast address.
      //! Check to see if we can receive multicast messages. If not, short circuit.
      if (GetComm()->GetMulticastLevel() != Comm::MulticastConformanceLevel::cLEVEL_2)
      {
         //! Halt processing in the layer, don't send to next layer.
         return std::make_pair(true, false);
      }

      //! Determine if the multicast address is a group
      //! we are interested in.
      for (auto& groupAddress : mJoinGroups)
      {
         if (groupAddress == destinationAddress)
         {
            return std::make_pair(true, true);
         }
      }

      //! If we made it to this point, the multicast message isn't
      //! destined for us. However, if we are a router, we must
      //! check if its destined for someone in our network.
      //! Note: This is done on the physical layer, as this action is taken via
      //! OS level controls and port assignments.
      if (GetComm()->GetRouter()->IsActive())
      {
         auto networkManagerPtr = GetComm()->GetSimulation()->GetCommNetworkManager();
         if (networkManagerPtr->HasMulticastMember(destinationAddress, GetComm()->GetNetwork()))
         {
            //! Note: We just pass along the message now. The actual addresses will
            //! be resolved in the network layer, and retransmitted there.
            return std::make_pair(true, true);
         }
      }
   }

   return std::make_pair(false, true);
}

// ============================================================================
Address ProtocolIGMP::GetQuerier() const
{
   auto    networkManagerPtr = GetComm()->GetSimulation()->GetCommNetworkManager();
   auto    members           = networkManagerPtr->GetAddressesInNetwork(GetComm()->GetNetwork());
   Address curAddress        = Address();

   auto it = std::min_element(std::begin(members),
                              std::end(members),
                              [](const Address& aLhs, const Address& aRhs)
                              { return aLhs.GetBinaryInteger() < aRhs.GetBinaryInteger(); });

   if (it != std::end(members))
   {
      curAddress = *it;
   }

   return curAddress;
}

// ============================================================================
void ProtocolIGMP::JoinGroup(double aSimTime, const Address& aAddress)
{
   auto querierAddress    = GetQuerier();
   auto networkManagerPtr = GetComm()->GetSimulation()->GetCommNetworkManager();

   if (!querierAddress.IsNull())
   {
      if (querierAddress == GetComm()->GetAddress())
      {
         networkManagerPtr->AddMulticastMember(aSimTime, aAddress, GetComm()->GetAddress());
      }
      else
      {
         auto messagePtr = ut::make_unique<WsfMessage>(UtStringIdLiteral("IGMP_JOIN_MESSAGE"));
         messagePtr->SetSimulation(GetComm()->GetSimulation());
         messagePtr->SetSerialNumber(GetComm()->GetSimulation()->NextMessageSerialNumber());

         WsfAttributeContainer& aux = messagePtr->GetAuxData();
         aux.AddString("IGMP_ADDRESS", aAddress.GetAddress());
         aux.AddInt("IGMP_ADDRESS_CIDR", static_cast<int>(aAddress.GetSubnetMaskBitLength()));

         GetComm()->Send(aSimTime, std::move(messagePtr), querierAddress);
      }

      mJoinGroups.push_back(aAddress);
   }
}

// ============================================================================
void ProtocolIGMP::LeaveGroup(double aSimTime, const Address& aAddress)
{
   auto querierAddress    = GetQuerier();
   auto networkManagerPtr = GetComm()->GetSimulation()->GetCommNetworkManager();

   if (!querierAddress.IsNull())
   {
      if (querierAddress == GetComm()->GetAddress())
      {
         networkManagerPtr->AddMulticastMember(aSimTime, aAddress, GetComm()->GetAddress());
      }
      else
      {
         auto messagePtr = ut::make_unique<WsfMessage>(UtStringIdLiteral("IGMP_LEAVE_MESSAGE"));
         messagePtr->SetSimulation(GetComm()->GetSimulation());
         messagePtr->SetSerialNumber(GetComm()->GetSimulation()->NextMessageSerialNumber());

         WsfAttributeContainer& aux = messagePtr->GetAuxData();
         aux.AddString("IGMP_ADDRESS", aAddress.GetAddress());
         aux.AddInt("IGMP_ADDRESS_CIDR", static_cast<int>(aAddress.GetSubnetMaskBitLength()));

         GetComm()->Send(aSimTime, std::move(messagePtr), querierAddress);
      }

      auto it = std::find(std::begin(mJoinGroups), std::end(mJoinGroups), aAddress);
      if (it != std::end(mJoinGroups))
      {
         mJoinGroups.erase(it);
      }
   }
}

// ============================================================================
Address ProtocolIGMP::GetMessageAddressFromAux(Message& aMessage)
{
   WsfStringId messageType = aMessage.SourceMessage()->GetType();
   if (messageType == UtStringIdLiteral("IGMP_JOIN_MESSAGE") || messageType == UtStringIdLiteral("IGMP_LEAVE_MESSAGE"))
   {
      const auto& aux = aMessage.SourceMessage()->GetAuxDataConst();
      std::string addressString;
      int         cidrValue;
      aux.Get("IGMP_ADDRESS", addressString);
      aux.Get("IGMP_ADDRESS_CIDR", cidrValue);

      return Address{addressString, static_cast<size_t>(cidrValue)};
   }

   return Address{};
}

// ============================================================================
IGMP_SubscriptionEvent::IGMP_SubscriptionEvent(double         aSimTime,
                                               EventType      aEventType,
                                               const Address& aCommAddress,
                                               const Address& aMulticastGroupAddress)
   : WsfEvent(aSimTime)
   , mEventType(aEventType)
   , mCommAddress(aCommAddress)
   , mMulticastAddress(aMulticastGroupAddress)
{
}

// ============================================================================
IGMP_SubscriptionEvent::EventDisposition IGMP_SubscriptionEvent::Execute()
{
   EventDisposition eventDisposition  = cDELETE;
   auto             networkManagerPtr = GetSimulation()->GetCommNetworkManager();

   if (mEventType == EventType::cJOIN)
   {
      networkManagerPtr->AddMulticastMember(GetTime(), mMulticastAddress, mCommAddress);
   }
   else if (mEventType == EventType::cLEAVE)
   {
      networkManagerPtr->RemoveMulticastMember(GetTime(), mMulticastAddress, mCommAddress);
   }

   return eventDisposition;
}

// ============================================================================
ScriptProtocolClassIGMP::ScriptProtocolClassIGMP(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : ScriptProtocolClass(aClassName, aTypesPtr)
{
   SetClassName("WsfCommProtocolIGMP");
   mConstructible      = false;
   mCloneable          = false;
   mIsScriptAccessible = true;

   AddMethod(ut::make_unique<JoinMulticastGroup>());
   AddMethod(ut::make_unique<LeaveMulticastGroup>());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptProtocolClassIGMP, ProtocolIGMP, JoinMulticastGroup, 1, "void", "WsfAddress")
{
   auto addressPtr = aVarArgs[0].GetPointer()->GetAppObject<Address>();
   aObjectPtr->JoinGroup(WsfScriptContext::GetTIME_NOW(aContext), *addressPtr);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptProtocolClassIGMP, ProtocolIGMP, LeaveMulticastGroup, 1, "void", "WsfAddress")
{
   auto addressPtr = aVarArgs[0].GetPointer()->GetAppObject<Address>();
   aObjectPtr->LeaveGroup(WsfScriptContext::GetTIME_NOW(aContext), *addressPtr);
}


} // namespace comm
} // namespace wsf
