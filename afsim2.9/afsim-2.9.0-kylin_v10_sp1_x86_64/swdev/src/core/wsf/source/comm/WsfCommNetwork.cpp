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

#include "WsfCommNetwork.hpp"

#include <memory>
#include <sstream>

#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "WsfComm.hpp"
#include "WsfCommNetworkManager.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "script/WsfScriptCommAddress.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptDefs.hpp"


namespace wsf
{
namespace comm
{

// =================================================================================================
Network::Network(const std::string& aNetworkName, const Address& aAddress)
   : WsfObject()
   , mAddress(aAddress)
{
   SetType(aNetworkName);
}

// =================================================================================================
Address Network::GetAddressFromNamedComm(WsfStringId aPlatformName, WsfStringId aCommName, WsfSimulation& aSimulation)
{
   if (aPlatformName.Empty() || aCommName.Empty())
   {
      return Address();
   }

   auto platformPtr = aSimulation.GetPlatformByName(aPlatformName);
   if (!platformPtr)
   {
      return Address();
   }

   auto commPtr = platformPtr->GetComponent<Comm>(aCommName);
   if (!commPtr)
   {
      return Address();
   }

   return commPtr->GetAddress();
}

// =================================================================================================
bool Network::InitializeLinkage(WsfSimulation& aSimulation)
{
   for (auto& names : mNamedList)
   {
      auto address = GetAddressFromNamedComm(names.mPlatformName, names.mCommName, aSimulation);
      if (!address.IsNull())
      {
         if (std::end(mAddressList) == std::find(std::begin(mAddressList), std::end(mAddressList), address))
         {
            mAddressList.push_back(std::move(address));
         }
      }
      else
      {
         auto out = ut::log::warning() << "Address not found for network member.";
         out.AddNote() << "Network: " << GetType();
         out.AddNote() << "Platform: " << names.mPlatformName;
         out.AddNote() << "Comm: " << names.mCommName;
      }
   }

   for (auto& address : mInputAddressList)
   {
      if (aSimulation.GetCommNetworkManager()->GetComm(address))
      {
         if (std::end(mAddressList) == std::find(std::begin(mAddressList), std::end(mAddressList), address))
         {
            mAddressList.push_back(address);
         }
      }
      else
      {
         auto out = ut::log::warning() << "Unassigned address used as network member. Skipping address.";
         out.AddNote() << "Network: " << GetType();
         out.AddNote() << "Address: " << address;
      }
   }

   for (auto& namePair : mNamedLinkList)
   {
      auto sourceAddress = GetAddressFromNamedComm(namePair.first.mPlatformName, namePair.first.mCommName, aSimulation);
      auto destinationAddress =
         GetAddressFromNamedComm(namePair.second.mPlatformName, namePair.second.mCommName, aSimulation);

      if ((!sourceAddress.IsNull()) && (!destinationAddress.IsNull()))
      {
         if (std::end(mAddressLinkList) == std::find(std::begin(mAddressLinkList),
                                                     std::end(mAddressLinkList),
                                                     std::make_pair(sourceAddress, destinationAddress)))
         {
            mAddressLinkList.emplace_back(std::move(sourceAddress), std::move(destinationAddress));
         }
      }
      else
      {
         auto out = ut::log::warning() << "Named Link Members could not be found.";
         out.AddNote() << "Network: " << GetType();
         out.AddNote() << "Source Platform: " << namePair.first.mPlatformName;
         out.AddNote() << "Source Comm: " << namePair.first.mCommName;
         out.AddNote() << "Destination Platform: " << namePair.second.mPlatformName;
         out.AddNote() << "Destination Comm: " << namePair.second.mCommName;
      }
   }

   for (auto& linkAddress : mInputAddressLinkList)
   {
      if (aSimulation.GetCommNetworkManager()->GetComm(linkAddress.first) &&
          aSimulation.GetCommNetworkManager()->GetComm(linkAddress.second))
      {
         if (std::end(mAddressLinkList) == std::find(std::begin(mAddressLinkList), std::end(mAddressLinkList), linkAddress))
         {
            mAddressLinkList.push_back(linkAddress);
         }
      }
      else
      {
         auto out = ut::log::warning() << "Address Link Members could not be found.";
         out.AddNote() << "Network: " << GetType();
         out.AddNote() << "Source Address: " << linkAddress.first;
         out.AddNote() << "Destination Address: " << linkAddress.second;
      }
   }

   mNamedList.clear();
   mNamedLinkList.clear();
   mInputAddressLinkList.clear();
   mInputAddressList.clear();

   return InitializeLinkageP(aSimulation);
}

// =================================================================================================
bool Network::ProcessAddressListInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "address_list")
   {
      UtInputBlock inputBlock(aInput, "end_address_list");
      while (inputBlock.ReadCommand())
      {
         auto& input = inputBlock.GetInput();

         command = input.GetCommand();
         if (command != "member")
         {
            throw UtInput::BadValue(input);
         }

         mInputAddressList.emplace_back(Address::ProcessInput(input));
      }
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}
// =================================================================================================
bool Network::ProcessAddressLinkListInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "address_link_list")
   {
      Address      firstAddress, secondAddress;
      UtInputBlock inputBlock(aInput, "end_address_link_list");
      while (inputBlock.ReadCommand())
      {
         auto& input = inputBlock.GetInput();

         command = input.GetCommand();
         if (command != "link")
         {
            throw UtInput::BadValue(input);
         }

         firstAddress  = Address::ProcessInput(input);
         secondAddress = Address::ProcessInput(input);

         mAddressLinkList.emplace_back(firstAddress, secondAddress);
      }
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}
// =================================================================================================
bool Network::ProcessNamedListInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "comm_list")
   {
      UtInputBlock inputBlock(aInput, "end_comm_list");
      while (inputBlock.ReadCommand())
      {
         auto&     input = inputBlock.GetInput();
         NamedComm namedComm;

         command = input.GetCommand();
         if (command != "member")
         {
            throw UtInput::BadValue(input);
         }

         input.ReadValue(namedComm.mPlatformName);
         input.ReadValue(namedComm.mCommName);

         mNamedList.push_back(namedComm);
      }
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}
// =================================================================================================
bool Network::ProcessNamedLinkListInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "comm_link_list")
   {
      UtInputBlock inputBlock(aInput, "end_comm_link_list");
      while (inputBlock.ReadCommand())
      {
         auto&     input = inputBlock.GetInput();
         NamedComm sourceComm, destinationComm;

         command = input.GetCommand();
         if (command != "link")
         {
            throw UtInput::BadValue(input);
         }

         input.ReadValue(sourceComm.mPlatformName);
         input.ReadValue(sourceComm.mCommName);
         input.ReadValue(destinationComm.mPlatformName);
         input.ReadValue(destinationComm.mCommName);

         mNamedLinkList.emplace_back(sourceComm, destinationComm);
      }
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

// =================================================================================================
bool Network::AddMember(const Address& aAddress,
                        WsfSimulation& aSimulation,
                        const Address& aOptionalAddress) // = Address()
{
   //! Avoid duplicates
   auto it = std::find(std::begin(mAddressList), std::end(mAddressList), aAddress);
   if (it != std::end(mAddressList))
   {
      return false;
   }

   if (AddMemberP(aAddress, aSimulation))
   {
      mAddressList.push_back(aAddress);
      return true;
   }
   else
   {
      return false;
   }
}

// =================================================================================================
bool Network::AddLink(const Address& aSourceAddress,
                      const Address& aDestinationAddress,
                      WsfSimulation& aSimulation,
                      const Address& aOptionalAddress) // = Address()
{
   auto it = std::find(std::begin(mAddressLinkList),
                       std::end(mAddressLinkList),
                       std::make_pair(aSourceAddress, aDestinationAddress));
   if (it != std::end(mAddressLinkList))
   {
      return false;
   }

   if (AddLinkP(aSourceAddress, aDestinationAddress, aSimulation))
   {
      mAddressLinkList.emplace_back(aSourceAddress, aDestinationAddress);
      return true;
   }
   else
   {
      return false;
   }
}

// =================================================================================================
bool Network::RemoveMember(const Address& aAddress,
                           WsfSimulation& aSimulation,
                           const Address& aOptionalAddress) // = Address()
{
   auto it = std::find(std::begin(mAddressList), std::end(mAddressList), aAddress);
   if (it == std::end(mAddressList))
   {
      return false;
   }

   if (RemoveMemberP(aAddress, aSimulation))
   {
      mAddressList.erase(it);
      return true;
   }
   else
   {
      return false;
   }
}

// =================================================================================================
bool Network::RemoveLink(const Address& aSourceAddress,
                         const Address& aDestinationAddress,
                         WsfSimulation& aSimulation,
                         const Address& aOptionalAddress) // = Address()
{
   auto it = std::find(std::begin(mAddressLinkList),
                       std::end(mAddressLinkList),
                       std::make_pair(aSourceAddress, aDestinationAddress));
   if (it == std::end(mAddressLinkList))
   {
      return false;
   }

   if (RemoveLinkP(aSourceAddress, aDestinationAddress, aSimulation))
   {
      mAddressLinkList.erase(it);
      return true;
   }
   else
   {
      return false;
   }
}

// =================================================================================================
bool Network::HasMember(const Address& aAddress) const
{
   auto it = std::find(std::begin(mAddressList), std::end(mAddressList), aAddress);
   return it != std::end(mAddressList);
}

// =================================================================================================
ScriptNetworkClass::ScriptNetworkClass(const std::string& aClassName,
                                       UtScriptTypes*     aTypesPtr) // = Address()
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName("WsfNetwork");

   mConstructible      = false;
   mCloneable          = false;
   mIsScriptAccessible = true;

   AddMethod(ut::make_unique<GetAddress>());
   AddMethod(ut::make_unique<GetName>());
   AddMethod(ut::make_unique<SetName>());
   AddMethod(ut::make_unique<SetAddress>());

   AddMethod(ut::make_unique<AddMember_1>("AddMember"));
   AddMethod(ut::make_unique<AddMember_2>("AddMember"));
   AddStaticMethod(ut::make_unique<RemoveMember_1>("RemoveMember"));
   AddStaticMethod(ut::make_unique<RemoveMember_2>("RemoveMember"));
   AddMethod(ut::make_unique<AddLink_1>("AddLink"));
   AddMethod(ut::make_unique<AddLink_2>("AddLink"));
   AddMethod(ut::make_unique<RemoveLink_1>("RemoveLink"));
   AddMethod(ut::make_unique<RemoveLink_2>("RemoveLink"));
   AddMethod(ut::make_unique<GetMembers>());

   AddStaticMethod(ut::make_unique<AddNetwork>());
   AddStaticMethod(ut::make_unique<RemoveNetwork>());
   AddStaticMethod(ut::make_unique<GetNetwork_1>("GetNetwork"));
   AddStaticMethod(ut::make_unique<GetNetwork_2>("GetNetwork"));
   AddStaticMethod(ut::make_unique<GetManagedNetworks>());
}

// =================================================================================================
std::string ScriptNetworkClass::ToString(void* aObjectPtr) const
{
   std::stringstream ss;
   ss << "WsfNetwork(";
   if (aObjectPtr)
   {
      auto networkPtr = static_cast<Network*>(aObjectPtr);
      ss << "Address: " << networkPtr->GetAddress();
      ss << " Name: " << networkPtr->GetType();
   }
   else
   {
      ss << "null";
   }

   ss << ")";
   return ss.str();
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptNetworkClass, Network, GetAddress, 0, "WsfAddress", "")
{
   WSF_SCRIPT_WARN_INIT_NOT_INIT2
   const Address& address = aObjectPtr->GetAddress();
   aReturnVal.SetPointer(WsfScriptCommAddressClass::Create(address));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptNetworkClass, Network, SetAddress, 1, "void", "WsfAddress")
{
   WSF_SCRIPT_WARN_INIT_NOT_INIT2
   Address* address = static_cast<Address*>(aVarArgs[0].GetPointer()->GetAppObject());
   aObjectPtr->SetAddress(*address);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptNetworkClass, Network, GetName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetType());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptNetworkClass, Network, SetName, 1, "void", "string")
{
   std::string name = aVarArgs[0].GetString();
   aObjectPtr->SetType(name);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptNetworkClass, Network, AddMember_1, 1, "bool", "WsfComm")
{
   WSF_SCRIPT_WARN_INIT_NOT_INIT2
   bool result = false;

   Comm* commPtr = static_cast<Comm*>(aVarArgs[0].GetPointer()->GetAppObject());
   if (commPtr)
   {
      WsfSimulation*  simPtr      = WsfScriptContext::GetSIMULATION(aContext);
      NetworkManager* managerPtr  = simPtr->GetCommNetworkManager();
      std::string     networkName = aObjectPtr->GetType();

      auto address = managerPtr->AddComm(simPtr->GetSimTime(), commPtr, networkName);
      if (!address.IsNull())
      {
         result = true;
      }
   }

   aReturnVal.SetBool(result);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptNetworkClass, Network, AddMember_2, 2, "bool", "string, string")
{
   WSF_SCRIPT_WARN_INIT_NOT_INIT2
   bool result = false;

   std::string     platName    = aVarArgs[0].GetString();
   std::string     commName    = aVarArgs[1].GetString();
   WsfSimulation*  simPtr      = WsfScriptContext::GetSIMULATION(aContext);
   NetworkManager* managerPtr  = simPtr->GetCommNetworkManager();
   std::string     networkName = aObjectPtr->GetType();

   WsfPlatform* platPtr = simPtr->GetPlatformByName("platName");
   if (platPtr)
   {
      Comm* commPtr = platPtr->GetComponent<Comm>("commName");
      if (commPtr)
      {
         auto address = managerPtr->AddComm(simPtr->GetSimTime(), commPtr, networkName);
         if (!address.IsNull())
         {
            result = true;
         }
      }
   }

   aReturnVal.SetBool(result);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptNetworkClass, Network, RemoveMember_1, 1, "bool", "WsfAddress")
{
   WSF_SCRIPT_WARN_INIT_NOT_INIT2
   Address* addressPtr = static_cast<Address*>(aVarArgs[0].GetPointer()->GetAppObject());
   bool     wasRemoved = false;
   if (addressPtr && !addressPtr->IsNull())
   {
      WsfSimulation*  simPtr     = WsfScriptContext::GetSIMULATION(aContext);
      NetworkManager* managerPtr = simPtr->GetCommNetworkManager();
      Comm*           commPtr    = managerPtr->GetComm(*addressPtr);

      managerPtr->RemoveComm(simPtr->GetSimTime(), *addressPtr);
      wasRemoved = !managerPtr->IsCommManaged(commPtr);
   }
   aReturnVal.SetBool(wasRemoved);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptNetworkClass, Network, RemoveMember_2, 2, "bool", "string, string")
{
   WSF_SCRIPT_WARN_INIT_NOT_INIT2
   std::string     platName   = aVarArgs[0].GetString();
   std::string     commName   = aVarArgs[1].GetString();
   WsfSimulation*  simPtr     = WsfScriptContext::GetSIMULATION(aContext);
   NetworkManager* managerPtr = simPtr->GetCommNetworkManager();
   auto            address    = Network::GetAddressFromNamedComm(platName, commName, *simPtr);
   Comm*           commPtr    = managerPtr->GetComm(address);

   if (!address.IsNull())
   {
      managerPtr->RemoveComm(simPtr->GetSimTime(), address);
      bool wasRemoved = !managerPtr->IsCommManaged(commPtr);
      aReturnVal.SetBool(wasRemoved);
   }
   else
   {
      aReturnVal.SetBool(false);
   }
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptNetworkClass, Network, AddLink_1, 2, "bool", "WsfAddress, WsfAddress")
{
   WSF_SCRIPT_WARN_INIT_NOT_INIT2
   Address*       addressSendPtr    = static_cast<Address*>(aVarArgs[0].GetPointer()->GetAppObject());
   Address*       addressReceivePtr = static_cast<Address*>(aVarArgs[1].GetPointer()->GetAppObject());
   WsfSimulation* simPtr            = WsfScriptContext::GetSIMULATION(aContext);

   auto result = aObjectPtr->AddLink(*addressSendPtr, *addressReceivePtr, *simPtr);
   aReturnVal.SetBool(result);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptNetworkClass, Network, AddLink_2, 4, "bool", "string, string, string, string")
{
   WSF_SCRIPT_WARN_INIT_NOT_INIT2
   std::string    platSendName    = aVarArgs[0].GetString();
   std::string    commSendName    = aVarArgs[1].GetString();
   std::string    platReceiveName = aVarArgs[2].GetString();
   std::string    commReceiveName = aVarArgs[3].GetString();
   WsfSimulation* simPtr          = WsfScriptContext::GetSIMULATION(aContext);
   auto           addressSend     = Network::GetAddressFromNamedComm(platSendName, commSendName, *simPtr);
   auto           addressReceive  = Network::GetAddressFromNamedComm(platReceiveName, commReceiveName, *simPtr);

   if ((!addressSend.IsNull()) && (!addressReceive.IsNull()))
   {
      auto result = aObjectPtr->AddLink(addressSend, addressReceive, *simPtr);
      aReturnVal.SetBool(result);
   }
   else
   {
      aReturnVal.SetBool(false);
   }
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptNetworkClass, Network, RemoveLink_1, 2, "bool", "WsfAddress, WsfAddress")
{
   WSF_SCRIPT_WARN_INIT_NOT_INIT2
   Address*       addressSendPtr    = static_cast<Address*>(aVarArgs[0].GetPointer()->GetAppObject());
   Address*       addressReceivePtr = static_cast<Address*>(aVarArgs[1].GetPointer()->GetAppObject());
   WsfSimulation* simPtr            = WsfScriptContext::GetSIMULATION(aContext);

   auto result = aObjectPtr->RemoveLink(*addressSendPtr, *addressReceivePtr, *simPtr);
   aReturnVal.SetBool(result);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptNetworkClass, Network, RemoveLink_2, 4, "bool", "string, string, string, string")
{
   WSF_SCRIPT_WARN_INIT_NOT_INIT2
   std::string    platSendName    = aVarArgs[0].GetString();
   std::string    commSendName    = aVarArgs[1].GetString();
   std::string    platReceiveName = aVarArgs[2].GetString();
   std::string    commReceiveName = aVarArgs[3].GetString();
   WsfSimulation* simPtr          = WsfScriptContext::GetSIMULATION(aContext);
   auto           addressSend     = Network::GetAddressFromNamedComm(platSendName, commSendName, *simPtr);
   auto           addressReceive  = Network::GetAddressFromNamedComm(platReceiveName, commReceiveName, *simPtr);

   if ((!addressSend.IsNull()) && (!addressReceive.IsNull()))
   {
      auto result = aObjectPtr->RemoveLink(addressSend, addressReceive, *simPtr);
      aReturnVal.SetBool(result);
   }
   else
   {
      aReturnVal.SetBool(false);
   }
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptNetworkClass, Network, GetMembers, 0, "Array<WsfAddress>", "")
{
   WsfSimulation* simPtr            = WsfScriptContext::GetSIMULATION(aContext);
   auto           networkManagerPtr = simPtr->GetCommNetworkManager();
   Network*       networkPtr        = aObjectPtr;

   auto memberSet = networkManagerPtr->GetAddressesInNetwork(networkPtr->GetType());
   auto data      = ut::make_unique<std::vector<UtScriptData>>();
   for (const auto& memberAddress : memberSet)
   {
      data->emplace_back(WsfScriptCommAddressClass::Create(memberAddress));
   }

   aReturnVal.SetPointer(new UtScriptRef(data.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptNetworkClass, Network, AddNetwork, 1, "bool", "WsfNetwork")
{
   WSF_SCRIPT_WARN_INIT_NOT_INIT2
   Network* networkPtr = static_cast<Network*>(aVarArgs[0].GetPointer()->GetAppObject());

   WsfSimulation* simPtr            = WsfScriptContext::GetSIMULATION(aContext);
   auto           networkManagerPtr = simPtr->GetCommNetworkManager();

   bool success = networkManagerPtr->AddNetwork(simPtr->GetSimTime(), ut::clone(networkPtr));

   aReturnVal.SetBool(success);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptNetworkClass, Network, RemoveNetwork, 1, "void", "WsfNetwork")
{
   WSF_SCRIPT_WARN_INIT_NOT_INIT2
   WsfSimulation* simPtr            = WsfScriptContext::GetSIMULATION(aContext);
   auto           networkManagerPtr = simPtr->GetCommNetworkManager();
   Network*       networkPtr        = static_cast<Network*>(aVarArgs[0].GetPointer()->GetAppObject());

   networkManagerPtr->RemoveNetwork(simPtr->GetSimTime(), networkPtr->GetType());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptNetworkClass, Network, GetNetwork_1, 1, "WsfNetwork", "string")
{
   WSF_SCRIPT_WARN_INIT_NOT_INIT2
   std::string networkName = aVarArgs[0].GetString();

   WsfSimulation* simPtr            = WsfScriptContext::GetSIMULATION(aContext);
   auto           networkManagerPtr = simPtr->GetCommNetworkManager();

   Network* networkPtr = networkManagerPtr->GetNetwork(networkName);

   UtScriptRef* networkRefPtr = new UtScriptRef(networkPtr, aObjectClassPtr);
   aReturnVal.SetPointer(networkRefPtr);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptNetworkClass, Network, GetNetwork_2, 1, "WsfNetwork", "WsfAddress")
{
   WSF_SCRIPT_WARN_INIT_NOT_INIT2
   Address* networkAddressPtr = static_cast<Address*>(aVarArgs[0].GetPointer()->GetAppObject());

   WsfSimulation* simPtr            = WsfScriptContext::GetSIMULATION(aContext);
   auto           networkManagerPtr = simPtr->GetCommNetworkManager();

   std::string networkName = networkManagerPtr->GetNetworkNameFromAddress(*networkAddressPtr);
   Network*    networkPtr  = networkManagerPtr->GetNetwork(networkName);

   UtScriptRef* networkRefPtr = new UtScriptRef(networkPtr, aObjectClassPtr);
   aReturnVal.SetPointer(networkRefPtr);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptNetworkClass, Network, GetManagedNetworks, 0, "Array<string>", "")
{
   WSF_SCRIPT_WARN_INIT_NOT_INIT2
   WsfSimulation* simPtr            = WsfScriptContext::GetSIMULATION(aContext);
   auto           networkManagerPtr = simPtr->GetCommNetworkManager();

   std::vector<std::string>   networks = networkManagerPtr->GetManagedNetworks();
   std::vector<UtScriptData>* data     = new std::vector<UtScriptData>(networks.size());
   for (size_t i = 0; i < networks.size(); ++i)
   {
      (*data)[i].SetString(networks[i]);
   }

   aReturnVal.SetPointer(new UtScriptRef(data, aReturnClassPtr, UtScriptRef::cMANAGE));
}

} // namespace comm
} // namespace wsf
