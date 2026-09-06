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

#ifndef UTSCRIPTREFERENCEFRAME_HPP
#define UTSCRIPTREFERENCEFRAME_HPP

#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"

namespace ut
{
namespace script
{

class ReferenceFrame : public UtScriptClass
{
public:
   ReferenceFrame(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~ReferenceFrame() noexcept override = default;

   void Destroy(void* aObjectPtr) override;
   bool EqualTo(void* aLHS, void* aRHS) override;

   UT_DECLARE_SCRIPT_METHOD(Inertial);
   UT_DECLARE_SCRIPT_METHOD(J2000);
   UT_DECLARE_SCRIPT_METHOD(TRUE_OF_DATE);
   UT_DECLARE_SCRIPT_METHOD(TEME);
};

} // namespace script
} // namespace ut

#endif // UTSCRIPTREFERENCEFRAME_HPP
