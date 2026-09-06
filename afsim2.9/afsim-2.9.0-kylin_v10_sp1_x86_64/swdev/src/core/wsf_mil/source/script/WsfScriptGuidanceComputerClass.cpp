// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "script/WsfScriptGuidanceComputerClass.hpp"

#include "UtEarth.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "WsfGuidanceComputer.hpp"
#include "script/WsfScriptDefs.hpp"

WsfScriptGuidanceComputerClass::WsfScriptGuidanceComputerClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : WsfScriptProcessorClass(aClassName, aTypesPtr)
{
   SetClassName("WsfGuidanceComputer");

   AddMethod(ut::make_unique<SelectPhase>());
   AddMethod(ut::make_unique<Phase>());
   AddMethod(ut::make_unique<EjectStage_1>("EjectStage"));
   AddMethod(ut::make_unique<EjectStage_2>("EjectStage"));
   AddMethod(ut::make_unique<StopEngines>());

   AddMethod(ut::make_unique<ClearCommandedAltitude_1>("ClearCommandedAltitude"));
   AddMethod(ut::make_unique<ClearCommandedAltitude_2>("ClearCommandedAltitude"));

   AddMethod(ut::make_unique<ClearCommandedFlightPathAngle_1>("ClearCommandedFlightPathAngle"));
   AddMethod(ut::make_unique<ClearCommandedFlightPathAngle_2>("ClearCommandedFlightPathAngle"));

   AddMethod(ut::make_unique<ClearCommandedSpeed_1>("ClearCommandedSpeed"));
   AddMethod(ut::make_unique<ClearCommandedSpeed_2>("ClearCommandedSpeed"));

   AddMethod(ut::make_unique<ClearCommandedThrottle_1>("ClearCommandedThrottle"));
   AddMethod(ut::make_unique<ClearCommandedThrottle_2>("ClearCommandedThrottle"));

   AddMethod(ut::make_unique<SetAimpointAltitudeOffset_1>("SetAimpointAltitudeOffset"));
   AddMethod(ut::make_unique<SetAimpointAltitudeOffset_2>("SetAimpointAltitudeOffset"));

   AddMethod(ut::make_unique<SetAimpointAzimuthOffset_1>("SetAimpointAzimuthOffset"));
   AddMethod(ut::make_unique<SetAimpointAzimuthOffset_2>("SetAimpointAzimuthOffset"));

   AddMethod(ut::make_unique<SetAimpointRangeOffset_1>("SetAimpointRangeOffset"));
   AddMethod(ut::make_unique<SetAimpointRangeOffset_2>("SetAimpointRangeOffset"));

   AddMethod(ut::make_unique<SetAimpointEvaluationInterval_1>("SetAimpointEvaluationInterval"));
   AddMethod(ut::make_unique<SetAimpointEvaluationInterval_2>("SetAimpointEvaluationInterval"));

   AddMethod(ut::make_unique<SetAllowRouteFollowing_1>("SetAllowRouteFollowing"));
   AddMethod(ut::make_unique<SetAllowRouteFollowing_2>("SetAllowRouteFollowing"));

   AddMethod(ut::make_unique<SetCommandedAltitude_1>("SetCommandedAltitude"));
   AddMethod(ut::make_unique<SetCommandedAltitude_2>("SetCommandedAltitude"));

   AddMethod(ut::make_unique<SetCommandedAltitudeAGL_1>("SetCommandedAltitudeAGL"));
   AddMethod(ut::make_unique<SetCommandedAltitudeAGL_2>("SetCommandedAltitudeAGL"));

   AddMethod(ut::make_unique<SetCommandedAzimuthOffset_1>("SetCommandedAzimuthOffset"));
   AddMethod(ut::make_unique<SetCommandedAzimuthOffset_2>("SetCommandedAzimuthOffset"));

   AddMethod(ut::make_unique<SetCommandedFlightPathAngle_1>("SetCommandedFlightPathAngle"));
   AddMethod(ut::make_unique<SetCommandedFlightPathAngle_2>("SetCommandedFlightPathAngle"));

   AddMethod(ut::make_unique<SetCommandedMach_1>("SetCommandedMach"));
   AddMethod(ut::make_unique<SetCommandedMach_2>("SetCommandedMach"));

   AddMethod(ut::make_unique<SetCommandedSpeed_1>("SetCommandedSpeed"));
   AddMethod(ut::make_unique<SetCommandedSpeed_2>("SetCommandedSpeed"));

   AddMethod(ut::make_unique<SetCommandedThrottle_1>("SetCommandedThrottle"));
   AddMethod(ut::make_unique<SetCommandedThrottle_2>("SetCommandedThrottle"));

   AddMethod(ut::make_unique<SetGeeBias_1>("SetGeeBias"));
   AddMethod(ut::make_unique<SetGeeBias_2>("SetGeeBias"));

   AddMethod(ut::make_unique<SetGuidanceDelay_1>("SetGuidanceDelay"));
   AddMethod(ut::make_unique<SetGuidanceDelay_2>("SetGuidanceDelay"));

   AddMethod(ut::make_unique<SetGuidanceTarget_1>("SetGuidanceTarget"));
   AddMethod(ut::make_unique<SetGuidanceTarget_2>("SetGuidanceTarget"));

   AddMethod(ut::make_unique<SetLateralGeeBias_1>("SetLateralGeeBias"));
   AddMethod(ut::make_unique<SetLateralGeeBias_2>("SetLateralGeeBias"));

   AddMethod(ut::make_unique<SetMaximumAscentRate_1>("SetMaximumAscentRate"));
   AddMethod(ut::make_unique<SetMaximumAscentRate_2>("SetMaximumAscentRate"));

   AddMethod(ut::make_unique<SetMaximumCommandedGees_1>("SetMaximumCommandedGees"));
   AddMethod(ut::make_unique<SetMaximumCommandedGees_2>("SetMaximumCommandedGees"));

   AddMethod(ut::make_unique<SetMaximumDescentRate_1>("SetMaximumDescentRate"));
   AddMethod(ut::make_unique<SetMaximumDescentRate_2>("SetMaximumDescentRate"));

   AddMethod(ut::make_unique<SetMaximumPitchAngle_1>("SetMaximumPitchAngle"));
   AddMethod(ut::make_unique<SetMaximumPitchAngle_2>("SetMaximumPitchAngle"));

   AddMethod(ut::make_unique<SetPitchChangeGain_1>("SetPitchChangeGain"));
   AddMethod(ut::make_unique<SetPitchChangeGain_2>("SetPitchChangeGain"));

   AddMethod(ut::make_unique<SetProportionalNavigationGain_1>("SetProportionalNavigationGain"));
   AddMethod(ut::make_unique<SetProportionalNavigationGain_2>("SetProportionalNavigationGain"));

   AddMethod(ut::make_unique<SetProportionalNavigationLimitAngle_1>("SetProportionalNavigationLimitAngle"));
   AddMethod(ut::make_unique<SetProportionalNavigationLimitAngle_2>("SetProportionalNavigationLimitAngle"));

   AddMethod(ut::make_unique<SetProportionalNavigationMethod_1>("SetProportionalNavigationMethod"));
   AddMethod(ut::make_unique<SetProportionalNavigationMethod_2>("SetProportionalNavigationMethod"));

   AddMethod(ut::make_unique<SetVelocityPursuitGain_1>("SetVelocityPursuitGain"));
   AddMethod(ut::make_unique<SetVelocityPursuitGain_2>("SetVelocityPursuitGain"));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SelectPhase, 1, "bool", "string")
{
   WsfStringId phaseName(aVarArgs[0].GetString());
   bool        ok = aObjectPtr->SelectPhase(phaseName);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, Phase, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetCurrentPhase().mPhaseName);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, EjectStage_1, 0, "bool", "")
{
   bool ok = aObjectPtr->EjectStage(-1.0, -1.0);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, EjectStage_2, 2, "bool", "double, double")
{
   bool ok = aObjectPtr->EjectStage(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble());
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, StopEngines, 0, "bool", "")
{
   bool ok = aObjectPtr->StopEngines();
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, ClearCommandedAltitude_1, 0, "bool", "")
{
   WsfStringId phaseName;
   bool        ok = aObjectPtr->ClearCommandedAltitude(phaseName);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, ClearCommandedAltitude_2, 1, "bool", "string")
{
   WsfStringId phaseName(aVarArgs[0].GetString());
   bool        ok = aObjectPtr->ClearCommandedAltitude(phaseName);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, ClearCommandedFlightPathAngle_1, 0, "bool", "")
{
   WsfStringId phaseName;
   bool        ok = aObjectPtr->ClearCommandedFlightPathAngle(phaseName);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, ClearCommandedFlightPathAngle_2, 1, "bool", "string")
{
   WsfStringId phaseName(aVarArgs[0].GetString());
   bool        ok = aObjectPtr->ClearCommandedFlightPathAngle(phaseName);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, ClearCommandedSpeed_1, 0, "bool", "")
{
   WsfStringId phaseName;
   bool        ok = aObjectPtr->ClearCommandedSpeed(phaseName);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, ClearCommandedSpeed_2, 1, "bool", "string")
{
   WsfStringId phaseName(aVarArgs[0].GetString());
   bool        ok = aObjectPtr->ClearCommandedSpeed(phaseName);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, ClearCommandedThrottle_1, 0, "bool", "")
{
   WsfStringId phaseName;
   bool        ok = aObjectPtr->ClearCommandedThrottle(phaseName);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, ClearCommandedThrottle_2, 1, "bool", "string")
{
   WsfStringId phaseName(aVarArgs[0].GetString());
   bool        ok = aObjectPtr->ClearCommandedThrottle(phaseName);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetAllowRouteFollowing_1, 1, "bool", "bool")
{
   WsfStringId phaseName;
   bool        value(aVarArgs[0].GetBool());
   bool        ok = aObjectPtr->SetAllowRouteFollowing(phaseName, value);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetAllowRouteFollowing_2, 2, "bool", "string, bool")
{
   WsfStringId phaseName(aVarArgs[0].GetString());
   bool        value(aVarArgs[1].GetBool());
   bool        ok = aObjectPtr->SetAllowRouteFollowing(phaseName, value);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetAimpointAltitudeOffset_1, 1, "bool", "double")
{
   WsfStringId phaseName;
   double      value(aVarArgs[0].GetDouble());
   bool        ok = aObjectPtr->SetAimpointAltitudeOffset(phaseName, value);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetAimpointAltitudeOffset_2, 2, "bool", "string, double")
{
   WsfStringId phaseName(aVarArgs[0].GetString());
   double      value(aVarArgs[1].GetDouble());
   bool        ok = aObjectPtr->SetAimpointAltitudeOffset(phaseName, value);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetAimpointAzimuthOffset_1, 2, "bool", "double, string")
{
   WsfStringId phaseName;
   double      value(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
   std::string direction(aVarArgs[1].GetString());
   bool        ok = aObjectPtr->SetAimpointAzimuthOffset(phaseName, value, direction);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetAimpointAzimuthOffset_2, 3, "bool", "string, double, string")
{
   WsfStringId phaseName(aVarArgs[0].GetString());
   double      value(aVarArgs[1].GetDouble() * UtMath::cRAD_PER_DEG);
   std::string direction(aVarArgs[2].GetString());
   bool        ok = aObjectPtr->SetAimpointAzimuthOffset(phaseName, value, direction);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetAimpointRangeOffset_1, 1, "bool", "double")
{
   WsfStringId phaseName;
   double      value(aVarArgs[0].GetDouble());
   bool        ok = aObjectPtr->SetAimpointRangeOffset(phaseName, value);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetAimpointRangeOffset_2, 2, "bool", "string, double")
{
   WsfStringId phaseName(aVarArgs[0].GetString());
   double      value(aVarArgs[1].GetDouble());
   bool        ok = aObjectPtr->SetAimpointRangeOffset(phaseName, value);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetAimpointEvaluationInterval_1, 1, "bool", "double")
{
   WsfStringId phaseName;
   double      value(aVarArgs[0].GetDouble());
   bool        ok = aObjectPtr->SetAimpointEvaluationInterval(phaseName, value);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetAimpointEvaluationInterval_2, 2, "bool", "string, double")
{
   WsfStringId phaseName(aVarArgs[0].GetString());
   double      value(aVarArgs[1].GetDouble());
   bool        ok = aObjectPtr->SetAimpointEvaluationInterval(phaseName, value);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetCommandedAltitude_1, 1, "bool", "double")
{
   WsfStringId phaseName;
   double      value(aVarArgs[0].GetDouble());
   bool        ok = aObjectPtr->SetCommandedAltitude(phaseName, value, false);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetCommandedAltitude_2, 2, "bool", "string, double")
{
   WsfStringId phaseName(aVarArgs[0].GetString());
   double      value(aVarArgs[1].GetDouble());
   bool        ok = aObjectPtr->SetCommandedAltitude(phaseName, value, false);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetCommandedAltitudeAGL_1, 1, "bool", "double")
{
   WsfStringId phaseName;
   double      value(aVarArgs[0].GetDouble());
   bool        ok = aObjectPtr->SetCommandedAltitude(phaseName, value, true);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetCommandedAltitudeAGL_2, 2, "bool", "string, double")
{
   WsfStringId phaseName(aVarArgs[0].GetString());
   double      value(aVarArgs[1].GetDouble());
   bool        ok = aObjectPtr->SetCommandedAltitude(phaseName, value, true);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetCommandedAzimuthOffset_1, 1, "bool", "double")
{
   WsfStringId phaseName;
   double      value(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
   bool        ok = aObjectPtr->SetCommandedAzimuthOffset(phaseName, value);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetCommandedAzimuthOffset_2, 2, "bool", "string, double")
{
   WsfStringId phaseName(aVarArgs[0].GetString());
   double      value(aVarArgs[1].GetDouble() * UtMath::cRAD_PER_DEG);
   bool        ok = aObjectPtr->SetCommandedAzimuthOffset(phaseName, value);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetCommandedFlightPathAngle_1, 1, "bool", "double")
{
   WsfStringId phaseName;
   double      value(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
   bool        ok = aObjectPtr->SetCommandedFlightPathAngle(phaseName, value);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetCommandedFlightPathAngle_2, 2, "bool", "string, double")
{
   WsfStringId phaseName(aVarArgs[0].GetString());
   double      value(aVarArgs[1].GetDouble() * UtMath::cRAD_PER_DEG);
   bool        ok = aObjectPtr->SetCommandedFlightPathAngle(phaseName, value);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetCommandedMach_1, 1, "bool", "double")
{
   WsfStringId phaseName;
   double      value(aVarArgs[0].GetDouble());
   bool        ok = aObjectPtr->SetCommandedMach(phaseName, value);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetCommandedMach_2, 2, "bool", "string, double")
{
   WsfStringId phaseName(aVarArgs[0].GetString());
   double      value(aVarArgs[1].GetDouble());
   bool        ok = aObjectPtr->SetCommandedMach(phaseName, value);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetCommandedSpeed_1, 1, "bool", "double")
{
   WsfStringId phaseName;
   double      value(aVarArgs[0].GetDouble());
   bool        ok = aObjectPtr->SetCommandedSpeed(phaseName, value);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetCommandedSpeed_2, 2, "bool", "string, double")
{
   WsfStringId phaseName(aVarArgs[0].GetString());
   double      value(aVarArgs[1].GetDouble());
   bool        ok = aObjectPtr->SetCommandedSpeed(phaseName, value);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetCommandedThrottle_1, 1, "bool", "double")
{
   WsfStringId phaseName;
   double      value(aVarArgs[0].GetDouble());
   bool        ok = aObjectPtr->SetCommandedThrottle(phaseName, value);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetCommandedThrottle_2, 2, "bool", "string, double")
{
   WsfStringId phaseName(aVarArgs[0].GetString());
   double      value(aVarArgs[1].GetDouble());
   bool        ok = aObjectPtr->SetCommandedThrottle(phaseName, value);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetGeeBias_1, 1, "bool", "double")
{
   WsfStringId phaseName;
   double      value(aVarArgs[0].GetDouble());
   bool        ok = aObjectPtr->SetGeeBias(phaseName, value);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetGeeBias_2, 2, "bool", "string, double")
{
   WsfStringId phaseName(aVarArgs[0].GetString());
   double      value(aVarArgs[1].GetDouble());
   bool        ok = aObjectPtr->SetGeeBias(phaseName, value);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetGuidanceDelay_1, 1, "bool", "double")
{
   WsfStringId phaseName;
   double      value(aVarArgs[0].GetDouble());
   bool        ok = aObjectPtr->SetGuidanceDelay(phaseName, value);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetGuidanceDelay_2, 2, "bool", "string, double")
{
   WsfStringId phaseName(aVarArgs[0].GetString());
   double      value(aVarArgs[1].GetDouble());
   bool        ok = aObjectPtr->SetGuidanceDelay(phaseName, value);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetGuidanceTarget_1, 1, "bool", "string")
{
   WsfStringId phaseName;
   std::string value(aVarArgs[0].GetString());
   bool        ok = aObjectPtr->SetGuidanceTarget(phaseName, value);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetGuidanceTarget_2, 2, "bool", "string, string")
{
   WsfStringId phaseName(aVarArgs[0].GetString());
   std::string value(aVarArgs[1].GetString());
   bool        ok = aObjectPtr->SetGuidanceTarget(phaseName, value);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetLateralGeeBias_1, 1, "bool", "double")
{
   WsfStringId phaseName;
   double      value(aVarArgs[0].GetDouble());
   bool        ok = aObjectPtr->SetLateralGeeBias(phaseName, value);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetLateralGeeBias_2, 2, "bool", "string, double")
{
   WsfStringId phaseName(aVarArgs[0].GetString());
   double      value(aVarArgs[1].GetDouble());
   bool        ok = aObjectPtr->SetLateralGeeBias(phaseName, value);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetMaximumAscentRate_1, 1, "bool", "double")
{
   WsfStringId phaseName;
   double      value(aVarArgs[0].GetDouble());
   bool        ok = aObjectPtr->SetMaximumAscentRate(phaseName, value);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetMaximumAscentRate_2, 2, "bool", "string, double")
{
   WsfStringId phaseName(aVarArgs[0].GetString());
   double      value(aVarArgs[1].GetDouble());
   bool        ok = aObjectPtr->SetMaximumAscentRate(phaseName, value);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetMaximumCommandedGees_1, 1, "bool", "double")
{
   WsfStringId phaseName;
   double      value(aVarArgs[0].GetDouble() * UtEarth::cACCEL_OF_GRAVITY);
   bool        ok = aObjectPtr->SetMaximumCommandedGees(phaseName, value);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetMaximumCommandedGees_2, 2, "bool", "string, double")
{
   WsfStringId phaseName(aVarArgs[0].GetString());
   double      value(aVarArgs[1].GetDouble() * UtEarth::cACCEL_OF_GRAVITY);
   bool        ok = aObjectPtr->SetMaximumCommandedGees(phaseName, value);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetMaximumDescentRate_1, 1, "bool", "double")
{
   WsfStringId phaseName;
   double      value(aVarArgs[0].GetDouble());
   bool        ok = aObjectPtr->SetMaximumDescentRate(phaseName, value);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetMaximumDescentRate_2, 2, "bool", "string, double")
{
   WsfStringId phaseName(aVarArgs[0].GetString());
   double      value(aVarArgs[1].GetDouble());
   bool        ok = aObjectPtr->SetMaximumDescentRate(phaseName, value);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetMaximumPitchAngle_1, 1, "bool", "double")
{
   WsfStringId phaseName;
   double      value(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
   bool        ok = aObjectPtr->SetMaximumPitchAngle(phaseName, value);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetMaximumPitchAngle_2, 2, "bool", "string, double")
{
   WsfStringId phaseName(aVarArgs[0].GetString());
   double      value(aVarArgs[1].GetDouble() * UtMath::cRAD_PER_DEG);
   bool        ok = aObjectPtr->SetMaximumPitchAngle(phaseName, value);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetPitchChangeGain_1, 1, "bool", "double")
{
   WsfStringId phaseName;
   double      value(aVarArgs[0].GetDouble());
   bool        ok = aObjectPtr->SetPitchChangeGain(phaseName, value);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetPitchChangeGain_2, 2, "bool", "string, double")
{
   WsfStringId phaseName(aVarArgs[0].GetString());
   double      value(aVarArgs[1].GetDouble());
   bool        ok = aObjectPtr->SetPitchChangeGain(phaseName, value);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetProportionalNavigationGain_1, 1, "bool", "double")
{
   WsfStringId phaseName;
   double      value(aVarArgs[0].GetDouble());
   bool        ok = aObjectPtr->SetProportionalNavigationGain(phaseName, value);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetProportionalNavigationGain_2, 2, "bool", "string, double")
{
   WsfStringId phaseName(aVarArgs[0].GetString());
   double      value(aVarArgs[1].GetDouble());
   bool        ok = aObjectPtr->SetProportionalNavigationGain(phaseName, value);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetProportionalNavigationLimitAngle_1, 1, "bool", "double")
{
   WsfStringId phaseName;
   double      value(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
   bool        ok = aObjectPtr->SetProportionalNavigationLimitAngle(phaseName, value);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass,
                        WsfGuidanceComputer,
                        SetProportionalNavigationLimitAngle_2,
                        2,
                        "bool",
                        "string, double")
{
   WsfStringId phaseName(aVarArgs[0].GetString());
   double      value(aVarArgs[1].GetDouble() * UtMath::cRAD_PER_DEG);
   bool        ok = aObjectPtr->SetProportionalNavigationLimitAngle(phaseName, value);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetProportionalNavigationMethod_1, 1, "bool", "string")
{
   WsfStringId phaseName;
   std::string value(aVarArgs[0].GetString());
   bool        ok = aObjectPtr->SetProportionalNavigationMethod(phaseName, value);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetProportionalNavigationMethod_2, 2, "bool", "string, string")
{
   WsfStringId phaseName(aVarArgs[0].GetString());
   std::string value(aVarArgs[1].GetString());
   bool        ok = aObjectPtr->SetProportionalNavigationMethod(phaseName, value);
   aReturnVal.SetBool(ok);
}
UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetVelocityPursuitGain_1, 1, "bool", "double")
{
   WsfStringId phaseName;
   double      value(aVarArgs[0].GetDouble());
   bool        ok = aObjectPtr->SetVelocityPursuitGain(phaseName, value);
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidanceComputerClass, WsfGuidanceComputer, SetVelocityPursuitGain_2, 2, "bool", "string, double")
{
   WsfStringId phaseName(aVarArgs[0].GetString());
   double      value(aVarArgs[1].GetDouble());
   bool        ok = aObjectPtr->SetVelocityPursuitGain(phaseName, value);
   aReturnVal.SetBool(ok);
}
