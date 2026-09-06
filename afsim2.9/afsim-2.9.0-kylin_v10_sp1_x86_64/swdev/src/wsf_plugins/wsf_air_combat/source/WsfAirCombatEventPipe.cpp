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

#include "WsfAirCombatEventPipe.hpp"

#include <fstream>

#include "UtSphericalEarth.hpp"
#include "WsfAirCombatEventPipeClasses.hpp"
#include "WsfAirCombatEventPipeClassesRegister.hpp"
#include "WsfAirCombatEventPipeSchema.hpp"
#include "WsfAirCombatObserver.hpp"
#include "WsfAirMover.hpp"
#include "WsfBrawlerMover.hpp"
#include "WsfEventPipe.hpp"
#include "WsfExplicitWeapon.hpp"
#include "WsfGuidedMover.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfP6DOF_Mover.hpp"
#include "WsfSA_Assess.hpp"
#include "WsfSA_Group.hpp"
#include "WsfSA_Perceive.hpp"
#include "WsfSixDOF_Mover.hpp"
#include "WsfTrackManager.hpp"

// Event pipe constructor. It assigns the internal simulation pointer to the simulation referenced by
// aEventPipeInterfacePtr, assigns its internal interface pointer to aEventPipeInterfacePtr, and sets
// cAIRCOMBAT to its event ID from aEventPipeInterfacePtr.
wsf::AirCombat::EventPipe::EventPipe(WsfEventPipeInterface* aEventPipeInterfacePtr)
   : mSimulationPtr(&aEventPipeInterfacePtr->GetSimulation())
   , mEventPipePtr(aEventPipeInterfacePtr)
{
   cAIRCOMBAT = aEventPipeInterfacePtr->GetEventId("AIRCOMBAT");
}

// This registers the AIRCOMBAT option with aEventPipeExtension, adds its schema, adds EventPipe::RegisterMessages
// to aEventPipeExtension's callback list.
void wsf::AirCombat::EventPipe::RegisterEvents(WsfEventPipeExtension& aEventPipeExtension)
{
   aEventPipeExtension.AddSchema(cWsfAirCombatEventPipe_SCHEMA);
   aEventPipeExtension.RegisterEventOption("AIRCOMBAT", false);
   aEventPipeExtension.AddCallback(aEventPipeExtension.RegisterExtensionMessages.Connect(&EventPipe::RegisterMessages));
}

// Calls a function called UtPack_register_all_wsf_air_combat_events_types. That function appears to be
// auto-generated. It seems to add a couple struct definitions to aSerializePtr.
void wsf::AirCombat::EventPipe::RegisterMessages(UtPackSerializer* aSerializePtr)
{
   WsfEventPipe::UtPack_register_all_wsf_air_combat_events_types(*aSerializePtr);
}

// This should be called whenever an event pipe option is changed. It adds this plugin's observer callbacks to
// the EventPipe's internal callback list if the AIRCOMBAT option was enabled, or removes them otherwise.
void wsf::AirCombat::EventPipe::UpdateSubscriptions(const WsfEventPipeOptions& aNewCombinedOptions,
                                                    const WsfEventPipeOptions& aPreviousCombinedOptions)
{
   if (aNewCombinedOptions.IsOptionEnabled(cAIRCOMBAT))
   {
      // Only add them if they weren't already enabled
      if (!aPreviousCombinedOptions.IsOptionEnabled(cAIRCOMBAT))
      {
         mCallbacks += WsfObserver::AirCombatSendEngagementSummaryData(mSimulationPtr)
                          .Connect(&EventPipe::AirCombatSendEngagementSummaryData, this);
         mCallbacks += WsfObserver::AirCombatSendFlightKinematicsData(mSimulationPtr)
                          .Connect(&EventPipe::AirCombatSendFlightKinematicsData, this);
         mCallbacks += WsfObserver::AirCombatSendFuelData(mSimulationPtr).Connect(&EventPipe::AirCombatSendFuelData, this);
         mCallbacks += WsfObserver::AirCombatSendNavData(mSimulationPtr).Connect(&EventPipe::AirCombatSendNavData, this);
         mCallbacks += WsfObserver::AirCombatSendFlightControlsData(mSimulationPtr)
                          .Connect(&EventPipe::AirCombatSendFlightControlsData, this);
         mCallbacks +=
            WsfObserver::AirCombatSendWeaponsData(mSimulationPtr).Connect(&EventPipe::AirCombatSendWeaponsData, this);
         mCallbacks +=
            WsfObserver::AirCombatSendTrackData(mSimulationPtr).Connect(&EventPipe::AirCombatSendTrackData, this);
         mCallbacks += WsfObserver::AirCombatSendPerceivedAssetsData(mSimulationPtr)
                          .Connect(&EventPipe::AirCombatSendPerceivedAssetsData, this);
         mCallbacks += WsfObserver::AirCombatSendPerceivedItemsData(mSimulationPtr)
                          .Connect(&EventPipe::AirCombatSendPerceivedItemsData, this);
         mCallbacks += WsfObserver::AirCombatSendPrioritizedThreatsAndTargetsData(mSimulationPtr)
                          .Connect(&EventPipe::AirCombatSendPrioritizedThreatsAndTargetsData, this);

         // TODO - Determine if these callbacks should be reinstated
         // mCallbacks += WsfObserver::AirCombatPlatformAdded(mSimulationPtr).Connect(&EventPipe::AirCombatAddPlatform,
         // this); mCallbacks += WsfObserver::PlatformAdded(mSimulationPtr).Connect(&EventPipe::AirCombatPlatformAdded,
         // this); mCallbacks +=
         // WsfObserver::PlatformDeleted(mSimulationPtr).Connect(&EventPipe::AirCombatPlatformDeleted, this);
      }
   }
   else if (aPreviousCombinedOptions.IsOptionEnabled(cAIRCOMBAT))
   {
      // Only remove them if they were enabled before
      mCallbacks.Clear();
   }
}

// This creates a MsgAirCombatEngagementData, populates its data members with values from aPlatform and aProcessor.
// Then, it 'sends' the message (which means writing it to the .aer file).
void wsf::AirCombat::EventPipe::AirCombatSendEngagementSummaryData(double                 aSimTime,
                                                                   const WsfPlatform&     aPlatform,
                                                                   const WsfSA_Processor& aProcessor)
{
   // Create the message.
   auto engagementDataMsg = ut::make_unique<WsfEventPipe::MsgSA_EngagementSummaryData>();

   // Get the platform index from the platform.
   engagementDataMsg->platformIndex(WsfEventPipeInterface::Platform(aPlatform));

   // Set discrete values
   engagementDataMsg->radarEmitting(aProcessor.RadarEmitting());
   engagementDataMsg->jammerEmitting(aProcessor.JammerEmitting());
   engagementDataMsg->otherEmitting(aProcessor.OtherSystemsEmitting());
   engagementDataMsg->afterburner(aProcessor.AfterburnerOn());
   engagementDataMsg->contrails(aProcessor.IsContrailing());
   engagementDataMsg->missionTask(aProcessor.Assess()->MissionTask());
   engagementDataMsg->riskAcceptable(aProcessor.Assess()->AcceptableLevelOfRiskByte());
   engagementDataMsg->riskCurrent(static_cast<float>(aProcessor.Assess()->Risk()));
   engagementDataMsg->defensiveness(static_cast<float>(aProcessor.Assess()->Defensiveness()));
   engagementDataMsg->urgency(static_cast<float>(aProcessor.Assess()->Urgency()));
   engagementDataMsg->selfRisk(static_cast<float>(aProcessor.Assess()->SelfRisk()));
   engagementDataMsg->flightRisk(static_cast<float>(aProcessor.Assess()->FlightRisk()));
   engagementDataMsg->packageRisk(static_cast<float>(aProcessor.Assess()->PackageRisk()));
   engagementDataMsg->missionRisk(static_cast<float>(aProcessor.Assess()->MissionRisk()));

   // Set the list of prioritized targets
   WsfEventPipe::StringList prioritizedTargetList;
   for (const auto& targetName : aProcessor.Assess()->PrioritizedTargetNames())
   {
      prioritizedTargetList.push_back(targetName);
   }
   engagementDataMsg->prioritizedTargetList(prioritizedTargetList);

   // Set the list of prioritized threats
   WsfEventPipe::StringList prioritizedThreatList;
   for (const auto& threatName : aProcessor.Assess()->PrioritizedThreatNames())
   {
      prioritizedThreatList.push_back(threatName);
   }
   engagementDataMsg->prioritizedThreatList(prioritizedThreatList);

   // Set the list of tracked platforms
   WsfEventPipe::StringList trackedPlatformsList;
   for (const auto& targetName : aProcessor.Assess()->CurrentlyTrackedPlatformNames())
   {
      trackedPlatformsList.push_back(targetName);
   }
   engagementDataMsg->trackedPlatformList(trackedPlatformsList);

   const WsfSA_Processor* saProcessor = WsfSA_Processor::GetPlatformSA_Processor(&aPlatform);
   if (saProcessor != nullptr)
   {
      const std::vector<size_t> platformIdList = saProcessor->GetFilteredPlatformIdList();

      // Set the list of engagement data
      WsfEventPipe::SA_EngagementDataList tempEngageDataList;
      for (const auto& platformId : platformIdList)
      {
         // Get the current platform in the loop
         WsfPlatform* tmpPlatform = mSimulationPtr->GetPlatformByIndex(platformId);

         if (tmpPlatform != nullptr)
         {
            std::string tmpPlatformName = tmpPlatform->GetName();

            if (tmpPlatformName != aPlatform.GetName()) // Be sure to exclude "self"
            {
               if (!saProcessor->IsFriendlySide(tmpPlatform->GetSideId().GetString())) // Also exclude friendly forces
               {
                  WsfEventPipe::SA_EngagementData engageData;
                  engageData.targetName(tmpPlatformName);
                  engageData.targetIndex(static_cast<unsigned int>(platformId));
                  engageData.targetType(tmpPlatform->GetType());
                  engageData.angleOff_deg(
                     static_cast<float>(aProcessor.Assess()->AngleOffTarget2D(*tmpPlatform) * UtMath::cDEG_PER_RAD));
                  engageData.aspectAngle_deg(static_cast<float>(
                     aProcessor.Assess()->AspectAngleForTarget2D(*tmpPlatform) * UtMath::cDEG_PER_RAD));

                  double currentRange_nm = aProcessor.Assess()->CalcSlantRange(*tmpPlatform) * UtMath::cNM_PER_M;
                  engageData.currentRange_nm(static_cast<float>(currentRange_nm));

                  engageData.currentDeltaAlt_ft(
                     static_cast<float>(aProcessor.Assess()->CalcDeltaAltitude(*tmpPlatform) * UtMath::cFT_PER_M));

                  // TODO - These will be added in the future:
                  //   engageData.trackQuality();
                  //   engageData.targetCID();
                  //   engageData.threatLevel();
                  //   engageData.targetValue();

                  engageData.detectionRange_nm(static_cast<float>(
                     aProcessor.Assess()->ExpectedDetectionRangeAgainstTarget(*tmpPlatform) * UtMath::cNM_PER_M));

                  double wezDesiredRange_nm =
                     aProcessor.Assess()->ExpectedWezRangeAgainstTarget(*tmpPlatform) * UtMath::cNM_PER_M;
                  engageData.wezDesiredRange_nm(static_cast<float>(wezDesiredRange_nm));

                  engageData.wezMinimum_nm(static_cast<float>(
                     aProcessor.Assess()->ExpectedMinimumEngagementRangeAgainstTarget(*tmpPlatform) * UtMath::cNM_PER_M));
                  engageData.wezMaximum_nm(static_cast<float>(
                     aProcessor.Assess()->ExpectedMaximumEngagementRangeAgainstTarget(*tmpPlatform) * UtMath::cNM_PER_M));
                  engageData.wezNoEscape_nm(static_cast<float>(
                     aProcessor.Assess()->ExpectedNoEscapeRangeAgainstTarget(*tmpPlatform) * UtMath::cNM_PER_M));

                  // TODO - These will be added in the future:
                  //   engageData.risk();
                  //   engageData.defensiveness();
                  //   engageData.urgency();

                  if (aProcessor.Assess()->IsCurrentlyBeingTracked(platformId))
                  {
                     engageData.tgtTracked(true);
                  }
                  else
                  {
                     engageData.tgtTracked(false);
                  }

                  if (currentRange_nm < wezDesiredRange_nm)
                  {
                     engageData.acceptableWez(true);
                  }
                  else
                  {
                     engageData.acceptableWez(false);
                  }

                  // Add this item to the list
                  tempEngageDataList.push_back(engageData);
               }
            }
         }
      }
      engagementDataMsg->engagementDataList(tempEngageDataList);

      // Finally, send the message
      Send(aSimTime, std::move(engagementDataMsg));
   }
}

void wsf::AirCombat::EventPipe::AirCombatSendFlightKinematicsData(double                 aSimTime,
                                                                  const WsfPlatform&     aPlatform,
                                                                  const WsfSA_Processor& aProcessor)
{
   // Get the SA Processor -- if none exists, return
   WsfSA_Processor* saProc = WsfSA_Processor::GetSA_Processor(aPlatform);
   if (saProc == nullptr)
   {
      return;
   }

   // Create the message
   auto flightDataMsg = ut::make_unique<WsfEventPipe::MsgSA_FlightKinematicsData>();

   // Get the flight data
   const WsfSA_Processor::SA_AircraftKinematics& tempFlightData = saProc->GetAircraftKinematics();

   // Set the outgoing data
   flightDataMsg->platformIndex(static_cast<unsigned int>(aPlatform.GetIndex()));
   flightDataMsg->flightId(static_cast<unsigned short>(saProc->FlightId()));
   flightDataMsg->idFlag(saProc->IdFlag());
   flightDataMsg->lat_deg(tempFlightData.lat_deg);
   flightDataMsg->lon_deg(tempFlightData.lon_deg);
   flightDataMsg->kcas(tempFlightData.kcas);
   flightDataMsg->ktas(tempFlightData.ktas);
   flightDataMsg->mach(tempFlightData.mach);
   flightDataMsg->altBaro_ft(tempFlightData.altBaro_ft);
   flightDataMsg->altRadar_ft(tempFlightData.altRadar_ft);
   flightDataMsg->vertSpd_fpm(tempFlightData.vertSpd_fpm);
   flightDataMsg->alpha_deg(tempFlightData.alpha_deg);
   flightDataMsg->beta_deg(tempFlightData.beta_deg);
   flightDataMsg->gLoad(tempFlightData.gLoad);
   flightDataMsg->gAvail(tempFlightData.gAvail);
   flightDataMsg->heading_deg(tempFlightData.heading_deg);
   flightDataMsg->pitch_deg(tempFlightData.pitch_deg);
   flightDataMsg->roll_deg(tempFlightData.roll_deg);
   flightDataMsg->gX(tempFlightData.gX);
   flightDataMsg->gY(tempFlightData.gY);
   flightDataMsg->pilotType(tempFlightData.pilotType);
   flightDataMsg->afterburnerOn(tempFlightData.afterburnerOn);
   flightDataMsg->contrailing(tempFlightData.contrailing);
   flightDataMsg->stallWarning(tempFlightData.stallWarning);
   flightDataMsg->alphaValid(tempFlightData.alphaValid);
   flightDataMsg->betaValid(tempFlightData.betaValid);
   flightDataMsg->kcasValid(tempFlightData.kcasValid);
   flightDataMsg->nxValid(tempFlightData.nxValid);
   flightDataMsg->nyValid(tempFlightData.nyValid);
   flightDataMsg->nzValid(tempFlightData.nzValid);
   flightDataMsg->gAvailValid(tempFlightData.gAvailValid);

   // Send the message
   Send(aSimTime, std::move(flightDataMsg));
}

void wsf::AirCombat::EventPipe::AirCombatSendFuelData(double                 aSimTime,
                                                      const WsfPlatform&     aPlatform,
                                                      const WsfSA_Processor& aProcessor)
{
   // Get the SA Processor -- if none exists, return
   WsfSA_Processor* saProc = WsfSA_Processor::GetSA_Processor(aPlatform);
   if (saProc == nullptr)
   {
      return;
   }

   // Create the message
   auto fuelDataMsg = ut::make_unique<WsfEventPipe::MsgSA_FuelData>();

   // Get the fuel data
   const WsfSA_Processor::SA_FuelSystemData& tempFuelData = saProc->GetFuelSystemData();

   // Set the outgoing data
   fuelDataMsg->platformIndex(static_cast<unsigned int>(aPlatform.GetIndex()));
   fuelDataMsg->fuelInternal_lbs(tempFuelData.fuelInternal_lbs);
   fuelDataMsg->fuelExternal_lbs(tempFuelData.fuelExternal_lbs);
   fuelDataMsg->fuelCapInt_lbs(tempFuelData.fuelCapInt_lbs);
   fuelDataMsg->fuelCapExt_lbs(tempFuelData.fuelCapExt_lbs);
   fuelDataMsg->grossWgt_lbs(tempFuelData.grossWgt_lbs);
   fuelDataMsg->joker_lbs(tempFuelData.joker_lbs);
   fuelDataMsg->bingo_lbs(tempFuelData.bingo_lbs);
   fuelDataMsg->fuelFlow_pph(tempFuelData.fuelFlow_pph);
   fuelDataMsg->normalizedFuelQty(tempFuelData.normalizedFuelQty);
   fuelDataMsg->numExtTanks(static_cast<unsigned char>(tempFuelData.numExtTanks));
   fuelDataMsg->dropTanksPresent(tempFuelData.dropTanksPresent);
   fuelDataMsg->fuelValid(tempFuelData.fuelValid);

   // Send the message
   Send(aSimTime, std::move(fuelDataMsg));
}

void wsf::AirCombat::EventPipe::AirCombatSendNavData(double                 aSimTime,
                                                     const WsfPlatform&     aPlatform,
                                                     const WsfSA_Processor& aProcessor)
{
   // Get the SA Processor -- if none exists, return
   WsfSA_Processor* saProc = WsfSA_Processor::GetSA_Processor(aPlatform);
   if (saProc == nullptr)
   {
      return;
   }

   // Create the message
   auto navDataMsg = ut::make_unique<WsfEventPipe::MsgSA_NavData>();

   // Get the fuel data
   const WsfSA_Processor::SA_NavigationDataSummary& tempNavData = saProc->GetNavigationDataSummary();

   // Set the outgoing data
   navDataMsg->platformIndex(static_cast<unsigned int>(aPlatform.GetIndex()));

   navDataMsg->wayptDataValid(tempNavData.dataValid);

   if (tempNavData.dataValid)
   {
      navDataMsg->waypointData().numWaypoints(static_cast<unsigned char>(tempNavData.numWaypoints));
      navDataMsg->waypointData().currentWaypoint(static_cast<unsigned char>(tempNavData.currentWaypoint));
      navDataMsg->waypointData().headingBug_deg(tempNavData.headingBug_deg);
      navDataMsg->waypointData().distanceToWaypoint_nm(tempNavData.distanceToWaypoint_nm);
      navDataMsg->waypointData().timeToWaypoint_sec(tempNavData.timeToWaypoint_sec);

      for (auto& wp : tempNavData.waypoints)
      {
         WsfEventPipe::WaypointLatLon wayPt;
         wayPt.lat_deg(wp.lat_deg);
         wayPt.lon_deg(wp.lon_deg);

         navDataMsg->waypointData().waypoints().push_back(wayPt);
      }
   }
   else
   {
      navDataMsg->waypointData().numWaypoints(0);
      navDataMsg->waypointData().currentWaypoint(0);
      navDataMsg->waypointData().headingBug_deg(0.0);
      navDataMsg->waypointData().distanceToWaypoint_nm(0.0);
      navDataMsg->waypointData().timeToWaypoint_sec(0.0);
   }

   // Send the message
   Send(aSimTime, std::move(navDataMsg));
}

void wsf::AirCombat::EventPipe::AirCombatSendFlightControlsData(double                 aSimTime,
                                                                const WsfPlatform&     aPlatform,
                                                                const WsfSA_Processor& aProcessor)
{
   // Get the SA Processor -- if none exists, return
   WsfSA_Processor* saProc = WsfSA_Processor::GetSA_Processor(aPlatform);
   if (saProc == nullptr)
   {
      return;
   }

   // Create the message
   auto controlsDataMsg = ut::make_unique<WsfEventPipe::MsgSA_FlightControlsData>();

   // Get the flight controls data
   const WsfSA_Processor::SA_FlightControlsDataSummary& tempFlightControlsData = saProc->GetFlightControlsDataSummary();

   // Set the outgoing data
   controlsDataMsg->platformIndex(static_cast<unsigned int>(aPlatform.GetIndex()));
   controlsDataMsg->stickRgtInput(tempFlightControlsData.stickRgtInput);
   controlsDataMsg->stickBackInput(tempFlightControlsData.stickBackInput);
   controlsDataMsg->rudderRgtInput(tempFlightControlsData.rudderRgtInput);
   controlsDataMsg->speedBrakeInput(tempFlightControlsData.speedBrakeInput);
   controlsDataMsg->throttleInput(tempFlightControlsData.throttleInput);
   controlsDataMsg->aileronRgtNormalized(tempFlightControlsData.aileronRgtNormalized);
   controlsDataMsg->aileronLftNormalized(tempFlightControlsData.aileronLftNormalized);
   controlsDataMsg->flapRgtNormalized(tempFlightControlsData.flapRgtNormalized);
   controlsDataMsg->flapLftNormalized(tempFlightControlsData.flapLftNormalized);
   controlsDataMsg->stabilizerRgtNormalized(tempFlightControlsData.stabilizerRgtNormalized);
   controlsDataMsg->stabilizerLftNormalized(tempFlightControlsData.stabilizerLftNormalized);
   controlsDataMsg->rudderRgtNormalized(tempFlightControlsData.rudderRgtNormalized);
   controlsDataMsg->rudderLftNormalized(tempFlightControlsData.rudderLftNormalized);
   controlsDataMsg->rudderNormalized(tempFlightControlsData.rudderNormalized);
   controlsDataMsg->speedBrakeNormalized(tempFlightControlsData.speedBrakeNormalized);
   controlsDataMsg->thrustNormalized(tempFlightControlsData.thrustNormalized);
   controlsDataMsg->landingGear(tempFlightControlsData.landingGear);
   controlsDataMsg->throttleValid(tempFlightControlsData.throttleValid);
   controlsDataMsg->aileronsValid(tempFlightControlsData.aileronsValid);
   controlsDataMsg->flapsValid(tempFlightControlsData.flapsValid);
   controlsDataMsg->twinRuddersValid(tempFlightControlsData.twinRuddersValid);
   controlsDataMsg->singleRudderValid(tempFlightControlsData.singleRudderValid);

   // Send the message
   Send(aSimTime, std::move(controlsDataMsg));
}

void wsf::AirCombat::EventPipe::AirCombatSendWeaponsData(double                 aSimTime,
                                                         const WsfPlatform&     aPlatform,
                                                         const WsfSA_Processor& aProcessor)
{
   WsfSA_Processor* sap = WsfSA_Processor::GetSA_Processor(aPlatform);

   if (sap != nullptr)
   {
      // Create the message
      auto weaponDataMsg = ut::make_unique<WsfEventPipe::MsgSA_WeaponsData>();

      const WsfSA_Processor::SA_WeaponsDataSummary weaponSummary = sap->GetWeaponsDataSummary();

      weaponDataMsg->platformIndex(static_cast<unsigned int>(aPlatform.GetIndex()));
      weaponDataMsg->selectedWeapon(weaponSummary.selectedWeapon);
      weaponDataMsg->numSelectedWeapon(weaponSummary.numSelectedWeapon);
      weaponDataMsg->initialNumWeapons(weaponSummary.initialNumWeapons);
      weaponDataMsg->numChaff(weaponSummary.numChaff);
      weaponDataMsg->numFlares(weaponSummary.numFlares);
      weaponDataMsg->numDecoys(weaponSummary.numDecoys);
      weaponDataMsg->weaponBayDoorsOpen(weaponSummary.weaponBayDoorsOpen);
      weaponDataMsg->supportingWeapon(weaponSummary.supportingWeapon);
      weaponDataMsg->masterWarning(weaponSummary.masterWarning);
      weaponDataMsg->masterCaution(weaponSummary.masterCaution);
      weaponDataMsg->shootCueActive(weaponSummary.shootCueActive);
      weaponDataMsg->shootCueBlink(weaponSummary.shootCueBlink);
      weaponDataMsg->masterArmActive(weaponSummary.masterArmActive);
      weaponDataMsg->jammingDetected(weaponSummary.jammingDetected);

      if (!weaponSummary.weaponList.empty())
      {
         WsfEventPipe::WeaponList weaponList;
         for (auto& wpn : weaponSummary.weaponList)
         {
            WsfEventPipe::WeaponNameQty weapon;
            weapon.weaponName(wpn.weaponName);
            weapon.quantity(wpn.quantity);
            weaponList.push_back(weapon);
         }
         weaponDataMsg->weaponList(weaponList);
      }

      if (!weaponSummary.engagedTargetList.empty())
      {
         WsfEventPipe::EngagedTargetList engagedList;
         for (auto& wpn : weaponSummary.engagedTargetList)
         {
            WsfEventPipe::EngagedTargetData tgtData;
            tgtData.targetIndex(wpn.targetIndex);
            tgtData.rangeMax_nm(wpn.rangeMax_nm);
            tgtData.rangeNe_nm(wpn.rangeNe_nm);
            tgtData.rangeMin_nm(wpn.rangeMin_nm);
            tgtData.rangeCurrent_nm(wpn.rangeCurrent_nm);
            tgtData.rangeClosureRate_kts(wpn.rangeClosureRate_kts);
            tgtData.timeToIntercept_sec(wpn.timeToIntercept_sec);
            tgtData.timeToActive_sec(wpn.timeToActive_sec);
            tgtData.targetKTAS(wpn.targetKTAS);
            tgtData.targetAspect_deg(wpn.targetAspect_deg);
            tgtData.targetAz_deg(wpn.targetAz_deg);
            tgtData.targetEl_deg(wpn.targetEl_deg);
            engagedList.push_back(tgtData);
         }
         weaponDataMsg->engagedTargetList(engagedList);
      }

      // Send the message
      Send(aSimTime, std::move(weaponDataMsg));
   }
}

void wsf::AirCombat::EventPipe::AirCombatSendTrackData(double                 aSimTime,
                                                       const WsfPlatform&     aPlatform,
                                                       const WsfSA_Processor& aProcessor)
{
   WsfSA_Processor* sap = WsfSA_Processor::GetSA_Processor(aPlatform);

   if (sap != nullptr)
   {
      // Create the message
      auto trackDataMsg = ut::make_unique<WsfEventPipe::MsgSA_TrackData>();

      // Set the platform index
      trackDataMsg->platformIndex(WsfEventPipeInterface::Platform(aPlatform));

      const std::vector<WsfSA_Processor::SA_TrackManagerData>& trackManagerList = sap->GetTrackManagerList();

      // This will be our track processor list for the pipe
      WsfEventPipe::TrackProcessorList trackProcessorList;

      for (auto& trkMgr : trackManagerList)
      {
         WsfEventPipe::TrackProcessorTracks trackProcessorData;

         trackProcessorData.trackProcessorName(trkMgr.name);
         trackProcessorData.isMasterProcessor(trkMgr.isMasterProcessor);

         for (auto& trackItem : trkMgr.trackList)
         {
            WsfEventPipe::TargetTrack tempTrack;

            tempTrack.targetPlatformIndex(trackItem.targetPlatformIndex);
            tempTrack.lat_deg(trackItem.lat_deg);
            tempTrack.lon_deg(trackItem.lon_deg);
            tempTrack.altitude_ft(trackItem.altitude_ft);
            tempTrack.bearing_deg(trackItem.bearing_deg);
            tempTrack.elevation_deg(trackItem.elevation_deg);
            tempTrack.range_nm(trackItem.range_nm);
            tempTrack.rangeRate_kts(trackItem.rangeRate_kts);
            tempTrack.targetSpeed_kts(trackItem.targetSpeed_kts);
            tempTrack.targetHdg_deg(trackItem.targetHdg_deg);
            tempTrack.targetAspect_deg(trackItem.targetAspect_deg);
            tempTrack.idString(trackItem.idString);
            tempTrack.tgtPlatformName(trackItem.tgtPlatformName);
            tempTrack.tgtTypeName(trackItem.tgtTypeName);
            tempTrack.identification(trackItem.identification);
            tempTrack.latLonValid(trackItem.latLonValid);
            tempTrack.altitudeValid(trackItem.altitudeValid);
            tempTrack.bearingValid(trackItem.bearingValid);
            tempTrack.elevationValid(trackItem.elevationValid);
            tempTrack.rangeValid(trackItem.rangeValid);
            tempTrack.rangeRateValid(trackItem.rangeRateValid);
            tempTrack.targetSpeedValid(trackItem.targetSpeedValid);
            tempTrack.targetHdgValid(trackItem.targetHdgValid);
            tempTrack.targetAspectValid(trackItem.targetAspectValid);
            tempTrack.idIffValid(trackItem.idIffValid);
            tempTrack.idAuxValid(trackItem.idAuxValid);
            tempTrack.idSideValid(trackItem.idSideValid);
            tempTrack.idTypeValid(trackItem.idTypeValid);
            tempTrack.isMissile(trackItem.isMissile);

            // Add the track
            trackProcessorData.tracks().push_back(tempTrack);
         }

         // Add this track processor
         trackDataMsg->trackProcessors().push_back(trackProcessorData);
      }

      trackDataMsg->esmTrackProcessor(sap->ESM_TrackProcessor());
      trackDataMsg->mwsTrackProcessor(sap->MWS_TrackProcessor());
      trackDataMsg->radarTrackProcessor(sap->RadarTrackProcessor());
      trackDataMsg->irstTrackProcessor(sap->IRST_TrackProcessor());
      trackDataMsg->dasTrackProcessor(sap->DAS_TrackProcessor());
      trackDataMsg->flirTrackProcessor(sap->FLIR_TrackProcessor());
      trackDataMsg->eyesTrackProcessor(sap->EyesTrackProcessor());

      // Send the message
      Send(aSimTime, std::move(trackDataMsg));
   }
}

void wsf::AirCombat::EventPipe::AirCombatSendPerceivedAssetsData(double                 aSimTime,
                                                                 const WsfPlatform&     aPlatform,
                                                                 const WsfSA_Processor& aProcessor)
{
   WsfSA_Processor* sap = WsfSA_Processor::GetSA_Processor(aPlatform);

   if (sap != nullptr)
   {
      // Create the message
      auto assetsDataMsg = ut::make_unique<WsfEventPipe::MsgSA_PerceivedAssetsData>();

      // Set the platform index
      assetsDataMsg->platformIndex(WsfEventPipeInterface::Platform(aPlatform));

      // Get the perception data (to get the assets)
      const WsfSA_Processor::PerceiveData* data = sap->GetData();

      // This is a temporary list of entities
      WsfEventPipe::SA_EntityPerceptionList list;

      // Look through the list
      for (auto& assetEntity : data->mPerceivedAssets)
      {
         WsfEventPipe::SA_EntityPerception entity;

         PopulateEntity(entity, assetEntity);

         list.push_back(entity);
      }

      // Add the list
      assetsDataMsg->assetsList(list);

      // Send the message
      Send(aSimTime, std::move(assetsDataMsg));
   }
}

void wsf::AirCombat::EventPipe::AirCombatSendPerceivedItemsData(double                 aSimTime,
                                                                const WsfPlatform&     aPlatform,
                                                                const WsfSA_Processor& aProcessor)
{
   AirCombatSendPerceivedBogiesAndBanditsData(aSimTime, aPlatform, aProcessor);
   AirCombatSendGroupsData(aSimTime, aPlatform, aProcessor);
}

void wsf::AirCombat::EventPipe::AirCombatSendPerceivedBogiesAndBanditsData(double                 aSimTime,
                                                                           const WsfPlatform&     aPlatform,
                                                                           const WsfSA_Processor& aProcessor)
{
   WsfSA_Processor* sap = WsfSA_Processor::GetSA_Processor(aPlatform);

   if (sap != nullptr)
   {
      // Create the message
      auto bogiesAndBanditsDataMsg = ut::make_unique<WsfEventPipe::MsgSA_PerceivedBogiesAndBanditsData>();

      // Set the platform index
      bogiesAndBanditsDataMsg->platformIndex(WsfEventPipeInterface::Platform(aPlatform));

      // Get the perception data (to get the bogies and bandits)
      const WsfSA_Processor::PerceiveData* data = sap->GetData();

      // This is a temporary list of bogie entities
      WsfEventPipe::SA_EntityPerceptionList bogieList;

      // Look through the bogies list
      for (auto& bogieEntity : data->mPerceivedBogies)
      {
         WsfEventPipe::SA_EntityPerception entity;

         PopulateEntity(entity, bogieEntity);

         bogieList.push_back(entity);
      }

      // Add the bogies list
      bogiesAndBanditsDataMsg->bogiesList(bogieList);

      // This is a temporary list of bandit entities
      WsfEventPipe::SA_EntityPerceptionList banditList;

      // Look through the bandits list
      for (auto& banditEntity : data->mPerceivedBandits)
      {
         WsfEventPipe::SA_EntityPerception entity;

         PopulateEntity(entity, banditEntity);

         banditList.push_back(entity);
      }

      // Add the threat list
      bogiesAndBanditsDataMsg->banditsList(banditList);

      // Send the message
      Send(aSimTime, std::move(bogiesAndBanditsDataMsg));
   }
}

void wsf::AirCombat::EventPipe::AirCombatSendGroupsData(double                 aSimTime,
                                                        const WsfPlatform&     aPlatform,
                                                        const WsfSA_Processor& aProcessor)
{
   WsfSA_Processor* sap = WsfSA_Processor::GetSA_Processor(aPlatform);
   if (sap != nullptr)
   {
      // Create the message
      auto groupsDataMsg = ut::make_unique<WsfEventPipe::MsgSA_GroupsData>();

      // Set the platform index
      groupsDataMsg->platformIndex(WsfEventPipeInterface::Platform(aPlatform));

      // Get the perceived groups
      const std::list<WsfSA_Group*> saGroupList = sap->Assess()->PerceivedGroups();

      // This will contain the groups
      WsfEventPipe::PerceivedGroupList groupList;

      for (auto& group : saGroupList)
      {
         WsfEventPipe::PerceivedGroup tmpGroup;

         tmpGroup.groupName(group->GetPerceivedName());
         tmpGroup.centroidLat_deg(group->GetLat_deg());
         tmpGroup.centroidLon_deg(group->GetLon_deg());
         tmpGroup.centroidAlt_ft(static_cast<float>(group->GetAltitude_ft()));
         tmpGroup.centroidHeading_deg(static_cast<float>(group->GetHeading_deg()));
         tmpGroup.centroidSpeed_kts(static_cast<float>(group->GetSpeed_kts()));
         tmpGroup.radiusCurrent_nm(group->GetRadiusCurrent_nm());
         tmpGroup.radiusMinimum_nm(group->GetRadiusMinimum_nm());

         WsfEventPipe::SA_EntityPerceptionList entityList;
         for (auto& element : group->GetElementList())
         {
            WsfEventPipe::SA_EntityPerception entity;

            PopulateEntity(entity, element);

            // Add entity to list
            entityList.push_back(entity);
         }

         // Set the list
         tmpGroup.elementList(entityList);

         tmpGroup.focus(group->GetFocus());
         tmpGroup.isImportant(group->GetImportance() == WsfSA_PerceivedItem::IMPORTANT);

         // Add this group to the group list
         groupList.push_back(tmpGroup);
      }

      // Add the group list
      groupsDataMsg->groupList(groupList);

      // Send the message
      Send(aSimTime, std::move(groupsDataMsg));
   }
}

void wsf::AirCombat::EventPipe::AirCombatSendPrioritizedThreatsAndTargetsData(double                 aSimTime,
                                                                              const WsfPlatform&     aPlatform,
                                                                              const WsfSA_Processor& aProcessor)
{
   WsfSA_Processor* sap = WsfSA_Processor::GetSA_Processor(aPlatform);

   if (sap != nullptr)
   {
      // Create the message
      auto threatsAndTargetsDataMsg = ut::make_unique<WsfEventPipe::MsgSA_PrioritizedThreatsAndTargetsData>();

      // Set the platform index
      threatsAndTargetsDataMsg->platformIndex(WsfEventPipeInterface::Platform(aPlatform));

      // Get the prioritized threats
      const auto& prioritizedThreatsList = sap->Assess()->GetPrioritizedThreatItems();

      // This is a temporary list of threat entities
      WsfEventPipe::SA_EntityPerceptionList threatList;

      // Look through the threats list
      for (auto threatItem : prioritizedThreatsList)
      {
         if (threatItem->GetItemType() == WsfSA_PerceivedItem::ENTITY)
         {
            WsfSA_EntityPerception*           threatEntity = dynamic_cast<WsfSA_EntityPerception*>(threatItem.Get());
            WsfEventPipe::SA_EntityPerception entity;

            PopulateEntity(entity, threatEntity);

            threatList.push_back(entity);
         }
      }

      // Add the threats list
      threatsAndTargetsDataMsg->prioritizedThreatsList(threatList);

      // Get the prioritized targets
      const auto& prioritizedTargetsList = sap->Assess()->GetPrioritizedTargetItems();

      // This is a temporary list of target entities
      WsfEventPipe::SA_EntityPerceptionList targetList;

      // Look through the targets list
      for (auto& targetItem : prioritizedTargetsList)
      {
         if (targetItem.IsValid() && targetItem->GetItemType() == WsfSA_PerceivedItem::ENTITY)
         {
            WsfSA_EntityPerception*           targetEntity = dynamic_cast<WsfSA_EntityPerception*>(targetItem.Get());
            WsfEventPipe::SA_EntityPerception entity;

            PopulateEntity(entity, targetEntity);

            targetList.push_back(entity);
         }
      }

      // Add the target list
      threatsAndTargetsDataMsg->prioritizedTargetsList(targetList);

      // Send the message
      Send(aSimTime, std::move(threatsAndTargetsDataMsg));
   }
}

void wsf::AirCombat::EventPipe::PopulateEntity(WsfEventPipe::SA_EntityPerception& aMessageEntity,
                                               const WsfSA_EntityPerception*      aPerceivedEntity)
{
   aMessageEntity.lat_deg(aPerceivedEntity->GetLat_deg());
   aMessageEntity.lon_deg(aPerceivedEntity->GetLon_deg());
   aMessageEntity.altitude_ft(static_cast<float>(aPerceivedEntity->GetAltitude_ft()));
   aMessageEntity.bearing_deg(static_cast<float>(aPerceivedEntity->GetBearing_deg()));
   aMessageEntity.speed_kts(static_cast<float>(aPerceivedEntity->GetSpeed_kts()));
   aMessageEntity.heading_deg(static_cast<float>(aPerceivedEntity->GetHeading_deg()));
   aMessageEntity.threatLevel(static_cast<float>(aPerceivedEntity->GetThreatLevel()));
   aMessageEntity.targetValue(static_cast<float>(aPerceivedEntity->GetTargetValue()));
   aMessageEntity.risk(static_cast<float>(aPerceivedEntity->GetRisk()));
   aMessageEntity.defensiveness(static_cast<float>(aPerceivedEntity->GetDefensiveness()));
   aMessageEntity.urgency(static_cast<float>(aPerceivedEntity->GetUrgency()));
   aMessageEntity.idString(aPerceivedEntity->GetIdString());
   aMessageEntity.perceivedIndex(static_cast<unsigned int>(aPerceivedEntity->GetPerceivedIndex()));
   aMessageEntity.perceivedName(aPerceivedEntity->GetPerceivedName());
   aMessageEntity.perceivedType(aPerceivedEntity->GetPerceivedType());
   aMessageEntity.flightId(static_cast<unsigned short>(aPerceivedEntity->GetFlightId()));
   aMessageEntity.idFlag(aPerceivedEntity->GetIdFlag());
   aMessageEntity.identification(static_cast<unsigned char>(aPerceivedEntity->GetIdentification()));
   aMessageEntity.friendlyAsset(aPerceivedEntity->GetFriendlyAsset());
   aMessageEntity.angleOnly(aPerceivedEntity->GetAngleOnly());
   aMessageEntity.idIffFriend(aPerceivedEntity->GetIdIffFriend());
   aMessageEntity.idIffFoe(aPerceivedEntity->GetIdIffFoe());
   aMessageEntity.idIffNeutral(aPerceivedEntity->GetIdIffNeutral());
   aMessageEntity.idIffUnknown(aPerceivedEntity->GetIdIffUnknown());
   aMessageEntity.idAuxFriend(aPerceivedEntity->GetIdAuxFoe());
   aMessageEntity.idAuxFoe(aPerceivedEntity->GetIdAuxFoe());
   aMessageEntity.idAuxNeutral(aPerceivedEntity->GetIdAuxNeutral());
   aMessageEntity.idAuxUnknown(aPerceivedEntity->GetIdAuxUnknown());
   aMessageEntity.idSideFriend(aPerceivedEntity->GetIdSideFriend());
   aMessageEntity.idSideFoe(aPerceivedEntity->GetIdSideFoe());
   aMessageEntity.idSideNeutral(aPerceivedEntity->GetIdSideNeutral());
   aMessageEntity.idSideUnknown(aPerceivedEntity->GetIdSideUnknown());
   aMessageEntity.idTypeFriend(aPerceivedEntity->GetIdTypeFriend());
   aMessageEntity.idTypeFoe(aPerceivedEntity->GetIdTypeFoe());
   aMessageEntity.idTypeNeutral(aPerceivedEntity->GetIdTypeNeutral());
   aMessageEntity.idTypeUnknown(aPerceivedEntity->GetIdTypeUnknown());
   aMessageEntity.isHostile(aPerceivedEntity->GetIsHostile());
   aMessageEntity.altitudeValid(aPerceivedEntity->GetAltitudeValid());
   aMessageEntity.speedValid(aPerceivedEntity->GetSpeedValid());
   aMessageEntity.headingValid(aPerceivedEntity->GetHeadingValid());
   aMessageEntity.isMissile(aPerceivedEntity->GetIsMissile());
   aMessageEntity.focus(aPerceivedEntity->GetParentGroup() ? aPerceivedEntity->GetParentGroup()->GetFocus() : true);
   aMessageEntity.isCoasting(aPerceivedEntity->GetIsCoasting());
   aMessageEntity.isImportant(aPerceivedEntity->GetImportance() == WsfSA_PerceivedItem::IMPORTANT);
}

// The EventPipeInterface constructor. All it does is set its internal pointer to an EventPipe to nullptr
wsf::AirCombat::EventPipeInterface::EventPipeInterface()
   : mEventPipePtr(nullptr)
{
}

// Get the EventPipeInterface associated with aSimulation
wsf::AirCombat::EventPipeInterface& wsf::AirCombat::EventPipeInterface::Get(const WsfSimulation& aSimulation)
{
   WsfSimulationExtension* extensionPtr = aSimulation.FindExtension("air_combat_eventpipe");
   return static_cast<EventPipeInterface&>(*extensionPtr);
}

// Called when the extension is added to the simulation. This creates the AirCombat::EventPipe and adds it as
// a logger to a WsfEventPipeInterface found in the simulation.
void wsf::AirCombat::EventPipeInterface::AddedToSimulation()
{
   WsfEventPipeInterface* eventPipePtr = WsfEventPipeInterface::Find(GetSimulation());
   if (eventPipePtr != nullptr)
   {
      mEventPipePtr = ut::make_unique<EventPipe>(eventPipePtr);
      eventPipePtr->AddLogger(mEventPipePtr.get());
   }
}
