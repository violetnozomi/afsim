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

#ifndef WSFSCRIPTMANEUVERCONSTRAINT_HPP
#define WSFSCRIPTMANEUVERCONSTRAINT_HPP

#include "wsf_p6dof_export.h"

#include "UtScriptBasicTypes.hpp"

class WSF_P6DOF_EXPORT WsfScriptManeuverConstraint : public UtScriptClass
{
public:
   WsfScriptManeuverConstraint(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~WsfScriptManeuverConstraint() override = default;

   void  Destroy(void* aObjectPtr) override;
   void* Clone(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(AT_TIME);
   UT_DECLARE_SCRIPT_METHOD(AT_RELATIVE_TIME);
   UT_DECLARE_SCRIPT_METHOD(AT_ROLL_ANGLE);
   UT_DECLARE_SCRIPT_METHOD(AT_PITCH_ANGLE);
   UT_DECLARE_SCRIPT_METHOD(AT_HEADING);
   UT_DECLARE_SCRIPT_METHOD(AT_ALTITUDE);
   UT_DECLARE_SCRIPT_METHOD(AT_FLIGHT_PATH_ANGLE);
   UT_DECLARE_SCRIPT_METHOD(SCRIPT);

   UT_DECLARE_SCRIPT_METHOD(Type);
};

#endif // WSFSCRIPTMANEUVERCONSTRAINT_HPP
