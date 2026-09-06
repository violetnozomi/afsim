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

#include "WsfScriptFormationAttachCommand.hpp"

#include "UtMemory.hpp"
#include "WsfFormationAttachCommand.hpp"

WsfScriptFormationAttachCommand::WsfScriptFormationAttachCommand(const std::string& aClassName,
                                                                 UtScriptTypes*     aScriptTypesPtr)
   : WsfScriptFormationCommand(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfFormationAttachCommand");

   AddStaticMethod(ut::make_unique<Construct>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationAttachCommand, WsfFormationAttachCommand, Construct, 0, "WsfFormationAttachCommand", "")
{
   WsfFormationAttachCommand* commPtr = new WsfFormationAttachCommand{};
   aReturnVal.SetPointer(new UtScriptRef(commPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}
