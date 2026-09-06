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

#include "WsfScriptOrbitalIntegrator.hpp"

#include "UtMemory.hpp"
#include "WsfOrbitalIntegrator.hpp"

WsfScriptOrbitalIntegrator::WsfScriptOrbitalIntegrator(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   mIsScriptAccessible = true;

   SetClassName("WsfOrbitalIntegrator");

   AddMethod(ut::make_unique<IntegratorType>());
}

void WsfScriptOrbitalIntegrator::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfOrbitalIntegrator*>(aObjectPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptOrbitalIntegrator, WsfOrbitalIntegrator, IntegratorType, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetIntegratorType());
}
