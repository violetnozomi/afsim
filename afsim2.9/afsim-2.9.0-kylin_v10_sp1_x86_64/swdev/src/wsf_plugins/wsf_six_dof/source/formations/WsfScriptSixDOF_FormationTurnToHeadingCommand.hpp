// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSF_SIXDOF_SCRIPTFORMATIONTURNTOHEADINGCOMMAND_HPP
#define WSF_SIXDOF_SCRIPTFORMATIONTURNTOHEADINGCOMMAND_HPP

#include "wsf_six_dof_export.h"

#include "WsfScriptSixDOF_FormationCommand.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
class WSF_SIX_DOF_EXPORT FormationTurnToHeadingCommand : public FormationCommand
{
public:
   FormationTurnToHeadingCommand(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~FormationTurnToHeadingCommand() override = default;

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
} // namespace script
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_SCRIPTFORMATIONTURNTOHEADINGCOMMAND_HPP
