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

#ifndef WSFSCRIPTVISUALPARTCLASS_HPP
#define WSFSCRIPTVISUALPARTCLASS_HPP

#include "wsf_export.h"

#include "script/WsfScriptArticulatedPartClass.hpp"

//! Define script methods for visual part.
class WSF_EXPORT WsfScriptVisualPartClass : public WsfScriptArticulatedPartClass
{
public:
   WsfScriptVisualPartClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(IsA_TypeOf); // static IsA_TypeOf("derived-type", "base-type")

   UT_DECLARE_SCRIPT_METHOD(TurnOff);
   UT_DECLARE_SCRIPT_METHOD(TurnOn);
};

#endif
