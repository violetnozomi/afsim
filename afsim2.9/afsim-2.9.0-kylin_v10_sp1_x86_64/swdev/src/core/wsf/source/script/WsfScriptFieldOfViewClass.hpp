// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSCRIPTFIELDOFVIEWCLASS_HPP
#define WSFSCRIPTFIELDOFVIEWCLASS_HPP

#include "wsf_export.h"

#include <string>

#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"

//! Define script methods for WsfFieldOfView.
class WSF_EXPORT WsfScriptFieldOfViewClass : public UtScriptClass
{
public:
   WsfScriptFieldOfViewClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   void* Clone(void* aObject) override;
   void  Destroy(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(Type);
};

#endif
