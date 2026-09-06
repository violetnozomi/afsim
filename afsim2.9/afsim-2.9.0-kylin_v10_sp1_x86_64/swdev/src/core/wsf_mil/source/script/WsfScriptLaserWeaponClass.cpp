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

#include "WsfScriptLaserWeaponClass.hpp"

#include "UtMemory.hpp"
#include "WsfAttributeContainer.hpp"
#include "WsfLaserWeapon.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "WsfTrack.hpp"
#include "WsfWeapon.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptDefs.hpp"
#include "script/WsfScriptManager.hpp"

WsfScriptLaserWeaponClass::WsfScriptLaserWeaponClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptDirectedEnergyWeaponClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfLaserWeapon");

   AddMethod(ut::make_unique<Fire_2>("Fire"));
   AddMethod(ut::make_unique<CanLase>());
   AddMethod(ut::make_unique<TargetOccluded>());
   AddMethod(ut::make_unique<TestLasing1>("TestLasing"));
   AddMethod(ut::make_unique<TestLasing2>("TestLasing"));
   AddMethod(ut::make_unique<TestLasing3>("TestLasing"));
   AddMethod(ut::make_unique<DurationEnergy_1>("DurationEnergy"));
   AddMethod(ut::make_unique<DurationFluence_1>("DurationFluence"));
   AddMethod(ut::make_unique<DurationEnergy_2>("DurationEnergy"));
   AddMethod(ut::make_unique<DurationFluence_2>("DurationFluence"));
   AddMethod(ut::make_unique<EdgeRadius>());
   AddMethod(ut::make_unique<EnergyDensity>());
   AddMethod(ut::make_unique<PeakFluence>());
   AddMethod(ut::make_unique<Power>());
   AddMethod(ut::make_unique<IncidenceAngle>());
   AddMethod(ut::make_unique<PeakIrradiance>());
   AddMethod(ut::make_unique<EdgeIrradiance>());
   AddMethod(ut::make_unique<AverageIrradiance>());
   AddMethod(ut::make_unique<SpotRadius>());
   AddMethod(ut::make_unique<WindowStrehl>());
   AddMethod(ut::make_unique<AeroStrehl>());
   AddMethod(ut::make_unique<SetWindowStrehl>());
   AddMethod(ut::make_unique<SetAeroStrehl>());
   AddMethod(ut::make_unique<SystemStrehl>());
   AddMethod(ut::make_unique<SystemTransmission>());
   AddMethod(ut::make_unique<SpotSize>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaserWeaponClass, WsfLaserWeapon, Fire_2, 2, "bool", "WsfTrack, string")
{
   WsfTrack*   trackPtr     = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   std::string targetOffset = aVarArgs[1].GetString();
   double      simTime      = TIME_NOW;
   // Set target offset aux data in track
   trackPtr->GetAuxData().Assign("TARGET_OFFSET", targetOffset);
   bool success = aObjectPtr->Fire(simTime, trackPtr, WsfWeapon::FireOptions());
   aReturnVal   = success;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaserWeaponClass, WsfLaserWeapon, TargetOccluded, 2, "bool", "string, string")
{
   WsfHighEnergyLaser* helPtr     = aObjectPtr->GetHEL();
   std::string         targetName = aVarArgs[0].GetString();
   std::string         offsetName = aVarArgs[1].GetString();
   double              simTime    = TIME_NOW;
   WsfPlatform*        targetPtr  = SIMULATION->GetPlatformByName(targetName);
   bool                occluded   = false;
   if (targetPtr != nullptr)
   {
      helPtr->UpdateEngagementGeometry(simTime, targetPtr->GetIndex(), offsetName, false);
      occluded = helPtr->TargetOccluded();
   }
   aReturnVal.SetBool(occluded);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaserWeaponClass, WsfLaserWeapon, CanLase, 0, "bool", "")
{
   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   bool   canLase = aObjectPtr->CanLase(simTime);
   aReturnVal     = canLase;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaserWeaponClass, WsfLaserWeapon, TestLasing1, 2, "bool", "double, WsfTrack")
{
   double    duration  = aVarArgs[0].GetDouble();
   WsfTrack* trackPtr  = (WsfTrack*)aVarArgs[1].GetPointer()->GetAppObject();
   bool      testValid = aObjectPtr->GetHEL()->TestLasing(duration, trackPtr->GetTargetIndex());
   aReturnVal          = testValid;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaserWeaponClass, WsfLaserWeapon, TestLasing2, 3, "bool", "double, WsfGeoPoint, WsfGeoPoint")
{
   double       duration     = aVarArgs[0].GetDouble();
   WsfGeoPoint* sourceLocPtr = (WsfGeoPoint*)aVarArgs[1].GetPointer()->GetAppObject();
   WsfGeoPoint* targetLocPtr = (WsfGeoPoint*)aVarArgs[2].GetPointer()->GetAppObject();
   bool         testValid    = aObjectPtr->GetHEL()->TestLasing(duration, *sourceLocPtr, *targetLocPtr);
   aReturnVal                = testValid;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaserWeaponClass, WsfLaserWeapon, TestLasing3, 3, "bool", "double, int, string")
{
   double      duration    = aVarArgs[0].GetDouble();
   int         targetIndex = aVarArgs[1].GetInt();
   std::string offset      = aVarArgs[2].GetString();
   bool        success     = false;
   if (SIMULATION->PlatformExists(targetIndex))
   {
      success = aObjectPtr->GetHEL()->TestLasing(duration, targetIndex, offset);
   }
   aReturnVal = success;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaserWeaponClass, WsfLaserWeapon, DurationEnergy_1, 3, "double", "double, WsfGeoPoint, WsfGeoPoint")
{
   double       energy       = aVarArgs[0].GetDouble();
   WsfGeoPoint* sourceLocPtr = (WsfGeoPoint*)aVarArgs[1].GetPointer()->GetAppObject();
   WsfGeoPoint* targetLocPtr = (WsfGeoPoint*)aVarArgs[2].GetPointer()->GetAppObject();
   double       duration     = aObjectPtr->GetHEL()->RequiredDurationEnergy(energy, *sourceLocPtr, *targetLocPtr);
   aReturnVal                = duration;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaserWeaponClass, WsfLaserWeapon, DurationFluence_1, 3, "double", "double, WsfGeoPoint, WsfGeoPoint")
{
   double       energyDensity = aVarArgs[0].GetDouble();
   WsfGeoPoint* sourceLocPtr  = (WsfGeoPoint*)aVarArgs[1].GetPointer()->GetAppObject();
   WsfGeoPoint* targetLocPtr  = (WsfGeoPoint*)aVarArgs[2].GetPointer()->GetAppObject();
   double       duration = aObjectPtr->GetHEL()->RequiredDurationFluence(energyDensity, *sourceLocPtr, *targetLocPtr);
   aReturnVal            = duration;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaserWeaponClass, WsfLaserWeapon, DurationEnergy_2, 3, "double", "double, int, string")
{
   double      energy      = aVarArgs[0].GetDouble();
   int         targetIndex = aVarArgs[1].GetInt();
   std::string offset      = aVarArgs[2].GetString();
   double      duration    = 1.0e+10;
   if (SIMULATION->PlatformExists(targetIndex))
   {
      duration = aObjectPtr->GetHEL()->RequiredDurationEnergy(energy, targetIndex, offset);
   }
   aReturnVal = duration;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaserWeaponClass, WsfLaserWeapon, DurationFluence_2, 3, "double", "double, int, string")
{
   double      energyDensity = aVarArgs[0].GetDouble();
   int         targetIndex   = aVarArgs[1].GetInt();
   std::string offset        = aVarArgs[2].GetString();
   double      duration      = 1.0e+10;
   if (SIMULATION->PlatformExists(targetIndex))
   {
      duration = aObjectPtr->GetHEL()->RequiredDurationFluence(energyDensity, targetIndex, offset);
   }
   aReturnVal = duration;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaserWeaponClass, WsfLaserWeapon, Energy, 0, "double", "")
{
   double energy = aObjectPtr->GetHEL()->GetEnergy();
   aReturnVal    = energy;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaserWeaponClass, WsfLaserWeapon, EdgeRadius, 0, "double", "")
{
   double edgeRadius = aObjectPtr->GetHEL()->GetEdgeRadius();
   aReturnVal        = edgeRadius;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaserWeaponClass, WsfLaserWeapon, EnergyDensity, 0, "double", "")
{
   double energyDensity = aObjectPtr->GetHEL()->GetEnergyDensity();
   aReturnVal           = energyDensity;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaserWeaponClass, WsfLaserWeapon, Power, 0, "double", "")
{
   double enclosedPower = aObjectPtr->GetHEL()->GetAveragePower();
   aReturnVal           = enclosedPower;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaserWeaponClass, WsfLaserWeapon, PeakFluence, 0, "double", "")
{
   double peak = aObjectPtr->GetHEL()->GetPeakFluence();
   aReturnVal  = peak;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaserWeaponClass, WsfLaserWeapon, IncidenceAngle, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetHEL()->GetIncidenceAngle() * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaserWeaponClass, WsfLaserWeapon, PeakIrradiance, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetHEL()->GetPeakIrradiance());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaserWeaponClass, WsfLaserWeapon, EdgeIrradiance, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetHEL()->GetEdgeIrradiance());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaserWeaponClass, WsfLaserWeapon, AverageIrradiance, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetHEL()->GetAverageIrradiance());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaserWeaponClass, WsfLaserWeapon, SpotRadius, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetHEL()->GetSpotRadius());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaserWeaponClass, WsfLaserWeapon, WindowStrehl, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetHEL()->GetWindowStrehl());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaserWeaponClass, WsfLaserWeapon, AeroStrehl, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetHEL()->GetAeroStrehl());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaserWeaponClass, WsfLaserWeapon, SystemStrehl, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetHEL()->GetSystemStrehl());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaserWeaponClass, WsfLaserWeapon, SetWindowStrehl, 1, "void", "double")
{
   double windowStrehl = aVarArgs[0].GetDouble();
   aObjectPtr->GetHEL()->SetWindowStrehl(windowStrehl);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaserWeaponClass, WsfLaserWeapon, SetAeroStrehl, 1, "void", "double")
{
   double aeroStrehl = aVarArgs[0].GetDouble();
   aObjectPtr->GetHEL()->SetAeroStrehl(aeroStrehl);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaserWeaponClass, WsfLaserWeapon, SystemTransmission, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetHEL()->GetSystemTransmission());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLaserWeaponClass, WsfLaserWeapon, SpotSize, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetHEL()->GetSpotRadius());
}
