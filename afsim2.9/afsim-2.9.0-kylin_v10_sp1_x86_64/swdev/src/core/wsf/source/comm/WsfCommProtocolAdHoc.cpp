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

#include "WsfCommProtocolAdHoc.hpp"

#include "WsfComm.hpp"
#include "WsfCommEvent.hpp"
#include "WsfCommNetworkManager.hpp"
#include "WsfCommObserver.hpp"
#include "WsfCommRoutingAlgorithmLibrary.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"

namespace wsf
{
namespace comm
{
namespace router
{

// =================================================================================================
ProtocolAdHoc* ProtocolAdHoc::Find(const Router& aParent)
{
   auto protocolPtr = aParent.GetComponents().GetComponent<ProtocolAdHoc>();
   return protocolPtr;
}

// =================================================================================================
ProtocolAdHoc* ProtocolAdHoc::FindOrCreate(Router& aParent)
{
   auto protocolPtr = Find(aParent);
   if (!protocolPtr)
   {
      protocolPtr = new ProtocolAdHoc(aParent.GetScenario());
      protocolPtr->SetName("protocol_ad_hoc");
      aParent.GetComponents().AddComponent(protocolPtr);
   }

   return protocolPtr;
}

// =================================================================================================
ProtocolAdHoc::ProtocolAdHoc(const WsfScenario& aScenario)
   : mScenario(aScenario)
   , mGraph(ut::make_unique<graph::Graph>())
   , mContextPtr(new WsfScriptContext(*aScenario.GetScriptContext(), "WsfCommProtocolAdHoc"))
{
   mGraph->SetUserModifiable(true);
}

// =================================================================================================
ProtocolAdHoc::ProtocolAdHoc(const ProtocolAdHoc& aSrc)
   : ProtocolInterface(aSrc)
   , mScenario(aSrc.mScenario)
   , mGraph(ut::make_unique<graph::Graph>())
   , mContextPtr(new WsfScriptContext(*(aSrc.mContextPtr)))
   , mOnCommAddedDelay(aSrc.mOnCommAddedDelay)
   , mOnCommRemovedDelay(aSrc.mOnCommRemovedDelay)
   , mOnConnectionAddedDelay(aSrc.mOnConnectionAddedDelay)
   , mOnConnectionRemovedDelay(aSrc.mOnConnectionRemovedDelay)
   , mOnCommAdded(aSrc.mOnCommAdded)
   , mOnCommRemoved(aSrc.mOnCommRemoved)
   , mOnConnectionAdded(aSrc.mOnConnectionAdded)
   , mOnConnectionRemoved(aSrc.mOnConnectionRemoved)
   , mOnMessageRouting(aSrc.mOnMessageRouting)
{
   mGraph->SetUserModifiable(true);
}

// =================================================================================================
WsfComponent* ProtocolAdHoc::CloneComponent() const
{
   return new ProtocolAdHoc(*this);
}

// =================================================================================================
const int* ProtocolAdHoc::GetComponentRoles() const
{
   static const int roles[] = {cWSF_COMPONENT_COMM_ROUTER_PROTOCOL_AD_HOC,
                               cWSF_COMPONENT_COMM_ROUTER_PROTOCOL,
                               cWSF_COMPONENT_NULL};

   return roles;
}

// =================================================================================================
ProtocolAdHoc* ProtocolAdHoc::Clone() const
{
   return new ProtocolAdHoc(*this);
}

// =================================================================================================
void* ProtocolAdHoc::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_COMM_ROUTER_PROTOCOL_AD_HOC)
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
int ProtocolAdHoc::GetPriority() const
{
   return static_cast<int>(ProtocolPriority::cAD_HOC);
}

// =================================================================================================
bool ProtocolAdHoc::Initialize(double aSimTime)
{
   bool ok = mContextPtr->Initialize(aSimTime, GetRouter()->GetPlatform()->GetScriptContext(), this);

   std::string commAddedName         = "comm_added_delay_time";
   std::string commRemovedName       = "comm_removed_delay_time";
   std::string connectionAddedName   = "connection_added_delay_time";
   std::string connectionRemovedName = "connection_removed_delay_time";

   ok &= mOnCommAddedDelay.Initialize(commAddedName, this, mContextPtr.get(), *GetSimulation());
   ok &= mOnCommRemovedDelay.Initialize(commRemovedName, this, mContextPtr.get(), *GetSimulation());
   ok &= mOnConnectionAddedDelay.Initialize(connectionAddedName, this, mContextPtr.get(), *GetSimulation());
   ok &= mOnConnectionRemovedDelay.Initialize(connectionRemovedName, this, mContextPtr.get(), *GetSimulation());

   return ok;
}

// =================================================================================================
void ProtocolAdHoc::Setup()
{
   InitializeState();
}

// =================================================================================================
bool ProtocolAdHoc::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;
   auto command   = aInput.GetCommand();

   if (command == "comm_added_delay_time")
   {
      mOnCommAddedDelay.ProcessInput(aInput, UtInput::cTIME);
   }
   else if (command == "comm_removed_delay_time")
   {
      mOnCommRemovedDelay.ProcessInput(aInput, UtInput::cTIME);
   }
   else if (command == "connection_added_delay_time")
   {
      mOnConnectionAddedDelay.ProcessInput(aInput, UtInput::cTIME);
   }
   else if (command == "connection_removed_delay_time")
   {
      mOnConnectionRemovedDelay.ProcessInput(aInput, UtInput::cTIME);
   }
   else if (command == "script")
   {
      UtScript*   scriptPtr      = mContextPtr->Compile(aInput, "end_script");
      std::string scriptName     = scriptPtr->GetName();
      bool        validSignature = true;

      if (scriptName == "OnCommAdded")
      {
         validSignature = mContextPtr->ValidateScript(scriptPtr, "bool", "WsfAddress, WsfCommGraph, WsfCommRouter");
         mOnCommAdded   = scriptPtr;
      }
      else if (scriptName == "OnCommRemoved")
      {
         validSignature = mContextPtr->ValidateScript(scriptPtr, "bool", "WsfAddress, WsfCommGraph, WsfCommRouter");
         mOnCommRemoved = scriptPtr;
      }
      else if (scriptName == "OnConnectionAdded")
      {
         validSignature =
            mContextPtr->ValidateScript(scriptPtr, "bool", "WsfAddress, WsfAddress, WsfCommGraph, WsfCommRouter");
         mOnConnectionAdded = scriptPtr;
      }
      else if (scriptName == "OnConnectionRemoved")
      {
         validSignature =
            mContextPtr->ValidateScript(scriptPtr, "bool", "WsfAddress, WsfAddress, WsfCommGraph, WsfCommRouter");
         mOnConnectionRemoved = scriptPtr;
      }
      else if (scriptName == "OnMessageRouting")
      {
         validSignature =
            mContextPtr->ValidateScript(scriptPtr, "WsfAddress", "WsfCommMessage, WsfAddress, WsfCommGraph, WsfCommRouter");
         mOnMessageRouting = scriptPtr;
      }
      else
      {
         // Only allow script method definitions for the above types. Anything
         // else will throw an exception
         throw UtException("Unrecognized script method in " + GetName());
      }

      // Throw an exception if the script signature is incorrect
      if (!validSignature)
      {
         throw UtException("Invalid script return type, argument type, or argument count in ad_hoc protocol: " + GetType());
      }
   }
   else if (mContextPtr->ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

// =================================================================================================
bool ProtocolAdHoc::Send(double aSimTime, Router::SendData& aData)
{
   double curCost = 0.0;
   size_t numHops = 0;
   auto&  message = aData.GetMessages().front();

   auto pathFound = Routing(aSimTime, aData.GetXmtr()->GetAddress(), numHops, curCost, message);

   if (pathFound.size() == 1)
   {
      //! Special case in this protocol for user defined next hop.
      message.SourceMessage()->SetNextHopAddr(pathFound.front());
      aData.SetAbortProcessing(true);
      return true;
   }

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
std::vector<Address> ProtocolAdHoc::Routing(double         aSimTime,
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

   //! If the user has a custom routing algorithm via script, and wants
   //! to route, it will return the next hop as an address. In such
   //! cases, simply return the next hop.
   auto userNextHop = ExecuteOnMessageRouting(aSimTime, aSendingInterface, aMessage);
   if (!userNextHop.IsNull())
   {
      path.push_back(userNextHop);
      aCost      = 0.0;
      aHopNumber = 0;
      return path;
   }
   else if (mOnMessageRouting)
   {
      //! If the custom routing script was defined, but a next hop
      //! was not provided, the user does not want this message
      //! forwarded.
      aCost      = 0.0;
      aHopNumber = 0;
      return path;
   }

   auto pathFound =
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

// =================================================================================================
void ProtocolAdHoc::ExecuteOnCommAdded(double aSimTime, const Address& aComm)
{
   bool addComm = true;

   if (mOnCommAdded)
   {
      UtScriptData       scriptRetVal;
      UtScriptDataPacker scriptArgs(mContextPtr->GetTypes());

      scriptArgs << &aComm << mGraph.get() << GetRouter();
      mContextPtr->ExecuteScript(aSimTime, mOnCommAdded, scriptRetVal, scriptArgs);

      addComm = scriptRetVal.GetBool();
   }

   if (addComm && !mGraph->FindNode(aComm))
   {
      auto nodePtr = mGraph->CreateNode(aComm);

      if (nodePtr)
      {
         // Inform observer.
         auto address = aComm;
         GetRouter()->GetSimulation()->GetCommObserver().CommAddedToLocal(aSimTime, GetRouter(), this, &address);
      }
   }
}

// =================================================================================================
void ProtocolAdHoc::ExecuteOnCommRemoved(double aSimTime, const Address& aComm)
{
   bool removeComm = true;

   if (mOnCommRemoved)
   {
      UtScriptData       scriptRetVal;
      UtScriptDataPacker scriptArgs(mContextPtr->GetTypes());

      scriptArgs << &aComm << mGraph.get() << GetRouter();
      mContextPtr->ExecuteScript(aSimTime, mOnCommRemoved, scriptRetVal, scriptArgs);

      removeComm = scriptRetVal.GetBool();
   }

   if (removeComm && mGraph->FindNode(aComm))
   {
      // Remove any edges associated with this comm.
      auto allEdges = mGraph->GetAllNodeEdges(aComm);
      for (auto edgePtr : allEdges)
      {
         ExecuteOnConnectionRemoved(aSimTime, edgePtr->GetSourceAddress(), edgePtr->GetDestinationAddress());
      }

      auto result = mGraph->RemoveNode(aComm);

      if (result)
      {
         // Inform observer.
         auto address = aComm;
         GetRouter()->GetSimulation()->GetCommObserver().CommRemovedFromLocal(aSimTime, GetRouter(), this, &address);
      }
   }
}

// =================================================================================================
void ProtocolAdHoc::ExecuteOnConnectionAdded(double aSimTime, const Address& aSourceAddress, const Address& aDestinationAddress)
{
   bool addConnection = true;

   if (mOnConnectionAdded)
   {
      UtScriptData       scriptRetVal;
      UtScriptDataPacker scriptArgs(mContextPtr->GetTypes());

      scriptArgs << &aSourceAddress << &aDestinationAddress << mGraph.get() << GetRouter();

      mContextPtr->ExecuteScript(aSimTime, mOnConnectionAdded, scriptRetVal, scriptArgs);

      addConnection = scriptRetVal.GetBool();
   }

   if (addConnection && !mGraph->FindEdge(aSourceAddress, aDestinationAddress))
   {
      auto edgePtr = mGraph->InsertEdge(aSourceAddress, aDestinationAddress, true);

      if (edgePtr)
      {
         // Inform observer.
         auto sourceAddress      = aSourceAddress;
         auto destinationAddress = aDestinationAddress;
         GetRouter()->GetSimulation()->GetCommObserver().LinkAddedToLocal(aSimTime,
                                                                          GetRouter(),
                                                                          this,
                                                                          &sourceAddress,
                                                                          &destinationAddress);
      }
   }
}

// =================================================================================================
void ProtocolAdHoc::ExecuteOnConnectionRemoved(double aSimTime, const Address& aSourceAddress, const Address& aDestinationAddress)
{
   bool removeConnection = true;

   if (mOnConnectionRemoved)
   {
      UtScriptData       scriptRetVal;
      UtScriptDataPacker scriptArgs(mContextPtr->GetTypes());

      scriptArgs << &aSourceAddress << &aDestinationAddress << mGraph.get() << GetRouter();

      mContextPtr->ExecuteScript(aSimTime, mOnConnectionRemoved, scriptRetVal, scriptArgs);

      removeConnection = scriptRetVal.GetBool();
   }

   if (removeConnection && mGraph->FindEdge(aSourceAddress, aDestinationAddress))
   {
      bool result = mGraph->EraseEdge(aSourceAddress, aDestinationAddress);

      if (result)
      {
         // Inform observer.
         auto sourceAddress      = aSourceAddress;
         auto destinationAddress = aDestinationAddress;
         GetRouter()->GetSimulation()->GetCommObserver().LinkRemovedFromLocal(aSimTime,
                                                                              GetRouter(),
                                                                              this,
                                                                              &sourceAddress,
                                                                              &destinationAddress);
      }
   }
}

// =================================================================================================
Address ProtocolAdHoc::ExecuteOnMessageRouting(double aSimTime, const Address& aInterface, const Message& aMessage)
{
   Address address;

   if (mOnMessageRouting)
   {
      UtScriptData       scriptRetVal;
      UtScriptDataPacker scriptArgs(mContextPtr->GetTypes());

      scriptArgs << &aMessage << &aInterface << mGraph.get() << GetRouter();
      mContextPtr->ExecuteScript(aSimTime, mOnMessageRouting, scriptRetVal, scriptArgs);

      address = *scriptRetVal.GetPointer()->GetAppObject<Address>();
   }

   return address;
}

// =================================================================================================
void ProtocolAdHoc::PerfectCommAdded(double aSimTime, Comm* aCommPtr)
{
   const auto& address = aCommPtr->GetAddress();

   if (!ValidNetwork(address))
   {
      return;
   }

   auto delay = mOnCommAddedDelay.Draw();

   if (delay > 0.0)
   {
      auto eventTime = aSimTime + delay;
      GetSimulation()->AddEvent(ut::make_unique<GenericEvent<int>>(eventTime,
                                                                   mEvents,
                                                                   [this, eventTime, address]()
                                                                   { ExecuteOnCommAdded(eventTime, address); }));
   }
   else
   {
      ExecuteOnCommAdded(aSimTime, address);
   }
}

// =================================================================================================
void ProtocolAdHoc::PerfectCommRemoved(double aSimTime, Comm* aCommPtr)
{
   const auto& address = aCommPtr->GetAddress();

   if (!ValidNetwork(address))
   {
      return;
   }

   auto delay = mOnCommRemovedDelay.Draw();

   if (delay > 0.0)
   {
      auto eventTime = aSimTime + delay;
      GetSimulation()->AddEvent(ut::make_unique<GenericEvent<int>>(eventTime,
                                                                   mEvents,
                                                                   [this, eventTime, address]()
                                                                   { ExecuteOnCommRemoved(eventTime, address); }));
   }
   else
   {
      ExecuteOnCommRemoved(aSimTime, address);
   }
}

// =================================================================================================
void ProtocolAdHoc::PerfectConnectionAdded(double aSimTime, const Address& aSourceAddress, const Address& aDestinationAddress)
{
   if (!ValidNetwork(aSourceAddress) && !ValidNetwork(aDestinationAddress))
   {
      return;
   }

   auto delay = mOnConnectionAddedDelay.Draw();
   if (delay > 0.0)
   {
      auto eventTime = aSimTime + delay;
      GetSimulation()->AddEvent(
         ut::make_unique<GenericEvent<int>>(eventTime,
                                            mEvents,
                                            [this, eventTime, aSourceAddress, aDestinationAddress]() {
                                               ExecuteOnConnectionAdded(eventTime, aSourceAddress, aDestinationAddress);
                                            }));
   }
   else
   {
      ExecuteOnConnectionAdded(aSimTime, aSourceAddress, aDestinationAddress);
   }
}

// =================================================================================================
void ProtocolAdHoc::PerfectConnectionRemoved(double aSimTime, const Address& aSourceAddress, const Address& aDestinationAddress)
{
   if (!ValidNetwork(aSourceAddress) && !ValidNetwork(aDestinationAddress))
   {
      return;
   }

   auto delay = mOnConnectionRemovedDelay.Draw();
   if (delay > 0.0)
   {
      auto eventTime = aSimTime + delay;
      GetSimulation()->AddEvent(
         ut::make_unique<GenericEvent<int>>(eventTime,
                                            mEvents,
                                            [this, eventTime, aSourceAddress, aDestinationAddress]() {
                                               ExecuteOnConnectionRemoved(eventTime, aSourceAddress, aDestinationAddress);
                                            }));
   }
   else
   {
      ExecuteOnConnectionRemoved(aSimTime, aSourceAddress, aDestinationAddress);
   }
}

// =================================================================================================
bool ProtocolAdHoc::ValidNetwork(const Address& aAddress) const
{
   const auto& interfaces = GetRouter()->GetInterfaces();
   for (auto interfacePtr : interfaces)
   {
      auto networkPtr = GetSimulation()->GetCommNetworkManager()->GetNetwork(interfacePtr->GetNetwork());
      if (networkPtr->HasMember(aAddress))
      {
         return true;
      }
   }

   return false;
}

// =================================================================================================
void ProtocolAdHoc::InitializeState()
{
   std::vector<std::string> networkNames;

   const auto& interfaces = GetRouter()->GetInterfaces();
   for (auto interfacePtr : interfaces)
   {
      networkNames.push_back(interfacePtr->GetNetwork());
   }

   for (const auto& networkName : networkNames)
   {
      const auto& networkAddresses = GetSimulation()->GetCommNetworkManager()->GetAddressesInNetwork(networkName);
      for (const auto& networkMember : networkAddresses)
      {
         if (!mGraph->FindNode(networkMember))
         {
            mGraph->CreateNode(networkMember);
         }
      }

      for (const auto& networkMember : networkAddresses)
      {
         const auto& outEdges = GetSimulation()->GetCommNetworkManager()->GetGraph().GetOutgoingNodeEdges(networkMember);
         for (const auto& edge : outEdges)
         {
            if (!mGraph->FindEdge(edge->GetSourceAddress(), edge->GetDestinationAddress()))
            {
               mGraph->InsertEdge(edge->GetSourceAddress(), edge->GetDestinationAddress(), edge->IsEnabled());
            }
         }

         const auto& inEdges = GetSimulation()->GetCommNetworkManager()->GetGraph().GetIncomingNodeEdges(networkMember);
         for (const auto& edge : inEdges)
         {
            if (!mGraph->FindEdge(edge->GetSourceAddress(), edge->GetDestinationAddress()))
            {
               mGraph->InsertEdge(edge->GetSourceAddress(), edge->GetDestinationAddress(), edge->IsEnabled());
            }
         }
      }
   }
}

} // namespace router
} // namespace comm
} // namespace wsf
