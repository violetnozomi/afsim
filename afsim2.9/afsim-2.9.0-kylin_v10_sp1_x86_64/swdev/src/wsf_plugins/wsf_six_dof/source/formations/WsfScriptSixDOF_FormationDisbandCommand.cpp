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

#include "WsfScriptSixDOF_FormationDisbandCommand.hpp"

#include "UtMemory.hpp"
#include "WsfSixDOF_FormationDisbandCommand.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
FormationDisbandCommand::FormationDisbandCommand(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : FormationCommand(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfSixDOF_FormationDisbandCommand");

   AddStaticMethod(ut::make_unique<Construct>());
}

UT_DEFINE_SCRIPT_METHOD(FormationDisbandCommand,
                        wsf::six_dof::FormationDisbandCommand,
                        Construct,
                        0,
                        "WsfSixDOF_FormationDisbandCommand",
                        "")
{
   wsf::six_dof::FormationDisbandCommand* commPtr = new wsf::six_dof::FormationDisbandCommand{};
   aReturnVal.SetPointer(new UtScriptRef(commPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}
} // namespace script
} // namespace six_dof
} // namespace wsf
