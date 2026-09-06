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

#ifndef WSFSCRIPTSPEEDKCASMANEUVER_HPP
#define WSFSCRIPTSPEEDKCASMANEUVER_HPP

#include "wsf_p6dof_export.h"

#include "WsfScriptManeuver.hpp"

class WSF_P6DOF_EXPORT WsfScriptSpeedKCAS_Maneuver : public WsfScriptManeuver
{
public:
   WsfScriptSpeedKCAS_Maneuver(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~WsfScriptSpeedKCAS_Maneuver() override = default;

   UT_DECLARE_SCRIPT_METHOD(Construct);
   UT_DECLARE_SCRIPT_METHOD(GetSpeed);
};

#endif // WSFSCRIPTSPEEDKCASMANEUVER_HPP
