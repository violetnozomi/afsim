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

#include "WsfScriptSixDOF_FormationRemoveSubCommand.hpp"

#include "UtMemory.hpp"
#include "WsfSixDOF_FormationRemoveSubCommand.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
FormationRemoveSubCommand::FormationRemoveSubCommand(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : FormationCommand(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfSixDOF_FormationRemoveSubCommand");

   AddStaticMethod(ut::make_unique<Construct>());

   AddMethod(ut::make_unique<GetFormationToRemove>());
}

UT_DEFINE_SCRIPT_METHOD(FormationRemoveSubCommand,
                        wsf::six_dof::FormationRemoveSubCommand,
                        Construct,
                        1,
                        "WsfSixDOF_FormationRemoveSubCommand",
                        "string")
{
   wsf::six_dof::FormationRemoveSubCommand* commPtr = new wsf::six_dof::FormationRemoveSubCommand{};
   commPtr->SetSubformationToRemove(aVarArgs[0].GetString());
   aReturnVal.SetPointer(new UtScriptRef(commPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(FormationRemoveSubCommand, wsf::six_dof::FormationRemoveSubCommand, GetFormationToRemove, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetSubformationToRemove());
}
} // namespace script
} // namespace six_dof
} // namespace wsf
