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

#ifndef WSFSCRIPTORBITALDYNAMICS_HPP
#define WSFSCRIPTORBITALDYNAMICS_HPP

#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"

class WsfScriptOrbitalDynamics : public UtScriptClass
{
public:
   explicit WsfScriptOrbitalDynamics(UtScriptTypes* aScriptTypesPtr);
   ~WsfScriptOrbitalDynamics() override = default;

   void* Create(const UtScriptContext& aInstance) override;
   void  Destroy(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(NumTerms);
   UT_DECLARE_SCRIPT_METHOD(Term);
   UT_DECLARE_SCRIPT_METHOD(AddTerm);
   UT_DECLARE_SCRIPT_METHOD(RemoveTerm);
   UT_DECLARE_SCRIPT_METHOD(ComputeAcceleration);
};

#endif // WSFSCRIPTORBITALDYNAMICS_HPP
