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

#include "WsfScriptSixDOF_FormationManeuverLimitsCommand.hpp"

#include "UtMemory.hpp"
#include "WsfSixDOF_FormationManeuverLimitsCommand.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
FormationManeuverLimitsCommand::FormationManeuverLimitsCommand(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : FormationCommand(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfSixDOF_FormationManeuverLimitsCommand");

   AddStaticMethod(ut::make_unique<Construct_1>("Construct"));
   AddStaticMethod(ut::make_unique<Construct_2>("Construct"));

   AddMethod(ut::make_unique<SetMaxPitchG_Load>());
   AddMethod(ut::make_unique<GetMaxPitchG_Load>());
   AddMethod(ut::make_unique<SetMaxBankAngle>());
   AddMethod(ut::make_unique<GetMaxBankAngle>());
   AddMethod(ut::make_unique<SetMaxRollRate>());
   AddMethod(ut::make_unique<GetMaxRollRate>());
}

UT_DEFINE_SCRIPT_METHOD(FormationManeuverLimitsCommand,
                        wsf::six_dof::FormationManeuverLimitsCommand,
                        Construct_1,
                        0,
                        "WsfSixDOF_FormationManeuverLimitsCommand",
                        "")
{
   wsf::six_dof::FormationManeuverLimitsCommand* cmdPtr = new wsf::six_dof::FormationManeuverLimitsCommand{};
   aReturnVal.SetPointer(new UtScriptRef(cmdPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(FormationManeuverLimitsCommand,
                        wsf::six_dof::FormationManeuverLimitsCommand,
                        Construct_2,
                        3,
                        "WsfSixDOF_FormationManeuverLimitsCommand",
                        "double, double, double")
{
   wsf::six_dof::FormationManeuverLimitsCommand* cmdPtr = new wsf::six_dof::FormationManeuverLimitsCommand{};
   cmdPtr->SetPitchG_LoadMax(aVarArgs[0].GetDouble());
   cmdPtr->SetRollRateMaxDegPerSec(aVarArgs[1].GetDouble());
   cmdPtr->SetBankAngleMaxDeg(aVarArgs[2].GetDouble());
   aReturnVal.SetPointer(new UtScriptRef(cmdPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(FormationManeuverLimitsCommand,
                        wsf::six_dof::FormationManeuverLimitsCommand,
                        SetMaxPitchG_Load,
                        1,
                        "void",
                        "double")
{
   aObjectPtr->SetPitchG_LoadMax(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(FormationManeuverLimitsCommand,
                        wsf::six_dof::FormationManeuverLimitsCommand,
                        GetMaxPitchG_Load,
                        0,
                        "double",
                        "")
{
   aReturnVal.SetDouble(aObjectPtr->GetPitchG_LoadMax());
}

UT_DEFINE_SCRIPT_METHOD(FormationManeuverLimitsCommand,
                        wsf::six_dof::FormationManeuverLimitsCommand,
                        SetMaxBankAngle,
                        1,
                        "void",
                        "double")
{
   aObjectPtr->SetBankAngleMaxDeg(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(FormationManeuverLimitsCommand,
                        wsf::six_dof::FormationManeuverLimitsCommand,
                        GetMaxBankAngle,
                        0,
                        "double",
                        "")
{
   aReturnVal.SetDouble(aObjectPtr->GetBankAngleMaxDeg());
}

UT_DEFINE_SCRIPT_METHOD(FormationManeuverLimitsCommand,
                        wsf::six_dof::FormationManeuverLimitsCommand,
                        SetMaxRollRate,
                        1,
                        "void",
                        "double")
{
   aObjectPtr->SetRollRateMaxDegPerSec(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(FormationManeuverLimitsCommand,
                        wsf::six_dof::FormationManeuverLimitsCommand,
                        GetMaxRollRate,
                        0,
                        "double",
                        "")
{
   aReturnVal.SetDouble(aObjectPtr->GetRollRateMaxDegPerSec());
}
} // namespace script
} // namespace six_dof
} // namespace wsf
