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

#ifndef UTSCRIPTCOORDINATESYSTEM_HPP
#define UTSCRIPTCOORDINATESYSTEM_HPP

#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"

namespace ut
{
namespace script
{

class CoordinateSystem : public UtScriptClass
{
public:
   CoordinateSystem(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~CoordinateSystem() noexcept override = default;

   void Destroy(void* aObjectPtr) override;
   bool EqualTo(void* aLHS, void* aRHS) override;

   UT_DECLARE_SCRIPT_METHOD(Equatorial);
   UT_DECLARE_SCRIPT_METHOD(Ecliptic);
};

} // namespace script
} // namespace ut

#endif // UTSCRIPTCOORDINATESYSTEM_HPP
