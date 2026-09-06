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

#ifndef WSFSCRIPTFORMATIONTURNTOHEADINGCOMMAND_HPP
#define WSFSCRIPTFORMATIONTURNTOHEADINGCOMMAND_HPP

#include "wsf_p6dof_export.h"

#include "WsfScriptFormationCommand.hpp"

class WSF_P6DOF_EXPORT WsfScriptFormationTurnToHeadingCommand : public WsfScriptFormationCommand
{
public:
   WsfScriptFormationTurnToHeadingCommand(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~WsfScriptFormationTurnToHeadingCommand() override = default;

   UT_DECLARE_SCRIPT_METHOD(Construct);

   UT_DECLARE_SCRIPT_METHOD(GetTargetHeading);
   UT_DECLARE_SCRIPT_METHOD(SetTargetHeading);
   UT_DECLARE_SCRIPT_METHOD(GetMinSpeedRatio);
   UT_DECLARE_SCRIPT_METHOD(SetMinSpeedRatio);
   UT_DECLARE_SCRIPT_METHOD(GetMaxSpeedRatio);
   UT_DECLARE_SCRIPT_METHOD(SetMaxSpeedRatio);
   UT_DECLARE_SCRIPT_METHOD(GetSmallAngleLimit);
   UT_DECLARE_SCRIPT_METHOD(SetSmallAngleLimit);
   UT_DECLARE_SCRIPT_METHOD(GetMaxBankAngle);
   UT_DECLARE_SCRIPT_METHOD(SetMaxBankAngle);
   UT_DECLARE_SCRIPT_METHOD(GetMaxRollRate);
   UT_DECLARE_SCRIPT_METHOD(SetMaxRollRate);
};

#endif // WSFSCRIPTFORMATIONTURNTOHEADINGCOMMAND_HPP
