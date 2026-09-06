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

#include "WsfScriptSixDOF_FormationChangeOffsetCommand.hpp"

#include "UtMemory.hpp"
#include "WsfSixDOF_FormationChangeOffsetCommand.hpp"
#include "WsfSixDOF_FormationOffset.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
FormationChangeOffsetCommand::FormationChangeOffsetCommand(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : FormationCommand(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfSixDOF_FormationChangeOffsetCommand");

   AddStaticMethod(ut::make_unique<Construct>());

   AddMethod(ut::make_unique<AddOffset>());
   AddMethod(ut::make_unique<GetNumOffsets>());
   AddMethod(ut::make_unique<SetTransitionTime>());
   AddMethod(ut::make_unique<GetTransitionTime>());
}

UT_DEFINE_SCRIPT_METHOD(FormationChangeOffsetCommand,
                        wsf::six_dof::FormationChangeOffsetCommand,
                        Construct,
                        0,
                        "WsfSixDOF_FormationChangeOffsetCommand",
                        "")
{
   wsf::six_dof::FormationChangeOffsetCommand* commPtr = new wsf::six_dof::FormationChangeOffsetCommand{};
   aReturnVal.SetPointer(new UtScriptRef(commPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(FormationChangeOffsetCommand,
                        wsf::six_dof::FormationChangeOffsetCommand,
                        AddOffset,
                        1,
                        "void",
                        "WsfSixDOF_FormationOffset")
{
   wsf::six_dof::FormationOffset* inputOffset =
      static_cast<wsf::six_dof::FormationOffset*>(aVarArgs[0].GetPointer()->GetAppObject());
   wsf::six_dof::FormationOffset offset{*inputOffset};
   aObjectPtr->AddOffset(offset);
}

UT_DEFINE_SCRIPT_METHOD(FormationChangeOffsetCommand, wsf::six_dof::FormationChangeOffsetCommand, GetNumOffsets, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(aObjectPtr->GetOffsetCount()));
}

UT_DEFINE_SCRIPT_METHOD(FormationChangeOffsetCommand,
                        wsf::six_dof::FormationChangeOffsetCommand,
                        SetTransitionTime,
                        1,
                        "void",
                        "double")
{
   aObjectPtr->SetTransitionTime(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(FormationChangeOffsetCommand, wsf::six_dof::FormationChangeOffsetCommand, GetTransitionTime, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetTransitionTime());
}
} // namespace script
} // namespace six_dof
} // namespace wsf
