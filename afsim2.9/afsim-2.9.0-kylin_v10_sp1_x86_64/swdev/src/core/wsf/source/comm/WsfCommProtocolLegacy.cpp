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

#include "WsfCommProtocolLegacy.hpp"

#include "WsfComm.hpp"
#include "WsfCommMessage.hpp"
#include "WsfCommNetworkManager.hpp"
#include "WsfCommRoutingAlgorithmLibrary.hpp"
#include "WsfSimulation.hpp"


namespace wsf
{
namespace comm
{
namespace router
{

// =================================================================================================
ProtocolLegacy* ProtocolLegacy::Find(const Router& aParent) // static
{
   auto protocolPtr = aParent.GetComponents().GetComponent<ProtocolLegacy>();
   return protocolPtr;
}

// =================================================================================================
ProtocolLegacy* ProtocolLegacy::FindOrCreate(Router& aParent) // static
{
   auto protocolPtr = Find(aParent);
   if (!protocolPtr)
   {
      protocolPtr = new ProtocolLegacy;
      protocolPtr->SetName("protocol_legacy");
      aParent.GetComponents().AddComponent(protocolPtr);
   }

   return protocolPtr;
}

// =================================================================================================
WsfComponent* ProtocolLegacy::CloneComponent() const
{
   return new ProtocolLegacy(*this);
}

// =================================================================================================
const int* ProtocolLegacy::GetComponentRoles() const
{
   static const int roles[] = {cWSF_COMPONENT_COMM_ROUTER_PROTOCOL_LEGACY,
                               cWSF_COMPONENT_COMM_ROUTER_PROTOCOL,
                               cWSF_COMPONENT_NULL};

   return roles;
}

// =================================================================================================
ProtocolLegacy* ProtocolLegacy::Clone() const
{
   return new ProtocolLegacy(*this);
}

// =================================================================================================
void* ProtocolLegacy::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_COMM_ROUTER_PROTOCOL_LEGACY)
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
int ProtocolLegacy::GetPriority() const
{
   return static_cast<int>(ProtocolPriority::cLEGACY);
}

// =================================================================================================
bool ProtocolLegacy::Initialize(double aSimTime)
{
   //! Set the graph now, if not already set.
   if (!GetGraph())
   {
      mGraph = &GetSimulation()->GetCommNetworkManager()->GetGraph();
   }

   return true;
}

// =================================================================================================
void ProtocolLegacy::Setup()
{
   mGraph = &GetSimulation()->GetCommNetworkManager()->GetGraph();
}

// =================================================================================================
bool ProtocolLegacy::Send(double aSimTime, Router::SendData& aData)
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

// =================================================================================================
std::vector<Address> ProtocolLegacy::Routing(double         aSimTime,
                                             const Address& aSendingInterface,
                                             size_t&        aHopNumber,
                                             double&        aCost,
                                             const Message& aMessage)
{
   std::vector<Address> path;
   LeastHops            leastHopsAlgorithm;

   //! Short circuit immediately if the incremented number of hops (possible due to recursive calls)
   //! exceeds the allowable number of hops set by the originating router call.
   if (aHopNumber > GetRouter()->GetHopLimit())
   {
      aHopNumber = 0;
      aCost      = 0.0;
      return path;
   }

   double cost = 0.0;
   auto   pathFound =
      GetGraph()->FindPath(aSendingInterface, aMessage.SourceMessage()->GetDstAddr(), path, cost, &leastHopsAlgorithm);

   if (pathFound)
   {
      aCost += cost;
      aHopNumber += path.size() - 1;

      if (aHopNumber > GetRouter()->GetHopLimit())
      {
         aCost      = 0.0;
         aHopNumber = 0;
         path.clear();
      }
   }
   else
   {
      aCost      = 0.0;
      aHopNumber = 0;
      path.clear();
   }

   return path;
}

// ============================================================================
ScriptProtocolClassLegacy::ScriptProtocolClassLegacy(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : ScriptRouterProtocolClass(aClassName, aTypesPtr)
{
   SetClassName("WsfCommProtocolLegacy");
   mConstructible      = false;
   mCloneable          = false;
   mIsScriptAccessible = true;
}

} // namespace router
} // namespace comm
} // namespace wsf
