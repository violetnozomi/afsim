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

#include "WsfScriptDirectedEnergyWeaponClass.hpp"

#include "UtMemory.hpp"
#include "WsfDirectedEnergyWeapon.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrack.hpp"
#include "WsfWeapon.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptManager.hpp"

WsfScriptDirectedEnergyWeaponClass::WsfScriptDirectedEnergyWeaponClass(const std::string& aClassName,
                                                                       UtScriptTypes*     aScriptTypesPtr)
   : WsfScriptImplicitWeaponClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfDirectedEnergyWeapon");
   AddMethod(ut::make_unique<IsFiring>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDirectedEnergyWeaponClass, WsfDirectedEnergyWeapon, IsFiring, 0, "bool", "")
{
   bool ok    = aObjectPtr->IsFiring();
   aReturnVal = ok;
}
