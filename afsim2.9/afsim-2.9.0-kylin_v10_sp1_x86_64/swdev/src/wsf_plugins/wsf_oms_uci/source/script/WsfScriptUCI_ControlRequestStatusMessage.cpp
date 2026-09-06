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

#include "WsfScriptUCI_ControlRequestStatusMessage.hpp"

#include "script/WsfScriptDefs.hpp"
#include "UtMemory.hpp"
#include "WsfUCI_Messages.hpp"
#include "WsfUCI_Types.hpp"

namespace wsf
{
namespace script
{

UCI_ControlRequestStatusMessageClass::UCI_ControlRequestStatusMessageClass(const std::string& aClassName,
                                                                           UtScriptTypes*     aTypesPtr)
   : UCI_MessageClass(aClassName, aTypesPtr)
{
   SetClassName(aClassName);

   AddMethod(ut::make_unique<GetRemarks>());
   AddMethod(ut::make_unique<IsApproved>());
   AddMethod(ut::make_unique<IsRejected>());
   AddMethod(ut::make_unique<IsPending>());
   AddMethod(ut::make_unique<IsDeleted>());
   AddMethod(ut::make_unique<UUID>());

}

UT_DEFINE_SCRIPT_METHOD(UCI_ControlRequestStatusMessageClass, UCI_ControlRequestStatusMessage, IsApproved, 0, "bool", "")
{
   aReturnVal.SetBool(UCI_Util::GetInterface(SIMULATION) != nullptr &&
                      aObjectPtr->Get().getMessageData().getApprovalType() == uci::type::ApprovalEnum::UCI_APPROVED);
}

UT_DEFINE_SCRIPT_METHOD(UCI_ControlRequestStatusMessageClass, UCI_ControlRequestStatusMessage, IsRejected, 0, "bool", "")
{
   aReturnVal.SetBool(UCI_Util::GetInterface(SIMULATION) != nullptr &&
                      aObjectPtr->Get().getMessageData().getApprovalType() == uci::type::ApprovalEnum::UCI_REJECTED);
}

UT_DEFINE_SCRIPT_METHOD(UCI_ControlRequestStatusMessageClass, UCI_ControlRequestStatusMessage, IsPending, 0, "bool", "")
{
   aReturnVal.SetBool(UCI_Util::GetInterface(SIMULATION) != nullptr &&
                      aObjectPtr->Get().getMessageData().getApprovalType() == uci::type::ApprovalEnum::UCI_PENDING);
}

UT_DEFINE_SCRIPT_METHOD(UCI_ControlRequestStatusMessageClass, UCI_ControlRequestStatusMessage, IsDeleted, 0, "bool", "")
{
   aReturnVal.SetBool(UCI_Util::GetInterface(SIMULATION) != nullptr &&
                      aObjectPtr->Get().getMessageData().getApprovalType() == uci::type::ApprovalEnum::UCI_DELETED);
}

UT_DEFINE_SCRIPT_METHOD(UCI_ControlRequestStatusMessageClass, UCI_ControlRequestStatusMessage, UUID, 0, "string", "")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr)
   {
      aReturnVal.SetString(aObjectPtr->Get().getMessageData().getControlRequestID().getUUID().getValueAsString());
   }
   else
   {
      aReturnVal.SetNullString();
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_ControlRequestStatusMessageClass, UCI_ControlRequestStatusMessage, GetRemarks, 0, "string", "")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr &&
       aObjectPtr->Get().getMessageData().hasRemarks())
   {
      aReturnVal.SetString(aObjectPtr->Get().getMessageData().getRemarks());
   }
   else
   {
      aReturnVal.SetNullString();
   }
}

} // script
} // wsf
