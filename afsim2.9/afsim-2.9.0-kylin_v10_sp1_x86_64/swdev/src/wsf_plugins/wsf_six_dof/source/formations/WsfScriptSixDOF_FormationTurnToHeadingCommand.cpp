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

#include "WsfScriptSixDOF_FormationTurnToHeadingCommand.hpp"

#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "WsfSixDOF_FormationTurnToHeadingCommand.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
FormationTurnToHeadingCommand::FormationTurnToHeadingCommand(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : FormationCommand(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfSixDOF_FormationTurnToHeadingCommand");

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

UT_DEFINE_SCRIPT_METHOD(FormationTurnToHeadingCommand,
                        wsf::six_dof::FormationTurnToHeadingCommand,
                        Construct,
                        1,
                        "WsfSixDOF_FormationTurnToHeadingCommand",
                        "double")
{
   wsf::six_dof::FormationTurnToHeadingCommand* returnPtr = new wsf::six_dof::FormationTurnToHeadingCommand();
   returnPtr->SetTargetHeading(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
   aReturnVal.SetPointer(new UtScriptRef(returnPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(FormationTurnToHeadingCommand,
                        wsf::six_dof::FormationTurnToHeadingCommand,
                        GetTargetHeading,
                        0,
                        "double",
                        "")
{
   aReturnVal.SetDouble(aObjectPtr->GetTargetHeading() * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(FormationTurnToHeadingCommand,
                        wsf::six_dof::FormationTurnToHeadingCommand,
                        SetTargetHeading,
                        1,
                        "void",
                        "double")
{
   aObjectPtr->SetTargetHeading(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
}

UT_DEFINE_SCRIPT_METHOD(FormationTurnToHeadingCommand,
                        wsf::six_dof::FormationTurnToHeadingCommand,
                        GetMinSpeedRatio,
                        0,
                        "double",
                        "")
{
   aReturnVal.SetDouble(aObjectPtr->GetMinSpeedRatio());
}

UT_DEFINE_SCRIPT_METHOD(FormationTurnToHeadingCommand,
                        wsf::six_dof::FormationTurnToHeadingCommand,
                        SetMinSpeedRatio,
                        1,
                        "void",
                        "double")
{
   aObjectPtr->SetMinSpeedRatio(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(FormationTurnToHeadingCommand,
                        wsf::six_dof::FormationTurnToHeadingCommand,
                        GetMaxSpeedRatio,
                        0,
                        "double",
                        "")
{
   aReturnVal.SetDouble(aObjectPtr->GetMaxSpeedRatio());
}

UT_DEFINE_SCRIPT_METHOD(FormationTurnToHeadingCommand,
                        wsf::six_dof::FormationTurnToHeadingCommand,
                        SetMaxSpeedRatio,
                        1,
                        "void",
                        "double")
{
   aObjectPtr->SetMaxSpeedRatio(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(FormationTurnToHeadingCommand,
                        wsf::six_dof::FormationTurnToHeadingCommand,
                        GetSmallAngleLimit,
                        0,
                        "double",
                        "")
{
   aReturnVal.SetDouble(aObjectPtr->GetSmallThetaLimit());
}

UT_DEFINE_SCRIPT_METHOD(FormationTurnToHeadingCommand,
                        wsf::six_dof::FormationTurnToHeadingCommand,
                        SetSmallAngleLimit,
                        1,
                        "void",
                        "double")
{
   aObjectPtr->SetSmallThetaLimit(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(FormationTurnToHeadingCommand, wsf::six_dof::FormationTurnToHeadingCommand, GetMaxBankAngle, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetMaxBankAngle());
}

UT_DEFINE_SCRIPT_METHOD(FormationTurnToHeadingCommand,
                        wsf::six_dof::FormationTurnToHeadingCommand,
                        SetMaxBankAngle,
                        1,
                        "void",
                        "double")
{
   aObjectPtr->SetMaxBankAngle(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(FormationTurnToHeadingCommand, wsf::six_dof::FormationTurnToHeadingCommand, GetMaxRollRate, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetMaxRollRate());
}

UT_DEFINE_SCRIPT_METHOD(FormationTurnToHeadingCommand, wsf::six_dof::FormationTurnToHeadingCommand, SetMaxRollRate, 1, "void", "double")
{
   aObjectPtr->SetMaxRollRate(aVarArgs[0].GetDouble());
}
} // namespace script
} // namespace six_dof
} // namespace wsf
