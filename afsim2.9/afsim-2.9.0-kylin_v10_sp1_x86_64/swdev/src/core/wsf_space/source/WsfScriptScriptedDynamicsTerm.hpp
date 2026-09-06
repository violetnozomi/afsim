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

#ifndef WSFSCRIPTSCRIPTEDDYNAMICSTERM_HPP
#define WSFSCRIPTSCRIPTEDDYNAMICSTERM_HPP

#include "WsfScriptOrbitalDynamicsTerm.hpp"

class WsfScriptScriptedDynamicsTerm : public WsfScriptOrbitalDynamicsTerm
{
public:
   WsfScriptScriptedDynamicsTerm(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~WsfScriptScriptedDynamicsTerm() override = default;

   UT_DECLARE_SCRIPT_METHOD(Construct);
   UT_DECLARE_SCRIPT_METHOD(ScriptName);
};

#endif // WSFSCRIPTSCRIPTEDDYNAMICSTERM_HPP
