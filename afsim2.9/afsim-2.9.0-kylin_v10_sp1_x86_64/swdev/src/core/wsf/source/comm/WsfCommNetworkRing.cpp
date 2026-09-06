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

#include "WsfCommNetworkRing.hpp"

#include "WsfCommNetworkManager.hpp"
#include "WsfSimulation.hpp"
#include "script/WsfScriptContext.hpp"

namespace wsf
{
namespace comm
{

// =================================================================================================
NetworkRing::NetworkRing(const std::string& aNetworkName, const Address& aAddress)
   : Network(aNetworkName, aAddress)
{
}

// =================================================================================================
bool NetworkRing::ProcessInput(UtInput& aInput)
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
bool NetworkRing::InitializeLinkageP(WsfSimulation& aSimulation)
{
   bool ok = true;

   //! No linkage necessary for insufficient members.
   if (mAddressList.size() < 3)
   {
      return true;
   }

   //! Note: We don't incrementally add the members one at a time in the ring
   //! topology to avoid creating and removing links with every add. This will
   //! either cause a bunch of output we don't want, or we have to suppress it,
   //! neither of which is favorable. Instead, we'll just do it all here in one
   //! pass if the minimum conditions are correct, so that event output actually
   //! reflects the true state of the network without a bunch of noise.
   auto managerPtr = aSimulation.GetCommNetworkManager();
   auto simTime    = aSimulation.GetSimTime();

   for (size_t i = 0; i < (mAddressList.size() - 1); ++i)
   {
      ok &= managerPtr->AddConnection(simTime, mAddressList[i], mAddressList[i + 1]);
      ok &= managerPtr->AddConnection(simTime, mAddressList[i + 1], mAddressList[i]);
   }

   //! Complete the ring by connecting the first and last members.
   ok &= managerPtr->AddConnection(simTime, mAddressList.back(), mAddressList[0]);
   ok &= managerPtr->AddConnection(simTime, mAddressList[0], mAddressList.back());

   return ok;
}

// =================================================================================================
bool NetworkRing::AddMemberP(const Address& aAddress,
                             WsfSimulation& aSimulation,
                             const Address& aOptionalAddress) // = Address()
{
   bool ok = true;

   //! It takes at least three members to make this topology.
   if (mAddressList.size() < 2)
   {
      return true;
   }
   else if (mAddressList.size() == 2)
   {
      //! If the pending size is 3 (current size == 2), then we currently don't have any
      //! links in this network, and need to establish links for all members, not just the
      //! currently added member. (Note - This will only occur during runtime, as either there
      //! are sufficient members to establish linkage or not at initialization - this is a
      //! condition of members being added or removed during a simulation run.)
      auto managerPtr = aSimulation.GetCommNetworkManager();
      auto simTime    = aSimulation.GetSimTime();

      ok &= managerPtr->AddConnection(simTime, mAddressList[0], mAddressList[1]);
      ok &= managerPtr->AddConnection(simTime, mAddressList[1], mAddressList[0]);
      ok &= managerPtr->AddConnection(simTime, mAddressList[1], aAddress);
      ok &= managerPtr->AddConnection(simTime, aAddress, mAddressList[1]);
      ok &= managerPtr->AddConnection(simTime, aAddress, mAddressList[0]);
      ok &= managerPtr->AddConnection(simTime, mAddressList[0], aAddress);

      return ok;
   }

   //! The ring is established, add this member to the "end" of the ring.
   //! Since this is an "incremental" add, we have to delete links where we insert the
   //! new member.
   auto managerPtr = aSimulation.GetCommNetworkManager();
   auto simTime    = aSimulation.GetSimTime();

   auto it = std::find(std::begin(mAddressList), std::end(mAddressList), aOptionalAddress);
   if (it != std::end(mAddressList))
   {
      //! A special insertion location has been specified and located.
      //! If the location isn't at the end (where we ordinarily insert),
      //! then handle it, otherwise continue processing at the "end" of the ring.
      size_t insertLocation = (it - std::begin(mAddressList));
      if (insertLocation != (mAddressList.size() - 1))
      {
         ok &= managerPtr->RemoveConnection(simTime, mAddressList[insertLocation], mAddressList[insertLocation + 1]);
         ok &= managerPtr->RemoveConnection(simTime, mAddressList[insertLocation + 1], mAddressList[insertLocation]);
         ok &= managerPtr->AddConnection(simTime, aAddress, mAddressList[insertLocation]);
         ok &= managerPtr->AddConnection(simTime, mAddressList[insertLocation], aAddress);
         ok &= managerPtr->AddConnection(simTime, aAddress, mAddressList[insertLocation + 1]);
         ok &= managerPtr->AddConnection(simTime, mAddressList[insertLocation + 1], aAddress);

         return ok;
      }
   }

   //! Insert the new member at the "end" of the ring.
   ok &= managerPtr->RemoveConnection(simTime, mAddressList.back(), mAddressList.front());
   ok &= managerPtr->RemoveConnection(simTime, mAddressList.front(), mAddressList.back());
   ok &= managerPtr->AddConnection(simTime, aAddress, mAddressList.back());
   ok &= managerPtr->AddConnection(simTime, mAddressList.back(), aAddress);
   ok &= managerPtr->AddConnection(simTime, aAddress, mAddressList.front());
   ok &= managerPtr->AddConnection(simTime, mAddressList.front(), aAddress);

   return ok;
}

// =================================================================================================
bool NetworkRing::RemoveMemberP(const Address& aAddress,
                                WsfSimulation& aSimulation,
                                const Address& aOptionalAddress) // = Address()
{
   bool ok = true;

   //! No links to remove is size is already less than 3
   if (mAddressList.size() < 3)
   {
      return true;
   }
   //! If the size is exactly 3, we lose all linkage.
   else if (mAddressList.size() == 3)
   {
      auto managerPtr = aSimulation.GetCommNetworkManager();
      auto simTime    = aSimulation.GetSimTime();

      for (size_t i = 0; i < (mAddressList.size() - 1); ++i)
      {
         ok &= managerPtr->RemoveConnection(simTime, mAddressList[i], mAddressList[i + 1]);
         ok &= managerPtr->RemoveConnection(simTime, mAddressList[i + 1], mAddressList[i]);
      }

      ok &= managerPtr->RemoveConnection(simTime, mAddressList[0], mAddressList.back());
      ok &= managerPtr->RemoveConnection(simTime, mAddressList.back(), mAddressList[0]);

      return ok;
   }
   else
   {
      //! Remove the member and its connections, and add connections between adjacent members.
      auto managerPtr = aSimulation.GetCommNetworkManager();
      auto simTime    = aSimulation.GetSimTime();

      auto   it             = std::find(std::begin(mAddressList), std::end(mAddressList), aAddress);
      size_t removeLocation = (it - std::begin(mAddressList));
      if (removeLocation == (mAddressList.size() - 1))
      {
         //! The removed member is at the "end" of the ring.
         ok &= managerPtr->RemoveConnection(simTime, mAddressList[removeLocation], mAddressList[0]);
         ok &= managerPtr->RemoveConnection(simTime, mAddressList[0], mAddressList[removeLocation]);
         ok &= managerPtr->RemoveConnection(simTime, mAddressList[removeLocation], mAddressList[removeLocation - 1]);
         ok &= managerPtr->RemoveConnection(simTime, mAddressList[removeLocation - 1], mAddressList[removeLocation]);

         //! Re-connect
         ok &= managerPtr->AddConnection(simTime, mAddressList[0], mAddressList[removeLocation - 1]);
         ok &= managerPtr->AddConnection(simTime, mAddressList[removeLocation - 1], mAddressList[0]);
      }
      else if (removeLocation == 0)
      {
         //! The removed member is at the "beginning" of the ring.
         ok &= managerPtr->RemoveConnection(simTime, mAddressList[removeLocation], mAddressList[removeLocation + 1]);
         ok &= managerPtr->RemoveConnection(simTime, mAddressList[removeLocation + 1], mAddressList[removeLocation]);
         ok &= managerPtr->RemoveConnection(simTime, mAddressList[removeLocation], mAddressList.back());
         ok &= managerPtr->RemoveConnection(simTime, mAddressList.back(), mAddressList[removeLocation]);

         //! Re-connect
         ok &= managerPtr->AddConnection(simTime, mAddressList.back(), mAddressList[removeLocation + 1]);
         ok &= managerPtr->AddConnection(simTime, mAddressList[removeLocation + 1], mAddressList.back());
      }
      else
      {
         ok &= managerPtr->RemoveConnection(simTime, mAddressList[removeLocation], mAddressList[removeLocation + 1]);
         ok &= managerPtr->RemoveConnection(simTime, mAddressList[removeLocation + 1], mAddressList[removeLocation]);
         ok &= managerPtr->RemoveConnection(simTime, mAddressList[removeLocation], mAddressList[removeLocation - 1]);
         ok &= managerPtr->RemoveConnection(simTime, mAddressList[removeLocation - 1], mAddressList[removeLocation]);

         //! Re-connect
         ok &= managerPtr->AddConnection(simTime, mAddressList[removeLocation - 1], mAddressList[removeLocation + 1]);
         ok &= managerPtr->AddConnection(simTime, mAddressList[removeLocation + 1], mAddressList[removeLocation - 1]);
      }

      return ok;
   }
}

// =================================================================================================
NetworkRing* NetworkRing::Clone() const
{
   return new NetworkRing(*this);
}

// =================================================================================================
NetworkDirectedRing::NetworkDirectedRing(const std::string& aNetworkName, const Address& aAddress)
   : NetworkRing(aNetworkName, aAddress)
{
}

// =================================================================================================
bool NetworkDirectedRing::InitializeLinkageP(WsfSimulation& aSimulation)
{
   bool ok = true;

   //! No linkage necessary for insufficient members.
   if (mAddressList.size() < 3)
   {
      return true;
   }

   //! Note: We don't incrementally add the members one at a time in the ring
   //! topology to avoid creating and removing links with every add. This will
   //! either cause a bunch of output we don't want, or we have to suppress it,
   //! neither of which is favorable. Instead, we'll just do it all here in one
   //! pass if the minimum conditions are correct, so that event output actually
   //! reflects the true state of the network without a bunch of noise.
   auto managerPtr = aSimulation.GetCommNetworkManager();
   auto simTime    = aSimulation.GetSimTime();

   for (size_t i = 0; i < (mAddressList.size() - 1); ++i)
   {
      ok &= managerPtr->AddConnection(simTime, mAddressList[i], mAddressList[i + 1]);
   }

   //! Complete the ring by connecting the first and last members.
   ok &= managerPtr->AddConnection(simTime, mAddressList.back(), mAddressList[0]);

   return ok;
}

// =================================================================================================
NetworkDirectedRing* NetworkDirectedRing::Clone() const
{
   return new NetworkDirectedRing(*this);
}

// =================================================================================================
bool NetworkDirectedRing::AddMemberP(const Address& aAddress,
                                     WsfSimulation& aSimulation,
                                     const Address& aOptionalAddress) // = Address()
{
   bool ok = true;

   //! It takes at least three members to make this topology.
   if (mAddressList.size() < 2)
   {
      return true;
   }
   else if (mAddressList.size() == 2)
   {
      //! If the pending size is 3 (current size == 2), then we currently don't have any
      //! links in this network, and need to establish links for all members, not just the
      //! currently added member. (Note - This will only occur during runtime, as either there
      //! are sufficient members to establish linkage or not at initialization - this is a
      //! condition of members being added or removed during a simulation run.)
      auto managerPtr = aSimulation.GetCommNetworkManager();
      auto simTime    = aSimulation.GetSimTime();

      ok &= managerPtr->AddConnection(simTime, mAddressList[0], mAddressList[1]);
      ok &= managerPtr->AddConnection(simTime, mAddressList[1], aAddress);
      ok &= managerPtr->AddConnection(simTime, aAddress, mAddressList[0]);

      return ok;
   }

   //! The ring is established, add this member to the "end" of the ring.
   //! Since this is an "incremental" add, we have to delete links where we insert the
   //! new member.
   auto managerPtr = aSimulation.GetCommNetworkManager();
   auto simTime    = aSimulation.GetSimTime();

   auto it = std::find(std::begin(mAddressList), std::end(mAddressList), aOptionalAddress);
   if (it != std::end(mAddressList))
   {
      //! A special insertion location has been specified and located.
      //! If the location isn't at the end (where we ordinarily insert),
      //! then handle it, otherwise continue processing at the "end" of the ring.
      size_t insertLocation = (it - std::begin(mAddressList));
      if (insertLocation != (mAddressList.size() - 1))
      {
         ok &= managerPtr->RemoveConnection(simTime, mAddressList[insertLocation], mAddressList[insertLocation + 1]);
         ok &= managerPtr->AddConnection(simTime, mAddressList[insertLocation], aAddress);
         ok &= managerPtr->AddConnection(simTime, aAddress, mAddressList[insertLocation + 1]);

         return ok;
      }
   }

   //! Insert the new member at the "end" of the ring.
   ok &= managerPtr->RemoveConnection(simTime, mAddressList.back(), mAddressList[0]);
   ok &= managerPtr->AddConnection(simTime, mAddressList.back(), aAddress);
   ok &= managerPtr->AddConnection(simTime, aAddress, mAddressList[0]);

   return ok;
}

// =================================================================================================
bool NetworkDirectedRing::RemoveMemberP(const Address& aAddress,
                                        WsfSimulation& aSimulation,
                                        const Address& aOptionalAddress) // = Address()
{
   bool ok = true;

   //! No links to remove is size is already less than 3
   if (mAddressList.size() < 3)
   {
      return true;
   }
   //! If the size is exactly 3, we lose all linkage.
   else if (mAddressList.size() == 3)
   {
      auto managerPtr = aSimulation.GetCommNetworkManager();
      auto simTime    = aSimulation.GetSimTime();

      for (size_t i = 0; i < (mAddressList.size() - 1); ++i)
      {
         ok &= managerPtr->RemoveConnection(simTime, mAddressList[i], mAddressList[i + 1]);
      }

      ok &= managerPtr->RemoveConnection(simTime, mAddressList.back(), mAddressList.front());

      return ok;
   }
   else
   {
      //! Remove the member and its connections, and add connections between adjacent members.
      auto managerPtr = aSimulation.GetCommNetworkManager();
      auto simTime    = aSimulation.GetSimTime();

      auto   it             = std::find(std::begin(mAddressList), std::end(mAddressList), aAddress);
      size_t removeLocation = (it - std::begin(mAddressList));
      if (removeLocation == (mAddressList.size() - 1))
      {
         //! The removed member is at the "end" of the ring.
         ok &= managerPtr->RemoveConnection(simTime, mAddressList[removeLocation], mAddressList[0]);
         ok &= managerPtr->RemoveConnection(simTime, mAddressList[removeLocation - 1], mAddressList[removeLocation]);

         //! Re-connect
         ok &= managerPtr->AddConnection(simTime, mAddressList[removeLocation - 1], mAddressList[0]);
      }
      else if (removeLocation == 0)
      {
         //! The removed member is at the "beginning" of the ring.
         ok &= managerPtr->RemoveConnection(simTime, mAddressList[removeLocation], mAddressList[removeLocation + 1]);
         ok &= managerPtr->RemoveConnection(simTime, mAddressList.back(), mAddressList[removeLocation]);

         //! Re-connect
         ok &= managerPtr->AddConnection(simTime, mAddressList.back(), mAddressList[removeLocation + 1]);
      }
      else
      {
         ok &= managerPtr->RemoveConnection(simTime, mAddressList[removeLocation], mAddressList[removeLocation + 1]);
         ok &= managerPtr->RemoveConnection(simTime, mAddressList[removeLocation - 1], mAddressList[removeLocation]);

         //! Re-connect
         ok &= managerPtr->AddConnection(simTime, mAddressList[removeLocation - 1], mAddressList[removeLocation + 1]);
      }

      return ok;
   }
}

// =================================================================================================
ScriptNetworkRingClass::ScriptNetworkRingClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : ScriptNetworkClass(aClassName, aTypesPtr)
{
   SetClassName("WsfNetworkRing");
   mConstructible = true;
}

// =================================================================================================
void* ScriptNetworkRingClass::Create(const UtScriptContext& aInstance)
{
   return new NetworkRing();
}

// =================================================================================================
void ScriptNetworkRingClass::Destroy(void* aNetworkPtr)
{
   DestroyHelper<NetworkRing>(aNetworkPtr);
}

// =================================================================================================
ScriptNetworkDirectedRingClass::ScriptNetworkDirectedRingClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : ScriptNetworkRingClass(aClassName, aTypesPtr)
{
   SetClassName("WsfNetworkDirectedRing");
}

// =================================================================================================
void* ScriptNetworkDirectedRingClass::Create(const UtScriptContext& aInstance)
{
   return new NetworkDirectedRing();
}

// =================================================================================================
void ScriptNetworkDirectedRingClass::Destroy(void* aNetworkPtr)
{
   DestroyHelper<NetworkDirectedRing>(aNetworkPtr);
}

} // namespace comm
} // namespace wsf
