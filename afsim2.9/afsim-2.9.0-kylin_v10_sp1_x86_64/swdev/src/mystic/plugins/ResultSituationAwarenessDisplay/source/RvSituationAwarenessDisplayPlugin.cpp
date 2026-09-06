// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "RvSituationAwarenessDisplayPlugin.hpp"

#include <vector>

#include <QString>

#include "RvEventPipeClasses.hpp"
#include "RvEventPipeUtil.hpp"
#include "RvResultData.hpp"
#include "RvResultDb.hpp"
#include "RvResultPlatform.hpp"
#include "RvTrackDb.hpp"
#include "UtMemory.hpp"
#include "UtVec3.hpp"

WKF_PLUGIN_DEFINE_SYMBOLS(RvSA_Display::Plugin,
                          "Situation Awareness Display",
                          "The SA Display plugin provides a custom-display of SA Processor data, inlcuding perception "
                          "of assets, bogies, and bandits, perceived groups, and truth.",
                          "mystic",
                          false) // Plugin not loaded by default

RvSA_Display::Plugin::Plugin(const QString& aPluginName, const size_t aUniqueID)
   : rv::PluginT<wkf::SA_Display::PluginBase>(aPluginName, aUniqueID)
{
}

void RvSA_Display::Plugin::RegularRead(const rv::ResultData& aData)
{
   AdvanceTimeRead(aData);
}

void RvSA_Display::Plugin::AdvanceTimeRead(const rv::ResultData& aData)
{
   UpdateDataContainer(aData);
}

bool RvSA_Display::Plugin::HasSituationAwarenessProcessor(wkf::Platform* aPlatform)
{
   if (rv::Environment::Exists())
   {
      rv::ResultData* currentData = rvEnv.GetData();
      if (currentData != nullptr)
      {
         auto platform = currentData->FindPlatform(aPlatform->GetIndex());
         if (platform != nullptr)
         {
            return HasSA_Data(platform, currentData->GetSimTime());
         }
      }
   }
   return false;
}

void RvSA_Display::Plugin::ClearScenario(bool aFullReset)
{
   Reset();
}

void RvSA_Display::Plugin::NewDisplay(wkf::Platform* aPlatform)
{
   mPlatformsOfInterest.insert(aPlatform->GetIndex());
}

void RvSA_Display::Plugin::RemoveDisplay(size_t aIndex)
{
   mPlatformsOfInterest.erase(aIndex);
}

void RvSA_Display::Plugin::PopulateEntityPerception(wkf::SA_Display::EntityPerception& aEntityPerception,
                                                    const rv::SA_EntityPerception&     aData)
{
   aEntityPerception.lat_deg        = aData.lat_deg();
   aEntityPerception.lon_deg        = aData.lon_deg();
   aEntityPerception.altitude_ft    = aData.altitude_ft();
   aEntityPerception.bearing_deg    = aData.bearing_deg();
   aEntityPerception.speed_kts      = aData.speed_kts();
   aEntityPerception.heading_deg    = aData.heading_deg();
   aEntityPerception.threatLevel    = aData.threatLevel();
   aEntityPerception.targetValue    = aData.targetValue();
   aEntityPerception.risk           = aData.risk();
   aEntityPerception.defensiveness  = aData.defensiveness();
   aEntityPerception.urgency        = aData.urgency();
   aEntityPerception.idString       = aData.idString();
   aEntityPerception.perceivedIndex = aData.perceivedIndex();
   aEntityPerception.perceivedName  = aData.perceivedName();
   aEntityPerception.perceivedType  = aData.perceivedType();
   aEntityPerception.flightId       = static_cast<unsigned short>(aData.flightId());
   aEntityPerception.idFlag         = aData.idFlag();
   aEntityPerception.identification = static_cast<wkf::SA_Display::Identification>(aData.identification());
   aEntityPerception.friendlyAsset  = aData.friendlyAsset();
   aEntityPerception.angleOnly      = aData.angleOnly();
   aEntityPerception.idIffFriend    = aData.idIffFriend();
   aEntityPerception.idIffFoe       = aData.idIffFoe();
   aEntityPerception.idIffNeutral   = aData.idIffNeutral();
   aEntityPerception.idIffUnknown   = aData.idIffUnknown();
   aEntityPerception.idAuxFriend    = aData.idAuxFriend();
   aEntityPerception.idAuxFoe       = aData.idAuxFoe();
   aEntityPerception.idAuxNeutral   = aData.idAuxNeutral();
   aEntityPerception.idAuxUnknown   = aData.idAuxUnknown();
   aEntityPerception.idSideFriend   = aData.idSideFriend();
   aEntityPerception.idSideFoe      = aData.idSideFoe();
   aEntityPerception.idSideNeutral  = aData.idSideNeutral();
   aEntityPerception.idSideUnknown  = aData.idSideUnknown();
   aEntityPerception.idTypeFriend   = aData.idTypeFriend();
   aEntityPerception.idTypeFoe      = aData.idTypeFoe();
   aEntityPerception.idTypeNeutral  = aData.idTypeNeutral();
   aEntityPerception.idTypeUnknown  = aData.idTypeUnknown();
   aEntityPerception.isHostile      = aData.isHostile();
   aEntityPerception.altitudeValid  = aData.altitudeValid();
   aEntityPerception.speedValid     = aData.speedValid();
   aEntityPerception.headingValid   = aData.headingValid();
   aEntityPerception.isMissile      = aData.isMissile();
}

void RvSA_Display::Plugin::UpdateDataContainer(const rv::ResultData& aData)
{
   // Get the current time
   double simTime = aData.GetSimTime();

   // Only update the data container if we have a sa display to update
   if (mSA_Displays.size() > 0)
   {
      // Gather truth data of every platform that is valid (not deleted/removed)
      std::vector<wkf::SA_Display::EntityTruth> tempSA_TruthPlatforms;
      rv::ResultDb*                             db = aData.GetDb();
      // Get the database to get the platforms
      if (db != nullptr)
      {
         // Loop through each platform
         for (unsigned int i = 0; i < db->GetPlatformCount(); i++)
         {
            rv::ResultPlatform* platformPtr = aData.FindPlatform(i);
            // If we have a valid platform, populate using rv::ResultPlatform* data
            if (platformPtr != nullptr)
            {
               // If our platform shouldn't exist yet, skip it
               if (platformPtr->GetStartTime() > rvEnv.GetData()->GetSimTime())
               {
                  continue;
               }

               // If the platform is deleted, skip it
               const auto entityStatus =
                  platformPtr->FindFirstBefore<rv::MsgPlatformStatus>(rvEnv.GetData()->GetSimTime());
               if (entityStatus != nullptr)
               {
                  if (entityStatus->removed())
                  {
                     continue;
                  }
               }

               // Create a temp container
               wkf::SA_Display::EntityTruth entity;

               // Use EntityState data to fill the truth data
               const auto entityState = platformPtr->FindFirstBefore<rv::MsgEntityState>(rvEnv.GetData()->GetSimTime());
               if (entityState != nullptr)
               {
                  rv::EntityState state = entityState->state();
                  // Create a temporary entity to get the data we need
                  UtEntity tmpEntity;
                  tmpEntity.SetLocationWCS(rv::toVec3(state.locationWCS()).GetData());
                  tmpEntity.SetOrientationWCS(state.orientationWCS().x(),
                                              state.orientationWCS().y(),
                                              state.orientationWCS().z());
                  tmpEntity.SetVelocityWCS(rv::toVec3(state.velocityWCS()).GetData());
                  tmpEntity.SetAccelerationWCS(rv::toVec3(state.accelerationWCS()).GetData());

                  double lat, lon, alt;
                  tmpEntity.GetLocationLLA(lat, lon, alt);
                  entity.lat_deg = lat;
                  entity.lon_deg = lon;

                  double heading_rad, dummypitch, dummyroll;
                  tmpEntity.GetOrientationNED(heading_rad, dummypitch, dummyroll);
                  entity.heading_deg = heading_rad * UtMath::cDEG_PER_RAD;
                  entity.altitude_ft = tmpEntity.GetAltitude() * UtMath::cFT_PER_M;
                  entity.speed_kts   = tmpEntity.GetSpeed() * UtMath::cMPS_PER_NMPH;

                  double lwcs[3];
                  tmpEntity.GetLocationWCS(lwcs);
                  entity.bearing_deg = tmpEntity.TrueBearing(lwcs) * UtMath::cDEG_PER_RAD;

                  entity.index = platformPtr->GetPlatformIndex();
                  entity.name  = platformPtr->GetName();

                  // Set missile flag to false, but it may become true below
                  entity.isMissile = false;

                  // Get the type from the platform info message, if it exists.
                  auto infoPtr = platformPtr->FindFirstBefore<rv::MsgPlatformInfo>(simTime);
                  if (infoPtr == nullptr)
                  {
                     infoPtr = platformPtr->FindFirstAfter<rv::MsgPlatformInfo>(simTime);
                  }
                  if (infoPtr != nullptr)
                  {
                     entity.domain = rv::SpatialDomain::ToString(infoPtr->spatialDomain());

                     if (!infoPtr->types().empty())
                     {
                        entity.type = *infoPtr->types().begin();
                     }

                     rv::StringList catList = infoPtr->categories();
                     for (auto& category : catList)
                     {
                        if (category == "missile")
                        {
                           entity.isMissile = true;
                           break; // We can quit since we found a missile
                        }
                     }
                  }

                  entity.altitudeValid = true;
                  entity.speedValid    = true;
                  entity.headingValid  = true;
               }

               tempSA_TruthPlatforms.emplace_back(entity);
            }
         }
      }
      // Truth data end

      // SA data start
      // Create a temp container for the sa data
      std::map<size_t, wkf::SA_Display::SA_Data> tempSA_Data;

      // For each sa display, update the sa data
      for (auto sad : mSA_Displays)
      {
         // Get the platform from the result data
         rv::ResultPlatform* platform = aData.FindPlatform(sad.first);

         // Create a temporary data container
         wkf::SA_Display::SA_Data tempData;

         // If we found the platform, fill in the data
         if (platform != nullptr)
         {
            // Platform Status Message
            const rv::MsgPlatformStatus* platformStatusData = platform->FindFirstBefore<rv::MsgPlatformStatus>(simTime);

            // Check for a broken or removed platform here, and activate the kill frame if so
            if (platformStatusData != nullptr)
            {
               if (platformStatusData->broken() || platformStatusData->removed())
               {
                  sad.second->ActivateKillFrame(true);
               }
            }
            else
            {
               sad.second->ActivateKillFrame(false);
            }

            // Set the time
            tempData.platformData.simTime = simTime;

            // Use EntityState data first.  If there is other data, it will override with data below
            const auto entityState = platform->FindFirstBefore<rv::MsgEntityState>(rvEnv.GetData()->GetSimTime());
            if (entityState != nullptr)
            {
               rv::EntityState state = entityState->state();
               // Create a temporary entity to get the NED data (heading, pitch, roll)
               UtEntity tmpEntity;
               tmpEntity.SetLocationWCS(rv::toVec3(state.locationWCS()).GetData());
               tmpEntity.SetOrientationWCS(state.orientationWCS().x(),
                                           state.orientationWCS().y(),
                                           state.orientationWCS().z());
               tmpEntity.SetVelocityWCS(rv::toVec3(state.velocityWCS()).GetData());
               tmpEntity.SetAccelerationWCS(rv::toVec3(state.accelerationWCS()).GetData());

               double tmpHeading_rad, tmpPitch_rad, tmpRoll_rad;
               tmpEntity.GetOrientationNED(tmpHeading_rad, tmpPitch_rad, tmpRoll_rad);

               tempData.platformData.heading_deg = tmpHeading_rad * UtMath::cDEG_PER_RAD;
               tempData.platformData.pitch_deg   = tmpPitch_rad * UtMath::cRAD_PER_DEG;
               tempData.platformData.roll_deg    = tmpRoll_rad * UtMath::cRAD_PER_DEG;
            }

            // Most data will come from the SA messages
            const rv::MsgSA_FlightKinematicsData* flightData =
               platform->FindFirstBefore<rv::MsgSA_FlightKinematicsData>(aData.GetSimTime());
            const rv::MsgSA_NavData*     navData  = platform->FindFirstBefore<rv::MsgSA_NavData>(aData.GetSimTime());
            const rv::MsgSA_FuelData*    fuelData = platform->FindFirstBefore<rv::MsgSA_FuelData>(aData.GetSimTime());
            const rv::MsgSA_WeaponsData* weaponsData =
               platform->FindFirstBefore<rv::MsgSA_WeaponsData>(aData.GetSimTime());
            const rv::MsgSA_TrackData* trackData = platform->FindFirstBefore<rv::MsgSA_TrackData>(aData.GetSimTime());
            const rv::MsgSA_PerceivedAssetsData* assetsData =
               platform->FindFirstBefore<rv::MsgSA_PerceivedAssetsData>(aData.GetSimTime());
            const rv::MsgSA_PerceivedBogiesAndBanditsData* bogiesAndBanditsData =
               platform->FindFirstBefore<rv::MsgSA_PerceivedBogiesAndBanditsData>(aData.GetSimTime());
            const rv::MsgSA_PrioritizedThreatsAndTargetsData* prioritizedThreatsAndTargetsData =
               platform->FindFirstBefore<rv::MsgSA_PrioritizedThreatsAndTargetsData>(aData.GetSimTime());
            const rv::MsgSA_GroupsData* groupsData = platform->FindFirstBefore<rv::MsgSA_GroupsData>(aData.GetSimTime());

            // Set the data (will override any data from above)
            // If we have valid flight data, fill it in
            if (flightData != nullptr)
            {
               tempData.platformData.platformIndex = flightData->platformIndex();
               tempData.platformData.simTime       = simTime;
               tempData.platformData.flightId      = flightData->flightId();
               tempData.platformData.idFlag        = flightData->idFlag();
               tempData.platformData.lat_deg       = flightData->lat_deg();
               tempData.platformData.lon_deg       = flightData->lon_deg();
               tempData.platformData.kcas          = flightData->kcas();
               tempData.platformData.ktas          = flightData->ktas();
               tempData.platformData.mach          = flightData->mach();
               tempData.platformData.altBaro_ft    = flightData->altBaro_ft();
               tempData.platformData.altRadar_ft   = flightData->altRadar_ft();
               tempData.platformData.vertSpd_fpm   = flightData->vertSpd_fpm();
               tempData.platformData.alpha_deg     = flightData->alpha_deg();
               tempData.platformData.beta_deg      = flightData->beta_deg();
               tempData.platformData.gLoad         = flightData->gLoad();
               tempData.platformData.gAvail        = flightData->gAvail();
               tempData.platformData.heading_deg   = flightData->heading_deg();
               tempData.platformData.pitch_deg     = flightData->pitch_deg();
               tempData.platformData.roll_deg      = flightData->roll_deg();
               tempData.platformData.gX            = flightData->gX();
               tempData.platformData.gY            = flightData->gY();
               tempData.platformData.pilotType     = flightData->pilotType();
               tempData.platformData.afterburnerOn = flightData->afterburnerOn();
               tempData.platformData.contrailing   = flightData->contrailing();
               tempData.platformData.stallWarning  = flightData->stallWarning();
               tempData.platformData.alphaValid    = flightData->alphaValid();
               tempData.platformData.betaValid     = flightData->betaValid();
               tempData.platformData.kcasValid     = flightData->kcasValid();
               tempData.platformData.nxValid       = flightData->nxValid();
               tempData.platformData.nyValid       = flightData->nyValid();
               tempData.platformData.nzValid       = flightData->nzValid();
               tempData.platformData.gAvailValid   = flightData->gAvailValid();
            }

            // If we have valid nav data, fill it in
            if (navData != nullptr)
            {
               tempData.navData.platformIndex  = navData->platformIndex();
               tempData.navData.wayptDataValid = navData->wayptDataValid();

               tempData.navData.waypointData.numWaypoints          = navData->waypointData().numWaypoints();
               tempData.navData.waypointData.currentWaypoint       = navData->waypointData().currentWaypoint();
               tempData.navData.waypointData.headingBug_deg        = navData->waypointData().headingBug_deg();
               tempData.navData.waypointData.distanceToWaypoint_nm = navData->waypointData().distanceToWaypoint_nm();
               tempData.navData.waypointData.timeToWaypoint_sec    = navData->waypointData().timeToWaypoint_sec();

               for (auto& wp : navData->waypointData().waypoints())
               {
                  wkf::SA_Display::WaypointLatLon wayPt;
                  wayPt.lat_deg = wp.lat_deg();
                  wayPt.lon_deg = wp.lon_deg();

                  tempData.navData.waypointData.waypoints.push_back(wayPt);
               }
            }
            else
            {
               tempData.navData.platformIndex                      = 0;
               tempData.navData.wayptDataValid                     = false;
               tempData.navData.waypointData.numWaypoints          = 0;
               tempData.navData.waypointData.currentWaypoint       = 0;
               tempData.navData.waypointData.headingBug_deg        = 0.0;
               tempData.navData.waypointData.distanceToWaypoint_nm = 0.0;
               tempData.navData.waypointData.timeToWaypoint_sec    = 0.0;

               tempData.navData.waypointData.waypoints.clear();
            }

            // If we have valid fuel data, fill it in
            if (fuelData != nullptr)
            {
               tempData.fuelData.platformIndex     = fuelData->platformIndex();
               tempData.fuelData.fuelInternal_lbs  = fuelData->fuelInternal_lbs();
               tempData.fuelData.fuelExternal_lbs  = fuelData->fuelExternal_lbs();
               tempData.fuelData.fuelCapInt_lbs    = fuelData->fuelCapInt_lbs();
               tempData.fuelData.fuelCapExt_lbs    = fuelData->fuelCapExt_lbs();
               tempData.fuelData.grossWgt_lbs      = fuelData->grossWgt_lbs();
               tempData.fuelData.joker_lbs         = fuelData->joker_lbs();
               tempData.fuelData.bingo_lbs         = fuelData->bingo_lbs();
               tempData.fuelData.fuelFlow_pph      = fuelData->fuelFlow_pph();
               tempData.fuelData.normalizedFuelQty = fuelData->normalizedFuelQty();
               tempData.fuelData.numExtTanks       = fuelData->numExtTanks();
               tempData.fuelData.dropTanksPresent  = fuelData->dropTanksPresent();
               tempData.fuelData.fuelValid         = fuelData->fuelValid();
            }
            else
            {
               tempData.fuelData.platformIndex     = 0;
               tempData.fuelData.fuelInternal_lbs  = 0.0;
               tempData.fuelData.fuelExternal_lbs  = 0.0;
               tempData.fuelData.fuelCapInt_lbs    = 0.0;
               tempData.fuelData.fuelCapExt_lbs    = 0.0;
               tempData.fuelData.grossWgt_lbs      = 0.0;
               tempData.fuelData.joker_lbs         = 0.0;
               tempData.fuelData.bingo_lbs         = 0.0;
               tempData.fuelData.fuelFlow_pph      = 0.0;
               tempData.fuelData.normalizedFuelQty = 0.0;
               tempData.fuelData.numExtTanks       = 0;
               tempData.fuelData.dropTanksPresent  = false;
               tempData.fuelData.fuelValid         = false;
            }

            // If we have valid weapons data, fill it in
            if (weaponsData != nullptr)
            {
               tempData.weaponData.platformIndex      = weaponsData->platformIndex();
               tempData.weaponData.selectedWeapon     = weaponsData->selectedWeapon();
               tempData.weaponData.numSelectedWeapon  = weaponsData->numSelectedWeapon();
               tempData.weaponData.numChaff           = weaponsData->numChaff();
               tempData.weaponData.numFlares          = weaponsData->numFlares();
               tempData.weaponData.numDecoys          = weaponsData->numDecoys();
               tempData.weaponData.weaponBayDoorsOpen = weaponsData->weaponBayDoorsOpen();
               tempData.weaponData.supportingWeapon   = weaponsData->supportingWeapon();
               tempData.weaponData.masterWarning      = weaponsData->masterWarning();
               tempData.weaponData.masterCaution      = weaponsData->masterCaution();
               tempData.weaponData.shootCueActive     = weaponsData->shootCueActive();
               tempData.weaponData.shootCueBlink      = weaponsData->shootCueBlink();
               tempData.weaponData.masterArmActive    = weaponsData->masterArmActive();
               tempData.weaponData.jammingDetected    = weaponsData->jammingDetected();

               if (!weaponsData->weaponList().empty())
               {
                  for (auto& wpn : weaponsData->weaponList())
                  {
                     wkf::SA_Display::WeaponNameQty weapon;
                     weapon.quantity   = wpn.quantity();
                     weapon.weaponName = wpn.weaponName();

                     tempData.weaponData.weaponList.push_back(weapon);
                  }
               }
               else
               {
                  tempData.weaponData.weaponList.clear();
               }

               if (!weaponsData->engagedTargetList().empty())
               {
                  for (auto& tgt : weaponsData->engagedTargetList())
                  {
                     wkf::SA_Display::EngagedTargetData target;

                     target.targetIndex          = tgt.targetIndex();
                     target.rangeMax_nm          = tgt.rangeMax_nm();
                     target.rangeNe_nm           = tgt.rangeNe_nm();
                     target.rangeMin_nm          = tgt.rangeMin_nm();
                     target.rangeCurrent_nm      = tgt.rangeCurrent_nm();
                     target.rangeClosureRate_kts = tgt.rangeClosureRate_kts();
                     target.timeToIntercept_sec  = tgt.timeToIntercept_sec();
                     target.timeToActive_sec     = tgt.timeToActive_sec();
                     target.targetKTAS           = tgt.targetKTAS();
                     target.targetAspect_deg     = tgt.targetAspect_deg();
                     target.targetAz_deg         = tgt.targetAz_deg();
                     target.targetEl_deg         = tgt.targetEl_deg();

                     tempData.weaponData.engagedTargetList.push_back(target);
                  }
               }
               else
               {
                  tempData.weaponData.engagedTargetList.clear();
               }
            }
            else
            {
               tempData.weaponData.platformIndex      = 0;
               tempData.weaponData.selectedWeapon     = "";
               tempData.weaponData.numSelectedWeapon  = 0;
               tempData.weaponData.numChaff           = 0;
               tempData.weaponData.numFlares          = 0;
               tempData.weaponData.numDecoys          = 0;
               tempData.weaponData.weaponBayDoorsOpen = false;
               tempData.weaponData.supportingWeapon   = false;
               tempData.weaponData.masterWarning      = false;
               tempData.weaponData.masterCaution      = false;
               tempData.weaponData.shootCueActive     = false;
               tempData.weaponData.shootCueBlink      = false;
               tempData.weaponData.masterArmActive    = false;
               tempData.weaponData.jammingDetected    = false;

               tempData.weaponData.weaponList.clear();
               tempData.weaponData.engagedTargetList.clear();
            }

            // If we have valid track data, fill it in
            if (trackData != nullptr)
            {
               // Loop through the track processors
               for (auto& trkProc : trackData->trackProcessors())
               {
                  // We use the first master track processor we find
                  if (trkProc.isMasterProcessor())
                  {
                     for (auto& trk : trkProc.tracks())
                     {
                        wkf::SA_Display::TargetTrackItem track;

                        track.tgtPlatformIndex = trk.targetPlatformIndex();
                        track.tgtLat_deg       = trk.lat_deg();
                        track.tgtLon_deg       = trk.lon_deg();
                        track.tgtAlt_ft        = trk.altitude_ft();
                        track.tgtBearing_rad   = trk.bearing_deg() * UtMath::cRAD_PER_DEG;
                        track.tgtElevation_rad = trk.elevation_deg() * UtMath::cRAD_PER_DEG;
                        track.tgtRange_nm      = trk.range_nm();
                        track.tgtRangeRate_kts = trk.rangeRate_kts();
                        track.tgtSpeed_kts     = trk.targetSpeed_kts();
                        track.tgtHdg_deg       = trk.targetHdg_deg();
                        track.tgtAspect_deg    = trk.targetAspect_deg();
                        track.tgtIdString      = trk.idString();
                        track.tgtPlatformName  = trk.tgtPlatformName();
                        track.tgtTypeName      = trk.tgtTypeName();
                        track.identification =
                           static_cast<wkf::SA_Display::Identification>(static_cast<int>(trk.identification()));
                        track.latLonValid       = trk.latLonValid();
                        track.altitudeValid     = trk.altitudeValid();
                        track.bearingValid      = trk.bearingValid();
                        track.elevationValid    = trk.elevationValid();
                        track.rangeValid        = trk.rangeValid();
                        track.rangeRateValid    = trk.rangeRateValid();
                        track.targetSpeedValid  = trk.targetSpeedValid();
                        track.targetHdgValid    = trk.targetHdgValid();
                        track.targetAspectValid = trk.targetAspectValid();
                        track.idIffValid        = trk.idIffValid();
                        track.idAuxValid        = trk.idAuxValid();
                        track.idSideValid       = trk.idSideValid();
                        track.idTypeValid       = trk.idTypeValid();
                        track.isMissile         = trk.isMissile();

                        // Add to the list of tracks
                        tempData.trackList.emplace_back(track);
                     }

                     break;
                  }
               }
            }

            // If we have valid asset data, fill it in
            if (assetsData != nullptr)
            {
               for (auto& asset : assetsData->assetsList())
               {
                  wkf::SA_Display::EntityPerception entity;
                  PopulateEntityPerception(entity, asset);

                  // Add to the list
                  tempData.assetsList.emplace_back(entity);
               }
            }

            // If we have valid bogie and bandit data, fill it in
            if (bogiesAndBanditsData != nullptr)
            {
               // Perceived Bogies
               for (auto& bogie : bogiesAndBanditsData->bogiesList())
               {
                  wkf::SA_Display::EntityPerception entity;
                  PopulateEntityPerception(entity, bogie);

                  // Add to the list
                  tempData.bogiesList.emplace_back(entity);
               }

               // Perceived Bandits

               for (auto& bandit : bogiesAndBanditsData->banditsList())
               {
                  wkf::SA_Display::EntityPerception entity;
                  PopulateEntityPerception(entity, bandit);

                  // Add to the list
                  tempData.banditsList.emplace_back(entity);
               }
            }

            // If we have valid priority data, fill it in
            if (prioritizedThreatsAndTargetsData != nullptr)
            {
               // Prioritized Threats
               for (auto& threat : prioritizedThreatsAndTargetsData->prioritizedThreatsList())
               {
                  wkf::SA_Display::EntityPerception entity;
                  PopulateEntityPerception(entity, threat);

                  // Add to the list
                  tempData.prioritizedThreatsList.emplace_back(entity);
               }

               // Prioritized Targets
               for (auto& target : prioritizedThreatsAndTargetsData->prioritizedTargetsList())
               {
                  wkf::SA_Display::EntityPerception entity;
                  PopulateEntityPerception(entity, target);

                  // Add to the list
                  tempData.prioritizedTargetsList.emplace_back(entity);
               }
            }

            // If we have valid group data, fill it in
            if (groupsData != nullptr)
            {
               for (auto& group : groupsData->groupList())
               {
                  wkf::SA_Display::PerceivedGroup perceivedGroup;

                  perceivedGroup.groupName           = group.groupName();
                  perceivedGroup.centroidLat_deg     = group.centroidLat_deg();
                  perceivedGroup.centroidLon_deg     = group.centroidLon_deg();
                  perceivedGroup.centroidAlt_ft      = group.centroidAlt_ft();
                  perceivedGroup.centroidHeading_deg = group.centroidHeading_deg();
                  perceivedGroup.centroidSpeed_kts   = group.centroidSpeed_kts();
                  perceivedGroup.radiusCurrent_nm    = group.radiusCurrent_nm();
                  perceivedGroup.radiusMinimum_nm    = group.radiusMinimum_nm();

                  for (auto& target : group.elementList())
                  {
                     wkf::SA_Display::EntityPerception entity;
                     PopulateEntityPerception(entity, target);

                     // Add to the list
                     perceivedGroup.elementList.emplace_back(entity);
                  }

                  // Add to the list
                  tempData.groupsList.emplace_back(perceivedGroup);
               }
            }

            // Add this platform's temp data to the data container
            tempSA_Data.emplace(sad.first, tempData);
         } // platform != nullptr
      }    // for (auto sad : mSA_Displays)

      // Update our SA data
      mDataContainer.SetSA_Data(tempSA_Data);
      mDataContainer.SetSA_TruthPlatforms(tempSA_TruthPlatforms);
   } // if (mSA_Displays.size() > 0)
}

const bool RvSA_Display::Plugin::HasSA_Data(rv::ResultPlatform* aPlatformPtr, const double aSimTime)
{
   // If the platform has any SA messages, then it has SA data.  Check if the platform has SA messages.
   auto eData = aPlatformPtr->FindFirstBefore<rv::MsgSA_EngagementSummaryData>(aSimTime);
   if (eData == nullptr)
   {
      eData = aPlatformPtr->FindFirstAfter<rv::MsgSA_EngagementSummaryData>(aSimTime);
   }
   if (eData != nullptr)
   {
      return true;
   }

   auto fData = aPlatformPtr->FindFirstBefore<rv::MsgSA_FlightControlsData>(aSimTime);
   if (fData == nullptr)
   {
      fData = aPlatformPtr->FindFirstAfter<rv::MsgSA_FlightControlsData>(aSimTime);
   }
   if (fData != nullptr)
   {
      return true;
   }

   auto kData = aPlatformPtr->FindFirstBefore<rv::MsgSA_FlightKinematicsData>(aSimTime);
   if (kData == nullptr)
   {
      kData = aPlatformPtr->FindFirstAfter<rv::MsgSA_FlightKinematicsData>(aSimTime);
   }
   if (kData != nullptr)
   {
      return true;
   }

   auto fuelData = aPlatformPtr->FindFirstBefore<rv::MsgSA_FuelData>(aSimTime);
   if (fuelData == nullptr)
   {
      fuelData = aPlatformPtr->FindFirstAfter<rv::MsgSA_FuelData>(aSimTime);
   }
   if (fuelData != nullptr)
   {
      return true;
   }

   auto gData = aPlatformPtr->FindFirstBefore<rv::MsgSA_GroupsData>(aSimTime);
   if (gData == nullptr)
   {
      gData = aPlatformPtr->FindFirstAfter<rv::MsgSA_GroupsData>(aSimTime);
   }
   if (gData != nullptr)
   {
      return true;
   }

   auto nData = aPlatformPtr->FindFirstBefore<rv::MsgSA_NavData>(aSimTime);
   if (nData == nullptr)
   {
      nData = aPlatformPtr->FindFirstAfter<rv::MsgSA_NavData>(aSimTime);
   }
   if (nData != nullptr)
   {
      return true;
   }

   auto paData = aPlatformPtr->FindFirstBefore<rv::MsgSA_PerceivedAssetsData>(aSimTime);
   if (paData == nullptr)
   {
      paData = aPlatformPtr->FindFirstAfter<rv::MsgSA_PerceivedAssetsData>(aSimTime);
   }
   if (paData != nullptr)
   {
      return true;
   }

   auto pbabData = aPlatformPtr->FindFirstBefore<rv::MsgSA_PerceivedBogiesAndBanditsData>(aSimTime);
   if (pbabData == nullptr)
   {
      pbabData = aPlatformPtr->FindFirstAfter<rv::MsgSA_PerceivedBogiesAndBanditsData>(aSimTime);
   }
   if (pbabData != nullptr)
   {
      return true;
   }

   auto ptatData = aPlatformPtr->FindFirstBefore<rv::MsgSA_PrioritizedThreatsAndTargetsData>(aSimTime);
   if (ptatData == nullptr)
   {
      ptatData = aPlatformPtr->FindFirstAfter<rv::MsgSA_PrioritizedThreatsAndTargetsData>(aSimTime);
   }
   if (ptatData != nullptr)
   {
      return true;
   }

   auto tData = aPlatformPtr->FindFirstBefore<rv::MsgSA_TrackData>(aSimTime);
   if (tData == nullptr)
   {
      tData = aPlatformPtr->FindFirstAfter<rv::MsgSA_TrackData>(aSimTime);
   }
   if (tData != nullptr)
   {
      return true;
   }

   auto wData = aPlatformPtr->FindFirstBefore<rv::MsgSA_WeaponsData>(aSimTime);
   if (wData == nullptr)
   {
      wData = aPlatformPtr->FindFirstAfter<rv::MsgSA_WeaponsData>(aSimTime);
   }
   if (wData != nullptr)
   {
      return true;
   }

   return false;
}
