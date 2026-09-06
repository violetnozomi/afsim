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

#include "WsfScriptFormationTurnToHeadingCommand.hpp"

#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "WsfFormationTurnToHeadingCommand.hpp"

WsfScriptFormationTurnToHeadingCommand::WsfScriptFormationTurnToHeadingCommand(const std::string& aClassName,
                                                                               UtScriptTypes*     aScriptTypesPtr)
   : WsfScriptFormationCommand(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfFormationTurnToHeadingCommand");

   AddStaticMethod(ut::make_unique<Construct>());

   AddMethod(ut::make_unique<GetTargetHeading>());
   AddMethod(ut::make_unique<SetTargetHeading>());
   AddMethod(ut::make_unique<GetMinSpeedRatio>());
   AddMethod(ut::make_unique<SetMinSpeedRatio>());
   AddMethod(ut::make_unique<GetMaxSpeedRatio>());
   AddMethod(ut::make_unique<SetMaxSpeedRatio>());
   AddMethod(ut::make_unique<GetSmallAngleLimit>());
   AddMethod(ut::make_unique<SetSmallAngleLimit>());
   AddMethod(ut::make_unique<GetMaxBankAngle>());
   AddMethod(ut::make_unique<SetMaxBankAngle>());
   AddMethod(ut::make_unique<GetMaxRollRate>());
   AddMethod(ut::make_unique<SetMaxRollRate>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationTurnToHeadingCommand,
                        WsfFormationTurnToHeadingCommand,
                        Construct,
                        1,
                        "WsfFormationTurnToHeadingCommand",
                        "double")
{
   WsfFormationTurnToHeadingCommand* returnPtr = new WsfFormationTurnToHeadingCommand();
   returnPtr->SetTargetHeading(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
   aReturnVal.SetPointer(new UtScriptRef(returnPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationTurnToHeadingCommand, WsfFormationTurnToHeadingCommand, GetTargetHeading, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetTargetHeading() * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationTurnToHeadingCommand, WsfFormationTurnToHeadingCommand, SetTargetHeading, 1, "void", "double")
{
   aObjectPtr->SetTargetHeading(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationTurnToHeadingCommand, WsfFormationTurnToHeadingCommand, GetMinSpeedRatio, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetMinSpeedRatio());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationTurnToHeadingCommand, WsfFormationTurnToHeadingCommand, SetMinSpeedRatio, 1, "void", "double")
{
   aObjectPtr->SetMinSpeedRatio(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationTurnToHeadingCommand, WsfFormationTurnToHeadingCommand, GetMaxSpeedRatio, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetMaxSpeedRatio());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationTurnToHeadingCommand, WsfFormationTurnToHeadingCommand, SetMaxSpeedRatio, 1, "void", "double")
{
   aObjectPtr->SetMaxSpeedRatio(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationTurnToHeadingCommand,
                        WsfFormationTurnToHeadingCommand,
                        GetSmallAngleLimit,
                        0,
                        "double",
                        "")
{
   aReturnVal.SetDouble(aObjectPtr->GetSmallThetaLimit());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationTurnToHeadingCommand,
                        WsfFormationTurnToHeadingCommand,
                        SetSmallAngleLimit,
                        1,
                        "void",
                        "double")
{
   aObjectPtr->SetSmallThetaLimit(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationTurnToHeadingCommand, WsfFormationTurnToHeadingCommand, GetMaxBankAngle, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetMaxBankAngle());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationTurnToHeadingCommand, WsfFormationTurnToHeadingCommand, SetMaxBankAngle, 1, "void", "double")
{
   aObjectPtr->SetMaxBankAngle(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationTurnToHeadingCommand, WsfFormationTurnToHeadingCommand, GetMaxRollRate, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetMaxRollRate());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationTurnToHeadingCommand, WsfFormationTurnToHeadingCommand, SetMaxRollRate, 1, "void", "double")
{
   aObjectPtr->SetMaxRollRate(aVarArgs[0].GetDouble());
}
