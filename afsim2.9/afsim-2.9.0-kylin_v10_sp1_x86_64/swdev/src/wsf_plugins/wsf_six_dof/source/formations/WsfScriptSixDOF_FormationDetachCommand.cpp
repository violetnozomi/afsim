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

#include "WsfScriptSixDOF_FormationDetachCommand.hpp"

#include "UtMemory.hpp"
#include "WsfSixDOF_FormationDetachCommand.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
FormationDetachCommand::FormationDetachCommand(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : FormationCommand(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfSixDOF_FormationDetachCommand");

   AddStaticMethod(ut::make_unique<Construct>());
}

UT_DEFINE_SCRIPT_METHOD(FormationDetachCommand,
                        wsf::six_dof::FormationDetachCommand,
                        Construct,
                        0,
                        "WsfSixDOF_FormationDetachCommand",
                        "")
{
   wsf::six_dof::FormationDetachCommand* commPtr = new wsf::six_dof::FormationDetachCommand{};
   aReturnVal.SetPointer(new UtScriptRef(commPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}
} // namespace script
} // namespace six_dof
} // namespace wsf
