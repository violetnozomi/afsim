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

#include "script/WsfScriptMessageClass.hpp"

#include "UtMemory.hpp"
#include "WsfCommAddress.hpp"
#include "WsfMessage.hpp"
#include "WsfSimulation.hpp"
#include "script/WsfScriptCommAddress.hpp"
#include "script/WsfScriptContext.hpp"

// =================================================================================================
WsfScriptMessageClass::WsfScriptMessageClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName("WsfMessage");
   mIsScriptAccessible = true;
   mConstructible      = true;
   mCloneable          = true;

   // Add each of the method objects to the class.

   AddMethod(ut::make_unique<Originator>());
   AddMethod(ut::make_unique<SizeInBits>());
   AddMethod(ut::make_unique<SetSizeInBits>());
   AddMethod(ut::make_unique<SizeInBytes>());
   AddMethod(ut::make_unique<SetSizeInBytes>());
   AddMethod(ut::make_unique<Priority>());
   AddMethod(ut::make_unique<SetPriority>());
   AddMethod(ut::make_unique<SerialNumber>());
   AddMethod(ut::make_unique<DataTag>());
   AddMethod(ut::make_unique<SetDataTag>());
   AddMethod(ut::make_unique<CreateDataTag>());
   AddMethod(ut::make_unique<Type>());
   AddMethod(ut::make_unique<SubType>());
   AddMethod(ut::make_unique<SetType>());
   AddMethod(ut::make_unique<SetSubType>());
   AddMethod(ut::make_unique<Destination>());
   AddMethod(ut::make_unique<NextHop>());

   WsfScriptAuxDataUtil::AddAuxDataScriptMethods(*this);
}

// =================================================================================================
// virtual
WsfScriptMessageClass::~WsfScriptMessageClass() {}

// =================================================================================================
// virtual
void* WsfScriptMessageClass::Create(const UtScriptContext& aInstance)
{
   WsfPlatform* platformPtr = WsfScriptContext::GetPLATFORM(aInstance);

   if (platformPtr != nullptr)
   {
      return new WsfMessage(platformPtr);
   }
   else
   {
      // This creates a message that won't cause a crash if sent.
      WsfMessage*    messagePtr = new WsfMessage(WsfMessage::GetTypeId());
      WsfSimulation* simPtr     = WsfScriptContext::GetSIMULATION(aInstance);
      messagePtr->SetSimulation(simPtr);
      messagePtr->SetSerialNumber(simPtr->NextMessageSerialNumber());
      return messagePtr;
   }
}

// =================================================================================================
// virtual
void* WsfScriptMessageClass::Clone(void* aObjectPtr)
{
   return static_cast<WsfMessage*>(aObjectPtr)->Clone();
}

// =================================================================================================
// virtual
void WsfScriptMessageClass::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfMessage*>(aObjectPtr);
}

#include "script/WsfScriptDefs.hpp"

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptMessageClass, WsfMessage, Originator, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetOriginator());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptMessageClass, WsfMessage, SizeInBits, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetSizeBits());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptMessageClass, WsfMessage, SetSizeInBits, 1, "void", "int")
{
   aObjectPtr->SetSizeBits(aVarArgs[0].GetInt());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptMessageClass, WsfMessage, SizeInBytes, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetSizeBytes());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptMessageClass, WsfMessage, SetSizeInBytes, 1, "void", "int")
{
   aObjectPtr->SetSizeBytes(aVarArgs[0].GetInt());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptMessageClass, WsfMessage, Priority, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetPriority());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptMessageClass, WsfMessage, SetPriority, 1, "void", "int")
{
   aObjectPtr->SetPriority(aVarArgs[0].GetInt());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptMessageClass, WsfMessage, SerialNumber, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(aObjectPtr->GetSerialNumber()));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptMessageClass, WsfMessage, DataTag, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetDataTag());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptMessageClass, WsfMessage, SetDataTag, 1, "void", "double")
{
   aObjectPtr->SetDataTag(aVarArgs[0].GetDouble());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptMessageClass, WsfMessage, CreateDataTag, 0, "void", "")
{
   aObjectPtr->SetDataTag(SIMULATION->CreateMessageDataTag());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptMessageClass, WsfMessage, Type, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetType());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptMessageClass, WsfMessage, SubType, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetSubType());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptMessageClass, WsfMessage, SetType, 1, "void", "string")
{
   aObjectPtr->SetType(aVarArgs[0].GetString());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptMessageClass, WsfMessage, SetSubType, 1, "void", "string")
{
   aObjectPtr->SetSubType(aVarArgs[0].GetString());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptMessageClass, WsfMessage, Destination, 0, "WsfAddress", "")
{
   aReturnVal.SetPointer(WsfScriptCommAddressClass::Create(aObjectPtr->GetDstAddr()));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptMessageClass, WsfMessage, NextHop, 0, "WsfAddress", "")
{
   aReturnVal.SetPointer(WsfScriptCommAddressClass::Create(aObjectPtr->GetNextHopAddr()));
}

UT_DEFINE_AUX_DATA_SCRIPT_METHODS(WsfScriptMessageClass, WsfMessage)
