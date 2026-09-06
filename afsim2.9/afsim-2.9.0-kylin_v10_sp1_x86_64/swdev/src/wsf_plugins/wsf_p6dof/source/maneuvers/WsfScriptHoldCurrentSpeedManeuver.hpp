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

#ifndef WSFSCRIPTHOLDCURRENTSPEEDMANEUVER_HPP
#define WSFSCRIPTHOLDCURRENTSPEEDMANEUVER_HPP

#include "wsf_p6dof_export.h"

#include "WsfScriptManeuver.hpp"

class WSF_P6DOF_EXPORT WsfScriptHoldCurrentSpeedManeuver : public WsfScriptManeuver
{
public:
   WsfScriptHoldCurrentSpeedManeuver(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~WsfScriptHoldCurrentSpeedManeuver() override = default;

   UT_DECLARE_SCRIPT_METHOD(Construct);
};

#endif // WSFSCRIPTHOLDCURRENTSPEEDMANEUVER_HPP
