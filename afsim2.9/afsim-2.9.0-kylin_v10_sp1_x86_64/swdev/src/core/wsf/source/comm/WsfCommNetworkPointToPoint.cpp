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

#include "WsfCommNetworkPointToPoint.hpp"

#include "WsfCommNetworkManager.hpp"
#include "WsfSimulation.hpp"
#include "script/WsfScriptContext.hpp"

namespace wsf
{
namespace comm
{

// =================================================================================================
NetworkPointToPoint::NetworkPointToPoint(const std::string& aNetworkName, const Address& aAddress)
   : Network(aNetworkName, aAddress)
{
}

// =================================================================================================
NetworkPointToPoint* NetworkPointToPoint::Clone() const
{
   return new NetworkPointToPoint(*this);
}

// =================================================================================================
bool NetworkPointToPoint::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "network_address")
   {
      mAddress = Address::ProcessInput(aInput);
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
bool NetworkPointToPoint::InitializeLinkageP(WsfSimulation& aSimulation)
{
   bool ok = true;

   for (auto& address : mAddressList)
   {
      ok &= AddMemberP(address, aSimulation);
   }

   return ok;
}

// =================================================================================================
bool NetworkPointToPoint::AddMemberP(const Address& aAddress,
                                     WsfSimulation& aSimulation,
                                     const Address& aOptionalAddress) // = Address()
{
   bool ok = true;

   //! If the initial list is empty, just add the member, as there's nobody to connect to.
   if (mAddressList.empty())
   {
      return true;
   }
   else if (mAddressList.size() == 2)
   {
      //! This addition will result in an invalid P2P topology. Disallow.
      return false;
   }
   else
   {
      //! Add linkage when the current list is size 1, as we now have two members.
      auto managerPtr = aSimulation.GetCommNetworkManager();
      auto simTime    = aSimulation.GetSimTime();

      ok &= managerPtr->AddConnection(simTime, mAddressList[0], aAddress);
      ok &= managerPtr->AddConnection(simTime, aAddress, mAddressList[0]);
   }

   return ok;
}

// =================================================================================================
bool NetworkPointToPoint::RemoveMemberP(const Address& aAddress,
                                        WsfSimulation& aSimulation,
                                        const Address& aOptionalAddress) // = Address()
{
   bool ok         = true;
   auto managerPtr = aSimulation.GetCommNetworkManager();
   auto simTime    = aSimulation.GetSimTime();

   //! Only remove links if we have current linkage established, indicated by having two
   //! members in the network.
   if (mAddressList.size() == 2)
   {
      ok &= managerPtr->RemoveConnection(simTime, aAddress, mAddressList[0]);
      ok &= managerPtr->RemoveConnection(simTime, mAddressList[0], aAddress);
   }

   return ok;
}

// =================================================================================================
ScriptNetworkPointToPointClass::ScriptNetworkPointToPointClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : ScriptNetworkClass(aClassName, aTypesPtr)
{
   SetClassName("WsfNetworkP2P");

   mConstructible = true;
}

// =================================================================================================
void* ScriptNetworkPointToPointClass::Create(const UtScriptContext& aInstance)
{
   return new NetworkPointToPoint();
}

// =================================================================================================
void ScriptNetworkPointToPointClass::Destroy(void* aNetworkPtr)
{
   DestroyHelper<NetworkPointToPoint>(aNetworkPtr);
}

} // namespace comm
} // namespace wsf
