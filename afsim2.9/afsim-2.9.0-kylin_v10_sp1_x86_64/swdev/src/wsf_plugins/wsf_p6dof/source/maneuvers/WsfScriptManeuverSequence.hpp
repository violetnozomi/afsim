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

#ifndef WSFSCRIPTMANEUVERSEQUENCE_HPP
#define WSFSCRIPTMANEUVERSEQUENCE_HPP

#include "wsf_p6dof_export.h"

#include "WsfScriptManeuver.hpp"

class WSF_P6DOF_EXPORT WsfScriptManeuverSequence : public WsfScriptManeuver
{
public:
   WsfScriptManeuverSequence(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~WsfScriptManeuverSequence() override = default;

   UT_DECLARE_SCRIPT_METHOD(Construct);

   UT_DECLARE_SCRIPT_METHOD(IsLoop);
   UT_DECLARE_SCRIPT_METHOD(SetLoop);

   UT_DECLARE_SCRIPT_METHOD(Clear);
   UT_DECLARE_SCRIPT_METHOD(Append);
   UT_DECLARE_SCRIPT_METHOD(InsertNext);

   UT_DECLARE_SCRIPT_METHOD(GetCurrentManeuver);
};

#endif // WSFSCRIPTMANEUVERSEQUENCE_HPP
