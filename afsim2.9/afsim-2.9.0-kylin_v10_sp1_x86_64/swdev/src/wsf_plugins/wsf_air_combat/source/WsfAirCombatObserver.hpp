// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFAIRCOMBATOBSERVER_HPP
#define WSFAIRCOMBATOBSERVER_HPP

#include "wsf_air_combat_export.h"

#include "UtCallback.hpp"
#include "WsfPlatform.hpp"
#include "WsfSA_Processor.hpp"
#include "WsfScriptSA_Processor.hpp"
#include "WsfSimulation.hpp"
#include "WsfSimulationExtension.hpp"

namespace WsfObserver
{
using AirCombatSendEngagementSummaryDataCallback =
   UtCallbackListN<void(double, const WsfPlatform&, const WsfSA_Processor&)>;
WSF_AIR_COMBAT_EXPORT AirCombatSendEngagementSummaryDataCallback&
AirCombatSendEngagementSummaryData(const WsfSimulation* aSimulationPtr);

using AirCombatSendFlightKinematicsDataCallback =
   UtCallbackListN<void(double, const WsfPlatform&, const WsfSA_Processor&)>;
WSF_AIR_COMBAT_EXPORT AirCombatSendFlightKinematicsDataCallback&
AirCombatSendFlightKinematicsData(const WsfSimulation* aSimulationPtr);

using AirCombatSendFuelDataCallback = UtCallbackListN<void(double, const WsfPlatform&, const WsfSA_Processor&)>;
WSF_AIR_COMBAT_EXPORT AirCombatSendFuelDataCallback& AirCombatSendFuelData(const WsfSimulation* aSimulationPtr);

using AirCombatSendNavDataCallback = UtCallbackListN<void(double, const WsfPlatform&, const WsfSA_Processor&)>;
WSF_AIR_COMBAT_EXPORT AirCombatSendNavDataCallback& AirCombatSendNavData(const WsfSimulation* aSimulationPtr);

using AirCombatSendFlightControlsDataCallback = UtCallbackListN<void(double, const WsfPlatform&, const WsfSA_Processor&)>;
WSF_AIR_COMBAT_EXPORT AirCombatSendFlightControlsDataCallback&
AirCombatSendFlightControlsData(const WsfSimulation* aSimulationPtr);

using AirCombatSendWeaponsDataCallback = UtCallbackListN<void(double, const WsfPlatform&, const WsfSA_Processor&)>;
WSF_AIR_COMBAT_EXPORT AirCombatSendWeaponsDataCallback& AirCombatSendWeaponsData(const WsfSimulation* aSimulationPtr);

using AirCombatSendTrackDataCallback = UtCallbackListN<void(double, const WsfPlatform&, const WsfSA_Processor&)>;
WSF_AIR_COMBAT_EXPORT AirCombatSendTrackDataCallback& AirCombatSendTrackData(const WsfSimulation* aSimulationPtr);

using AirCombatSendPerceivedAssetsDataCallback = UtCallbackListN<void(double, const WsfPlatform&, const WsfSA_Processor&)>;
WSF_AIR_COMBAT_EXPORT AirCombatSendPerceivedAssetsDataCallback&
AirCombatSendPerceivedAssetsData(const WsfSimulation* aSimulationPtr);

using AirCombatSendPerceivedItemsDataCallback = UtCallbackListN<void(double, const WsfPlatform&, const WsfSA_Processor&)>;
WSF_AIR_COMBAT_EXPORT AirCombatSendPerceivedItemsDataCallback&
AirCombatSendPerceivedItemsData(const WsfSimulation* aSimulationPtr);

using AirCombatSendPrioritizedThreatsAndTargetsDataCallback =
   UtCallbackListN<void(double, const WsfPlatform&, const WsfSA_Processor&)>;
WSF_AIR_COMBAT_EXPORT AirCombatSendPrioritizedThreatsAndTargetsDataCallback&
AirCombatSendPrioritizedThreatsAndTargetsData(const WsfSimulation* aSimulationPtr);

// TODO -- NOTE: These callbacks may be reinstated after more testing
// using AirCombatSendAssetsBogiesGroupsDataCallback = UtCallbackListN<void(double, const WsfPlatform&, const
// WsfSA_Processor&)>; WSF_AIR_COMBAT_EXPORT AirCombatSendAssetsBogiesGroupsDataCallback&
// AirCombatSendAssetsBogiesGroupsData(const WsfSimulation* aSimulationPtr); using AirCombatPlatformAddedCallback =
// UtCallbackListN<void(double, const WsfPlatform&)>; WSF_AIR_COMBAT_EXPORT AirCombatPlatformAddedCallback&
// AirCombatPlatformAdded(const WsfSimulation* aSimulationPtr);
} // namespace WsfObserver

class WSF_AIR_COMBAT_EXPORT WsfAirCombatObserver : public WsfSimulationExtension
{
public:
   WsfObserver::AirCombatSendEngagementSummaryDataCallback            AirCombatSendEngagementSummaryData;
   WsfObserver::AirCombatSendFlightKinematicsDataCallback             AirCombatSendFlightKinematicsData;
   WsfObserver::AirCombatSendFuelDataCallback                         AirCombatSendFuelData;
   WsfObserver::AirCombatSendNavDataCallback                          AirCombatSendNavData;
   WsfObserver::AirCombatSendFlightControlsDataCallback               AirCombatSendFlightControlsData;
   WsfObserver::AirCombatSendWeaponsDataCallback                      AirCombatSendWeaponsData;
   WsfObserver::AirCombatSendTrackDataCallback                        AirCombatSendTrackData;
   WsfObserver::AirCombatSendPerceivedAssetsDataCallback              AirCombatSendPerceivedAssetsData;
   WsfObserver::AirCombatSendPerceivedItemsDataCallback               AirCombatSendPerceivedItemsData;
   WsfObserver::AirCombatSendPrioritizedThreatsAndTargetsDataCallback AirCombatSendPrioritizedThreatsAndTargetsData;

   // TODO -- NOTE: These callbacks may be reinstated after more testing
   // WsfObserver::AirCombatSendAssetsBogiesGroupsDataCallback         AirCombatSendAssetsBogiesGroupsData;
   // WsfObserver::AirCombatPlatformAddedCallback                      AirCombatPlatformAdded;
};

#endif // !WSFAIRCOMBATOBSERVER_HPP
