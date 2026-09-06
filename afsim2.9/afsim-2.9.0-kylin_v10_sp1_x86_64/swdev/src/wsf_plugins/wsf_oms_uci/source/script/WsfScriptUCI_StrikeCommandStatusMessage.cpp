// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfScriptUCI_StrikeCommandStatusMessage.hpp"

#include "UtCast.hpp"
#include "UtMemory.hpp"
#include "UtScriptRef.hpp"
#include "UtScriptTypes.hpp"
#include "WsfUCI_Messages.hpp"
#include "WsfScriptUCI_Message.hpp"
#include "WsfScriptUCI_CommonTypes.hpp"
#include "script/WsfScriptDefs.hpp"
#include "WsfUCI_Types.hpp"

namespace wsf
{
namespace script
{

UCI_StrikeCommandStatusMessageClass::UCI_StrikeCommandStatusMessageClass(const std::string& aClassName,
                                                                         UtScriptTypes*     aTypesPtr)
   : UCI_MessageClass(aClassName, aTypesPtr)
{
   SetClassName(aClassName);

   AddMethod(ut::make_unique<CommandUUID>());
   AddMethod(ut::make_unique<IsAccepted>());
   AddMethod(ut::make_unique<IsRejected>());
   AddMethod(ut::make_unique<IsReceived>());
   AddMethod(ut::make_unique<IsRemoved>());
}

UT_DEFINE_SCRIPT_METHOD(UCI_StrikeCommandStatusMessageClass, UCI_StrikeCommandStatusMessage, CommandUUID, 0, "UCI_CommandId", "")
{
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   if (interfacePtr != nullptr)
   {
      auto commandID = new UCI_CapabilityCommandID_Type(*interfacePtr);
      commandID->Get().setUUID(aObjectPtr->Get().getMessageData().getCommandID().getUUID());
      aReturnVal.SetPointer(new UtScriptRef(commandID, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_StrikeCommandStatusMessageClass, UCI_StrikeCommandStatusMessage, IsAccepted, 0, "bool", "")
{
   if (UCI_Util::GetInterface(SIMULATION))
   {
      aReturnVal.SetBool(aObjectPtr->Get().getMessageData().getState() == uci::type::CommandStateEnum::UCI_ACCEPTED);
   }
   else
   {
      aReturnVal.SetBool(false);
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_StrikeCommandStatusMessageClass, UCI_StrikeCommandStatusMessage, IsReceived, 0, "bool", "")
{
   if (UCI_Util::GetInterface(SIMULATION))
   {
      aReturnVal.SetBool(aObjectPtr->Get().getMessageData().getState() == uci::type::CommandStateEnum::UCI_RECEIVED);
   }
   else
   {
      aReturnVal.SetBool(false);
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_StrikeCommandStatusMessageClass, UCI_StrikeCommandStatusMessage, IsRejected, 0, "bool", "")
{
   if (UCI_Util::GetInterface(SIMULATION))
   {
      aReturnVal.SetBool(aObjectPtr->Get().getMessageData().getState() == uci::type::CommandStateEnum::UCI_REJECTED);
   }
   else
   {
      aReturnVal.SetBool(false);
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_StrikeCommandStatusMessageClass, UCI_StrikeCommandStatusMessage, IsRemoved, 0, "bool", "")
{
   if (UCI_Util::GetInterface(SIMULATION))
   {
      aReturnVal.SetBool(aObjectPtr->Get().getMessageData().getState() == uci::type::CommandStateEnum::UCI_REMOVED);
   }
   else
   {
      aReturnVal.SetBool(false);
   }
}

} // script
} // wsf
