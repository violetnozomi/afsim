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

#include "WsfComm.hpp"

#include <algorithm>
#include <string>

#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "WsfCommMediumTypeIdentifier.hpp"
#include "WsfCommMediumTypes.hpp"
#include "WsfCommNetworkLayer.hpp"
#include "WsfCommNetworkManager.hpp"
#include "WsfCommObserver.hpp"
#include "WsfCommProtocolInterface.hpp"
#include "WsfCommRouter.hpp"
#include "WsfComponentFactoryList.hpp"
#include "WsfComponentListDefines.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"

namespace wsf
{
namespace comm
{

// ============================================================================
Comm::Comm(const WsfScenario& aScenario, CommType aCommType)
   : WsfArticulatedPart(aScenario, cCOMPONENT_ROLE<Comm>())
   , mProtocolStack()
   , mInputAddress()
   , mInputNetwork()
   , mInputNetworkAddress()
   , mNumChannels(1U)
   , mCommTrueType(aCommType)
   , mComponents()
   , mAddress()
   , mNetwork()
   , mClassId()
   , mLinkAddresses()
   , mLinks()
   , mRouterName("default")
   , mRouterPtr(nullptr)
   , mGateway()
   , mMulticastLevel(MulticastConformanceLevel::cLEVEL_0)
{
   // Set the initial capability of the device
   mReceiveEnabled  = (mCommTrueType & cCT_RCV_ONLY) > 0;
   mTransmitEnabled = (mCommTrueType & cCT_XMT_ONLY) > 0;

   // All comm devices are initially turned on
   // but may not be sending or receiving
   WsfPlatformPart::SetInitiallyTurnedOn(true);

   // Set parent of comm-specific components
   mComponents.SetParentOfComponents(this);

   // Set parent of protocol stack
   mProtocolStack.SetParent(this);

   // Add the medium container component and configure.
   auto containerPtr = medium::ContainerComponent<Comm>::FindOrCreate(*this, aScenario);
   if (!containerPtr)
   {
      throw UtException("Failed addition of medium container on WsfComm construction.");
   }

   containerPtr->AddValidMedium(medium::cGUIDED);
   containerPtr->SetDefaultMedium(medium::cGUIDED);
}

// ============================================================================
//! Copy constructor (for Clone())
// protected
Comm::Comm(const Comm& aSrc)
   : WsfArticulatedPart(aSrc)
   , mProtocolStack(aSrc.mProtocolStack)
   , mInputAddress(aSrc.mInputAddress)
   , mInputNetwork(aSrc.mInputNetwork)
   , mInputNetworkAddress(aSrc.mInputNetworkAddress)
   , mNumChannels(aSrc.mNumChannels)
   , mCommTrueType(aSrc.mCommTrueType)
   , mReceiveEnabled(aSrc.mReceiveEnabled)
   , mTransmitEnabled(aSrc.mTransmitEnabled)
   , mComponents(aSrc.mComponents)
   , mAddress(aSrc.mAddress)
   , mNetwork(aSrc.mNetwork)
   , mClassId(aSrc.mClassId)
   , mLinkAddresses(aSrc.mLinkAddresses)
   , mLinks(aSrc.mLinks)
   , mRouterName(aSrc.mRouterName)
   , mRouterPtr(aSrc.mRouterPtr)
   , mGateway(aSrc.mGateway)
   , mMulticastLevel(aSrc.mMulticastLevel)
{
   // Set parent of comm-specific components
   mComponents.SetParentOfComponents(this);

   // Set parent of protocol stack
   mProtocolStack.SetParent(this);
}

// ============================================================================
const int* Comm::GetComponentRoles() const
{
   static const int roles[] = {cWSF_COMPONENT_COMM,
                               cWSF_COMPONENT_ARTICULATED_PART,
                               cWSF_COMPONENT_PLATFORM_PART,
                               cWSF_COMPONENT_NULL};
   return roles;
}

// ============================================================================
void* Comm::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_COMM)
   {
      return this;
   }
   if (aRole == cWSF_COMPONENT_ARTICULATED_PART)
   {
      return dynamic_cast<WsfArticulatedPart*>(this);
   }
   if (aRole == cWSF_COMPONENT_PLATFORM_PART)
   {
      return dynamic_cast<WsfPlatformPart*>(this);
   }
   return nullptr;
}

// ============================================================================
//! Standard Clone() method.
// virtual
Comm* Comm::Clone() const
{
   return new Comm(*this);
}

// ================================================================================================
// virtual
bool Comm::PreInitialize(double aSimTime)
{
   bool ok = WsfArticulatedPart::PreInitialize(aSimTime);

   // Component initialization
   WsfComponentFactoryList<Comm>::PreInitialize(GetScenario(), aSimTime, *this);
   if (mComponents.HasComponents())
   {
      for (auto component : mComponents)
      {
         ok &= component->PreInitialize(aSimTime);
      }
   }
   return ok;
}

// ============================================================================
// virtual
bool Comm::Initialize(double aSimTime)
{
   // Initialize the base class first
   bool ok = WsfArticulatedPart::Initialize(aSimTime);

   // Set medium container simulation reference.
   medium::ContainerComponent<Comm>::Find(*this)->SetSimulation(GetSimulation());

   // Initialize the comm-specific components
   for (auto component : mComponents)
   {
      ok &= component->Initialize(aSimTime);
   }

   // Initialize the protocol stack
   ok &= mProtocolStack.Initialize(aSimTime);

   // Set the debug state
   mProtocolStack.SetDebugEnabled(DebugEnabled());

   //! Add this comm to be managed by the network manager
   GetSimulation()->GetCommNetworkManager()->AddManagedComm(this);

   return ok;
}

// ============================================================================
// virtual
bool Comm::Initialize2(double aSimTime)
{
   // Initialize the base class first
   bool ok = WsfArticulatedPart::Initialize2(aSimTime);

   // Initialize the comm-specific components
   for (auto component : mComponents)
   {
      ok &= component->Initialize2(aSimTime);
   }

   // Initialize the protocol stack
   ok &= mProtocolStack.Initialize2(aSimTime);

   return ok;
}

// ============================================================================
// See WsfComponent::PreInput about this.
void Comm::PreInput()
{
   WsfComponentFactoryList<Comm>::PreInput(GetScenario(), *this);
}

// ============================================================================
// virtual
bool Comm::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (mProtocolStack.ProcessInput(aInput))
   {
   }
   else if (WsfArticulatedPart::ProcessInput(aInput))
   {
   }
   else if (command == "add")
   {
      aInput.ReadCommand(command);
      if (!WsfComponentFactoryList<Comm>::ProcessAddOrEditCommand(GetScenario(), aInput, *this, true))
      {
         throw UtInput::BadValue(aInput, "add must be followed by a valid registered comm component or medium.");
      }
   }
   else if (command == "edit")
   {
      aInput.ReadCommand(command);
      if (!WsfComponentFactoryList<Comm>::ProcessAddOrEditCommand(GetScenario(), aInput, *this, false))
      {
         throw UtInput::BadValue(aInput, "edit must be followed by a valid registered comm component or medium.");
      }
   }
   else if (command == "delete")
   {
      aInput.ReadCommand(command);
      WsfComponentFactoryList<Comm>::ProcessDeleteCommand(GetScenario(), aInput, *this);
   }
   else if (command == "network_name")
   {
      aInput.ReadValue(mInputNetwork);
   }
   else if (command == "network_address")
   {
      mInputNetworkAddress = Address::ProcessInput(aInput);
   }
   else if (command == "address")
   {
      mInputAddress = Address::ProcessInput(aInput);
   }
   else if (command == "link")
   {
      std::string platformName;
      std::string commName;

      aInput.ReadValue(platformName);
      aInput.ReadValue(commName);

      mLinks.emplace_back(platformName, commName);
   }
   else if (command == "local_link")
   {
      std::string commName;
      aInput.ReadValue(commName);

      mLinks.emplace_back(std::string(), commName);
   }
   else if (command == "link_address")
   {
      mLinkAddresses.push_back(Address::ProcessInput(aInput));
   }
   else if (command == "router_name")
   {
      aInput.ReadValue(mRouterName);
   }
   else if (command == "gateway_address")
   {
      mGateway.mAddress = Address::ProcessInput(aInput);
   }
   else if (command == "gateway")
   {
      aInput.ReadValue(mGateway.mPlatformName);
      aInput.ReadValue(mGateway.mCommName);
   }
   else if (command == "channels")
   {
      unsigned int numChannels = 0U;
      aInput.ReadValue(numChannels);
      mNumChannels = numChannels;
   }
   else
   {
      // Possible component input. This is tried last because it is a more intensive. First try
      // components already attached. If the input was not recognized by one of them then try the
      // component factories, which may result in the dynamic creation of a new component.
      myCommand = false;
      if (mComponents.HasComponents())
      {
         for (auto component : mComponents)
         {
            if (component->ProcessInput(aInput))
            {
               myCommand = true;
               break;
            }
         }
      }
      if (!myCommand)
      {
         myCommand = WsfComponentFactoryList<Comm>::ProcessInput(GetScenario(), aInput, *this);
      }
   }
   return myCommand;
}

// ============================================================================
//! Update the state of the communications system.
//! This method should perform any actions that are necessary so that the system
//! reflects its condition at the supplied time.
//! @param aSimTime The current simulation time.
// virtual
void Comm::Update(double aSimTime) {}

// ============================================================================
//! Status change to Broken.  Terminally, mortally wounded.
//! @param aSimTime The current simulation time.
//! @note  This method should not be invoked directly by application code.
// virtual
void Comm::PartBroken(double aSimTime)
{
   WsfPlatformPart::PartBroken(aSimTime);
   WsfObserver::CommBroken(GetSimulation())(aSimTime, this);
}

// ============================================================================
//! Status change to non-operational.
//! @param aSimTime The current simulation time.
//! @note  This method should not be invoked directly by application code.
// virtual
void Comm::SetNonOperational(double aSimTime)
{
   WsfPlatformPart::SetNonOperational(aSimTime);
   WsfObserver::CommNonOperational(GetSimulation())(aSimTime, this);
}

// ============================================================================
//! Status change to Operational.
//! @param aSimTime The current simulation time.
//! @note  This method should not be invoked directly by application code.
// virtual
void Comm::SetOperational(double aSimTime)
{
   WsfPlatformPart::SetOperational(aSimTime);
   WsfObserver::CommOperational(GetSimulation())(aSimTime, this);
}

// ============================================================================
//! Turn off the communication device.
//! @param aSimTime The current simulation time.
//! @note  This method should not be invoked directly by application code.
//!        Use the WsfSimulation::TurnCommOff method.
// virtual
void Comm::TurnOff(double aSimTime)
{
   WsfPlatformPart::TurnOff(aSimTime);
   for (auto component : GetComponents())
   {
      auto commComponentPtr = dynamic_cast<Component*>(component);
      if (commComponentPtr)
      {
         commComponentPtr->TurnOff(aSimTime);
      }
   }
   WsfObserver::CommTurnedOff(GetSimulation())(aSimTime, this);
}

// ============================================================================
//! Turn on the communication device.
//! @param aSimTime The current simulation time.
//! @note  This method should not be invoked directly by application code.
//!        Use the WsfSimulation::TurnCommOn method.
// virtual
void Comm::TurnOn(double aSimTime)
{
   WsfPlatformPart::TurnOn(aSimTime);
   for (auto component : GetComponents())
   {
      auto commComponentPtr = dynamic_cast<Component*>(component);
      if (commComponentPtr)
      {
         commComponentPtr->TurnOn(aSimTime);
      }
   }
   WsfObserver::CommTurnedOn(GetSimulation())(aSimTime, this);
}

// ============================================================================
//! Receive a message.
//! It is the responsibility of the receiver to actually determine if the message
//! can be received (i.e. line-of-sight, enough power, etc).
//! @param aSimTime  The current simulation time.
//! @param aXmtrPtr  The comm device that sent the message.
//! @param aMessage  The received message.
//! @returns true if the message was received or false if it could not.
// virtual
bool Comm::Receive(double aSimTime, Comm* aXmtrPtr, Message& aMessage)
{
   // Base functionality for core AFSIM communications uses the Protocol Stack.
   // This is not required for user implementations, as the various Send/Receive
   // methods utilized are all virtual. The base comm object defines the typical
   // use case to eliminate repetitive code usage and ease maintenance/debugging.
   if (!ReceiveSetup(aSimTime, aXmtrPtr, aMessage))
   {
      return false;
   }

   // Perform a comm filter check
   if (!wsf::comm::Component::Receive(*this, aSimTime, aXmtrPtr, aMessage))
   {
      return false;
   }

   // Pass the message to the stack to see if it can be processed
   auto messageReceived = mProtocolStack.Receive(aSimTime, aXmtrPtr, aMessage);

   // Take common actions based on success/failure of reception.
   return ReceiveActions(aSimTime, messageReceived, aXmtrPtr, aMessage);
}

// ============================================================================
//! Send a message to a specific platform.
//! @param aSimTime       The current simulation time.
//! @param aMessagePtr    The message to be sent.
//! @param aAddress       The comm address to whom the message is to be sent.
// virtual
bool Comm::Send(double aSimTime, std::unique_ptr<WsfMessage> aMessagePtr, const Address& aAddress)
{
   // This object is only valid in the scope of this method call, and will
   // deallocate upon returning. If any object (layer, event, etc.) in the
   // stack call chain requires an extended lifetime of this object, it is
   // their responsibility to create such an object and manage it.
   Message message(std::move(aMessagePtr));

   // Setup the message properties. Fail if comm is turned off.
   if (!SendSetup(aSimTime, message, aAddress))
   {
      return false;
   }

   // Perform a comm filter check
   if (!wsf::comm::Component::Send(*this, aSimTime, *message.SourceMessage(), aAddress))
   {
      return false;
   }

   // Pass the message to the stack to see if it can be processed
   auto messageSent = mProtocolStack.Send(aSimTime, message);

   // Potential derivation functionality.
   return SendActions(aSimTime, messageSent, message);
}

// ============================================================================
// deprecated.
bool Comm::Send(double aSimTime, const WsfMessage& aMessage, const Address& aAddress)
{
   return Send(aSimTime, ut::clone(aMessage), aAddress);
}

// ============================================================================
//! Process a layer event.
// virtual
bool Comm::ProcessLayerMessage(double aSimTime, size_t aLayerIndex, layer::Message aLayerMessage, Message* aCommMessagePtr)
{
   return mProtocolStack.ProcessLayerMessage(aSimTime, aLayerIndex, aLayerMessage, aCommMessagePtr);
}

// ============================================================================
//! A convenience function that appends the comm device's name to the platform's name.
//! @return "<platform_name>.<comm_name>",
// virtual
std::string Comm::GetFullName() const
{
   //! Need to check platform pointer, as this may be a nullptr when instantiated as a type.
   if (GetPlatform())
   {
      return GetPlatform()->GetName() + '.' + GetName();
   }
   else
   {
      return ("NULL." + GetName());
   }
}

// ============================================================================
bool Comm::CanSendTo(double aSimTime, Comm* aRcvrPtr, Message* aMessagePtr /* = nullptr*/)
{
   // Note: We don't check for the comm device being on in this call.
   // This call may be used prior to simulation start, and the comm
   // object may not yet be on. An additional check to comm status being
   // on should be made in addition to this call during runtime usage.

   // Check initially for a bridged connection (via router connection).
   // This allows various conditions that would normally forbid communications.
   // For instance, EM interactions are not required to pass
   // (due to modeling a software or hardware comm bridge on the same platform),
   // and comms devices that would typically only transmit
   // could potentially receive and pass on such messages.
   auto routerPtr = GetRouter();
   if (routerPtr && routerPtr->HasInterface(aRcvrPtr))
   {
      return true;
   }

   if (!aRcvrPtr->CanReceive())
   {
      return false;
   }

   if (GetClassId() != aRcvrPtr->GetClassId())
   {
      return false;
   }

   return true;
}

// ============================================================================
std::vector<ProtocolInterface*> Comm::GetSortedProtocols() const
{
   std::vector<ProtocolInterface*> protocols;
   for (WsfComponentList::RoleIterator<ProtocolInterface> iter{GetComponents()}; !iter.AtEnd(); ++iter)
   {
      protocols.push_back(*iter);
   }

   std::sort(std::begin(protocols),
             std::end(protocols),
             [](ProtocolInterface* lhs, ProtocolInterface* rhs) { return lhs->GetPriority() < rhs->GetPriority(); });

   return protocols;
}

// ============================================================================
void Comm::Setup()
{
   if (!ValidateGateway())
   {
      mGateway.mAddress      = Address();
      mGateway.mPlatformName = mGateway.mCommName = std::string();

      auto out = ut::log::warning() << "Invalid gateway was set for comm.";
      out.AddNote() << "Comm: " << GetFullName();
      out.AddNote() << "Ensure the name and address are correct.";
   }

   //! Set the multicast conformance level
   SetMulticastLevel();

   for (auto component : GetComponents())
   {
      auto commComponentPtr = dynamic_cast<Component*>(component);
      if (commComponentPtr)
      {
         commComponentPtr->Setup();
      }
   }
}

// ============================================================================
void Comm::PendingStart()
{
   for (auto component : GetComponents())
   {
      auto commComponentPtr = dynamic_cast<Component*>(component);
      if (commComponentPtr)
      {
         commComponentPtr->PendingStart();
      }
   }
}

// ============================================================================
void Comm::SetGateway(const Address& aAddress)
{
   Gateway prevGateway = mGateway;
   mGateway.mAddress   = aAddress;
   mGateway.mCommName = mGateway.mPlatformName = std::string();

   if (!ValidateGateway())
   {
      auto out = ut::log::warning() << "Cannot change the gateway address.";
      out.AddNote() << "Comm: " << GetFullName();
      out.AddNote() << "Gateway Address: " << aAddress;
      out.AddNote() << "The gateway has not changed. Ensure the provided gateway is a valid interface.";
   }
}

// ============================================================================
bool Comm::ValidateGateway()
{
   Comm* addressCommPtr = nullptr;
   Comm* nameCommPtr    = nullptr;

   if (!mGateway.mAddress.IsNull())
   {
      addressCommPtr = GetSimulation()->GetCommNetworkManager()->GetComm(mGateway.mAddress);
   }

   if ((!mGateway.mPlatformName.empty()) && (!mGateway.mCommName.empty()))
   {
      auto platformPtr = GetSimulation()->GetPlatformByName(mGateway.mPlatformName);
      if (platformPtr)
      {
         nameCommPtr = platformPtr->GetComponent<Comm>(mGateway.mCommName);
      }
   }

   // If both the address and name was set, ensure they are the same interface.
   if ((addressCommPtr && nameCommPtr) && (!(addressCommPtr == nameCommPtr)))
   {
      return false;
   }

   // Set both the name and address as only one may have been provided.
   if (addressCommPtr && !nameCommPtr)
   {
      mGateway.mPlatformName = addressCommPtr->GetPlatform()->GetName();
      mGateway.mCommName     = addressCommPtr->GetName();
   }
   else if (!addressCommPtr && nameCommPtr)
   {
      mGateway.mAddress = nameCommPtr->GetAddress();
   }
   return true;
}

// ============================================================================
void Comm::SetRouter(router::Router* aRouterPtr)
{
   mRouterPtr = aRouterPtr;
   SetMulticastLevel();
}

// ============================================================================
void Comm::SetMulticastLevel()
{
   // For baseline comm models used in core, the two protocols of concern
   // are WsfCommProtocolIGMP and WsfCommProtocolMulticast. The former is
   // located on the comm itself, while the latter is associated with a
   // router. The conformance is determined by the availability of these
   // protocols or protocols of the same component type.
   bool hasIGMP      = GetComponents().GetComponentEntryByRole(cWSF_COMPONENT_COMM_PROTOCOL_IGMP, 0);
   bool hasMulticast = false;

   if (mRouterPtr)
   {
      hasMulticast = mRouterPtr->GetComponents().GetComponentEntryByRole(cWSF_COMPONENT_COMM_ROUTER_PROTOCOL_MULTICAST, 0);
   }

   if (hasIGMP && hasMulticast)
   {
      mMulticastLevel = MulticastConformanceLevel::cLEVEL_2;
   }
   else if (hasMulticast)
   {
      mMulticastLevel = MulticastConformanceLevel::cLEVEL_1;
   }
   else
   {
      mMulticastLevel = MulticastConformanceLevel::cLEVEL_0;
   }
}

// ============================================================================
bool Comm::SendSetup(double aSimTime, Message& aMessage, const Address& aAddress)
{
   // Don't send if the device isn't turned on
   if (!IsTurnedOn())
   {
      return false;
   }

   aMessage.SourceMessage()->SetDstAddr(aAddress);
   aMessage.SourceMessage()->SetSrcAddr(GetAddress());

   // Use properties from the message table
   GetScenario().GetMessageTable()->SetMessageProp(GetTypeId(), *aMessage.SourceMessage());

   return true;
}

// ============================================================================
bool Comm::ReceiveSetup(double aSimTime, Comm* aXmtrPtr, Message& aMessage)
{
   // Short-circuit if comm is off.
   if (!IsTurnedOn())
   {
      return false;
   }

   if (DebugEnabled())
   {
      PrintDebugReception(aSimTime, aMessage);
   }

   return true;
}

// ============================================================================
bool Comm::SendActions(double aSimTime, bool aSendStatus, Message& aMessage)
{
   return aSendStatus;
}

// ============================================================================
bool Comm::ReceiveActions(double aSimTime, bool aReceiveStatus, Comm* aXmtrPtr, Message& aMessage)
{
   if (aReceiveStatus)
   {
      // Notify the observers that a message was received.
      WsfObserver::MessageReceived(
         GetSimulation())(aSimTime, aXmtrPtr, this, *aMessage.SourceMessage(), aMessage.GetResult());

      // Forward the message to each of my internal links.
      SendMessage(aSimTime, *aMessage.SourceMessage());
   }
   else if (aMessage.SourceMessage()->GetDstAddr() == GetAddress())
   {
      // Failure to receive results in message being discarded if intended for this comm.
      WsfObserver::MessageDiscarded(
         GetSimulation())(aSimTime, aXmtrPtr, *aMessage.SourceMessage(), "Message Receive Failure");
   }

   return aReceiveStatus;
}

// ============================================================================
void Comm::PrintDebugReception(double aSimTime, const Message& aMessage) const
{
   auto out = ut::log::debug() << "Comm receiving message.";
   out.AddNote() << "T = " << aSimTime;
   out.AddNote() << "Comm: " << GetFullName();
   out.AddNote() << "Message: " << *aMessage.SourceMessage();
}

} // namespace comm
} // namespace wsf
