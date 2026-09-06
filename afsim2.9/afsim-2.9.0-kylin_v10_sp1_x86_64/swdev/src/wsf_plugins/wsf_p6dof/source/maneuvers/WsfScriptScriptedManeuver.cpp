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

#include "WsfScriptScriptedManeuver.hpp"

#include "UtMemory.hpp"
#include "WsfScriptedManeuver.hpp"
#include "script/WsfScriptDefs.hpp"

WsfScriptScriptedManeuver::WsfScriptScriptedManeuver(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptManeuver(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfScriptedManeuver");

   AddStaticMethod(ut::make_unique<Construct>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptScriptedManeuver, WsfScriptedManeuver, Construct, 1, "WsfScriptedManeuver", "string")
{
   WsfScriptedManeuver* mvrPtr = new WsfScriptedManeuver{};
   mvrPtr->SetExecuteScript(aVarArgs[0].GetString(), *SIMULATION);
   aReturnVal.SetPointer(new UtScriptRef(mvrPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}
