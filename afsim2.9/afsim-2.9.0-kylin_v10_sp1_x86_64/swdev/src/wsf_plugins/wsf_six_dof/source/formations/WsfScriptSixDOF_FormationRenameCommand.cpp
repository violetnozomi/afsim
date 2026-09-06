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

#include "WsfScriptSixDOF_FormationRenameCommand.hpp"

#include "UtMemory.hpp"
#include "WsfSixDOF_FormationRenameCommand.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
FormationRenameCommand::FormationRenameCommand(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : FormationCommand(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfSixDOF_FormationRenameCommand");

   AddStaticMethod(ut::make_unique<Construct>());

   AddMethod(ut::make_unique<GetNewName>());
}

UT_DEFINE_SCRIPT_METHOD(FormationRenameCommand,
                        wsf::six_dof::FormationRenameCommand,
                        Construct,
                        1,
                        "WsfSixDOF_FormationRenameCommand",
                        "string")
{
   wsf::six_dof::FormationRenameCommand* commPtr = new wsf::six_dof::FormationRenameCommand{};
   commPtr->SetNewName(aVarArgs[0].GetString());
   aReturnVal.SetPointer(new UtScriptRef(commPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(FormationRenameCommand, wsf::six_dof::FormationRenameCommand, GetNewName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetNewName());
}
} // namespace script
} // namespace six_dof
} // namespace wsf
