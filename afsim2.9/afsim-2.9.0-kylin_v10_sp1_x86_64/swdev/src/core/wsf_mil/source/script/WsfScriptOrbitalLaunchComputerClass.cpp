// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "script/WsfScriptOrbitalLaunchComputerClass.hpp"

#include <iostream>

#include "UtMemory.hpp"
#include "WsfDateTime.hpp"
#include "WsfEnvironment.hpp"
#include "WsfOrbitalLaunchComputer.hpp"
#include "WsfSimulation.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptDefs.hpp"

// =================================================================================================
WsfScriptOrbitalLaunchComputerClass::WsfScriptOrbitalLaunchComputerClass(const std::string& aClassName,
                                                                         UtScriptTypes*     aScriptTypesPtr)
   : WsfScriptLaunchComputerClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfOrbitalLaunchComputer");

   AddMethod(ut::make_unique<ComputeLEO_Insertion>());
   AddMethod(ut::make_unique<LaunchHeading>());
   // LoftAngle is from the base class...
   AddMethod(ut::make_unique<Inclination>());
   AddMethod(ut::make_unique<InsertionLocation>());
   AddMethod(ut::make_unique<InsertionTimeOfFlight>());
   AddMethod(ut::make_unique<InsertionSpeed>());
   AddMethod(ut::make_unique<InsertionSpeedFixed>());
   AddMethod(ut::make_unique<InsertionHeading>());
   AddMethod(ut::make_unique<InsertionPitch>());
   AddMethod(ut::make_unique<InsertionResidualDeltaV>());
   AddMethod(ut::make_unique<InsertionFuelRemaining>());
   AddMethod(ut::make_unique<InsertionTimeRemaining>());
   AddMethod(ut::make_unique<OrbitalElements>());

   AddStaticMethod(ut::make_unique<ComputeOrbitalElements>());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptOrbitalLaunchComputerClass, WsfOrbitalLaunchComputer, ComputeLEO_Insertion, 2, "bool", "double, double")
{
   double altitude     = aVarArgs[0].GetDouble();
   double inclination  = aVarArgs[1].GetDouble() * UtMath::cRAD_PER_DEG;
   double simTime      = WsfScriptContext::GetTIME_NOW(aContext);
   bool   canIntercept = aObjectPtr->ComputeLEO_Insertion(simTime, altitude, inclination);
   aReturnVal.SetBool(canIntercept);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptOrbitalLaunchComputerClass, WsfOrbitalLaunchComputer, LaunchHeading, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetLaunchHeading() * UtMath::cDEG_PER_RAD);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptOrbitalLaunchComputerClass, WsfOrbitalLaunchComputer, Inclination, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetInclination() * UtMath::cDEG_PER_RAD);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptOrbitalLaunchComputerClass, WsfOrbitalLaunchComputer, InsertionTimeOfFlight, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetInsertionTimeOfFlight());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptOrbitalLaunchComputerClass, WsfOrbitalLaunchComputer, InsertionLocation, 0, "WsfGeoPoint", "")
{
   WsfGeoPoint* pointPtr = new WsfGeoPoint(aObjectPtr->GetInsertionLocation());
   aReturnVal.SetPointer(new UtScriptRef(pointPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptOrbitalLaunchComputerClass, WsfOrbitalLaunchComputer, InsertionSpeed, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetInsertionSpeed());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptOrbitalLaunchComputerClass, WsfOrbitalLaunchComputer, InsertionSpeedFixed, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetInsertionSpeedFixed());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptOrbitalLaunchComputerClass, WsfOrbitalLaunchComputer, InsertionHeading, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetInsertionHeading() * UtMath::cDEG_PER_RAD);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptOrbitalLaunchComputerClass, WsfOrbitalLaunchComputer, InsertionPitch, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetInsertionPitch() * UtMath::cDEG_PER_RAD);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptOrbitalLaunchComputerClass, WsfOrbitalLaunchComputer, InsertionResidualDeltaV, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetInsertionResidualDeltaV());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptOrbitalLaunchComputerClass, WsfOrbitalLaunchComputer, InsertionFuelRemaining, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetInsertionFuelRemaining());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptOrbitalLaunchComputerClass, WsfOrbitalLaunchComputer, InsertionTimeRemaining, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetInsertionTimeRemaining());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptOrbitalLaunchComputerClass, WsfOrbitalLaunchComputer, OrbitalElements, 0, "Array<double>", "")
{
   double oe[6];
   aObjectPtr->GetOrbitalElements(oe);

   // oe[0];                            // GetSemiMajorAxis();
   // oe[1];                            // GetEccentricity();
   oe[2] *= UtMath::cDEG_PER_RAD; // GetInclination();
   oe[3] *= UtMath::cDEG_PER_RAD; // GetMeanAnomaly();
   oe[4] *= UtMath::cDEG_PER_RAD; // GetRAAN();
   oe[5] *= UtMath::cDEG_PER_RAD; // GetArgumentOfPeriapsis();

   std::vector<UtScriptData>* resultVecPtr = new std::vector<UtScriptData>();
   for (int i = 0; i < 6; ++i)
   {
      resultVecPtr->push_back(UtScriptData(oe[i]));
   }
   aReturnVal.SetPointer(new UtScriptRef(resultVecPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// =================================================================================================
// static method
UT_DEFINE_SCRIPT_METHOD(WsfScriptOrbitalLaunchComputerClass,
                        WsfOrbitalLaunchComputer,
                        ComputeOrbitalElements,
                        3,
                        "Array<double>",
                        "Vec3, Vec3, double")
{
   double         oe[6];
   UtVec3d*       eciLocPtr = (UtVec3d*)aVarArgs[0].GetPointer()->GetAppObject();
   UtVec3d*       eciVelPtr = (UtVec3d*)aVarArgs[1].GetPointer()->GetAppObject();
   double         simTime   = aVarArgs[2].GetDouble();
   WsfSimulation* simPtr    = WsfScriptContext::GetSIMULATION(aContext);
   UtCalendar     calendar(simPtr->GetDateTime().GetStartDateAndTime());
   calendar.AdvanceTimeBy(simTime);
   WsfOrbitalLaunchComputer::ComputeOrbitalElements(SIMULATION->GetEnvironment().GetCentralBody(),
                                                    oe,
                                                    eciLocPtr->GetData(),
                                                    eciVelPtr->GetData(),
                                                    calendar);

   // oe[0];                            // GetSemiMajorAxis();
   // oe[1];                            // GetEccentricity();
   oe[2] *= UtMath::cDEG_PER_RAD; // GetInclination();
   oe[3] *= UtMath::cDEG_PER_RAD; // GetMeanAnomaly();
   oe[4] *= UtMath::cDEG_PER_RAD; // GetRAAN();
   oe[5] *= UtMath::cDEG_PER_RAD; // GetArgumentOfPeriapsis();

   std::vector<UtScriptData>* resultVecPtr = new std::vector<UtScriptData>();
   for (int i = 0; i < 6; ++i)
   {
      resultVecPtr->push_back(UtScriptData(oe[i]));
   }
   aReturnVal.SetPointer(new UtScriptRef(resultVecPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}
