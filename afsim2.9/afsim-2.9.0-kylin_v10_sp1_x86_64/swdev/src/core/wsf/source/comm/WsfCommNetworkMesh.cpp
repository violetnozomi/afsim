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

#include "WsfCommNetworkMesh.hpp"

#include "WsfCommNetworkManager.hpp"
#include "WsfSimulation.hpp"
#include "script/WsfScriptContext.hpp"

namespace wsf
{
namespace comm
{

// =================================================================================================
NetworkMesh::NetworkMesh(const std::string& aNetworkName, const Address& aAddress)
   : Network(aNetworkName, aAddress)
{
}

// =================================================================================================
NetworkMesh* NetworkMesh::Clone() const
{
   return new NetworkMesh(*this);
}

// =================================================================================================
bool NetworkMesh::ProcessInput(UtInput& aInput)
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
bool NetworkMesh::InitializeLinkageP(WsfSimulation& aSimulation)
{
   bool ok = true;

   for (auto& address : mAddressList)
   {
      ok &= AddMemberP(address, aSimulation);
   }

   return ok;
}

// =================================================================================================
bool NetworkMesh::AddMemberP(const Address& aAddress,
                             WsfSimulation& aSimulation,
                             const Address& aOptionalAddress) // = Address()
{
   bool ok = true;

   //! If this is the first member, there isn't anybody to connect to. Just return.
   if (mAddressList.empty())
   {
      return ok;
   }

   auto managerPtr = aSimulation.GetCommNetworkManager();
   auto simTime    = aSimulation.GetSimTime();

   for (auto& address : mAddressList)
   {
      ok &= managerPtr->AddConnection(simTime, aAddress, address);
      ok &= managerPtr->AddConnection(simTime, address, aAddress);
   }

   return ok;
}

// =================================================================================================
bool NetworkMesh::RemoveMemberP(const Address& aAddress,
                                WsfSimulation& aSimulation,
                                const Address& aOptionalAddress) // = Address()
{
   bool ok = true;

   //! If only one member exists, there are no links to remove.
   if (mAddressList.size() == 1)
   {
      return true;
   }

   auto managerPtr = aSimulation.GetCommNetworkManager();
   auto simTime    = aSimulation.GetSimTime();

   for (auto& address : mAddressList)
   {
      ok &= managerPtr->RemoveConnection(simTime, aAddress, address);
      ok &= managerPtr->RemoveConnection(simTime, address, aAddress);
   }

   return ok;
}

// =================================================================================================
NetworkMeshLegacy::NetworkMeshLegacy(const std::string& aNetworkName, const Address& aAddress)
   : NetworkMesh(aNetworkName, aAddress)
{
}

// =================================================================================================
NetworkMeshLegacy* NetworkMeshLegacy::Clone() const
{
   return new NetworkMeshLegacy(*this);
}

// =================================================================================================
bool NetworkMeshLegacy::AddMemberP(const Address& aAddress,
                                   WsfSimulation& aSimulation,
                                   const Address& aOptionalAddress) // = Address()
{
   bool ok = true;

   //! If this is the first member, there isn't anybody to connect to. Just return.
   if (mAddressList.empty())
   {
      return ok;
   }

   auto managerPtr = aSimulation.GetCommNetworkManager();
   auto simTime    = aSimulation.GetSimTime();

   for (auto& address : mAddressList)
   {
      auto comm1Ptr = managerPtr->GetComm(aAddress);
      auto comm2Ptr = managerPtr->GetComm(address);
      if (comm1Ptr && comm2Ptr)
      {
         if (comm1Ptr->CanSendTo(simTime, comm2Ptr))
         {
            ok &= managerPtr->AddConnection(simTime, aAddress, address);
         }
         if (comm2Ptr->CanSendTo(simTime, comm1Ptr))
         {
            ok &= managerPtr->AddConnection(simTime, address, aAddress);
         }
      }
   }

   return ok;
}

// =================================================================================================
ScriptNetworkMeshClass::ScriptNetworkMeshClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : ScriptNetworkClass(aClassName, aTypesPtr)
{
   SetClassName("WsfNetworkMesh");

   mConstructible = true;
}

// =================================================================================================
void* ScriptNetworkMeshClass::Create(const UtScriptContext& aInstance)
{
   return new NetworkMesh();
}

// =================================================================================================
void ScriptNetworkMeshClass::Destroy(void* aNetworkPtr)
{
   DestroyHelper<NetworkMesh>(aNetworkPtr);
}

// =================================================================================================
ScriptNetworkMeshLegacyClass::ScriptNetworkMeshLegacyClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : ScriptNetworkMeshClass(aClassName, aTypesPtr)
{
   SetClassName("WsfNetworkMeshLegacy");
}

// =================================================================================================
void* ScriptNetworkMeshLegacyClass::Create(const UtScriptContext& aInstance)
{
   return new NetworkMeshLegacy();
}

// =================================================================================================
void ScriptNetworkMeshLegacyClass::Destroy(void* aNetworkPtr)
{
   DestroyHelper<NetworkMeshLegacy>(aNetworkPtr);
}

} // namespace comm
} // namespace wsf
