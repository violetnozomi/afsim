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

#ifndef WSFSCRIPTIMPLICTWEAPONCLASS_HPP
#define WSFSCRIPTIMPLICTWEAPONCLASS_HPP

#include "wsf_mil_export.h"

#include "WsfScriptWeaponClass.hpp"

//! Define script methods for WsfWeapon.
class WSF_MIL_EXPORT WsfScriptImplicitWeaponClass : public WsfScriptWeaponClass
{
public:
   WsfScriptImplicitWeaponClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(Fire1);
   UT_DECLARE_SCRIPT_METHOD(Fire2);
};

#endif
