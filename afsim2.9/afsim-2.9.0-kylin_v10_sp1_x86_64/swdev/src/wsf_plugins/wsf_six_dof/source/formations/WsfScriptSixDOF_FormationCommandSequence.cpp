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

#include "WsfScriptSixDOF_FormationCommandSequence.hpp"

#include "UtMemory.hpp"
#include "WsfSixDOF_FormationCommand.hpp"
#include "WsfSixDOF_FormationCommandSequence.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
FormationCommandSequence::FormationCommandSequence(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : FormationCommand(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfSixDOF_FormationCommandSequence");

   AddStaticMethod(ut::make_unique<Construct>());

   AddMethod(ut::make_unique<AppendCommand_1>("AppendCommand"));
   AddMethod(ut::make_unique<AppendCommand_2>("AppendCommand"));
   AddMethod(ut::make_unique<GetNumCommands>());
   AddMethod(ut::make_unique<GetCurrentCommand>());
}

UT_DEFINE_SCRIPT_METHOD(FormationCommandSequence,
                        wsf::six_dof::FormationCommandSequence,
                        Construct,
                        0,
                        "WsfSixDOF_FormationCommandSequence",
                        "")
{
   auto seqPtr = new wsf::six_dof::FormationCommandSequence{};
   aReturnVal.SetPointer(new UtScriptRef(seqPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(FormationCommandSequence,
                        wsf::six_dof::FormationCommandSequence,
                        AppendCommand_1,
                        1,
                        "void",
                        "WsfSixDOF_FormationCommand")
{
   auto inputPtr  = aVarArgs[0].GetPointer()->GetAppObject<wsf::six_dof::FormationCommand>();
   auto appendPtr = ut::clone(inputPtr);
   aObjectPtr->AppendCommand(std::move(appendPtr));
}

UT_DEFINE_SCRIPT_METHOD(FormationCommandSequence,
                        wsf::six_dof::FormationCommandSequence,
                        AppendCommand_2,
                        2,
                        "void",
                        "WsfSixDOF_FormationCommand, WsfSixDOF_FormationCommandConstraint")
{
   auto inputPtr  = aVarArgs[0].GetPointer()->GetAppObject<wsf::six_dof::FormationCommand>();
   auto appendPtr = ut::clone(inputPtr);
   auto consPtr   = aVarArgs[1].GetPointer()->GetAppObject<wsf::six_dof::FormationCommand::Constraint>();
   appendPtr->SetConstraint(ut::clone(consPtr));
   aObjectPtr->AppendCommand(std::move(appendPtr));
}

UT_DEFINE_SCRIPT_METHOD(FormationCommandSequence, wsf::six_dof::FormationCommandSequence, GetNumCommands, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetNumCommands());
}

UT_DEFINE_SCRIPT_METHOD(FormationCommandSequence,
                        wsf::six_dof::FormationCommandSequence,
                        GetCurrentCommand,
                        0,
                        "WsfSixDOF_FormationCommand",
                        "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->CurrentCommand(), aReturnClassPtr));
}
} // namespace script
} // namespace six_dof
} // namespace wsf
