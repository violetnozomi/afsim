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

#include "WsfScriptFormationChangeLeadSubCommand.hpp"

#include "UtMemory.hpp"
#include "WsfFormationChangeLeadSubCommand.hpp"

WsfScriptFormationChangeLeadSubCommand::WsfScriptFormationChangeLeadSubCommand(const std::string& aClassName,
                                                                               UtScriptTypes*     aScriptTypesPtr)
   : WsfScriptFormationCommand(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfFormationChangeLeadSubCommand");

   AddStaticMethod(ut::make_unique<Construct>());

   AddMethod(ut::make_unique<GetNewLeaderName>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationChangeLeadSubCommand,
                        WsfFormationChangeLeadSubCommand,
                        Construct,
                        1,
                        "WsfFormationChangeLeadSubCommand",
                        "string")
{
   WsfFormationChangeLeadSubCommand* commPtr = new WsfFormationChangeLeadSubCommand();
   commPtr->SetNewLeaderName(aVarArgs[0].GetString());
   aReturnVal.SetPointer(new UtScriptRef(commPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationChangeLeadSubCommand, WsfFormationChangeLeadSubCommand, GetNewLeaderName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetNewLeaderName());
}
