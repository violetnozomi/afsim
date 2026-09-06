// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfScriptFormation.hpp"

#include <string>
#include <vector>

#include "UtMemory.hpp"
#include "UtScriptContext.hpp"
#include "UtScriptRef.hpp"
#include "WsfFormation.hpp"
#include "WsfPlatform.hpp"
#include "script/WsfScriptDefs.hpp"

WsfScriptFormation::WsfScriptFormation(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   mConstructible = false;
   SetClassName("WsfFormation");

   AddMethod(ut::make_unique<GetQualifiedName>());
   AddMethod(ut::make_unique<GetRelativeName>());
   AddMethod(ut::make_unique<GetParentFormation>());
   AddMethod(ut::make_unique<IsRoot>());
   AddMethod(ut::make_unique<IsLeaf>());
   AddMethod(ut::make_unique<GetNumSubFormations>());
   AddMethod(ut::make_unique<GetSubFormation>());
   AddMethod(ut::make_unique<GetAllSubFormations>());
   AddMethod(ut::make_unique<IsAttached>());
   AddMethod(ut::make_unique<GetLeadSubFormation>());
   AddMethod(ut::make_unique<GetFormationLeader>());
   AddMethod(ut::make_unique<HasLeader>());
   AddMethod(ut::make_unique<IsLeader>());
   AddMethod(ut::make_unique<GetNumMemberPlatforms>());
   AddMethod(ut::make_unique<GetMemberPlatform>());
   AddMethod(ut::make_unique<HasMemberPlatform>());
   AddMethod(ut::make_unique<GetOffset>());
   AddMethod(ut::make_unique<AddSubFormation>());
   AddMethod(ut::make_unique<RemoveSubFormation>());
   AddMethod(ut::make_unique<SetLeader>());
   AddMethod(ut::make_unique<SetMember_1>("SetMember"));
   AddMethod(ut::make_unique<SetMember_2>("SetMember"));
   AddMethod(ut::make_unique<Rename>());
   AddMethod(ut::make_unique<ExecuteCommand_1>("ExecuteCommand"));
   AddMethod(ut::make_unique<ExecuteCommand_2>("ExecuteCommand"));
   AddMethod(ut::make_unique<GetCurrentCommand>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormation, WsfFormation, GetQualifiedName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetQualifiedName());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormation, WsfFormation, GetRelativeName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetRelativeName());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormation, WsfFormation, GetParentFormation, 0, "WsfFormation", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetParentFormation(), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormation, WsfFormation, IsRoot, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsRoot());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormation, WsfFormation, IsLeaf, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsLeaf());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormation, WsfFormation, GetNumSubFormations, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetNumChildFormations());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormation, WsfFormation, GetSubFormation, 1, "WsfFormation", "string")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetChildFormation(aVarArgs[0].GetString()), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormation, WsfFormation, GetAllSubFormations, 0, "Array<WsfFormation>", "")
{
   std::vector<UtScriptData>* retvalPtr = new std::vector<UtScriptData>();
   auto                       func      = [retvalPtr, aObjectClassPtr](WsfFormation* aSubPtr)
   { retvalPtr->push_back(UtScriptData(new UtScriptRef(aSubPtr, aObjectClassPtr))); };
   aObjectPtr->InvokeOnSubFormations(func);
   aReturnVal.SetPointer(new UtScriptRef(retvalPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormation, WsfFormation, IsAttached, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsAttached());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormation, WsfFormation, GetLeadSubFormation, 0, "WsfFormation", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetLeadSubFormation(), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormation, WsfFormation, GetFormationLeader, 0, "WsfFormation", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetFormationLeader(), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormation, WsfFormation, HasLeader, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->HasLeader());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormation, WsfFormation, IsLeader, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsLeader());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormation, WsfFormation, GetNumMemberPlatforms, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetNumMembers());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormation, WsfFormation, GetMemberPlatform, 0, "WsfPlatform", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetMemberPlatform(), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormation, WsfFormation, HasMemberPlatform, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->HasMemberPlatform());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormation, WsfFormation, GetOffset, 0, "WsfFormationOffset", "")
{
   WsfFormationOffset* retvalPtr = new WsfFormationOffset(aObjectPtr->GetOffset());
   aReturnVal.SetPointer(new UtScriptRef(retvalPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormation, WsfFormation, AddSubFormation, 2, "bool", "WsfFormation, WsfFormationOffset")
{
   WsfFormation*       formPtr   = static_cast<WsfFormation*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfFormationOffset* offsetPtr = static_cast<WsfFormationOffset*>(aVarArgs[1].GetPointer()->GetAppObject());
   formPtr->SetOffset(*offsetPtr);
   bool retval = aObjectPtr->AddChildFormation(formPtr);
   aReturnVal.SetBool(retval);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormation, WsfFormation, RemoveSubFormation, 1, "WsfFormation", "string")
{
   WsfFormation* retvalPtr = aObjectPtr->RemoveChildFormation(aVarArgs[0].GetString());
   aReturnVal.SetPointer(new UtScriptRef(retvalPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormation, WsfFormation, SetLeader, 1, "bool", "string")
{
   aReturnVal.SetBool(aObjectPtr->SetLead(aVarArgs[0].GetString()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormation, WsfFormation, SetMember_1, 1, "bool", "string")
{
   aReturnVal.SetBool(aObjectPtr->SetMemberName(aVarArgs[0].GetString()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormation, WsfFormation, SetMember_2, 1, "bool", "WsfPlatform")
{
   auto platPtr = aVarArgs[0].GetPointer()->GetAppObject<WsfPlatform>();
   aReturnVal.SetBool(aObjectPtr->SetMemberName(platPtr->GetName()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormation, WsfFormation, Rename, 1, "bool", "string")
{
   aReturnVal.SetBool(aObjectPtr->SetRelativeName(aVarArgs[0].GetString()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormation, WsfFormation, ExecuteCommand_1, 1, "bool", "WsfFormationCommand")
{
   auto commInputPtr = aVarArgs[0].GetPointer()->GetAppObject<WsfFormationCommand>();
   auto commPtr      = ut::clone(commInputPtr);
   bool retval       = aObjectPtr->ExecuteCommand(std::move(commPtr), TIME_NOW);
   aReturnVal.SetBool(retval);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormation, WsfFormation, ExecuteCommand_2, 2, "bool", "WsfFormationCommand, WsfFormationCommandConstraint")
{
   auto commInputPtr = aVarArgs[0].GetPointer()->GetAppObject<WsfFormationCommand>();
   auto consInputPtr = aVarArgs[1].GetPointer()->GetAppObject<WsfFormationCommand::Constraint>();
   auto commPtr      = ut::clone(commInputPtr);
   commPtr->SetConstraint(ut::clone(consInputPtr));
   bool retval = aObjectPtr->ExecuteCommand(std::move(commPtr), TIME_NOW);
   aReturnVal.SetBool(retval);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormation, WsfFormation, GetCurrentCommand, 0, "WsfFormationCommand", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetCurrentCommand(), aReturnClassPtr));
}
