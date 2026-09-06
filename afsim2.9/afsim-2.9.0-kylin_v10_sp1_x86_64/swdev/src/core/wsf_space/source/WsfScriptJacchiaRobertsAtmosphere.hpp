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

#ifndef WSFSCRIPTJACCHIAROBERTSATMOSPHERE_HPP
#define WSFSCRIPTJACCHIAROBERTSATMOSPHERE_HPP

#include "WsfScriptAtmosphere.hpp"

class WsfScriptJacchiaRobertsAtmosphere : public WsfScriptAtmosphere
{
public:
   WsfScriptJacchiaRobertsAtmosphere(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~WsfScriptJacchiaRobertsAtmosphere() override = default;

   UT_DECLARE_SCRIPT_METHOD(Construct);
   UT_DECLARE_SCRIPT_METHOD(SolarFlux);
   UT_DECLARE_SCRIPT_METHOD(AverageSolarFlux);
   UT_DECLARE_SCRIPT_METHOD(GeomagneticIndex);
};

#endif // WSFSCRIPTJACCHIAROBERTSATMOSPHERE_HPP
