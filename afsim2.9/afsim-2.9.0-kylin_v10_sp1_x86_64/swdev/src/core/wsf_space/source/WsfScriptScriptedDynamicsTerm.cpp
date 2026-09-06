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

#include "WsfScriptScriptedDynamicsTerm.hpp"

#include "UtMemory.hpp"
#include "WsfScriptedOrbitalDynamicsTerm.hpp"

WsfScriptScriptedDynamicsTerm::WsfScriptScriptedDynamicsTerm(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptOrbitalDynamicsTerm(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfScriptedDynamicsTerm");

   AddStaticMethod(ut::make_unique<Construct>());
   AddMethod(ut::make_unique<ScriptName>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptScriptedDynamicsTerm,
                        WsfScriptedOrbitalDynamicsTerm,
                        Construct,
                        1,
                        "WsfScriptedDynamicsTerm",
                        "string")
{
   auto termPtr = ut::make_unique<WsfScriptedOrbitalDynamicsTerm>();
   termPtr->SetScriptName(aVarArgs[0].GetString());
   aReturnVal.SetPointer(new UtScriptRef(termPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptScriptedDynamicsTerm, WsfScriptedOrbitalDynamicsTerm, ScriptName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetScriptName());
}
