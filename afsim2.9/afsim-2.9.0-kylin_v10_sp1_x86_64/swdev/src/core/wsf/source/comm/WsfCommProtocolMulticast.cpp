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

#include "WsfCommProtocolMulticast.hpp"

#include "WsfAttributeContainer.hpp"
#include "WsfCommNetworkManager.hpp"
#include "WsfCommRoutingAlgorithmLibrary.hpp"
#include "WsfSimulation.hpp"

namespace wsf
{
namespace comm
{
namespace router
{
// this used to be a local variable, but benchmarks on the timeline_lead demo showed
// that the repeated construction of multicastRange took up 10% of total runtime.
static const Address cMulticastRange{"224.0.0.0", 4u};

// =================================================================================================
ProtocolMulticast* ProtocolMulticast::Find(const Router& aParent) // static
{
   auto protocolPtr = aParent.GetComponents().GetComponent<ProtocolMulticast>();
   return protocolPtr;
}

// =================================================================================================
ProtocolMulticast* ProtocolMulticast::FindOrCreate(Router& aParent) // static
{
   auto protocolPtr = Find(aParent);
   if (!protocolPtr)
   {
      protocolPtr = new ProtocolMulticast;
      protocolPtr->SetName("protocol_multicast");
      aParent.GetComponents().AddComponent(protocolPtr);
   }

   return protocolPtr;
}

// =================================================================================================
WsfComponent* ProtocolMulticast::CloneComponent() const
{
   return new ProtocolMulticast(*this);
}

// =================================================================================================
const int* ProtocolMulticast::GetComponentRoles() const
{
   static const int roles[] = {cWSF_COMPONENT_COMM_ROUTER_PROTOCOL_MULTICAST,
                               cWSF_COMPONENT_COMM_ROUTER_PROTOCOL,
                               cWSF_COMPONENT_NULL};

   return roles;
}

// =================================================================================================
ProtocolMulticast* ProtocolMulticast::Clone() const
{
   return new ProtocolMulticast(*this);
}

// =================================================================================================
void* ProtocolMulticast::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_COMM_ROUTER_PROTOCOL_MULTICAST)
   {
      return this;
   }
   if (aRole == cWSF_COMPONENT_COMM_ROUTER_PROTOCOL)
   {
      return dynamic_cast<ProtocolInterface*>(this);
   }

   return nullptr;
}

// =================================================================================================
int ProtocolMulticast::GetPriority() const
{
   return static_cast<int>(ProtocolPriority::cMULTICAST);
}

// =================================================================================================
bool ProtocolMulticast::Initialize(double aSimTime)
{
   //! Set the graph now, if not already set.
   if (!GetGraph())
   {
      mGraph = &GetSimulation()->GetCommNetworkManager()->GetGraph();
   }

   return true;
}

// =================================================================================================
void ProtocolMulticast::Setup()
{
   mGraph = &GetSimulation()->GetCommNetworkManager()->GetGraph();
}

// =================================================================================================
bool ProtocolMulticast::Send(double aSimTime, Router::SendData& aData)
{
   auto                   networkManagerPtr = GetRouter()->GetSimulation()->GetCommNetworkManager();
   bool                   success           = false;
   std::vector<RouteData> routeData;
   bool                   hasSufficientConformance = false;

   //! If conformance level is above 0, we can send a message using the interface.
   if (aData.GetXmtr()->GetMulticastLevel() != Comm::MulticastConformanceLevel::cLEVEL_0)
   {
      hasSufficientConformance = true;
   }

   // Safe call to .front() as this is validated in Router::Send(). End users should validate
   // this if this method is used externally.
   const Address& destinationAddress = aData.GetMessages().front().SourceMessage()->GetDstAddr();

   if (Address::CheckInAddressRange(cMulticastRange, destinationAddress) && hasSufficientConformance)
   {
      // This is a multicast address - prevent other protocol processing.
      aData.SetAbortProcessing(true);

      // Potential multiple recipients in multicast. Resolve those recipients.
      auto recipients = networkManagerPtr->GetMulticastMembers(destinationAddress);

      //! Remove any references to the sender - the sender won't send to self
      auto it = std::find(std::begin(recipients), std::end(recipients), aData.GetXmtr()->GetAddress());
      if (it != std::end(recipients))
      {
         recipients.erase(it);
      }

      // The current sending interface is the source of the transmission if the
      // traceroute is empty
      bool sender = aData.GetMessages().front().GetTraceRoute().empty();

      if (!sender)
      {
         // If we aren't the sender, then we are the destination or along the path
         // to other recipients. Query the message aux data for the correct list.
         auto& aux = aData.GetMessages().front().GetAuxData();
         recipients.clear();
         aux.Get("multicast_recipients", recipients);
         aux.Delete("multicast_recipients");
      }

      if (!recipients.empty())
      {
         // This message must go to all of the multicast group members, so we
         // find a path for each.
         std::vector<double>               costVec;
         std::vector<size_t>               hopVec;
         std::vector<std::vector<Address>> pathVec;

         for (const auto& recipientAddress : recipients)
         {
            double curCost = 0.0;
            size_t curHops = aData.GetMessages().front().GetTraceRoute().size();

            // Create a message for each recipient
            Message newMessage(aData.GetMessages().front());
            newMessage.SourceMessage()->SetDstAddr(recipientAddress);

            auto pathFound = Routing(aSimTime, aData.GetXmtr()->GetAddress(), curHops, curCost, newMessage);

            // Fill the pathing containers with the relevant data
            if (pathFound.size() > 1)
            {
               costVec.push_back(curCost);
               hopVec.push_back(curHops);
               pathVec.push_back(pathFound);
            }
         }

         //! Now we have all the possible paths to members.
         //! For each unique next hop, we have send a message.
         //! However, we also have to look and the traceroute to ensure
         //! we're not going back to a node we've already visited.
         //! This will ensure the correct number of messages are cloned and
         //! sent, and at the appropriate routers.
         std::set<Address> uniquePath;
         size_t            counter = 0;
         for (auto& possiblePath : pathVec)
         {
            if ((std::end(uniquePath) == uniquePath.find(possiblePath[1])))
            {
               // This next hop is unique (not yet encountered). Continue.
               auto& traceRoute = aData.GetMessages().front().GetTraceRoute();
               auto  it         = std::find(std::begin(traceRoute), std::end(traceRoute), possiblePath[1]);
               if (it == std::end(traceRoute))
               {
                  //! This next hop has not been traversed. Add it to our set.
                  uniquePath.insert(possiblePath[1]);
                  routeData.emplace_back();
                  RouteData& data = routeData.back();
                  data.mNextHop   = possiblePath[1];
                  data.mHops      = hopVec[counter];
                  data.mCost      = costVec[counter];
                  data.mFullPath  = possiblePath;
               }
            }
            ++counter;
         }

         //! Insert the set of intended destinations to each unique path.
         for (auto possiblePath : pathVec)
         {
            for (auto& singlePath : routeData)
            {
               if (possiblePath[1] == singlePath.mNextHop)
               {
                  singlePath.mDestinationSet.insert(*(--possiblePath.end()));
               }
            }
         }

         auto numIterations = routeData.size();

         if (numIterations > 1)
         {
            aData.GetMessages().reserve(numIterations);
            for (size_t i = 1; i < numIterations; ++i)
            {
               aData.GetMessages().emplace_back(aData.GetMessages().front());
               Message& sendingMessage = aData.GetMessages().back();
               sendingMessage.SourceMessage()->SetNextHopAddr(routeData[i].mNextHop);
               sendingMessage.GetAuxData().Assign("multicast_recipients", std::move(routeData[i].mDestinationSet));
            }
         }

         if (numIterations != 0)
         {
            aData.GetMessages().front().GetAuxData().Assign("multicast_recipients",
                                                            std::move(routeData[0].mDestinationSet));
            aData.GetMessages().front().SourceMessage()->SetNextHopAddr(routeData[0].mNextHop);
            success = true;
         }
      } // if(!recipients.empty())
   }    // if multicast address

   return success;
}

// =================================================================================================
bool ProtocolMulticast::Receive(double aSimTime, const Address& aReceivingInterface, Message& aMessage, bool& aOverrideForward)
{
   // This method is used by the router to determine if the message needs to be forwarded
   // for multicast routing purposes. Multicast level 2 conformance is required to receive.

   auto           interfacePtr       = GetRouter()->GetNetworkManager()->GetComm(aReceivingInterface);
   const Address& destinationAddress = aMessage.SourceMessage()->GetDstAddr();

   if (!interfacePtr)
   {
      return false;
   }

   // Only process multicast messages with this protocol, and only if the router is functional,
   // as this protocol is only concerned with the routing of the message, not the actual
   // receipt by the interface itself.
   if (Address::CheckInAddressRange(cMulticastRange, destinationAddress) && GetRouter()->IsActive())
   {
      // If receiving is not available for multicast, return false and disable forwarding.
      if (interfacePtr->GetMulticastLevel() != Comm::MulticastConformanceLevel::cLEVEL_2)
      {
         aOverrideForward = true;
         return false;
      }

      // Determine if there are any other recipients of this message.
      // If so, return true so the network layer will forward the message
      if (ForwardMessage(aSimTime, aReceivingInterface, aMessage))
      {
         return true;
      }
   }

   return false;
}

// =================================================================================================
std::vector<Address> ProtocolMulticast::Routing(double         aSimTime,
                                                const Address& aSendingInterface,
                                                size_t&        aHopNumber,
                                                double&        aCost,
                                                const Message& aMessage)
{
   double             curCost = 0.0;
   graph::AddressList path;
   size_t             curHops = aHopNumber;

   //! TODO - ALLOW PASSAGE OF A ROUTING ALGORITHM FOR USER SPECIFICATION
   LeastHops leastHopsAlgorithm;

   // We use the highest priority protocol that can give us a path
   // to the target. This uses the existing available protocols
   // to route the message, such that multicasting works in conjunction
   // with the protocols available to the router.
   for (auto protocolPtr : GetRouter()->GetSortedProtocols())
   {
      // Don't recurse into this protocol
      if (protocolPtr != this)
      {
         auto graphPtr = protocolPtr->GetGraph();
         if (graphPtr)
         {
            graphPtr->FindPath(aSendingInterface, aMessage.SourceMessage()->GetDstAddr(), path, curCost, &leastHopsAlgorithm);
         }

         //! We have to check for path sizes of 1 here, which indicate
         //! a destination of self. The protocol pushes received multicast
         //! messages here, and this comm/router/protocol may be one of
         //! the recipients. In such a case, we ignore it, since it has
         //! already been received.
         if (path.size() > 1)
         {
            curHops += path.size() - 1;
            curCost += aCost;

            if (curHops <= GetRouter()->GetHopLimit())
            {
               aCost      = curCost;
               aHopNumber = curHops;
               break;
            }
         }
      }
   }

   return path;
}

// =================================================================================================
bool ProtocolMulticast::ForwardMessage(double aSimTime, const Address& aReceivingInterface, Message& aMessage)
{
   auto& aux = aMessage.GetAuxData();

   NetworkManager::AddressSet* recipients;
   auto*                       multicast_recipients = aux.FindAttribute("multicast_recipients");
   if (multicast_recipients != nullptr)
   {
      multicast_recipients->Get(recipients);
      // If this interface is in the recipients list, remove it.
      auto it = std::find(std::begin(*recipients), std::end(*recipients), aReceivingInterface);
      if (it != std::end(*recipients))
      {
         recipients->erase(it);
      }
   }

   if (recipients == nullptr || recipients->empty())
   {
      // If no recipients, no need to forward.
      return false;
   }

   // Run through the list of recipients and determine if one of them can
   // be reached through the receiving interface. If so, we immediately
   // return true - the full routing determination will be done when
   // the interface sends the message.

   Address originalDstAddr = aMessage.SourceMessage()->GetDstAddr();
   for (const auto& recipientAddress : *recipients)
   {
      double curCost = 0.0;
      size_t curHops = aMessage.GetTraceRoute().size();

      // Temporarily set the destinaton address to the recipient.
      aMessage.SourceMessage()->SetDstAddr(recipientAddress);

      // Don't be confused - the sending interface in the routing call is correctly
      // referenced as the receiving interface - we're checking if the receiving
      // interface can forward the message to the recipients yet to be reached
      // by this multicast message. The destination "recipient" is contained in
      // the message itself.
      auto pathFound = Routing(aSimTime, aReceivingInterface, curHops, curCost, aMessage);

      // Fill the pathing containers with the relevant data
      if (!pathFound.empty())
      {
         aMessage.SourceMessage()->SetDstAddr(originalDstAddr);
         return true;
      }
   }
   aMessage.SourceMessage()->SetDstAddr(originalDstAddr);
   return false;
}

// ============================================================================
ScriptProtocolClassMulticast::ScriptProtocolClassMulticast(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : ScriptRouterProtocolClass(aClassName, aTypesPtr)
{
   SetClassName("WsfCommProtocolMulticast");
   mConstructible      = false;
   mCloneable          = false;
   mIsScriptAccessible = true;
}

} // namespace router
} // namespace comm
} // namespace wsf
