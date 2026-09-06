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

#include "WsfCommProtocolOSPF.hpp"

#include "UtCast.hpp"
#include "WsfAttributeContainer.hpp"
#include "WsfCommEvent.hpp"
#include "WsfCommMessage.hpp"
#include "WsfCommNetworkManager.hpp"
#include "WsfCommPhysicalLayer.hpp"
#include "WsfCommProtocolIGMP.hpp"
#include "WsfCommProtocolMulticast.hpp"
#include "WsfCommRouter.hpp"
#include "WsfCommRoutingAlgorithmLibrary.hpp"
#include "WsfMessage.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformObserver.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"

namespace wsf
{
namespace comm
{
namespace router
{
constexpr const char* ProtocolOSPF::cOSPF_ALL_HELLO;
constexpr const char* ProtocolOSPF::cOSPF_ALL_DROP;
constexpr const char* ProtocolOSPF::cOSPF_ALL_DR_DROP;
constexpr const char* ProtocolOSPF::cOSPF_ALL_DR_ADD;

static const WsfStringId cID_OSPF_ALL_HELLO   = ProtocolOSPF::cOSPF_ALL_HELLO;
static const WsfStringId cID_OSPF_ALL_DROP    = ProtocolOSPF::cOSPF_ALL_DROP;
static const WsfStringId cID_OSPF_ALL_DR_DROP = ProtocolOSPF::cOSPF_ALL_DR_DROP;
static const WsfStringId cID_OSPF_ALL_DR_ADD  = ProtocolOSPF::cOSPF_ALL_DR_ADD;

// ============================================================================
ProtocolOSPF* ProtocolOSPF::Find(const Router& aParent) // static
{
   auto protocolPtr = aParent.GetComponents().GetComponent<ProtocolOSPF>();
   return protocolPtr;
}

// ============================================================================
ProtocolOSPF* ProtocolOSPF::FindOrCreate(Router& aParent) // static
{
   auto protocolPtr = Find(aParent);
   if (!protocolPtr)
   {
      protocolPtr = new ProtocolOSPF;
      aParent.GetComponents().AddComponent(protocolPtr);
   }

   return protocolPtr;
}

// ============================================================================
ProtocolOSPF::ProtocolOSPF(const ProtocolOSPF& aSrc)
   : ProtocolInterface(aSrc)
   , mInputBackbone(aSrc.mInputBackbone)
   , mInputAreas(aSrc.mInputAreas)
   , mAreas()
   , mPriorityDR(aSrc.mPriorityDR)
   , mAreaType(aSrc.mAreaType)
   , mHelloInterval(aSrc.mHelloInterval)
   , mHoldTimer(aSrc.mHoldTimer)
   , mRandomInterval(aSrc.mRandomInterval)
   , mSetup(aSrc.mSetup)
   , mGraph()
   , mLinkStates(aSrc.mLinkStates)
   , mContext(std::make_shared<int>(0))
   , mCallbacks()
{
}

// ============================================================================
WsfComponent* ProtocolOSPF::CloneComponent() const
{
   return new ProtocolOSPF(*this);
}

// ============================================================================
const int* ProtocolOSPF::GetComponentRoles() const
{
   static const int roles[] = {cWSF_COMPONENT_COMM_ROUTER_PROTOCOL_OSPF,
                               cWSF_COMPONENT_COMM_ROUTER_PROTOCOL,
                               cWSF_COMPONENT_NULL};

   return roles;
}

// ============================================================================
ProtocolOSPF* ProtocolOSPF::Clone() const
{
   return new ProtocolOSPF(*this);
}

// ============================================================================
void* ProtocolOSPF::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_COMM_ROUTER_PROTOCOL_OSPF)
   {
      return this;
   }
   if (aRole == cWSF_COMPONENT_COMM_ROUTER_PROTOCOL)
   {
      return dynamic_cast<ProtocolInterface*>(this);
   }

   return nullptr;
}

// ============================================================================
int ProtocolOSPF::GetPriority() const
{
   return static_cast<int>(ProtocolPriority::cOSPF);
}

// ============================================================================
void ProtocolOSPF::Setup()
{
   // This is called via the network manager when all interfaces and routers
   // have finished most of the comm framework initialization tasks. At this
   // point it is appropriate to set up this protocol to ensure correct
   // functionality, based on initial simulation state and user settings.
   // A majority of the methods called here validate user input for correct
   // setup of OSPF enabled routers and interfaces. In addition, this protocol
   // assumes full network convergence and equilibrium at simulation start. This
   // is not the case for any OSPF enabled protocol being activated during sim
   // runtime.
   //
   // NOTE: At run-time, the initialization method will perform the tasks
   // accomplished here. This is required due to the lack of granularity
   // of needed hooks in the simulation initialization process.
   //
   // NOTE: This method should only be functionally executed once before simulation
   // start. All other calls from other interfaces will short-circuit since
   // we check the boolean flag. Avoids usage of statics, and std::call_once
   // type locks that may not provide intended behavior in multi-threaded
   // environments in atypical AFSIM use cases.
   if (!IsSetup())
   {
      auto allOSPF = GetAllOSPF();

      // Get all of the areas defined for all protocols
      // A set will enforce uniqueness
      std::set<Address> areaSet;
      AreaVector        allAreas;
      for (auto protocolPtr : allOSPF)
      {
         const auto& areaAddresses   = protocolPtr->GetInputAreas();
         const auto& backboneAddress = protocolPtr->GetInputBackbone();
         for (const auto& areaAddress : areaAddresses)
         {
            auto result = areaSet.insert(areaAddress);
            if (result.second)
            {
               // This area has not yet been encountered. Create it.
               std::shared_ptr<OSPF_Area> areaPtr = std::make_shared<OSPF_Area>();
               areaPtr->InitializeCallbacks(*GetSimulation());
               areaPtr->SetAddress(areaAddress);
               if (areaAddress == backboneAddress)
               {
                  areaPtr->SetBackbone(true);
               }

               areaPtr->GetProtocols().push_back(protocolPtr);
               allAreas.push_back(areaPtr);
               protocolPtr->GetAreas().push_back(areaPtr);
            }
            else
            {
               // This area has already been created. Fetch it and provide it to the
               // protocol. Ensure the backbone settings are consistent.
               auto areaIt = std::find_if(std::begin(allAreas),
                                          std::end(allAreas),
                                          [&areaAddress](const std::shared_ptr<OSPF_Area>& aAreaPtr)
                                          { return aAreaPtr->GetAddress() == areaAddress; });

               if (areaAddress == backboneAddress)
               {
                  (*areaIt)->SetBackbone(true);
               }

               (*areaIt)->GetProtocols().push_back(protocolPtr);
               protocolPtr->GetAreas().push_back(*areaIt);
            }
         }
      } // end for all OSPF protocols

      // Set the protocol/router role types. All areas must be defined before
      // this occurs to accurately determine the router type within OSPF.
      for (auto protocolPtr : allOSPF)
      {
         SetRouterRoleType(*protocolPtr);

         // Join every interface for every router to the all OSPF multicast group, if possible. Warn otherwise.
         static const Address allOSPF("224.0.0.5", 32);
         if (protocolPtr->GetRouter()->IsActive())
         {
            for (auto commPtr : protocolPtr->GetRouter()->GetInterfaces())
            {
               if (commPtr->GetMulticastLevel() != Comm::MulticastConformanceLevel::cLEVEL_2)
               {
                  auto out = ut::log::warning() << "OSPF usage requires level 2 multicast conformance.";
                  out.AddNote() << "Comm: " << commPtr->GetFullName();
                  out.AddNote() << "OSPF may be non-functional.";
               }
               else
               {
                  auto networkManagerPtr = commPtr->GetSimulation()->GetCommNetworkManager();
                  networkManagerPtr->AddMulticastMember(0.0, allOSPF, commPtr->GetAddress());
               }
            }
         }
      }

      // Verify each area is connected to a backbone
      VerifyBackboneConnections(allAreas);

      // Set the DRs/BDRs for each area.
      // Set the initial graph state for each area using truth.
      for (auto& area : allAreas)
      {
         area->SetDRs();

         auto& graph = GetSimulation()->GetCommNetworkManager()->GetGraph();
         area->InitializeGraphData(graph);
      }

      // Initialize link state data
      InitializeLinkState(allAreas, GetSimulation()->GetCommNetworkManager()->GetGraph());

      // Disable pending start for other OSPF protocols
      for (auto protocolPtr : allOSPF)
      {
         protocolPtr->SetSetup(true);
      }
   }
}

// ============================================================================
bool ProtocolOSPF::Initialize(double aSimTime)
{
   // Test for the existence of the multicast IGMP protocol. If its missing,
   // notify the user.
   auto multicastPtr = GetRouter()->GetComponents().GetComponent<ProtocolMulticast>();
   if (!multicastPtr)
   {
      auto out = ut::log::warning() << "Multicast protocol is not present on router.";
      out.AddNote() << "Platform: " << GetRouter()->GetPlatform()->GetName();
      out.AddNote() << "Router: " << GetRouter()->GetName();
      out.AddNote() << "The OSPF protocol will not function correctly.";
   }

   mHelloInterval.Initialize("hello_interval", nullptr, GetSimulation()->GetScenario().GetScriptContext(), *GetSimulation());
   mHoldTimer.Initialize("hold_timer", nullptr, GetSimulation()->GetScenario().GetScriptContext(), *GetSimulation());
   mRandomInterval.Initialize("random_interval", nullptr, GetSimulation()->GetScenario().GetScriptContext(), *GetSimulation());

   // Draw the random time interval. This should only be done here.
   // All other queries should use the last draw.
   mRandomInterval.Draw();

   // Schedule hello packet sending.
   double futureTime = (aSimTime + mHelloInterval.Draw() + mRandomInterval.LastDraw());
   ScheduleEvent(ut::make_unique<GenericEvent<int>>(futureTime, mContext, [this, futureTime]() { SendHello(futureTime); }));


   // Schedule link state data timeout checks.
   futureTime = (aSimTime + mHoldTimer.Draw() + mRandomInterval.LastDraw());
   ScheduleEvent(
      ut::make_unique<GenericEvent<int>>(futureTime, mContext, [this, futureTime]() { TimeoutUpdate(futureTime, true); }));

   mCallbacks.Add(
      GetSimulation()->GetCommNetworkManager()->CommFrameworkPlatformInitialized.Connect(&ProtocolOSPF::PlatformInitialized,
                                                                                         this));

   return true;
}

// ============================================================================
bool ProtocolOSPF::Send(double aSimTime, Router::SendData& aData)
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

// ============================================================================
bool ProtocolOSPF::Receive(double aSimTime, const Address& aReceivingInterface, Message& aMessage, bool& aOverrideForward)
{
   auto messageType = aMessage.SourceMessage()->GetType();

   if (messageType == cID_OSPF_ALL_HELLO)
   {
      //! Received a hello message.
      auto matchingAreas = MyMessage(aMessage);
      if (!matchingAreas.empty())
      {
         ReceiveHello(aSimTime, aReceivingInterface, matchingAreas, aMessage);
      }
      else
      {
         aOverrideForward = true;
      }
   }
   // else if (messageType == cID_OSPF_ALL_DROP)
   //{
   //    //! Received a drop message for summary table entries.
   //    auto matchingAreas = MyMessage(aMessage);
   //    if (!matchingAreas.empty())
   //    {
   //       ReceiveDropNetwork(aSimTime, aMessage);
   //    }
   //    else
   //    {
   //       aOverrideForward = true;
   //    }
   // }
   else if (messageType == cID_OSPF_ALL_DR_DROP)
   {
      //! Received a drop message for DR/BDR graph usage.
      auto matchingAreas = MyMessage(aMessage);
      if (!matchingAreas.empty())
      {
         ReceiveDR_GraphDrop(aSimTime, aReceivingInterface, matchingAreas, aMessage);
      }
      else
      {
         aOverrideForward = true;
      }
   }
   else if (messageType == cID_OSPF_ALL_DR_ADD)
   {
      //! Receive an add message for DR/BDR graph usage.
      auto matchingAreas = MyMessage(aMessage);
      if (!matchingAreas.empty())
      {
         ReceiveDR_GraphAdd(aSimTime, aReceivingInterface, matchingAreas, aMessage);
      }
      else
      {
         aOverrideForward = true;
      }
   }

   // Routing of multicast messages is handled by multicasting protocols, and default addressing
   // is used for standard OSPF traffic, so we just return false.
   return false;
}

// ============================================================================
std::vector<Address> ProtocolOSPF::MyMessage(Message& aMessage) const
{
   const auto&                 aux = aMessage.SourceMessage()->GetAuxDataConst();
   const std::vector<Address>* areaAddressVecPtr;
   std::vector<Address>        matchingAddresses;

   if (aux.TryGet("areas", areaAddressVecPtr))
   {
      for (const auto& address : *areaAddressVecPtr)
      {
         for (const auto& area : mAreas)
         {
            if (area->GetAddress() == address)
            {
               matchingAddresses.push_back(address);
            }
         }
      }
   }

   return matchingAddresses;
}

// ============================================================================
std::vector<Address> ProtocolOSPF::Routing(double         aSimTime,
                                           const Address& aSendingInterface,
                                           size_t&        aHopNumber,
                                           double&        aCost,
                                           const Message& aMessage)
{
   std::vector<Address> path;
   LeastHops            leastHopsAlgorithm;
   auto                 networkManagerPtr   = GetSimulation()->GetCommNetworkManager();
   double               curCost             = 0.0;
   bool                 pathFound           = false;
   const Address&       destinationAddress  = aMessage.SourceMessage()->GetDstAddr();
   auto                 sendingInterfacePtr = networkManagerPtr->GetComm(aSendingInterface);
   Address              backboneAddress     = Address();

   // Determine if the destination address is in one of our areas we are a member of
   for (const auto& area : mAreas)
   {
      // Save this data for later, in case the destination is not in a local area.
      if (area->IsBackbone())
      {
         backboneAddress = area->GetAddress();
      }

      if (FindLinkState(area->GetAddress(), aSendingInterface, destinationAddress))
      {
         // If in the local area, just route to the destination using the local DR
         auto itDR = area->GetDRs().find(sendingInterfacePtr->GetNetwork());
         if ((itDR != std::end(area->GetDRs())) && itDR->second)
         {
            auto graphPtr = itDR->second->GetGraph(area->GetAddress());
            pathFound = graphPtr->FindPath(aSendingInterface, destinationAddress, path, curCost, &leastHopsAlgorithm);

            if (pathFound && (path.size() > 1))
            {
               aCost += curCost;
               aHopNumber += path.size() - 1;
               return path;
            }
            else
            {
               path.clear();
               curCost = 0.0;
            }
         }
      }
   }

   // Destination was not in local area. Determine if its in a connected OSPF area.
   std::set<Address> checkedAreas;
   auto              areaAddress = InterfaceInContiguousOSPF(destinationAddress, *this, checkedAreas);

   if (!areaAddress.IsNull())
   {
      // The destination is in a connected OSPF region.
      if (!backboneAddress.IsNull())
      {
         path = RouteFromBackbone(*this, aSendingInterface, destinationAddress, aCost, aHopNumber);
         return path;
      }
      else
      {
         // Send this message to the backbone area using the optimal route to an interface
         // on an ABR/ABSR in the backbone. We send to the backbone because we know the destination
         // is not in our area, and we know the destination information must be contained in the
         // backbone.
         path = RouteToBackbone(*this, aSendingInterface, aCost, aHopNumber);
         return path;
      }
   }

   // The destination is not in a connected OSPF area. The ASBRs may be able to route this message
   // if they have connectivity using another protocol. Check if this is possible.
   // auto localAddress = Address();
   ProtocolOSPF* optimalASBR_Ptr = nullptr;
   auto          pathPair        = RouteExternallyASBR(aMessage);

   if (pathPair.first.size() > 1)
   {
      OSPF_Area* sharedArea = nullptr;
      optimalASBR_Ptr       = pathPair.second->GetRouter()->GetComponents().GetComponent<ProtocolOSPF>();
      if (!optimalASBR_Ptr)
      {
         throw UtException("Missing OSPF protocol exception");
      }

      // We have an ASBR that can route to the destination. Now we need to path to the ASBR within OSPF.
      // If the ASBR is in our area, route directly to it. If not, route to the backbone.
      for (const auto& area : GetAreas())
      {
         for (const auto& ASBR_area : optimalASBR_Ptr->GetAreas())
         {
            if (area->GetAddress() == ASBR_area->GetAddress())
            {
               // Same area. Check link state to validate path.
               if (FindLinkState(area->GetAddress(), aSendingInterface, pathPair.second->GetAddress()))
               {
                  // Link state exists, and in the same area.
                  sharedArea = area.get();
                  break;
               }
            }
         }
         if (sharedArea)
         {
            break;
         }
      }

      if (sharedArea)
      {
         // We share the same area as the ASBR - route within the area.
         path.clear();

         auto itDR = sharedArea->GetDRs().find(sendingInterfacePtr->GetNetwork());
         if ((itDR != std::end(sharedArea->GetDRs())) && itDR->second)
         {
            auto graphPtr = itDR->second->GetGraph(sharedArea->GetAddress());
            pathFound =
               graphPtr->FindPath(aSendingInterface, pathPair.second->GetAddress(), path, curCost, &leastHopsAlgorithm);

            if (pathFound && (path.size() > 1))
            {
               aCost += curCost;
               aHopNumber += path.size() - 1;
               return path;
            }
            else
            {
               path.clear();
               curCost = 0.0;
               return path;
            }
         }
      }
      else
      {
         // Not in the same area. Route to the correct area.
         path.clear();
         for (const auto& area : GetAreas())
         {
            if (area->IsBackbone())
            {
               backboneAddress = area->GetAddress();
            }
         }

         if (!backboneAddress.IsNull())
         {
            // We are in the backbone. Send to the ASBR interface with the optimal external route for further
            // routing outside of OSPF.
            path = RouteFromBackbone(*this, aSendingInterface, pathPair.second->GetAddress(), aCost, aHopNumber);
            return path;
         }
         else
         {
            // We are not in the backbone - send to backbone.
            path = RouteToBackbone(*this, aSendingInterface, aCost, aHopNumber);
            return path;
         }
      }
   }

   return path;
}

// ============================================================================
OSPF_Area* ProtocolOSPF::GetArea(const Address& aAreaAddress)
{
   auto it = std::find_if(std::begin(mAreas),
                          std::end(mAreas),
                          [aAreaAddress](const std::shared_ptr<OSPF_Area>& aArea)
                          { return aArea->GetAddress() == aAreaAddress; });

   if (it != std::end(mAreas))
   {
      return it->get();
   }

   return nullptr;
}

// ============================================================================
std::vector<Address> ProtocolOSPF::RouteFromBackbone(ProtocolOSPF&  aProtocol,
                                                     const Address& aSendingAddress,
                                                     const Address& aDestinationAddress,
                                                     double&        aCost,
                                                     size_t&        aHops)
{
   LeastHops            leastHopsAlgorithm;
   auto                 localCost = std::numeric_limits<double>::max();
   double               curCost   = 0.0;
   std::vector<Address> localPath, path;
   auto                 localAddress        = Address();
   bool                 localPathFound      = false;
   auto                 sendingInterfacePtr = GetSimulation()->GetCommNetworkManager()->GetComm(aSendingAddress);

   // Since every area is connected to the backbone, and we've established that we
   // do not have the link state data locally, one of the other ABR/ASBRs in area zero
   // must have this information. Find the router(s) that do, and use the optimal path to it.
   for (const auto& area : mAreas)
   {
      if (area->IsBackbone())
      {
         for (auto protocolPtr : area->GetProtocols())
         {
            if ((protocolPtr->GetAreaType() == RouterAreaType::cABR) ||
                (protocolPtr->GetAreaType() == RouterAreaType::cASBR))
            {
               for (const auto& otherArea : protocolPtr->GetAreas())
               {
                  if (!otherArea->IsBackbone())
                  {
                     auto linkStates = protocolPtr->FindLinkState(otherArea->GetAddress(), aDestinationAddress);
                     for (auto linkState : linkStates)
                     {
                        // We found the destination. Find the optimal path.
                        auto itDR      = area->GetDRs().find(sendingInterfacePtr->GetNetwork());
                        auto graphPtr  = itDR->second->GetGraph(area->GetAddress());
                        localPathFound = graphPtr->FindPath(aSendingAddress,
                                                            linkState.mLocalInterface,
                                                            localPath,
                                                            curCost,
                                                            &leastHopsAlgorithm);

                        if (localPathFound && (localPath.size() > 1))
                        {
                           if (curCost < localCost)
                           {
                              localCost    = curCost;
                              path         = localPath;
                              localAddress = linkState.mLocalInterface;
                           }
                        }

                        localPathFound = false;
                     }
                  }
               }
            }
         }
      }
   }

   if (!(path.size() > 1))
   {
      path.clear();
      aHops = 0;
      aCost = 0.0;
   }
   else
   {
      aHops += path.size() - 1;
      aCost += localCost;
   }

   return path;
}

// ============================================================================
std::vector<Address>
ProtocolOSPF::RouteToBackbone(ProtocolOSPF& aProtocol, const Address& aSendingAddress, double& aCost, size_t& aHops)
{
   LeastHops            leastHopsAlgorithm;
   auto                 localCost = std::numeric_limits<double>::max();
   double               curCost   = 0.0;
   std::vector<Address> localPath, path;
   auto                 localAddress        = Address();
   bool                 localPathFound      = false;
   auto                 sendingInterfacePtr = GetSimulation()->GetCommNetworkManager()->GetComm(aSendingAddress);

   // Send this message to the backbone area using the optimal route to an interface
   // on an ABR/ABSR in the backbone. We send to the backbone because we know the destination
   // is not in our area, and we know the destination information must be contained in the
   // backbone.
   for (const auto& area : mAreas)
   {
      for (auto protocolPtr : area->GetProtocols())
      {
         if ((protocolPtr->GetAreaType() == RouterAreaType::cABR) || (protocolPtr->GetAreaType() == RouterAreaType::cASBR))
         {
            //! We only want the ABR/ASBRs. Check that they are in the backbone.
            for (const auto& otherArea : protocolPtr->GetAreas())
            {
               if (otherArea->IsBackbone())
               {
                  // This router connects to area 0. Find optimal pathing, assuming we
                  // have a link state to the interfaces.
                  for (auto interfacePtr : protocolPtr->GetRouter()->GetInterfaces())
                  {
                     if (aProtocol.FindLinkState(area->GetAddress(), aSendingAddress, interfacePtr->GetAddress()))
                     {
                        auto itDR      = area->GetDRs().find(sendingInterfacePtr->GetNetwork());
                        auto graphPtr  = itDR->second->GetGraph(area->GetAddress());
                        localPathFound = graphPtr->FindPath(aSendingAddress,
                                                            interfacePtr->GetAddress(),
                                                            localPath,
                                                            curCost,
                                                            &leastHopsAlgorithm);

                        if (localPathFound && (localPath.size() > 1))
                        {
                           if (curCost < localCost)
                           {
                              localCost    = curCost;
                              path         = localPath;
                              localAddress = interfacePtr->GetAddress();
                           }
                        }

                        localPathFound = false;
                     }
                  }
               }
            }
         }
      }
   }

   if (!(path.size() > 1))
   {
      path.clear();
      aHops = 0;
      aCost = 0.0;
   }
   else
   {
      aHops += path.size() - 1;
      aCost += localCost;
   }

   return path;
}

// ============================================================================
std::pair<std::vector<Address>, Comm*> ProtocolOSPF::RouteExternallyASBR(const Message& aMessage)
{
   std::set<Address>    checkedAreas;
   auto                 allASBRs  = GetAllASBR(*this, checkedAreas);
   auto                 localCost = std::numeric_limits<double>::max();
   double               curCost   = 0.0;
   std::vector<Address> localPath, path;
   auto                 localAddress = Address();
   Comm*                optimalPtr   = nullptr;

   for (auto protocolPtr : allASBRs)
   {
      for (auto interfacePtr : protocolPtr->GetRouter()->GetInterfaces())
      {
         for (auto componentPtr : protocolPtr->GetRouter()->GetComponents())
         {
            // Determine if a path exists using other components. We exclude ourself (OSPF)
            // and multicast, and ensure its a routing protocol.
            if ((!componentPtr->ComponentHasRole(cWSF_COMPONENT_COMM_ROUTER_PROTOCOL_OSPF)) &&
                (!componentPtr->ComponentHasRole(cWSF_COMPONENT_COMM_ROUTER_PROTOCOL_MULTICAST)) &&
                (componentPtr->ComponentHasRole(cWSF_COMPONENT_COMM_ROUTER_PROTOCOL)))
            {
               // This is safe, since we know its a routing protocol.
               ProtocolInterface* routingProtocol = static_cast<ProtocolInterface*>(componentPtr);

               size_t hops = 0;
               localPath =
                  routingProtocol->Routing(GetSimulation()->GetSimTime(), interfacePtr->GetAddress(), hops, curCost, aMessage);

               if (localPath.size() > 1)
               {
                  if (curCost < localCost)
                  {
                     optimalPtr   = interfacePtr;
                     localCost    = curCost;
                     path         = localPath;
                     localAddress = interfacePtr->GetAddress();
                  }
               }
            }
         }
      }
   }

   if (!(path.size() > 1))
   {
      path.clear();
      optimalPtr = nullptr;
   }

   return std::make_pair(path, optimalPtr);
}

// ============================================================================
ProtocolOSPF::ProtocolVector ProtocolOSPF::GetAllOSPF()
{
   ProtocolVector returnList;
   auto           simPtr = GetRouter()->GetSimulation();

   auto numPlats = simPtr->GetPlatformCount();
   for (size_t i = 0; i < numPlats; ++i)
   {
      auto platformPtr = simPtr->GetPlatformEntry(i);
      for (WsfComponentList::RoleIterator<router::Router> iter{platformPtr->GetComponents()}; !iter.AtEnd(); ++iter)
      {
         for (WsfComponentList::RoleIterator<ProtocolOSPF> protocolIter{iter->GetComponents()}; !protocolIter.AtEnd();
              ++protocolIter)
         {
            returnList.push_back(*protocolIter);
         }
      }
   }

   return returnList;
}

// ============================================================================
std::set<std::shared_ptr<OSPF_Area>> ProtocolOSPF::GetAllAreas()
{
   std::set<std::shared_ptr<OSPF_Area>> returnAreas;
   auto                                 protocols = GetAllOSPF();

   for (auto protocolPtr : protocols)
   {
      for (const auto& area : protocolPtr->GetAreas())
      {
         returnAreas.insert(area);
      }
   }

   return returnAreas;
}

// ============================================================================
void ProtocolOSPF::SetRouterRoleType(ProtocolOSPF& aProtocol)
{
   auto areas = aProtocol.GetAreas();

   if (areas.empty())
   {
      auto out = ut::log::error() << "Missing area designation on OSPF enabled router.";
      out.AddNote() << "Platform: " << aProtocol.GetRouter()->GetPlatform()->GetName();
      out.AddNote() << "Router: " << aProtocol.GetRouter()->GetName();
      throw UtException("OSPF missing area designation.");
   }
   else if (!aProtocol.GetRouter()->IsActive() && aProtocol.IsSetup())
   {
      aProtocol.SetAreaType(RouterAreaType::cNONE);
   }
   else if (areas.size() > 1)
   {
      aProtocol.SetAreaType(RouterAreaType::cABR);
   }

   if (aProtocol.GetAreaType() != RouterAreaType::cNONE)
   {
      // Check protocol usage to determine if an ABSR.
      for (auto componentPtr : aProtocol.GetRouter()->GetComponents())
      {
         auto isOSPF      = componentPtr->ComponentHasRole(cWSF_COMPONENT_COMM_ROUTER_PROTOCOL_OSPF);
         auto isMulticast = componentPtr->ComponentHasRole(cWSF_COMPONENT_COMM_ROUTER_PROTOCOL_MULTICAST);
         auto isRouting   = componentPtr->ComponentHasRole(cWSF_COMPONENT_COMM_ROUTER_PROTOCOL);
         if ((!(isOSPF || isMulticast)) && isRouting)
         {
            aProtocol.SetAreaType(RouterAreaType::cASBR);
         }
      }
   }
}

// ============================================================================
bool ProtocolOSPF::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();

   if (command == "ospf_area")
   {
      auto address = Address::ProcessInput(aInput);
      mInputAreas.push_back(address);
   }
   else if (command == "remove_ospf_area")
   {
      auto address = Address::ProcessInput(aInput);
      auto it      = std::find(std::begin(mInputAreas), std::end(mInputAreas), address);
      if (it != std::end(mInputAreas))
      {
         mInputAreas.erase(it);
      }
   }
   else if (command == "backbone")
   {
      auto address   = Address::ProcessInput(aInput);
      mInputBackbone = address;
   }
   else if (command == "remove_backbone")
   {
      mInputBackbone = Address();
   }
   else if (command == "ospf_dr_priority")
   {
      int id;
      aInput.ReadValue(id);
      mPriorityDR = ut::cast_to_size_t(id);
   }
   else if (command == "hello_interval")
   {
      WsfRandomVariable temp;
      temp.ProcessInput(aInput, UtInput::cTIME);
      mHelloInterval = temp;
   }
   else if (command == "hold_timer")
   {
      WsfRandomVariable temp;
      temp.ProcessInput(aInput, UtInput::cTIME);
      mHoldTimer = temp;
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

// ============================================================================
void ProtocolOSPF::VerifyBackboneConnections(const AreaVector& aAreas)
{
   for (const auto& area : aAreas)
   {
      if (!area->IsBackbone())
      {
         auto protocols = area->GetProtocols();
         bool connected = false;
         for (auto protocolPtr : protocols)
         {
            auto otherAreas = protocolPtr->GetAreas();
            for (auto otherArea : otherAreas)
            {
               if (otherArea->IsBackbone())
               {
                  connected = true;
                  break;
               }
            }

            if (connected)
            {
               break;
            }
         }

         if (!connected)
         {
            auto out = ut::log::error() << "OSPF area not connected to designated backbone area.";
            out.AddNote() << "Area: " << area->GetAddress();
            throw UtException("Invalid OSPF backbone setup.");
         }
      }
   }
}

// ============================================================================
Comm* ProtocolOSPF::GetPriorityInterface(const std::string aNetwork) const
{
   Comm*  commPtr  = nullptr;
   size_t priority = 0;

   for (auto interfacePtr : GetRouter()->GetInterfaces())
   {
      if (interfacePtr->GetNetwork() == aNetwork)
      {
         auto curPriority = interfacePtr->GetAddress().GetBinaryInteger();
         if (curPriority > priority)
         {
            priority = curPriority;
            commPtr  = interfacePtr;
         }
      }
   }

   return commPtr;
}

// ============================================================================
void ProtocolOSPF::SendHello(double aSimTime)
{
   static const Address allOSPF("224.0.0.5", 32);

   std::vector<Address> areaAddressVec;
   for (const auto& area : mAreas)
   {
      areaAddressVec.push_back(area->GetAddress());
   }

   // Send out a hello message using each interface available to the router.
   for (auto interfacePtr : GetRouter()->GetInterfaces())
   {
      auto messagePtr = ut::make_unique<WsfMessage>(cID_OSPF_ALL_HELLO);
      messagePtr->SetSimulation(GetSimulation());
      messagePtr->SetSerialNumber(GetSimulation()->NextMessageSerialNumber());
      auto& aux = messagePtr->GetAuxData();
      aux.Add("source", interfacePtr->GetAddress());
      aux.Add("network", interfacePtr->GetNetwork());
      aux.Add("areas", areaAddressVec);

      interfacePtr->Send(aSimTime, std::move(messagePtr), allOSPF);
   }

   //! Reschedule hello packet sending.
   double futureTime = (aSimTime + mHelloInterval.Draw() + mRandomInterval.LastDraw());
   ScheduleEvent(ut::make_unique<GenericEvent<int>>(futureTime, mContext, [this, futureTime]() { SendHello(futureTime); }));
}

// ============================================================================
void ProtocolOSPF::ReceiveHello(double                      aSimTime,
                                const Address&              aReceiverAddress,
                                const std::vector<Address>& aValidAreas,
                                Message&                    aMessage)
{
   // Note - this assumes this message pertains an area we belong to
   // and of this particular type.
   // This is validated already in the Receive() method.
   const Address*     senderAddressPtr;
   const std::string* senderNetworkPtr;
   const auto&        aux = aMessage.SourceMessage()->GetAuxDataConst();
   aux.Get("source", senderAddressPtr);
   aux.Get("network", senderNetworkPtr);

   // Check to see if we already have an entry for the sender
   for (const auto& validAreaAddress : aValidAreas)
   {
      auto linkStatePtr = FindLinkState(validAreaAddress, aReceiverAddress, *senderAddressPtr);
      if (linkStatePtr)
      {
         // We already have this entry. Update the timeout value.
         linkStatePtr->mLastUpdateTime = aSimTime;
      }
      else
      {
         // A link state wasn't found. Create it.
         LinkStateData data;
         auto          networkManagerPtr = GetSimulation()->GetCommNetworkManager();
         auto          receivingCommPtr  = networkManagerPtr->GetComm(aReceiverAddress);

         if (receivingCommPtr)
         {
            data.mLastUpdateTime  = aSimTime;
            data.mLocalInterface  = aReceiverAddress;
            data.mLocalNetwork    = receivingCommPtr->GetNetwork();
            data.mRemoteInterface = *senderAddressPtr;
            data.mRemoteNetwork   = *senderNetworkPtr;

            AddLinkState(validAreaAddress, data);

            // If we are the DR/BDR, update the network graph locally.
            const auto& DRs  = GetArea(validAreaAddress)->GetDRs();
            const auto& BDRs = GetArea(validAreaAddress)->GetBDRs();

            auto itDR  = DRs.find(receivingCommPtr->GetNetwork());
            auto itBDR = BDRs.find(receivingCommPtr->GetNetwork());

            if (((itDR != std::end(DRs)) && (itDR->second == this)) ||
                ((itBDR != std::end(BDRs)) && (itBDR->second == this)))
            {
               DR_GraphAdd(*GetGraph(validAreaAddress), aReceiverAddress, aMessage.GetTraceRoute());
            }

            SendDR_GraphAdd(aSimTime, aReceiverAddress, aMessage.GetTraceRoute());
         }
      }
   }

   TimeoutUpdate(aSimTime, false);
}

// ============================================================================
void ProtocolOSPF::DR_GraphAdd(graph::Graph& aGraph, const Address& aReceivingInterface, const std::vector<Address>& aTraceRoute)
{
   const Address* prevAddressPtr = nullptr;

   for (const auto& address : aTraceRoute)
   {
      if (!aGraph.FindNode(address))
      {
         aGraph.CreateNode(address);
      }

      if (prevAddressPtr != nullptr && !prevAddressPtr->IsNull())
      {
         if (!aGraph.FindEdge(*prevAddressPtr, address))
         {
            aGraph.InsertEdge(*prevAddressPtr, address, true);
         }
      }

      prevAddressPtr = &address;
   }

   if (!aGraph.FindNode(aReceivingInterface))
   {
      aGraph.CreateNode(aReceivingInterface);
   }

   if (prevAddressPtr != nullptr)
   {
      if (!aGraph.FindEdge(*prevAddressPtr, aReceivingInterface))
      {
         aGraph.InsertEdge(*prevAddressPtr, aReceivingInterface, true);
      }
   }
}

// ============================================================================
void ProtocolOSPF::DR_GraphDrop(graph::Graph&      aGraph,
                                const Address&     aArea,
                                const Address&     aDroppedInterface,
                                const std::string& aDroppedNetwork)
{
   // auto networkManagerPtr = GetSimulation()->GetCommNetworkManager();
   // auto droppedInterfacePtr = networkManagerPtr->GetComm(aDroppedInterface);
   auto areaPtr = GetArea(aArea);

   auto itDR = areaPtr->GetRemovedInterfacesDR().find(aDroppedNetwork);
   if (itDR != std::end(areaPtr->GetRemovedInterfacesDR()))
   {
      auto addressIt = std::find(std::begin(itDR->second), std::end(itDR->second), aDroppedInterface);
      if (addressIt != std::end(itDR->second))
      {
         if (itDR->second.size() == 1)
         {
            areaPtr->GetRemovedInterfacesDR().erase(itDR);

            // The DR has been removed. Have an election.
            areaPtr->ElectionDR(GetSimulation()->GetSimTime(), aDroppedNetwork, aArea, true);
         }
         else
         {
            itDR->second.erase(addressIt);
         }
      }
   }

   auto itBDR = areaPtr->GetRemovedInterfacesBDR().find(aDroppedNetwork);
   if (itBDR != std::end(areaPtr->GetRemovedInterfacesBDR()))
   {
      auto addressIt = std::find(std::begin(itBDR->second), std::end(itBDR->second), aDroppedInterface);
      if (addressIt != std::end(itBDR->second))
      {
         if (itBDR->second.size() == 1)
         {
            areaPtr->GetRemovedInterfacesBDR().erase(itBDR);

            // The BDR has been removed. Have an election.
            areaPtr->ElectionDR(GetSimulation()->GetSimTime(), aDroppedNetwork, aArea, false);

            // Set the BDR graph to coincide with ours.
            auto itNewBDR = areaPtr->GetBDRs().find(aDroppedNetwork);
            if (itNewBDR != std::end(areaPtr->GetBDRs()))
            {
               auto graph = *GetGraph(areaPtr->GetAddress());
               itNewBDR->second->SetGraph(areaPtr->GetAddress(), ut::make_unique<graph::Graph>(graph));
            }
         }
         else
         {
            itBDR->second.erase(addressIt);
         }
      }
   }

   aGraph.RemoveNode(aDroppedInterface);
}

// ============================================================================
void ProtocolOSPF::SendDR_GraphAdd(double aSimTime, const Address& aReceivingInterface, const std::vector<Address>& aTraceRoute)
{
   //! Send a message to DR/BDR over multicast about a discovered interface and connections.
   auto networkManagerPtr = GetSimulation()->GetCommNetworkManager();
   auto interfacePtr      = networkManagerPtr->GetComm(aReceivingInterface);
   if (interfacePtr)
   {
      std::vector<Address> areaAddressVec;
      areaAddressVec.reserve(mAreas.size());
      for (const auto& area : mAreas)
      {
         areaAddressVec.push_back(area->GetAddress());
      }

      // We need to add ourselves to the traceroute, since we haven't "sent"
      // the route we're pushing out.
      auto updatedTrace = aTraceRoute;
      updatedTrace.push_back(aReceivingInterface);

      static const Address allDR("224.0.0.6", 32);
      auto                 messagePtr = ut::make_unique<WsfMessage>(cID_OSPF_ALL_DR_ADD);
      messagePtr->SetSerialNumber(GetSimulation()->NextMessageSerialNumber());
      messagePtr->SetSimulation(GetSimulation());
      auto& aux = messagePtr->GetAuxData();
      aux.Add("destination", aReceivingInterface);
      aux.Add("traceroute", std::move(updatedTrace));
      aux.Add("areas", std::move(areaAddressVec));

      interfacePtr->Send(aSimTime, std::move(messagePtr), allDR);
   }
}

// ============================================================================
void ProtocolOSPF::ReceiveDR_GraphAdd(double                      aSimTime,
                                      const Address&              aReceivingInterface,
                                      const std::vector<Address>& aValidAreas,
                                      Message&                    aMessage)
{
   const std::vector<Address>* traceRoutePtr;
   const Address*              destinationAddressPtr;
   auto                        networkManagerPtr = GetSimulation()->GetCommNetworkManager();
   auto                        interfacePtr      = networkManagerPtr->GetComm(aReceivingInterface);

   const auto& aux = aMessage.SourceMessage()->GetAuxDataConst();
   aux.Get("destination", destinationAddressPtr);
   aux.Get("traceroute", traceRoutePtr);

   for (const auto& areaAddress : aValidAreas)
   {
      const auto& DRs  = GetArea(areaAddress)->GetDRs();
      const auto& BDRs = GetArea(areaAddress)->GetBDRs();

      auto itDR  = DRs.find(interfacePtr->GetNetwork());
      auto itBDR = BDRs.find(interfacePtr->GetNetwork());

      if (((itDR != std::end(DRs)) && (itDR->second == this)) || ((itBDR != std::end(BDRs)) && (itBDR->second == this)))
      {
         DR_GraphAdd(*GetGraph(areaAddress), aReceivingInterface, *traceRoutePtr);
      }
   }
}

// ============================================================================
void ProtocolOSPF::SendDR_GraphDrop(double             aSimTime,
                                    const Address&     aReceivingInterface,
                                    const Address&     aDroppedInterface,
                                    const std::string& aDroppedInterfaceNetwork)
{
   auto networkManagerPtr = GetSimulation()->GetCommNetworkManager();
   auto interfacePtr      = networkManagerPtr->GetComm(aReceivingInterface);
   if (interfacePtr)
   {
      std::vector<Address> areaAddressVec;
      areaAddressVec.reserve(mAreas.size());
      for (const auto& area : mAreas)
      {
         areaAddressVec.push_back(area->GetAddress());
      }

      static const Address allDR("224.0.0.6", 32);
      auto                 messagePtr = ut::make_unique<WsfMessage>(cID_OSPF_ALL_DR_DROP);
      messagePtr->SetSerialNumber(GetSimulation()->NextMessageSerialNumber());
      messagePtr->SetSimulation(GetSimulation());
      auto& aux = messagePtr->GetAuxData();
      aux.Add("destination", aDroppedInterface);
      aux.Add("areas", std::move(areaAddressVec));
      aux.Add("network", aDroppedInterfaceNetwork);

      interfacePtr->Send(aSimTime, std::move(messagePtr), allDR);
   }
}

// ============================================================================
void ProtocolOSPF::ReceiveDR_GraphDrop(double                      aSimTime,
                                       const Address&              aReceivingInterface,
                                       const std::vector<Address>& aValidAreas,
                                       Message&                    aMessage)
{
   const Address*     destinationAddressPtr;
   const std::string* droppedNetworkPtr;
   auto               networkManagerPtr = GetSimulation()->GetCommNetworkManager();
   auto               interfacePtr      = networkManagerPtr->GetComm(aReceivingInterface);

   const auto& aux = aMessage.SourceMessage()->GetAuxDataConst();
   aux.Get("destination", destinationAddressPtr);
   aux.Get("network", droppedNetworkPtr);

   for (const auto& areaAddress : aValidAreas)
   {
      const auto& DRs  = GetArea(areaAddress)->GetDRs();
      const auto& BDRs = GetArea(areaAddress)->GetBDRs();

      auto itDR  = DRs.find(interfacePtr->GetNetwork());
      auto itBDR = BDRs.find(interfacePtr->GetNetwork());

      if (((itDR != std::end(DRs)) && (itDR->second == this)) || ((itBDR != std::end(BDRs)) && (itBDR->second == this)))
      {
         DR_GraphDrop(*GetGraph(areaAddress), areaAddress, *destinationAddressPtr, *droppedNetworkPtr);
      }
   }
}

// ============================================================================
void ProtocolOSPF::InitializeLinkState(const AreaVector& aAreas, graph::Graph& aGraph)
{
   for (auto& area : aAreas)
   {
      auto interfaces = area->GetInterfaces();

      // Determine if a link state exists by checking for paths between interfaces.
      for (auto sourceInterfacePtr : interfaces)
      {
         for (auto destinationInterfacePtr : interfaces)
         {
            if (sourceInterfacePtr != destinationInterfacePtr)
            {
               const Address& sourceAddress      = sourceInterfacePtr.first->GetAddress();
               const Address& destinationAddress = destinationInterfacePtr.first->GetAddress();

               double cost       = 0.0;
               bool   pathExists = aGraph.FindAnyPath(sourceAddress, destinationAddress, nullptr, cost);

               if (pathExists)
               {
                  LinkStateData data;
                  data.mLastUpdateTime  = 0.0;
                  data.mLocalInterface  = sourceAddress;
                  data.mRemoteInterface = destinationAddress;
                  data.mLocalNetwork    = sourceInterfacePtr.first->GetNetwork();
                  data.mRemoteNetwork   = destinationInterfacePtr.first->GetNetwork();

                  sourceInterfacePtr.second->AddLinkState(area->GetAddress(), data);
               }
            }
         }
      }
   }
}

// ============================================================================
Address ProtocolOSPF::InterfaceInContiguousOSPF(const Address&     aDestinationAddress,
                                                ProtocolOSPF&      aProtocol,
                                                std::set<Address>& aCheckedAreas)
{
   for (const auto& area : aProtocol.GetAreas())
   {
      if (aCheckedAreas.find(area->GetAddress()) == std::end(aCheckedAreas))
      {
         aCheckedAreas.insert(area->GetAddress());

         for (auto protocolPtr : area->GetProtocols())
         {
            for (auto commPtr : protocolPtr->GetRouter()->GetInterfaces())
            {
               if (protocolPtr->FindLinkState(area->GetAddress(), commPtr->GetAddress(), aDestinationAddress))
               {
                  return area->GetAddress();
               }
            }

            auto address = InterfaceInContiguousOSPF(aDestinationAddress, *protocolPtr, aCheckedAreas);
            if (!address.IsNull())
            {
               return address;
            }
         }
      }
   }

   return Address();
}

// ============================================================================
std::set<ProtocolOSPF*> ProtocolOSPF::GetAllASBR(ProtocolOSPF& aProtocol, std::set<Address>& aCheckedAreas)
{
   std::set<ProtocolOSPF*> ASBRs;

   for (const auto& area : aProtocol.GetAreas())
   {
      if (aCheckedAreas.find(area->GetAddress()) == std::end(aCheckedAreas))
      {
         aCheckedAreas.insert(area->GetAddress());

         for (auto protocolPtr : area->GetProtocols())
         {
            if (protocolPtr->GetAreaType() == RouterAreaType::cASBR)
            {
               ASBRs.insert(protocolPtr);
            }

            auto otherASBRs = GetAllASBR(*protocolPtr, aCheckedAreas);
            for (auto entry : otherASBRs)
            {
               ASBRs.insert(entry);
            }
         }
      }
   }

   return ASBRs;
}

// ============================================================================
void ProtocolOSPF::AddLinkState(const Address& aAreaAddress, const LinkStateData& aData)
{
   auto           it            = mLinkStates.find(aAreaAddress);
   const Address& localAddress  = aData.mLocalInterface;
   const Address& remoteAddress = aData.mRemoteInterface;
   if (it != std::end(mLinkStates))
   {
      auto LinkStateIt =
         std::find_if(std::begin(it->second),
                      std::end(it->second),
                      [&localAddress, &remoteAddress](const LinkStateData& aData)
                      { return (aData.mLocalInterface == localAddress) && (aData.mRemoteInterface == remoteAddress); });

      if (LinkStateIt == std::end(it->second))
      {
         it->second.push_back(aData);
      }
   }
   else
   {
      mLinkStates.insert(std::make_pair(aAreaAddress, std::vector<LinkStateData>{aData}));
   }
}

// ============================================================================
ProtocolOSPF::LinkStateData* ProtocolOSPF::FindLinkState(const Address& aAreaAddress,
                                                         const Address& aLocalAddress,
                                                         const Address& aRemoteAddress)
{
   auto entryIt = mLinkStates.find(aAreaAddress);
   {
      if (entryIt != std::end(mLinkStates))
      {
         auto it = std::find_if(std::begin(entryIt->second),
                                std::end(entryIt->second),
                                [&aLocalAddress, &aRemoteAddress](const LinkStateData& aData) {
                                   return (aData.mLocalInterface == aLocalAddress) &&
                                          (aData.mRemoteInterface == aRemoteAddress);
                                });

         if (it != std::end(entryIt->second))
         {
            return &(*it);
         }
      }
   }

   return nullptr;
}

// ============================================================================
std::vector<ProtocolOSPF::LinkStateData> ProtocolOSPF::FindLinkState(const Address& aAreaAddress,
                                                                     const Address& aRemoteAddress)
{
   std::vector<LinkStateData> data;

   auto entryIt = mLinkStates.find(aAreaAddress);
   if (entryIt != std::end(mLinkStates))
   {
      for (auto entry : entryIt->second)
      {
         if (entry.mRemoteInterface == aRemoteAddress)
         {
            data.push_back(entry);
         }
      }
   }

   return data;
}

// ============================================================================
bool ProtocolOSPF::RemoveLinkState(const Address& aAreaAddress, const Address& aLocalAddress, const Address& aRemoteAddress)
{
   auto entryIt = mLinkStates.find(aAreaAddress);
   if (entryIt != std::end(mLinkStates))
   {
      auto it =
         std::find_if(std::begin(entryIt->second),
                      std::end(entryIt->second),
                      [&aLocalAddress, &aRemoteAddress](const LinkStateData& aData)
                      { return (aData.mLocalInterface == aLocalAddress) && (aData.mRemoteInterface == aRemoteAddress); });

      if (it != std::end(entryIt->second))
      {
         entryIt->second.erase(it);
         if (entryIt->second.empty())
         {
            mLinkStates.erase(entryIt);
         }

         return true;
      }
   }


   return false;
}

// ============================================================================
void ProtocolOSPF::ScheduleEvent(std::unique_ptr<WsfEvent> aEvent) const
{
   if (GetComponentParent())
   {
      auto sim = GetSimulation();
      if (sim)
      {
         sim->AddEvent(std::move(aEvent));
      }
   }
}

// ============================================================================
void ProtocolOSPF::TimeoutUpdate(double aSimTime, bool aCalledFromEvent)
{
   // Traverse link state data and find any values the exceed the hold time since last update.
   // Send a drop message for any stale entry.
   for (auto& mapEntry : mLinkStates)
   {
      for (auto it = std::begin(mapEntry.second); it != std::end(mapEntry.second);)
      {
         if ((aSimTime - it->mLastUpdateTime) > mHoldTimer.Draw())
         {
            SendDR_GraphDrop(aSimTime, it->mLocalInterface, it->mRemoteInterface, it->mRemoteNetwork);
            it = mapEntry.second.erase(it);
         }
         else
         {
            ++it;
         }
      }
   }

   // Run back through the link states, so we can delete any key values that may no longer have values.
   for (auto it = std::begin(mLinkStates); it != std::end(mLinkStates);)
   {
      if (it->second.empty())
      {
         it = mLinkStates.erase(it);
      }
      else
      {
         ++it;
      }
   }


   if (aCalledFromEvent)
   {
      double futureTime = (aSimTime + mHoldTimer.Draw() + mRandomInterval.LastDraw());
      ScheduleEvent(ut::make_unique<GenericEvent<int>>(futureTime,
                                                       mContext,
                                                       [this, futureTime]() { TimeoutUpdate(futureTime, true); }));
   }
}

// ============================================================================
void ProtocolOSPF::PlatformInitialized(double aSimTime, WsfPlatform* aPlatformPtr)
{
   if (GetSimulation()->IsActive())
   {
      // Iterate through any newly added routers, and determine if they have OSPF protocol.
      for (WsfComponentList::RoleIterator<Router> iter{aPlatformPtr->GetComponents()}; !iter.AtEnd(); ++iter)
      {
         auto protocolPtr = (*iter)->GetComponents().GetComponent<ProtocolOSPF>();
         if (protocolPtr && !protocolPtr->IsSetup())
         {
            static const Address allOSPF("224.0.0.5", 32);

            auto        allAreas      = GetAllAreas();
            const auto& areaAddresses = protocolPtr->GetInputAreas();

            for (const auto& address : areaAddresses)
            {
               const auto& backboneAddress = protocolPtr->GetInputBackbone();
               auto        areaIt          = std::find_if(std::begin(allAreas),
                                          std::end(allAreas),
                                          [&address](const std::shared_ptr<OSPF_Area>& areaPtr)
                                          { return areaPtr->GetAddress() == address; });

               if (std::end(allAreas) != areaIt)
               {
                  // This area already exists. Process adding the new router into this area.
                  if ((address == backboneAddress) && !(*areaIt)->IsBackbone())
                  {
                     auto out = ut::log::error() << "Inconsistent backbone setting.";
                     out.AddNote() << "Address: " << address;
                     out.AddNote() << "Ensure all backbone settings are consistent.";
                     throw UtException("Inconsistent OSPF backbone setting.");
                  }

                  (*areaIt)->GetProtocols().push_back(protocolPtr);
                  protocolPtr->GetAreas().push_back(*areaIt);
                  (*areaIt)->SetNetworks();
               }
               else
               {
                  // This is a new area. Create the area, and set it up with this router as the first member.
                  std::shared_ptr<OSPF_Area> areaPtr = std::make_shared<OSPF_Area>();
                  areaPtr->InitializeCallbacks(*GetSimulation());
                  areaPtr->SetAddress(address);
                  if (address == backboneAddress)
                  {
                     areaPtr->SetBackbone(true);
                  }

                  areaPtr->GetProtocols().push_back(protocolPtr);
                  protocolPtr->GetAreas().push_back(areaPtr);
                  areaPtr->SetNetworks();
               }
            }

            SetRouterRoleType(*protocolPtr);

            // Join every interface for every router to the all OSPF multicast group, if possible. Warn otherwise.
            for (auto commPtr : protocolPtr->GetRouter()->GetInterfaces())
            {
               if (commPtr->GetMulticastLevel() != Comm::MulticastConformanceLevel::cLEVEL_2)
               {
                  auto out = ut::log::warning() << "OSPF usage requires level 2 multicast conformance.";
                  out.AddNote() << "Comm: " << commPtr->GetFullName();
                  out.AddNote() << "OSPF may be non-functional.";
               }
               else
               {
                  auto networkManagerPtr = commPtr->GetSimulation()->GetCommNetworkManager();
                  networkManagerPtr->AddMulticastMember(aSimTime, allOSPF, commPtr->GetAddress());
               }
            }

            // Verify each area is connected to a backbone
            VerifyBackboneConnections(AreaVector(std::begin(allAreas), std::end(allAreas)));

            // Set the DRs/BDRs for each area, if required.
            for (auto& area : protocolPtr->GetAreas())
            {
               auto& DRs  = area->GetDRs();
               auto& BDRs = area->GetBDRs();

               for (auto interfacePtr : protocolPtr->GetRouter()->GetInterfaces())
               {
                  auto itDR  = DRs.find(interfacePtr->GetNetwork());
                  auto itBDR = BDRs.find(interfacePtr->GetNetwork());
                  if (itDR == std::end(DRs))
                  {
                     // This network in the area does not have a DR. Elect one (us).
                     area->ElectionDR(aSimTime, interfacePtr->GetNetwork(), area->GetAddress(), true);

                     // Since we're the first DR in this segment, we'll add ourselves
                     // to the graph.
                     protocolPtr->SetGraph(area->GetAddress(), ut::make_unique<graph::Graph>());
                     protocolPtr->GetGraph(area->GetAddress())->CreateNode(interfacePtr->GetAddress());
                  }
                  else if (itBDR == std::end(BDRs))
                  {
                     area->ElectionDR(aSimTime, interfacePtr->GetNetwork(), area->GetAddress(), false);

                     // This protocol is guaranteed the BDR if we didn't have one.
                     // Set its graph to coincide with the DR.
                     itDR = DRs.find(interfacePtr->GetNetwork());
                     if (itDR == std::end(DRs))
                     {
                        throw UtException("OSPF Added BDR election exception.");
                     }

                     graph::Graph copyGraph = *itDR->second->GetGraph(area->GetAddress());
                     protocolPtr->SetGraph(area->GetAddress(), ut::make_unique<graph::Graph>(copyGraph));
                  }
               }
            }
            protocolPtr->SetSetup(true);
         }
      }
   }
}

// ============================================================================
graph::Graph* ProtocolOSPF::GetGraph() const
{
   return &GetSimulation()->GetCommNetworkManager()->GetGraph();
}

// ============================================================================
graph::Graph* ProtocolOSPF::GetGraph(const Address& aArea)
{
   auto it = mGraph.find(aArea);
   if (it != std::end(mGraph))
   {
      return it->second.get();
   }

   return nullptr;
}

// ============================================================================
void ProtocolOSPF::SetGraph(const Address& aArea, std::unique_ptr<graph::Graph> aGraph)
{
   mGraph[aArea] = std::move(aGraph);
}

// ============================================================================
void OSPF_Area::InitializeGraphData(graph::Graph& aGraph)
{
   // This method grabs all the interfaces in the area, then using the provided graph,
   // checks for the existence of these interfaces and their connections. If found in
   // the provided graph, they are used for this area.
   graph::Graph curGraph;

   for (auto protocolPtr : mProtocols)
   {
      auto interfaces = protocolPtr->GetRouter()->GetInterfaces();
      for (auto interfacePtr : interfaces)
      {
         const auto& sourceAddress    = interfacePtr->GetAddress();
         auto        physicalLayerPtr = interfacePtr->GetProtocolStack().GetLayer<PhysicalLayer>();
         auto        weight           = 0.0;

         if (physicalLayerPtr)
         {
            // OSPF uses a reference of 100 Mbps for the weight metric, which is
            // what the bandwidth is divided by, with a fixed lowest value of 1.
            // We'll still use 0 for instantaneous transfers.
            weight = (physicalLayerPtr->GetTransferRate() / 1000000);
            if ((weight < 1.0) && (weight != 0.0))
            {
               weight = 1.0;
            }
         }

         for (auto otherProtocolPtr : mProtocols)
         {
            for (auto otherInterfacePtr : otherProtocolPtr->GetRouter()->GetInterfaces())
            {
               // Don't compare the same interfaces
               if (interfacePtr != otherInterfacePtr)
               {
                  const auto& destinationAddress = otherInterfacePtr->GetAddress();

                  auto sourceNodePtr      = aGraph.FindNode(sourceAddress);
                  auto destinationNodePtr = aGraph.FindNode(destinationAddress);
                  auto edgePtr            = aGraph.FindEdge(sourceAddress, destinationAddress);

                  if (sourceNodePtr)
                  {
                     if (!curGraph.FindNode(sourceAddress))
                     {
                        curGraph.CreateNode(sourceAddress);
                     }
                  }
                  if (destinationNodePtr)
                  {
                     if (!curGraph.FindNode(destinationAddress))
                     {
                        curGraph.CreateNode(destinationAddress);
                     }
                  }
                  if (edgePtr)
                  {
                     if (!curGraph.FindEdge(sourceAddress, destinationAddress))
                     {
                        auto curEdgePtr = curGraph.InsertEdge(sourceAddress, destinationAddress, true);
                        curEdgePtr->SetWeight(weight);
                     }
                  }
               }
            }
         }
      }
   }

   //! Now that the graph is completed with all connections available in an area,
   //! distribute this graph to all DR/BDRs in the area.
   for (auto& entryDR : mMapDR)
   {
      if (entryDR.second)
      {
         auto graphCopy = curGraph;
         entryDR.second->SetGraph(GetAddress(), ut::make_unique<graph::Graph>(graphCopy));
      }
   }

   for (auto& entryBDR : mMapBDR)
   {
      if (entryBDR.second)
      {
         auto graphCopy = curGraph;
         entryBDR.second->SetGraph(GetAddress(), ut::make_unique<graph::Graph>(graphCopy));
      }
   }
}

// ============================================================================
void OSPF_Area::SetNetworks()
{
   mNetworks.clear();

   for (auto protocolPtr : mProtocols)
   {
      for (auto interfacePtr : protocolPtr->GetRouter()->GetInterfaces())
      {
         mNetworks.insert(interfacePtr->GetNetwork());
      }
   }
}

// ============================================================================
OSPF_Area::ProtocolVector OSPF_Area::GetProtocols(const std::string& aNetwork)
{
   ProtocolVector returnProtocols;

   for (auto protocolPtr : mProtocols)
   {
      for (auto interfacePtr : protocolPtr->GetRouter()->GetInterfaces())
      {
         if (interfacePtr->GetNetwork() == aNetwork)
         {
            returnProtocols.push_back(protocolPtr);
            break;
         }
      }
   }

   return returnProtocols;
}

// ============================================================================
void OSPF_Area::SetDRs()
{
   // Ensure network membership is correct before processing.
   SetNetworks();

   // Each network results in a DR, and possibly a BDR, assuming router availability
   for (auto networkName : mNetworks)
   {
      ElectionDR(0.0, networkName, GetAddress(), true);
   }
}

// ============================================================================
void OSPF_Area::ElectionDR(double aSimTime, const std::string aNetwork, const Address& aArea, bool aElectionDR)
{
   static const Address allDR("224.0.0.6", 32);

   // Get the protocols/routers in this area who have interfaces in the network
   auto protocols = GetProtocols(aNetwork);

   if (aElectionDR)
   {
      // This is a DR Election - a BDR may or may not exist
      // Check for BDR existence
      auto it = mMapBDR.find(aNetwork);
      if (it != std::end(mMapBDR) && it->second)
      {
         // A BDR exists
         // Promote the BDR to DR, then elect a new BDR with a recursive call
         mMapDR[aNetwork] = it->second;
         mMapBDR.erase(it);
         ElectionDR(aSimTime, aNetwork, aArea, false);
      }
      else
      {
         // A BDR does NOT exist
         // Elect the DR, then elect a BDR with a recursive call
         size_t        priority    = std::numeric_limits<size_t>::max();
         ProtocolOSPF* routerPtrDR = nullptr;

         for (auto protocolPtr : protocols)
         {
            auto curPriority = protocolPtr->GetPriorityDR();
            if ((curPriority < priority) || !routerPtrDR)
            {
               priority    = curPriority;
               routerPtrDR = protocolPtr;
            }
            else if (curPriority == priority)
            {
               // Break ties with interface addressing
               auto curAddressVal   = routerPtrDR->GetPriorityInterface(aNetwork)->GetAddress().GetBinaryInteger();
               auto checkAddressVal = protocolPtr->GetPriorityInterface(aNetwork)->GetAddress().GetBinaryInteger();
               if (checkAddressVal > curAddressVal)
               {
                  priority    = curPriority;
                  routerPtrDR = protocolPtr;
               }
            }
         }

         if (!routerPtrDR)
         {
            auto out = ut::log::warning() << "A designated router could not be elected for OSPF.";
            out.AddNote() << "Network: " << aNetwork;
            out.AddNote() << "Area: " << GetAddress();
            out.AddNote() << "This area will not be able to route messages using OSPF and will fail communications "
                             "using this protocol.";
            mMapDR.erase(aNetwork);
         }
         else
         {
            mMapDR[aNetwork] = routerPtrDR;
            // This router/protocol must join the multicast
            // group 224.0.0.6 to receive updates from other OSPF enabled
            // routers for connectivity updates.
            // Use the lowest binary value addressed interface for this purpose.
            auto networkManagerPtr = routerPtrDR->GetSimulation()->GetCommNetworkManager();
            networkManagerPtr->AddMulticastMember(aSimTime,
                                                  allDR,
                                                  routerPtrDR->GetPriorityInterface(aNetwork)->GetAddress());
            ElectionDR(aSimTime, aNetwork, aArea, false);
         }
      }
   }
   else
   {
      // This is a BDR Election - the DR already exists
      auto itDR = mMapDR.find(aNetwork);
      if ((itDR == std::end(mMapDR)) || !itDR->second)
      {
         throw UtException("Invalid election of BDR with missing DR.");
      }

      auto          protocolPtrDR = itDR->second;
      ProtocolOSPF* routerPtrBDR  = nullptr;
      size_t        priority      = std::numeric_limits<size_t>::max();

      for (auto protocolPtr : protocols)
      {
         // Don't consider the DR in the election for the BDR
         if (protocolPtr != protocolPtrDR)
         {
            auto curPriority = protocolPtr->GetPriorityDR();
            if ((curPriority < priority) || !routerPtrBDR)
            {
               priority     = curPriority;
               routerPtrBDR = protocolPtr;
            }
            else if (curPriority == priority)
            {
               // Break ties with interface addressing
               auto curAddressVal   = routerPtrBDR->GetPriorityInterface(aNetwork)->GetAddress().GetBinaryInteger();
               auto checkAddressVal = protocolPtr->GetPriorityInterface(aNetwork)->GetAddress().GetBinaryInteger();
               if (checkAddressVal > curAddressVal)
               {
                  priority     = curPriority;
                  routerPtrBDR = protocolPtr;
               }
            }
         }
      }

      if (!routerPtrBDR)
      {
         // Don't warn if there isn't a BDR - this is common and doesn't affect functionality.
         mMapBDR.erase(aNetwork);
      }
      else
      {
         mMapBDR[aNetwork]      = routerPtrBDR;
         graph::Graph graphCopy = *itDR->second->GetGraph();
         routerPtrBDR->SetGraph(aArea, ut::make_unique<graph::Graph>(graphCopy));

         // This router/protocol must join the multicast
         // group 224.0.0.6 to receive updates from other OSPF enabled
         // routers for connectivity updates.
         // Use the lowest binary value addressed interface for this purpose.
         auto networkManagerPtr = routerPtrBDR->GetSimulation()->GetCommNetworkManager();
         networkManagerPtr->AddMulticastMember(aSimTime, allDR, routerPtrBDR->GetPriorityInterface(aNetwork)->GetAddress());
      }
   }
}

// ============================================================================
std::vector<std::pair<Comm*, ProtocolOSPF*>> OSPF_Area::GetInterfaces()
{
   std::vector<std::pair<Comm*, ProtocolOSPF*>> allAreaInterfaces;

   for (auto protocolPtr : GetProtocols())
   {
      if (protocolPtr->GetRouter()->IsActive())
      {
         for (auto interfacePtr : protocolPtr->GetRouter()->GetInterfaces())
         {
            allAreaInterfaces.push_back(std::make_pair(interfacePtr, protocolPtr));
         }
      }
   }

   return allAreaInterfaces;
}

// ============================================================================
void OSPF_Area::PlatformDeleted(double aSimTime, WsfPlatform* aPlatformPtr)
{
   for (WsfComponentList::RoleIterator<Router> iter{aPlatformPtr->GetComponents()}; !iter.AtEnd(); ++iter)
   {
      auto protocolPtr = (*iter)->GetComponents().GetComponent<ProtocolOSPF>();
      if (protocolPtr)
      {
         // Remove the protocol from those associated with the area.
         auto protocolIt = std::find(std::begin(mProtocols), std::end(mProtocols), protocolPtr);
         if (protocolIt != std::end(mProtocols))
         {
            mProtocols.erase(protocolIt);
         }

         // Remove any DR/BDR references
         for (auto entryIt = std::begin(mMapDR); entryIt != std::end(mMapDR);)
         {
            if (entryIt->second == protocolPtr)
            {
               for (auto interfacePtr : (*iter)->GetInterfaces())
               {
                  AddRemovedInterfaceDR(interfacePtr);
               }

               entryIt = mMapDR.erase(entryIt);
            }
            else
            {
               ++entryIt;
            }
         }

         for (auto entryIt = std::begin(mMapBDR); entryIt != std::end(mMapBDR);)
         {
            if (entryIt->second == protocolPtr)
            {
               for (auto interfacePtr : (*iter)->GetInterfaces())
               {
                  AddRemovedInterfaceBDR(interfacePtr);
               }

               entryIt = mMapBDR.erase(entryIt);
            }
            else
            {
               ++entryIt;
            }
         }

         SetNetworks();
      }
   }
}

// ============================================================================
void OSPF_Area::InitializeCallbacks(WsfSimulation& aSim)
{
   mCallbacks.Add(aSim.GetCommNetworkManager()->CommFrameworkPlatformDeleted.Connect(&OSPF_Area::PlatformDeleted, this));
}

// ============================================================================
void OSPF_Area::AddRemovedInterfaceDR(Comm* aInterfacePtr)
{
   auto network = aInterfacePtr->GetNetwork();
   auto it      = mRemovedInterfacesDR.find(network);

   if (it != std::end(mRemovedInterfacesDR))
   {
      auto addressIt = std::find(std::begin(it->second), std::end(it->second), aInterfacePtr->GetAddress());
      if (addressIt == std::end(it->second))
      {
         it->second.push_back(aInterfacePtr->GetAddress());
      }
   }
   else
   {
      mRemovedInterfacesDR[network] = std::vector<Address>{aInterfacePtr->GetAddress()};
   }
}

// ============================================================================
void OSPF_Area::AddRemovedInterfaceBDR(Comm* aInterfacePtr)
{
   auto network = aInterfacePtr->GetNetwork();
   auto it      = mRemovedInterfacesBDR.find(network);

   if (it != std::end(mRemovedInterfacesBDR))
   {
      auto addressIt = std::find(std::begin(it->second), std::end(it->second), aInterfacePtr->GetAddress());
      if (addressIt == std::end(it->second))
      {
         it->second.push_back(aInterfacePtr->GetAddress());
      }
   }
   else
   {
      mRemovedInterfacesBDR[network] = std::vector<Address>{aInterfacePtr->GetAddress()};
   }
}

} // namespace router
} // namespace comm
} // namespace wsf
