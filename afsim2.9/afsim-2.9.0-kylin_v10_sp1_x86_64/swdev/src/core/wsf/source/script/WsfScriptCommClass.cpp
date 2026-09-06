// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "script/WsfScriptCommClass.hpp"

#include "UtLog.hpp"
#include "WsfComm.hpp"
#include "WsfCommAddress.hpp"
#include "WsfCommComponentHW.hpp"
#include "WsfCommGraph.hpp"
#include "WsfCommMedium.hpp"
#include "WsfCommMediumContainer.hpp"
#include "WsfCommNetworkLayer.hpp"
#include "WsfCommNetworkManager.hpp"
#include "WsfCommProtocolInterface.hpp"
#include "WsfCommRouter.hpp"
#include "WsfCommTypes.hpp"
#include "WsfCommUtil.hpp"
#include "WsfCommandChain.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "script/WsfScriptCommAddress.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptDefs.hpp"

// ============================================================================
WsfScriptCommClass::WsfScriptCommClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptArticulatedPartClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfComm");

   AddStaticMethod(ut::make_unique<IsA_TypeOf>());
   AddStaticMethod(ut::make_unique<GetComm>());

   AddMethod(ut::make_unique<TurnOff>());
   AddMethod(ut::make_unique<TurnOn>());

   AddMethod(ut::make_unique<NetworkName>());
   AddMethod(ut::make_unique<GetAddress>());
   AddMethod(ut::make_unique<GetRouter>());

   AddMethod(ut::make_unique<GetMedium>());

   // Connection methods
   AddMethod(ut::make_unique<IsConnectedTo_1>("IsConnectedTo"));
   AddMethod(ut::make_unique<IsConnectedTo_2>("IsConnectedTo"));
   AddMethod(ut::make_unique<PathExistsTo_1>("PathExistsTo"));
   AddMethod(ut::make_unique<PathExistsTo_2>("PathExistsTo"));
   AddMethod(ut::make_unique<IsConnectionEnabled_1>("IsConnectionEnabled"));
   AddMethod(ut::make_unique<IsConnectionEnabled_2>("IsConnectionEnabled"));
   AddMethod(ut::make_unique<EnableConnection_1>("EnableConnection"));
   AddMethod(ut::make_unique<EnableConnection_2>("EnableConnection"));
   AddMethod(ut::make_unique<DisableConnection_1>("DisableConnection"));
   AddMethod(ut::make_unique<DisableConnection_2>("DisableConnection"));
   AddMethod(ut::make_unique<AddConnection_1>("AddConnection"));
   AddMethod(ut::make_unique<AddConnection_2>("AddConnection"));
   AddMethod(ut::make_unique<RemoveConnection_1>("RemoveConnection"));
   AddMethod(ut::make_unique<RemoveConnection_2>("RemoveConnection"));

   // Transmission methods
   AddMethod(ut::make_unique<SendMessage_1>("SendMessage"));
   AddMethod(ut::make_unique<SendMessage_2>("SendMessage"));
   AddMethod(ut::make_unique<SendMessage_3>("SendMessage"));
   AddMethod(ut::make_unique<SendMessageToCommander_1>("SendMessageToCommander"));
   AddMethod(ut::make_unique<SendMessageToCommander_2>("SendMessageToCommander"));
   AddMethod(ut::make_unique<SendMessageToPeers>());
   AddMethod(ut::make_unique<SendMessageToSubordinates>());

   AddMethod(ut::make_unique<CanSendTo>());

   AddMethod(ut::make_unique<Xmtr>());
   AddMethod(ut::make_unique<XmtrCount>());

   AddMethod(ut::make_unique<Rcvr>());
   AddMethod(ut::make_unique<RcvrCount>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptCommClass, WsfScriptCommClass, IsA_TypeOf, 2, "bool", "string, string")
{
   bool             isA_TypeOf = false;
   wsf::comm::Comm* objectPtr  = wsf::comm::Types::Get(*SCENARIO).Find(aVarArgs[0].GetString());
   if (objectPtr != nullptr)
   {
      isA_TypeOf = objectPtr->IsA_TypeOf(aVarArgs[1].GetString());
   }
   aReturnVal.SetBool(isA_TypeOf);
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommClass, WsfScriptCommClass, GetComm, 1, "WsfComm", "WsfAddress")
{
   WSF_SCRIPT_WARN_INIT_NOT_INIT2
   auto addressPtr        = static_cast<wsf::comm::Address*>(aVarArgs[0].GetPointer()->GetAppObject());
   auto networkManagerPtr = SIMULATION->GetCommNetworkManager();

   auto commPtr = networkManagerPtr->GetComm(*addressPtr);
   aReturnVal.SetPointer(new UtScriptRef(commPtr, aObjectClassPtr));
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD_NOCHECK(WsfScriptCommClass, wsf::comm::Comm, TurnOff, 0, "bool", "")
{
   bool ok = false;
   if (aObjectPtr != nullptr)
   {
      ok = SIMULATION->TurnPartOff(TIME_NOW, aObjectPtr);
   }
   aReturnVal.SetBool(ok);
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD_NOCHECK(WsfScriptCommClass, wsf::comm::Comm, TurnOn, 0, "bool", "")
{
   bool ok = false;
   if (aObjectPtr != nullptr)
   {
      ok = SIMULATION->TurnPartOn(TIME_NOW, aObjectPtr);
   }
   aReturnVal.SetBool(ok);
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommClass, wsf::comm::Comm, NetworkName, 0, "string", "")
{
   WSF_SCRIPT_WARN_INIT_NOT_INIT2
   aReturnVal.SetString(aObjectPtr->GetNetwork());
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommClass, wsf::comm::Comm, GetAddress, 0, "WsfAddress", "")
{
   WSF_SCRIPT_WARN_INIT_NOT_INIT2
   const wsf::comm::Address& address = aObjectPtr->GetAddress();
   aReturnVal.SetPointer(WsfScriptCommAddressClass::Create(address));
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommClass, wsf::comm::Comm, GetRouter, 0, "WsfCommRouter", "")
{
   wsf::comm::router::Router* routerPtr = aObjectPtr->GetRouter();
   aReturnVal.SetPointer(new UtScriptRef(routerPtr, aReturnClassPtr));
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommClass, wsf::comm::Comm, GetMedium, 0, "WsfCommMedium", "")
{
   auto mediumContainer                 = wsf::comm::medium::ContainerComponent<wsf::comm::Comm>::Find(*aObjectPtr);
   wsf::comm::medium::Medium* mediumPtr = nullptr;
   if (mediumContainer)
   {
      mediumPtr = mediumContainer->GetMedium();
   }

   aReturnVal.SetPointer(UtScriptRef::Ref(mediumPtr, aReturnClassPtr));
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommClass, wsf::comm::Comm, IsConnectedTo_1, 1, "bool", "WsfAddress")
{
   WSF_SCRIPT_WARN_INIT_NOT_INIT2
   bool isConnected       = false;
   auto networkManagerPtr = SIMULATION->GetCommNetworkManager();
   auto addressPtr        = static_cast<wsf::comm::Address*>(aVarArgs[0].GetPointer()->GetAppObject());

   auto edgePtr = networkManagerPtr->GetGraph().FindEdge(aObjectPtr->GetAddress(), *addressPtr);
   if (edgePtr)
   {
      isConnected = true;
   }

   aReturnVal.SetBool(isConnected);
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommClass, wsf::comm::Comm, IsConnectedTo_2, 2, "bool", "WsfAddress, WsfAddress")
{
   WSF_SCRIPT_WARN_INIT_NOT_INIT2
   bool isConnected       = false;
   auto networkManagerPtr = SIMULATION->GetCommNetworkManager();

   auto sourceAddressPtr      = static_cast<wsf::comm::Address*>(aVarArgs[0].GetPointer()->GetAppObject());
   auto destinationAddressPtr = static_cast<wsf::comm::Address*>(aVarArgs[1].GetPointer()->GetAppObject());

   auto edgePtr = networkManagerPtr->GetGraph().FindEdge(*sourceAddressPtr, *destinationAddressPtr);
   if (edgePtr)
   {
      isConnected = true;
   }

   aReturnVal.SetBool(isConnected);
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommClass, wsf::comm::Comm, PathExistsTo_1, 1, "bool", "WsfAddress")
{
   WSF_SCRIPT_WARN_INIT_NOT_INIT2
   auto networkManagerPtr = SIMULATION->GetCommNetworkManager();

   const auto& sourceAddress         = aObjectPtr->GetAddress();
   auto        destinationAddressPtr = static_cast<wsf::comm::Address*>(aVarArgs[0].GetPointer()->GetAppObject());

   bool pathExists = networkManagerPtr->PathExists(sourceAddress, *destinationAddressPtr);

   aReturnVal.SetBool(pathExists);
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommClass, wsf::comm::Comm, PathExistsTo_2, 2, "bool", "WsfAddress, WsfAddress")
{
   WSF_SCRIPT_WARN_INIT_NOT_INIT2
   auto networkManagerPtr = SIMULATION->GetCommNetworkManager();

   auto sourceAddressPtr      = static_cast<wsf::comm::Address*>(aVarArgs[0].GetPointer()->GetAppObject());
   auto destinationAddressPtr = static_cast<wsf::comm::Address*>(aVarArgs[1].GetPointer()->GetAppObject());

   bool pathExists = networkManagerPtr->PathExists(*sourceAddressPtr, *destinationAddressPtr);

   aReturnVal.SetBool(pathExists);
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommClass, wsf::comm::Comm, IsConnectionEnabled_1, 1, "bool", "WsfAddress")
{
   WSF_SCRIPT_WARN_INIT_NOT_INIT2
   bool isEnabled         = false;
   auto networkManagerPtr = SIMULATION->GetCommNetworkManager();
   auto addressPtr        = static_cast<wsf::comm::Address*>(aVarArgs[0].GetPointer()->GetAppObject());

   auto edgePtr = networkManagerPtr->GetGraph().FindEdge(aObjectPtr->GetAddress(), *addressPtr);
   if (edgePtr && edgePtr->IsEnabled())
   {
      isEnabled = true;
   }

   aReturnVal.SetBool(isEnabled);
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommClass, wsf::comm::Comm, IsConnectionEnabled_2, 2, "bool", "WsfAddress, WsfAddress")
{
   WSF_SCRIPT_WARN_INIT_NOT_INIT2
   bool isEnabled         = false;
   auto networkManagerPtr = SIMULATION->GetCommNetworkManager();

   auto sourceAddressPtr      = static_cast<wsf::comm::Address*>(aVarArgs[0].GetPointer()->GetAppObject());
   auto destinationAddressPtr = static_cast<wsf::comm::Address*>(aVarArgs[0].GetPointer()->GetAppObject());

   auto edgePtr = networkManagerPtr->GetGraph().FindEdge(*sourceAddressPtr, *destinationAddressPtr);
   if (edgePtr && edgePtr->IsEnabled())
   {
      isEnabled = true;
   }

   aReturnVal.SetBool(isEnabled);
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommClass, wsf::comm::Comm, EnableConnection_1, 1, "bool", "WsfAddress")
{
   WSF_SCRIPT_WARN_INIT_NOT_INIT2
   auto networkManagerPtr = SIMULATION->GetCommNetworkManager();
   auto addressPtr        = static_cast<wsf::comm::Address*>(aVarArgs[0].GetPointer()->GetAppObject());

   auto result = networkManagerPtr->EnableConnection(TIME_NOW, aObjectPtr->GetAddress(), *addressPtr);

   aReturnVal.SetBool(result);
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommClass, wsf::comm::Comm, EnableConnection_2, 2, "bool", "WsfAddress, WsfAddress")
{
   WSF_SCRIPT_WARN_INIT_NOT_INIT2
   auto networkManagerPtr     = SIMULATION->GetCommNetworkManager();
   auto sourceAddressPtr      = static_cast<wsf::comm::Address*>(aVarArgs[0].GetPointer()->GetAppObject());
   auto destinationAddressPtr = static_cast<wsf::comm::Address*>(aVarArgs[1].GetPointer()->GetAppObject());

   auto result = networkManagerPtr->EnableConnection(TIME_NOW, *sourceAddressPtr, *destinationAddressPtr);

   aReturnVal.SetBool(result);
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommClass, wsf::comm::Comm, DisableConnection_1, 1, "bool", "WsfAddress")
{
   WSF_SCRIPT_WARN_INIT_NOT_INIT2
   auto networkManagerPtr = SIMULATION->GetCommNetworkManager();
   auto addressPtr        = static_cast<wsf::comm::Address*>(aVarArgs[0].GetPointer()->GetAppObject());

   auto result = networkManagerPtr->DisableConnection(TIME_NOW, aObjectPtr->GetAddress(), *addressPtr);

   aReturnVal.SetBool(result);
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommClass, wsf::comm::Comm, DisableConnection_2, 2, "bool", "WsfAddress, WsfAddress")
{
   WSF_SCRIPT_WARN_INIT_NOT_INIT2
   auto networkManagerPtr     = SIMULATION->GetCommNetworkManager();
   auto sourceAddressPtr      = static_cast<wsf::comm::Address*>(aVarArgs[0].GetPointer()->GetAppObject());
   auto destinationAddressPtr = static_cast<wsf::comm::Address*>(aVarArgs[1].GetPointer()->GetAppObject());

   auto result = networkManagerPtr->DisableConnection(TIME_NOW, *sourceAddressPtr, *destinationAddressPtr);

   aReturnVal.SetBool(result);
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommClass, wsf::comm::Comm, AddConnection_1, 1, "bool", "WsfAddress")
{
   WSF_SCRIPT_WARN_INIT_NOT_INIT2
   bool        isConnected           = false;
   auto        networkManagerPtr     = SIMULATION->GetCommNetworkManager();
   const auto& sourceAddress         = aObjectPtr->GetAddress();
   auto        destinationAddressPtr = static_cast<wsf::comm::Address*>(aVarArgs[0].GetPointer()->GetAppObject());

   auto commPtr = networkManagerPtr->GetComm(*destinationAddressPtr);
   if (commPtr)
   {
      //! If both comms are members of the same network, we use the network API
      //! to enable the connection. This allows the network to either accept or
      //! reject the connection based off its implementation, topology, etc.
      const std::string& senderNetworkName   = aObjectPtr->GetNetwork();
      const std::string& receiverNetworkName = commPtr->GetNetwork();
      if (senderNetworkName == receiverNetworkName)
      {
         auto networkPtr = networkManagerPtr->GetNetwork(senderNetworkName);
         if (!networkPtr)
         {
            aReturnVal.SetBool(false);
            return;
         }

         isConnected = networkPtr->AddLink(sourceAddress, *destinationAddressPtr, *SIMULATION);
      }
      else
      {
         //! This is an inter-network connection. Simply add the connection between
         //! the members.
         isConnected = networkManagerPtr->AddConnection(TIME_NOW, sourceAddress, *destinationAddressPtr);
      }
   }

   aReturnVal.SetBool(isConnected);
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommClass, wsf::comm::Comm, AddConnection_2, 2, "bool", "WsfAddress, WsfAddress")
{
   WSF_SCRIPT_WARN_INIT_NOT_INIT2
   bool isConnected           = false;
   auto networkManagerPtr     = SIMULATION->GetCommNetworkManager();
   auto sourceAddressPtr      = static_cast<wsf::comm::Address*>(aVarArgs[0].GetPointer()->GetAppObject());
   auto destinationAddressPtr = static_cast<wsf::comm::Address*>(aVarArgs[1].GetPointer()->GetAppObject());

   auto sourceCommPtr      = networkManagerPtr->GetComm(*sourceAddressPtr);
   auto destinationCommPtr = networkManagerPtr->GetComm(*destinationAddressPtr);
   if (sourceCommPtr && destinationCommPtr)
   {
      //! If both comms are members of the same network, we use the network API
      //! to enable the connection. This allows the network to either accept or
      //! reject the connection based off its implementation, topology, etc.
      const std::string& senderNetworkName   = sourceCommPtr->GetNetwork();
      const std::string& receiverNetworkName = destinationCommPtr->GetNetwork();
      if (senderNetworkName == receiverNetworkName)
      {
         auto networkPtr = networkManagerPtr->GetNetwork(senderNetworkName);
         if (!networkPtr)
         {
            aReturnVal.SetBool(false);
            return;
         }

         isConnected = networkPtr->AddLink(*sourceAddressPtr, *destinationAddressPtr, *SIMULATION);
      }
      else
      {
         //! This is an inter-network connection. Simply add the connection between
         //! the members.
         isConnected = networkManagerPtr->AddConnection(TIME_NOW, *sourceAddressPtr, *destinationAddressPtr);
      }
   }

   aReturnVal.SetBool(isConnected);
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommClass, wsf::comm::Comm, RemoveConnection_1, 1, "bool", "WsfAddress")
{
   WSF_SCRIPT_WARN_INIT_NOT_INIT2
   bool isRemoved             = false;
   auto networkManagerPtr     = SIMULATION->GetCommNetworkManager();
   auto destinationAddressPtr = static_cast<wsf::comm::Address*>(aVarArgs[0].GetPointer()->GetAppObject());

   auto sourceCommPtr      = aObjectPtr;
   auto destinationCommPtr = networkManagerPtr->GetComm(*destinationAddressPtr);
   if (sourceCommPtr && destinationCommPtr)
   {
      //! If both comms are members of the same network, we use the network API
      //! to enable the connection. This allows the network to either accept or
      //! reject the connection based off its implementation, topology, etc.
      const std::string& senderNetworkName   = sourceCommPtr->GetNetwork();
      const std::string& receiverNetworkName = destinationCommPtr->GetNetwork();
      if (senderNetworkName == receiverNetworkName)
      {
         auto networkPtr = networkManagerPtr->GetNetwork(senderNetworkName);
         if (!networkPtr)
         {
            aReturnVal.SetBool(false);
            return;
         }

         isRemoved = networkPtr->RemoveLink(sourceCommPtr->GetAddress(), *destinationAddressPtr, *SIMULATION);
      }
      else
      {
         //! This is an inter-network connection. Simply remove the connection between
         //! the members.
         isRemoved = networkManagerPtr->RemoveConnection(TIME_NOW, sourceCommPtr->GetAddress(), *destinationAddressPtr);
      }
   }

   aReturnVal.SetBool(isRemoved);
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommClass, wsf::comm::Comm, RemoveConnection_2, 2, "bool", "WsfAddress, WsfAddress")
{
   WSF_SCRIPT_WARN_INIT_NOT_INIT2
   bool isRemoved             = false;
   auto networkManagerPtr     = SIMULATION->GetCommNetworkManager();
   auto sourceAddressPtr      = static_cast<wsf::comm::Address*>(aVarArgs[0].GetPointer()->GetAppObject());
   auto destinationAddressPtr = static_cast<wsf::comm::Address*>(aVarArgs[1].GetPointer()->GetAppObject());

   auto sourceCommPtr      = networkManagerPtr->GetComm(*sourceAddressPtr);
   auto destinationCommPtr = networkManagerPtr->GetComm(*destinationAddressPtr);
   if (sourceCommPtr && destinationCommPtr)
   {
      //! If both comms are members of the same network, we use the network API
      //! to enable the connection. This allows the network to either accept or
      //! reject the connection based off its implementation, topology, etc.
      const std::string& senderNetworkName   = sourceCommPtr->GetNetwork();
      const std::string& receiverNetworkName = destinationCommPtr->GetNetwork();
      if (senderNetworkName == receiverNetworkName)
      {
         auto networkPtr = networkManagerPtr->GetNetwork(senderNetworkName);
         if (!networkPtr)
         {
            aReturnVal.SetBool(false);
            return;
         }

         isRemoved = networkPtr->RemoveLink(*sourceAddressPtr, *destinationAddressPtr, *SIMULATION);
      }
      else
      {
         //! This is an inter-network connection. Simply add the connection between
         //! the members.
         isRemoved = networkManagerPtr->RemoveConnection(TIME_NOW, *sourceAddressPtr, *destinationAddressPtr);
      }
   }

   aReturnVal.SetBool(isRemoved);
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommClass, wsf::comm::Comm, SendMessage_1, 2, "void", "WsfMessage, WsfComm")
{
   WSF_SCRIPT_WARN_INIT
   auto   msgPtr  = aVarArgs[0].GetPointer()->GetAppObject<WsfMessage>();
   auto   commPtr = aVarArgs[1].GetPointer()->GetAppObject<wsf::comm::Comm>();
   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   aObjectPtr->Send(simTime, ut::clone(msgPtr), commPtr->GetAddress());
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommClass, wsf::comm::Comm, SendMessage_2, 3, "void", "WsfMessage, string, string")
{
   WSF_SCRIPT_WARN_INIT
   auto         msgPtr         = aVarArgs[0].GetPointer()->GetAppObject<WsfMessage>();
   WsfStringId  platformNameId = aVarArgs[1].GetString();
   auto         commName       = aVarArgs[2].GetString();
   WsfPlatform* platformPtr    = SIMULATION->GetPlatformByName(platformNameId);
   if (platformPtr)
   {
      auto commPtr = platformPtr->GetComponent<wsf::comm::Comm>(commName);
      if (commPtr)
      {
         aObjectPtr->Send(TIME_NOW, ut::clone(msgPtr), commPtr->GetAddress());
         return;
      }
   }
   auto out = ut::log::warning() << "Unable to send message to comm.";
   out.AddNote() << "Comm: " << platformNameId << "." << commName;
   out.AddNote() << "Platform or comm does not exist.";
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommClass, wsf::comm::Comm, SendMessage_3, 2, "void", "WsfMessage, WsfAddress")
{
   WSF_SCRIPT_WARN_INIT
   auto msgPtr     = aVarArgs[0].GetPointer()->GetAppObject<WsfMessage>();
   auto addressPtr = aVarArgs[1].GetPointer()->GetAppObject<wsf::comm::Address>();

   if (!addressPtr->IsNull())
   {
      aObjectPtr->Send(TIME_NOW, ut::clone(msgPtr), *addressPtr);
   }
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommClass, wsf::comm::Comm, SendMessageToCommander_1, 2, "void", "string, WsfMessage")
{
   WSF_SCRIPT_WARN_INIT
   auto msgPtr = aVarArgs[1].GetPointer()->GetAppObject<WsfMessage>();
   // Argument 1: string aCommandChainName (if an empty string then use the default name)
   WsfStringId chainNameId = WsfStringId(aVarArgs[0].GetString());
   if (!chainNameId)
   {
      chainNameId = WsfCommandChain::GetDefaultName();
   }
   WsfCommandChain* chainPtr    = aObjectPtr->GetPlatform()->template GetComponent<WsfCommandChain>(chainNameId);
   WsfPlatform*     platformPtr = nullptr;
   if (chainPtr != nullptr)
   {
      platformPtr = chainPtr->GetCommander();
   }
   if (platformPtr != nullptr)
   {
      for (WsfComponentList::RoleIterator<wsf::comm::Comm> it(*platformPtr); !it.AtEnd(); ++it)
      {
         auto commRcvrPtr = *it;
         if (commRcvrPtr->IsTurnedOn() && commRcvrPtr->CanReceive())
         {
            msgPtr->SetGroup(chainNameId);
            aObjectPtr->Send(TIME_NOW, ut::clone(msgPtr), commRcvrPtr->GetAddress());
         }
      }
   }
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommClass, wsf::comm::Comm, SendMessageToCommander_2, 1, "void", "WsfMessage")
{
   WSF_SCRIPT_WARN_INIT
   auto msgPtr = aVarArgs[0].GetPointer()->GetAppObject<WsfMessage>();
   // Use default command chain
   WsfStringId      chainNameId(WsfCommandChain::GetDefaultNameId());
   WsfCommandChain* chainPtr    = aObjectPtr->GetPlatform()->template GetComponent<WsfCommandChain>(chainNameId);
   WsfPlatform*     platformPtr = nullptr;
   if (chainPtr != nullptr)
   {
      platformPtr = chainPtr->GetCommander();
   }
   if (platformPtr != nullptr)
   {
      for (WsfComponentList::RoleIterator<wsf::comm::Comm> it(*platformPtr); !it.AtEnd(); ++it)
      {
         auto commRcvrPtr = *it;
         if (commRcvrPtr->IsTurnedOn() && commRcvrPtr->CanReceive())
         {
            msgPtr->SetGroup(chainNameId);
            aObjectPtr->Send(TIME_NOW, ut::clone(msgPtr), commRcvrPtr->GetAddress());
         }
      }
   }
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommClass, wsf::comm::Comm, SendMessageToPeers, 2, "void", "string, WsfMessage")
{
   WSF_SCRIPT_WARN_INIT
   // Argument 1: string aCommandChainName (if an empty string then use the default name)
   WsfStringId chainNameId = WsfStringId(aVarArgs[0].GetString());
   if (!chainNameId)
   {
      chainNameId = WsfCommandChain::GetDefaultNameId();
   }
   // Argument 2: Message to send
   auto msgPtr = aVarArgs[1].GetPointer()->GetAppObject<WsfMessage>();

   WsfCommandChain* chainPtr = aObjectPtr->GetPlatform()->template GetComponent<WsfCommandChain>(chainNameId);
   if (chainPtr != nullptr)
   {
      std::vector<WsfPlatform*> recipients;
      wsf::comm::util::SelectRecipients(chainPtr, wsf::comm::util::cPEERS, recipients);
      for (auto platform : recipients)
      {
         for (WsfComponentList::RoleIterator<wsf::comm::Comm> it(*platform); !it.AtEnd(); ++it)
         {
            auto commRcvrPtr = *it;
            if (commRcvrPtr->IsTurnedOn() && commRcvrPtr->CanReceive())
            {
               msgPtr->SetGroup(chainNameId);
               aObjectPtr->Send(TIME_NOW, ut::clone(msgPtr), commRcvrPtr->GetAddress());
            }
         }
      }
   }
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommClass, wsf::comm::Comm, SendMessageToSubordinates, 2, "void", "string, WsfMessage")
{
   WSF_SCRIPT_WARN_INIT
   // Argument 1: string aCommandChainName (if an empty string then use the default name)
   WsfStringId chainNameId = WsfStringId(aVarArgs[0].GetString());
   if (!chainNameId)
   {
      chainNameId = WsfCommandChain::GetDefaultNameId();
   }
   // Argument 2: Message to send
   auto msgPtr = aVarArgs[1].GetPointer()->GetAppObject<WsfMessage>();

   WsfCommandChain* chainPtr = aObjectPtr->GetPlatform()->template GetComponent<WsfCommandChain>(chainNameId);
   if (chainPtr != nullptr)
   {
      std::vector<WsfPlatform*> recipients;
      wsf::comm::util::SelectRecipients(chainPtr, wsf::comm::util::cSUBORDINATES, recipients);
      for (auto platform : recipients)
      {
         for (WsfComponentList::RoleIterator<wsf::comm::Comm> it(*platform); !it.AtEnd(); ++it)
         {
            auto commRcvrPtr = *it;
            if (commRcvrPtr->IsTurnedOn() && commRcvrPtr->CanReceive())
            {
               msgPtr->SetGroup(chainNameId);
               aObjectPtr->Send(TIME_NOW, ut::clone(*msgPtr), commRcvrPtr->GetAddress());
            }
         }
      }
   }
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommClass, wsf::comm::Comm, CanSendTo, 1, "bool", "WsfComm")
{
   auto targetInterfacePtr = aVarArgs[0].GetPointer()->GetAppObject<wsf::comm::Comm>();
   bool canSendTo          = aObjectPtr->CanSendTo(TIME_NOW, targetInterfacePtr);

   aReturnVal.SetBool(canSendTo);
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommClass, wsf::comm::Comm, Xmtr, 1, "WsfEM_Xmtr", "int")
{
   int                     index      = aVarArgs[0].GetInt();
   wsf::comm::ComponentHW* commHW_Ptr = wsf::comm::ComponentHW::Find(*aObjectPtr);
   WsfEM_Xmtr*             xmtrPtr    = nullptr;
   if (commHW_Ptr != nullptr)
   {
      xmtrPtr = &commHW_Ptr->GetEM_Xmtr(index);
   }

   aReturnVal.SetPointer(new UtScriptRef(xmtrPtr, aReturnClassPtr));
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommClass, wsf::comm::Comm, XmtrCount, 0, "int", "")
{
   wsf::comm::ComponentHW* commHW_Ptr = wsf::comm::ComponentHW::Find(*aObjectPtr);
   int                     xmtrCnt    = 0;
   ;
   if (commHW_Ptr != nullptr)
   {
      xmtrCnt = commHW_Ptr->GetEM_XmtrCount();
   }
   aReturnVal.SetInt(xmtrCnt);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptCommClass, wsf::comm::Comm, Rcvr, 1, "WsfEM_Rcvr", "int")
{
   auto        commHW_Ptr = wsf::comm::ComponentHW::Find(*aObjectPtr);
   WsfEM_Rcvr* rcvrPtr    = nullptr;
   if (commHW_Ptr != nullptr)
   {
      int index = aVarArgs[0].GetInt();
      rcvrPtr   = &commHW_Ptr->GetEM_Rcvr(ut::safe_cast<unsigned>(index));
   }

   aReturnVal.SetPointer(UtScriptRef::Ref(rcvrPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptCommClass, wsf::comm::Comm, RcvrCount, 0, "int", "")
{
   auto commHW_Ptr = wsf::comm::ComponentHW::Find(*aObjectPtr);
   int  rcvrCnt    = 0;
   if (commHW_Ptr != nullptr)
   {
      rcvrCnt = ut::cast_to_int(commHW_Ptr->GetEM_RcvrCount());
   }
   aReturnVal.SetInt(rcvrCnt);
}

WsfScriptCommInteractionClass::WsfScriptCommInteractionClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptEM_InteractionClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfCommInteraction");
}
