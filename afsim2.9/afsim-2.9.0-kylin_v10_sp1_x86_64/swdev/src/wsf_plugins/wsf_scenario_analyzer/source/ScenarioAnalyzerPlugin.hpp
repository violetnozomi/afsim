// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/*******************************************************************************
*
*   File: ScenarioAnalyzerPlugin.hpp
*
*   Classification: UNCLASSIFIED
*
*   Details:
*
*   Contract Number:
*   Contractor Name: Radiance Technologies
*   Address:         350 Wynn Drive
*                    Huntsville, AL 35805
*
*   Abstract:
*
*******************************************************************************/

#ifndef SCENARIO_ANALYZER_PLUGIN_HPP
#define SCENARIO_ANALYZER_PLUGIN_HPP

#include "wsf_scenario_analyzer_lib_export.h"
#include "ScenarioAnalyzerUtilities.hpp"

#include <string>

class WsfApplication;
class WsfPlatform;
class WsfSimulation;
class WsfWeaponsManagerAI;

// Plugin interface
WSF_SCENARIO_ANALYZER_LIB_EXPORT void ScenarioAnalyzerRegisterScriptTypes(WsfApplication& aApplication);

// Session Notes
WSF_SCENARIO_ANALYZER_LIB_EXPORT void notifyOfPlatformsNotPresentInSimulation(WsfSimulation& sim);

// Core Checks
WSF_SCENARIO_ANALYZER_LIB_EXPORT Messages checkCommanderInDeclaredCommandChain(WsfSimulation& sim);
WSF_SCENARIO_ANALYZER_LIB_EXPORT Messages checkCommInternallyLinked(WsfSimulation& sim);
WSF_SCENARIO_ANALYZER_LIB_EXPORT Messages checkDeclaredCommandChainHasStructure(WsfSimulation& sim);
WSF_SCENARIO_ANALYZER_LIB_EXPORT Messages checkLargeScenarioHasCommandChain(WsfSimulation& sim);
WSF_SCENARIO_ANALYZER_LIB_EXPORT Messages checkPlatformHasMeaningfulLocation(WsfSimulation& sim);
WSF_SCENARIO_ANALYZER_LIB_EXPORT Messages checkPlatformHasRequiredSignatures(WsfSimulation& sim);
WSF_SCENARIO_ANALYZER_LIB_EXPORT Messages checkPlatformInCommandChainHasComm(WsfSimulation& sim);
WSF_SCENARIO_ANALYZER_LIB_EXPORT Messages checkScriptProcessorHasUpdateInterval(WsfSimulation& sim);
WSF_SCENARIO_ANALYZER_LIB_EXPORT Messages checkSensorInternallyLinked(WsfSimulation& sim);
WSF_SCENARIO_ANALYZER_LIB_EXPORT Messages checkSensorInternallyLinkedToTrackProcessor(WsfSimulation& sim);
WSF_SCENARIO_ANALYZER_LIB_EXPORT Messages checkSensorOn(WsfSimulation& sim);
WSF_SCENARIO_ANALYZER_LIB_EXPORT Messages checkSignaturesDetectableByEnemySensor(WsfSimulation& sim);
WSF_SCENARIO_ANALYZER_LIB_EXPORT Messages checkTrackProcessorHasPurgeInterval(WsfSimulation& sim);
WSF_SCENARIO_ANALYZER_LIB_EXPORT Messages checkPurgeIntervalLongEnoughToMaintainTrack(WsfSimulation& sim);
WSF_SCENARIO_ANALYZER_LIB_EXPORT Messages checkPurgeIntervalLongEnoughToEstablishTrack(WsfSimulation& sim);
WSF_SCENARIO_ANALYZER_LIB_EXPORT Messages checkTrackProcessorsDontReportFusedTracksToEachOther(WsfSimulation& sim);
WSF_SCENARIO_ANALYZER_LIB_EXPORT Messages checkUserConfiguredSpeedsWithinMoverCapabilities(WsfSimulation& sim);
WSF_SCENARIO_ANALYZER_LIB_EXPORT Messages checkWeaponsNonzeroQuantity(WsfSimulation& sim);

#endif