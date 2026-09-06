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

#include "WsfScriptFormationRenameCommand.hpp"

#include "UtMemory.hpp"
#include "WsfFormationRenameCommand.hpp"

WsfScriptFormationRenameCommand::WsfScriptFormationRenameCommand(const std::string& aClassName,
                                                                 UtScriptTypes*     aScriptTypesPtr)
   : WsfScriptFormationCommand(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfFormationRenameCommand");

   AddStaticMethod(ut::make_unique<Construct>());

   AddMethod(ut::make_unique<GetNewName>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationRenameCommand, WsfFormationRenameCommand, Construct, 1, "WsfFormationRenameCommand", "string")
{
   WsfFormationRenameCommand* commPtr = new WsfFormationRenameCommand{};
   commPtr->SetNewName(aVarArgs[0].GetString());
   aReturnVal.SetPointer(new UtScriptRef(commPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationRenameCommand, WsfFormationRenameCommand, GetNewName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetNewName());
}
