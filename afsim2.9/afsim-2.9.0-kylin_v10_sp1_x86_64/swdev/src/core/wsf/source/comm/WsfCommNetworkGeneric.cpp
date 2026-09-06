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

#include "WsfCommNetworkGeneric.hpp"

#include "WsfCommNetworkManager.hpp"
#include "WsfSimulation.hpp"
#include "script/WsfScriptContext.hpp"

namespace wsf
{
namespace comm
{

// =================================================================================================
NetworkGeneric::NetworkGeneric(const std::string& aNetworkName, const Address& aAddress)
   : Network(aNetworkName, aAddress)
{
}

// =================================================================================================
NetworkGeneric* NetworkGeneric::Clone() const
{
   return new NetworkGeneric(*this);
}

// =================================================================================================
bool NetworkGeneric::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "network_address")
   {
      mAddress = Address::ProcessInput(aInput);
   }
   else if (ProcessAddressLinkListInput(aInput))
   {
   }
   else if (ProcessNamedLinkListInput(aInput))
   {
   }
   else if (ProcessNamedListInput(aInput))
   {
   }
   else if (ProcessAddressListInput(aInput))
   {
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

// =================================================================================================
bool NetworkGeneric::InitializeLinkageP(WsfSimulation& aSimulation)
{
   bool ok = true;

   for (auto& link : mAddressLinkList)
   {
      ok &= AddLinkP(link.first, link.second, aSimulation);
   }

   return ok;
}

// =================================================================================================
bool NetworkGeneric::AddLinkP(const Address& aSourceAddress,
                              const Address& aDestinationAddress,
                              WsfSimulation& aSimulation,
                              const Address& aOptionalAddress) // = Address()
{
   bool ok         = true;
   auto managerPtr = aSimulation.GetCommNetworkManager();
   auto simTime    = aSimulation.GetSimTime();

   //! This implementation has no topology, so any specified links are allowed.
   ok &= managerPtr->AddConnection(simTime, aSourceAddress, aDestinationAddress);

   return ok;
}

// =================================================================================================
bool NetworkGeneric::RemoveLinkP(const Address& aSourceAddress,
                                 const Address& aDestinationAddress,
                                 WsfSimulation& aSimulation,
                                 const Address& aOptionalAddress) // = Address()
{
   bool ok         = true;
   auto managerPtr = aSimulation.GetCommNetworkManager();
   auto simTime    = aSimulation.GetSimTime();

   ok &= managerPtr->RemoveConnection(simTime, aSourceAddress, aDestinationAddress);

   return ok;
}

// =================================================================================================
bool NetworkGeneric::RemoveMemberP(const Address& aAddress,
                                   WsfSimulation& aSimulation,
                                   const Address& aOptionalAddress) // = Address()
{
   // We only use this method in the generic network to remove any linkage associated
   // with a member being removed. It will always succeed.
   // Note: Objects in mAddressLinkList are possibly being removed during this operation,
   // so use a local copy of the original list to avoid invalidated iterators/pointers.
   auto localLinkList = mAddressLinkList;
   for (const auto& link : localLinkList)
   {
      if (link.first == aAddress || link.second == aAddress)
      {
         RemoveLink(link.first, link.second, aSimulation);
      }
   }

   return true;
}

// =================================================================================================
ScriptNetworkGenericClass::ScriptNetworkGenericClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : ScriptNetworkClass(aClassName, aTypesPtr)
{
   SetClassName("WsfNetworkGeneric");
   mConstructible = true;
}

// =================================================================================================
void* ScriptNetworkGenericClass::Create(const UtScriptContext& aInstance)
{
   return new NetworkGeneric();
}

// =================================================================================================
void ScriptNetworkGenericClass::Destroy(void* aNetworkPtr)
{
   DestroyHelper<NetworkGeneric>(aNetworkPtr);
}

} // namespace comm
} // namespace wsf
