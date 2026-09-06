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

#include "script/WsfScriptPlatformPartClass.hpp"

#include "UtMemory.hpp"
#include "WsfGroup.hpp"
#include "WsfGroupManager.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformPart.hpp"
#include "WsfScriptContext.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"

WsfScriptPlatformPartClass::WsfScriptPlatformPartClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptObjectClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfPlatformPart");

   AddMethod(ut::make_unique<Platform>());
   AddMethod(ut::make_unique<IsNonOperational>());
   AddMethod(ut::make_unique<IsOperational>());
   AddMethod(ut::make_unique<SetOperational>());
   AddMethod(ut::make_unique<IsTurnedOff>());
   AddMethod(ut::make_unique<IsTurnedOn>());
   AddMethod(ut::make_unique<InitiallyTurnedOn>());

   AddMethod(ut::make_unique<CanBeMadeOperational>());    // NO_DOC | PENDING
   AddMethod(ut::make_unique<CanBeMadeNonOperational>()); // NO_DOC | PENDING
   AddMethod(ut::make_unique<CanBeTurnedOn>());           // NO_DOC | PENDING
   AddMethod(ut::make_unique<CanBeTurnedOff>());          // NO_DOC | PENDING

   AddMethod(ut::make_unique<TimeSinceLastStatusChange>());
   AddMethod(ut::make_unique<SendMessage_1>("SendMessage")); // SendMessage(msg)
   AddMethod(ut::make_unique<SendMessage_2>("SendMessage")); // SendMessage(msg, partName)
   AddMethod(ut::make_unique<SendMessageToProcessor>());     // NO_DOC | DEPRECATED
   AddMethod(ut::make_unique<SetDebugEnabled>());
   AddMethod(ut::make_unique<DebugEnabled>());
   AddMethod(ut::make_unique<SetDamageFactor>());
   AddMethod(ut::make_unique<DamageFactor>());

   WsfScriptAuxDataUtil::AddAuxDataScriptMethods(*this);

   AddMethod(ut::make_unique<CategoryMemberOf>());

   AddMethod(ut::make_unique<GroupMemberOf_2>("GroupMemberOf")); // GroupMemberOf(string)
   AddMethod(ut::make_unique<GroupMemberOf_3>("GroupMemberOf")); // GroupMemberOf(WsfGroup)
   AddMethod(ut::make_unique<GetGroupByIndex>("Group"));         // NO_DOC | DEPRECATED
   AddMethod(ut::make_unique<GetGroupByName>("Group"));          // NO_DOC | DEPRECATED
   AddMethod(ut::make_unique<GroupsByName>());                   // returns Array<int>
   AddMethod(ut::make_unique<Groups>());                         // returns Array<WsfGroup>
   AddMethod(ut::make_unique<GroupJoin>());                      // GroupJoin(string)
   AddMethod(ut::make_unique<GroupLeave>());                     // GroupLeave(string)
}


// virtual
void WsfScriptPlatformPartClass::OnNewScriptRef(UtScriptRef& aReference)
{
   // If this platform part is on a valid platform, set the reference count to the platform's.
   // We assume that the lifetime of the part is equal to that of the platform.
   if (aReference.GetMemManagement() != UtScriptRef::cMANAGE)
   {
      WsfPlatformPart* platformPartPtr = static_cast<WsfPlatformPart*>(aReference.GetAppObject());
      if ((platformPartPtr != nullptr) && (platformPartPtr->GetPlatform() != nullptr))
      {
         aReference.SetExternalReference(platformPartPtr->GetPlatform()->GetReferenceCount());
      }
   }
}

#include "script/WsfScriptDefs.hpp"

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformPartClass, WsfPlatformPart, Platform, 0, "WsfPlatform", "")
{
   WsfPlatform* platformPtr = aObjectPtr->GetPlatform();
   aReturnVal.SetPointer(new UtScriptRef(platformPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD_NOCHECK(WsfScriptPlatformPartClass, WsfPlatformPart, IsTurnedOff, 0, "bool", "")
{
   bool isTurnedOff = true;
   if (aObjectPtr != nullptr)
   {
      isTurnedOff = !aObjectPtr->IsTurnedOn();
   }
   aReturnVal.SetBool(isTurnedOff);
}

UT_DEFINE_SCRIPT_METHOD_NOCHECK(WsfScriptPlatformPartClass, WsfPlatformPart, IsTurnedOn, 0, "bool", "")
{
   bool isTurnedOn = false;
   if (aObjectPtr != nullptr)
   {
      isTurnedOn = aObjectPtr->IsTurnedOn();
   }
   aReturnVal.SetBool(isTurnedOn);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformPartClass, WsfPlatformPart, InitiallyTurnedOn, 0, "bool", "")
{
   bool isInitiallyOn = aObjectPtr->InitiallyTurnedOn();
   aReturnVal.SetBool(isInitiallyOn);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformPartClass, WsfPlatformPart, TimeSinceLastStatusChange, 0, "double", "")
{
   double simTime = TIME_NOW;
   aReturnVal.SetDouble(simTime - aObjectPtr->GetLastStatusChangeTime());
}

UT_DEFINE_SCRIPT_METHOD_NOCHECK(WsfScriptPlatformPartClass, WsfPlatformPart, IsNonOperational, 0, "bool", "")
{
   bool isNonOperational = true;
   if (aObjectPtr != nullptr)
   {
      isNonOperational = !aObjectPtr->IsOperational();
   }
   aReturnVal.SetBool(isNonOperational);
}

UT_DEFINE_SCRIPT_METHOD_NOCHECK(WsfScriptPlatformPartClass, WsfPlatformPart, IsOperational, 0, "bool", "")
{
   bool isOperational = false;
   if (aObjectPtr != nullptr)
   {
      isOperational = aObjectPtr->IsOperational();
   }
   aReturnVal.SetBool(isOperational);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformPartClass, WsfPlatformPart, SetOperational, 1, "bool", "bool")
{
   double simTime       = TIME_NOW;
   bool   isOperational = aVarArgs[0].GetBool();
   bool   tookAction    = aObjectPtr->SetOperational(simTime, isOperational);
   aReturnVal.SetBool(tookAction);
}

UT_DEFINE_SCRIPT_METHOD_NOCHECK(WsfScriptPlatformPartClass, WsfPlatformPart, CanBeMadeOperational, 0, "bool", "") // NO_DOC | PENDING
{
   bool canBeMadeOper = false;
   if (aObjectPtr != nullptr)
   {
      canBeMadeOper = aObjectPtr->CanBeMadeOperational();
   }
   aReturnVal.SetBool(canBeMadeOper);
}

UT_DEFINE_SCRIPT_METHOD_NOCHECK(WsfScriptPlatformPartClass, WsfPlatformPart, CanBeMadeNonOperational, 0, "bool", "") // NO_DOC | PENDING
{
   bool canBeMadeNonOper = false;
   if (aObjectPtr != nullptr)
   {
      canBeMadeNonOper = aObjectPtr->CanBeMadeNonOperational();
   }
   aReturnVal.SetBool(canBeMadeNonOper);
}

UT_DEFINE_SCRIPT_METHOD_NOCHECK(WsfScriptPlatformPartClass, WsfPlatformPart, CanBeTurnedOn, 0, "bool", "") // NO_DOC | PENDING
{
   bool canBeTurnedOn = false;
   if (aObjectPtr != nullptr)
   {
      canBeTurnedOn = aObjectPtr->CanBeTurnedOn();
   }
   aReturnVal.SetBool(canBeTurnedOn);
}

UT_DEFINE_SCRIPT_METHOD_NOCHECK(WsfScriptPlatformPartClass, WsfPlatformPart, CanBeTurnedOff, 0, "bool", "") // NO_DOC | PENDING
{
   bool canBeTurnedOff = false;
   if (aObjectPtr != nullptr)
   {
      canBeTurnedOff = aObjectPtr->CanBeTurnedOff();
   }
   aReturnVal.SetBool(canBeTurnedOff);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformPartClass, WsfPlatformPart, SendMessage_1, 1, "void", "WsfMessage")
{
   // Argument 0 - the message to be sent
   WsfMessage* messagePtr = (WsfMessage*)aVarArgs[0].GetPointer()->GetAppObject();
   double      simTime    = TIME_NOW;
#undef SendMessage
   aObjectPtr->SendMessage(simTime, *messagePtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformPartClass, WsfPlatformPart, SendMessage_2, 2, "void", "WsfMessage, string")
{
   // Argument 0 - the message to be sent
   // Argument 1 - the name of the intended platform part.
   WsfMessage* messagePtr = (WsfMessage*)aVarArgs[0].GetPointer()->GetAppObject();
   WsfStringId recipientId(aVarArgs[1].GetString());
   double      simTime = TIME_NOW;
   aObjectPtr->SendMessageTo(simTime, *messagePtr, recipientId);
}

// This method is DEPRECATED, use the two argument for of SendMessage.
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformPartClass, WsfPlatformPart, SendMessageToProcessor, 2, "void", "WsfMessage, string") // NO_DOC | DEPRECATED
{
   // Argument 0 - the message to be sent
   // Argument 1 - the processor to whom it is to be sent
   WsfMessage* messagePtr = (WsfMessage*)aVarArgs[0].GetPointer()->GetAppObject();
   WsfStringId processorId(aVarArgs[1].GetString());
   if (messagePtr != nullptr)
   {
      double simTime = TIME_NOW;
      aObjectPtr->SendMessageToProcessor(simTime, *messagePtr, processorId);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformPartClass, WsfPlatformPart, SetDebugEnabled, 1, "void", "bool")
{
   aObjectPtr->SetDebugEnabled(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformPartClass, WsfPlatformPart, DebugEnabled, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->DebugEnabled());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformPartClass, WsfPlatformPart, SetDamageFactor, 1, "void", "double")
{
   double simTime = TIME_NOW;
   aObjectPtr->SetDamageFactor(simTime, aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformPartClass, WsfPlatformPart, DamageFactor, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetDamageFactor());
}

//! bool isCategoryMemberOf = CategoryMemberOf(string aCategory);
//! Returns true if the platform part is a member of the given category.
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformPartClass, WsfPlatformPart, CategoryMemberOf, 1, "bool", "string")
{
   WsfStringId categoryNameId = aVarArgs[0].GetString();
   aReturnVal.SetBool(aObjectPtr->IsCategoryMember(categoryNameId));
}

// =================================================================================================
//! bool isGroupMemberOf = GroupMemberOf(string aGroupName);
//! Returns true if the platform part is a member of the given group.
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformPartClass, WsfPlatformPart, GroupMemberOf_2, 1, "bool", "string")
{
   WsfStringId groupNameId = aVarArgs[0].GetString();
   aReturnVal.SetBool(aObjectPtr->IsGroupMember(groupNameId));
}

// =================================================================================================
//! bool isGroupMemberOf = GroupMemberOf(WsfGroup aGroup);
//! Returns true if the platform part is a member of the given group.
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformPartClass, WsfPlatformPart, GroupMemberOf_3, 1, "bool", "WsfGroup")
{
   bool      isMember(false);
   WsfGroup* groupPtr = (WsfGroup*)aVarArgs[0].GetPointer()->GetAppObject();
   if (groupPtr != nullptr)
   {
      isMember = aObjectPtr->IsGroupMember(groupPtr->GetNameId());
   }
   aReturnVal.SetBool(isMember);
}

// =================================================================================================
//! WsfGroup GroupByIndex(int aIndex); returns the WsfGroup object that is in the given element,
//!    if the platform part is a member
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformPartClass, WsfPlatformPart, GetGroupByIndex, 1, "WsfGroup", "int") // NO_DOC | DEPRECATED
{
   WsfGroupList::GroupList tempIdList = aObjectPtr->GetGroups().GetGroupList();

   WsfGroup* groupPtr = SIMULATION->GetGroupManager().GetGroup(tempIdList[aVarArgs[0].GetInt()]);
   if ((groupPtr != nullptr) && groupPtr->IsGroupMember(aObjectPtr->GetPlatform()->GetIndex(), aObjectPtr->GetUniqueId()))
   {
      aReturnVal.SetPointer(new UtScriptRef(groupPtr, aReturnClassPtr, UtScriptRef::cDONT_MANAGE));
      return;
   }

   aReturnVal.SetPointer(new UtScriptRef(nullptr, aReturnClassPtr, UtScriptRef::cDONT_MANAGE));
}

// =================================================================================================
//! WsfGroup GroupByName(string aGroupName);
//!   returns the WsfGroup object that matches the given name, if the platform part is a member.
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformPartClass, WsfPlatformPart, GetGroupByName, 1, "WsfGroup", "string") // NO_DOC | DEPRECATED
{
   WsfGroup* groupPtr = SIMULATION->GetGroupManager().GetGroup(aVarArgs[0].GetString());
   if ((groupPtr != nullptr) && groupPtr->IsGroupMember(aObjectPtr->GetPlatform()->GetIndex(), aObjectPtr->GetUniqueId()))
   {
      aReturnVal.SetPointer(new UtScriptRef(groupPtr, aReturnClassPtr, UtScriptRef::cDONT_MANAGE));
      return;
   }

   aReturnVal.SetPointer(new UtScriptRef(nullptr, aReturnClassPtr, UtScriptRef::cDONT_MANAGE));
}

// =================================================================================================
//! GroupsByName(); returns list of group names that this platform part is a member of
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformPartClass, WsfPlatformPart, GroupsByName, 0, "Array<string>", "")
{
   std::vector<UtScriptData>* resultVecPtr = new std::vector<UtScriptData>();

   WsfGroupList::GroupList tempIdList = aObjectPtr->GetGroups().GetGroupList();
   for (auto id : tempIdList)
   {
      resultVecPtr->emplace_back(id);
   }

   aReturnVal.SetPointer(new UtScriptRef(resultVecPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// =================================================================================================
//! Groups(); returns list of group objects that this platform part is a member of
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformPartClass, WsfPlatformPart, Groups, 0, "Array<WsfGroup>", "")
{
   std::vector<UtScriptData>* resultVecPtr  = new std::vector<UtScriptData>();
   UtScriptClass*             groupClassPtr = aContext.GetTypes()->GetClass("WsfGroup");
   WsfGroupList::GroupList    tempIdList    = aObjectPtr->GetGroups().GetGroupList();
   for (auto id : tempIdList)
   {
      WsfGroup* groupPtr = SIMULATION->GetGroupManager().GetGroup(id);
      if (groupPtr != nullptr)
      {
         resultVecPtr->emplace_back(new UtScriptRef(groupPtr, groupClassPtr, groupPtr->GetReferenceCount()));
      }
   }

   aReturnVal.SetPointer(new UtScriptRef(resultVecPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// =================================================================================================
//! GroupJoin(int); add the platform part to the given group
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformPartClass, WsfPlatformPart, GroupJoin, 1, "void", "string")
{
   WsfGroup* groupPtr = SIMULATION->GetGroupManager().GetGroup(aVarArgs[0].GetString());
   if (groupPtr != nullptr)
   {
      aObjectPtr->JoinGroup(groupPtr);
   }
}

// =================================================================================================
//! GroupLeave(int); remove the platform part from the given group
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformPartClass, WsfPlatformPart, GroupLeave, 1, "void", "string")
{
   WsfGroup* groupPtr = SIMULATION->GetGroupManager().GetGroup(aVarArgs[0].GetString());
   if (groupPtr != nullptr)
   {
      aObjectPtr->LeaveGroup(groupPtr);
   }
}

UT_DEFINE_AUX_DATA_SCRIPT_METHODS(WsfScriptPlatformPartClass, WsfPlatformPart)
