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

#ifndef WSFSCRIPTORBITALINTEGRATOR_HPP
#define WSFSCRIPTORBITALINTEGRATOR_HPP

#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"

class WsfScriptOrbitalIntegrator : public UtScriptClass
{
public:
   WsfScriptOrbitalIntegrator(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~WsfScriptOrbitalIntegrator() override = default;

   void Destroy(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(IntegratorType);
};

#endif // WSFSCRIPTORBITALINTEGRATOR_HPP
