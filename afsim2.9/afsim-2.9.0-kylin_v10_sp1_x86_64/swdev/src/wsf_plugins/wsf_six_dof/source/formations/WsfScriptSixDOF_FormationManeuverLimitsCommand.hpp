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

#ifndef WSF_SIXDOF_SCRIPTFORMATIONMANEUVERLIMITSCOMMAND_HPP
#define WSF_SIXDOF_SCRIPTFORMATIONMANEUVERLIMITSCOMMAND_HPP

#include "wsf_six_dof_export.h"

#include "WsfScriptSixDOF_FormationCommand.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
class WSF_SIX_DOF_EXPORT FormationManeuverLimitsCommand : public FormationCommand
{
public:
   FormationManeuverLimitsCommand(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~FormationManeuverLimitsCommand() override = default;

   UT_DECLARE_SCRIPT_METHOD(Construct_1);
   UT_DECLARE_SCRIPT_METHOD(Construct_2);

   UT_DECLARE_SCRIPT_METHOD(SetMaxPitchG_Load);
   UT_DECLARE_SCRIPT_METHOD(GetMaxPitchG_Load);
   UT_DECLARE_SCRIPT_METHOD(SetMaxBankAngle);
   UT_DECLARE_SCRIPT_METHOD(GetMaxBankAngle);
   UT_DECLARE_SCRIPT_METHOD(SetMaxRollRate);
   UT_DECLARE_SCRIPT_METHOD(GetMaxRollRate);
};
} // namespace script
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_SCRIPTFORMATIONMANEUVERLIMITSCOMMAND_HPP
