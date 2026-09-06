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

#include "WsfScriptUCI_StrikeCommandTypes.hpp"

#include "script/WsfScriptDefs.hpp"

#include "UtMemory.hpp"
#include "UtScriptRef.hpp"
#include "UtScriptTypes.hpp"
#include "WsfUCI_Interface.hpp"
#include "WsfUCI_Messages.hpp"
#include "WsfUCI_Types.hpp"
#include "WsfUCI_Utils.hpp"

namespace wsf
{
namespace script
{

UCI_StrikeCommandTypeClass::UCI_StrikeCommandTypeClass(const std::string& aClassName,
                                                     UtScriptTypes*     aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName(aClassName);

   AddStaticMethod(ut::make_unique<Construct_1>("Construct"));
   AddStaticMethod(ut::make_unique<Construct_2>("Construct"));
}

UT_DEFINE_SCRIPT_METHOD(UCI_StrikeCommandTypeClass, UCI_StrikeCommandTypeClass, Construct_1, 1, "UCI_StrikeCommand",
                        "UCI_StrikeCapabilityCommand")
{
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   if (interfacePtr != nullptr)
   {
      auto capabilityCommandPtr = aVarArgs[0].GetPointer()->GetAppObject<UCI_StrikeCapabilityCommandType>();
      auto commandPtr = new UCI_StrikeCommandType(SIMULATION);
      commandPtr->Get().setStrikeCommandTypeChoiceOrdinal(uci::type::StrikeCommandType::STRIKECOMMANDTYPE_CHOICE_CAPABILITY);
      commandPtr->Get().setCapability(capabilityCommandPtr->Get());
      aReturnVal.SetPointer(new UtScriptRef(commandPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_StrikeCommandTypeClass, UCI_StrikeCommandTypeClass, Construct_2, 1, "UCI_StrikeCommand",
                        "UCI_StrikeActivityCommand")
{
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   if (interfacePtr != nullptr)
   {
      auto capabilityCommandPtr = aVarArgs[0].GetPointer()->GetAppObject<UCI_StrikeActivityCommandType>();
      auto commandPtr = new UCI_StrikeCommandType(SIMULATION);
      commandPtr->Get().setStrikeCommandTypeChoiceOrdinal(uci::type::StrikeCommandType::STRIKECOMMANDTYPE_CHOICE_ACTIVITY);
      commandPtr->Get().setActivity(capabilityCommandPtr->Get());
      aReturnVal.SetPointer(new UtScriptRef(commandPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

//================================================================================================
UCI_StrikeCapabilityCommandTypeClass::UCI_StrikeCapabilityCommandTypeClass(const std::string& aClassName,
                                                                           UtScriptTypes*     aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName(aClassName);

   AddStaticMethod(ut::make_unique<Construct>());
   AddMethod(ut::make_unique<SetWeaponCommandType>());
}

UT_DEFINE_SCRIPT_METHOD(UCI_StrikeCapabilityCommandTypeClass, UCI_StrikeCapabilityCommandTypeClass, Construct, 2,
                        "UCI_StrikeCapabilityCommand", "UCI_CapabilityId, UCI_StrikeWeaponCommand")
{
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   if (interfacePtr != nullptr)
   {
      auto capabilityID_Ptr = aVarArgs[0].GetPointer()->GetAppObject<UCI_CapabilityID_Type>();
      auto commandTypePtr = aVarArgs[1].GetPointer()->GetAppObject<UCI_StrikeWeaponCommandType>();

      auto commandPtr = new UCI_StrikeCapabilityCommandType(WsfScriptContext::GetPLATFORM(aContext));
      commandPtr->Get().setCapabilityID(capabilityID_Ptr->Get());
      commandPtr->Get().setCommandState(uci::type::MessageStateEnum::UCI_NEW);
      commandPtr->Get().getCommandID().setUUID(interfacePtr->GetConnection()->generateUUID());

      commandPtr->Get().getRanking().getRank().setPrecedence(0);
      commandPtr->Get().getRanking().getRank().setPriority(0);
      commandPtr->Get().setCommandType(commandTypePtr->Get());

      aReturnVal.SetPointer(new UtScriptRef(commandPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_StrikeCapabilityCommandTypeClass, UCI_StrikeCapabilityCommandType, SetWeaponCommandType, 1,
                        "void", "UCI_StrikeWeaponCommand")
{
   auto typePtr = aVarArgs[0].GetPointer()->GetAppObject<UCI_StrikeWeaponCommandType>();
   aObjectPtr->Get().setCommandType(typePtr->Get());
}

//================================================================================================
UCI_StrikeActivityCommandTypeClass::UCI_StrikeActivityCommandTypeClass(const std::string& aClassName,
                                                                       UtScriptTypes*     aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName(aClassName);

   AddStaticMethod(ut::make_unique<Construct>());
}

UT_DEFINE_SCRIPT_METHOD(UCI_StrikeActivityCommandTypeClass, UCI_StrikeActivityCommandTypeClass, Construct, 1, "UCI_StrikeActivityCommand",
                        "UCI_ActivityId")
{
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   if (interfacePtr != nullptr)
   {
      auto activityID_Ptr = aVarArgs[0].GetPointer()->GetAppObject<UCI_ActivityID_Type>();
      auto commandPtr = new UCI_StrikeActivityCommandType(SIMULATION);
      commandPtr->Get().setActivityID(activityID_Ptr->Get());
      commandPtr->Get().setCommandState(uci::type::MessageStateEnum::UCI_NEW);
      auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
      commandPtr->Get().getCommandID().setUUID(interfacePtr->GetConnection()->generateUUID());

      aReturnVal.SetPointer(new UtScriptRef(commandPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

//================================================================================================
UCI_StrikeWeaponCommandTypeClass::UCI_StrikeWeaponCommandTypeClass(const std::string& aClassName,
                                                                   UtScriptTypes*     aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName(aClassName);
   mConstructible = true;

   AddStaticMethod(ut::make_unique<AssignDMPI_ID>());
   AddStaticMethod(ut::make_unique<SelectForJettison>());
   AddStaticMethod(ut::make_unique<SelectForKeyLoad>());
   AddStaticMethod(ut::make_unique<SelectForRelease>());
   AddStaticMethod(ut::make_unique<WeaponArm>());
}

UT_DEFINE_SCRIPT_METHOD(UCI_StrikeWeaponCommandTypeClass, UCI_StrikeWeaponCommandTypeClass, SelectForJettison, 1, "UCI_StrikeWeaponCommand",
                        "bool")
{
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   if (interfacePtr != nullptr)
   {
      auto commandTypePtr = new UCI_StrikeWeaponCommandType(SIMULATION);
      commandTypePtr->Get().setStrikeWeaponCommandTypeChoiceOrdinal(uci::type::StrikeWeaponCommandType::STRIKEWEAPONCOMMANDTYPE_CHOICE_SELECTFORJETTISON);
      commandTypePtr->Get().setSelectForJettison(aVarArgs[0].GetBool());
      aReturnVal.SetPointer(new UtScriptRef(commandTypePtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_StrikeWeaponCommandTypeClass, UCI_StrikeWeaponCommandTypeClass, SelectForKeyLoad, 1, "UCI_StrikeWeaponCommand",
                        "bool")
{
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   if (interfacePtr != nullptr)
   {
      auto commandTypePtr = new UCI_StrikeWeaponCommandType(SIMULATION);
      commandTypePtr->Get().setStrikeWeaponCommandTypeChoiceOrdinal(uci::type::StrikeWeaponCommandType::STRIKEWEAPONCOMMANDTYPE_CHOICE_SELECTFORKEYLOAD);
      commandTypePtr->Get().setSelectForKeyLoad(aVarArgs[0].GetBool());
      aReturnVal.SetPointer(new UtScriptRef(commandTypePtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_StrikeWeaponCommandTypeClass, UCI_StrikeWeaponCommandTypeClass, SelectForRelease, 1, "UCI_StrikeWeaponCommand",
                        "bool")
{
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   if (interfacePtr != nullptr)
   {
      auto commandTypePtr = new UCI_StrikeWeaponCommandType(SIMULATION);
      commandTypePtr->Get().setStrikeWeaponCommandTypeChoiceOrdinal(uci::type::StrikeWeaponCommandType::STRIKEWEAPONCOMMANDTYPE_CHOICE_SELECTFORKEYLOAD);
      commandTypePtr->Get().setSelectForRelease(aVarArgs[0].GetBool());
      aReturnVal.SetPointer(new UtScriptRef(commandTypePtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_StrikeWeaponCommandTypeClass, UCI_StrikeWeaponCommandTypeClass, WeaponArm, 1, "UCI_StrikeWeaponCommand",
   "bool")
{
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   if (interfacePtr != nullptr)
   {
      auto commandTypePtr = new UCI_StrikeWeaponCommandType(SIMULATION);
      commandTypePtr->Get().setStrikeWeaponCommandTypeChoiceOrdinal(uci::type::StrikeWeaponCommandType::STRIKEWEAPONCOMMANDTYPE_CHOICE_WEAPONARM);
      commandTypePtr->Get().setWeaponArm(aVarArgs[0].GetBool());
      aReturnVal.SetPointer(new UtScriptRef(commandTypePtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_StrikeWeaponCommandTypeClass, UCI_StrikeWeaponCommandTypeClass, AssignDMPI_ID, 1, "UCI_StrikeWeaponCommand",
   "UCI_DMPI_ID")
{
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   if (interfacePtr != nullptr)
   {
      auto commandTypePtr = new UCI_StrikeWeaponCommandType(SIMULATION);
      commandTypePtr->Get().setStrikeWeaponCommandTypeChoiceOrdinal(uci::type::StrikeWeaponCommandType::STRIKEWEAPONCOMMANDTYPE_CHOICE_ASSIGNDMPI_ID);
      commandTypePtr->Get().setAssignDMPI_ID(aVarArgs[0].GetPointer()->GetAppObject<UCI_DMPI_ID_Type>()->Get());
      aReturnVal.SetPointer(new UtScriptRef(commandTypePtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

} // script
} // wsf
