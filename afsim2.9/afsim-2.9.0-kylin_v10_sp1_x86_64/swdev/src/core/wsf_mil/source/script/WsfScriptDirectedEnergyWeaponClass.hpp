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

#ifndef WSFSCRIPTDIRECTEDENERGYWEAPONCLASS_HPP
#define WSFSCRIPTDIRECTEDENERGYWEAPONCLASS_HPP

#include "wsf_mil_export.h"

#include "WsfScriptImplicitWeaponClass.hpp"

//! Define script methods for WsfDirectedEnergyWeapon.
class WSF_MIL_EXPORT WsfScriptDirectedEnergyWeaponClass : public WsfScriptImplicitWeaponClass
{
public:
   WsfScriptDirectedEnergyWeaponClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(IsFiring);
};

#endif
