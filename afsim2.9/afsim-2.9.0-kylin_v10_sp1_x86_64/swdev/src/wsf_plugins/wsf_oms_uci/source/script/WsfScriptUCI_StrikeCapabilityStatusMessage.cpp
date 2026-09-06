// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfScriptUCI_StrikeCapabilityStatusMessage.hpp"

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

//========================================================================
UCI_StrikeCapabilityStatusTypeClass::UCI_StrikeCapabilityStatusTypeClass(const std::string& aClassName,
                                                                         UtScriptTypes*     aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName(aClassName);
   AddMethod(ut::make_unique<CapabilityId>());
   AddMethod(ut::make_unique<IsAvailable>());
   AddMethod(ut::make_unique<IsUnavailable>());
   AddMethod(ut::make_unique<IsTemporarilyUnavailable>());
   AddMethod(ut::make_unique<IsExpended>());
   AddMethod(ut::make_unique<IsDisabled>());
   AddMethod(ut::make_unique<IsFaulted>());
}

UT_DEFINE_SCRIPT_METHOD(UCI_StrikeCapabilityStatusTypeClass, UCI_StrikeCapabilityStatusType, CapabilityId, 0, "UCI_CapabilityId", "")
{
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   if (interfacePtr != nullptr)
   {
      auto capabilityIdPtr = new UCI_CapabilityID_Type(*interfacePtr,
                                                       aObjectPtr->Get().getCapabilityID());
      aReturnVal.SetPointer(new UtScriptRef(capabilityIdPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_StrikeCapabilityStatusTypeClass, UCI_StrikeCapabilityStatusType, IsAvailable, 0, "bool", "")
{
   aReturnVal.SetBool(UCI_Util::GetInterface(SIMULATION) != nullptr &&
                      aObjectPtr->Get().getAvailabilityInfo().getAvailability() == uci::type::CapabilityAvailabilityEnum::UCI_AVAILABLE);
}

UT_DEFINE_SCRIPT_METHOD(UCI_StrikeCapabilityStatusTypeClass, UCI_StrikeCapabilityStatusType, IsUnavailable, 0, "bool", "")
{
   aReturnVal.SetBool(UCI_Util::GetInterface(SIMULATION) != nullptr &&
                      aObjectPtr->Get().getAvailabilityInfo().getAvailability() == uci::type::CapabilityAvailabilityEnum::UCI_UNAVAILABLE);
}

UT_DEFINE_SCRIPT_METHOD(UCI_StrikeCapabilityStatusTypeClass, UCI_StrikeCapabilityStatusType, IsTemporarilyUnavailable, 0, "bool", "")
{
   aReturnVal.SetBool(UCI_Util::GetInterface(SIMULATION) != nullptr &&
                      aObjectPtr->Get().getAvailabilityInfo().getAvailability() == uci::type::CapabilityAvailabilityEnum::UCI_TEMPORARILY_UNAVAILABLE);
}

UT_DEFINE_SCRIPT_METHOD(UCI_StrikeCapabilityStatusTypeClass, UCI_StrikeCapabilityStatusType, IsExpended, 0, "bool", "")
{
   aReturnVal.SetBool(UCI_Util::GetInterface(SIMULATION) != nullptr &&
                      aObjectPtr->Get().getAvailabilityInfo().getAvailability() == uci::type::CapabilityAvailabilityEnum::UCI_EXPENDED);
}

UT_DEFINE_SCRIPT_METHOD(UCI_StrikeCapabilityStatusTypeClass, UCI_StrikeCapabilityStatusType, IsDisabled, 0, "bool", "")
{
   aReturnVal.SetBool(UCI_Util::GetInterface(SIMULATION) != nullptr &&
                      aObjectPtr->Get().getAvailabilityInfo().getAvailability() == uci::type::CapabilityAvailabilityEnum::UCI_DISABLED);
}

UT_DEFINE_SCRIPT_METHOD(UCI_StrikeCapabilityStatusTypeClass, UCI_StrikeCapabilityStatusType, IsFaulted, 0, "bool", "")
{
   aReturnVal.SetBool(UCI_Util::GetInterface(SIMULATION) != nullptr &&
                      aObjectPtr->Get().getAvailabilityInfo().getAvailability() == uci::type::CapabilityAvailabilityEnum::UCI_FAULTED);
}

UCI_StrikeCapabilityStatusMessageClass::UCI_StrikeCapabilityStatusMessageClass(const std::string& aClassName,
                                                                               UtScriptTypes*     aTypesPtr)
   : UCI_MessageClass(aClassName, aTypesPtr)
{
   SetClassName(aClassName);

   AddMethod(ut::make_unique<CapabilityStatus>());
   AddMethod(ut::make_unique<Size>());
}

UT_DEFINE_SCRIPT_METHOD(UCI_StrikeCapabilityStatusMessageClass, UCI_StrikeCapabilityStatusMessage, CapabilityStatus, 1, "UCI_StrikeCapabilityStatus", "int")
{
   bool set{ false };
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   if (interfacePtr != nullptr)
   {
      auto index = static_cast<size_t>(aVarArgs[0].GetInt());
      if (!aObjectPtr->Get().getMessageData().getCapabilityStatus().empty() &&
          index < aObjectPtr->Get().getMessageData().getCapabilityStatus().size())
      {
         set = true;
         auto capabilityStatusTypePtr = new UCI_StrikeCapabilityStatusType(*interfacePtr,
                                                                           aObjectPtr->Get().getMessageData().getCapabilityStatus().at(index));
         aReturnVal.SetPointer(new UtScriptRef(capabilityStatusTypePtr, aReturnClassPtr, UtScriptRef::cMANAGE));
      }
   }
   if (!set)
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_StrikeCapabilityStatusMessageClass, UCI_StrikeCapabilityStatusMessage, Size, 0, "int", "")
{
   aReturnVal.SetInt(UCI_Util::GetInterface(SIMULATION) != nullptr ?
                     ut::cast_to_int(aObjectPtr->Get().getMessageData().getCapabilityStatus().size()) : 0);
}

} // script
} // wsf
