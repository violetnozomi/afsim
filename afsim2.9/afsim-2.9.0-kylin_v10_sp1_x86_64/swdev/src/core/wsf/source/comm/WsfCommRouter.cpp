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

#include "WsfCommRouter.hpp"

#include "UtException.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "WsfComm.hpp"
#include "WsfCommDatalinkLayer.hpp"
#include "WsfCommMediumTypeIdentifier.hpp"
#include "WsfCommMessage.hpp"
#include "WsfCommNetwork.hpp"
#include "WsfCommNetworkLayer.hpp"
#include "WsfCommNetworkManager.hpp"
#include "WsfCommObserver.hpp"
#include "WsfCommProtocolLegacy.hpp"
#include "WsfCommProtocolMulticast.hpp"
#include "WsfCommRouterProtocolInterface.hpp"
#include "WsfCommTransportLayer.hpp"
#include "WsfComponentFactoryList.hpp"
#include "WsfMessage.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"

namespace wsf
{
namespace comm
{
namespace router
{

// =================================================================================================
Router::Router(const WsfScenario& aScenario)
   : WsfPlatformPart(aScenario, cCOMPONENT_ROLE<Router>())
   , mNetworkManager(nullptr)
   , mGateway()
   , mHopLimit(64)
   , // default 64 hop limit (TCP default standard)
   mComponents()
   , mScenario(aScenario)
   , mCallbacks()
   , mComms()
   , mAutomatedInterfaceLinking(false)
   , mUseLegacyProtocol(true)
   , mUseMulticastProtocol(true)
{
   mComponents.SetParentOfComponents(this);
   SetInitiallyTurnedOn(true);

   // Add the medium container component and configure
   auto containerPtr = medium::ContainerComponent<Router>::FindOrCreate(*this, aScenario);
   if (!containerPtr)
   {
      throw UtException("Failed addition of medium container on router construction.");
   }
   else
   {
      containerPtr->AddValidMedium(medium::cGUIDED);
      containerPtr->AddValidMedium(medium::cUNGUIDED);
      containerPtr->SetDefaultMedium(medium::cGUIDED);
   }
}

// =================================================================================================
Router::Router(const Router& aSrc)
   : WsfPlatformPart(aSrc)
   , mNetworkManager(aSrc.mNetworkManager)
   , mGateway(aSrc.mGateway)
   , mHopLimit(aSrc.mHopLimit)
   , mComponents(aSrc.mComponents)
   , mScenario(aSrc.mScenario)
   , mCallbacks()
   , mComms(aSrc.mComms)
   , mAutomatedInterfaceLinking(aSrc.mAutomatedInterfaceLinking)
   , mUseLegacyProtocol(aSrc.mUseLegacyProtocol)
   , mUseMulticastProtocol(aSrc.mUseMulticastProtocol)
{
   mComponents.SetParentOfComponents(this);
}

// =================================================================================================
Router* Router::Clone() const
{
   return new Router(*this);
}

// =================================================================================================
const int* Router::GetComponentRoles() const
{
   static const int roles[] = {cWSF_COMPONENT_COMM_ROUTER, cWSF_COMPONENT_PLATFORM_PART, cWSF_COMPONENT_NULL};
   return roles;
}

// =================================================================================================
void* Router::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_COMM_ROUTER)
   {
      return this;
   }

   if (aRole == cWSF_COMPONENT_PLATFORM_PART)
   {
      return static_cast<WsfPlatformPart*>(this);
   }

   return nullptr;
}

// =================================================================================================
bool Router::PreInitialize(double aSimTime)
{
   bool ok = WsfPlatformPart::PreInitialize(aSimTime);
   WsfComponentFactoryList<Router>::PreInitialize(mScenario, aSimTime, *this);
   if (mComponents.HasComponents())
   {
      for (auto component : mComponents)
      {
         ok &= component->PreInitialize(aSimTime);
      }
   }
   return ok;
}

// =================================================================================================
bool Router::Initialize(double aSimTime)
{
   bool ok = WsfPlatformPart::Initialize(aSimTime);

   mNetworkManager = GetSimulation()->GetCommNetworkManager();
   if (!mNetworkManager)
   {
      auto out = ut::log::warning() << "Missing network manager.";
      out.AddNote() << "Router: " << GetName();
      return false;
   }

   // Set medium container simulation reference.
   medium::ContainerComponent<Router>::Find(*this)->SetSimulation(GetSimulation());

   // Add the default legacy protocol component
   if (mUseLegacyProtocol)
   {
      ProtocolLegacy::FindOrCreate(*this);
   }

   // Add the default multicasting protocol
   if (mUseMulticastProtocol)
   {
      ProtocolMulticast::FindOrCreate(*this);
   }

   for (auto component : mComponents)
   {
      ok &= component->Initialize(aSimTime);
   }

   // Add this router to the network manager
   GetSimulation()->GetCommNetworkManager()->AddManagedRouter(this);

   return ok;
}

// =================================================================================================
void Router::Setup()
{
   // Verify the gateway is an available interface for the router.
   if (!ValidateGateway())
   {
      mGateway.mAddress         = Address();
      mGateway.mGatewayCommName = std::string();

      auto out = ut::log::warning() << "Invalid gateway set for platform.";
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Router: " << GetName();
      out.AddNote()
         << "Ensure the name or address is correct, and the specified gateway is an interface assigned to the router.";
   }

   // Ensure the router is turned on prior to other objects to ensure routing
   // capabilities for initial platform part sending of status at t=0.0
   // This should be removed in AFSIM 3.0.
   TurnOn(0.0);

   for (auto componentPtr : mComponents)
   {
      auto routerComponentPtr = dynamic_cast<RouterComponent*>(componentPtr);
      if (routerComponentPtr)
      {
         routerComponentPtr->Setup();
      }
   }
}

// =================================================================================================
void Router::PendingStart()
{
   // Call pending start for all attached components with router component interface.
   for (auto componentPtr : mComponents)
   {
      auto routerComponentPtr = dynamic_cast<RouterComponent*>(componentPtr);
      if (routerComponentPtr)
      {
         routerComponentPtr->PendingStart();
      }
   }
}

// =================================================================================================
void Router::InitializeCallbacks()
{
   mCallbacks.Add(mNetworkManager->CommAdded.Connect(&Router::PerfectCommAdded, this));
   mCallbacks.Add(mNetworkManager->CommRemoved.Connect(&Router::PerfectCommRemoved, this));
   mCallbacks.Add(mNetworkManager->NetworkAdded.Connect(&Router::PerfectNetworkAdded, this));
   mCallbacks.Add(mNetworkManager->NetworkRemoved.Connect(&Router::PerfectNetworkRemoved, this));
   mCallbacks.Add(mNetworkManager->CommEnabled.Connect(&Router::PerfectCommEnabled, this));
   mCallbacks.Add(mNetworkManager->CommDisabled.Connect(&Router::PerfectCommDisabled, this));
   mCallbacks.Add(mNetworkManager->ConnectionAdded.Connect(&Router::PerfectConnectionAdded, this));
   mCallbacks.Add(mNetworkManager->ConnectionRemoved.Connect(&Router::PerfectConnectionRemoved, this));
   mCallbacks.Add(mNetworkManager->ConnectionEnabled.Connect(&Router::PerfectConnectionEnabled, this));
   mCallbacks.Add(mNetworkManager->ConnectionDisabled.Connect(&Router::PerfectConnectionDisabled, this));
}

// =================================================================================================
bool Router::Initialize2(double aSimTime)
{
   bool ok = WsfPlatformPart::Initialize2(aSimTime);

   for (auto component : mComponents)
   {
      ok &= component->Initialize2(aSimTime);
   }

   return ok;
}

// =================================================================================================
void Router::PreInput()
{
   WsfComponentFactoryList<Router>::PreInput(mScenario, *this);
}

// =================================================================================================
bool Router::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();

   if (command == "gateway_address") //! Usage: gateway_address "xxx.xxx.xxx.xxx"
   {
      //! This router's gateway, specified by address.
      mGateway.mAddress = Address::ProcessInput(aInput);
   }
   else if (command == "gateway") //! Usage: gateway "comm_name"
   {
      aInput.ReadValue(mGateway.mGatewayCommName);
   }
   else if (command == "hop_limit")
   {
      //! Use int to check for invalid user input < 0
      //! to avoid unsigned rollover and excessive values without error
      std::string hopValue = "";
      aInput.ReadValue(hopValue);

      int hopValueInt = std::stoi(hopValue);
      aInput.ValueGreaterOrEqual(hopValueInt, 0);
      mHopLimit = static_cast<size_t>(hopValueInt);
   }
   else if (command == "use_default_protocol")
   {
      aInput.ReadValue(mUseLegacyProtocol);
   }
   else if (command == "use_multicast_protocol")
   {
      aInput.ReadValue(mUseMulticastProtocol);
   }
   else if (command == "add")
   {
      aInput.ReadCommand(command);
      if (!WsfComponentFactoryList<Router>::ProcessAddOrEditCommand(GetScenario(), aInput, *this, true))
      {
         throw UtInput::BadValue(aInput, "add/edit must be followed by a valid registered router component or medium.");
      }
   }
   else if (command == "edit")
   {
      aInput.ReadCommand(command);
      if (!WsfComponentFactoryList<Router>::ProcessAddOrEditCommand(GetScenario(), aInput, *this, false))
      {
         throw UtInput::BadValue(aInput, "add/edit must be followed by a valid registered router component or medium.");
      }
   }
   else if (command == "delete")
   {
      aInput.ReadCommand(command);
      WsfComponentFactoryList<Router>::ProcessDeleteCommand(GetScenario(), aInput, *this);
   }
   else if (command == "automated_interface_linking")
   {
      aInput.ReadValue(mAutomatedInterfaceLinking);
   }
   // These commands belong to WsfPlatformPart::ProcessInput() below. However, we capture these commands
   // here to inform the user that these commands are not currently supported to ensure consistency with
   // legacy comm framework behavior. This should be removed in AFSIM 3.0.
   else if (command == "off" || command == "non_operational" || command == "broken" || command == "dead")
   {
      ut::log::warning()
         << "The " << command << " command is not supported for router objects. All routers are "
         << "required to be operational by default to support legacy AFSIM behavior. Disabling/turning off the router "
         << "is still supported via script method usage after instantiation in the simulation.";
   }
   else if (WsfPlatformPart::ProcessInput(aInput))
   {
   }
   else if (ProcessTypeAddEditDelete(aInput))
   {
   }
   else
   {
      //! Component based input.
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
         myCommand = WsfComponentFactoryList<Router>::ProcessInput(mScenario, aInput, *this);
      }
   }

   return myCommand;
}

// =================================================================================================
bool Router::ProcessTypeAddEditDelete(UtInput& aInput)
{
   bool myCommand = false;
   return myCommand;
}

// ============================================================================
const char* Router::GetScriptClassName() const
{
   return "WsfCommRouter";
}

// =================================================================================================
bool Router::AddLink(double             aSimTime,
                     const Address&     aSender,
                     const Address&     aDestination,
                     ProtocolInterface& aProtocol,
                     bool               aNotifyObserver) /* = true*/
{
   auto graphPtr = aProtocol.GetGraph();

   if (!IsActive() || !graphPtr)
   {
      return false;
   }

   auto sender      = aSender;
   auto destination = aDestination;

   //! First, add (or find) the node(s) required.
   bool sendNodeAdded = false;
   if (graphPtr->FindNode(aSender) == nullptr)
   {
      auto sendNodePtr = graphPtr->CreateNode(aSender);
      if (sendNodePtr)
      {
         sendNodeAdded = true;
         if (aNotifyObserver)
         {
            GetSimulation()->GetCommObserver().CommAddedToLocal(aSimTime, this, &aProtocol, &sender);
         }
      }
      else
      {
         return false;
      }
   }

   bool destinationNodeAdded = false;
   if (graphPtr->FindNode(aDestination) == nullptr)
   {
      auto destinationNodePtr = graphPtr->CreateNode(aDestination);
      if (destinationNodePtr)
      {
         destinationNodeAdded = true;
         if (aNotifyObserver)
         {
            GetSimulation()->GetCommObserver().CommAddedToLocal(aSimTime, this, &aProtocol, &destination);
         }
      }
      else
      {
         if (sendNodeAdded)
         {
            graphPtr->RemoveNode(aSender);
         }
         return false;
      }
   }

   //! Now add (or find) the edge.
   if (graphPtr->FindEdge(aSender, aDestination) == nullptr)
   {
      auto edgeLinkPtr = graphPtr->InsertEdge(aSender, aDestination, true);
      if (edgeLinkPtr != nullptr)
      {
         if (aNotifyObserver)
         {
            GetSimulation()->GetCommObserver().LinkAddedToLocal(aSimTime, this, &aProtocol, &sender, &destination);
         }
      }
      else
      {
         if (sendNodeAdded)
         {
            graphPtr->RemoveNode(aSender);
         }

         if (destinationNodeAdded)
         {
            graphPtr->RemoveNode(aDestination);
         }
         return false;
      }
   }

   return true;
}

// =================================================================================================
bool Router::RemoveLink(double             aSimTime,
                        const Address&     aSender,
                        const Address&     aDestination,
                        ProtocolInterface& aProtocol,
                        bool               aNotifyObserver) // = true
{
   auto graphPtr = aProtocol.GetGraph();

   if (!IsActive() || !graphPtr)
   {
      return false;
   }

   if (graphPtr->FindEdge(aSender, aDestination) != nullptr) // edge exists
   {
      Address sender{aSender};
      Address destination{aDestination};

      if (graphPtr->EraseEdge(aSender, aDestination))
      {
         if (aNotifyObserver)
         {
            GetSimulation()->GetCommObserver().LinkRemovedFromLocal(aSimTime, this, &aProtocol, &sender, &destination);
         }
      }
   }

   //! Finally, we must check any removed link to ensure it isn't the gateway. If it is the gateway,
   //! then we will provide a warning and clear the gateway, as it is no longer viable.
   if (mGateway.mAddress == aDestination)
   {
      auto out = ut::log::warning() << "Router no longer has a gateway due to loss of connectivity with the gateway.";
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Router: " << GetName();

      mGateway.mAddress         = Address();
      mGateway.mGatewayCommName = "";
   }

   return true;
}

// =================================================================================================
bool Router::AddNode(double             aSimTime,
                     const Address&     aNodeAddress,
                     ProtocolInterface& aProtocol,
                     bool               aNotifyObserver) // = true
{
   auto graphPtr = aProtocol.GetGraph();

   if (!IsActive() || !graphPtr)
   {
      return false;
   }

   //! Check for existence of node in graph.
   if (graphPtr->FindNode(aNodeAddress) != nullptr)
   {
      return false;
   }

   //! Node doesn't exist. Add it to the graph.
   if (graphPtr->CreateNode(aNodeAddress) == nullptr)
   {
      return false;
   }

   //! Notify observer, if set to do so.
   if (aNotifyObserver)
   {
      Address address{aNodeAddress};

      GetSimulation()->GetCommObserver().CommAddedToLocal(aSimTime, this, &aProtocol, &address);
   }

   return true;
}

// =================================================================================================
bool Router::RemoveNode(double             aSimTime,
                        const Address&     aNodeAddress,
                        ProtocolInterface& aProtocol,
                        bool               aNotifyObserver) // = true
{
   auto graphPtr = aProtocol.GetGraph();

   if (!IsActive() || !graphPtr)
   {
      return false;
   }

   //! Check for existence of node in graph.
   auto nodePtr = graphPtr->FindNode(aNodeAddress);
   if (nodePtr == nullptr)
   {
      // Node doesn't exist
      return false;
   }

   Address address{aNodeAddress};

   //! Node exists. Remove all incoming and outgoing edges.
   //! Track the existing edges first, so we can notify the observer after the fact
   //! and track applicable local event logging.
   auto                                     outgoingEdges = graphPtr->GetOutgoingNodeEdges(aNodeAddress);
   std::vector<std::pair<Address, Address>> edgeAddresses;
   for (auto& edge : outgoingEdges)
   {
      edgeAddresses.emplace_back(aNodeAddress, edge->GetDestinationAddress());
   }

   auto incomingEdges = graphPtr->GetIncomingNodeEdges(aNodeAddress);
   for (auto& edge : incomingEdges)
   {
      edgeAddresses.emplace_back(edge->GetSourceAddress(), aNodeAddress);
   }

   graphPtr->RemoveNodeEdges(nodePtr);

   if (aNotifyObserver)
   {
      for (auto& edgeAddress : edgeAddresses)
      {
         GetSimulation()->GetCommObserver().LinkRemovedFromLocal(aSimTime,
                                                                 this,
                                                                 &aProtocol,
                                                                 &edgeAddress.first,
                                                                 &edgeAddress.second);
      }
   }

   //! Remove the node itself.
   graphPtr->RemoveNode(aNodeAddress);
   if (aNotifyObserver)
   {
      GetSimulation()->GetCommObserver().CommRemovedFromLocal(aSimTime, this, &aProtocol, &address);
   }

   return true;
}

// =================================================================================================
bool Router::EnableLink(double             aSimTime,
                        const Address&     aSender,
                        const Address&     aDestination,
                        ProtocolInterface& aProtocol,
                        bool               aNotifyObserver) // = true
{
   bool ok       = false;
   auto graphPtr = aProtocol.GetGraph();

   if (!IsActive() || !graphPtr)
   {
      return ok;
   }

   if (!graphPtr->FindNode(aSender) || !graphPtr->FindNode(aDestination))
   {
      //! One or both nodes are not in the local graph. Can't enable a missing link.
      return ok;
   }

   auto edgePtr = graphPtr->FindEdge(aSender, aDestination);
   if (edgePtr)
   {
      edgePtr->SetEnabled();

      Address sender{aSender};
      Address destination{aDestination};
      GetSimulation()->GetCommObserver().LinkEnabledOnLocal(aSimTime, this, &aProtocol, &sender, &destination);
      ok = true;
   }

   return ok;
}

// =================================================================================================
bool Router::DisableLink(double             aSimTime,
                         const Address&     aSender,
                         const Address&     aDestination,
                         ProtocolInterface& aProtocol,
                         bool               aNotifyObserver) // = true
{
   bool ok       = false;
   auto graphPtr = aProtocol.GetGraph();

   if (!IsActive() || !graphPtr)
   {
      return ok;
   }

   if (!graphPtr->FindNode(aSender) || !graphPtr->FindNode(aDestination))
   {
      //! One or both nodes are not in the local graph. Can't disable a missing link.
      return ok;
   }

   auto edgePtr = graphPtr->FindEdge(aSender, aDestination);
   if (edgePtr)
   {
      edgePtr->SetDisabled();
      Address sender{aSender};
      Address destination{aDestination};
      GetSimulation()->GetCommObserver().LinkDisabledOnLocal(aSimTime, this, &aProtocol, &sender, &destination);

      ok = true;
   }

   return ok;
}

// =================================================================================================
bool Router::EnableNode(double aSimTime, const Address& aNodeAddress, ProtocolInterface& aProtocol)
{
   bool ok       = false;
   auto graphPtr = aProtocol.GetGraph();

   if (!IsActive() || !graphPtr)
   {
      return ok;
   }

   auto nodePtr = graphPtr->FindNode(aNodeAddress);

   if (!nodePtr)
   {
      return ok;
   }
   else
   {
      nodePtr->SetEnabled();
      ok = true;
   }

   return ok;
}

// =================================================================================================
bool Router::DisableNode(double aSimTime, const Address& aNodeAddress, ProtocolInterface& aProtocol)
{
   bool ok       = false;
   auto graphPtr = aProtocol.GetGraph();

   if (!IsActive() || !graphPtr)
   {
      return ok;
   }

   auto nodePtr = graphPtr->FindNode(aNodeAddress);

   if (!nodePtr)
   {
      return ok;
   }
   else
   {
      nodePtr->SetDisabled();
      ok = true;
   }

   return ok;
}

// =================================================================================================
void Router::SetGateway(const Address& aAddress)
{
   Gateway prevGateway       = mGateway;
   mGateway.mAddress         = aAddress;
   mGateway.mGatewayCommName = std::string();

   if (!ValidateGateway())
   {
      auto out = ut::log::warning() << "Router cannot change gateway.";
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Router: " << GetName();
      out.AddNote() << "Old Address: " << prevGateway.mAddress;
      out.AddNote() << "New Address: " << aAddress;
      out.AddNote()
         << "The gateway has not changed. Ensure the provided gateway is an interface assigned to this router.";

      mGateway = prevGateway;
   }
}

// =================================================================================================
bool Router::Broadcast(double aSimTime, SendData& aData)
{
   //! Note: This method uses only truth data from the network manager.
   //! Protocols can handle broadcasts as they see fit, since they get first
   //! crack at handling a send. This is only provided for basic functionality
   //! as a commonly available router mechanic.
   auto& graph       = mNetworkManager->GetGraph();
   auto  networkList = mNetworkManager->GetManagedNetworks();

   //! This is safe, since we already validated this in Router::Send()
   auto message = aData.GetMessages().front();

   if (!aData.GetXmtr())
   {
      return false;
   }

   //! We only support this call from an originator
   if (!message.GetTraceRoute().empty())
   {
      return false;
   }

   auto networkPtr = mNetworkManager->GetNetwork(aData.GetXmtr()->GetNetwork());
   if (networkPtr)
   {
      const auto& networkAddress = networkPtr->GetAddress();
      if (networkAddress.GetBroadcastAddress() == (message.SourceMessage()->GetDstAddr()).GetAddress())
      {
         auto                 networkMembers = mNetworkManager->GetAddressesInNetwork(aData.GetXmtr()->GetNetwork());
         std::vector<Address> recipientList;
         for (const auto& memberAddress : networkMembers)
         {
            if (memberAddress != aData.GetXmtr()->GetAddress())
            {
               recipientList.push_back(memberAddress);
            }
         }

         if (recipientList.size() > 1)
         {
            for (size_t i = 1; i < recipientList.size(); ++i)
            {
               graph::AddressList list;
               double             cost = 0.0;
               auto pathFound          = graph.FindPath(aData.GetXmtr()->GetAddress(), recipientList[i], list, cost);
               if (pathFound && list.size() > 1)
               {
                  auto curIndex = aData.GetMessages().size();
                  aData.GetMessages().push_back(message);
                  auto& curMessage = aData.GetMessages()[curIndex];
                  curMessage.SourceMessage()->SetNextHopAddr(list[1]);
                  curMessage.SourceMessage()->SetDstAddr(recipientList[i]);
                  curMessage.SetTTL(GetHopLimit());
                  curMessage.GetTraceRoute().emplace_back(aData.GetXmtr()->GetAddress());
               }
            }
         }

         graph::AddressList list;
         double             cost      = 0.0;
         auto               pathFound = graph.FindPath(aData.GetXmtr()->GetAddress(), recipientList[0], list, cost);
         if (pathFound && list.size() > 1)
         {
            aData.GetMessages().front().SourceMessage()->SetNextHopAddr(list[1]);
            aData.GetMessages().front().SourceMessage()->SetDstAddr(recipientList[0]);
            return true;
         }
         else
         {
            return false;
         }
      }
   }

   return false;
}

// =================================================================================================
std::vector<ProtocolInterface*> Router::GetSortedProtocols() const
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

// =================================================================================================
bool Router::AddComponent(RouterComponent* aComponentPtr)
{
   return mComponents.AddComponent(aComponentPtr);
}

// =================================================================================================
bool Router::RemoveComponent(RouterComponent* aComponentPtr)
{
   auto removed = mComponents.RemoveComponent(aComponentPtr);
   if (removed)
   {
      delete aComponentPtr;
   }
   return removed;
}

// =================================================================================================
bool Router::Receive(double aSimTime, Comm* aXmtrPtr, Comm* aRcvrPtr, Message& aMessage, bool& aOverrideForward)
{
   if (IsActive())
   {
      // Check the protocols to process the incoming message. The protocols
      // return true to indicate the message requires forwarding. All protocols
      // are checked until/if truth is returned.
      for (auto protocolPtr : GetSortedProtocols())
      {
         if (protocolPtr->Receive(aSimTime, aRcvrPtr->GetAddress(), aMessage, aOverrideForward))
         {
            return true;
         }
      }
   }

   // This message does not require forwarding, as determined by the available protocols.
   return false;
}

// =================================================================================================
bool Router::Send(double aSimTime, SendData& aData)
{
   // The router send method allows the router object to inspect a message and its sending
   // interface, and determine the best path to the destination. Each routing protocol
   // attached to the router is given a turn at processing the message, in the order of
   // each protocol's priority. If a path is found, the message next hop address and
   // appropriate fields are updated, and the method returns true. Failure of the router
   // to find a valid protocol to handle the message and find a path returns false.

   // There should initially only be a single message provide via SendData
   if (aData.GetMessages().size() != 1)
   {
      throw UtException("Invalid number of messages passed to Router::Send()");
   }

   auto&       message            = aData.GetMessages().front();
   bool        pathFound          = false;
   const auto& destinationAddress = message.SourceMessage()->GetDstAddr();

   // The router can only function when "on"
   if (IsActive())
   {
      const auto& sourceAddress = message.SourceMessage()->GetSrcAddr();

      // Check that the source address was provided. This should only happen due to an incorrect comm
      // model implementation's failure to set this value as required when sending messages.
      if (sourceAddress.IsNull())
      {
         ut::log::error() << "Invalid send attempt, missing source address in router send method.";
         throw UtException("Invalid null address in WsfCommRouter::Send from Message.");
      }

      // Check TTL limit.
      if (message.GetTraceRoute().size() >= message.GetTTL())
      {
         auto out = ut::log::warning() << "A message exceeded TTL and has been dropped.";
         out.AddNote() << "Sent From: " << sourceAddress;
         out.AddNote() << "Sent To: " << destinationAddress;
         return false;
      }

      for (auto protocolPtr : GetSortedProtocols())
      {
         pathFound = protocolPtr->Send(aSimTime, aData);
         if (pathFound && aData.GetAbortProcessing())
         {
            // Abort protocol processing
            break;
         }
      }

      // If all protocols failed, check if this is a broadcast message.
      if (!pathFound)
      {
         pathFound = Broadcast(aSimTime, aData);
      }

      // if all protocols and broadcasting attempts failed, use the gateway.
      if (!pathFound)
      {
         const auto& gatewayInterfaceAddress = GetGateway();

         //! Ensure the gateway specified isn't the potential sender when
         //! this message is not originated from that sender.
         auto gatewayPtr = mNetworkManager->GetComm(gatewayInterfaceAddress);
         if (gatewayPtr)
         {
            const auto& gatewayAddress = gatewayPtr->GetGateway();
            if (!gatewayAddress.IsNull())
            {
               pathFound = true;
               message.SourceMessage()->SetNextHopAddr(gatewayAddress);
            }
         }
      }
   } // if(IsActive())

   // Routing success - update the messages appropriately.
   if (pathFound)
   {
      for (auto& updateMessage : aData.GetMessages())
      {
         if (updateMessage.GetTraceRoute().empty())
         {
            updateMessage.GetTraceRoute().push_back(aData.GetXmtr()->GetAddress());
         }

         updateMessage.SetTTL(GetHopLimit());
      }
   }
   // Routing has failed. Notify observers.
   else
   {
      auto         destinationCommPtr = mNetworkManager->GetComm(destinationAddress);
      WsfPlatform* receiverPlatPtr    = nullptr;

      if (destinationCommPtr)
      {
         receiverPlatPtr = destinationCommPtr->GetPlatform();
      }

      if (receiverPlatPtr)
      {
         WsfObserver::MessageFailedRouting(
            GetSimulation())(aSimTime, aData.GetXmtr(), receiverPlatPtr, *message.SourceMessage());
      }
   }

   return pathFound;
}

// =================================================================================================
void Router::PerfectCommAdded(double aSimTime, Comm* aCommPtr)
{
   if (IsActive())
   {
      for (auto protocol : GetSortedProtocols())
      {
         if (protocol->GetGraph() && protocol->ShouldPropagateTruth())
         {
            AddNode(aSimTime, aCommPtr->GetAddress(), *protocol);
         }
         else
         {
            protocol->PerfectCommAdded(aSimTime, aCommPtr);
         }
      }
   }
}

// =================================================================================================
void Router::PerfectCommRemoved(double aSimTime, Comm* aCommPtr)
{
   if (IsActive())
   {
      for (auto protocol : GetSortedProtocols())
      {
         if (protocol->GetGraph() && protocol->ShouldPropagateTruth())
         {
            RemoveNode(aSimTime, aCommPtr->GetAddress(), *protocol);
         }
         else
         {
            protocol->PerfectCommRemoved(aSimTime, aCommPtr);
         }
      }
   }
}

// =================================================================================================
void Router::PerfectNetworkAdded(double aSimTime, Network* aNetworkPtr)
{
   if (IsActive())
   {
      for (auto protocol : GetSortedProtocols())
      {
         if (protocol->GetGraph() && protocol->ShouldPropagateTruth())
         {
         }
         else
         {
            protocol->PerfectNetworkAdded(aSimTime, aNetworkPtr);
         }
      }
   }
}

// =================================================================================================
void Router::PerfectNetworkRemoved(double aSimTime, Network* aNetworkPtr)
{
   if (IsActive())
   {
      for (auto protocol : GetSortedProtocols())
      {
         if (protocol->GetGraph() && protocol->ShouldPropagateTruth())
         {
         }
         else
         {
            protocol->PerfectNetworkRemoved(aSimTime, aNetworkPtr);
         }
      }
   }
}

// =================================================================================================
void Router::PerfectCommEnabled(double aSimTime, Comm* aCommPtr)
{
   if (IsActive())
   {
      for (auto protocol : GetSortedProtocols())
      {
         if (protocol->GetGraph() && protocol->ShouldPropagateTruth())
         {
            EnableNode(aSimTime, aCommPtr->GetAddress(), *protocol);
         }
         else
         {
            protocol->PerfectCommEnabled(aSimTime, aCommPtr);
         }
      }
   }
}

// =================================================================================================
void Router::PerfectCommDisabled(double aSimTime, Comm* aCommPtr)
{
   if (IsActive())
   {
      for (auto protocol : GetSortedProtocols())
      {
         if (protocol->GetGraph() && protocol->ShouldPropagateTruth())
         {
            DisableNode(aSimTime, aCommPtr->GetAddress(), *protocol);
         }
         else
         {
            protocol->PerfectCommDisabled(aSimTime, aCommPtr);
         }
      }
   }
}

// =================================================================================================
void Router::PerfectConnectionAdded(double aSimTime, const Address& aSourceAddress, const Address& aDestinationAddress)
{
   if (IsActive())
   {
      for (auto protocol : GetSortedProtocols())
      {
         if (protocol->GetGraph() && protocol->ShouldPropagateTruth())
         {
            AddLink(aSimTime, aSourceAddress, aDestinationAddress, *protocol);
         }
         else
         {
            protocol->PerfectConnectionAdded(aSimTime, aSourceAddress, aDestinationAddress);
         }
      }
   }
}

// =================================================================================================
void Router::PerfectConnectionRemoved(double aSimTime, const Address& aSourceAddress, const Address& aDestinationAddress)
{
   if (IsActive())
   {
      for (auto protocol : GetSortedProtocols())
      {
         if (protocol->GetGraph() && protocol->ShouldPropagateTruth())
         {
            RemoveLink(aSimTime, aSourceAddress, aDestinationAddress, *protocol);
         }
         else
         {
            protocol->PerfectConnectionRemoved(aSimTime, aSourceAddress, aDestinationAddress);
         }
      }
   }
}

// =================================================================================================
void Router::PerfectConnectionEnabled(double aSimTime, const Address& aSourceAddress, const Address& aDestinationAddress)
{
   if (IsActive())
   {
      for (auto protocol : GetSortedProtocols())
      {
         if (protocol->GetGraph() && protocol->ShouldPropagateTruth())
         {
            EnableLink(aSimTime, aSourceAddress, aDestinationAddress, *protocol);
         }
         else
         {
            protocol->PerfectConnectionEnabled(aSimTime, aSourceAddress, aDestinationAddress);
         }
      }
   }
}

// =================================================================================================
void Router::PerfectConnectionDisabled(double aSimTime, const Address& aSourceAddress, const Address& aDestinationAddress)
{
   if (IsActive())
   {
      for (auto protocol : GetSortedProtocols())
      {
         if (protocol->GetGraph() && protocol->ShouldPropagateTruth())
         {
            DisableLink(aSimTime, aSourceAddress, aDestinationAddress, *protocol);
         }
         else
         {
            protocol->PerfectConnectionDisabled(aSimTime, aSourceAddress, aDestinationAddress);
         }
      }
   }
}

// =================================================================================================
void Router::PartBroken(double aSimTime)
{
   WsfPlatformPart::PartBroken(aSimTime);
   WsfObserver::RouterBroken(GetSimulation())(aSimTime, this);
}

// =================================================================================================
void Router::SetNonOperational(double aSimTime)
{
   WsfPlatformPart::SetNonOperational(aSimTime);
   WsfObserver::RouterNonOperational(GetSimulation())(aSimTime, this);
}

// =================================================================================================
void Router::SetOperational(double aSimTime)
{
   WsfPlatformPart::SetOperational(aSimTime);
   WsfObserver::RouterOperational(GetSimulation())(aSimTime, this);
}

// =================================================================================================
void Router::TurnOff(double aSimTime)
{
   WsfPlatformPart::TurnOff(aSimTime);
   WsfObserver::RouterTurnedOff(GetSimulation())(aSimTime, this);
}

// =================================================================================================
void Router::TurnOn(double aSimTime)
{
   WsfPlatformPart::TurnOn(aSimTime);
   WsfObserver::RouterTurnedOn(GetSimulation())(aSimTime, this);
}

// =================================================================================================
void Router::AddInterface(double aSimTime, Comm* aCommPtr)
{
   // Ensure the interface being added is for this router
   auto sameName     = aCommPtr->GetRouterName() == GetName();
   auto samePlatform = GetPlatform() == aCommPtr->GetPlatform();

   if ((sameName && samePlatform) || (GetSimulation()->IsActive() && samePlatform))
   {
      if (!HasInterface(aCommPtr))
      {
         if (aCommPtr->GetRouter())
         {
            aCommPtr->GetRouter()->RemoveInterface(aSimTime, aCommPtr->GetAddress());
         }

         mComms.emplace_back(aCommPtr);
         aCommPtr->SetRouter(this);
         aCommPtr->SetRouterName(GetName());
      }

      if (mAutomatedInterfaceLinking)
      {
         if (!AutomateAddedLinkage(aSimTime, aCommPtr))
         {
            auto out = ut::log::error() << "Automated linking for comm interface has failed.";
            out.AddNote() << "Comm: " << aCommPtr->GetFullName();
            out.AddNote() << "Router: " << GetName();
            out.AddNote() << "Ensure the network type allows the creation of links for interfaces on this router.";
            throw UtException("Invalid automated linking attempt.");
         }
      }
   }
}

// =================================================================================================
void Router::AddInterface(double aSimTime, const Address& aAddress)
{
   auto commPtr = GetSimulation()->GetCommNetworkManager()->GetComm(aAddress);
   if (commPtr)
   {
      AddInterface(aSimTime, commPtr);
   }
}

// =================================================================================================
void Router::RemoveInterface(double aSimTime, Comm* aCommPtr)
{
   auto it = std::find(std::begin(mComms), std::end(mComms), aCommPtr);
   if (it != std::end(mComms))
   {
      mComms.erase(it);
      aCommPtr->SetRouter(nullptr);
      aCommPtr->SetRouterName(std::string());
   }
}

// =================================================================================================
void Router::RemoveInterface(double aSimTime, const Address& aAddress)
{
   auto commPtr = GetSimulation()->GetCommNetworkManager()->GetComm(aAddress);
   if (commPtr)
   {
      RemoveInterface(aSimTime, commPtr);
   }
}

// =================================================================================================
bool Router::HasInterface(Comm* aCommPtr) const
{
   return (std::end(mComms) != std::find(std::begin(mComms), std::end(mComms), aCommPtr));
}

// =================================================================================================
bool Router::HasInterface(const Address& aAddress) const
{
   return HasInterface(GetSimulation()->GetCommNetworkManager()->GetComm(aAddress));
}

// =================================================================================================
bool Router::AutomateAddedLinkage(double aSimTime, Comm* aCommPtr)
{
   bool        ok          = true;
   auto        networkName = aCommPtr->GetNetwork();
   auto        networkPtr  = mNetworkManager->GetNetwork(networkName);
   const auto& address     = aCommPtr->GetAddress();

   if (!networkPtr || address.IsNull())
   {
      throw UtException("Missing comm initialization data for Router::InterfaceAddedLinkage().");
   }

   for (auto interfacePtr : mComms)
   {
      if (aCommPtr != interfacePtr)
      {
         // If the interfaces are in the same network, use the network object to link
         if (networkName == interfacePtr->GetNetwork())
         {
            ok &= networkPtr->AddLink(address, interfacePtr->GetAddress(), *GetSimulation());
            ok &= networkPtr->AddLink(interfacePtr->GetAddress(), address, *GetSimulation());
         }
         // If not in the same network, create the linkage directly.
         else
         {
            ok &= mNetworkManager->AddConnection(aSimTime, address, interfacePtr->GetAddress());
            ok &= mNetworkManager->AddConnection(aSimTime, interfacePtr->GetAddress(), address);
         }
      }
   }

   return ok;
}

// =================================================================================================
bool Router::ValidateGateway()
{
   Comm* addressCommPtr = nullptr;
   Comm* nameCommPtr    = nullptr;

   if (!mGateway.mAddress.IsNull())
   {
      auto it = std::find_if(std::begin(mComms),
                             std::end(mComms),
                             [this](Comm* aCommPtr) { return aCommPtr->GetAddress() == mGateway.mAddress; });

      if (it == std::end(mComms))
      {
         return false;
      }

      addressCommPtr = *it;
   }

   if (!mGateway.mGatewayCommName.empty())
   {
      auto it = std::find_if(std::begin(mComms),
                             std::end(mComms),
                             [this](Comm* aCommPtr) { return aCommPtr->GetName() == mGateway.mGatewayCommName; });

      if (it == std::end(mComms))
      {
         return false;
      }

      nameCommPtr = *it;
   }

   // If both the address and name was set, ensure they are the same interface.
   if ((addressCommPtr && nameCommPtr) && (!(addressCommPtr == nameCommPtr)))
   {
      return false;
   }

   // Set both the name and address as only one may have been provided.
   if (addressCommPtr && !nameCommPtr)
   {
      mGateway.mGatewayCommName = addressCommPtr->GetName();
   }
   else if (!addressCommPtr && nameCommPtr)
   {
      mGateway.mAddress = nameCommPtr->GetAddress();
   }
   return true;
}

// =================================================================================================
bool Router::IsActive() const
{
   if (GetSimulation()->IsActive())
   {
      return IsTurnedOn();
   }

   return true;
}

} // namespace router
} // namespace comm
} // namespace wsf
