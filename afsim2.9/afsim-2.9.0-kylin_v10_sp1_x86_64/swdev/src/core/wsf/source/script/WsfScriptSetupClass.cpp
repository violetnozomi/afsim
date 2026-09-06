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
#include "script/WsfScriptSetupClass.hpp"

WsfScriptSetupClass::WsfScriptSetupClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   mConstructible = false;
   SetClassName("WsfScenario");
}

// =================================================================================================
WsfScriptSetupClass::~WsfScriptSetupClass() {}

// =================================================================================================
// virtual
void* WsfScriptSetupClass::Create(const UtScriptContext& /*aContext*/)
{
   return nullptr;
}

// =================================================================================================
// virtual
void WsfScriptSetupClass::Destroy(void* aObjectPtr) {}
