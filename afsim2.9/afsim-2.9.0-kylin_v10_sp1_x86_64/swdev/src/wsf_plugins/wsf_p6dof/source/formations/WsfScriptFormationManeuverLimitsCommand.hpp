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

#ifndef WSFSCRIPTFORMATIONMANEUVERLIMITSCOMMAND_HPP
#define WSFSCRIPTFORMATIONMANEUVERLIMITSCOMMAND_HPP

#include "wsf_p6dof_export.h"

#include "WsfScriptFormationCommand.hpp"

class WSF_P6DOF_EXPORT WsfScriptFormationManeuverLimitsCommand : public WsfScriptFormationCommand
{
public:
   WsfScriptFormationManeuverLimitsCommand(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~WsfScriptFormationManeuverLimitsCommand() override = default;

   UT_DECLARE_SCRIPT_METHOD(Construct_1);
   UT_DECLARE_SCRIPT_METHOD(Construct_2);

   UT_DECLARE_SCRIPT_METHOD(SetMaxPitchG_Load);
   UT_DECLARE_SCRIPT_METHOD(GetMaxPitchG_Load);
   UT_DECLARE_SCRIPT_METHOD(SetMaxBankAngle);
   UT_DECLARE_SCRIPT_METHOD(GetMaxBankAngle);
   UT_DECLARE_SCRIPT_METHOD(SetMaxRollRate);
   UT_DECLARE_SCRIPT_METHOD(GetMaxRollRate);
};

#endif // WSFSCRIPTFORMATIONMANEUVERLIMITSCOMMAND_HPP
