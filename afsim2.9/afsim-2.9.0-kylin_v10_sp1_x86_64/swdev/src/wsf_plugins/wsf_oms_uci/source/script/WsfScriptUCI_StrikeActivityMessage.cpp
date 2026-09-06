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

#include "WsfScriptUCI_StrikeActivityMessage.hpp"

#include "UtMemory.hpp"

#include "script/WsfScriptDefs.hpp"
#include "WsfUCI_Messages.hpp"
#include "WsfUCI_Types.hpp"

namespace wsf
{
namespace script
{

UCI_StrikeActivityMessageClass::UCI_StrikeActivityMessageClass(const std::string& aClassName,
                                                               UtScriptTypes*     aTypesPtr)
   : UCI_MessageClass(aClassName, aTypesPtr)
{
   SetClassName(aClassName);

   AddMethod(ut::make_unique<Activity>());
   AddMethod(ut::make_unique<Size>());
}

UT_DEFINE_SCRIPT_METHOD(UCI_StrikeActivityMessageClass, UCI_StrikeActivityMessage, Activity, 1, "UCI_StrikeActivity", "int")
{
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   bool set{ false };
   if (interfacePtr != nullptr)
   {
      auto index = static_cast<size_t>(aVarArgs[0].GetInt());
      if (index < aObjectPtr->Get().getMessageData().getActivity().size())
      {
         set = true;
         auto strikeActivityPtr = new UCI_StrikeActivityType(*interfacePtr,
                                                             aObjectPtr->Get().getMessageData().getActivity().at(index));
         aReturnVal.SetPointer(new UtScriptRef(strikeActivityPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
      }
   }
   if (!set)
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_StrikeActivityMessageClass, UCI_StrikeActivityMessage, Size, 0, "int", "")
{
   int size{ 0 };
   if (UCI_Util::GetInterface(SIMULATION))
   {
      size = ut::cast_to_int(aObjectPtr->Get().getMessageData().getActivity().size());
   }
   aReturnVal.SetInt(size);
}

//====================================================
UCI_StrikeActivityTypeClass::UCI_StrikeActivityTypeClass(const std::string& aClassName,
                                                         UtScriptTypes*     aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName(aClassName);

   AddMethod(ut::make_unique<ActivityId>());
   AddMethod(ut::make_unique<CapabilityId>());
   AddMethod(ut::make_unique<Size>());

   AddMethod(ut::make_unique<IsReadyForRelease>());
   AddMethod(ut::make_unique<IsSelectedForJettison>());
   AddMethod(ut::make_unique<IsSelectedForKeyLoad>());
   AddMethod(ut::make_unique<IsSelectedForRelease>());
   AddMethod(ut::make_unique<IsWeaponArmed>());

   AddMethod(ut::make_unique<HasFailed>());
   AddMethod(ut::make_unique<IsActiveFullyConstrained>());
   AddMethod(ut::make_unique<IsActivePartiallyConstrained>());
   AddMethod(ut::make_unique<IsActiveUnconstrained>());
   AddMethod(ut::make_unique<IsCompleted>());
   AddMethod(ut::make_unique<IsDeleted>());
   AddMethod(ut::make_unique<IsDisabled>());
   AddMethod(ut::make_unique<IsEnabled>());
}

UT_DEFINE_SCRIPT_METHOD(UCI_StrikeActivityTypeClass, UCI_StrikeActivityType, ActivityId, 0, "UCI_ActivityId", "")
{
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   if (interfacePtr)
   {
      auto activityID = new UCI_ActivityID_Type(*interfacePtr,
                                                aObjectPtr->Get().getActivityID());
      aReturnVal.SetPointer(new UtScriptRef(activityID, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_StrikeActivityTypeClass, UCI_StrikeActivityType, CapabilityId, 1, "UCI_CapabilityId", "int")
{
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   bool set{ false };
   if (interfacePtr != nullptr)
   {
      auto index = static_cast<size_t>(aVarArgs[0].GetInt());
      if (index < aObjectPtr->Get().getCapabilityID().size())
      {
         set = true;
         auto capabilityID = new UCI_CapabilityID_Type(*interfacePtr,
                                                       aObjectPtr->Get().getCapabilityID().at(index));
         aReturnVal.SetPointer(new UtScriptRef(capabilityID, aReturnClassPtr, UtScriptRef::cMANAGE));
      }
   }
   if (!set)
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_StrikeActivityTypeClass, UCI_StrikeActivityType, Size, 0, "int", "")
{
   int size{ 0 };
   if (UCI_Util::GetInterface(SIMULATION))
   {
      size = ut::cast_to_int(aObjectPtr->Get().getCapabilityID().size());
   }
   aReturnVal.SetInt(size);
}

UT_DEFINE_SCRIPT_METHOD(UCI_StrikeActivityTypeClass, UCI_StrikeActivityType, IsReadyForRelease, 0, "bool", "")
{
   bool ready{ false };
   if (UCI_Util::GetInterface(SIMULATION))
   {
      ready = aObjectPtr->Get().getReadyForRelease();
   }
   aReturnVal.SetBool(ready);
}

UT_DEFINE_SCRIPT_METHOD(UCI_StrikeActivityTypeClass, UCI_StrikeActivityType, IsSelectedForJettison, 0, "bool", "")
{
   bool selected{ false };
   if (UCI_Util::GetInterface(SIMULATION))
   {
      selected = aObjectPtr->Get().getSelectedForJettison();
   }
   aReturnVal.SetBool(selected);
}

UT_DEFINE_SCRIPT_METHOD(UCI_StrikeActivityTypeClass, UCI_StrikeActivityType, IsSelectedForKeyLoad, 0, "bool", "")
{
   bool selected{ false };
   if (UCI_Util::GetInterface(SIMULATION))
   {
      selected = aObjectPtr->Get().getSelectedForKeyLoad();
   }
   aReturnVal.SetBool(selected);
}

UT_DEFINE_SCRIPT_METHOD(UCI_StrikeActivityTypeClass, UCI_StrikeActivityType, IsSelectedForRelease, 0, "bool", "")
{
   bool selected{ false };
   if (UCI_Util::GetInterface(SIMULATION))
   {
      selected = aObjectPtr->Get().getSelectedForRelease();
   }
   aReturnVal.SetBool(selected);
}

UT_DEFINE_SCRIPT_METHOD(UCI_StrikeActivityTypeClass, UCI_StrikeActivityType, IsWeaponArmed, 0, "bool", "")
{
   bool armed{ false };
   if (UCI_Util::GetInterface(SIMULATION))
   {
      armed = aObjectPtr->Get().getWeaponArmed();
   }
   aReturnVal.SetBool(armed);
}

UT_DEFINE_SCRIPT_METHOD(UCI_StrikeActivityTypeClass, UCI_StrikeActivityType, HasFailed, 0, "bool", "")
{
   bool status{ false };
   if (UCI_Util::GetInterface(SIMULATION))
   {
      status = aObjectPtr->Get().getActivityState() == uci::type::ActivityStateEnum::UCI_FAILED;
   }
   aReturnVal.SetBool(status);
}

UT_DEFINE_SCRIPT_METHOD(UCI_StrikeActivityTypeClass, UCI_StrikeActivityType, IsActiveFullyConstrained, 0, "bool", "")
{
   bool status{ false };
   if (UCI_Util::GetInterface(SIMULATION))
   {
      status = aObjectPtr->Get().getActivityState() == uci::type::ActivityStateEnum::UCI_ACTIVE_FULLY_CONSTRAINED;
   }
   aReturnVal.SetBool(status);
}

UT_DEFINE_SCRIPT_METHOD(UCI_StrikeActivityTypeClass, UCI_StrikeActivityType, IsActivePartiallyConstrained, 0, "bool", "")
{
   bool status{ false };
   if (UCI_Util::GetInterface(SIMULATION))
   {
      status = aObjectPtr->Get().getActivityState() == uci::type::ActivityStateEnum::UCI_ACTIVE_PARTIALLY_CONSTRAINED;
   }
   aReturnVal.SetBool(status);
}


UT_DEFINE_SCRIPT_METHOD(UCI_StrikeActivityTypeClass, UCI_StrikeActivityType, IsActiveUnconstrained, 0, "bool", "")
{
   bool status{ false };
   if (UCI_Util::GetInterface(SIMULATION))
   {
      status = aObjectPtr->Get().getActivityState() == uci::type::ActivityStateEnum::UCI_ACTIVE_UNCONSTRAINED;
   }
   aReturnVal.SetBool(status);
}


UT_DEFINE_SCRIPT_METHOD(UCI_StrikeActivityTypeClass, UCI_StrikeActivityType, IsCompleted, 0, "bool", "")
{
   bool status{ false };
   if (UCI_Util::GetInterface(SIMULATION))
   {
      status = aObjectPtr->Get().getActivityState() == uci::type::ActivityStateEnum::UCI_COMPLETED;
   }
   aReturnVal.SetBool(status);
}

UT_DEFINE_SCRIPT_METHOD(UCI_StrikeActivityTypeClass, UCI_StrikeActivityType, IsDeleted, 0, "bool", "")
{
   bool status{ false };
   if (UCI_Util::GetInterface(SIMULATION))
   {
      status = aObjectPtr->Get().getActivityState() == uci::type::ActivityStateEnum::UCI_DELETED;
   }
   aReturnVal.SetBool(status);
}

UT_DEFINE_SCRIPT_METHOD(UCI_StrikeActivityTypeClass, UCI_StrikeActivityType, IsDisabled, 0, "bool", "")
{
   bool status{ false };
   if (UCI_Util::GetInterface(SIMULATION))
   {
      status = aObjectPtr->Get().getActivityState() == uci::type::ActivityStateEnum::UCI_DISABLED;
   }
   aReturnVal.SetBool(status);
}

UT_DEFINE_SCRIPT_METHOD(UCI_StrikeActivityTypeClass, UCI_StrikeActivityType, IsEnabled, 0, "bool", "")
{
   bool status{ false };
   if (UCI_Util::GetInterface(SIMULATION))
   {
      status = aObjectPtr->Get().getActivityState() == uci::type::ActivityStateEnum::UCI_ENABLED;
   }
   aReturnVal.SetBool(status);
}

} // script
} // wsf
