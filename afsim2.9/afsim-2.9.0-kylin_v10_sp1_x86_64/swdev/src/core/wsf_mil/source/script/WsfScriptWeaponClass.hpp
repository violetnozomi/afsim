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

#ifndef WSFSCRIPTWEAPONCLASS_HPP
#define WSFSCRIPTWEAPONCLASS_HPP

#include "wsf_mil_export.h"

#include "script/WsfScriptArticulatedPartClass.hpp"

//! Define script methods for WsfWeapon.
class WSF_MIL_EXPORT WsfScriptWeaponClass : public WsfScriptArticulatedPartClass
{
public:
   WsfScriptWeaponClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(IsA_TypeOf); // static IsA_TypeOf("derived-type", "base-type")

   UT_DECLARE_SCRIPT_METHOD(TurnOff);
   UT_DECLARE_SCRIPT_METHOD(TurnOn);

   UT_DECLARE_SCRIPT_METHOD(CueToTarget);
   UT_DECLARE_SCRIPT_METHOD(Fire_1);
   UT_DECLARE_SCRIPT_METHOD(Fire_2);
   UT_DECLARE_SCRIPT_METHOD(FireAtLocation);
   UT_DECLARE_SCRIPT_METHOD(FireWithWeaponId); // NO_DOC | HIDDEN
   UT_DECLARE_SCRIPT_METHOD(FireSalvo);
   UT_DECLARE_SCRIPT_METHOD(AbortSalvo);
   UT_DECLARE_SCRIPT_METHOD(CeaseFire);
   UT_DECLARE_SCRIPT_METHOD(ActiveRequestCount);
   UT_DECLARE_SCRIPT_METHOD(MaximumRequestCount);
   UT_DECLARE_SCRIPT_METHOD(OffsetId);
   UT_DECLARE_SCRIPT_METHOD(SetOffsetId);
   UT_DECLARE_SCRIPT_METHOD(QuantityRemaining);
   UT_DECLARE_SCRIPT_METHOD(SetQuantityRemaining);
   UT_DECLARE_SCRIPT_METHOD(ReloadInventory);
   UT_DECLARE_SCRIPT_METHOD(TotalQuantityUsed);
   UT_DECLARE_SCRIPT_METHOD(TimeSinceLastFired);
   UT_DECLARE_SCRIPT_METHOD(TimeLastFired);
   UT_DECLARE_SCRIPT_METHOD(FiringInterval);

   UT_DECLARE_SCRIPT_METHOD(IsReloading);

   UT_DECLARE_SCRIPT_METHOD(TimeSinceWeaponLastFiredFor);
   UT_DECLARE_SCRIPT_METHOD(TimeSinceWeaponLastTerminatedFor);
   UT_DECLARE_SCRIPT_METHOD(WeaponsPendingFor);
   UT_DECLARE_SCRIPT_METHOD(WeaponsActiveFor);
   UT_DECLARE_SCRIPT_METHOD(RoundsCompleteFor);
   UT_DECLARE_SCRIPT_METHOD(RoundsFiredAt);
   UT_DECLARE_SCRIPT_METHOD(SalvosFiredAt);
   UT_DECLARE_SCRIPT_METHOD(ActiveWeaponPlatformsFor);

   UT_DECLARE_SCRIPT_METHOD(LaunchComputer);
   UT_DECLARE_SCRIPT_METHOD(CanIntercept_1);
   UT_DECLARE_SCRIPT_METHOD(CanIntercept_2);
   UT_DECLARE_SCRIPT_METHOD(TimeToIntercept_1);
   UT_DECLARE_SCRIPT_METHOD(TimeToIntercept_2);

   UT_DECLARE_SCRIPT_METHOD(ModeCount);
   UT_DECLARE_SCRIPT_METHOD(ModeName);
   UT_DECLARE_SCRIPT_METHOD(CurrentMode);
   UT_DECLARE_SCRIPT_METHOD(SelectMode);
   // UT_DECLARE_SCRIPT_METHOD(DeselectMode);

   UT_DECLARE_SCRIPT_METHOD(ActiveBeams);
   UT_DECLARE_SCRIPT_METHOD(MaximumBeams);
   UT_DECLARE_SCRIPT_METHOD(ActiveSpots);
   UT_DECLARE_SCRIPT_METHOD(MaximumSpots);
   UT_DECLARE_SCRIPT_METHOD(CanJam_1);
   UT_DECLARE_SCRIPT_METHOD(CanJam_2);
   UT_DECLARE_SCRIPT_METHOD(MaximumFrequency);
   UT_DECLARE_SCRIPT_METHOD(MinimumFrequency);
   UT_DECLARE_SCRIPT_METHOD(WithinFrequencyBand);
   UT_DECLARE_SCRIPT_METHOD(StartJamming_1);
   UT_DECLARE_SCRIPT_METHOD(StartJamming_2);
   UT_DECLARE_SCRIPT_METHOD(StartJamming_3);
   UT_DECLARE_SCRIPT_METHOD(StartJamming_4);
   UT_DECLARE_SCRIPT_METHOD(StartJamming_5);
   UT_DECLARE_SCRIPT_METHOD(StartJamming_6);
   UT_DECLARE_SCRIPT_METHOD(StartJamming_7);
   UT_DECLARE_SCRIPT_METHOD(StartJamming_8);
   UT_DECLARE_SCRIPT_METHOD(StartJammingTrack_1);
   UT_DECLARE_SCRIPT_METHOD(StartJammingTrack_2);
   UT_DECLARE_SCRIPT_METHOD(StopJamming_1);
   UT_DECLARE_SCRIPT_METHOD(StopJamming_2);
   UT_DECLARE_SCRIPT_METHOD(StopJamming_3);
   UT_DECLARE_SCRIPT_METHOD(StopJamming_4);
   UT_DECLARE_SCRIPT_METHOD(StopJamming_5);
   UT_DECLARE_SCRIPT_METHOD(StopJammingTrack);

   UT_DECLARE_SCRIPT_METHOD(SelectEA_Technique_1);
   UT_DECLARE_SCRIPT_METHOD(SelectEA_Technique_2);
   UT_DECLARE_SCRIPT_METHOD(SelectEA_Technique_3);
   UT_DECLARE_SCRIPT_METHOD(SelectEA_Technique_4);
   UT_DECLARE_SCRIPT_METHOD(DeselectEA_Technique_1);
   UT_DECLARE_SCRIPT_METHOD(DeselectEA_Technique_2);
   UT_DECLARE_SCRIPT_METHOD(DeselectEA_Technique_3);
   UT_DECLARE_SCRIPT_METHOD(DeselectEA_Technique_4);

   UT_DECLARE_SCRIPT_METHOD(SetEA_DeltaGainTechnique_1);
   UT_DECLARE_SCRIPT_METHOD(SetEA_DeltaGainTechnique_2);
   UT_DECLARE_SCRIPT_METHOD(SetEA_DeltaGainTechnique_3);
   UT_DECLARE_SCRIPT_METHOD(SetEA_DeltaGainTechnique_4);
   UT_DECLARE_SCRIPT_METHOD(SetEA_DeltaGainTechnique_5);
   //! Weapon Server Methods
   //@{
   //!  Methods initiating communication with the weapon server
   UT_DECLARE_SCRIPT_METHOD(AllocateTheWeapon);
   UT_DECLARE_SCRIPT_METHOD(State);
   UT_DECLARE_SCRIPT_METHOD(SetState);
   //@}
};

#endif
