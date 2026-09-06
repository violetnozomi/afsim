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

#ifndef WSFSCRIPTEARTHJ2TERM_HPP
#define WSFSCRIPTEARTHJ2TERM_HPP

#include "WsfScriptOrbitalDynamicsTerm.hpp"

class WsfScriptEarthJ2Term : public WsfScriptOrbitalDynamicsTerm
{
public:
   WsfScriptEarthJ2Term(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~WsfScriptEarthJ2Term() override = default;

   UT_DECLARE_SCRIPT_METHOD(Construct1);
   UT_DECLARE_SCRIPT_METHOD(Construct2);
   UT_DECLARE_SCRIPT_METHOD(GravitationalParameter);
   UT_DECLARE_SCRIPT_METHOD(MeanRadius);
   UT_DECLARE_SCRIPT_METHOD(J2);
   UT_DECLARE_SCRIPT_METHOD(DefaultJ2);
};

#endif // WSFSCRIPTEARTHJ2TERM_HPP
