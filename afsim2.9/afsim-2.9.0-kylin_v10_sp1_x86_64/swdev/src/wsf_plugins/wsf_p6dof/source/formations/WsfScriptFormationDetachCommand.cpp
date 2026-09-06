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

#include "WsfScriptFormationDetachCommand.hpp"

#include "UtMemory.hpp"
#include "WsfFormationDetachCommand.hpp"

WsfScriptFormationDetachCommand::WsfScriptFormationDetachCommand(const std::string& aClassName,
                                                                 UtScriptTypes*     aScriptTypesPtr)
   : WsfScriptFormationCommand(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfFormationDetachCommand");

   AddStaticMethod(ut::make_unique<Construct>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationDetachCommand, WsfFormationDetachCommand, Construct, 0, "WsfFormationDetachCommand", "")
{
   WsfFormationDetachCommand* commPtr = new WsfFormationDetachCommand{};
   aReturnVal.SetPointer(new UtScriptRef(commPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}
