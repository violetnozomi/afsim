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

#ifndef UTSCRIPTORBITALELEMENTS_HPP
#define UTSCRIPTORBITALELEMENTS_HPP

#include "UtScriptClassDefine.hpp"
#include "UtScriptTypes.hpp"

namespace ut
{
namespace script
{

class OrbitalElements : public UtScriptClass
{
public:
   OrbitalElements(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   ~OrbitalElements() noexcept override = default;

   void  Destroy(void* aObjectPtr) override;
   void* Clone(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(Construct);

   UT_DECLARE_SCRIPT_METHOD(SetSunSynchronous);
   UT_DECLARE_SCRIPT_METHOD(FastForward);

   UT_DECLARE_SCRIPT_METHOD(SetSemiMajorAxis);
   UT_DECLARE_SCRIPT_METHOD(SetMeanMotion);
   UT_DECLARE_SCRIPT_METHOD(SetEccentricity);
   UT_DECLARE_SCRIPT_METHOD(SetMeanAnomaly);
   UT_DECLARE_SCRIPT_METHOD(SetTrueAnomaly);
   UT_DECLARE_SCRIPT_METHOD(SetInclination);
   UT_DECLARE_SCRIPT_METHOD(SetRAAN);
   UT_DECLARE_SCRIPT_METHOD(SetArgumentOfPeriapsis);
   UT_DECLARE_SCRIPT_METHOD(SetPeriapsisRadius);
   UT_DECLARE_SCRIPT_METHOD(SetApoapsisRadius);
   UT_DECLARE_SCRIPT_METHOD(SetPeriapsisAltitude);
   UT_DECLARE_SCRIPT_METHOD(SetApoapsisAltitude);

   UT_DECLARE_SCRIPT_METHOD(GetSemiMajorAxis);
   UT_DECLARE_SCRIPT_METHOD(GetMeanMotion);
   UT_DECLARE_SCRIPT_METHOD(GetEccentricity);
   UT_DECLARE_SCRIPT_METHOD(GetMeanAnomaly);
   UT_DECLARE_SCRIPT_METHOD(GetTrueAnomaly);
   UT_DECLARE_SCRIPT_METHOD(GetInclination);
   UT_DECLARE_SCRIPT_METHOD(GetRAAN);
   UT_DECLARE_SCRIPT_METHOD(GetArgumentOfPeriapsis);
   UT_DECLARE_SCRIPT_METHOD(GetPeriapsisRadius);
   UT_DECLARE_SCRIPT_METHOD(GetApoapsisRadius);
   UT_DECLARE_SCRIPT_METHOD(GetPeriapsisAltitude);
   UT_DECLARE_SCRIPT_METHOD(GetApoapsisAltitude);
};

} // namespace script
} // namespace ut

#endif // UTSCRIPTORBITALELEMENTS_HPP
