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

#include "WsfScriptSixDOF_ScriptedManeuver.hpp"

#include "UtMemory.hpp"
#include "WsfSixDOF_ScriptedManeuver.hpp"
#include "script/WsfScriptDefs.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
ScriptedManeuver::ScriptedManeuver(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : Maneuver(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfSixDOF_ScriptedManeuver");

   AddStaticMethod(ut::make_unique<Construct>());
}

UT_DEFINE_SCRIPT_METHOD(ScriptedManeuver, wsf::six_dof::ScriptedManeuver, Construct, 1, "WsfSixDOF_ScriptedManeuver", "string")
{
   wsf::six_dof::ScriptedManeuver* mvrPtr = new wsf::six_dof::ScriptedManeuver{};
   mvrPtr->SetExecuteScript(aVarArgs[0].GetString(), *SIMULATION);
   aReturnVal.SetPointer(new UtScriptRef(mvrPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}
} // namespace script
} // namespace six_dof
} // namespace wsf
