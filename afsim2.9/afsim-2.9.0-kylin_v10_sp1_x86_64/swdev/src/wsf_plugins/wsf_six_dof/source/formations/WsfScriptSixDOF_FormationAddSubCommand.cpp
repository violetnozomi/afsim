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

#include "WsfScriptSixDOF_FormationAddSubCommand.hpp"

#include "UtMemory.hpp"
#include "WsfSixDOF_FormationAddSubCommand.hpp"
#include "WsfSixDOF_FormationOffset.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
FormationAddSubCommand::FormationAddSubCommand(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : FormationCommand(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfSixDOF_FormationAddSubCommand");

   AddStaticMethod(ut::make_unique<Construct_1>("Construct"));
   AddStaticMethod(ut::make_unique<Construct_2>("Construct"));

   AddMethod(ut::make_unique<GetParentRelativeName>());
   AddMethod(ut::make_unique<GetFormationToAdd>());
   AddMethod(ut::make_unique<GetOffset>());
}

UT_DEFINE_SCRIPT_METHOD(FormationAddSubCommand,
                        wsf::six_dof::FormationAddSubCommand,
                        Construct_1,
                        2,
                        "WsfSixDOF_FormationAddSubCommand",
                        "string, WsfSixDOF_FormationOffset")
{
   wsf::six_dof::FormationAddSubCommand* commPtr = new wsf::six_dof::FormationAddSubCommand{};
   commPtr->SetFormationToAdd(aVarArgs[0].GetString());
   commPtr->SetOffset(*static_cast<wsf::six_dof::FormationOffset*>(aVarArgs[1].GetPointer()->GetAppObject()));
   aReturnVal.SetPointer(new UtScriptRef(commPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(FormationAddSubCommand,
                        wsf::six_dof::FormationAddSubCommand,
                        Construct_2,
                        3,
                        "WsfSixDOF_FormationAddSubCommand",
                        "string, string, WsfSixDOF_FormationOffset")
{
   wsf::six_dof::FormationAddSubCommand* commPtr = new wsf::six_dof::FormationAddSubCommand{};
   commPtr->SetFormationToAdd(aVarArgs[0].GetString());
   commPtr->SetParentRelativeName(aVarArgs[1].GetString());
   commPtr->SetOffset(*static_cast<wsf::six_dof::FormationOffset*>(aVarArgs[2].GetPointer()->GetAppObject()));
   aReturnVal.SetPointer(new UtScriptRef(commPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(FormationAddSubCommand, wsf::six_dof::FormationAddSubCommand, GetParentRelativeName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetParentRelativeName());
}

UT_DEFINE_SCRIPT_METHOD(FormationAddSubCommand, wsf::six_dof::FormationAddSubCommand, GetFormationToAdd, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetFormationToAdd());
}

UT_DEFINE_SCRIPT_METHOD(FormationAddSubCommand, wsf::six_dof::FormationAddSubCommand, GetOffset, 0, "WsfSixDOF_FormationOffset", "")
{
   wsf::six_dof::FormationOffset* offsetPtr = new wsf::six_dof::FormationOffset(aObjectPtr->GetOffset());
   aReturnVal.SetPointer(new UtScriptRef(offsetPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}
} // namespace script
} // namespace six_dof
} // namespace wsf
