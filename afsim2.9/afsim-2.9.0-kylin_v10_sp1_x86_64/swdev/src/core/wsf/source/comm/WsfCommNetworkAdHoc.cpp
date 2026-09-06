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

#include "WsfCommNetworkAdHoc.hpp"

#include <iostream>

#include "UtInputBlock.hpp"
#include "WsfCommEvent.hpp"
#include "WsfCommNetworkManager.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "script/WsfScriptDefs.hpp"

namespace wsf
{
namespace comm
{

// =================================================================================================
NetworkAdHoc::NetworkAdHoc(const std::string& aNetworkName, const Address& aAddress)
   : NetworkGeneric(aNetworkName, aAddress)
{
}

// =================================================================================================
NetworkAdHoc::NetworkAdHoc(const NetworkAdHoc& aSrc)
   : NetworkGeneric(aSrc)
   , mUpdateRate(aSrc.mUpdateRate)
   , mNamedRate(aSrc.mNamedRate)
   , mAddressRate(aSrc.mAddressRate)
   , mEvents(std::make_shared<int>(0))
{
}

// =================================================================================================
NetworkAdHoc& NetworkAdHoc::operator=(const NetworkAdHoc& aRhs)
{
   if (this != &aRhs)
   {
      NetworkGeneric::operator=(aRhs);
      mUpdateRate             = aRhs.mUpdateRate;
      mNamedRate              = aRhs.mNamedRate;
      mAddressRate            = aRhs.mAddressRate;
      mEvents                 = std::make_shared<int>(0);
   }

   return *this;
}

// =================================================================================================
NetworkAdHoc* NetworkAdHoc::Clone() const
{
   return new NetworkAdHoc(*this);
}

// =================================================================================================
bool NetworkAdHoc::Initialize(WsfSimulation& aSim)
{
   // Schedule all of the network update events.
   auto simTime = aSim.GetSimTime();

   // Schedule the generic network update.
   mUpdateRate.SetGenerator(aSim);
   auto eventTime = simTime + mUpdateRate.Draw();
   aSim.AddEvent(ut::make_unique<GenericEvent<int>>(eventTime, mEvents, [this, &aSim]() { Update(aSim); }));

   // Schedule the named comm events
   for (auto& namedMapEntry : mNamedRate)
   {
      namedMapEntry.second.SetGenerator(aSim);
      eventTime       = simTime + namedMapEntry.second.Draw();
      auto NamedEntry = namedMapEntry.first;
      aSim.AddEvent(
         ut::make_unique<GenericEvent<int>>(eventTime, mEvents, [this, NamedEntry, &aSim]() { Update(aSim, NamedEntry); }));
   }

   // Schedule the address events
   for (auto& addressMapEntry : mAddressRate)
   {
      addressMapEntry.second.SetGenerator(aSim);
      eventTime         = simTime + addressMapEntry.second.Draw();
      auto AddressEntry = addressMapEntry.first;
      aSim.AddEvent(ut::make_unique<GenericEvent<int>>(eventTime,
                                                       mEvents,
                                                       [this, AddressEntry, &aSim]() { Update(aSim, AddressEntry); }));
   }

   return true;
}

// =================================================================================================
void NetworkAdHoc::ChangeRate(WsfSimulation& aSim, const WsfRandomVariable& aRate)
{
   if (mUpdateRate.Draw() == std::numeric_limits<double>::max())
   {
      mUpdateRate = aRate;
      mUpdateRate.SetGenerator(aSim);
      auto eventTime = aSim.GetSimTime() + mUpdateRate.Draw();
      aSim.AddEvent(ut::make_unique<GenericEvent<int>>(eventTime, mEvents, [this, &aSim]() { Update(aSim); }));
   }
   else
   {
      mUpdateRate = aRate;
   }
}

// =================================================================================================
bool NetworkAdHoc::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "update_rate")
   {
      mUpdateRate.ProcessInput(aInput, UtInput::cTIME);
   }
   else if (ProcessAddressRate(aInput))
   {
   }
   else if (ProcessNamedRate(aInput))
   {
   }
   else if (NetworkGeneric::ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

// =================================================================================================
bool NetworkAdHoc::ProcessNamedRate(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "comm_update_rates")
   {
      UtInputBlock inputBlock(aInput, "end_comm_update_rates");
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

         input.ReadValue(command);
         if (command != "update_rate")
         {
            throw UtInput::BadValue(input);
         }

         WsfRandomVariable rate;
         rate.ProcessInput(input, UtInput::cTIME);

         AddNamedRate(namedComm, rate);
      }
   }
   else if (command == "remove_comm_update_rate")
   {
      NamedComm namedComm;
      aInput.ReadValue(namedComm.mPlatformName);
      aInput.ReadValue(namedComm.mCommName);
      RemoveNamedRate(namedComm);
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

// =================================================================================================
bool NetworkAdHoc::ProcessAddressRate(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "address_update_rates")
   {
      UtInputBlock inputBlock(aInput, "end_address_update_rates");
      while (inputBlock.ReadCommand())
      {
         auto& input = inputBlock.GetInput();
         command     = input.GetCommand();
         if (command != "member")
         {
            throw UtInput::BadValue(input);
         }

         auto address = Address::ProcessInput(input);

         input.ReadValue(command);
         if (command != "update_rate")
         {
            throw UtInput::BadValue(input);
         }

         WsfRandomVariable rate;
         rate.ProcessInput(input, UtInput::cTIME);

         AddAddressRate(address, rate);
      }
   }
   else if (command == "remove_address_update_rate")
   {
      auto address = Address::ProcessInput(aInput);
      RemoveAddressRate(address);
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

// =================================================================================================
void NetworkAdHoc::AddNamedRate(const NamedComm& aComm, const WsfRandomVariable& aRate, WsfSimulation* aSimPtr)
{
   auto& namedRate = mNamedRate[aComm];
   namedRate       = aRate;

   if (aSimPtr)
   {
      auto eventTime = aSimPtr->GetSimTime() + namedRate.Draw();
      aSimPtr->AddEvent(
         ut::make_unique<GenericEvent<int>>(eventTime, mEvents, [this, aComm, aSimPtr]() { Update(*aSimPtr, aComm); }));
   }
}

// =================================================================================================
void NetworkAdHoc::AddAddressRate(const Address& aAddress, const WsfRandomVariable& aRate, WsfSimulation* aSimPtr)
{
   auto& addressRate = mAddressRate[aAddress];
   addressRate       = aRate;

   if (aSimPtr)
   {
      auto eventTime = aSimPtr->GetSimTime() + addressRate.Draw();
      aSimPtr->AddEvent(ut::make_unique<GenericEvent<int>>(eventTime,
                                                           mEvents,
                                                           [this, aAddress, aSimPtr]() { Update(*aSimPtr, aAddress); }));
   }
}

// =================================================================================================
bool NetworkAdHoc::RemoveNamedRate(const NamedComm& aComm)
{
   auto it = mNamedRate.find(aComm);
   if (it != std::end(mNamedRate))
   {
      mNamedRate.erase(it);
      return true;
   }

   return false;
}

// =================================================================================================
bool NetworkAdHoc::RemoveAddressRate(const Address& aAddress)
{
   auto it = mAddressRate.find(aAddress);
   if (it != std::end(mAddressRate))
   {
      mAddressRate.erase(it);
      return true;
   }

   return false;
}

// =================================================================================================
WsfRandomVariable* NetworkAdHoc::GetNamedRate(const NamedComm& aComm)
{
   auto it = mNamedRate.find(aComm);
   if (it != std::end(mNamedRate))
   {
      return &it->second;
   }
   return nullptr;
}

// =================================================================================================
WsfRandomVariable* NetworkAdHoc::GetAddressRate(const Address& aAddress)
{
   auto it = mAddressRate.find(aAddress);
   if (it != std::end(mAddressRate))
   {
      return &it->second;
   }
   return nullptr;
}

// =================================================================================================
void NetworkAdHoc::Update(WsfSimulation& aSim)
{
   // NOTE: This is an expensive call. It's O(n^2) by nature of comparing every comm against
   // every other comm in the network. Much like sensor detections, there isn't any way around this
   // without approximations, akin to what is done for n-body problems.
   for (const auto& address : mAddressList)
   {
      auto commPtr = aSim.GetCommNetworkManager()->GetComm(address);
      if (commPtr)
      {
         if ((!GetAddressRate(address)) && (!GetNamedRate({commPtr->GetPlatform()->GetNameId(), commPtr->GetNameId()})))
         {
            // Not a specified member rate - uses generic rate for entire network
            for (const auto& otherAddress : mAddressList)
            {
               if (address != otherAddress)
               {
                  auto otherCommPtr = aSim.GetCommNetworkManager()->GetComm(otherAddress);
                  if (otherCommPtr)
                  {
                     if ((commPtr->GetRouter() == otherCommPtr->GetRouter()) ||
                         (commPtr->CanSendTo(aSim.GetSimTime(), otherCommPtr)))
                     {
                        AddLink(address, otherAddress, aSim);
                     }
                     else
                     {
                        RemoveLink(address, otherAddress, aSim);
                     }
                  }
               }
            }
         }
      }
   }

   // Schedule the next generic update event.
   auto eventTime = aSim.GetSimTime() + mUpdateRate.Draw();
   aSim.AddEvent(ut::make_unique<GenericEvent<int>>(eventTime, mEvents, [this, &aSim]() { Update(aSim); }));
}

// =================================================================================================
void NetworkAdHoc::Update(WsfSimulation& aSim, const NamedComm& aComm)
{
   // If this member is no longer specified in the rate map, abort
   auto it = mNamedRate.find(aComm);
   if (it == std::end(mNamedRate))
   {
      return;
   }

   auto platformPtr = aSim.GetPlatformByName(aComm.mPlatformName);
   if (platformPtr)
   {
      auto commPtr = platformPtr->GetComponent<Comm>(aComm.mCommName);
      if (commPtr)
      {
         for (const auto& address : mAddressList)
         {
            auto otherCommPtr = aSim.GetCommNetworkManager()->GetComm(address);
            if (otherCommPtr && (commPtr != otherCommPtr))
            {
               if ((commPtr->GetRouter() == otherCommPtr->GetRouter()) ||
                   (commPtr->CanSendTo(aSim.GetSimTime(), otherCommPtr)))
               {
                  AddLink(commPtr->GetAddress(), address, aSim);
               }
               else
               {
                  RemoveLink(commPtr->GetAddress(), address, aSim);
               }
            }
         }

         // Only reschedule if the comm existed, and it is still in the map.
         auto eventTime = aSim.GetSimTime() + it->second.Draw();
         aSim.AddEvent(
            ut::make_unique<GenericEvent<int>>(eventTime, mEvents, [this, aComm, &aSim]() { Update(aSim, aComm); }));
      }
   }
}

// =================================================================================================
void NetworkAdHoc::Update(WsfSimulation& aSim, const Address& aAddress)
{
   // If this member is no longer specified in the rate map, abort
   auto it = mAddressRate.find(aAddress);
   if (it == std::end(mAddressRate))
   {
      return;
   }

   auto commPtr = aSim.GetCommNetworkManager()->GetComm(aAddress);
   if (commPtr)
   {
      for (const auto& address : mAddressList)
      {
         auto otherCommPtr = aSim.GetCommNetworkManager()->GetComm(address);
         if (otherCommPtr && (commPtr != otherCommPtr))
         {
            if ((commPtr->GetRouter() == otherCommPtr->GetRouter()) ||
                (commPtr->CanSendTo(aSim.GetSimTime(), otherCommPtr)))
            {
               AddLink(aAddress, address, aSim);
            }
            else
            {
               RemoveLink(aAddress, address, aSim);
            }
         }
      }

      // Only reschedule if the comm existed, and it is still in the map.
      auto eventTime = aSim.GetSimTime() + it->second.Draw();
      aSim.AddEvent(
         ut::make_unique<GenericEvent<int>>(eventTime, mEvents, [this, aAddress, &aSim]() { Update(aSim, aAddress); }));
   }
}

// =================================================================================================
ScriptNetworkAdHocClass::ScriptNetworkAdHocClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : ScriptNetworkGenericClass(aClassName, aTypesPtr)
{
   SetClassName("WsfNetworkAdHoc");
   mConstructible = true;

   AddMethod(ut::make_unique<ChangeUpdateRate>());
   AddMethod(ut::make_unique<AddMemberUpdateRate>());
   AddMethod(ut::make_unique<RemoveMemberUpdateRate>());
}

// =================================================================================================
void* ScriptNetworkAdHocClass::Create(const UtScriptContext& aInstance)
{
   return new NetworkAdHoc();
}

// =================================================================================================
void ScriptNetworkAdHocClass::Destroy(void* aNetworkPtr)
{
   DestroyHelper<NetworkAdHoc>(aNetworkPtr);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptNetworkAdHocClass, NetworkAdHoc, ChangeUpdateRate, 1, "void", "WsfRandomVariable")
{
   auto ratePtr = aVarArgs[0].GetPointer()->GetAppObject<WsfRandomVariable>();
   aObjectPtr->ChangeRate(*SIMULATION, *ratePtr);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptNetworkAdHocClass, NetworkAdHoc, AddMemberUpdateRate, 2, "bool", "WsfAddress, WsfRandomVariable")
{
   auto addressPtr = aVarArgs[0].GetPointer()->GetAppObject<Address>();
   auto ratePtr    = aVarArgs[1].GetPointer()->GetAppObject<WsfRandomVariable>();

   auto oldRatePtr = aObjectPtr->GetAddressRate(*addressPtr);
   if (oldRatePtr)
   {
      (*oldRatePtr) = *ratePtr;
      aReturnVal.SetBool(true);
      return;
   }

   Comm*              commPtr = nullptr;
   Network::NamedComm namedComm;
   if (addressPtr)
   {
      commPtr = SIMULATION->GetCommNetworkManager()->GetComm(*addressPtr);
      if (commPtr)
      {
         namedComm.mPlatformName = commPtr->GetPlatform()->GetNameId();
         namedComm.mCommName     = commPtr->GetNameId();

         oldRatePtr = aObjectPtr->GetNamedRate(namedComm);
         if (oldRatePtr)
         {
            (*oldRatePtr) = *ratePtr;
            aReturnVal.SetBool(true);
            return;
         }
      }
   }

   auto networkAddresses = SIMULATION->GetCommNetworkManager()->GetAddressesInNetwork(aObjectPtr->GetType());
   auto it               = std::find(std::begin(networkAddresses), std::end(networkAddresses), *addressPtr);
   if (it != std::end(networkAddresses))
   {
      aObjectPtr->AddAddressRate(*addressPtr, *ratePtr, SIMULATION);
      aReturnVal.SetBool(true);
   }
   else
   {
      aReturnVal.SetBool(false);
   }
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptNetworkAdHocClass, NetworkAdHoc, RemoveMemberUpdateRate, 1, "bool", "WsfAddress")
{
   auto addressPtr = aVarArgs[0].GetPointer()->GetAppObject<Address>();
   auto result     = aObjectPtr->RemoveAddressRate(*addressPtr);

   aReturnVal.SetBool(result);
}

} // namespace comm
} // namespace wsf
