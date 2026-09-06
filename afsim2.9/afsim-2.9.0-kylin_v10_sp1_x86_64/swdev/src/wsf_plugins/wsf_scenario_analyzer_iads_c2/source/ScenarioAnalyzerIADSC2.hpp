// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef SCENARIO_ANALYZER_IADS_C2_HPP
#define SCENARIO_ANALYZER_IADS_C2_HPP

#include "ScenarioAnalyzerUtilities.hpp"
#include "wsf_scenario_analyzer_iads_c2_export.h"

class WsfPlatform;
class WsfSimulation;

// Plugin interface
WSF_SCENARIO_ANALYZER_IADS_C2_EXPORT void ScenarioAnalyzerIADSC2RegisterScriptTypes(WsfApplication* aApplication);

// IADS C2 Checks
WSF_SCENARIO_ANALYZER_IADS_C2_EXPORT Messages checkAssetManagersHaveUpdateInterval(WsfSimulation& sim);
WSF_SCENARIO_ANALYZER_IADS_C2_EXPORT Messages checkPlatformWithAssetManagerHasDisseminateC2Manager(WsfSimulation& sim);
WSF_SCENARIO_ANALYZER_IADS_C2_EXPORT Messages checkBattleManagerConnectedToSubordinateSensorsManagerByC2CapablePlatforms(WsfSimulation& sim);
WSF_SCENARIO_ANALYZER_IADS_C2_EXPORT Messages checkBattleManagerConnectedToSubordinateWeaponsManagerByC2CapablePlatforms(WsfSimulation& sim);
WSF_SCENARIO_ANALYZER_IADS_C2_EXPORT Messages checkBattleManagerReachableBySubordinateSensors(WsfSimulation& sim);
WSF_SCENARIO_ANALYZER_IADS_C2_EXPORT Messages checkBattleManagersDontConflict(WsfSimulation& sim);
WSF_SCENARIO_ANALYZER_IADS_C2_EXPORT Messages checkMaxAquisitionTimeLongEnoughForSensorsToFormTracks(WsfSimulation& sim);
WSF_SCENARIO_ANALYZER_IADS_C2_EXPORT Messages checkPlatformWithBattleManagerIsC2Capable(WsfSimulation& sim);
WSF_SCENARIO_ANALYZER_IADS_C2_EXPORT Messages checkPlatformWithDisseminateC2ManagerHasInternalCommLinks(WsfSimulation& sim);
WSF_SCENARIO_ANALYZER_IADS_C2_EXPORT Messages checkPlatformWithSensorsManagerConnectedToBattleManagerWithCommitAuthority(WsfSimulation& sim);
WSF_SCENARIO_ANALYZER_IADS_C2_EXPORT Messages checkPlatformWithSensorsManagerConnectedToTAROrTTR(WsfSimulation& sim);
WSF_SCENARIO_ANALYZER_IADS_C2_EXPORT Messages checkPlatformWithSensorsManagerIsC2Capable(WsfSimulation& sim);
WSF_SCENARIO_ANALYZER_IADS_C2_EXPORT Messages checkPlatformWithTAROrTTRConnectedToSensorsManager(WsfSimulation& sim);
WSF_SCENARIO_ANALYZER_IADS_C2_EXPORT Messages checkPlatformWithWeaponHasWeaponsManager(WsfSimulation& sim);
WSF_SCENARIO_ANALYZER_IADS_C2_EXPORT Messages checkPlatformWithWeaponsManagerConnectedToBattleManagerWithCommitAuthority(WsfSimulation& sim);
WSF_SCENARIO_ANALYZER_IADS_C2_EXPORT Messages checkPlatformWithWeaponsManagerHasAccessToRequiredSensor(WsfSimulation& sim);
WSF_SCENARIO_ANALYZER_IADS_C2_EXPORT Messages checkPlatformWithWeaponsManagerHasWeapon(WsfSimulation& sim);
WSF_SCENARIO_ANALYZER_IADS_C2_EXPORT Messages checkPlatformWithWeaponsManagerIsC2Capable(WsfSimulation& sim);
WSF_SCENARIO_ANALYZER_IADS_C2_EXPORT Messages checkSensorsManagersMustNotConflict(WsfSimulation& sim);
WSF_SCENARIO_ANALYZER_IADS_C2_EXPORT Messages checkTTRWithFOVSensorsManagerDoesNotManipulateOnOff(WsfSimulation& sim);
WSF_SCENARIO_ANALYZER_IADS_C2_EXPORT Messages checkTTRWithFOVSensorsManagerHasAuxDataFields(WsfSimulation& sim);
WSF_SCENARIO_ANALYZER_IADS_C2_EXPORT Messages checkTTRWithFOVSensorsManagerHasOneBeamPerMode(WsfSimulation& sim);
WSF_SCENARIO_ANALYZER_IADS_C2_EXPORT Messages checkTTRWithFOVSensorsManagerUsesDefaultSensorScheduler(WsfSimulation& sim);
WSF_SCENARIO_ANALYZER_IADS_C2_EXPORT Messages checkWeaponsOnWMAIPlatformHavRequiredAuxData(WsfSimulation& sim);
WSF_SCENARIO_ANALYZER_IADS_C2_EXPORT Messages checkWMAIPlatformDetectsEnemyTTRs(WsfSimulation& sim);
WSF_SCENARIO_ANALYZER_IADS_C2_EXPORT Messages checkWMAIPlatformHasRequiredRWR(WsfSimulation& sim);

#endif