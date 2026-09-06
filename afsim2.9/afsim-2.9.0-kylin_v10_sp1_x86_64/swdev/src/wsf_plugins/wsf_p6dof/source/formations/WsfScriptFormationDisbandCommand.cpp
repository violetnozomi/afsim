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

#include "WsfScriptFormationDisbandCommand.hpp"

#include "UtMemory.hpp"
#include "WsfFormationDisbandCommand.hpp"

WsfScriptFormationDisbandCommand::WsfScriptFormationDisbandCommand(const std::string& aClassName,
                                                                   UtScriptTypes*     aScriptTypesPtr)
   : WsfScriptFormationCommand(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfFormationDisbandCommand");

   AddStaticMethod(ut::make_unique<Construct>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationDisbandCommand, WsfFormationDisbandCommand, Construct, 0, "WsfFormationDisbandCommand", "")
{
   WsfFormationDisbandCommand* commPtr = new WsfFormationDisbandCommand{};
   aReturnVal.SetPointer(new UtScriptRef(commPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}
