// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "RvAcesDisplayPlugin.hpp"

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

WKF_PLUGIN_DEFINE_SYMBOLS(RvAcesDisplay::Plugin,
                          "Aces Display",
                          "The Aces Display plugin provides a custom-display of SA Processor data, inlcuding "
                          "perception of assets, bogies, and bandits, perceived groups, and truth.",
                          "mystic",
                          false) // Plugin not loaded by default

RvAcesDisplay::Plugin::Plugin(const QString& aPluginName, const size_t aUniqueID)
   : rv::PluginT<wkf::AcesDisplay::PluginBase>(aPluginName, aUniqueID)
{
   // This updates the target platform list, removing any deselected platforms
   // and adding any newly selected ones.
   connect(&wkfEnv,
           &wkf::Environment::PlatformSelectionChanged,
           this,
           [&](const wkf::PlatformList& aSelected, const wkf::PlatformList& aDeselected)
           {
              // Only handle platform selection change if we have a
              // dockwidget open. Handle everything on a base reference
              // list, then set the focus displays list from it.
              if (mDisplayInFocus != 0)
              {
                 // Add any newly selected platforms
                 for (const auto& selectedPlatform : aSelected)
                 {
                    mTargetPlatforms[0].push_back(selectedPlatform->GetIndex());
                 }

                 // Remove any deselected platforms
                 for (const auto& deselectedPlatform : aDeselected)
                 {
                    // Then remove any targets that need removed
                    auto target =
                       std::find(mTargetPlatforms[0].begin(), mTargetPlatforms[0].end(), deselectedPlatform->GetIndex());
                    if (target != mTargetPlatforms[0].end())
                    {
                       mTargetPlatforms[0].erase(target);
                    }
                 }

                 // Set the current displays list to the reference list, and
                 // remove the focus displays index from its own list if found
                 mTargetPlatforms[mDisplayInFocus] = mTargetPlatforms[0];
                 auto focusIndex                   = std::find(mTargetPlatforms[mDisplayInFocus].begin(),
                                             mTargetPlatforms[mDisplayInFocus].end(),
                                             mDisplayInFocus);
                 if (focusIndex != mTargetPlatforms[mDisplayInFocus].end())
                 {
                    mTargetPlatforms[mDisplayInFocus].erase(focusIndex);
                 }

                 // Set the list for the active dockwidget so that
                 // it can draw the engagement data
                 emit SetTargetPlatforms(mTargetPlatforms[mDisplayInFocus]);
              }
           });
}


void RvAcesDisplay::Plugin::Plugin::PlatformAddedRead(double aSimTime, rv::ResultDb& aData, const rv::MsgPlatformInfo& aPlatform)
{
   rv::ResultPlatform* platformPtr = aData.FindPlatform(aPlatform.platformIndex());
   // If we have a valid platform, populate using rv::ResultPlatform* data.
   if (platformPtr != nullptr)
   {
      // Create a temp container.
      wkf::AcesDisplay::EntityTruth entity;

      entity.index = platformPtr->GetPlatformIndex();
      entity.name  = platformPtr->GetName();

      // Set missile flag to false, but it may become true below
      entity.isMissile = false;

      // Get the type from the platform info message, if it exists.
      entity.domain = rv::SpatialDomain::ToString(aPlatform.spatialDomain());

      if (!aPlatform.types().empty())
      {
         entity.type = *aPlatform.types().begin();
      }

      rv::StringList catList = aPlatform.categories();
      for (auto& category : catList)
      {
         if (category == "missile")
         {
            entity.isMissile = true;
            break; // We can quit since we found a missile
         }
      }

      // Use EntityState data to fill the truth data.
      auto entityState = platformPtr->FindFirstBefore<rv::MsgEntityState>(aSimTime);
      if (entityState == nullptr)
      {
         entityState = platformPtr->FindFirstAfter<rv::MsgEntityState>(aSimTime);
      }

      if (entityState != nullptr)
      {
         rv::EntityState state = entityState->state();
         // Create a temporary entity to get the data we need.
         UtEntity tmpEntity;
         tmpEntity.SetLocationWCS(rv::toVec3(state.locationWCS()).GetData());
         tmpEntity.SetOrientationWCS(state.orientationWCS().x(), state.orientationWCS().y(), state.orientationWCS().z());
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
         entity.speed_kts   = tmpEntity.GetSpeed() * UtMath::cNMPH_PER_MPS;

         double lwcs[3];
         tmpEntity.GetLocationWCS(lwcs);
         entity.bearing_deg = tmpEntity.TrueBearing(lwcs) * UtMath::cDEG_PER_RAD;

         entity.entityValid   = true;
         entity.altitudeValid = true;
         entity.speedValid    = true;
         entity.headingValid  = true;
      }

      mDataContainer.AddEntityTruth(entity);
   }
}


void RvAcesDisplay::Plugin::Plugin::CommentRead(rv::ResultDb& aData, const rv::MsgComment& aComment)
{
   rv::ResultPlatform* plat = aData.FindPlatform(aComment.platformIndex());
   if (plat)
   {
      mDataContainer.AddPlatformComment(aComment.platformIndex(), aComment.simTime(), aComment.text());
   }
}

void RvAcesDisplay::Plugin::RegularRead(const rv::ResultData& aData)
{
   AdvanceTimeRead(aData);
}

void RvAcesDisplay::Plugin::AdvanceTimeRead(const rv::ResultData& aData)
{
   UpdateDataContainer(aData);
}

bool RvAcesDisplay::Plugin::HasSituationAwarenessProcessor(const size_t aPlatformIndex)
{
   // It'd be nice to remove this override and swap it for
   // PlatformAddedRead, but the data hasn't fully loaded all messages
   // at the point it gets called, and HasSA_Data will not call
   // correctly, leaving mPlatformsWithSAP empty when it shouldnt be.
   if (rv::Environment::Exists())
   {
      rv::ResultData* currentData = rvEnv.GetData();
      if (currentData != nullptr)
      {
         auto platform = currentData->FindPlatform(aPlatformIndex);
         if (platform != nullptr)
         {
            return HasSA_Data(platform, currentData->GetSimTime());
         }
      }
   }
   return false;
}

bool RvAcesDisplay::Plugin::EntityTruthValid(wkf::AcesDisplay::EntityTruth& aEntityTruth)
{
   auto                currentData = rvEnv.GetData();
   auto                simTime     = currentData->GetSimTime();
   rv::ResultPlatform* platformPtr = currentData->FindPlatform(aEntityTruth.index);
   // If we have a valid platform
   aEntityTruth.entityValid = false;
   if (platformPtr != nullptr)
   {
      aEntityTruth.entityValid = true;

      // If our platform shouldn't exist yet
      if (platformPtr->GetStartTime() > simTime)
      {
         aEntityTruth.entityValid = false;
      }

      // If the platform is deleted
      const auto entityStatus = platformPtr->FindFirstBefore<rv::MsgPlatformStatus>(simTime);
      if (entityStatus != nullptr)
      {
         if (entityStatus->removed())
         {
            aEntityTruth.entityValid = false;
         }
      }
   }
   return aEntityTruth.entityValid;
}

void RvAcesDisplay::Plugin::ClearScenario(bool aFullReset)
{
   Reset();
}

void RvAcesDisplay::Plugin::PopulateEntityPerception(wkf::AcesDisplay::EntityPerception& aEntityPerception,
                                                     const rv::SA_EntityPerception&      aData)
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
   aEntityPerception.identification = static_cast<wkf::AcesDisplay::Identification>(aData.identification());
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
   aEntityPerception.focus          = aData.focus();
   aEntityPerception.isCoasting     = aData.isCoasting();
   aEntityPerception.isImportant    = aData.isImportant();
}

void RvAcesDisplay::Plugin::PopulateEngagementData(const rv::ResultData&             aData,
                                                   wkf::AcesDisplay::EngagementData& aTempData,
                                                   rv::ResultPlatform*               aPlatform)
{
   const rv::MsgSA_EngagementSummaryData* engagementData =
      aPlatform->FindFirstBefore<rv::MsgSA_EngagementSummaryData>(aData.GetSimTime());

   if (engagementData != nullptr)
   {
      // Get the list of prioritized target names
      for (const auto& target : engagementData->prioritizedTargetList())
      {
         aTempData.mPrioritizedTargetNames.emplace_back(target);
      }

      // Get the list of prioritized threat names
      for (const auto& threat : engagementData->prioritizedThreatList())
      {
         aTempData.mPrioritizedThreatNames.emplace_back(threat);
      }

      // Get the list of tracked platform names
      for (const auto& trackedPlatform : engagementData->trackedPlatformList())
      {
         aTempData.mTrackedPlatformNames.emplace_back(trackedPlatform);
      }

      // Set the data for each engagement
      for (const auto& data : engagementData->engagementDataList())
      {
         aTempData.mAngleOffMap_deg.emplace(data.targetIndex(), data.angleOff_deg());
         aTempData.mAspectAngleMap_deg.emplace(data.targetIndex(), data.aspectAngle_deg());
         aTempData.mCurrentRangeMap_nm.emplace(data.targetIndex(), data.currentRange_nm());
         aTempData.mCurrentDeltaAltMap_ft.emplace(data.targetIndex(), data.currentDeltaAlt_ft());
         aTempData.mTrackQuality.emplace(data.targetIndex(), data.trackQuality());
         aTempData.mTargetCID.emplace(data.targetIndex(), data.targetCID());
         aTempData.mTargetThreatLevel.emplace(data.targetIndex(), data.threatLevel());
         aTempData.mRawDetectionAgainstTargetMap_nm.emplace(data.targetIndex(), data.detectionRange_nm());
         aTempData.mRawWEZAgainstTargetMap_nm.emplace(data.targetIndex(), data.wezDesiredRange_nm());
         aTempData.mWEZMinRangeMap_nm.emplace(data.targetIndex(), data.wezMinimum_nm());
         aTempData.mWEZMaxRangeMap_nm.emplace(data.targetIndex(), data.wezMaximum_nm());
         aTempData.mWEZNoEscapeMap_nm.emplace(data.targetIndex(), data.wezNoEscape_nm());
         aTempData.mRiskMap.emplace(data.targetIndex(), data.risk());
         aTempData.mDefensivenessMap.emplace(data.targetIndex(), data.defensiveness());
         aTempData.mUrgencyMap.emplace(data.targetIndex(), data.urgency());
         aTempData.mTargetLocked.emplace(data.targetIndex(), data.tgtTracked());
         aTempData.mAcceptableWEZ.emplace(data.targetIndex(), data.acceptableWez());
      }
   }
}

void RvAcesDisplay::Plugin::PopulateBehaviorHistory(const rv::ResultData&     aData,
                                                    std::vector<std::string>& aBehaviorHistory,
                                                    rv::ResultPlatform*       aPlatform)
{
   // Get the behavior tree state array, and grab the last cBEHAVIOR_HISTORY_COUNT number of node executions for this platform.
   auto btArray = aPlatform->GetArray<rv::MsgBehaviorTreeState>();
   if (btArray != nullptr)
   {
      rv::MsgBehaviorTreeState* msg = nullptr;
      auto                      it  = btArray->FindFirstIteratorBefore(aData.GetSimTime());
      if (it != btArray->end())
      {
         msg = dynamic_cast<rv::MsgBehaviorTreeState*>(*it);
      }
      // While we still have behaviors to grab or we've hit the beginning of the list
      while (aBehaviorHistory.size() < wkf::AcesDisplay::cBEHAVIOR_HISTORY_COUNT && msg != nullptr)
      {
         auto executedNames = msg->behaviorHistoryList();
         for (auto& names : executedNames)
         {
            aBehaviorHistory.push_back(names);
            if (aBehaviorHistory.size() >= wkf::AcesDisplay::cBEHAVIOR_HISTORY_COUNT)
            {
               break;
            }
         }

         if (it == btArray->begin())
         {
            break;
         }

         msg = dynamic_cast<rv::MsgBehaviorTreeState*>(*(--it));
      }
   }
}

void RvAcesDisplay::Plugin::UpdateDataContainer(const rv::ResultData& aData)
{
   // Get the current time
   double simTime = aData.GetSimTime();

   // Only update the data container if we have an Aces display to update.
   if (mDisplays.size() > 0)
   {
      // Create a temp container for the ACES data.
      std::map<size_t, wkf::AcesDisplay::AcesData> tempAcesData;

      // For each ACES display, update the ACES data.
      for (auto display : mDisplays)
      {
         // Get the platform from the result data.
         rv::ResultPlatform* platform = aData.FindPlatform(display.first);

         // Create a temporary data container
         wkf::AcesDisplay::AcesData tempData;

         // If we found the platform, fill in the data.
         if (platform != nullptr)
         {
            // Platform Status Message
            const rv::MsgPlatformStatus* platformStatusData = platform->FindFirstBefore<rv::MsgPlatformStatus>(simTime);

            // Check for a broken or removed platform here, and activate the kill frame if so.
            if (platformStatusData != nullptr)
            {
               if (platformStatusData->broken() || platformStatusData->removed())
               {
                  display.second->ActivateKillFrame(true);
               }
            }
            else
            {
               display.second->ActivateKillFrame(false);
            }

            // Set the time.
            tempData.platformData.simTime = simTime;

            // Use EntityState data first.  If there is other data, it will override with data below.
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

            PopulateBehaviorHistory(aData, tempData.behaviorHistory, platform);

            // Most data will come from the SA messages
            const rv::MsgSA_FlightKinematicsData* flightData =
               platform->FindFirstBefore<rv::MsgSA_FlightKinematicsData>(aData.GetSimTime());
            const rv::MsgSA_FlightControlsData* controlsData =
               platform->FindFirstBefore<rv::MsgSA_FlightControlsData>(aData.GetSimTime());
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
            const rv::MsgSA_EngagementSummaryData* engagementData =
               platform->FindFirstBefore<rv::MsgSA_EngagementSummaryData>(aData.GetSimTime());

            // Set the data (will override any data from above)
            // If we have valid flight data, fill it in.
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

            if (controlsData != nullptr)
            {
               tempData.flightControlsData.platformIndex           = controlsData->platformIndex();
               tempData.flightControlsData.stickRgtInput           = controlsData->stickRgtInput();
               tempData.flightControlsData.stickBackInput          = controlsData->stickBackInput();
               tempData.flightControlsData.rudderRgtInput          = controlsData->rudderRgtInput();
               tempData.flightControlsData.speedBrakeInput         = controlsData->speedBrakeInput();
               tempData.flightControlsData.throttleInput           = controlsData->throttleInput();
               tempData.flightControlsData.aileronRgtNormalized    = controlsData->aileronRgtNormalized();
               tempData.flightControlsData.aileronLftNormalized    = controlsData->aileronLftNormalized();
               tempData.flightControlsData.flapRgtNormalized       = controlsData->flapRgtNormalized();
               tempData.flightControlsData.flapLftNormalized       = controlsData->flapLftNormalized();
               tempData.flightControlsData.stabilizerRgtNormalized = controlsData->stabilizerRgtNormalized();
               tempData.flightControlsData.stabilizerLftNormalized = controlsData->stabilizerLftNormalized();
               tempData.flightControlsData.rudderRgtNormalized     = controlsData->rudderRgtNormalized();
               tempData.flightControlsData.rudderLftNormalized     = controlsData->rudderLftNormalized();
               tempData.flightControlsData.rudderNormalized        = controlsData->rudderNormalized();
               tempData.flightControlsData.speedBrakeNormalized    = controlsData->speedBrakeNormalized();
               tempData.flightControlsData.thrustNormalized        = controlsData->thrustNormalized();
               tempData.flightControlsData.landingGear             = controlsData->landingGear();
               tempData.flightControlsData.throttleValid           = controlsData->throttleValid();
               tempData.flightControlsData.aileronsValid           = controlsData->aileronsValid();
               tempData.flightControlsData.flapsValid              = controlsData->flapsValid();
               tempData.flightControlsData.twinRuddersValid        = controlsData->twinRuddersValid();
               tempData.flightControlsData.singleRudderValid       = controlsData->singleRudderValid();
            }
            else
            {
               tempData.flightControlsData.platformIndex           = 0;
               tempData.flightControlsData.stickRgtInput           = 0.0;
               tempData.flightControlsData.stickBackInput          = 0.0;
               tempData.flightControlsData.rudderRgtInput          = 0.0;
               tempData.flightControlsData.speedBrakeInput         = 0.0;
               tempData.flightControlsData.throttleInput           = 0.0;
               tempData.flightControlsData.aileronRgtNormalized    = 0.0;
               tempData.flightControlsData.aileronLftNormalized    = 0.0;
               tempData.flightControlsData.flapRgtNormalized       = 0.0;
               tempData.flightControlsData.flapLftNormalized       = 0.0;
               tempData.flightControlsData.stabilizerRgtNormalized = 0.0;
               tempData.flightControlsData.stabilizerLftNormalized = 0.0;
               tempData.flightControlsData.rudderRgtNormalized     = 0.0;
               tempData.flightControlsData.rudderLftNormalized     = 0.0;
               tempData.flightControlsData.rudderNormalized        = 0.0;
               tempData.flightControlsData.speedBrakeNormalized    = 0.0;
               tempData.flightControlsData.thrustNormalized        = 0.0;
               tempData.flightControlsData.landingGear             = 0.0;
               tempData.flightControlsData.throttleValid           = false;
               tempData.flightControlsData.aileronsValid           = false;
               tempData.flightControlsData.flapsValid              = false;
               tempData.flightControlsData.twinRuddersValid        = false;
               tempData.flightControlsData.singleRudderValid       = false;
            }

            // If we have valid nav data, fill it in.
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
                  wkf::AcesDisplay::WaypointLatLon wayPt;
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

            // If we have valid fuel data, fill it in.
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

            // If we have valid weapons data, fill it in.
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
                     wkf::AcesDisplay::WeaponNameQty weapon;
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
                     wkf::AcesDisplay::EngagedTargetData target;

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

            if (engagementData != nullptr)
            {
               // Populate the source platform data
               wkf::AcesDisplay::EngagementData sourcePlatformData;
               PopulateEngagementData(aData, sourcePlatformData, platform);
               tempData.engagementData.emplace(platform->GetPlatformIndex(), sourcePlatformData);


               // Get the indices to gather info for from each targets list of targets
               std::set<size_t> targetPlatforms;
               for (const auto& target : mTargetPlatforms)
               {
                  for (const auto& targetIndex : target.second)
                  {
                     targetPlatforms.emplace(targetIndex);
                  }
               }

               // Target platforms
               for (const auto& targetIndex : targetPlatforms)
               {
                  wkf::AcesDisplay::EngagementData targetData;
                  rv::ResultPlatform*              targetPlatform = aData.FindPlatform(targetIndex);
                  if (targetPlatform != nullptr)
                  {
                     PopulateEngagementData(aData, targetData, targetPlatform);
                     tempData.engagementData.emplace(targetIndex, targetData);
                  }
               }

               // Prioritized Targets
               for (auto targetName : sourcePlatformData.mPrioritizedTargetNames)
               {
                  wkf::AcesDisplay::EngagementData targetData;
                  auto                             targetIndex    = aData.FindPlatformIndex(targetName);
                  rv::ResultPlatform*              targetPlatform = aData.FindPlatform(targetIndex);
                  if (targetPlatform != nullptr)
                  {
                     PopulateEngagementData(aData, targetData, targetPlatform);
                     tempData.engagementData.emplace(targetIndex, targetData);
                  }
               }

               // Prioritized Threats
               for (auto threatName : sourcePlatformData.mPrioritizedThreatNames)
               {
                  wkf::AcesDisplay::EngagementData threatData;
                  auto                             threatIndex    = aData.FindPlatformIndex(threatName);
                  rv::ResultPlatform*              threatPlatform = aData.FindPlatform(threatIndex);
                  if (threatPlatform != nullptr)
                  {
                     PopulateEngagementData(aData, threatData, threatPlatform);
                     tempData.engagementData.emplace(threatIndex, threatData);
                  }
               }
            }

            if (trackData != nullptr)
            {
               // Loop through the track processors
               for (auto& trkProc : trackData->trackProcessors())
               {
                  // Temporary track processor (tp)
                  wkf::AcesDisplay::TrackProcessor tp;

                  // Set the name
                  tp.trackProcessorName = trkProc.trackProcessorName();

                  // Set the master flag
                  tp.isMasterProcessor = trkProc.isMasterProcessor();

                  for (auto& trk : trkProc.tracks())
                  {
                     wkf::AcesDisplay::TargetTrackItem track;

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
                        static_cast<wkf::AcesDisplay::Identification>(static_cast<int>(trk.identification()));
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

                     // Add to the list of tracks for this track processor
                     tp.trackList.mTracks.emplace(track.tgtPlatformIndex, track);
                  }

                  // If this is the MTP, save the data into the masterTrackListData
                  if (tp.isMasterProcessor)
                  {
                     tempData.masterTrackListData = tp.trackList;
                  }

                  // Add this track processor to the list
                  tempData.trackProcessorList.emplace_back(tp);
               }

               tempData.esmTrackprocessor   = trackData->esmTrackProcessor();
               tempData.mwsTrackprocessor   = trackData->mwsTrackProcessor();
               tempData.radarTrackprocessor = trackData->radarTrackProcessor();
               tempData.irstTrackprocessor  = trackData->irstTrackProcessor();
               tempData.dasTrackprocessor   = trackData->dasTrackProcessor();
               tempData.flirTrackprocessor  = trackData->flirTrackProcessor();
               tempData.eyesTrackprocessor  = trackData->eyesTrackProcessor();
            }

            // If we have valid asset data, fill it in.
            if (assetsData != nullptr)
            {
               for (auto& asset : assetsData->assetsList())
               {
                  wkf::AcesDisplay::EntityPerception entity;
                  PopulateEntityPerception(entity, asset);

                  // Add to the list
                  tempData.assetsList.emplace_back(entity);
               }
            }

            // If we have valid bogie and bandit data, fill it in.
            if (bogiesAndBanditsData != nullptr)
            {
               // Perceived Bogies
               for (auto& bogie : bogiesAndBanditsData->bogiesList())
               {
                  wkf::AcesDisplay::EntityPerception entity;
                  PopulateEntityPerception(entity, bogie);

                  // Add to the list
                  tempData.bogiesList.emplace_back(entity);
               }

               // Perceived Bandits

               for (auto& bandit : bogiesAndBanditsData->banditsList())
               {
                  wkf::AcesDisplay::EntityPerception entity;
                  PopulateEntityPerception(entity, bandit);

                  // Add to the list
                  tempData.banditsList.emplace_back(entity);
               }
            }

            // If we have valid priority data, fill it in.
            if (prioritizedThreatsAndTargetsData != nullptr)
            {
               // Prioritized Threats
               for (auto& threat : prioritizedThreatsAndTargetsData->prioritizedThreatsList())
               {
                  wkf::AcesDisplay::EntityPerception entity;
                  PopulateEntityPerception(entity, threat);

                  // Add to the list
                  tempData.prioritizedThreatsList.emplace_back(entity);
               }

               // Prioritized Targets
               for (auto& target : prioritizedThreatsAndTargetsData->prioritizedTargetsList())
               {
                  wkf::AcesDisplay::EntityPerception entity;
                  PopulateEntityPerception(entity, target);

                  // Add to the list
                  tempData.prioritizedTargetsList.emplace_back(entity);
               }
            }

            // If we have valid group data, fill it in.
            if (groupsData != nullptr)
            {
               for (auto& group : groupsData->groupList())
               {
                  wkf::AcesDisplay::PerceivedGroup perceivedGroup;

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
                     wkf::AcesDisplay::EntityPerception entity;
                     PopulateEntityPerception(entity, target);

                     // Add to the list
                     perceivedGroup.elementList.emplace_back(entity);
                  }

                  perceivedGroup.focus       = group.focus();
                  perceivedGroup.isImportant = group.isImportant();

                  // Add to the list
                  tempData.groupsList.emplace_back(perceivedGroup);
               }
            }

            // Add this platform's temp data to the data container
            tempAcesData.emplace(display.first, tempData);
         } // platform != nullptr
      }    // for (auto display : mDisplays)

      // Update our ACES data
      mDataContainer.SetAcesData(tempAcesData);

   } // if (mDisplays.size() > 0)
}

const bool RvAcesDisplay::Plugin::HasSA_Data(rv::ResultPlatform* aPlatformPtr, const double aSimTime)
{
   // If the plaform has any SA messages, then it has SA data.  Check if the platform has SA messages.
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
