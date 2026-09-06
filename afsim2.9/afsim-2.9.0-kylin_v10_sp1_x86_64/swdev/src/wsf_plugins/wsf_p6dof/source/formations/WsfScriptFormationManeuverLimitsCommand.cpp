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

#include "WsfScriptFormationManeuverLimitsCommand.hpp"

#include "UtMemory.hpp"
#include "WsfFormationManeuverLimitsCommand.hpp"

WsfScriptFormationManeuverLimitsCommand::WsfScriptFormationManeuverLimitsCommand(const std::string& aClassName,
                                                                                 UtScriptTypes*     aScriptTypesPtr)
   : WsfScriptFormationCommand(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfFormationManeuverLimitsCommand");

   AddStaticMethod(ut::make_unique<Construct_1>("Construct"));
   AddStaticMethod(ut::make_unique<Construct_2>("Construct"));

   AddMethod(ut::make_unique<SetMaxPitchG_Load>());
   AddMethod(ut::make_unique<GetMaxPitchG_Load>());
   AddMethod(ut::make_unique<SetMaxBankAngle>());
   AddMethod(ut::make_unique<GetMaxBankAngle>());
   AddMethod(ut::make_unique<SetMaxRollRate>());
   AddMethod(ut::make_unique<GetMaxRollRate>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationManeuverLimitsCommand,
                        WsfFormationManeuverLimitsCommand,
                        Construct_1,
                        0,
                        "WsfFormationManeuverLimitsCommand",
                        "")
{
   WsfFormationManeuverLimitsCommand* cmdPtr = new WsfFormationManeuverLimitsCommand{};
   aReturnVal.SetPointer(new UtScriptRef(cmdPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationManeuverLimitsCommand,
                        WsfFormationManeuverLimitsCommand,
                        Construct_2,
                        3,
                        "WsfFormationManeuverLimitsCommand",
                        "double, double, double")
{
   WsfFormationManeuverLimitsCommand* cmdPtr = new WsfFormationManeuverLimitsCommand{};
   cmdPtr->SetPitchG_LoadMax(aVarArgs[0].GetDouble());
   cmdPtr->SetRollRateMaxDegPerSec(aVarArgs[1].GetDouble());
   cmdPtr->SetBankAngleMaxDeg(aVarArgs[2].GetDouble());
   aReturnVal.SetPointer(new UtScriptRef(cmdPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationManeuverLimitsCommand,
                        WsfFormationManeuverLimitsCommand,
                        SetMaxPitchG_Load,
                        1,
                        "void",
                        "double")
{
   aObjectPtr->SetPitchG_LoadMax(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationManeuverLimitsCommand,
                        WsfFormationManeuverLimitsCommand,
                        GetMaxPitchG_Load,
                        0,
                        "double",
                        "")
{
   aReturnVal.SetDouble(aObjectPtr->GetPitchG_LoadMax());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationManeuverLimitsCommand,
                        WsfFormationManeuverLimitsCommand,
                        SetMaxBankAngle,
                        1,
                        "void",
                        "double")
{
   aObjectPtr->SetBankAngleMaxDeg(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationManeuverLimitsCommand, WsfFormationManeuverLimitsCommand, GetMaxBankAngle, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetBankAngleMaxDeg());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationManeuverLimitsCommand, WsfFormationManeuverLimitsCommand, SetMaxRollRate, 1, "void", "double")
{
   aObjectPtr->SetRollRateMaxDegPerSec(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationManeuverLimitsCommand, WsfFormationManeuverLimitsCommand, GetMaxRollRate, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetRollRateMaxDegPerSec());
}
