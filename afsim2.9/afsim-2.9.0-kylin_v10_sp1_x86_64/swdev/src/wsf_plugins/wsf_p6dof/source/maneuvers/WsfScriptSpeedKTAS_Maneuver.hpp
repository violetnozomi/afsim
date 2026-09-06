// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSCRIPTSPEEDKTASMANEUVER_HPP
#define WSFSCRIPTSPEEDKTASMANEUVER_HPP

#include "wsf_p6dof_export.h"

#include "WsfScriptManeuver.hpp"

class WSF_P6DOF_EXPORT WsfScriptSpeedKTAS_Maneuver : public WsfScriptManeuver
{
public:
   WsfScriptSpeedKTAS_Maneuver(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~WsfScriptSpeedKTAS_Maneuver() override = default;

   UT_DECLARE_SCRIPT_METHOD(Construct);
   UT_DECLARE_SCRIPT_METHOD(GetSpeed);
};

#endif // WSFSCRIPTSPEEDKTASMANEUVER_HPP
