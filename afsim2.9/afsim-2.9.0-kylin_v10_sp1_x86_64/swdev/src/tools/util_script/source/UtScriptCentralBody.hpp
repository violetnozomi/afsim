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

#ifndef UTSCRIPTCENTRALBODY_HPP
#define UTSCRIPTCENTRALBODY_HPP

#include "UtScriptClassDefine.hpp"
#include "UtScriptTypes.hpp"

namespace ut
{
namespace script
{

class CentralBody : public UtScriptClass
{
public:
   CentralBody(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   ~CentralBody() noexcept override = default;

   void Destroy(void* aObjectPtr) override;
   bool EqualTo(void* aLHS, void* aRHS) override;

   UT_DECLARE_SCRIPT_METHOD(EarthWGS72);
   UT_DECLARE_SCRIPT_METHOD(EarthWGS84);
   UT_DECLARE_SCRIPT_METHOD(EarthEGM96);
   UT_DECLARE_SCRIPT_METHOD(Moon);
   UT_DECLARE_SCRIPT_METHOD(Sun);
};

} // namespace script
} // namespace ut

#endif // UTSCRIPTCENTRALBODY_HPP
