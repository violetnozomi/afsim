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

#ifndef WSFSCRIPTORBITALREFERENCEFRAME_HPP
#define WSFSCRIPTORBITALREFERENCEFRAME_HPP

#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"

namespace wsf
{
namespace space
{

class ScriptOrbitalReferenceFrame : public UtScriptClass
{
public:
   ScriptOrbitalReferenceFrame(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~ScriptOrbitalReferenceFrame() override = default;

   void Destroy(void* aObjectPtr) override;
   bool EqualTo(void* aLHS, void* aRHS) override;

   // Static creation methods
   UT_DECLARE_SCRIPT_METHOD(INERTIAL);
   UT_DECLARE_SCRIPT_METHOD(RIC);
   UT_DECLARE_SCRIPT_METHOD(NTW);
};

} // namespace space
} // namespace wsf

#endif // WSFSCRIPTORBITALREFERENCEFRAME_HPP
