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

#ifndef WSFSCRIPTTURNTOHEADINGMANEUVER_HPP
#define WSFSCRIPTTURNTOHEADINGMANEUVER_HPP

#include "wsf_p6dof_export.h"

#include "WsfScriptManeuver.hpp"

class WSF_P6DOF_EXPORT WsfScriptTurnToHeadingManeuver : public WsfScriptManeuver
{
public:
   WsfScriptTurnToHeadingManeuver(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~WsfScriptTurnToHeadingManeuver() override = default;

   UT_DECLARE_SCRIPT_METHOD(Construct);
   UT_DECLARE_SCRIPT_METHOD(GetHeading);
};

#endif // WSFSCRIPTTURNTOHEADINGMANEUVER_HPP
