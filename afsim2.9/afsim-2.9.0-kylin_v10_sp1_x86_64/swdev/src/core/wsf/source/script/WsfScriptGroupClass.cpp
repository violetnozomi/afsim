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

#include "WsfScriptGroupClass.hpp"

#include "UtMemory.hpp"
#include "WsfGroup.hpp"
#include "WsfGroupManager.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformPart.hpp"
#include "WsfScriptContext.hpp"
#include "WsfScriptManager.hpp"
#include "WsfSimulation.hpp"

// =================================================================================================
WsfScriptGroupClass::WsfScriptGroupClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : WsfScriptObjectClass(aClassName, aTypesPtr)
{
   SetClassName("WsfGroup");

   // WsfObject = group[int];
   SetContainerKeyTypeId("int");
   SetContainerDataTypeId("WsfObject");

   // Add each of the method objects to the class.

   AddMethod(ut::make_unique<AddMember_1>("AddMember"));       // AddMember(WsfPlatform)
   AddMethod(ut::make_unique<AddMember_2>("AddMember"));       // AddMember(WsfPlatformPart)
   AddMethod(ut::make_unique<RemoveMember_1>("RemoveMember")); // RemoveMember(WsfPlatform)
   AddMethod(ut::make_unique<RemoveMember_2>("RemoveMember")); // RemoveMember(WsfPlatformPart)
   AddMethod(ut::make_unique<MembersByName>());                // returns Array<string>
   AddMethod(ut::make_unique<Members>());                      // returns Array<WsfObject>
   AddMethod(ut::make_unique<Member>());                       // returns WsfObject
   AddMethod(ut::make_unique<Member>("Get"));                  // NO_DOC | DEPRECATED
   AddMethod(ut::make_unique<Size>());                         // returns int;

   WsfScriptAuxDataUtil::AddAuxDataScriptMethods(*this);

   AddStaticMethod(ut::make_unique<CreateGroup>()); // WsfGroup group = CreateGroup(string,string)
   AddStaticMethod(ut::make_unique<Group>());       // Group(string)
   AddStaticMethod(ut::make_unique<Groups>());      // Groups() - returns Array<string>
}

#include "script/WsfScriptDefs.hpp"

// =================================================================================================
//! virtual
void* WsfScriptGroupClass::Create(const UtScriptContext& aInstance)
{
   return new WsfGroup();
}

// =================================================================================================
//! virtual
void* WsfScriptGroupClass::Clone(void* aObjectPtr)
{
   return static_cast<WsfGroup*>(aObjectPtr)->Clone();
}

// =================================================================================================
//! virtual
void WsfScriptGroupClass::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfGroup*>(aObjectPtr);
}

// =================================================================================================
//! virtual
void WsfScriptGroupClass::OnNewScriptRef(UtScriptRef& aReference)
{
   if (aReference.GetMemManagement() != UtScriptRef::cMANAGE)
   {
      UtReferenceTracked* refPtr = static_cast<WsfGroup*>(aReference.GetAppObject());
      if (refPtr != nullptr)
      {
         aReference.SetExternalReference(refPtr->GetReferenceCount());
      }
   }
}

// =================================================================================================
//! group.AddMember(WsfPlatform)
UT_DEFINE_SCRIPT_METHOD(WsfScriptGroupClass, WsfGroup, AddMember_1, 1, "void", "WsfPlatform")
{
   WsfPlatform* platformPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   if (platformPtr != nullptr)
   {
      platformPtr->JoinGroup(aObjectPtr);
   }
}

// =================================================================================================
//! group.AddMember(WsfPlatformPart)
UT_DEFINE_SCRIPT_METHOD(WsfScriptGroupClass, WsfGroup, AddMember_2, 1, "void", "WsfPlatformPart")
{
   WsfPlatformPart* platformPartPtr = (WsfPlatformPart*)aVarArgs[0].GetPointer()->GetAppObject();
   if (platformPartPtr != nullptr)
   {
      platformPartPtr->JoinGroup(aObjectPtr);
   }
}

// =================================================================================================
//! group.RemoveMember(WsfPlatform)
UT_DEFINE_SCRIPT_METHOD(WsfScriptGroupClass, WsfGroup, RemoveMember_1, 1, "void", "WsfPlatform")
{
   WsfPlatform* platformPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   aObjectPtr->LeaveGroup(platformPtr->GetIndex());
}

// =================================================================================================
//! group.RemoveMember(WsfPlatformPart)
UT_DEFINE_SCRIPT_METHOD(WsfScriptGroupClass, WsfGroup, RemoveMember_2, 1, "void", "WsfPlatformPart")
{
   WsfPlatformPart* platformPartPtr = (WsfPlatformPart*)aVarArgs[0].GetPointer()->GetAppObject();
   aObjectPtr->LeaveGroup(platformPartPtr->GetPlatform()->GetIndex(), platformPartPtr->GetUniqueId());
}

// =================================================================================================
//! Array<string> = group.Members();
UT_DEFINE_SCRIPT_METHOD(WsfScriptGroupClass, WsfGroup, MembersByName, 0, "Array<string>", "")
{
   std::vector<UtScriptData>* resultVecPtr = new std::vector<UtScriptData>();

   WsfGroup::MemberList tempMembersList = aObjectPtr->GetMembers();
   for (const auto& memberPair : tempMembersList)
   {
      WsfPlatform* platformPtr = SIMULATION->GetPlatformByIndex(memberPair.first);
      if (platformPtr != nullptr)
      {
         if (memberPair.second > 0)
         {
            WsfPlatformPart* platformPartPtr = (WsfPlatformPart*)platformPtr->GetArticulatedPart(memberPair.second);
            if (platformPartPtr != nullptr)
            {
               resultVecPtr->emplace_back(platformPartPtr->GetName());
            }
         }
         else
         {
            resultVecPtr->emplace_back(platformPtr->GetName());
         }
      }
   }

   aReturnVal.SetPointer(new UtScriptRef(resultVecPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// =================================================================================================
// Array<Object> = group.Members();
UT_DEFINE_SCRIPT_METHOD(WsfScriptGroupClass, WsfGroup, Members, 0, "Array<WsfObject>", "")
{
   std::vector<UtScriptData>* resultVecPtr = new std::vector<UtScriptData>();

   UtScriptClass* platformClassPtr     = aContext.GetTypes()->GetClass("WsfPlatform");
   UtScriptClass* platformPartClassPtr = aContext.GetTypes()->GetClass("WsfPlatformPart");

   WsfGroup::MemberList tempMembersList = aObjectPtr->GetMembers();
   for (const auto& memberPair : tempMembersList)
   {
      WsfPlatform* platformPtr = SIMULATION->GetPlatformByIndex(memberPair.first);
      if (platformPtr != nullptr)
      {
         if (memberPair.second > 0)
         {
            WsfPlatformPart* platformPartPtr = (WsfPlatformPart*)platformPtr->GetArticulatedPart(memberPair.second);
            if (platformPartPtr != nullptr)
            {
               resultVecPtr->push_back(UtScriptData(new UtScriptRef(platformPartPtr, platformPartClassPtr)));
            }
         }
         else
         {
            resultVecPtr->push_back(UtScriptData(new UtScriptRef(platformPtr, platformClassPtr)));
         }
      }
   }

   aReturnVal.SetPointer(new UtScriptRef(resultVecPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// =================================================================================================
// WsfObject = group.Member(int);
UT_DEFINE_SCRIPT_METHOD(WsfScriptGroupClass, WsfGroup, Member, 1, "WsfObject", "int")
{
   WsfGroup::MemberList& members = aObjectPtr->GetMembers();
   int                   index   = aVarArgs[0].GetInt();
   if (index >= 0 && index < (int)members.size())
   {
      UtScriptClass*      platformClassPtr     = aContext.GetTypes()->GetClass("WsfPlatform");
      UtScriptClass*      platformPartClassPtr = aContext.GetTypes()->GetClass("WsfPlatformPart");
      WsfGroup::GroupPair memberPair           = members[index];
      WsfPlatform*        platformPtr          = SIMULATION->GetPlatformByIndex(memberPair.first);
      if (platformPtr != nullptr)
      {
         if (memberPair.second > 0)
         {
            WsfPlatformPart* platformPartPtr = (WsfPlatformPart*)platformPtr->GetArticulatedPart(memberPair.second);
            if (platformPartPtr != nullptr)
            {
               aReturnVal.SetPointer(new UtScriptRef(platformPartPtr, platformPartClassPtr));
            }
         }
         else
         {
            aReturnVal.SetPointer(new UtScriptRef(platformPtr, platformClassPtr));
         }
      }
   }
   else
   {
      UT_SCRIPT_ABORT("Invalid index for WsfGroup.Member()\n");
   }
}

// =================================================================================================
// int groupSize = group.Size();
UT_DEFINE_SCRIPT_METHOD(WsfScriptGroupClass, WsfGroup, Size, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(aObjectPtr->GetMembers().size()));
}

// =================================================================================================
// creates a group of the given type with the given name
UT_DEFINE_SCRIPT_METHOD(WsfScriptGroupClass, WsfGroup, CreateGroup, 2, "WsfGroup", "string,string")
{
   WsfGroup* groupPtr = SIMULATION->GetGroupManager().CreateGroup(aVarArgs[0].GetString(), aVarArgs[1].GetString());
   aReturnVal.SetPointer(new UtScriptRef(groupPtr, aReturnClassPtr));
}

// =================================================================================================
// returns a pointer to the group matching the given name
UT_DEFINE_SCRIPT_METHOD(WsfScriptGroupClass, WsfGroup, Group, 1, "WsfGroup", "string")
{
   WsfGroup* groupPtr = SIMULATION->GetGroupManager().GetGroup(aVarArgs[0].GetString());
   aReturnVal.SetPointer(new UtScriptRef(groupPtr, aReturnClassPtr));
}

// =================================================================================================
// returns a list of names of all groups in the simulation
UT_DEFINE_SCRIPT_METHOD(WsfScriptGroupClass, WsfGroup, Groups, 0, "Array<string>", "")
{
   std::vector<UtScriptData>*       resultVecPtr = new std::vector<UtScriptData>();
   const WsfGroupManager::GroupMap& groupsList   = SIMULATION->GetGroupManager().GetGroups();
   for (const auto& group : groupsList)
   {
      resultVecPtr->emplace_back(group.first);
   }

   aReturnVal.SetPointer(new UtScriptRef(resultVecPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_AUX_DATA_SCRIPT_METHODS(WsfScriptGroupClass, WsfGroup)
