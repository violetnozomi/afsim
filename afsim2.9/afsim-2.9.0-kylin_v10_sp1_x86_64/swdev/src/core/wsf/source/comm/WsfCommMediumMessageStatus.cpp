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

#include "WsfCommMediumMessageStatus.hpp"

namespace wsf
{
namespace comm
{
namespace medium
{

// =================================================================================================
ScriptMediumMessageStatusClass::ScriptMediumMessageStatusClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfCommMediumMessageStatus");
   mConstructible      = false;
   mCloneable          = false;
   mIsScriptAccessible = false;

   AddMethod(ut::make_unique<GetMessage>());
   AddMethod(ut::make_unique<IsTransmitting>());
   AddMethod(ut::make_unique<GetTimeStart>());
   AddMethod(ut::make_unique<GetTimeTransmissionEnd>());
   AddMethod(ut::make_unique<GetTimeDelivery>());
   AddMethod(ut::make_unique<GetDeliveryFailure>());

   AddMethod(ut::make_unique<SetDeliveryFailure>());

   WsfScriptAuxDataUtil::AddAuxDataScriptMethods(*this);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptMediumMessageStatusClass, MessageStatus, GetMessage, 0, "WsfCommMessage", "")
{
   aReturnVal.SetPointer(UtScriptRef::Ref(&aObjectPtr->GetMessage(), aReturnClassPtr));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptMediumMessageStatusClass, MessageStatus, IsTransmitting, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsTransmitting());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptMediumMessageStatusClass, MessageStatus, GetTimeStart, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetTimeStart());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptMediumMessageStatusClass, MessageStatus, GetTimeTransmissionEnd, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetTimeTransmissionEnd());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptMediumMessageStatusClass, MessageStatus, GetTimeDelivery, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetTimeDelivery());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptMediumMessageStatusClass, MessageStatus, GetDeliveryFailure, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->AbortDelivery());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptMediumMessageStatusClass, MessageStatus, SetDeliveryFailure, 0, "bool", "")
{
   bool aborted = true;

   if (aObjectPtr->IsScheduledDelivery() || aObjectPtr->AbortDelivery())
   {
      aborted = false;
   }
   else
   {
      aObjectPtr->SetAbortDelivery(true);
   }

   aReturnVal.SetBool(aborted);
}

UT_DEFINE_AUX_DATA_SCRIPT_METHODS(ScriptMediumMessageStatusClass, MessageStatus)

} // namespace medium
} // namespace comm
} // namespace wsf
