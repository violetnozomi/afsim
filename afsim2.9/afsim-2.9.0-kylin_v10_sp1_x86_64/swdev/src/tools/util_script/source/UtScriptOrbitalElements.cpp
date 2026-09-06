// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "UtScriptOrbitalElements.hpp"

#include "UtOrbitalElements.hpp"
#include "UtOrbitalState.hpp"
#include "UtScriptCalendar.hpp"

namespace ut
{
namespace script
{

OrbitalElements::OrbitalElements(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("OrbitalElements");

   mConstructible = true;
   mCloneable     = true;

   AddStaticMethod(ut::make_unique<Construct>());

   AddMethod(ut::make_unique<SetSunSynchronous>());
   AddMethod(ut::make_unique<FastForward>());

   // Set functions for orbital elements
   AddMethod(ut::make_unique<SetSemiMajorAxis>());
   AddMethod(ut::make_unique<SetMeanMotion>());
   AddMethod(ut::make_unique<SetEccentricity>());
   AddMethod(ut::make_unique<SetMeanAnomaly>());
   AddMethod(ut::make_unique<SetTrueAnomaly>());
   AddMethod(ut::make_unique<SetInclination>());
   AddMethod(ut::make_unique<SetRAAN>());
   AddMethod(ut::make_unique<SetArgumentOfPeriapsis>());
   AddMethod(ut::make_unique<SetPeriapsisRadius>());
   AddMethod(ut::make_unique<SetApoapsisRadius>());
   AddMethod(ut::make_unique<SetPeriapsisAltitude>());
   AddMethod(ut::make_unique<SetApoapsisAltitude>());

   // Get functions for orbital elements
   AddMethod(ut::make_unique<GetSemiMajorAxis>());
   AddMethod(ut::make_unique<GetMeanMotion>());
   AddMethod(ut::make_unique<GetEccentricity>());
   AddMethod(ut::make_unique<GetMeanAnomaly>());
   AddMethod(ut::make_unique<GetTrueAnomaly>());
   AddMethod(ut::make_unique<GetInclination>());
   AddMethod(ut::make_unique<GetRAAN>());
   AddMethod(ut::make_unique<GetArgumentOfPeriapsis>());
   AddMethod(ut::make_unique<GetPeriapsisRadius>());
   AddMethod(ut::make_unique<GetApoapsisRadius>());
   AddMethod(ut::make_unique<GetPeriapsisAltitude>());
   AddMethod(ut::make_unique<GetApoapsisAltitude>());
}

void OrbitalElements::Destroy(void* aObjectPtr)
{
   delete static_cast<UtOrbitalElements*>(aObjectPtr);
}

void* OrbitalElements::Clone(void* aObjectPtr)
{
   return new UtOrbitalElements(*static_cast<UtOrbitalElements*>(aObjectPtr));
}

UT_DEFINE_SCRIPT_METHOD(OrbitalElements,
                        UtOrbitalElements,
                        Construct,
                        8,
                        "OrbitalElements",
                        "Calendar, CentralBody, double, double, double, double, double, double")
{
   auto  calendarPtr    = aVarArgs[0].GetPointer()->GetAppObject<UtCalendar>();
   auto* centralBodyPtr = aVarArgs[1].GetPointer()->GetAppObject<ut::CentralBody>();

   // Convert angle inputs from degrees to radians
   double meanAnomalyRad    = aVarArgs[4].GetDouble() * UtMath::cRAD_PER_DEG;
   double inclinationRad    = aVarArgs[5].GetDouble() * UtMath::cRAD_PER_DEG;
   double raanRad           = aVarArgs[6].GetDouble() * UtMath::cRAD_PER_DEG;
   double argOfPeriapsisRad = aVarArgs[7].GetDouble() * UtMath::cRAD_PER_DEG;

   auto orbitalElementsPtr = ut::make_unique<UtOrbitalElements>(*calendarPtr,
                                                                *centralBodyPtr,
                                                                aVarArgs[2].GetDouble(),
                                                                aVarArgs[3].GetDouble(),
                                                                meanAnomalyRad,
                                                                inclinationRad,
                                                                raanRad,
                                                                argOfPeriapsisRad);
   aReturnVal.SetPointer(new UtScriptRef(orbitalElementsPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(OrbitalElements, UtOrbitalElements, SetSunSynchronous, 4, "void", "Calendar, double, double, double")
{
   UtCalendar* calendarPtr = static_cast<UtCalendar*>(aVarArgs[0].GetPointer()->GetAppObject());
   aObjectPtr->SetSunSynchronous(*calendarPtr, aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble(), aVarArgs[3].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(OrbitalElements, UtOrbitalElements, FastForward, 1, "OrbitalElements", "double")
{
   auto futureOrbitalElementsPtr = ut::make_unique<UtOrbitalElements>(aObjectPtr->FastForward(aVarArgs[0].GetDouble()));
   aReturnVal.SetPointer(new UtScriptRef(futureOrbitalElementsPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(OrbitalElements, UtOrbitalElements, SetSemiMajorAxis, 1, "void", "double")
{
   aObjectPtr->SetSemiMajorAxis(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(OrbitalElements, UtOrbitalElements, SetMeanMotion, 1, "void", "double")
{
   double meanMotionRad = aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG;
   aObjectPtr->SetMeanMotion(meanMotionRad);
}

UT_DEFINE_SCRIPT_METHOD(OrbitalElements, UtOrbitalElements, SetEccentricity, 1, "void", "double")
{
   aObjectPtr->SetEccentricity(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(OrbitalElements, UtOrbitalElements, SetMeanAnomaly, 1, "void", "double")
{
   double meanAnomalyRad = aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG;
   aObjectPtr->SetMeanAnomaly(meanAnomalyRad);
}

UT_DEFINE_SCRIPT_METHOD(OrbitalElements, UtOrbitalElements, SetTrueAnomaly, 1, "void", "double")
{
   double trueAnomalyRad = aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG;
   aObjectPtr->SetTrueAnomaly(trueAnomalyRad);
}

UT_DEFINE_SCRIPT_METHOD(OrbitalElements, UtOrbitalElements, SetInclination, 1, "void", "double")
{
   double inclinationRad = aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG;
   aObjectPtr->SetInclination(inclinationRad);
}

UT_DEFINE_SCRIPT_METHOD(OrbitalElements, UtOrbitalElements, SetRAAN, 1, "void", "double")
{
   double raanRad = aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG;
   aObjectPtr->SetRAAN(raanRad);
}

UT_DEFINE_SCRIPT_METHOD(OrbitalElements, UtOrbitalElements, SetArgumentOfPeriapsis, 1, "void", "double")
{
   double argOfPeriapsisRad = aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG;
   aObjectPtr->SetArgumentOfPeriapsis(argOfPeriapsisRad);
}

UT_DEFINE_SCRIPT_METHOD(OrbitalElements, UtOrbitalElements, SetPeriapsisRadius, 1, "void", "double")
{
   aObjectPtr->SetPeriapsisRadius(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(OrbitalElements, UtOrbitalElements, SetApoapsisRadius, 1, "void", "double")
{
   aObjectPtr->SetApoapsisRadius(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(OrbitalElements, UtOrbitalElements, SetPeriapsisAltitude, 1, "void", "double")
{
   aObjectPtr->SetPeriapsisAltitude(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(OrbitalElements, UtOrbitalElements, SetApoapsisAltitude, 1, "void", "double")
{
   aObjectPtr->SetApoapsisAltitude(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(OrbitalElements, UtOrbitalElements, GetSemiMajorAxis, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetSemiMajorAxis());
}

UT_DEFINE_SCRIPT_METHOD(OrbitalElements, UtOrbitalElements, GetMeanMotion, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetMeanMotion() * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(OrbitalElements, UtOrbitalElements, GetEccentricity, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetEccentricity());
}

UT_DEFINE_SCRIPT_METHOD(OrbitalElements, UtOrbitalElements, GetMeanAnomaly, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetMeanAnomaly() * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(OrbitalElements, UtOrbitalElements, GetTrueAnomaly, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetTrueAnomaly() * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(OrbitalElements, UtOrbitalElements, GetInclination, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetInclination() * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(OrbitalElements, UtOrbitalElements, GetRAAN, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetRAAN() * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(OrbitalElements, UtOrbitalElements, GetArgumentOfPeriapsis, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetArgumentOfPeriapsis() * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(OrbitalElements, UtOrbitalElements, GetPeriapsisRadius, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetPeriapsisRadius());
}

UT_DEFINE_SCRIPT_METHOD(OrbitalElements, UtOrbitalElements, GetApoapsisRadius, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetApoapsisRadius());
}

UT_DEFINE_SCRIPT_METHOD(OrbitalElements, UtOrbitalElements, GetPeriapsisAltitude, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetPeriapsisAltitude());
}

UT_DEFINE_SCRIPT_METHOD(OrbitalElements, UtOrbitalElements, GetApoapsisAltitude, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetApoapsisAltitude());
}

} // namespace script
} // namespace ut
