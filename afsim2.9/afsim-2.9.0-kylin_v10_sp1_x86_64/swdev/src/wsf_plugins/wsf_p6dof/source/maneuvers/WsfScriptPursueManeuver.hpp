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

#ifndef WSFSCRIPTPURSUEMANEUVER_HPP
#define WSFSCRIPTPURSUEMANEUVER_HPP

#include "wsf_p6dof_export.h"

#include "WsfScriptRelativeManeuver.hpp"

class WSF_P6DOF_EXPORT WsfScriptPursueManeuver : public WsfScriptRelativeManeuver
{
public:
   WsfScriptPursueManeuver(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~WsfScriptPursueManeuver() override = default;

   UT_DECLARE_SCRIPT_METHOD(Construct);
   UT_DECLARE_SCRIPT_METHOD(GetPursuitDistanceMeters);
   UT_DECLARE_SCRIPT_METHOD(SetPursuitDistanceMeters);
   UT_DECLARE_SCRIPT_METHOD(GetTargetPlatformName);
};

#endif // WSFSCRIPTPURSUEMANEUVER_HPP
