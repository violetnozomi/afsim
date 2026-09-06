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

#include "WsfScriptFormationRemoveSubCommand.hpp"

#include "UtMemory.hpp"
#include "WsfFormationRemoveSubCommand.hpp"

WsfScriptFormationRemoveSubCommand::WsfScriptFormationRemoveSubCommand(const std::string& aClassName,
                                                                       UtScriptTypes*     aScriptTypesPtr)
   : WsfScriptFormationCommand(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfFormationRemoveSubCommand");

   AddStaticMethod(ut::make_unique<Construct>());

   AddMethod(ut::make_unique<GetFormationToRemove>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationRemoveSubCommand,
                        WsfFormationRemoveSubCommand,
                        Construct,
                        1,
                        "WsfFormationRemoveSubCommand",
                        "string")
{
   WsfFormationRemoveSubCommand* commPtr = new WsfFormationRemoveSubCommand{};
   commPtr->SetSubformationToRemove(aVarArgs[0].GetString());
   aReturnVal.SetPointer(new UtScriptRef(commPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationRemoveSubCommand, WsfFormationRemoveSubCommand, GetFormationToRemove, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetSubformationToRemove());
}
