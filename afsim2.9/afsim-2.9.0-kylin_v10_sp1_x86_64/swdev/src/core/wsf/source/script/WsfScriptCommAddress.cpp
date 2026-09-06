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

#include "WsfScriptCommAddress.hpp"

#include "UtMemory.hpp"

WsfScriptCommAddressClass* WsfScriptCommAddressClass::sClassPtr = nullptr;

// =================================================================================================
WsfScriptCommAddressClass::WsfScriptCommAddressClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   sClassPtr           = this;
   mConstructible      = true;
   mCloneable          = true;
   mIsScriptAccessible = true;

   SetClassName("WsfAddress");

   AddStaticMethod(ut::make_unique<WithinRange>());
   AddStaticMethod(ut::make_unique<IntegerValue>());

   AddMethod(ut::make_unique<SetAddress>());
   AddMethod(ut::make_unique<GetAddress>());
   AddMethod(ut::make_unique<SubnetMaskBitSize>());
   AddMethod(ut::make_unique<RoutingPrefix>());
   AddMethod(ut::make_unique<Subnet>());
   AddMethod(ut::make_unique<PossibleHosts>());
   AddMethod(ut::make_unique<IsNull>());
   AddMethod(ut::make_unique<Increment>());
   AddMethod(ut::make_unique<AddressToken>());
   AddMethod(ut::make_unique<GetBroadcastAddress>());
}

// =================================================================================================
WsfScriptCommAddressClass::~WsfScriptCommAddressClass() {}

// =================================================================================================
UtScriptRef* WsfScriptCommAddressClass::Create(const wsf::comm::Address& aAddress)
{
   return new UtScriptRef(new wsf::comm::Address(aAddress), sClassPtr, UtScriptRef::cMANAGE);
}

// =================================================================================================
void* WsfScriptCommAddressClass::Create(const UtScriptContext& aContext)
{
   return new wsf::comm::Address;
}

// =================================================================================================
void WsfScriptCommAddressClass::Destroy(void* aObject)
{
   delete static_cast<wsf::comm::Address*>(aObject);
}

// =================================================================================================
void* WsfScriptCommAddressClass::Clone(void* aObject)
{
   return new wsf::comm::Address(*static_cast<wsf::comm::Address*>(aObject));
}

// =================================================================================================
std::string WsfScriptCommAddressClass::ToString(void* aObjectPtr) const
{
   std::string address = "";
   if (aObjectPtr != nullptr)
   {
      wsf::comm::Address& addy = *static_cast<wsf::comm::Address*>(aObjectPtr);
      address                  = addy.GetAddress() + "/" + std::to_string(addy.GetSubnetMaskBitLength());
   }

   return address;
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommAddressClass, wsf::comm::Address, WithinRange, 2, "bool", "WsfAddress, WsfAddress")
{
   bool withinRange       = false;
   auto networkAddressPtr = static_cast<wsf::comm::Address*>(aVarArgs[0].GetPointer()->GetAppObject());
   auto checkAddressPtr   = static_cast<wsf::comm::Address*>(aVarArgs[1].GetPointer()->GetAppObject());

   withinRange = wsf::comm::Address::CheckInAddressRange(*networkAddressPtr, *checkAddressPtr);

   aReturnVal.SetBool(withinRange);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommAddressClass, wsf::comm::Address, SetAddress, 2, "bool", "string, int")
{
   bool success = false;

   std::string         addressString = aVarArgs[0].GetString();
   int                 cidrValSigned = aVarArgs[1].GetInt();
   wsf::comm::Address* addressPtr    = aObjectPtr;
   size_t              cidrVal;

   //! Ensure the integer value is positive.
   if (cidrValSigned >= 0)
   {
      cidrVal = static_cast<size_t>(cidrValSigned);
   }
   else
   {
      aReturnVal.SetBool(success);
      return;
   }

   if (addressPtr)
   {
      addressPtr->SetAddress(addressString, cidrVal);
      success = !addressPtr->IsNull();
   }

   aReturnVal.SetBool(success);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommAddressClass, wsf::comm::Address, GetAddress, 0, "string", "")
{
   std::string         address    = "";
   wsf::comm::Address* addressPtr = aObjectPtr;

   if (addressPtr)
   {
      address = addressPtr->GetAddress();
   }

   aReturnVal.SetString(address);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommAddressClass, wsf::comm::Address, SubnetMaskBitSize, 0, "int", "")
{
   int                 cidr       = -1;
   wsf::comm::Address* addressPtr = aObjectPtr;

   if (addressPtr)
   {
      cidr = static_cast<int>(addressPtr->GetSubnetMaskBitLength());
   }

   aReturnVal.SetInt(cidr);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommAddressClass, wsf::comm::Address, RoutingPrefix, 0, "string", "")
{
   std::string         prefix     = "";
   wsf::comm::Address* addressPtr = aObjectPtr;

   if (addressPtr)
   {
      prefix = addressPtr->GetRoutingPrefix();
   }

   aReturnVal.SetString(prefix);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommAddressClass, wsf::comm::Address, Subnet, 0, "string", "")
{
   std::string         subnet     = "";
   wsf::comm::Address* addressPtr = aObjectPtr;

   if (addressPtr)
   {
      subnet = addressPtr->GetSubnet();
   }

   aReturnVal.SetString(subnet);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommAddressClass, wsf::comm::Address, PossibleHosts, 0, "double", "")
{
   //! Note:
   //! Since we don't have unsigned return types, we're forced to use a double since the return value
   //! may exceed the limit of a signed integer.

   size_t              hosts       = 0;
   double              hostsDouble = 0.0;
   wsf::comm::Address* addressPtr  = aObjectPtr;

   if (addressPtr)
   {
      hosts = addressPtr->GetNumHostAddresses();
   }

   hostsDouble = static_cast<double>(hosts);
   aReturnVal.SetDouble(hostsDouble);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommAddressClass, wsf::comm::Address, IsNull, 0, "bool", "")
{
   bool                isNull     = true;
   wsf::comm::Address* addressPtr = aObjectPtr;

   if (addressPtr)
   {
      isNull = addressPtr->IsNull();
   }

   aReturnVal.SetBool(isNull);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommAddressClass, wsf::comm::Address, Increment, 0, "void", "")
{
   wsf::comm::Address* addressPtr = aObjectPtr;

   if (addressPtr)
   {
      (*addressPtr)++;
   }
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommAddressClass, wsf::comm::Address, IntegerValue, 0, "double", "")
{
   size_t              val        = 0;
   double              valDouble  = 0.0;
   wsf::comm::Address* addressPtr = aObjectPtr;

   if (addressPtr && (!(*addressPtr).IsNull()))
   {
      val       = (*addressPtr).GetBinaryInteger();
      valDouble = static_cast<double>(val);
      aReturnVal.SetDouble(valDouble);
      return;
   }

   aReturnVal.SetDouble(-1.0);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommAddressClass, wsf::comm::Address, AddressToken, 0, "string", "")
{
   std::string         token      = "";
   wsf::comm::Address* addressPtr = aObjectPtr;

   if (addressPtr)
   {
      token = addressPtr->GetGenerationToken();
   }

   aReturnVal.SetString(token);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommAddressClass, wsf::comm::Address, GetBroadcastAddress, 0, "string", "")
{
   std::string broadcastString = aObjectPtr->GetBroadcastAddress();
   aReturnVal.SetString(broadcastString);
}
