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

#ifndef WSFSCRIPTPRINCEDORMAND45_HPP
#define WSFSCRIPTPRINCEDORMAND45_HPP

#include "WsfScriptOrbitalIntegrator.hpp"

class WsfScriptPrinceDormand45Integrator : public WsfScriptOrbitalIntegrator
{
public:
   WsfScriptPrinceDormand45Integrator(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~WsfScriptPrinceDormand45Integrator() override = default;

   void* Create(const UtScriptContext& aInstance) override;
   void  Destroy(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(Tolerance);
   UT_DECLARE_SCRIPT_METHOD(SetTolerance);
   UT_DECLARE_SCRIPT_METHOD(MaxAdjustmentAttempts);
   UT_DECLARE_SCRIPT_METHOD(SetMaxAdjustmentAttempts);
   UT_DECLARE_SCRIPT_METHOD(MaxStepSize);
   UT_DECLARE_SCRIPT_METHOD(SetMaxStepSize);
   UT_DECLARE_SCRIPT_METHOD(MinStepSize);
   UT_DECLARE_SCRIPT_METHOD(SetMinStepSize);
   UT_DECLARE_SCRIPT_METHOD(ErrorCriterion);
   UT_DECLARE_SCRIPT_METHOD(SetErrorCriterion);
   UT_DECLARE_SCRIPT_METHOD(InitialStepSize);
   UT_DECLARE_SCRIPT_METHOD(SetInitialStepSize);
   UT_DECLARE_SCRIPT_METHOD(L_INFINITY);
   UT_DECLARE_SCRIPT_METHOD(L_TWO);
};

#endif // WSFSCRIPTPRINCEDORMAND45_HPP
