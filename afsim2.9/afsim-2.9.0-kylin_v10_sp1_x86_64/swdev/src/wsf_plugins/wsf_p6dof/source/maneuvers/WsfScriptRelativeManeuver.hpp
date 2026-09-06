// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSCRIPTRELATIVEMANEUVER_HPP
#define WSFSCRIPTRELATIVEMANEUVER_HPP

#include "WsfScriptManeuver.hpp"

class WsfScriptRelativeManeuver : public WsfScriptManeuver
{
public:
   WsfScriptRelativeManeuver(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~WsfScriptRelativeManeuver() override = default;

   UT_DECLARE_SCRIPT_METHOD(GetG_LoadMax);
   UT_DECLARE_SCRIPT_METHOD(SetG_LoadMax);
   UT_DECLARE_SCRIPT_METHOD(GetG_LoadMin);
   UT_DECLARE_SCRIPT_METHOD(SetG_LoadMin);
   UT_DECLARE_SCRIPT_METHOD(GetSpeedKTAS_Max);
   UT_DECLARE_SCRIPT_METHOD(SetSpeedKTAS_Max);
   UT_DECLARE_SCRIPT_METHOD(GetSpeedKTAS_Min);
   UT_DECLARE_SCRIPT_METHOD(SetSpeedKTAS_Min);
   UT_DECLARE_SCRIPT_METHOD(GetSpeedMpsMax);
   UT_DECLARE_SCRIPT_METHOD(SetSpeedMpsMax);
   UT_DECLARE_SCRIPT_METHOD(GetSpeedMpsMin);
   UT_DECLARE_SCRIPT_METHOD(SetSpeedMpsMin);
};

#endif // WSFSCRIPTRELATIVEMANEUVER_HPP
