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

#ifndef WSFSCRIPTATMOSPHERICDRAGTERM_HPP
#define WSFSCRIPTATMOSPHERICDRAGTERM_HPP

#include "WsfScriptOrbitalDynamicsTerm.hpp"

class WsfScriptAtmosphericDragTerm : public WsfScriptOrbitalDynamicsTerm
{
public:
   WsfScriptAtmosphericDragTerm(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~WsfScriptAtmosphericDragTerm() override = default;

   UT_DECLARE_SCRIPT_METHOD(Construct);
   UT_DECLARE_SCRIPT_METHOD(DragCoefficient);
   UT_DECLARE_SCRIPT_METHOD(CrossSectionalArea);
   UT_DECLARE_SCRIPT_METHOD(AtmosphereModelName);
};

#endif // WSFSCRIPTATMOSPHERICDRAGTERM_HPP
