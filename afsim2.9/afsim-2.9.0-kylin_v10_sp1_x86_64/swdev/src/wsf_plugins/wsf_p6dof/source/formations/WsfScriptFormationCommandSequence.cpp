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

#include "WsfScriptFormationCommandSequence.hpp"

#include "UtMemory.hpp"
#include "WsfFormationCommand.hpp"
#include "WsfFormationCommandSequence.hpp"

WsfScriptFormationCommandSequence::WsfScriptFormationCommandSequence(const std::string& aClassName,
                                                                     UtScriptTypes*     aScriptTypesPtr)
   : WsfScriptFormationCommand(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfFormationCommandSequence");

   AddStaticMethod(ut::make_unique<Construct>());

   AddMethod(ut::make_unique<AppendCommand_1>("AppendCommand"));
   AddMethod(ut::make_unique<AppendCommand_2>("AppendCommand"));
   AddMethod(ut::make_unique<GetNumCommands>());
   AddMethod(ut::make_unique<GetCurrentCommand>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationCommandSequence,
                        WsfFormationCommandSequence,
                        Construct,
                        0,
                        "WsfFormationCommandSequence",
                        "")
{
   auto seqPtr = new WsfFormationCommandSequence{};
   aReturnVal.SetPointer(new UtScriptRef(seqPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationCommandSequence, WsfFormationCommandSequence, AppendCommand_1, 1, "void", "WsfFormationCommand")
{
   auto inputPtr  = aVarArgs[0].GetPointer()->GetAppObject<WsfFormationCommand>();
   auto appendPtr = ut::clone(inputPtr);
   aObjectPtr->AppendCommand(std::move(appendPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationCommandSequence,
                        WsfFormationCommandSequence,
                        AppendCommand_2,
                        2,
                        "void",
                        "WsfFormationCommand, WsfFormationCommandConstraint")
{
   auto inputPtr  = aVarArgs[0].GetPointer()->GetAppObject<WsfFormationCommand>();
   auto appendPtr = ut::clone(inputPtr);
   auto consPtr   = aVarArgs[1].GetPointer()->GetAppObject<WsfFormationCommand::Constraint>();
   appendPtr->SetConstraint(ut::clone(consPtr));
   aObjectPtr->AppendCommand(std::move(appendPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationCommandSequence, WsfFormationCommandSequence, GetNumCommands, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetNumCommands());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationCommandSequence,
                        WsfFormationCommandSequence,
                        GetCurrentCommand,
                        0,
                        "WsfFormationCommand",
                        "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->CurrentCommand(), aReturnClassPtr));
}
