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

#ifndef WSFSCRIPTEARTHMONOPOLETERM_HPP
#define WSFSCRIPTEARTHMONOPOLETERM_HPP

#include "WsfScriptOrbitalDynamicsTerm.hpp"

class WsfScriptEarthMonopoleTerm : public WsfScriptOrbitalDynamicsTerm
{
public:
   WsfScriptEarthMonopoleTerm(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~WsfScriptEarthMonopoleTerm() override = default;

   UT_DECLARE_SCRIPT_METHOD(Construct1);
   UT_DECLARE_SCRIPT_METHOD(Construct2);
   UT_DECLARE_SCRIPT_METHOD(Construct3);
   UT_DECLARE_SCRIPT_METHOD(GravitationalParameter);
};

#endif // WSFSCRIPTEARTHMONOPOLETERM_HPP
