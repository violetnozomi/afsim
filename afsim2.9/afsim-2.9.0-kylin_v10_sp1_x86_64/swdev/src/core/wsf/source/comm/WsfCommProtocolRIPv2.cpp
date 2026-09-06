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

#include "WsfCommProtocolRIPv2.hpp"

#include <cmath>
#include <random>

#include "UtLexicalCast.hpp"
#include "UtMemory.hpp"
#include "UtRandom.hpp"
#include "WsfAttributeContainer.hpp"
#include "WsfCommEvent.hpp"
#include "WsfCommNetworkManager.hpp"
#include "WsfCommObserver.hpp"
#include "WsfCommRouter.hpp"
#include "WsfSimulation.hpp"

namespace wsf
{
namespace comm
{
namespace router
{
namespace rip
{

constexpr const char*    RIPv2::cRIP_MESSAGE;
static const WsfStringId cID_RIP_MESSAGE = RIPv2::cRIP_MESSAGE;

std::ostream& operator<<(std::ostream& out, const IPv4& ip)
{
   out << ip.to_string();
   return out;
}

std::istream& operator>>(std::istream& in, IPv4& ip)
{
   std::array<int, 4>  octet;
   std::array<char, 3> delim;
   in >> octet[0] >> delim[0] >> octet[1] >> delim[1] >> octet[2] >> delim[2] >> octet[3];
   if (delim[0] == '.' && delim[1] == '.' && delim[2] == '.')
   {
      ip = IPv4{static_cast<std::uint8_t>(octet[0]),
                static_cast<std::uint8_t>(octet[1]),
                static_cast<std::uint8_t>(octet[2]),
                static_cast<std::uint8_t>(octet[3])};
      return in;
   }
   in.setstate(std::ios::failbit);
   return in;
}

Message CreateFullTableRequest()
{
   Message result;
   Entry   entry;
   entry.addressFamilyIdentifier = 0;
   entry.metric                  = cINFINITY;
   result.entries.emplace_back(entry);
   return result;
}

bool isFullTableRequest(const Message& aMsg)
{
   if (aMsg.entries.size() == 1)
   {
      const auto& entry = aMsg.entries.front();
      if (entry.addressFamilyIdentifier == 0 && entry.metric == cINFINITY)
      {
         return true;
      }
   }
   return false;
}

RIPv2::RIPv2(const RIPv2& aSrc)
   : ProtocolInterface(aSrc)
   , mUpdateTimeOut(aSrc.mUpdateTimeOut)
   , mInvalidationTimeout(aSrc.mInvalidationTimeout)
   , mGarbageCollectionTimeOut(aSrc.mGarbageCollectionTimeOut)
   , mPoisonedReverse(aSrc.mPoisonedReverse)
   , mTable()
   , mLastTimeOut()
   , mTriggeredUpdatedPending()
   , mContext(std::make_shared<int>(0))
   , mCallbacks()
{
}

const int* RIPv2::GetComponentRoles() const
{
   static constexpr std::array<int, 4> roles = {cWSF_COMPONENT_COMM_ROUTER_PROTOCOL_RIPv2,
                                                cWSF_COMPONENT_COMM_ROUTER_PROTOCOL,
                                                cWSF_COMPONENT_NULL};
   return roles.data();
}

void* RIPv2::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_COMM_ROUTER_PROTOCOL_RIPv2 || aRole == cWSF_COMPONENT_COMM_ROUTER_PROTOCOL ||
       aRole == cWSF_COMPONENT_NULL)
   {
      return this;
   }
   return nullptr;
}

bool RIPv2::Initialize(double aSimTime)
{
   mCallbacks.Add(WsfObserver::RouterTurnedOn(GetSimulation()).Connect(&RIPv2::TurnOn, this));
   return true;
}

bool RIPv2::ProcessInput(UtInput& aInput)
{
   std::string command = aInput.GetCommand();
   if (command == "update_interval")
   {
      aInput.ReadValueOfType(mUpdateTimeOut, UtInput::cTIME);
      return true;
   }
   if (command == "invalidation_timeout")
   {
      aInput.ReadValueOfType(mInvalidationTimeout, UtInput::cTIME);
      return true;
   }
   if (command == "garbage_collection_timeout")
   {
      aInput.ReadValueOfType(mGarbageCollectionTimeOut, UtInput::cTIME);
      return true;
   }
   if (command == "poisoned_reverse")
   {
      mPoisonedReverse = aInput.ReadBool();
      return true;
   }
   return ProtocolInterface::ProcessInput(aInput);
}

void RIPv2::TurnOn(double aSimTime, Router* aRouterPtr)
{
   if (aRouterPtr && (aRouterPtr == GetRouter()))
   {
      for (auto interfacePtr : aRouterPtr->GetInterfaces())
      {
         // Request routes to populate the table
         auto    request = CreateFullTableRequest();
         Address broadcast{interfacePtr->GetAddress().GetBroadcastAddress()};

         // Send to our bridged comms
         for (auto otherInterfacePtr : aRouterPtr->GetInterfaces())
         {
            if (interfacePtr != otherInterfacePtr)
            {
               SendRIPMessage(aSimTime, interfacePtr->GetAddress(), otherInterfacePtr->GetAddress(), request);
            }
         }

         // Send to our networked comms
         auto networkManager = interfacePtr->GetSimulation()->GetCommNetworkManager();
         auto networkNames   = networkManager->GetManagedNetworks();
         for (const auto& name : networkNames)
         {
            const auto& network        = networkManager->GetNetwork(name);
            const auto& networkAddress = network->GetAddress();
            if (networkAddress.GetBroadcastAddress() == broadcast.GetAddress())
            {
               auto neighbors = networkManager->GetAddressesInNetwork(name);
               for (const auto& n : neighbors)
               {
                  if (n != interfacePtr->GetAddress())
                  {
                     SendRIPMessage(aSimTime, interfacePtr->GetAddress(), n, request);
                  }
               }
            }
         }
      }

      // Begin sending periodic updates of my routing table
      // Per RFC 2453 Section 3.8, the periodic update timer is offset by a
      // random time (+/- 0 to 5 seconds) each time it is set.
      double futureTime = aSimTime + mUpdateTimeOut + GetRandomValue(-5.0, 5.0);
      ScheduleEvent(ut::make_unique<wsf::comm::GenericEvent<int>>(futureTime,
                                                                  mContext,
                                                                  [this, futureTime]() { SendUpdate(futureTime, false); }));
   }
}

std::vector<Address> RIPv2::Routing(double                    aSimTime,
                                    const Address&            aSendingInterface,
                                    size_t&                   aHopNumber,
                                    double&                   aCost,
                                    const wsf::comm::Message& aMessage)
{
   // If it's to us, just return
   if (aMessage.SourceMessage()->GetDstAddr() == aSendingInterface)
   {
      aCost = 0;
      return {};
   }
   // First check if we had a defined route for the specific destination
   auto it = mTable.find(ut::lexical_cast<IPv4>(aMessage.SourceMessage()->GetDstAddr().GetAddress()));
   if (it == std::end(mTable))
   {
      // We don't have a direct route to that address, check to see if we have a route to the network
      auto routingPrefix = ut::lexical_cast<IPv4>(aMessage.SourceMessage()->GetDstAddr().GetRoutingPrefix());
      it                 = mTable.find(routingPrefix);
   }
   if (it != std::end(mTable) && it->second.valid)
   {
      // If the next gateway is myself, then I'm the last router and send it on to the end
      if (it->second.gateway == ut::lexical_cast<IPv4>(aSendingInterface.GetAddress()))
      {
         aCost += 1.0;
         return {{aSendingInterface, aMessage.SourceMessage()->GetDstAddr()}};
      }
      aCost += static_cast<double>(it->second.metric);
      return {{aSendingInterface, Address{it->second.gateway.to_string()}}};
   }
   // Check to see if it's a bridged connection
   auto routerPtr = GetRouter();
   if (routerPtr)
   {
      for (auto commPtr : routerPtr->GetInterfaces())
      {
         if (aMessage.SourceMessage()->GetDstAddr().GetAddress() == commPtr->GetAddress().GetAddress())
         {
            // We're bridged, so we have a direct route
            aCost = 1.0;
            return std::vector<Address>({aSendingInterface, Address{aMessage.SourceMessage()->GetDstAddr().GetAddress()}});
         }
      }
   }
   return std::vector<Address>{};
}

bool RIPv2::Send(double aSimTime, Router::SendData& aData)
{
   double curCost = 0.0;
   size_t numHops = 0;
   auto&  message = aData.GetMessages().front();

   auto pathFound = Routing(aSimTime, aData.GetXmtr()->GetAddress(), numHops, curCost, message);

   auto totalHops = numHops + message.GetTraceRoute().size();

   if ((pathFound.size() > 1) && (totalHops < message.GetTTL()) && (curCost < std::numeric_limits<double>::max()))
   {
      message.SourceMessage()->SetNextHopAddr(pathFound[1]);
      aData.SetAbortProcessing(true);
      return true;
   }

   return false;
}

bool RIPv2::Receive(double aSimTime, const Address& aReceivingInterface, wsf::comm::Message& aMessage, bool& aOverrideForward)
{
   if (aMessage.SourceMessage()->HasAuxData())
   {
      auto* datagramPtr = aMessage.SourceMessage()->GetAuxDataConst().FindAttribute(cRIP_DATAGRAM);
      if (datagramPtr)
      {
         rip::Message* msg;
         datagramPtr->Get(msg);
         MessageReceived(aSimTime, aMessage.SourceMessage()->GetSrcAddr(), aReceivingInterface, *msg);
         return true;
      }
   }
   return false;
}

void RIPv2::MessageReceived(double aSimTime, const Address& aSrcAddr, const Address& aReceivingInterface, const Message& aRIPMsg)
{
   if (aRIPMsg.command == Command::Request)
   {
      RequestReceived(aSimTime, aSrcAddr, aReceivingInterface, aRIPMsg);
   }
   else if (aRIPMsg.command == Command::Response)
   {
      ResponseReceived(aSimTime, aSrcAddr, aReceivingInterface, aRIPMsg);
   }
}

void RIPv2::RequestReceived(double              aSimTime,
                            const Address&      aSrcAddr,
                            const Address&      aReceivingInterface,
                            const rip::Message& aRIPMsg)
{
   // Two kinds of requests can happen, for the whole table or for a subset of the table
   // A whole table request is when there is exactly 1 entry, the address family id is 0,
   // and the metric is 16 (infinity).
   if (isFullTableRequest(aRIPMsg))
   {
      // Generate a response with the full table
      auto response = GenerateFullTableResponse(aSimTime, aSrcAddr);
      SendRIPMessage(aSimTime, aReceivingInterface, aSrcAddr, response);
      return;
   }
   // A request for a portion of the table
   // Note that this is not a typical request and is typically used for
   // diagnostic purposes, see RFC 2453 Section 3.9.1
   auto response = GenerateSpecificEntryResponse(aSimTime, aSrcAddr, aRIPMsg);
   SendRIPMessage(aSimTime, aReceivingInterface, aSrcAddr, response);
}

void RIPv2::ResponseReceived(double              aSimTime,
                             const Address&      aSrcAddr,
                             const Address&      aReceivingInterface,
                             const rip::Message& aRIPMsg)
{
   // Update routing table. If any entries changed, generate and send a new response message with changes.
   auto gateway = ut::lexical_cast<IPv4>(aSrcAddr.GetAddress());
   for (const auto& entry : aRIPMsg.entries)
   {
      auto address = IPv4{entry.address};
      if (address != ut::lexical_cast<IPv4>(aReceivingInterface.GetAddress()))
      {
         auto it = mTable.find(IPv4{address});
         if (it != std::end(mTable))
         {
            // We have an entry for this, so update if necessary
            // Only update if either the cost is cheaper or it's from the same gateway we learned it from
            std::uint64_t cost = std::min<std::uint64_t>(entry.metric + GetCost(gateway), cINFINITY);
            if (cost < it->second.metric || gateway == it->second.gateway)
            {
               it->second.gateway = gateway;
               it->second.metric  = cost;
               it->second.changed = true;
               it->second.valid   = true;
               if (cost == cINFINITY)
               {
                  it->second.valid = false;
                  // Schedule an event to cull this route
                  ScheduleEvent(ut::make_unique<GenericEvent<int>>(aSimTime + mGarbageCollectionTimeOut,
                                                                   mContext,
                                                                   [this, address]()
                                                                   {
                                                                      auto it = mTable.find(address);
                                                                      if (it != std::end(mTable) && !it->second.valid)
                                                                      {
                                                                         mTable.erase(it);
                                                                      }
                                                                   }));
               }
            }
            else if (cost == it->second.metric && gateway != it->second.gateway)
            {
               // RFC 2453 3.9.2: If the existing route is showing signs of timing out, it may be better
               // to switch to an equally-good alternative route immediately, rather than waiting for
               // the timeout to happen. If it's halfway to timing out, replace with an equally good route.
               if ((aSimTime - it->second.lastSeen) > (mInvalidationTimeout / 2.0))
               {
                  it->second.gateway = gateway;
                  it->second.changed = true;
               }
            }
         }
         else
         {
            // We didn't have an entry for this, so just add it to our table if the cost is
            // less than infinity. There's no reason to a log a route that we deem is unreachable.
            std::uint64_t cost = std::min<std::uint64_t>(entry.metric + GetCost(gateway), cINFINITY);
            if (cost < cINFINITY)
            {
               IPEntry e;
               e.subnetMask = IPv4{entry.subnetMask};
               e.gateway    = gateway;
               e.metric     = cost;
               e.changed    = true;
               mTable.emplace(address, e);
            }
         }
      }
   }
   // If we don't have an entry for the router we got the response from, they're new and we need to add them to our table
   if (mTable.find(gateway) == std::end(mTable))
   {
      IPEntry e;
      e.metric   = 1;
      e.lastSeen = aSimTime;
      e.gateway  = ut::lexical_cast<IPv4>(aReceivingInterface.GetAddress());
      e.changed  = true;
      mTable.emplace(gateway, e);
   }
   // Mark any routes from the sender as updated.
   for (auto& t : mTable)
   {
      if (t.second.gateway == gateway)
      {
         t.second.lastSeen = aSimTime;
      }
   }
   // If routes were updated, send a triggered response message, except if we already have a triggered update pending.
   // Or if a regular update will have already be sent by the time we send.
   double futureTime = aSimTime + GetRandomValue(1.0, 5.0);
   if (!mTriggeredUpdatedPending && (futureTime < mLastTimeOut) && HasUpdates())
   {
      mTriggeredUpdatedPending = true;
      ScheduleEvent(ut::make_unique<wsf::comm::GenericEvent<int>>(futureTime,
                                                                  mContext,
                                                                  [this, futureTime]()
                                                                  {
                                                                     mTriggeredUpdatedPending = false;
                                                                     SendUpdate(futureTime, true);
                                                                  }));
   }
}

Message RIPv2::GenerateResponse(double aSimTime, const Address& aSrcAddr, bool aFullTable) const
{
   Message response;
   response.command = Command::Response;
   for (const auto& it : mTable)
   {
      if (it.second.changed || aFullTable)
      {
         Entry e;
         e.address    = it.first.to_uint32();
         e.subnetMask = it.second.subnetMask.to_uint32();
         e.nextHop    = it.second.gateway.to_uint32();
         e.metric     = it.second.metric;
         if (mPoisonedReverse)
         {
            // If we are set to poison a reverse entry, i.e. if the requester is
            // in our table, we don't want them to think we have a better path through
            // them, so set the metric to cINFINITY
            e.metric = cINFINITY;
            response.entries.emplace_back(e);
         }
         else if (Address{it.first.to_string()} != aSrcAddr)
         {
            response.entries.emplace_back(e);
         }
      }
   }
   return response;
}

Message RIPv2::GenerateFullTableResponse(double aSimTime, const Address& aSrcAddr) const
{
   return GenerateResponse(aSimTime, aSrcAddr, true);
}

Message RIPv2::GenerateSpecificEntryResponse(double aSimTime, const Address& aSrcAddr, const Message& aRIPMsg) const
{
   auto response = aRIPMsg;
   for (auto& entry : response.entries)
   {
      auto it = mTable.find(IPv4(entry.address));
      if (it != std::end(mTable))
      {
         // RFC 2453 Section 3.9.1 states that this is for diagnostic purposes
         // and so split horizon and reverse poisoning should not be applied.
         entry.subnetMask = it->second.subnetMask.to_uint32();
         entry.nextHop    = it->second.gateway.to_uint32();
         entry.metric     = it->second.metric;
      }
   }
   return response;
}

void RIPv2::SendUpdate(double aSimTime, bool aTriggered)
{
   if (GetRouter()->IsTurnedOn())
   {
      std::set<IPv4> sentGateways;
      // To make sure we don't try to send to ourselves, add our addresses to the already sent list
      for (auto interfacePtr : GetRouter()->GetInterfaces())
      {
         sentGateways.emplace(ut::lexical_cast<IPv4>(interfacePtr->GetAddress().GetAddress()));
         // For each directly connected router, send a split horizon and/or reverse poisoned response
         for (const auto& entry : mTable)
         {
            if (sentGateways.find(entry.second.gateway) != std::end(sentGateways))
            {
               sentGateways.emplace(entry.second.gateway);
               auto response = GenerateResponse(aSimTime, Address{entry.second.gateway.to_string()}, false);
               SendRIPMessage(aSimTime, interfacePtr->GetAddress(), Address{entry.second.gateway.to_string()}, response);
            }
         }
      }
      for (auto& entry : mTable)
      {
         entry.second.changed = false;
      }

      if (!aTriggered)
      {
         // Reschedule my next update
         double futureTime = aSimTime + mUpdateTimeOut + GetRandomValue(-5.0, 5.0);
         ScheduleEvent(ut::make_unique<GenericEvent<int>>(futureTime,
                                                          mContext,
                                                          [this, futureTime]() { SendUpdate(futureTime, false); }));
      }
      // Check if we have any entries that are supposed to timeout before our next update and schedule
      // an invalidation event
      for (const auto& entry : mTable)
      {
         double anticipatedTimeOut = entry.second.lastSeen + mInvalidationTimeout;
         if (entry.second.valid && (aSimTime + mUpdateTimeOut > anticipatedTimeOut))
         {
            auto dest = entry.first;
            ScheduleEvent(ut::make_unique<GenericEvent<int>>(
               anticipatedTimeOut,
               mContext,
               [this, dest, anticipatedTimeOut]()
               {
                  auto it = mTable.find(dest);
                  // Make sure the entry was still in the table and hasn't been updated.
                  if (it != std::end(mTable))
                  {
                     if (it->second.valid && (it->second.lastSeen + mInvalidationTimeout <= anticipatedTimeOut))
                     {
                        // Mark the entry as invalid and send an update to all connected routers
                        it->second.valid  = false;
                        it->second.metric = cINFINITY;
                        SendExpiredNotification(anticipatedTimeOut, dest);
                        // Schedule a garbage collection for the expired entry.
                        ScheduleEvent(ut::make_unique<GenericEvent<int>>(anticipatedTimeOut + mGarbageCollectionTimeOut,
                                                                         mContext,
                                                                         [this, dest]()
                                                                         {
                                                                            auto it = mTable.find(dest);
                                                                            // If a new route was never found for this
                                                                            // entry delete it.
                                                                            if (it != std::end(mTable) && !it->second.valid)
                                                                            {
                                                                               mTable.erase(it);
                                                                            }
                                                                         }));
                     }
                  }
               }));
         }
      }
   }
}

void RIPv2::SendExpiredNotification(double aSimTime, const IPv4& route) const
{
   Message ripMsg;
   ripMsg.command = Command::Response;
   Entry ripEntry;
   auto  expiredEntry  = mTable.at(route);
   ripEntry.address    = route.to_uint32();
   ripEntry.metric     = expiredEntry.metric;
   ripEntry.subnetMask = expiredEntry.subnetMask.to_uint32();
   ripMsg.entries.emplace_back(ripEntry);
   std::set<IPv4> sentGateways;
   // To make sure we don't try to send to ourselves, add our addresses to the already sent list
   for (auto interfacePtr : GetRouter()->GetInterfaces())
   {
      sentGateways.emplace(ut::lexical_cast<IPv4>(interfacePtr->GetAddress().GetAddress()));
      // For each directly connected router, send a split horizon and/or reverse poisoned response
      for (const auto& entry : mTable)
      {
         if (sentGateways.find(entry.second.gateway) != std::end(sentGateways))
         {
            SendRIPMessage(aSimTime, interfacePtr->GetAddress(), Address{entry.second.gateway.to_string()}, ripMsg);
            sentGateways.emplace(entry.second.gateway);
         }
      }
   }
}

void RIPv2::SendRIPMessage(double aSimTime, const Address& aSource, const Address& aDest, const Message& aResponse) const
{
   auto comm = GetSimulation()->GetCommNetworkManager()->GetComm(aSource);
   if (comm)
   {
      auto sim = GetSimulation();
      if (sim)
      {
         auto responseMsgPtr = ut::make_unique<WsfMessage>(cID_RIP_MESSAGE, aSource, *sim);
         responseMsgPtr->GetAuxData().Add(cRIP_DATAGRAM, aResponse);
         comm->Send(aSimTime, std::move(responseMsgPtr), aDest);
      }
   }
}

std::uint64_t RIPv2::GetCost(const IPv4& aAddress) const
{
   auto it = mTable.find(aAddress);
   if (it != std::end(mTable))
   {
      return it->second.metric;
   }
   return 1;
}

bool RIPv2::HasUpdates() const
{
   auto it = std::find_if(std::begin(mTable),
                          std::end(mTable),
                          [](const std::pair<IPv4, IPEntry>& e) { return e.second.changed; });
   return it != std::end(mTable);
}

void RIPv2::ScheduleEvent(std::unique_ptr<WsfEvent> aEvent) const
{
   // If we're not part of a simulation, don't schedule the event
   if (GetComponentParent())
   {
      auto sim = GetSimulation();
      if (sim)
      {
         sim->AddEvent(std::move(aEvent));
      }
   }
}

double RIPv2::GetRandomValue(double min, double max) const
{
   if (GetComponentParent())
   {
      auto sim = GetSimulation();
      if (sim)
      {
         return sim->GetRandom().Uniform(min, max);
      }
   }
   // If not being run as part of a simulation, generate our own random result
   std::default_random_engine e(std::random_device{}());
   return std::uniform_real_distribution<double>(min, max)(e);
}

int RIPv2::GetPriority() const
{
   return static_cast<int>(ProtocolPriority::cRIPv2);
}

} // namespace rip
} // namespace router
} // namespace comm
} // namespace wsf
