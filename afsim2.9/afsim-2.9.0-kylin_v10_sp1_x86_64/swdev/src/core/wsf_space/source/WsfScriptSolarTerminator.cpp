// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfScriptSolarTerminator.hpp"

#include "UtMemory.hpp"
#include "WsfPlatform.hpp"
#include "WsfSolarTerminator.hpp"
#include "script/WsfScriptDefs.hpp"

WsfScriptSolarTerminator::WsfScriptSolarTerminator(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfSolarTerminator");

   AddStaticMethod(ut::make_unique<GetPhaseOfDay>());
   AddStaticMethod(ut::make_unique<TWILIGHT_CIVIL>());
   AddStaticMethod(ut::make_unique<TWILIGHT_NAUTICAL>());
   AddStaticMethod(ut::make_unique<TWILIGHT_ASTRONOMICAL>());
   AddStaticMethod(ut::make_unique<PHASE_DAY>());
   AddStaticMethod(ut::make_unique<PHASE_TWILIGHT>());
   AddStaticMethod(ut::make_unique<PHASE_NIGHT>());

   AddStaticMethod(ut::make_unique<PlatformSolarIllumination>());
   AddStaticMethod(ut::make_unique<ILLUMINATED>());
   AddStaticMethod(ut::make_unique<EARTH_PENUMBRA>());
   AddStaticMethod(ut::make_unique<EARTH_UMBRA>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSolarTerminator, WsfScriptSolarTerminator, GetPhaseOfDay, 4, "int", "double, double, Calendar, double")
{
   double      lat    = aVarArgs[0].GetDouble();
   double      lon    = aVarArgs[1].GetDouble();
   UtCalendar* calPtr = static_cast<UtCalendar*>(aVarArgs[2].GetPointer()->GetAppObject());
   double      limit  = aVarArgs[3].GetDouble();

   WsfSolarTerminator::PhaseOfDay state = WsfSolarTerminator::GetPhaseOfDay(lat, lon, *calPtr, limit);

   aReturnVal.SetInt(static_cast<int>(state));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSolarTerminator, WsfScriptSolarTerminator, TWILIGHT_CIVIL, 0, "double", "")
{
   aReturnVal.SetDouble(WsfSolarTerminator::cTWILIGHT_CIVIL);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSolarTerminator, WsfScriptSolarTerminator, TWILIGHT_NAUTICAL, 0, "double", "")
{
   aReturnVal.SetDouble(WsfSolarTerminator::cTWILIGHT_NAUTICAL);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSolarTerminator, WsfScriptSolarTerminator, TWILIGHT_ASTRONOMICAL, 0, "double", "")
{
   aReturnVal.SetDouble(WsfSolarTerminator::cTWILIGHT_ASTRONOMICAL);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSolarTerminator, WsfScriptSolarTerminator, PHASE_DAY, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(WsfSolarTerminator::PhaseOfDay::cDAY));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSolarTerminator, WsfScriptSolarTerminator, PHASE_TWILIGHT, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(WsfSolarTerminator::PhaseOfDay::cTWILIGHT));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSolarTerminator, WsfScriptSolarTerminator, PHASE_NIGHT, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(WsfSolarTerminator::PhaseOfDay::cNIGHT));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSolarTerminator, WsfScriptSolarTerminator, PlatformSolarIllumination, 1, "int", "WsfPlatform")
{
   WsfPlatform* platformPtr = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   platformPtr->Update(TIME_NOW);
   auto retval = WsfSolarTerminator::GetPlatformSolarIllumination(platformPtr);
   aReturnVal.SetInt(static_cast<int>(retval));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSolarTerminator, WsfScriptSolarTerminator, ILLUMINATED, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(WsfSolarTerminator::PlatformSolarIllumination::cILLUMINATED));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSolarTerminator, WsfScriptSolarTerminator, EARTH_PENUMBRA, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(WsfSolarTerminator::PlatformSolarIllumination::cEARTH_PENUMBRA));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSolarTerminator, WsfScriptSolarTerminator, EARTH_UMBRA, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(WsfSolarTerminator::PlatformSolarIllumination::cEARTH_UMBRA));
}
