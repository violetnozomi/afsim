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

#include "WsfScriptDE_File.hpp"

#include <memory>

#include "UtCalendar.hpp"
#include "UtMemory.hpp"
#include "UtScriptVec3.hpp"
#include "WsfDE_File.hpp"
#include "WsfDE_FileHandle.hpp"
#include "WsfDE_FileManager.hpp"
#include "WsfSimulation.hpp"
#include "script/WsfScriptDefs.hpp"

WsfScriptDE_File::WsfScriptDE_File(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfDE_File");

   AddStaticMethod(ut::make_unique<Construct>()); // NO_DOC | FOR_TEST_ONLY
   AddMethod(ut::make_unique<DE_Number>());       // NO_DOC | FOR_TEST_ONLY
   AddMethod(ut::make_unique<JD_Start>());        // NO_DOC | FOR_TEST_ONLY
   AddMethod(ut::make_unique<JD_End>());          // NO_DOC | FOR_TEST_ONLY
   AddMethod(ut::make_unique<FileName>());        // NO_DOC | FOR_TEST_ONLY

   AddStaticMethod(ut::make_unique<MERCURY>());                 // NO_DOC | FOR_TEST_ONLY
   AddStaticMethod(ut::make_unique<VENUS>());                   // NO_DOC | FOR_TEST_ONLY
   AddStaticMethod(ut::make_unique<EARTH>());                   // NO_DOC | FOR_TEST_ONLY
   AddStaticMethod(ut::make_unique<MARS>());                    // NO_DOC | FOR_TEST_ONLY
   AddStaticMethod(ut::make_unique<JUPITER>());                 // NO_DOC | FOR_TEST_ONLY
   AddStaticMethod(ut::make_unique<SATURN>());                  // NO_DOC | FOR_TEST_ONLY
   AddStaticMethod(ut::make_unique<URANUS>());                  // NO_DOC | FOR_TEST_ONLY
   AddStaticMethod(ut::make_unique<NEPTUNE>());                 // NO_DOC | FOR_TEST_ONLY
   AddStaticMethod(ut::make_unique<PLUTO>());                   // NO_DOC | FOR_TEST_ONLY
   AddStaticMethod(ut::make_unique<MOON>());                    // NO_DOC | FOR_TEST_ONLY
   AddStaticMethod(ut::make_unique<SUN>());                     // NO_DOC | FOR_TEST_ONLY
   AddStaticMethod(ut::make_unique<SOLAR_SYSTEM_BARYCENTER>()); // NO_DOC | FOR_TEST_ONLY
   AddStaticMethod(ut::make_unique<EARTH_MOON_BARYCENTER>());   // NO_DOC | FOR_TEST_ONLY

   AddMethod(ut::make_unique<PlanetPosition>()); // NO_DOC | FOR_TEST_ONLY
   AddMethod(ut::make_unique<PlanetVelocity>()); // NO_DOC | FOR_TEST_ONLY
}

void WsfScriptDE_File::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfDE_FileHandle*>(aObjectPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDE_File, WsfDE_FileHandle, Construct, 1, "WsfDE_File", "string")
{
   try
   {
      auto handlePtr = WsfDE_FileManager::Get(SIMULATION).GetOrLoadFile(aVarArgs[0].GetString());
      aReturnVal.SetPointer(new UtScriptRef(handlePtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   catch (WsfDE_File::Error&)
   {
      UT_SCRIPT_ABORT("Unable to open file");
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDE_File, WsfDE_FileHandle, DE_Number, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetDE_Num());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDE_File, WsfDE_FileHandle, JD_Start, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetFileStartJD());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDE_File, WsfDE_FileHandle, JD_End, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetFileEndJD());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDE_File, WsfDE_FileHandle, FileName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetFileName());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDE_File, WsfDE_FileHandle, MERCURY, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(WsfDE_File::Body::cMERCURY));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDE_File, WsfDE_FileHandle, VENUS, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(WsfDE_File::Body::cVENUS));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDE_File, WsfDE_FileHandle, EARTH, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(WsfDE_File::Body::cEARTH));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDE_File, WsfDE_FileHandle, MARS, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(WsfDE_File::Body::cMARS));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDE_File, WsfDE_FileHandle, JUPITER, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(WsfDE_File::Body::cJUPITER));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDE_File, WsfDE_FileHandle, SATURN, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(WsfDE_File::Body::cSATURN));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDE_File, WsfDE_FileHandle, URANUS, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(WsfDE_File::Body::cURANUS));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDE_File, WsfDE_FileHandle, NEPTUNE, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(WsfDE_File::Body::cNEPTUNE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDE_File, WsfDE_FileHandle, PLUTO, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(WsfDE_File::Body::cPLUTO));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDE_File, WsfDE_FileHandle, MOON, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(WsfDE_File::Body::cMOON));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDE_File, WsfDE_FileHandle, SUN, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(WsfDE_File::Body::cSUN));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDE_File, WsfDE_FileHandle, SOLAR_SYSTEM_BARYCENTER, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(WsfDE_File::Body::cSOLAR_SYSTEM_BARYCENTER));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDE_File, WsfDE_FileHandle, EARTH_MOON_BARYCENTER, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(WsfDE_File::Body::cEARTH_MOON_BARYCENTER));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDE_File, WsfDE_FileHandle, PlanetPosition, 3, "Vec3", "Calendar, int, int")
{
   // Interpret arguments
   UtCalendar        inputTime{*aVarArgs[0].GetPointer()->GetAppObject<UtCalendar>()};
   const UtCalendar& sdat = SIMULATION->GetDateTime().GetStartDateAndTime();
   inputTime.SetDeltaUT1(sdat.GetDeltaUT1());
   inputTime.SetDeltaAT(sdat.GetDeltaAT());
   WsfDE_File::Date epoch{inputTime.GetJulianTDB_Date(), 0.0};
   auto             target = static_cast<WsfDE_File::Body>(aVarArgs[1].GetInt());
   auto             center = static_cast<WsfDE_File::Body>(aVarArgs[2].GetInt());

   // Call method
   UtVec3d position;
   UtVec3d velocity;
   aObjectPtr->GetPlanetEphemeris(epoch, target, center, position, velocity);
   position.Multiply(1000.0); // km -> m

   // Set return value
   aReturnVal.SetPointer(UtScriptVec3::Create(position));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDE_File, WsfDE_FileHandle, PlanetVelocity, 3, "Vec3", "Calendar, int, int")
{
   // Interpret arguments
   UtCalendar        inputTime{*aVarArgs[0].GetPointer()->GetAppObject<UtCalendar>()};
   const UtCalendar& sdat = SIMULATION->GetDateTime().GetStartDateAndTime();
   inputTime.SetDeltaUT1(sdat.GetDeltaUT1());
   inputTime.SetDeltaAT(sdat.GetDeltaAT());
   WsfDE_File::Date epoch{inputTime.GetJulianTDB_Date(), 0.0};
   auto             target = static_cast<WsfDE_File::Body>(aVarArgs[1].GetInt());
   auto             center = static_cast<WsfDE_File::Body>(aVarArgs[2].GetInt());

   // Call method
   UtVec3d position;
   UtVec3d velocity;
   aObjectPtr->GetPlanetEphemeris(epoch, target, center, position, velocity);
   velocity.Multiply(1000.0); // km/s -> m/s

   // Set return value
   aReturnVal.SetPointer(UtScriptVec3::Create(velocity));
}
