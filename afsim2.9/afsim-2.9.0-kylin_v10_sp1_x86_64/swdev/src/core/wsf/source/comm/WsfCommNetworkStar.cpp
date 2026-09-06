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

#include "WsfCommNetworkStar.hpp"

#include <memory>

#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "WsfCommNetworkManager.hpp"
#include "WsfSimulation.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptDefs.hpp"

namespace wsf
{
namespace comm
{

// =================================================================================================
NetworkStar::NetworkStar(const std::string& aNetworkName, const Address& aAddress)
   : Network(aNetworkName, aAddress)
{
}

// =================================================================================================
bool NetworkStar::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "network_address")
   {
      mAddress = Address::ProcessInput(aInput);
   }
   else if (command == "hub_named")
   {
      aInput.ReadValue(mHubNamed.mPlatformName);
      aInput.ReadValue(mHubNamed.mCommName);
   }
   else if (command == "hub_address")
   {
      mHubAddress = Address::ProcessInput(aInput);
   }
   else if (ProcessAddressListInput(aInput))
   {
   }
   else if (ProcessNamedListInput(aInput))
   {
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

// =================================================================================================
bool NetworkStar::InitializeLinkageP(WsfSimulation& aSimulation)
{
   bool ok = true;

   //! Check for existence of hub, and convert to an address, if necessary.
   if (!mHubNamed.mPlatformName.Empty())
   {
      mHubAddress = GetAddressFromNamedComm(mHubNamed.mPlatformName, mHubNamed.mCommName, aSimulation);
   }

   if (mHubAddress.IsNull())
   {
      //! The user may later set the hub, but for now, we can't initialize any links since we
      //! don't have a hub. Warn the user, but continue.
      auto out = ut::log::warning() << "Star network does not have a hub.";
      out.AddNote() << "Network: " << GetType();
      out.AddNote() << "This network will not have any initial connectivity.";
      return true;
   }

   for (auto& address : mAddressList)
   {
      ok &= AddMemberP(address, aSimulation);
   }

   return ok;
}

// =================================================================================================
bool NetworkStar::AddMemberP(const Address& aAddress,
                             WsfSimulation& aSimulation,
                             const Address& aOptionalAddress) // = Address()
{
   bool ok = true;

   //! Only add links if the hub exists.
   if (mHubAddress.IsNull())
   {
      return ok;
   }

   auto managerPtr = aSimulation.GetCommNetworkManager();
   auto simTime    = aSimulation.GetSimTime();

   ok &= managerPtr->AddConnection(simTime, mHubAddress, aAddress);
   ok &= managerPtr->AddConnection(simTime, aAddress, mHubAddress);

   return ok;
}
// =================================================================================================
bool NetworkStar::RemoveMemberP(const Address& aAddress,
                                WsfSimulation& aSimulation,
                                const Address& aOptionalAddress) // = Address()
{
   bool ok = true;

   //! If there is not a hub, there are no links to remove.
   if (mHubAddress.IsNull())
   {
      return ok;
   }

   auto managerPtr = aSimulation.GetCommNetworkManager();
   auto simTime    = aSimulation.GetSimTime();

   ok &= managerPtr->RemoveConnection(simTime, mHubAddress, aAddress);
   ok &= managerPtr->RemoveConnection(simTime, aAddress, mHubAddress);

   return ok;
}

// =================================================================================================
bool NetworkStar::AddHub(const Address& aAddress, WsfSimulation& aSimulation)
{
   if (!mHubAddress.IsNull())
   {
      return false;
   }

   mHubAddress = aAddress;
   return InitializeLinkageP(aSimulation);
}

// =================================================================================================
bool NetworkStar::RemoveHub(const Address& aAddress, WsfSimulation& aSimulation)
{
   bool ok = true;

   if (mHubAddress.IsNull())
   {
      return false;
   }

   auto managerPtr = aSimulation.GetCommNetworkManager();
   auto simTime    = aSimulation.GetSimTime();

   for (auto& address : mAddressList)
   {
      ok &= managerPtr->RemoveConnection(simTime, mHubAddress, address);
      ok &= managerPtr->RemoveConnection(simTime, address, mHubAddress);
   }

   mHubAddress = Address();
   return ok;
}

// =================================================================================================
NetworkStar* NetworkStar::Clone() const
{
   return new NetworkStar(*this);
}

// =================================================================================================
ScriptNetworkStarClass::ScriptNetworkStarClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : ScriptNetworkClass(aClassName, aTypesPtr)
{
   SetClassName("WsfNetworkStar");

   AddMethod(ut::make_unique<AddHub_1>("AddHub"));
   AddMethod(ut::make_unique<AddHub_2>("AddHub"));
   AddMethod(ut::make_unique<RemoveHub_1>("RemoveHub"));
   AddMethod(ut::make_unique<RemoveHub_2>("RemoveHub"));

   mConstructible = true;
}

// =================================================================================================
void* ScriptNetworkStarClass::Create(const UtScriptContext& aInstance)
{
   return new NetworkStar();
}

// =================================================================================================
void ScriptNetworkStarClass::Destroy(void* aNetworkPtr)
{
   DestroyHelper<NetworkStar>(aNetworkPtr);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptNetworkStarClass, NetworkStar, AddHub_1, 1, "bool", "WsfAddress")
{
   WSF_SCRIPT_WARN_INIT_NOT_INIT2
   Address*       addressPtr = static_cast<Address*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfSimulation* simPtr     = WsfScriptContext::GetSIMULATION(aContext);

   auto result = aObjectPtr->AddHub(*addressPtr, *simPtr);
   aReturnVal.SetBool(result);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptNetworkStarClass, NetworkStar, AddHub_2, 2, "bool", "string, string")
{
   WSF_SCRIPT_WARN_INIT_NOT_INIT2
   std::string    platName = aVarArgs[0].GetString();
   std::string    commName = aVarArgs[1].GetString();
   WsfSimulation* simPtr   = WsfScriptContext::GetSIMULATION(aContext);
   auto           address  = Network::GetAddressFromNamedComm(platName, commName, *simPtr);

   if (!address.IsNull())
   {
      auto result = aObjectPtr->AddHub(address, *simPtr);
      aReturnVal.SetBool(result);
   }
   else
   {
      aReturnVal.SetBool(false);
   }
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptNetworkStarClass, NetworkStar, RemoveHub_1, 1, "bool", "WsfAddress")
{
   WSF_SCRIPT_WARN_INIT_NOT_INIT2
   Address*       addressPtr = static_cast<Address*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfSimulation* simPtr     = WsfScriptContext::GetSIMULATION(aContext);

   auto result = aObjectPtr->RemoveHub(*addressPtr, *simPtr);
   aReturnVal.SetBool(result);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptNetworkStarClass, NetworkStar, RemoveHub_2, 2, "bool", "string, string")
{
   WSF_SCRIPT_WARN_INIT_NOT_INIT2
   std::string    platName = aVarArgs[0].GetString();
   std::string    commName = aVarArgs[1].GetString();
   WsfSimulation* simPtr   = WsfScriptContext::GetSIMULATION(aContext);
   auto           address  = Network::GetAddressFromNamedComm(platName, commName, *simPtr);

   if (!address.IsNull())
   {
      auto result = aObjectPtr->RemoveHub(address, *simPtr);
      aReturnVal.SetBool(result);
   }
   else
   {
      aReturnVal.SetBool(false);
   }
}

} // namespace comm
} // namespace wsf
