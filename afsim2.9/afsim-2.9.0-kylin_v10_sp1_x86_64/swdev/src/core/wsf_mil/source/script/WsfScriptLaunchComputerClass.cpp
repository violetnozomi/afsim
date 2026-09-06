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

#include "WsfScriptLaunchComputerClass.hpp"

#include <map>

#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "WsfAirToAirLaunchComputer.hpp"
#include "WsfBallisticMissileLaunchComputer.hpp"
#include "WsfLaunchComputer.hpp"
#include "script/WsfScriptContext.hpp"

WsfScriptLaunchComputerClass::WsfScriptLaunchComputerClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : WsfScriptObjectClass(aClassName, aTypesPtr)
{
   SetClassName("WsfLaunchComputer");

   AddMethod(ut::make_unique<Weapon>());   // NO_DOC | DEPRECATED
   AddMethod(ut::make_unique<Platform>()); // NO_DOC | DEPRECATED

   AddMethod(ut::make_unique<HaveResultsFor>());
   AddMethod(ut::make_unique<LaunchTimeIsValid>());
   AddMethod(ut::make_unique<LauncherBearingIsValid>());
   AddMethod(ut::make_unique<LauncherElevationIsValid>());
   AddMethod(ut::make_unique<LoftAngleIsValid>());
   AddMethod(ut::make_unique<BurnTimeIsValid>());
   AddMethod(ut::make_unique<TimeOfFlightIsValid>());
   AddMethod(ut::make_unique<InterceptTimeIsValid>());
   AddMethod(ut::make_unique<InterceptPointIsValid>());
   AddMethod(ut::make_unique<TargetImpactTimeIsValid>());
   AddMethod(ut::make_unique<TargetImpactPointIsValid>());
   AddMethod(ut::make_unique<EndTimeIsValid>());
   AddMethod(ut::make_unique<EndPointIsValid>());

   AddMethod(ut::make_unique<CanIntercept_1>("CanIntercept"));
   AddMethod(ut::make_unique<CanIntercept_2>("CanIntercept"));
   AddMethod(ut::make_unique<LaunchTime>());
   AddMethod(ut::make_unique<LauncherBearing>());
   AddMethod(ut::make_unique<LauncherElevation>());
   AddMethod(ut::make_unique<LoftAngle>());
   AddMethod(ut::make_unique<BurnTime>());
   AddMethod(ut::make_unique<TimeOfFlight>());
   AddMethod(ut::make_unique<InterceptTime>());
   AddMethod(ut::make_unique<InterceptPoint>());
   AddMethod(ut::make_unique<TargetImpactTime>());
   AddMethod(ut::make_unique<TargetImpactPoint>());
   AddMethod(ut::make_unique<EndTime>());
   AddMethod(ut::make_unique<EndPoint>());

   // Special methods for WsfScriptLaunchComputer
   AddMethod(ut::make_unique<SetInterceptTime>());
   AddMethod(ut::make_unique<SetInterceptPoint>());
   AddMethod(ut::make_unique<SetLaunchTime>());
   AddMethod(ut::make_unique<SetTimeOfFlight>());
   AddMethod(ut::make_unique<SetLoftAngle>());

   // Special methods for WsfBallisticMissileComputer
   AddMethod(ut::make_unique<ComputeEndPoint>());
   AddMethod(ut::make_unique<ComputeLaunchWindows>());
   AddMethod(ut::make_unique<LaunchWindowCount>());
   AddMethod(ut::make_unique<FirstLaunchTime>());
   AddMethod(ut::make_unique<FirstInterceptTime>());
   AddMethod(ut::make_unique<FirstInterceptPoint>());
   AddMethod(ut::make_unique<LastLaunchTime>());
   AddMethod(ut::make_unique<LastInterceptTime>());
   AddMethod(ut::make_unique<LastInterceptPoint>());
   AddMethod(ut::make_unique<LateralTargetOffset>());
   AddMethod(ut::make_unique<TargetApogeeTime>());
   AddMethod(ut::make_unique<TargetApogeePoint>());

   // Special methods for WsfAirToAirLaunchComputer
   AddMethod(ut::make_unique<LookupResult>());

   // Deprecated methods
   AddMethod(ut::make_unique<ComputeLaunchWindows>("ComputeLastIntercept")); // NO_DOC | DEPRECATED
   AddMethod(ut::make_unique<LastLaunchTime_OLD>("LastLaunchTime"));         // NO_DOC | DEPRECATED
   AddMethod(ut::make_unique<LastInterceptTime_OLD>("LastInterceptTime"));   // NO_DOC | DEPRECATED
   AddMethod(ut::make_unique<LastInterceptPoint_OLD>("LastInterceptPoint")); // NO_DOC | DEPRECATED
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, Weapon, 0, "WsfWeapon", "") // NO_DOC | DEPRECATED
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetWeapon(), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, Platform, 0, "WsfPlatform", "") // NO_DOC | DEPRECATED
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetPlatform(), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, HaveResultsFor, 1, "bool", "WsfTrackId")
{
   WsfTrackId* trackIdPtr = (WsfTrackId*)aVarArgs[0].GetPointer()->GetAppObject();
   aReturnVal.SetBool(aObjectPtr->HaveResultsFor(*trackIdPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, LaunchTimeIsValid, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->LaunchTimeIsValid());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, LauncherBearingIsValid, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->LauncherBearingIsValid());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, LauncherElevationIsValid, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->LauncherElevationIsValid());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, LoftAngleIsValid, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->LoftAngleIsValid());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, BurnTimeIsValid, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->BurnTimeIsValid());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, TimeOfFlightIsValid, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->TimeOfFlightIsValid());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, InterceptTimeIsValid, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->InterceptTimeIsValid());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, InterceptPointIsValid, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->InterceptPointIsValid());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, TargetImpactTimeIsValid, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->TargetImpactTimeIsValid());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, TargetImpactPointIsValid, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->TargetImpactPointIsValid());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, EndTimeIsValid, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->EndTimeIsValid());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, EndPointIsValid, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->EndPointIsValid());
}

//! bool canIntercept = CanIntercept(WsfTrack aTrack)
UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, CanIntercept_1, 1, "bool", "WsfTrack")
{
   WsfTrack* trackPtr        = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   double    simTime         = WsfScriptContext::GetTIME_NOW(aContext);
   double    timeToIntercept = aObjectPtr->EstimatedTimeToIntercept(simTime, *trackPtr, 0.0);
   bool      canIntercept    = (timeToIntercept < WsfLaunchComputer::cFOREVER);
   aReturnVal.SetBool(canIntercept);
}

//! bool canIntercept = CanIntercept(WsfTrack aTrack, double aLaunchDelayTime)
UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, CanIntercept_2, 2, "bool", "WsfTrack, double")
{
   WsfTrack* trackPtr        = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   double    launchDelayTime = aVarArgs[1].GetDouble();
   double    simTime         = WsfScriptContext::GetTIME_NOW(aContext);
   double    timeToIntercept = aObjectPtr->EstimatedTimeToIntercept(simTime, *trackPtr, launchDelayTime);
   bool      canIntercept    = (timeToIntercept < WsfLaunchComputer::cFOREVER);
   aReturnVal.SetBool(canIntercept);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, LaunchTime, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetLaunchTime());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, LauncherBearing, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetLauncherBearing() * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, LauncherElevation, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetLauncherElevation() * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, LoftAngle, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetLoftAngle() * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, BurnTime, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetBurnTime());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, TimeOfFlight, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetTimeOfFlight());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, InterceptTime, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetInterceptTime());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, InterceptPoint, 0, "WsfGeoPoint", "")
{
   double locWCS[3];
   aObjectPtr->GetInterceptPoint().GetLocationWCS(locWCS);
   WsfGeoPoint* pointPtr = new WsfGeoPoint(locWCS);
   aReturnVal.SetPointer(new UtScriptRef(pointPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, TargetImpactTime, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetTargetImpactTime());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, TargetImpactPoint, 0, "WsfGeoPoint", "")
{
   double locWCS[3];
   aObjectPtr->GetTargetImpactPoint().GetLocationWCS(locWCS);
   WsfGeoPoint* pointPtr = new WsfGeoPoint(locWCS);
   aReturnVal.SetPointer(new UtScriptRef(pointPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, EndTime, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetEndTime());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, EndPoint, 0, "WsfGeoPoint", "")
{
   double locWCS[3];
   aObjectPtr->GetEndPoint().GetLocationWCS(locWCS);
   WsfGeoPoint* pointPtr = new WsfGeoPoint(locWCS);
   aReturnVal.SetPointer(new UtScriptRef(pointPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// =================================================================================================
// Special functions for WsfScriptLaunchComputer.

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, SetInterceptTime, 1, "void", "double")
{
   aObjectPtr->SetInterceptTime(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, SetInterceptPoint, 1, "void", "WsfGeoPoint")
{
   WsfGeoPoint* pointPtr = (WsfGeoPoint*)aVarArgs[0].GetPointer()->GetAppObject();
   aObjectPtr->SetInterceptPoint(*pointPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, SetLaunchTime, 1, "void", "double")
{
   aObjectPtr->SetLaunchTime(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, SetTimeOfFlight, 1, "void", "double")
{
   aObjectPtr->SetTimeOfFlight(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, SetLoftAngle, 1, "void", "double")
{
   aObjectPtr->SetLoftAngle(aVarArgs[0].GetDouble());
}

// =================================================================================================
// Special functions for WsfBallisticMissileLaunchComputer.

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, ComputeEndPoint, 0, "void", "")
{
   WsfBallisticMissileLaunchComputer* computerPtr = dynamic_cast<WsfBallisticMissileLaunchComputer*>(aObjectPtr);
   if (computerPtr != nullptr)
   {
      computerPtr->ComputeEndPoint();
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, ComputeLaunchWindows, 0, "void", "")
{
   WsfBallisticMissileLaunchComputer* computerPtr = dynamic_cast<WsfBallisticMissileLaunchComputer*>(aObjectPtr);
   if (computerPtr != nullptr)
   {
      computerPtr->ComputeLaunchWindows();
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, LaunchWindowCount, 0, "int", "")
{
   int                                value       = 0;
   WsfBallisticMissileLaunchComputer* computerPtr = dynamic_cast<WsfBallisticMissileLaunchComputer*>(aObjectPtr);
   if (computerPtr != nullptr)
   {
      value = computerPtr->GetLaunchWindowCount();
   }
   aReturnVal.SetInt(value);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, FirstLaunchTime, 1, "double", "int")
{
   double                             value       = -1.0;
   WsfBallisticMissileLaunchComputer* computerPtr = dynamic_cast<WsfBallisticMissileLaunchComputer*>(aObjectPtr);
   if (computerPtr != nullptr)
   {
      value = computerPtr->GetFirstLaunchTime(aVarArgs[0].GetInt());
   }
   aReturnVal.SetDouble(value);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, FirstInterceptTime, 1, "double", "int")
{
   double                             value       = -1.0;
   WsfBallisticMissileLaunchComputer* computerPtr = dynamic_cast<WsfBallisticMissileLaunchComputer*>(aObjectPtr);
   if (computerPtr != nullptr)
   {
      value = computerPtr->GetFirstInterceptTime(aVarArgs[0].GetInt());
   }
   aReturnVal.SetDouble(value);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, FirstInterceptPoint, 1, "WsfGeoPoint", "int")
{
   double                             locWCS[3]   = {0.0, 0.0, 0.0};
   WsfBallisticMissileLaunchComputer* computerPtr = dynamic_cast<WsfBallisticMissileLaunchComputer*>(aObjectPtr);
   if (computerPtr != nullptr)
   {
      computerPtr->GetFirstInterceptLocationWCS(aVarArgs[0].GetInt(), locWCS);
   }
   WsfGeoPoint* pointPtr = new WsfGeoPoint(locWCS);
   aReturnVal.SetPointer(new UtScriptRef(pointPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, LastLaunchTime, 1, "double", "int")
{
   double                             value       = -1.0;
   WsfBallisticMissileLaunchComputer* computerPtr = dynamic_cast<WsfBallisticMissileLaunchComputer*>(aObjectPtr);
   if (computerPtr != nullptr)
   {
      value = computerPtr->GetLastLaunchTime(aVarArgs[0].GetInt());
   }
   aReturnVal.SetDouble(value);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, LastInterceptTime, 1, "double", "int")
{
   double                             value       = -1.0;
   WsfBallisticMissileLaunchComputer* computerPtr = dynamic_cast<WsfBallisticMissileLaunchComputer*>(aObjectPtr);
   if (computerPtr != nullptr)
   {
      value = computerPtr->GetLastInterceptTime(aVarArgs[0].GetInt());
   }
   aReturnVal.SetDouble(value);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, LastInterceptPoint, 1, "WsfGeoPoint", "int")
{
   double                             locWCS[3]   = {0.0, 0.0, 0.0};
   WsfBallisticMissileLaunchComputer* computerPtr = dynamic_cast<WsfBallisticMissileLaunchComputer*>(aObjectPtr);
   if (computerPtr != nullptr)
   {
      computerPtr->GetLastInterceptLocationWCS(aVarArgs[0].GetInt(), locWCS);
   }
   WsfGeoPoint* pointPtr = new WsfGeoPoint(locWCS);
   aReturnVal.SetPointer(new UtScriptRef(pointPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, LateralTargetOffset, 1, "double", "WsfTrack")
{
   double                             lateralOffset = 1.0E+30;
   WsfBallisticMissileLaunchComputer* computerPtr   = dynamic_cast<WsfBallisticMissileLaunchComputer*>(aObjectPtr);
   if (computerPtr != nullptr)
   {
      WsfTrack* trackPtr = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
      double    simTime  = WsfScriptContext::GetTIME_NOW(aContext);
      lateralOffset      = computerPtr->ComputeLateralTargetOffset(simTime, *trackPtr);
   }
   aReturnVal.SetDouble(lateralOffset);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, TargetApogeeTime, 0, "double", "")
{
   double                             value       = -1.0;
   WsfBallisticMissileLaunchComputer* computerPtr = dynamic_cast<WsfBallisticMissileLaunchComputer*>(aObjectPtr);
   if (computerPtr != nullptr)
   {
      value = computerPtr->GetTargetApogeeTime();
   }
   aReturnVal.SetDouble(value);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, TargetApogeePoint, 0, "WsfGeoPoint", "")
{
   double                             locWCS[3]   = {0.0, 0.0, 0.0};
   WsfBallisticMissileLaunchComputer* computerPtr = dynamic_cast<WsfBallisticMissileLaunchComputer*>(aObjectPtr);
   if (computerPtr != nullptr)
   {
      computerPtr->GetTargetApogeeLocationWCS(locWCS);
   }
   WsfGeoPoint* pointPtr = new WsfGeoPoint(locWCS);
   aReturnVal.SetPointer(new UtScriptRef(pointPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// Deprecated
UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, LastLaunchTime_OLD, 0, "double", "") // NO_DOC | DEPRECATED
{
   double                             value       = -1.0;
   WsfBallisticMissileLaunchComputer* computerPtr = dynamic_cast<WsfBallisticMissileLaunchComputer*>(aObjectPtr);
   if (computerPtr != nullptr)
   {
      value = computerPtr->GetLastLaunchTime(computerPtr->GetLaunchWindowCount() - 1);
   }
   aReturnVal.SetDouble(value);
}

// Deprecated
UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, LastInterceptTime_OLD, 0, "double", "") // NO_DOC
                                                                                                                 // | DEPRECATED
{
   double                             value       = -1.0;
   WsfBallisticMissileLaunchComputer* computerPtr = dynamic_cast<WsfBallisticMissileLaunchComputer*>(aObjectPtr);
   if (computerPtr != nullptr)
   {
      value = computerPtr->GetLastInterceptTime(computerPtr->GetLaunchWindowCount() - 1);
   }
   aReturnVal.SetDouble(value);
}

// Deprecated
UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, LastInterceptPoint_OLD, 0, "WsfGeoPoint", "") // NO_DOC | DEPRECATED
{
   double                             locWCS[3]   = {0.0, 0.0, 0.0};
   WsfBallisticMissileLaunchComputer* computerPtr = dynamic_cast<WsfBallisticMissileLaunchComputer*>(aObjectPtr);
   if (computerPtr != nullptr)
   {
      computerPtr->GetLastInterceptLocationWCS(computerPtr->GetLaunchWindowCount() - 1, locWCS);
   }
   WsfGeoPoint* pointPtr = new WsfGeoPoint(locWCS);
   aReturnVal.SetPointer(new UtScriptRef(pointPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// =================================================================================================
// Special functions for WsfAirToAirLaunchComputer.

// Array<double> returnData = LookupResult(WsfTrack aTrack);
UT_DEFINE_SCRIPT_METHOD(WsfScriptLaunchComputerClass, WsfLaunchComputer, LookupResult, 1, "Array<double>", "WsfTrack")
{
   WsfAirToAirLaunchComputer* computerPtr = dynamic_cast<WsfAirToAirLaunchComputer*>(aObjectPtr);

   // This return value example plagiarized from WsfScriptGroupClass.cpp
   std::vector<UtScriptData>* returnDataPtr = new std::vector<UtScriptData>();

   double Rmax    = -1.0;
   double RmaxTOF = -1.0;
   double Rne     = -1.0;
   double RneTOF  = -1.0;
   double Rmin    = -1.0;
   double RminTOF = -1.0;

   if (computerPtr != nullptr)
   {
      WsfTrack* trackPtr = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();

      bool isValid = computerPtr->LookupResult(*trackPtr, Rmax, RmaxTOF, Rne, RneTOF, Rmin, RminTOF);
      if (aObjectPtr->DebugEnabled())
      {
         if (isValid)
         {
            ut::log::debug() << "WsfScriptLaunchComputerClass - lookup is valid.";
         }
         else
         {
            // returnDataPtr->clear();
            ut::log::debug() << "WsfScriptLaunchComputerClass - lookup is NOT valid.";
         }
      }
   }

   returnDataPtr->emplace_back(Rmax);
   returnDataPtr->emplace_back(RmaxTOF);
   returnDataPtr->emplace_back(Rne);
   returnDataPtr->emplace_back(RneTOF);
   returnDataPtr->emplace_back(Rmin);
   returnDataPtr->emplace_back(RminTOF);

   // This return value example plagiarized from WsfScriptGroupClass.cpp
   aReturnVal.SetPointer(new UtScriptRef(returnDataPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}
