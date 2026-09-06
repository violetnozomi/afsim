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

#ifndef WSFSCRIPTGUIDEDMOVERTESTOBJECT_HPP
#define WSFSCRIPTGUIDEDMOVERTESTOBJECT_HPP

#include "wsf_mil_export.h"

#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"

class WSF_MIL_EXPORT WsfScriptGuidedMoverTestObject : public UtScriptClass
{
public:
   WsfScriptGuidedMoverTestObject(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(GetLevelFlightThrust);       // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(GetLevelFlightDrag);         // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(GetLevelFlightFuelBurnRate); // NO_DOC | FOR_TEST_ONLY
};


#endif
