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

#include "WsfScriptSixDOF_FormationChangeLeadSubCommand.hpp"

#include "UtMemory.hpp"
#include "WsfSixDOF_FormationChangeLeadSubCommand.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
FormationChangeLeadSubCommand::FormationChangeLeadSubCommand(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : FormationCommand(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfSixDOF_FormationChangeLeadSubCommand");

   AddStaticMethod(ut::make_unique<Construct>());

   AddMethod(ut::make_unique<GetNewLeaderName>());
}

UT_DEFINE_SCRIPT_METHOD(FormationChangeLeadSubCommand,
                        wsf::six_dof::FormationChangeLeadSubCommand,
                        Construct,
                        1,
                        "WsfSixDOF_FormationChangeLeadSubCommand",
                        "string")
{
   wsf::six_dof::FormationChangeLeadSubCommand* commPtr = new wsf::six_dof::FormationChangeLeadSubCommand();
   commPtr->SetNewLeaderName(aVarArgs[0].GetString());
   aReturnVal.SetPointer(new UtScriptRef(commPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(FormationChangeLeadSubCommand,
                        wsf::six_dof::FormationChangeLeadSubCommand,
                        GetNewLeaderName,
                        0,
                        "string",
                        "")
{
   aReturnVal.SetString(aObjectPtr->GetNewLeaderName());
}
} // namespace script
} // namespace six_dof
} // namespace wsf
