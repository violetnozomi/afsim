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

#include "WsfScriptManeuverConstraint.hpp"

#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "WsfManeuverAbsoluteTimeConstraint.hpp"
#include "WsfManeuverAltitudeConstraint.hpp"
#include "WsfManeuverFlightPathAngleConstraint.hpp"
#include "WsfManeuverHeadingConstraint.hpp"
#include "WsfManeuverPitchConstraint.hpp"
#include "WsfManeuverRelativeTimeConstraint.hpp"
#include "WsfManeuverRollConstraint.hpp"
#include "WsfManeuverScriptConstraint.hpp"
#include "script/WsfScriptDefs.hpp"

WsfScriptManeuverConstraint::WsfScriptManeuverConstraint(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfManeuverConstraint");

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

void WsfScriptManeuverConstraint::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfManeuverConstraint*>(aObjectPtr);
}

void* WsfScriptManeuverConstraint::Clone(void* aObjectPtr)
{
   return static_cast<WsfManeuverConstraint*>(aObjectPtr)->Clone();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptManeuverConstraint, WsfManeuverConstraint, AT_TIME, 1, "WsfManeuverConstraint", "double")
{
   WsfManeuverAbsoluteTimeConstraint* consPtr = new WsfManeuverAbsoluteTimeConstraint{};
   consPtr->SetConstraintTime(aVarArgs[0].GetDouble());
   aReturnVal.SetPointer(new UtScriptRef(consPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptManeuverConstraint, WsfManeuverConstraint, AT_RELATIVE_TIME, 1, "WsfManeuverConstraint", "double")
{
   WsfManeuverRelativeTimeConstraint* consPtr = new WsfManeuverRelativeTimeConstraint{};
   consPtr->SetConstraintTime(aVarArgs[0].GetDouble());
   aReturnVal.SetPointer(new UtScriptRef(consPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptManeuverConstraint, WsfManeuverConstraint, AT_ROLL_ANGLE, 1, "WsfManeuverConstraint", "double")
{
   WsfManeuverRollConstraint* consPtr = new WsfManeuverRollConstraint{};
   consPtr->SetRollTarget(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
   aReturnVal.SetPointer(new UtScriptRef(consPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptManeuverConstraint, WsfManeuverConstraint, AT_PITCH_ANGLE, 1, "WsfManeuverConstraint", "double")
{
   WsfManeuverPitchConstraint* consPtr = new WsfManeuverPitchConstraint{};
   consPtr->SetPitchTarget(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
   aReturnVal.SetPointer(new UtScriptRef(consPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptManeuverConstraint, WsfManeuverConstraint, AT_HEADING, 1, "WsfManeuverConstraint", "double")
{
   WsfManeuverHeadingConstraint* consPtr = new WsfManeuverHeadingConstraint{};
   consPtr->SetHeadingTarget(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
   aReturnVal.SetPointer(new UtScriptRef(consPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptManeuverConstraint, WsfManeuverConstraint, AT_ALTITUDE, 1, "WsfManeuverConstraint", "double")
{
   WsfManeuverAltitudeConstraint* consPtr = new WsfManeuverAltitudeConstraint{};
   consPtr->SetAltitudeTarget(aVarArgs[0].GetDouble());
   aReturnVal.SetPointer(new UtScriptRef(consPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptManeuverConstraint, WsfManeuverConstraint, AT_FLIGHT_PATH_ANGLE, 1, "WsfManeuverConstraint", "double")
{
   WsfManeuverFlightPathAngleConstraint* consPtr = new WsfManeuverFlightPathAngleConstraint{};
   consPtr->SetFlightPathAngle(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
   aReturnVal.SetPointer(new UtScriptRef(consPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptManeuverConstraint, WsfManeuverConstraint, SCRIPT, 2, "WsfManeuverConstraint", "string, string")
{
   WsfManeuverScriptConstraint* consPtr = new WsfManeuverScriptConstraint{};
   consPtr->SetSatisfiedScript(aVarArgs[0].GetString(), *SIMULATION);
   consPtr->SetNextEvaluationTimeScript(aVarArgs[1].GetString(), *SIMULATION);
   aReturnVal.SetPointer(new UtScriptRef(consPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptManeuverConstraint, WsfManeuverConstraint, Type, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->Type());
}
