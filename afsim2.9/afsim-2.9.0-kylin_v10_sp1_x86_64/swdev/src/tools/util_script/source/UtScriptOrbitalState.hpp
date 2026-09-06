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

#ifndef UTSCRIPTORBITALSTATE_HPP
#define UTSCRIPTORBITALSTATE_HPP

#include "UtScriptClassDefine.hpp"
#include "UtScriptTypes.hpp"

namespace ut
{
namespace script
{

class OrbitalState : public UtScriptClass
{
public:
   OrbitalState(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   ~OrbitalState() noexcept override = default;

   void  Destroy(void* aObjectPtr) override;
   void* Clone(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(Construct_OE);
   UT_DECLARE_SCRIPT_METHOD(Construct_OE_Add);
   UT_DECLARE_SCRIPT_METHOD(Construct_RV);
   UT_DECLARE_SCRIPT_METHOD(Construct_RV_Add);

   UT_DECLARE_SCRIPT_METHOD(OrbitIsHyperbolic);
   UT_DECLARE_SCRIPT_METHOD(OrbitIsCircular);
   UT_DECLARE_SCRIPT_METHOD(OrbitIsEquatorial);
   UT_DECLARE_SCRIPT_METHOD(OrbitIsPrograde);
};

} // namespace script
} // namespace ut

#endif // UTSCRIPTORBITALSTATE_HPP
