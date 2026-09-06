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

#include "WsfAirCombatObserver.hpp"

#define WSF_AIR_COMBAT_OBSERVER_CALLBACK_DEFINE(EVENT)                                                        \
   WsfObserver::EVENT##Callback& WsfObserver::EVENT(const WsfSimulation* aSimulationPtr)                      \
   {                                                                                                          \
      return static_cast<WsfAirCombatObserver*>(aSimulationPtr->FindExtension("air_combat_observer"))->EVENT; \
   }

WSF_AIR_COMBAT_OBSERVER_CALLBACK_DEFINE(AirCombatSendEngagementSummaryData)
WSF_AIR_COMBAT_OBSERVER_CALLBACK_DEFINE(AirCombatSendFlightKinematicsData)
WSF_AIR_COMBAT_OBSERVER_CALLBACK_DEFINE(AirCombatSendFuelData)
WSF_AIR_COMBAT_OBSERVER_CALLBACK_DEFINE(AirCombatSendNavData)
WSF_AIR_COMBAT_OBSERVER_CALLBACK_DEFINE(AirCombatSendFlightControlsData)
WSF_AIR_COMBAT_OBSERVER_CALLBACK_DEFINE(AirCombatSendWeaponsData)
WSF_AIR_COMBAT_OBSERVER_CALLBACK_DEFINE(AirCombatSendTrackData)
WSF_AIR_COMBAT_OBSERVER_CALLBACK_DEFINE(AirCombatSendPerceivedAssetsData)
WSF_AIR_COMBAT_OBSERVER_CALLBACK_DEFINE(AirCombatSendPerceivedItemsData)
WSF_AIR_COMBAT_OBSERVER_CALLBACK_DEFINE(AirCombatSendPrioritizedThreatsAndTargetsData)
