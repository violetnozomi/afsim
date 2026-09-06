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

#ifndef WSFSCRIPTSUNMONOPOLETERM_HPP
#define WSFSCRIPTSUNMONOPOLETERM_HPP

#include "WsfScriptOrbitalDynamicsTerm.hpp"

class WsfScriptSunMonopoleTerm : public WsfScriptOrbitalDynamicsTerm
{
public:
   WsfScriptSunMonopoleTerm(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~WsfScriptSunMonopoleTerm() override = default;

   UT_DECLARE_SCRIPT_METHOD(Construct1);
   UT_DECLARE_SCRIPT_METHOD(Construct2);
   UT_DECLARE_SCRIPT_METHOD(GravitationalParameter);
   UT_DECLARE_SCRIPT_METHOD(SunPositionECI);
};

#endif // WSFSCRIPTSUNMONOPOLETERM_HPP
