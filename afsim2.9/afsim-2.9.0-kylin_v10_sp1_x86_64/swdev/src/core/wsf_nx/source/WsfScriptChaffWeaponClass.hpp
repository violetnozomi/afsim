// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef WSFSCRIPTCHAFFWEAPONCLASS_HPP
#define WSFSCRIPTCHAFFWEAPONCLASS_HPP

#include "wsf_nx_export.h"

#include "script/WsfScriptWeaponClass.hpp"

//! Define script methods for WsfChaffWeapon
class WSF_NX_EXPORT WsfScriptChaffWeaponClass : public WsfScriptWeaponClass
{
public:
   WsfScriptChaffWeaponClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(DropChaffCloud);
};
#endif
