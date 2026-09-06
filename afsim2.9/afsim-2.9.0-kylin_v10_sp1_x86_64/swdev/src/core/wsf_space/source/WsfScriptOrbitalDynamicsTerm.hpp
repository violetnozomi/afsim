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

#ifndef WSFSCRIPTORBITALDYNAMICSTERM_HPP
#define WSFSCRIPTORBITALDYNAMICSTERM_HPP

#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"

class WsfScriptOrbitalDynamicsTerm : public UtScriptClass
{
public:
   WsfScriptOrbitalDynamicsTerm(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~WsfScriptOrbitalDynamicsTerm() override = default;

   void Destroy(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(TermType);
   UT_DECLARE_SCRIPT_METHOD(ComputeAcceleration);
};

#endif // WSFSCRIPTORBITALDYNAMICSTERM_HPP
