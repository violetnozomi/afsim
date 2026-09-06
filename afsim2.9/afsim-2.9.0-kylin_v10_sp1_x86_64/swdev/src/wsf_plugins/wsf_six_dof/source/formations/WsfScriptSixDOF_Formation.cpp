// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfScriptSixDOF_Formation.hpp"

#include <string>
#include <vector>

#include "UtMemory.hpp"
#include "UtScriptContext.hpp"
#include "UtScriptRef.hpp"
#include "WsfPlatform.hpp"
#include "WsfSixDOF_Formation.hpp"
#include "script/WsfScriptDefs.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
Formation::Formation(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   mConstructible = false;
   SetClassName("WsfSixDOF_Formation");

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

UT_DEFINE_SCRIPT_METHOD(Formation, wsf::six_dof::Formation, GetQualifiedName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetQualifiedName());
}

UT_DEFINE_SCRIPT_METHOD(Formation, wsf::six_dof::Formation, GetRelativeName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetRelativeName());
}

UT_DEFINE_SCRIPT_METHOD(Formation, wsf::six_dof::Formation, GetParentFormation, 0, "WsfSixDOF_Formation", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetParentFormation(), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(Formation, wsf::six_dof::Formation, IsRoot, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsRoot());
}

UT_DEFINE_SCRIPT_METHOD(Formation, wsf::six_dof::Formation, IsLeaf, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsLeaf());
}

UT_DEFINE_SCRIPT_METHOD(Formation, wsf::six_dof::Formation, GetNumSubFormations, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetNumChildFormations());
}

UT_DEFINE_SCRIPT_METHOD(Formation, wsf::six_dof::Formation, GetSubFormation, 1, "WsfSixDOF_Formation", "string")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetChildFormation(aVarArgs[0].GetString()), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(Formation, wsf::six_dof::Formation, GetAllSubFormations, 0, "Array<WsfSixDOF_Formation>", "")
{
   std::vector<UtScriptData>* retvalPtr = new std::vector<UtScriptData>();
   auto                       func      = [retvalPtr, aObjectClassPtr](wsf::six_dof::Formation* aSubPtr)
   { retvalPtr->push_back(UtScriptData(new UtScriptRef(aSubPtr, aObjectClassPtr))); };
   aObjectPtr->InvokeOnSubFormations(func);
   aReturnVal.SetPointer(new UtScriptRef(retvalPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(Formation, wsf::six_dof::Formation, IsAttached, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsAttached());
}

UT_DEFINE_SCRIPT_METHOD(Formation, wsf::six_dof::Formation, GetLeadSubFormation, 0, "WsfSixDOF_Formation", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetLeadSubFormation(), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(Formation, wsf::six_dof::Formation, GetFormationLeader, 0, "WsfSixDOF_Formation", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetFormationLeader(), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(Formation, wsf::six_dof::Formation, HasLeader, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->HasLeader());
}

UT_DEFINE_SCRIPT_METHOD(Formation, wsf::six_dof::Formation, IsLeader, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsLeader());
}

UT_DEFINE_SCRIPT_METHOD(Formation, wsf::six_dof::Formation, GetNumMemberPlatforms, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetNumMembers());
}

UT_DEFINE_SCRIPT_METHOD(Formation, wsf::six_dof::Formation, GetMemberPlatform, 0, "WsfPlatform", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetMemberPlatform(), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(Formation, wsf::six_dof::Formation, HasMemberPlatform, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->HasMemberPlatform());
}

UT_DEFINE_SCRIPT_METHOD(Formation, wsf::six_dof::Formation, GetOffset, 0, "WsfSixDOF_FormationOffset", "")
{
   wsf::six_dof::FormationOffset* retvalPtr = new wsf::six_dof::FormationOffset(aObjectPtr->GetOffset());
   aReturnVal.SetPointer(new UtScriptRef(retvalPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(Formation, wsf::six_dof::Formation, AddSubFormation, 2, "bool", "WsfSixDOF_Formation, WsfSixDOF_FormationOffset")
{
   wsf::six_dof::Formation* formPtr = static_cast<wsf::six_dof::Formation*>(aVarArgs[0].GetPointer()->GetAppObject());
   wsf::six_dof::FormationOffset* offsetPtr =
      static_cast<wsf::six_dof::FormationOffset*>(aVarArgs[1].GetPointer()->GetAppObject());
   formPtr->SetOffset(*offsetPtr);
   bool retval = aObjectPtr->AddChildFormation(formPtr);
   aReturnVal.SetBool(retval);
}

UT_DEFINE_SCRIPT_METHOD(Formation, wsf::six_dof::Formation, RemoveSubFormation, 1, "WsfSixDOF_Formation", "string")
{
   wsf::six_dof::Formation* retvalPtr = aObjectPtr->RemoveChildFormation(aVarArgs[0].GetString());
   aReturnVal.SetPointer(new UtScriptRef(retvalPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(Formation, wsf::six_dof::Formation, SetLeader, 1, "bool", "string")
{
   aReturnVal.SetBool(aObjectPtr->SetLead(aVarArgs[0].GetString()));
}

UT_DEFINE_SCRIPT_METHOD(Formation, wsf::six_dof::Formation, SetMember_1, 1, "bool", "string")
{
   aReturnVal.SetBool(aObjectPtr->SetMemberName(aVarArgs[0].GetString()));
}

UT_DEFINE_SCRIPT_METHOD(Formation, wsf::six_dof::Formation, SetMember_2, 1, "bool", "WsfPlatform")
{
   auto platPtr = aVarArgs[0].GetPointer()->GetAppObject<WsfPlatform>();
   aReturnVal.SetBool(aObjectPtr->SetMemberName(platPtr->GetName()));
}

UT_DEFINE_SCRIPT_METHOD(Formation, wsf::six_dof::Formation, Rename, 1, "bool", "string")
{
   aReturnVal.SetBool(aObjectPtr->SetRelativeName(aVarArgs[0].GetString()));
}

UT_DEFINE_SCRIPT_METHOD(Formation, wsf::six_dof::Formation, ExecuteCommand_1, 1, "bool", "WsfSixDOF_FormationCommand")
{
   auto commInputPtr = aVarArgs[0].GetPointer()->GetAppObject<wsf::six_dof::FormationCommand>();
   auto commPtr      = ut::clone(commInputPtr);
   bool retval       = aObjectPtr->ExecuteCommand(std::move(commPtr), TIME_NOW);
   aReturnVal.SetBool(retval);
}

UT_DEFINE_SCRIPT_METHOD(Formation,
                        wsf::six_dof::Formation,
                        ExecuteCommand_2,
                        2,
                        "bool",
                        "WsfSixDOF_FormationCommand, WsfSixDOF_FormationCommandConstraint")
{
   auto commInputPtr = aVarArgs[0].GetPointer()->GetAppObject<wsf::six_dof::FormationCommand>();
   auto consInputPtr = aVarArgs[1].GetPointer()->GetAppObject<wsf::six_dof::FormationCommand::Constraint>();
   auto commPtr      = ut::clone(commInputPtr);
   commPtr->SetConstraint(ut::clone(consInputPtr));
   bool retval = aObjectPtr->ExecuteCommand(std::move(commPtr), TIME_NOW);
   aReturnVal.SetBool(retval);
}

UT_DEFINE_SCRIPT_METHOD(Formation, wsf::six_dof::Formation, GetCurrentCommand, 0, "WsfSixDOF_FormationCommand", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetCurrentCommand(), aReturnClassPtr));
}
} // namespace script
} // namespace six_dof
} // namespace wsf
