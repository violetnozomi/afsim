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

#include "WsfScriptFormationAddSubCommand.hpp"

#include "UtMemory.hpp"
#include "WsfFormationAddSubCommand.hpp"
#include "WsfFormationOffset.hpp"

WsfScriptFormationAddSubCommand::WsfScriptFormationAddSubCommand(const std::string& aClassName,
                                                                 UtScriptTypes*     aScriptTypesPtr)
   : WsfScriptFormationCommand(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfFormationAddSubCommand");

   AddStaticMethod(ut::make_unique<Construct_1>("Construct"));
   AddStaticMethod(ut::make_unique<Construct_2>("Construct"));

   AddMethod(ut::make_unique<GetParentRelativeName>());
   AddMethod(ut::make_unique<GetFormationToAdd>());
   AddMethod(ut::make_unique<GetOffset>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationAddSubCommand,
                        WsfFormationAddSubCommand,
                        Construct_1,
                        2,
                        "WsfFormationAddSubCommand",
                        "string, WsfFormationOffset")
{
   WsfFormationAddSubCommand* commPtr = new WsfFormationAddSubCommand{};
   commPtr->SetFormationToAdd(aVarArgs[0].GetString());
   commPtr->SetOffset(*static_cast<WsfFormationOffset*>(aVarArgs[1].GetPointer()->GetAppObject()));
   aReturnVal.SetPointer(new UtScriptRef(commPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationAddSubCommand,
                        WsfFormationAddSubCommand,
                        Construct_2,
                        3,
                        "WsfFormationAddSubCommand",
                        "string, string, WsfFormationOffset")
{
   WsfFormationAddSubCommand* commPtr = new WsfFormationAddSubCommand{};
   commPtr->SetFormationToAdd(aVarArgs[0].GetString());
   commPtr->SetParentRelativeName(aVarArgs[1].GetString());
   commPtr->SetOffset(*static_cast<WsfFormationOffset*>(aVarArgs[2].GetPointer()->GetAppObject()));
   aReturnVal.SetPointer(new UtScriptRef(commPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationAddSubCommand, WsfFormationAddSubCommand, GetParentRelativeName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetParentRelativeName());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationAddSubCommand, WsfFormationAddSubCommand, GetFormationToAdd, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetFormationToAdd());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationAddSubCommand, WsfFormationAddSubCommand, GetOffset, 0, "WsfFormationOffset", "")
{
   WsfFormationOffset* offsetPtr = new WsfFormationOffset(aObjectPtr->GetOffset());
   aReturnVal.SetPointer(new UtScriptRef(offsetPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}
