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

#include "WsfScriptWeaponClass.hpp"

#include <vector>

#include "UtMemory.hpp"
#include "WsfEW_EA.hpp"
#include "WsfExplicitWeapon.hpp"
#include "WsfLaunchComputer.hpp"
#include "WsfPlatform.hpp"
#include "WsfProcessor.hpp"
#include "WsfRF_Jammer.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackManager.hpp"
#include "WsfWeapon.hpp"
#include "WsfWeaponTypes.hpp"
#include "script/WsfScriptContext.hpp"

WsfScriptWeaponClass::WsfScriptWeaponClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptArticulatedPartClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfWeapon");

   AddStaticMethod(ut::make_unique<IsA_TypeOf>());

   AddMethod(ut::make_unique<TurnOff>());
   AddMethod(ut::make_unique<TurnOn>());

   AddMethod(ut::make_unique<CueToTarget>());
   AddMethod(ut::make_unique<Fire_1>("Fire"));
   AddMethod(ut::make_unique<Fire_2>("Fire"));
   AddMethod(ut::make_unique<FireAtLocation>());
   AddMethod(ut::make_unique<FireWithWeaponId>()); // NO_DOC | HIDDEN
   AddMethod(ut::make_unique<FireSalvo>("FireSalvo"));
   AddMethod(ut::make_unique<AbortSalvo>());
   AddMethod(ut::make_unique<CeaseFire>());
   AddMethod(ut::make_unique<ActiveRequestCount>());
   AddMethod(ut::make_unique<MaximumRequestCount>());
   AddMethod(ut::make_unique<OffsetId>());
   AddMethod(ut::make_unique<SetOffsetId>());
   AddMethod(ut::make_unique<QuantityRemaining>());
   AddMethod(ut::make_unique<SetQuantityRemaining>());
   AddMethod(ut::make_unique<ReloadInventory>());
   AddMethod(ut::make_unique<TotalQuantityUsed>());
   AddMethod(ut::make_unique<TimeLastFired>());
   AddMethod(ut::make_unique<TimeSinceLastFired>());
   AddMethod(ut::make_unique<FiringInterval>());

   AddMethod(ut::make_unique<IsReloading>());

   AddMethod(ut::make_unique<TimeSinceWeaponLastFiredFor>());
   AddMethod(ut::make_unique<TimeSinceWeaponLastTerminatedFor>());
   AddMethod(ut::make_unique<WeaponsPendingFor>());
   AddMethod(ut::make_unique<WeaponsActiveFor>());
   AddMethod(ut::make_unique<RoundsCompleteFor>());
   AddMethod(ut::make_unique<RoundsFiredAt>());
   AddMethod(ut::make_unique<SalvosFiredAt>());
   AddMethod(ut::make_unique<ActiveWeaponPlatformsFor>());

   AddMethod(ut::make_unique<LaunchComputer>());
   AddMethod(ut::make_unique<CanIntercept_1>("CanIntercept"));
   AddMethod(ut::make_unique<CanIntercept_2>("CanIntercept"));
   AddMethod(ut::make_unique<TimeToIntercept_1>("TimeToIntercept"));
   AddMethod(ut::make_unique<TimeToIntercept_2>("TimeToIntercept"));

   AddMethod(ut::make_unique<ModeCount>());
   AddMethod(ut::make_unique<ModeName>());
   AddMethod(ut::make_unique<CurrentMode>());
   AddMethod(ut::make_unique<SelectMode>());
   // AddMethod(ut::make_unique<DeselectMode>());

   AddMethod(ut::make_unique<ActiveBeams>("ActiveBeams"));
   AddMethod(ut::make_unique<MaximumBeams>("MaximumBeams"));
   AddMethod(ut::make_unique<ActiveSpots>("ActiveSpots"));
   AddMethod(ut::make_unique<MaximumSpots>("MaximumSpots"));
   AddMethod(ut::make_unique<CanJam_1>("CanJam"));
   AddMethod(ut::make_unique<CanJam_2>("CanJam"));
   AddMethod(ut::make_unique<MaximumFrequency>("MaximumFrequency"));
   AddMethod(ut::make_unique<MinimumFrequency>("MinimumFrequency"));
   AddMethod(ut::make_unique<WithinFrequencyBand>("WithinFrequencyBand"));
   AddMethod(ut::make_unique<StartJamming_1>("StartJamming"));
   AddMethod(ut::make_unique<StartJamming_2>("StartJamming"));
   AddMethod(ut::make_unique<StartJamming_3>("StartJamming"));
   AddMethod(ut::make_unique<StartJamming_4>("StartJamming"));
   AddMethod(ut::make_unique<StartJamming_5>("StartJamming"));
   AddMethod(ut::make_unique<StartJamming_6>("StartJamming"));
   AddMethod(ut::make_unique<StartJamming_7>("StartJamming"));
   AddMethod(ut::make_unique<StartJamming_8>("StartJamming"));
   AddMethod(ut::make_unique<StartJammingTrack_1>("StartJamming"));
   AddMethod(ut::make_unique<StartJammingTrack_2>("StartJamming"));
   AddMethod(ut::make_unique<StopJamming_1>("StopJamming"));
   AddMethod(ut::make_unique<StopJamming_2>("StopJamming"));
   AddMethod(ut::make_unique<StopJamming_3>("StopJamming"));
   AddMethod(ut::make_unique<StopJamming_4>("StopJamming"));
   AddMethod(ut::make_unique<StopJammingTrack>("StopJamming"));

   AddMethod(ut::make_unique<SelectEA_Technique_1>("SelectEA_Technique"));
   AddMethod(ut::make_unique<SelectEA_Technique_2>("SelectEA_Technique"));
   AddMethod(ut::make_unique<SelectEA_Technique_3>("SelectEA_Technique"));
   AddMethod(ut::make_unique<SelectEA_Technique_4>("SelectEA_Technique"));
   AddMethod(ut::make_unique<DeselectEA_Technique_1>("DeselectEA_Technique"));
   AddMethod(ut::make_unique<DeselectEA_Technique_2>("DeselectEA_Technique"));
   AddMethod(ut::make_unique<DeselectEA_Technique_3>("DeselectEA_Technique"));
   AddMethod(ut::make_unique<DeselectEA_Technique_4>("DeselectEA_Technique"));

   AddMethod(ut::make_unique<SetEA_DeltaGainTechnique_1>("SetEA_DeltaGainTechnique"));
   AddMethod(ut::make_unique<SetEA_DeltaGainTechnique_2>("SetEA_DeltaGainTechnique"));
   AddMethod(ut::make_unique<SetEA_DeltaGainTechnique_3>("SetEA_DeltaGainTechnique"));
   AddMethod(ut::make_unique<SetEA_DeltaGainTechnique_4>("SetEA_DeltaGainTechnique"));
   AddMethod(ut::make_unique<SetEA_DeltaGainTechnique_5>("SetEA_DeltaGainTechnique"));

   //!@ Weapon Server Methods
   //! Methods initiating communication with the weapon server
   //@{
   AddMethod(ut::make_unique<AllocateTheWeapon>());
   AddMethod(ut::make_unique<State>());
   AddMethod(ut::make_unique<SetState>());
   //@}
}

#include "script/WsfScriptDefs.hpp"

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfScriptWeaponClass, IsA_TypeOf, 2, "bool", "string, string")
{
   bool       isA_TypeOf = false;
   WsfWeapon* objectPtr  = WsfWeaponTypes::Get(*SCENARIO).Find(aVarArgs[0].GetString());
   if (objectPtr != nullptr)
   {
      isA_TypeOf = objectPtr->IsA_TypeOf(aVarArgs[1].GetString());
   }
   aReturnVal.SetBool(isA_TypeOf);
}

UT_DEFINE_SCRIPT_METHOD_NOCHECK(WsfScriptWeaponClass, WsfWeapon, TurnOff, 0, "bool", "")
{
   bool ok = false;
   if (aObjectPtr != nullptr)
   {
      ok = SIMULATION->TurnPartOff(TIME_NOW, aObjectPtr);
   }
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD_NOCHECK(WsfScriptWeaponClass, WsfWeapon, TurnOn, 0, "bool", "")
{
   bool ok = false;
   if (aObjectPtr != nullptr)
   {
      ok = SIMULATION->TurnPartOn(TIME_NOW, aObjectPtr);
   }
   aReturnVal.SetBool(ok);
}

//! CueToTarget(WsfTrack aTrack);
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, CueToTarget, 1, "void", "WsfTrack")
{
   WsfTrack* trackPtr = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   aObjectPtr->CueToTarget(TIME_NOW, trackPtr);
}

//! bool ok = Fire();
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, Fire_1, 0, "bool", "")
{
   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   bool   ok      = aObjectPtr->Fire(simTime, WsfWeapon::FireTarget(), WsfWeapon::FireOptions());
   aReturnVal.SetBool(ok);
}

//! bool ok = Fire(WsfTrack aTrack)
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, Fire_2, 1, "bool", "WsfTrack")
{
   WsfTrack* trackPtr = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   bool      ok       = aObjectPtr->Fire(TIME_NOW, trackPtr, WsfWeapon::FireOptions());
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, FireAtLocation, 1, "bool", "WsfGeoPoint")
{
   // Create track based on the WsfGeoPoint
   auto targetLocation = aVarArgs[0].GetPointer()->GetAppObject<WsfGeoPoint>();
   WsfTrack     rawTrack;
   rawTrack.SetLocationLLA(targetLocation->GetLat(), targetLocation->GetLon(), targetLocation->GetAlt());
   double temp[] = {0, 0, 0};
   rawTrack.SetVelocityWCS(temp);
   WsfTrackId trackId = aObjectPtr->GetPlatform()->GetNextTrackId();
   rawTrack.Initialize(TIME_NOW, trackId, *SIMULATION);
   // Add a Track Report so the Track Manager owns the track and it will be cleaned up properly
   WsfLocalTrack* localTrack = aObjectPtr->GetPlatform()->GetTrackManager().AddTrackReport(TIME_NOW, rawTrack);

   // Fire at the track
   bool ok = aObjectPtr->Fire(TIME_NOW, localTrack, WsfWeapon::FireOptions());
   aReturnVal.SetBool(ok);
}

//! bool ok = Fire(WsfTrack aTrack)
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, FireWithWeaponId, 2, "bool", "WsfTrack, int") // NO_DOC | HIDDEN
{
   WsfWeapon::FireTarget  target = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   WsfWeapon::FireOptions settings(aVarArgs[1].GetInt());
   bool                   ok = aObjectPtr->Fire(TIME_NOW, target, settings);
   aReturnVal.SetBool(ok);
}

//! bool ok = FireSalvo(WsfTrack aTrack, int aQuantity)
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, FireSalvo, 2, "bool", "WsfTrack, int")
{
   WsfTrack*               trackPtr = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   WsfWeapon::SalvoOptions settings(aVarArgs[1].GetInt());
   bool                    ok = aObjectPtr->FireSalvo(TIME_NOW, trackPtr, settings);
   aReturnVal.SetBool(ok);
}

//! void AbortSalvo(WsfTrackId aTrackId)
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, AbortSalvo, 1, "void", "WsfTrackId")
{
   WsfTrackId* trackIdPtr = (WsfTrackId*)aVarArgs[0].GetPointer()->GetAppObject();
   double      simTime    = WsfScriptContext::GetTIME_NOW(aContext);
   aObjectPtr->AbortSalvo(simTime, *trackIdPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, CeaseFire, 0, "void", "")
{
   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   aObjectPtr->CeaseFire(simTime);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, ActiveRequestCount, 0, "int", "")
{
   int value = static_cast<int>(aObjectPtr->GetActiveRequestCount());
   aReturnVal.SetInt(value);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, MaximumRequestCount, 0, "int", "")
{
   int value = static_cast<int>(aObjectPtr->GetMaximumRequestCount());
   aReturnVal.SetInt(value);
}

UT_DEFINE_SCRIPT_METHOD_NOCHECK(WsfScriptWeaponClass, WsfWeapon, OffsetId, 0, "int", "")
{
   int value = 0;
   if (aObjectPtr != nullptr)
   {
      WsfExplicitWeapon* expWeaponPtr = dynamic_cast<WsfExplicitWeapon*>(aObjectPtr);
      if (expWeaponPtr != nullptr)
      {
         value = expWeaponPtr->GetOffsetId();
      }
   }
   aReturnVal.SetInt(value);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, SetOffsetId, 1, "void", "int")
{
   WsfExplicitWeapon* expWeaponPtr = dynamic_cast<WsfExplicitWeapon*>(aObjectPtr);
   if (expWeaponPtr != nullptr)
   {
      expWeaponPtr->SetOffsetId(aVarArgs[0].GetInt());
   }
}

UT_DEFINE_SCRIPT_METHOD_NOCHECK(WsfScriptWeaponClass, WsfWeapon, QuantityRemaining, 0, "double", "")
{
   double value = 0.0;
   if (aObjectPtr != nullptr)
   {
      value = aObjectPtr->GetQuantityRemaining();
   }
   aReturnVal.SetDouble(value);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, SetQuantityRemaining, 1, "void", "double")
{
   aObjectPtr->SetQuantityRemaining(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD_NOCHECK(WsfScriptWeaponClass, WsfWeapon, ReloadInventory, 0, "double", "")
{
   double value = 0.0;
   if (aObjectPtr != nullptr)
   {
      value = aObjectPtr->GetReloadInventory();
   }
   aReturnVal.SetDouble(value);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, TotalQuantityUsed, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetTotalQuantityUsed());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, TimeLastFired, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetTimeLastFired());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, TimeSinceLastFired, 0, "double", "")
{
   double value = TIME_NOW - aObjectPtr->GetTimeLastFired();
   aReturnVal.SetDouble(value);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, FiringInterval, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetFiringInterval());
}

//! bool isReloading = IsReloading()
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, IsReloading, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsReloading());
}

// Return the elapsed time since the last weapon firing for the specified task.
// The return value will be less than zero if no weapon has been fired.
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, TimeSinceWeaponLastFiredFor, 1, "double", "WsfTrackId")
{
   double            timeSince  = -1.0;
   const WsfTrackId* trackIdPtr = static_cast<const WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   double            timeFired  = aObjectPtr->TimeWeaponLastFiredFor(*trackIdPtr);
   if (timeFired >= 0.0)
   {
      timeSince = WsfScriptContext::GetTIME_NOW(aContext) - timeFired;
   }
   aReturnVal.SetDouble(timeSince);
}

// Return the elapsed time since the last weapon termination for the specified task.
// The return value will be less than zero if no weapon has been terminated.
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, TimeSinceWeaponLastTerminatedFor, 1, "double", "WsfTrackId")
{
   double            timeSince      = -1.0;
   const WsfTrackId* trackIdPtr     = static_cast<const WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   double            timeTerminated = aObjectPtr->TimeWeaponLastTerminatedFor(*trackIdPtr);
   if (timeTerminated >= 0.0)
   {
      timeSince = WsfScriptContext::GetTIME_NOW(aContext) - timeTerminated;
   }
   aReturnVal.SetDouble(timeSince);
}

//! Returned the number of weapons that are active for the specified task.
//! int count = WeaponsActiveFor(WsfTrackId aTrackId);
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, WeaponsPendingFor, 1, "int", "WsfTrackId")
{
   const WsfTrackId* trackIdPtr = static_cast<const WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   aReturnVal.SetInt(aObjectPtr->WeaponsPendingFor(*trackIdPtr));
}

//! Returned the number of weapons that are active for the specified task.
//! int count = WeaponsActiveFor(WsfTrackId aTrackId);
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, WeaponsActiveFor, 1, "int", "WsfTrackId")
{
   const WsfTrackId* trackIdPtr = static_cast<const WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   aReturnVal.SetInt(aObjectPtr->WeaponsActiveFor(*trackIdPtr));
}

//! Returned the number of rounds that have been fired for the specified task.
//! int count = RoundsCompleteFor(WsfTrackId aTrackId);
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, RoundsCompleteFor, 1, "int", "WsfTrackId")
{
   const WsfTrackId* trackIdPtr = static_cast<const WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   aReturnVal.SetInt(aObjectPtr->RoundsCompleteFor(*trackIdPtr));
}

//! Returned the number of rounds that have been fired for the specified task.
//! int count = RoundsFiredAt(WsfTrackId aTrackId);
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, RoundsFiredAt, 1, "int", "WsfTrackId")
{
   const WsfTrackId* trackIdPtr = static_cast<const WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   aReturnVal.SetInt(aObjectPtr->RoundsFiredAt(*trackIdPtr));
}

//! Returned the number of salvos that have been fired for the specified task.
//! int count = SalvosFiredAt(WsfTrackId aTrackId);
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, SalvosFiredAt, 1, "int", "WsfTrackId")
{
   const WsfTrackId* trackIdPtr = static_cast<const WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   aReturnVal.SetInt(aObjectPtr->SalvosFiredAt(*trackIdPtr));
}

//! Returned the number of salvos that have been fired for the specified task.
//! WsfPlatformList pList = ActiveWeaponPlatformsFor(WsfTrackId aTrackId);
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, ActiveWeaponPlatformsFor, 1, "WsfPlatformList", "WsfTrackId")
{
   const WsfTrackId*   trackIdPtr        = static_cast<const WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   std::vector<size_t> weaponPlatformIds = aObjectPtr->ActiveWeaponPlatformsFor(*trackIdPtr);

   std::vector<WsfPlatform*>* platformListPtr = new std::vector<WsfPlatform*>();
   for (std::vector<size_t>::size_type i = 0; i < weaponPlatformIds.size(); ++i)
   {
      WsfPlatform* platformPtr = SIMULATION->GetPlatformByIndex(weaponPlatformIds[i]);
      if (platformPtr != nullptr)
      {
         platformListPtr->push_back(platformPtr);
      }
   }
   aReturnVal.SetPointer(new UtScriptRef(platformListPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

//! WsfLaunchComputer computer = LaunchComputer();
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, LaunchComputer, 0, "WsfLaunchComputer", "")
{
   WsfLaunchComputer* computerPtr = aObjectPtr->GetLaunchComputer();
   aReturnVal.SetPointer(new UtScriptRef(computerPtr, aReturnClassPtr));
}

//! bool canIntercept = CanIntercept(WsfTrack aTrack)
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, CanIntercept_1, 1, "bool", "WsfTrack")
{
   bool      canIntercept = false;
   WsfTrack* trackPtr     = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   if (aObjectPtr->GetLaunchComputer() != nullptr)
   {
      double timeToIntercept = aObjectPtr->GetLaunchComputer()->EstimatedTimeToIntercept(TIME_NOW, *trackPtr, 0.0);
      if (timeToIntercept < WsfLaunchComputer::cFOREVER)
      {
         canIntercept = true;
      }
   }
   aReturnVal.SetBool(canIntercept);
}

//! bool canIntercept = CanIntercept(WsfTrack aTrack, double aLaunchDelayTime)
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, CanIntercept_2, 2, "bool", "WsfTrack, double")
{
   bool      canIntercept = false;
   WsfTrack* trackPtr     = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   if (aObjectPtr->GetLaunchComputer() != nullptr)
   {
      double launchDelayTime = aVarArgs[1].GetDouble();
      double timeToIntercept =
         aObjectPtr->GetLaunchComputer()->EstimatedTimeToIntercept(TIME_NOW, *trackPtr, launchDelayTime);
      if (timeToIntercept < WsfLaunchComputer::cFOREVER)
      {
         canIntercept = true;
      }
   }
   aReturnVal.SetBool(canIntercept);
}

//! double timeToIntercept = TimeToIntercept(WsfTrack aTrack)
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, TimeToIntercept_1, 1, "double", "WsfTrack")
{
   double    timeToIntercept = 1.0E+10;
   WsfTrack* trackPtr        = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   if (aObjectPtr->GetLaunchComputer() != nullptr)
   {
      timeToIntercept = aObjectPtr->GetLaunchComputer()->EstimatedTimeToIntercept(TIME_NOW, *trackPtr, 0.0);
   }
   aReturnVal.SetDouble(timeToIntercept);
}

//! double timeToIntercept = TimeToIntercept(WsfTrack aTrack, double aLaunchDelayTime)
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, TimeToIntercept_2, 2, "double", "WsfTrack, double")
{
   double    timeToIntercept = 1.0E+10;
   WsfTrack* trackPtr        = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   if (aObjectPtr->GetLaunchComputer() != nullptr)
   {
      double launchDelayTime = aVarArgs[1].GetDouble();
      timeToIntercept = aObjectPtr->GetLaunchComputer()->EstimatedTimeToIntercept(TIME_NOW, *trackPtr, launchDelayTime);
   }
   aReturnVal.SetDouble(timeToIntercept);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, ModeCount, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(aObjectPtr->GetModeCount()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, ModeName, 1, "string", "int")
{
   WsfStringId value = aObjectPtr->GetModeName(aVarArgs[0].GetInt());
   aReturnVal.SetString(value);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, CurrentMode, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetCurrentModeName().GetString());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, SelectMode, 1, "void", "string")
{
   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   aObjectPtr->SelectMode(simTime, WsfStringId(aVarArgs[0].GetString()));
}

// UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, DeselectMode, 1, "void", "string")
//{
//    double simTime = WsfScriptContext::GetTIME_NOW(aContext);
//    aObjectPtr->DeselectMode(simTime, aVarArgs[0].GetString());
// }

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, ActiveBeams, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(aObjectPtr->GetActiveBeamCount()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, MaximumBeams, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(aObjectPtr->GetMaximumBeamCount()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, ActiveSpots, 1, "int", "int")
{
   // Argument 0: beamNumber
   size_t beamNumber = static_cast<size_t>(aVarArgs[0].GetInt());

   aReturnVal.SetInt(static_cast<int>(aObjectPtr->GetActiveSpotCount(beamNumber)));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, MaximumSpots, 1, "int", "int")
{
   // Argument 0: beamNumber
   size_t beamNumber = static_cast<size_t>(aVarArgs[0].GetInt());

   aReturnVal.SetInt(static_cast<int>(aObjectPtr->GetMaximumSpotCount(beamNumber)));
}

//! double maxFreq = MaximumFrequency();
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, MaximumFrequency, 0, "double", "")
{
   double        maxFreq   = 0.0;
   WsfRF_Jammer* jammerPtr = (WsfRF_Jammer*)aObjectPtr;
   if (jammerPtr != nullptr)
   {
      maxFreq = jammerPtr->GetMaximumFrequency();
   }
   aReturnVal.SetDouble(maxFreq);
}

//! double minFreq = MinimumFrequency();
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, MinimumFrequency, 0, "double", "")
{
   double        minFreq   = 0.0;
   WsfRF_Jammer* jammerPtr = (WsfRF_Jammer*)aObjectPtr;
   if (jammerPtr != nullptr)
   {
      minFreq = jammerPtr->GetMinimumFrequency();
   }
   aReturnVal.SetDouble(minFreq);
}

//! bool withinBand = WithinFrequencyBand(double aFrequencyHertz);
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, WithinFrequencyBand, 1, "bool", "double")
{
   // Argument 0: frequency (Hertz)
   double frequency  = aVarArgs[0].GetDouble();
   bool   withinBand = false;

   // Do a frequency check; assume current mode
   WsfRF_Jammer* jammerPtr = (WsfRF_Jammer*)aObjectPtr;
   if (jammerPtr != nullptr)
   {
      if ((jammerPtr->GetMinimumFrequency() <= frequency) && (jammerPtr->GetMaximumFrequency() >= frequency))
      {
         withinBand = true;
      }
   }
   aReturnVal.SetBool(withinBand);
}

//! bool canJam = CanJam(double aFrequencyHertz);
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, CanJam_1, 1, "bool", "double")
{
   // Argument 0: frequency (Hertz)
   bool canJam(false);
   // Check for available spots
   if ((aObjectPtr->GetQuantityRemaining() > 0.0) || (aObjectPtr->GetMaximumRequestCount() == 1))
   {
      // Do a frequency check; assume current mode
      double        frequency = aVarArgs[0].GetDouble();
      WsfRF_Jammer* jammerPtr = (WsfRF_Jammer*)aObjectPtr;
      if (jammerPtr != nullptr)
      {
         if ((jammerPtr->GetMinimumFrequency() <= frequency) && (jammerPtr->GetMaximumFrequency() >= frequency))
         {
            canJam = true;
         }
      }
   }
   aReturnVal.SetBool(canJam);
}

//! bool canJam = CanJam(double aFrequencyHertz, string aTechniqueName);
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, CanJam_2, 2, "bool", "double, string")
{
   // Argument 0: frequency (Hertz)
   // Argument 1: Technique Name
   bool canJam(false);
   // Check for available spots
   if ((aObjectPtr->GetQuantityRemaining() > 0.0) || (aObjectPtr->GetMaximumRequestCount() == 1))
   {
      // Do a frequency check; assume current mode
      double        frequency   = aVarArgs[0].GetDouble();
      WsfStringId   techniqueId = aVarArgs[1].GetString();
      WsfRF_Jammer* jammerPtr   = (WsfRF_Jammer*)aObjectPtr;
      if (jammerPtr != nullptr)
      {
         WsfEW_EA_EP* eaPtr = WsfEW_EA::GetElectronicAttack(jammerPtr->GetMasterXmtr());
         if (((jammerPtr->GetMinimumFrequency() <= frequency) && (jammerPtr->GetMaximumFrequency() >= frequency)) &&
             (eaPtr != nullptr) && (eaPtr->TechniqueIsAvailable(techniqueId)))
         {
            canJam = true;
         }
      }
   }
   aReturnVal.SetBool(canJam);
}

//! bool startedJamming = StartJamming(double aFrequencyHertz, double aBandwidthHertz);
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, StartJamming_1, 2, "bool", "double, double")
{
   // Argument 0: frequency (Hertz)
   // Argument 1: bandwidth (Hertz)
   bool       startedJamming(false);
   double     frequency = aVarArgs[0].GetDouble();
   double     bandwith  = aVarArgs[1].GetDouble();
   WsfTrackId trackId(nullptr, 0);

   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   startedJamming = aObjectPtr->StartJamming(simTime, frequency, bandwith, 0, nullptr, trackId, 0);

   aReturnVal.SetBool(startedJamming);
}

//! bool startedJamming = StartJamming(double aFrequencyHertz, double aBandwidthHertz, string aTechniqueName);
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, StartJamming_2, 3, "bool", "double, double, string")
{
   // Argument 0: frequency (Hertz)
   // Argument 1: bandwidth (Hertz)
   // Argument 2: technique name
   bool        startedJamming(false);
   double      frequency   = aVarArgs[0].GetDouble();
   double      bandwith    = aVarArgs[1].GetDouble();
   WsfStringId techniqueId = aVarArgs[2].GetString();
   WsfTrackId  trackId(nullptr, 0);

   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   startedJamming = aObjectPtr->StartJamming(simTime, frequency, bandwith, 0, techniqueId, trackId, 0);

   aReturnVal.SetBool(startedJamming);
}

//! bool startedJamming = StartJamming(double aFrequencyHertz, double aBandwidthHertz, WsfTrack aTrack);
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, StartJamming_3, 3, "bool", "double, double, WsfTrack")
{
   // Argument 0: frequency (Hertz)
   // Argument 1: bandwidth (Hertz)
   // Argument 2: track to jam
   bool      startedJamming(false);
   double    frequency = aVarArgs[0].GetDouble();
   double    bandwith  = aVarArgs[1].GetDouble();
   WsfTrack* trackPtr  = (WsfTrack*)aVarArgs[2].GetPointer()->GetAppObject();

   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   startedJamming =
      aObjectPtr->StartJamming(simTime, frequency, bandwith, 0, nullptr, trackPtr->GetTrackId(), trackPtr->GetTargetIndex());

   aReturnVal.SetBool(startedJamming);
}

//! bool startedJamming = StartJamming(double aFrequencyHertz, double aBandwidthHertz,
//!                                  string aTechniqueName, WsfTrack aTrack);
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, StartJamming_4, 4, "bool", "double, double, string, WsfTrack")
{
   // Argument 0: frequency (Hertz)
   // Argument 1: bandwidth (Hertz)
   // Argument 2: technique name
   // Argument 3: track to jam
   bool        startedJamming(false);
   double      frequency   = aVarArgs[0].GetDouble();
   double      bandwith    = aVarArgs[1].GetDouble();
   WsfStringId techniqueId = aVarArgs[2].GetString();
   WsfTrack*   trackPtr    = (WsfTrack*)aVarArgs[3].GetPointer()->GetAppObject();

   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   startedJamming =
      aObjectPtr->StartJamming(simTime, frequency, bandwith, 0, techniqueId, trackPtr->GetTrackId(), trackPtr->GetTargetIndex());

   aReturnVal.SetBool(startedJamming);
}

//! bool startedJamming = StartJamming(double aFrequencyHertz, double aBandwidthHertz, int aBeamNumber);
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, StartJamming_5, 3, "bool", "double, double, int")
{
   // Argument 0: frequency (Hertz)
   // Argument 1: bandwidth (Hertz)
   // Argument 2: beamNumber
   bool         startedJamming(false);
   double       frequency  = aVarArgs[0].GetDouble();
   double       bandwith   = aVarArgs[1].GetDouble();
   unsigned int beamNumber = (unsigned int)aVarArgs[2].GetInt();
   WsfTrackId   trackId(nullptr, 0);

   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   startedJamming = aObjectPtr->StartJamming(simTime, frequency, bandwith, beamNumber, nullptr, trackId, 0);

   aReturnVal.SetBool(startedJamming);
}

//! bool startedJamming = StartJamming(double aFrequencyHertz, double aBandwidthHertz, int aBeamNumber, string aTechniqueName);
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, StartJamming_6, 4, "bool", "double, double, int, string")
{
   // Argument 0: frequency (Hertz)
   // Argument 1: bandwidth (Hertz)
   // Argument 2: beamNumber
   // Argument 3: technique name
   bool         startedJamming(false);
   double       frequency   = aVarArgs[0].GetDouble();
   double       bandwith    = aVarArgs[1].GetDouble();
   unsigned int beamNumber  = (unsigned int)aVarArgs[2].GetInt();
   WsfStringId  techniqueId = aVarArgs[3].GetString();
   WsfTrackId   trackId(nullptr, 0);

   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   startedJamming = aObjectPtr->StartJamming(simTime, frequency, bandwith, beamNumber, techniqueId, trackId, 0);

   aReturnVal.SetBool(startedJamming);
}

//! bool startedJamming = StartJamming(double aFrequencyHertz, double aBandwidthHertz, int aBeamNumber, WsfTrack aTrack);
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, StartJamming_7, 4, "bool", "double, double, int, WsfTrack")
{
   // Argument 0: frequency (Hertz)
   // Argument 1: bandwidth (Hertz)
   // Argument 2: beamNumber
   // Argument 3: track to jam
   bool         startedJamming(false);
   double       frequency  = aVarArgs[0].GetDouble();
   double       bandwith   = aVarArgs[1].GetDouble();
   unsigned int beamNumber = (unsigned int)aVarArgs[2].GetInt();
   WsfTrack*    trackPtr   = (WsfTrack*)aVarArgs[3].GetPointer()->GetAppObject();

   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   startedJamming = aObjectPtr->StartJamming(simTime,
                                             frequency,
                                             bandwith,
                                             beamNumber,
                                             nullptr,
                                             trackPtr->GetTrackId(),
                                             trackPtr->GetTargetIndex());

   aReturnVal.SetBool(startedJamming);
}

//! bool startedJamming = StartJamming(double aFrequencyHertz, double aBandwidthHertz, int aBeamNumber,
//!                                    string aTechniqueName, WsfTrack aTrack);
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, StartJamming_8, 5, "bool", "double, double, int, string, WsfTrack")
{
   // Argument 0: frequency (Hertz)
   // Argument 1: bandwidth (Hertz)
   // Argument 2: beamNumber
   // Argument 3: technique name
   // Argument 4: track to jam
   bool         startedJamming(false);
   double       frequency   = aVarArgs[0].GetDouble();
   double       bandwith    = aVarArgs[1].GetDouble();
   unsigned int beamNumber  = (unsigned int)aVarArgs[2].GetInt();
   WsfStringId  techniqueId = aVarArgs[3].GetString();
   WsfTrack*    trackPtr    = (WsfTrack*)aVarArgs[4].GetPointer()->GetAppObject();

   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   startedJamming = aObjectPtr->StartJamming(simTime,
                                             frequency,
                                             bandwith,
                                             beamNumber,
                                             techniqueId,
                                             trackPtr->GetTrackId(),
                                             trackPtr->GetTargetIndex());

   aReturnVal.SetBool(startedJamming);
}

//! bool startedJamming = StartJamming(WsfTrack aTrack);
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, StartJammingTrack_1, 1, "bool", "WsfTrack")
{
   // Argument 0: track to jam
   bool      startedJamming(false);
   WsfTrack* trackPtr = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();

   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   startedJamming = aObjectPtr->StartJammingTrack(simTime, nullptr, trackPtr);

   aReturnVal.SetBool(startedJamming);
}

//! bool startedJamming = StartJamming(string aTechniqueName, WsfTrack aTrack);
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, StartJammingTrack_2, 2, "bool", "string, WsfTrack")
{
   // Argument 0: technique name
   // Argument 1: track to jam
   bool        startedJamming(false);
   WsfStringId techniqueId = aVarArgs[0].GetString();
   WsfTrack*   trackPtr    = (WsfTrack*)aVarArgs[1].GetPointer()->GetAppObject();

   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   startedJamming = aObjectPtr->StartJammingTrack(simTime, techniqueId, trackPtr);

   aReturnVal.SetBool(startedJamming);
}

//! bool stoppedJamming = StopJamming(double aFrequencyHertz, double aBandwidthHertz);
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, StopJamming_1, 2, "bool", "double, double")
{
   // Argument 0: frequency (Hertz)
   // Argument 1: bandwidth (Hertz)
   bool       stoppedJamming(false);
   double     frequency = aVarArgs[0].GetDouble();
   double     bandwith  = aVarArgs[1].GetDouble();
   WsfTrackId trackId(nullptr, 0);

   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   stoppedJamming = aObjectPtr->StopJamming(simTime, frequency, bandwith, 0, trackId);

   aReturnVal.SetBool(stoppedJamming);
}

//! bool stoppedJamming = StopJamming(double aFrequencyHertz, double aBandwidthHertz, int aBeamNumber);
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, StopJamming_2, 3, "bool", "double, double, int")
{
   // Argument 0: frequency (Hertz)
   // Argument 1: bandwidth (Hertz)
   // Argument 2: beamNumber
   bool         stoppedJamming(false);
   double       frequency  = aVarArgs[0].GetDouble();
   double       bandwith   = aVarArgs[1].GetDouble();
   unsigned int beamNumber = (unsigned int)aVarArgs[2].GetInt();
   WsfTrackId   trackId(nullptr, 0);

   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   stoppedJamming = aObjectPtr->StopJamming(simTime, frequency, bandwith, beamNumber, trackId);

   aReturnVal.SetBool(stoppedJamming);
}

//! bool stoppedJamming = StopJamming(double aFrequencyHertz, double aBandwidthHertz, WsfTrackId aTrackId);
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, StopJamming_3, 3, "bool", "double, double, WsfTrackId")
{
   // Argument 0: frequency (Hertz)
   // Argument 1: bandwidth (Hertz)
   // Argument 2: Track Id
   bool        stoppedJamming(false);
   double      frequency  = aVarArgs[0].GetDouble();
   double      bandwith   = aVarArgs[1].GetDouble();
   WsfTrackId* trackIdPtr = static_cast<WsfTrackId*>(aVarArgs[2].GetPointer()->GetAppObject());

   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   stoppedJamming = aObjectPtr->StopJamming(simTime, frequency, bandwith, 0, *trackIdPtr);

   aReturnVal.SetBool(stoppedJamming);
}


//! bool stoppedJamming = StopJamming(double aFrequencyHertz, double aBandwidthHertz, int aBeamNumber,
//!                                   WsfTrackId aTrackId);
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, StopJamming_4, 4, "bool", "double, double, int, WsfTrackId")
{
   // Argument 0: frequency (Hertz)
   // Argument 1: bandwidth (Hertz)
   // Argument 2: beamNumber
   // Argument 3: Track Id
   bool         stoppedJamming(false);
   double       frequency  = aVarArgs[0].GetDouble();
   double       bandwith   = aVarArgs[1].GetDouble();
   unsigned int beamNumber = (unsigned int)aVarArgs[2].GetInt();
   WsfTrackId*  trackIdPtr = static_cast<WsfTrackId*>(aVarArgs[3].GetPointer()->GetAppObject());

   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   stoppedJamming = aObjectPtr->StopJamming(simTime, frequency, bandwith, beamNumber, *trackIdPtr);

   aReturnVal.SetBool(stoppedJamming);
}

//! bool stoppedJamming = StopJamming(WsfTrackId aTrackId);
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, StopJammingTrack, 1, "bool", "WsfTrackId")
{
   // Argument 0: Track Id
   bool        stoppedJamming(false);
   WsfTrackId* trackIdPtr = static_cast<WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());

   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   stoppedJamming = aObjectPtr->StopJamming(simTime, 0.0, 0.0, 0, *trackIdPtr);

   aReturnVal.SetBool(stoppedJamming);
}

//! bool selectedEA_Technique = SelectEA_Technique(string aTechniqueName, double aFrequencyHertz,
//!                                                double aBandwidthHertz);
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, SelectEA_Technique_1, 3, "bool", "string, double, double")
{
   // Argument 0: technique name
   // Argument 1: frequency (Hertz)
   // Argument 2: bandwidth (Hertz)
   bool        selectedEA_Technique(false);
   WsfStringId techniqueId = aVarArgs[0].GetString();
   double      frequency   = aVarArgs[1].GetDouble();
   double      bandwith    = aVarArgs[2].GetDouble();
   WsfTrackId  trackId(nullptr, 0);

   double simTime       = WsfScriptContext::GetTIME_NOW(aContext);
   selectedEA_Technique = aObjectPtr->SelectEA_Technique(simTime, frequency, bandwith, 0, techniqueId, trackId);

   aReturnVal.SetBool(selectedEA_Technique);
}

//! bool selectedEA_Technique = SelectEA_Technique(string aTechniqueName, double aFrequencyHertz,
//!                                                double aBandwidthHertz, int aBeamNumber, WsfTrack aTrack);
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, SelectEA_Technique_2, 4, "bool", "string, double, double, WsfTrack")
{
   // Argument 0: technique name
   // Argument 1: frequency (Hertz)
   // Argument 2: bandwidth (Hertz)
   // Argument 3: track
   bool         selectedEA_Technique(false);
   WsfStringId  techniqueId = aVarArgs[0].GetString();
   double       frequency   = aVarArgs[1].GetDouble();
   double       bandwith    = aVarArgs[2].GetDouble();
   unsigned int beamNumber  = 0;
   WsfTrack*    trackPtr    = (WsfTrack*)aVarArgs[3].GetPointer()->GetAppObject();

   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   selectedEA_Technique =
      aObjectPtr->SelectEA_Technique(simTime, frequency, bandwith, beamNumber, techniqueId, trackPtr->GetTrackId());

   aReturnVal.SetBool(selectedEA_Technique);
}

//! bool selectedEA_Technique = SelectEA_Technique(string aTechniqueName, double aFrequencyHertz,
//!                                                double aBandwidthHertz, int aBeamNumber);
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, SelectEA_Technique_3, 4, "bool", "string, double, double, int")
{
   // Argument 0: technique name
   // Argument 1: frequency (Hertz)
   // Argument 2: bandwidth (Hertz)
   // Argument 3: beamNumber
   bool         selectedEA_Technique(false);
   WsfStringId  techniqueId = aVarArgs[0].GetString();
   double       frequency   = aVarArgs[1].GetDouble();
   double       bandwith    = aVarArgs[2].GetDouble();
   unsigned int beamNumber  = (unsigned int)aVarArgs[3].GetInt();
   WsfTrackId   trackId(nullptr, 0);

   double simTime       = WsfScriptContext::GetTIME_NOW(aContext);
   selectedEA_Technique = aObjectPtr->SelectEA_Technique(simTime, frequency, bandwith, beamNumber, techniqueId, trackId);

   aReturnVal.SetBool(selectedEA_Technique);
}

//! bool selectedEA_Technique = SelectEA_Technique(string aTechniqueName, double aFrequencyHertz,
//!                                                 double aBandwidthHertz, int aBeamNumber, WsfTrack aTrack);
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, SelectEA_Technique_4, 5, "bool", " string, double, double, int,WsfTrack")
{
   // Argument 0: technique name
   // Argument 1: frequency (Hertz)
   // Argument 2: bandwidth (Hertz)
   // Argument 3: beamNumber
   // Argument 4: track
   bool         selectedEA_Technique(false);
   WsfStringId  techniqueId = aVarArgs[0].GetString();
   double       frequency   = aVarArgs[1].GetDouble();
   double       bandwith    = aVarArgs[2].GetDouble();
   unsigned int beamNumber  = (unsigned int)aVarArgs[3].GetInt();
   WsfTrack*    trackPtr    = (WsfTrack*)aVarArgs[4].GetPointer()->GetAppObject();

   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   selectedEA_Technique =
      aObjectPtr->SelectEA_Technique(simTime, frequency, bandwith, beamNumber, techniqueId, trackPtr->GetTrackId());

   aReturnVal.SetBool(selectedEA_Technique);
}

//! bool deselectedEA_Technique = DeselectEA_Technique(string aTechniqueName, double aFrequencyHertz,
//!                                                    double aBandwidthHertz);
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, DeselectEA_Technique_1, 3, "bool", "string, double, double")
{
   // Argument 0: technique name
   // Argument 1: frequency (Hertz)
   // Argument 2: bandwidth (Hertz)
   bool        deselectedEA_Technique(false);
   WsfStringId techniqueId = aVarArgs[0].GetString();
   double      frequency   = aVarArgs[1].GetDouble();
   double      bandwith    = aVarArgs[2].GetDouble();
   WsfTrackId  trackId(nullptr, 0);

   double simTime         = WsfScriptContext::GetTIME_NOW(aContext);
   deselectedEA_Technique = aObjectPtr->DeselectEA_Technique(simTime, frequency, bandwith, 0, techniqueId, trackId);

   aReturnVal.SetBool(deselectedEA_Technique);
}

//! bool deselectedEA_Technique = DeselectEA_Technique(string aTechniqueName, double aFrequencyHertz,
//!                                                    double aBandwidthHertz, WsfTrack aTrack);
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, DeselectEA_Technique_2, 4, "bool", " string, double, double, WsfTrack")
{
   // Argument 0: technique name
   // Argument 1: frequency (Hertz)
   // Argument 2: bandwidth (Hertz)
   // Argument 3: track
   bool         deselectedEA_Technique(false);
   WsfStringId  techniqueId = aVarArgs[0].GetString();
   double       frequency   = aVarArgs[1].GetDouble();
   double       bandwith    = aVarArgs[2].GetDouble();
   unsigned int beamNumber  = 0;
   WsfTrack*    trackPtr    = (WsfTrack*)aVarArgs[3].GetPointer()->GetAppObject();

   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   deselectedEA_Technique =
      aObjectPtr->DeselectEA_Technique(simTime, frequency, bandwith, beamNumber, techniqueId, trackPtr->GetTrackId());

   aReturnVal.SetBool(deselectedEA_Technique);
}

//! bool deselectedEA_Technique = DeselectEA_Technique(string aTechniqueName, double aFrequencyHertz,
//!                                                    double aBandwidthHertz, int aBeamNumber);
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, DeselectEA_Technique_3, 4, "bool", "string, double, double, int")
{
   // Argument 0: technique name
   // Argument 1: frequency (Hertz)
   // Argument 2: bandwidth (Hertz)
   // Argument 3: beamNumber
   bool         deselectedEA_Technique(false);
   WsfStringId  techniqueId = aVarArgs[0].GetString();
   double       frequency   = aVarArgs[1].GetDouble();
   double       bandwith    = aVarArgs[2].GetDouble();
   unsigned int beamNumber  = (unsigned int)aVarArgs[3].GetInt();
   WsfTrackId   trackId(nullptr, 0);

   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   deselectedEA_Technique =
      aObjectPtr->DeselectEA_Technique(simTime, frequency, bandwith, beamNumber, techniqueId, trackId);
   aReturnVal.SetBool(deselectedEA_Technique);
}

//! bool deselectedEA_Technique = DeselectEA_Technique(string aTechniqueName, double aFrequencyHertz,
//!                                                    double aBandwidthHertz, int aBeamNumber, WsfTrack aTrack);
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, DeselectEA_Technique_4, 5, "bool", "string, double, double, int, WsfTrack")
{
   // Argument 0: technique name
   // Argument 1: frequency (Hertz)
   // Argument 2: bandwidth (Hertz)
   // Argument 3: beamNumber
   // Argument 4: track
   bool         deselectedEA_Technique(false);
   WsfStringId  techniqueId = aVarArgs[0].GetString();
   double       frequency   = aVarArgs[1].GetDouble();
   double       bandwith    = aVarArgs[2].GetDouble();
   unsigned int beamNumber  = (unsigned int)aVarArgs[3].GetInt();
   WsfTrack*    trackPtr    = (WsfTrack*)aVarArgs[4].GetPointer()->GetAppObject();

   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   deselectedEA_Technique =
      aObjectPtr->DeselectEA_Technique(simTime, frequency, bandwith, beamNumber, techniqueId, trackPtr->GetTrackId());

   aReturnVal.SetBool(deselectedEA_Technique);
}

//! bool ea_ModulationTechniqueSet = SetEA_DeltaGainTechnique(string aTechniqueName, bool aDefaultOn,
//!                                                           string aEffectName, double aJammingDeltaGain_dB,
//!                                                           string aSystemTypeName, string aSystemFunctionName,
//!                                                           double aFrequencyHertz, double aBandwidthHertz);
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass,
                        WsfWeapon,
                        SetEA_DeltaGainTechnique_1,
                        8,
                        "bool",
                        "string, bool, string, double, string, string, double, double")
{
   // Argument 0: technique name
   // Argument 1: if technique is on by default
   // Argument 2: effect name
   // Argument 3: jamming delta gain (dB)
   // Argument 4: system type name this effect can effect
   // Argument 5: system function name this effect can effect
   // Argument 6: frequency (Hertz)
   // Argument 7: bandwidth (Hertz)
   bool         ea_ModulationTechniqueSet(false);
   WsfStringId  techniqueId         = aVarArgs[0].GetString();
   bool         defaultOn           = aVarArgs[1].GetBool();
   WsfStringId  effectId            = aVarArgs[2].GetString();
   double       jammingDeltaGain_dB = aVarArgs[3].GetDouble();
   WsfStringId  systemTypeId        = aVarArgs[4].GetString();
   WsfStringId  systemFunctionId    = aVarArgs[5].GetString();
   double       frequency           = aVarArgs[6].GetDouble();
   double       bandwith            = aVarArgs[7].GetDouble();
   unsigned int beamNumber          = 0;
   WsfTrackId   trackId             = WsfTrackId(nullptr, 0);

   double simTime            = WsfScriptContext::GetTIME_NOW(aContext);
   ea_ModulationTechniqueSet = aObjectPtr->SetEA_DeltaGainTechnique(simTime,
                                                                    techniqueId,
                                                                    defaultOn,
                                                                    effectId,
                                                                    jammingDeltaGain_dB,
                                                                    systemTypeId,
                                                                    systemFunctionId,
                                                                    frequency,
                                                                    bandwith,
                                                                    beamNumber,
                                                                    trackId);

   aReturnVal.SetBool(ea_ModulationTechniqueSet);
}

//! bool ea_ModulationTechniqueSet = SetEA_DeltaGainTechnique(string aTechniqueName, bool aDefaultOn,
//!                                                           string aEffectName, double aJammingDeltaGain_dB,
//!                                                           string aSystemTypeName,  string aSystemFunctionName,
//!                                                           double aFrequencyHertz, double aBandwidthHertz,
//!                                                           WsfTrack aTrack);
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass,
                        WsfWeapon,
                        SetEA_DeltaGainTechnique_2,
                        9,
                        "bool",
                        "string, bool, string, double, string, string, double, double, WsfTrack")
{
   // Argument 0: technique name
   // Argument 1: if technique is on by default
   // Argument 2: effect name
   // Argument 3: jamming delta gain (dB)
   // Argument 4: system type name this effect can effect
   // Argument 5: system function name this effect can effect
   // Argument 6: frequency (Hertz)
   // Argument 7: bandwidth (Hertz)
   // Argument 8: track
   bool         ea_ModulationTechniqueSet(false);
   WsfStringId  techniqueId         = aVarArgs[0].GetString();
   bool         defaultOn           = aVarArgs[1].GetBool();
   WsfStringId  effectId            = aVarArgs[2].GetString();
   double       jammingDeltaGain_dB = aVarArgs[3].GetDouble();
   WsfStringId  systemTypeId        = aVarArgs[4].GetString();
   WsfStringId  systemFunctionId    = aVarArgs[5].GetString();
   double       frequency           = aVarArgs[6].GetDouble();
   double       bandwith            = aVarArgs[7].GetDouble();
   unsigned int beamNumber          = 0;
   WsfTrack*    trackPtr            = (WsfTrack*)aVarArgs[8].GetPointer()->GetAppObject();

   double simTime            = WsfScriptContext::GetTIME_NOW(aContext);
   ea_ModulationTechniqueSet = aObjectPtr->SetEA_DeltaGainTechnique(simTime,
                                                                    techniqueId,
                                                                    defaultOn,
                                                                    effectId,
                                                                    jammingDeltaGain_dB,
                                                                    systemTypeId,
                                                                    systemFunctionId,
                                                                    frequency,
                                                                    bandwith,
                                                                    beamNumber,
                                                                    trackPtr->GetTrackId());
   aReturnVal.SetBool(ea_ModulationTechniqueSet);
}

//! bool ea_ModulationTechniqueSet = SetEA_DeltaGainTechnique(string aTechniqueName, bool aDefaultOn,
//!                                                           string aEffectName, double aJammingDeltaGain_dB,
//!                                                           string aSystemTypeName, string aSystemFunctionName,
//!                                                           double aFrequencyHertz, double aBandwidthHertz,
//!                                                           int aBeamNumber);
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass,
                        WsfWeapon,
                        SetEA_DeltaGainTechnique_3,
                        9,
                        "bool",
                        "string, bool, string, double, string, string, double, double, int")
{
   // Argument 0: technique name
   // Argument 1: if technique is on by default
   // Argument 2: effect name
   // Argument 3: jamming delta gain (dB)
   // Argument 4: system type name this effect can effect
   // Argument 5: system function name this effect can effect
   // Argument 6: frequency (Hertz)
   // Argument 7: bandwidth (Hertz)
   // Argument 8: beamNumber
   bool         ea_ModulationTechniqueSet(false);
   WsfStringId  techniqueId         = aVarArgs[0].GetString();
   bool         defaultOn           = aVarArgs[1].GetBool();
   WsfStringId  effectId            = aVarArgs[2].GetString();
   double       jammingDeltaGain_dB = aVarArgs[3].GetDouble();
   WsfStringId  systemTypeId        = aVarArgs[4].GetString();
   WsfStringId  systemFunctionId    = aVarArgs[5].GetString();
   double       frequency           = aVarArgs[6].GetDouble();
   double       bandwith            = aVarArgs[7].GetDouble();
   unsigned int beamNumber          = (unsigned int)aVarArgs[8].GetInt();
   WsfTrackId   trackId             = WsfTrackId(nullptr, 0);

   double simTime            = WsfScriptContext::GetTIME_NOW(aContext);
   ea_ModulationTechniqueSet = aObjectPtr->SetEA_DeltaGainTechnique(simTime,
                                                                    techniqueId,
                                                                    defaultOn,
                                                                    effectId,
                                                                    jammingDeltaGain_dB,
                                                                    systemTypeId,
                                                                    systemFunctionId,
                                                                    frequency,
                                                                    bandwith,
                                                                    beamNumber,
                                                                    trackId);

   aReturnVal.SetBool(ea_ModulationTechniqueSet);
}

//! bool ea_ModulationTechniqueSet = SetEA_DeltaGainTechnique(string aTechniqueName, bool aDefaultOn,
//!                                                           string aEffectName, double aJammingDeltaGain_dB,
//!                                                           string aSystemTypeName, string aSystemFunctionName,
//!                                                           double aFrequencyHertz, double aBandwidthHertz,
//!                                                           int aBeamNumber, WsfTrack aTrack);
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass,
                        WsfWeapon,
                        SetEA_DeltaGainTechnique_4,
                        10,
                        "bool",
                        "string, bool, string, double, string, string, double, double, int, WsfTrack")
{
   // Argument 0: technique name
   // Argument 1: if technique is on by default
   // Argument 2: effect name
   // Argument 3: jamming delta gain (dB)
   // Argument 4: system type name this effect can effect
   // Argument 5: system function name this effect can effect
   // Argument 6: frequency (Hertz)
   // Argument 7: bandwidth (Hertz)
   // Argument 8: beamNumber
   // Argument 9: track
   bool         ea_ModulationTechniqueSet(false);
   WsfStringId  techniqueId         = aVarArgs[0].GetString();
   bool         defaultOn           = aVarArgs[1].GetBool();
   WsfStringId  effectId            = aVarArgs[2].GetString();
   double       jammingDeltaGain_dB = aVarArgs[3].GetDouble();
   WsfStringId  systemTypeId        = aVarArgs[4].GetString();
   WsfStringId  systemFunctionId    = aVarArgs[5].GetString();
   double       frequency           = aVarArgs[6].GetDouble();
   double       bandwith            = aVarArgs[7].GetDouble();
   unsigned int beamNumber          = (unsigned int)aVarArgs[8].GetInt();
   WsfTrack*    trackPtr            = (WsfTrack*)aVarArgs[9].GetPointer()->GetAppObject();

   double simTime            = WsfScriptContext::GetTIME_NOW(aContext);
   ea_ModulationTechniqueSet = aObjectPtr->SetEA_DeltaGainTechnique(simTime,
                                                                    techniqueId,
                                                                    defaultOn,
                                                                    effectId,
                                                                    jammingDeltaGain_dB,
                                                                    systemTypeId,
                                                                    systemFunctionId,
                                                                    frequency,
                                                                    bandwith,
                                                                    beamNumber,
                                                                    trackPtr->GetTrackId());

   aReturnVal.SetBool(ea_ModulationTechniqueSet);
}


//! bool ea_ModulationTechniqueSet = SetEA_DeltaGainTechnique(string aTechniqueName, bool aDefaultOn,
//!                                                           string aEffectName, double aJammingDeltaGain_dB,
//!                                                           string aSystemTypeName, string aSystemFunctionName);
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass,
                        WsfWeapon,
                        SetEA_DeltaGainTechnique_5,
                        6,
                        "bool",
                        "string, bool, string, double, string, string")
{
   // Argument 0: technique name
   // Argument 1: if technique is on by default
   // Argument 2: effect name
   // Argument 3: jamming delta gain (dB)
   // Argument 4: system type name this effect can effect
   // Argument 5: system function name this effect can effect
   bool        ea_ModulationTechniqueSet(false);
   WsfStringId techniqueId         = aVarArgs[0].GetString();
   bool        defaultOn           = aVarArgs[1].GetBool();
   WsfStringId effectId            = aVarArgs[2].GetString();
   double      jammingDeltaGain_dB = aVarArgs[3].GetDouble();
   WsfStringId systemTypeId        = aVarArgs[4].GetString();
   WsfStringId systemFunctionId    = aVarArgs[5].GetString();

   double simTime            = WsfScriptContext::GetTIME_NOW(aContext);
   ea_ModulationTechniqueSet = aObjectPtr->SetEA_DeltaGainTechnique(simTime,
                                                                    techniqueId,
                                                                    defaultOn,
                                                                    effectId,
                                                                    jammingDeltaGain_dB,
                                                                    systemTypeId,
                                                                    systemFunctionId);

   aReturnVal.SetBool(ea_ModulationTechniqueSet);
}

//! void SetState(int aState)
UT_DEFINE_SCRIPT_METHOD_NOCHECK(WsfScriptWeaponClass, WsfWeapon, SetState, 1, "int", "int")
{
   int                    istate = aVarArgs[0].GetInt();
   WsfWeapon::WeaponState state  = (WsfWeapon::WeaponState)istate;
   if (aObjectPtr != nullptr)
   {
      aObjectPtr->SetState(state);
   }
}

//! Weapon Server Weapon Allocation Scripts

//! int state = State()
UT_DEFINE_SCRIPT_METHOD_NOCHECK(WsfScriptWeaponClass, WsfWeapon, State, 0, "int", "")
{
   int value = 0;
   if (aObjectPtr != nullptr)
   {
      value = (int)aObjectPtr->GetState();
   }
   aReturnVal.SetInt(value);
}

//! bool ok = AllocateTheWeapon(WsfTrack aTrack, int aWpnId)
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponClass, WsfWeapon, AllocateTheWeapon, 2, "bool", "WsfTrack, int")
{
   WsfTrack* trackPtr = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   double    simTime  = WsfScriptContext::GetTIME_NOW(aContext);
   bool      ok       = aObjectPtr->AllocateTheWeapon(simTime, trackPtr, aVarArgs[1].GetInt());
   aReturnVal.SetBool(ok);
}
