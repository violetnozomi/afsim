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

#ifndef WSFSCRIPTFLIGHTPATHANGLEMANEUVER_HPP
#define WSFSCRIPTFLIGHTPATHANGLEMANEUVER_HPP

#include "wsf_p6dof_export.h"

#include "WsfScriptManeuver.hpp"

class WSF_P6DOF_EXPORT WsfScriptFlightPathAngleManeuver : public WsfScriptManeuver
{
public:
   WsfScriptFlightPathAngleManeuver(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~WsfScriptFlightPathAngleManeuver() override = default;

   UT_DECLARE_SCRIPT_METHOD(Construct);
   UT_DECLARE_SCRIPT_METHOD(GetFlightPathAngle);
};

#endif // WSFSCRIPTFLIGHTPATHANGLEMANEUVER_HPP
