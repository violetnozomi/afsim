// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfScriptImplicitWeaponClass.hpp"

#include "UtMemory.hpp"
#include "WsfImplicitWeapon.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrack.hpp"
#include "WsfWeapon.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptManager.hpp"

WsfScriptImplicitWeaponClass::WsfScriptImplicitWeaponClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptWeaponClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfImplicitWeapon");

   AddMethod(ut::make_unique<Fire1>("Fire"));
   AddMethod(ut::make_unique<Fire2>("Fire"));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptImplicitWeaponClass, WsfImplicitWeapon, Fire1, 1, "bool", "string")
{
   double                simTime = WsfScriptContext::GetTIME_NOW(aContext);
   WsfWeapon::FireTarget target;
   target.mTargetName = aVarArgs[0].GetString();
   bool ok            = aObjectPtr->Fire(simTime, target, WsfWeapon::FireOptions());
   aReturnVal         = ok;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptImplicitWeaponClass, WsfImplicitWeapon, Fire2, 2, "bool", "string, string")
{
   double                simTime = WsfScriptContext::GetTIME_NOW(aContext);
   WsfWeapon::FireTarget target;
   target.mTargetName   = aVarArgs[0].GetString();
   target.mTargetOffset = aVarArgs[1].GetString();
   bool ok              = aObjectPtr->Fire(simTime, target, WsfWeapon::FireOptions());
   aReturnVal           = ok;
}
