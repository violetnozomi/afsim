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

#include "WsfScriptSixDOF_ManeuverConstraint.hpp"

#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "WsfSixDOF_ManeuverAbsoluteTimeConstraint.hpp"
#include "WsfSixDOF_ManeuverAltitudeConstraint.hpp"
#include "WsfSixDOF_ManeuverFlightPathAngleConstraint.hpp"
#include "WsfSixDOF_ManeuverHeadingConstraint.hpp"
#include "WsfSixDOF_ManeuverPitchConstraint.hpp"
#include "WsfSixDOF_ManeuverRelativeTimeConstraint.hpp"
#include "WsfSixDOF_ManeuverRollConstraint.hpp"
#include "WsfSixDOF_ManeuverScriptConstraint.hpp"
#include "script/WsfScriptDefs.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
ManeuverConstraint::ManeuverConstraint(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfSixDOF_ManeuverConstraint");

   AddStaticMethod(ut::make_unique<AT_TIME>());
   AddStaticMethod(ut::make_unique<AT_RELATIVE_TIME>());
   AddStaticMethod(ut::make_unique<AT_ROLL_ANGLE>());
   AddStaticMethod(ut::make_unique<AT_PITCH_ANGLE>());
   AddStaticMethod(ut::make_unique<AT_HEADING>());
   AddStaticMethod(ut::make_unique<AT_ALTITUDE>());
   AddStaticMethod(ut::make_unique<AT_FLIGHT_PATH_ANGLE>());
   AddStaticMethod(ut::make_unique<SCRIPT>());

   AddMethod(ut::make_unique<Type>());
}

void ManeuverConstraint::Destroy(void* aObjectPtr)
{
   delete static_cast<wsf::six_dof::ManeuverConstraint*>(aObjectPtr);
}

void* ManeuverConstraint::Clone(void* aObjectPtr)
{
   return static_cast<wsf::six_dof::ManeuverConstraint*>(aObjectPtr)->Clone();
}

UT_DEFINE_SCRIPT_METHOD(ManeuverConstraint, wsf::six_dof::ManeuverConstraint, AT_TIME, 1, "WsfSixDOF_ManeuverConstraint", "double")
{
   wsf::six_dof::ManeuverAbsoluteTimeConstraint* consPtr = new wsf::six_dof::ManeuverAbsoluteTimeConstraint{};
   consPtr->SetConstraintTime(aVarArgs[0].GetDouble());
   aReturnVal.SetPointer(new UtScriptRef(consPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(ManeuverConstraint,
                        wsf::six_dof::ManeuverConstraint,
                        AT_RELATIVE_TIME,
                        1,
                        "WsfSixDOF_ManeuverConstraint",
                        "double")
{
   wsf::six_dof::ManeuverRelativeTimeConstraint* consPtr = new wsf::six_dof::ManeuverRelativeTimeConstraint{};
   consPtr->SetConstraintTime(aVarArgs[0].GetDouble());
   aReturnVal.SetPointer(new UtScriptRef(consPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(ManeuverConstraint,
                        wsf::six_dof::ManeuverConstraint,
                        AT_ROLL_ANGLE,
                        1,
                        "WsfSixDOF_ManeuverConstraint",
                        "double")
{
   wsf::six_dof::ManeuverRollConstraint* consPtr = new wsf::six_dof::ManeuverRollConstraint{};
   consPtr->SetRollTarget(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
   aReturnVal.SetPointer(new UtScriptRef(consPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(ManeuverConstraint,
                        wsf::six_dof::ManeuverConstraint,
                        AT_PITCH_ANGLE,
                        1,
                        "WsfSixDOF_ManeuverConstraint",
                        "double")
{
   wsf::six_dof::ManeuverPitchConstraint* consPtr = new wsf::six_dof::ManeuverPitchConstraint{};
   consPtr->SetPitchTarget(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
   aReturnVal.SetPointer(new UtScriptRef(consPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(ManeuverConstraint, wsf::six_dof::ManeuverConstraint, AT_HEADING, 1, "WsfSixDOF_ManeuverConstraint", "double")
{
   wsf::six_dof::ManeuverHeadingConstraint* consPtr = new wsf::six_dof::ManeuverHeadingConstraint{};
   consPtr->SetHeadingTarget(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
   aReturnVal.SetPointer(new UtScriptRef(consPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(ManeuverConstraint, wsf::six_dof::ManeuverConstraint, AT_ALTITUDE, 1, "WsfSixDOF_ManeuverConstraint", "double")
{
   wsf::six_dof::ManeuverAltitudeConstraint* consPtr = new wsf::six_dof::ManeuverAltitudeConstraint{};
   consPtr->SetAltitudeTarget(aVarArgs[0].GetDouble());
   aReturnVal.SetPointer(new UtScriptRef(consPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(ManeuverConstraint,
                        wsf::six_dof::ManeuverConstraint,
                        AT_FLIGHT_PATH_ANGLE,
                        1,
                        "WsfSixDOF_ManeuverConstraint",
                        "double")
{
   wsf::six_dof::ManeuverFlightPathAngleConstraint* consPtr = new wsf::six_dof::ManeuverFlightPathAngleConstraint{};
   consPtr->SetFlightPathAngle(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
   aReturnVal.SetPointer(new UtScriptRef(consPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(ManeuverConstraint, wsf::six_dof::ManeuverConstraint, SCRIPT, 2, "WsfSixDOF_ManeuverConstraint", "string, string")
{
   wsf::six_dof::ManeuverScriptConstraint* consPtr = new wsf::six_dof::ManeuverScriptConstraint{};
   consPtr->SetSatisfiedScript(aVarArgs[0].GetString(), *SIMULATION);
   consPtr->SetNextEvaluationTimeScript(aVarArgs[1].GetString(), *SIMULATION);
   aReturnVal.SetPointer(new UtScriptRef(consPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(ManeuverConstraint, wsf::six_dof::ManeuverConstraint, Type, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->Type());
}
} // namespace script
} // namespace six_dof
} // namespace wsf
