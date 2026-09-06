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

#include "WsfScriptSixDOF_FormationAttachCommand.hpp"

#include "UtMemory.hpp"
#include "WsfSixDOF_FormationAttachCommand.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
FormationAttachCommand::FormationAttachCommand(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : FormationCommand(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfSixDOF_FormationAttachCommand");

   AddStaticMethod(ut::make_unique<Construct>());
}

UT_DEFINE_SCRIPT_METHOD(FormationAttachCommand,
                        wsf::six_dof::FormationAttachCommand,
                        Construct,
                        0,
                        "WsfSixDOF_FormationAttachCommand",
                        "")
{
   wsf::six_dof::FormationAttachCommand* commPtr = new wsf::six_dof::FormationAttachCommand{};
   aReturnVal.SetPointer(new UtScriptRef(commPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}
} // namespace script
} // namespace six_dof
} // namespace wsf
