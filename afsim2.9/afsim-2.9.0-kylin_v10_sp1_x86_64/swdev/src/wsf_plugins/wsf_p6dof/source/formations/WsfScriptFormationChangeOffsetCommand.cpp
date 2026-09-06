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

#include "WsfScriptFormationChangeOffsetCommand.hpp"

#include "UtMemory.hpp"
#include "WsfFormationChangeOffsetCommand.hpp"
#include "WsfFormationOffset.hpp"

WsfScriptFormationChangeOffsetCommand::WsfScriptFormationChangeOffsetCommand(const std::string& aClassName,
                                                                             UtScriptTypes*     aScriptTypesPtr)
   : WsfScriptFormationCommand(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfFormationChangeOffsetCommand");

   AddStaticMethod(ut::make_unique<Construct>());

   AddMethod(ut::make_unique<AddOffset>());
   AddMethod(ut::make_unique<GetNumOffsets>());
   AddMethod(ut::make_unique<SetTransitionTime>());
   AddMethod(ut::make_unique<GetTransitionTime>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationChangeOffsetCommand,
                        WsfFormationChangeOffsetCommand,
                        Construct,
                        0,
                        "WsfFormationChangeOffsetCommand",
                        "")
{
   WsfFormationChangeOffsetCommand* commPtr = new WsfFormationChangeOffsetCommand{};
   aReturnVal.SetPointer(new UtScriptRef(commPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationChangeOffsetCommand, WsfFormationChangeOffsetCommand, AddOffset, 1, "void", "WsfFormationOffset")
{
   WsfFormationOffset* inputOffset = static_cast<WsfFormationOffset*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfFormationOffset  offset{*inputOffset};
   aObjectPtr->AddOffset(offset);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationChangeOffsetCommand, WsfFormationChangeOffsetCommand, GetNumOffsets, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(aObjectPtr->GetOffsetCount()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationChangeOffsetCommand, WsfFormationChangeOffsetCommand, SetTransitionTime, 1, "void", "double")
{
   aObjectPtr->SetTransitionTime(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationChangeOffsetCommand, WsfFormationChangeOffsetCommand, GetTransitionTime, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetTransitionTime());
}
