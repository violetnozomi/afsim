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

#include "RvAirCombatVisualization.hpp"

#include <QMenu>

#include "RvAirCombatEventPipeClasses.hpp"
#include "RvEnvironment.hpp"
#include "RvEventPipeUtil.hpp"
#include "RvResultData.hpp"
#include "RvResultPlatform.hpp"
#include "RvTrackDb.hpp"
#include "UtSphericalEarth.hpp"
#include "VaCamera.hpp"
#include "VaCameraMotionTethered.hpp"
#include "WkfAction.hpp"
#include "WkfVtkEnvironment.hpp"

namespace
{
wkf::AirCombatDataEngagementSummary ExtractEngagementSummary(const rv::ResultPlatform& aPlatform, double aSimTime)
{
   wkf::AirCombatDataEngagementSummary data;
   data.platformData.mPlatformName  = aPlatform.GetName();
   data.platformData.mPlatformIndex = aPlatform.GetPlatformIndex();

   auto* platformInfo = aPlatform.FindFirstAfter<rv::MsgPlatformInfo>(aSimTime);
   if (platformInfo != nullptr)
   {
      data.platformData.mPlatformType = platformInfo->types().front();
   }

   return data;
}
} // namespace

WKF_PLUGIN_DEFINE_SYMBOLS(RvAirCombatVisualization::Plugin,
                          "Air Combat Visualization",
                          "The Air Combat Visualization plugin adds various GUI elements to the map and tether views "
                          "that support air-to-air combat analysis.",
                          "mystic",
                          false) // Plugin not loaded by default

RvAirCombatVisualization::Plugin::Plugin(const QString& aName, const size_t aUniqueId)
   : rv::Plugin(aName, aUniqueId)
   , mPrefWidgetPtr(new wkf::AirCombatVisualizationPrefWidget())
   , mSourcePlatformPtr(nullptr)
   , mEnabled(false)
{
   mDisplayInterfacePtr = ut::make_unique<wkf::AirCombatDisplayInterface>(*mPrefWidgetPtr->GetPreferenceObject());

   connect(&wkfEnv, &wkf::Environment::Initialize, this, [&]() { mEnabled = (vaEnv.GetStandardViewer() != nullptr); });

   connect(
      &wkfEnv,
      &wkf::Environment::PlatformSelectionChanged,
      this,
      [&](const wkf::PlatformList& aSelectedPlatforms, const wkf::PlatformList& aUnselectedPlatforms)
      {
         // For each unselected platform, remove it from the sim interface's target list, remove any
         // target blocks in all overlays that correspond to it, and remove all attachments from it.
         bool sourceRemoved = false;
         for (const auto& unselectedPlatform : aUnselectedPlatforms)
         {
            // If the source platform was unselected, remove it from any overlays that may have it
            // as a manual target.
            if (unselectedPlatform == mSourcePlatformPtr)
            {
               sourceRemoved = true;
            }
            else
            {
               // Remove the platform from our list of targets.
               auto target = std::find(mTargetPlatforms.begin(), mTargetPlatforms.end(), unselectedPlatform);
               if (target != mTargetPlatforms.end())
               {
                  mTargetPlatforms.erase(target);
               }

               // Remove the platform from all overlay target lists.
               mDisplayInterfacePtr->RemoveManualTargetFromAllOverlays(*unselectedPlatform);
            }

            // Stop collecting data for this platform by removing it from the data list.
            auto platformEquals = [unselectedPlatform](const wkf::AirCombatDataEngagementSummary& aData)
            { return unselectedPlatform->GetIndex() == aData.platformData.mPlatformIndex; };

            auto foundPlatform = std::find_if(mDataContainer.GetEngagementDataList().begin(),
                                              mDataContainer.GetEngagementDataList().end(),
                                              platformEquals);
            if (foundPlatform != mDataContainer.GetEngagementDataList().end())
            {
               mDataContainer.RemoveEngagementFromDataList(foundPlatform);
            }

            // Remove attachments from the platform.
            mDisplayInterfacePtr->RemoveAttachments(*unselectedPlatform);
         }

         if (sourceRemoved)
         {
            mSourcePlatformPtr = nullptr;
         }

         // If exactly one platform is selected, that platform becomes the new source platform.
         if (aSelectedPlatforms.size() == 1)
         {
            mSourcePlatformPtr = wkfEnv.GetSelectedPlatforms()[0];
         }

         // For each newly selected platform, if it's viable, create attachments for it based on the preference object.
         for (auto& selectedPlatform : aSelectedPlatforms)
         {
            // A platform is considered "viable" if it has any air combat data associated with it (SA processor).
            rv::ResultData* currentData = rvEnv.GetData();
            if (currentData != nullptr)
            {
               rv::ResultPlatform* platform = currentData->FindPlatform(selectedPlatform->GetIndex());
               if (platform != nullptr)
               {
                  // Does this platform have SA Data?
                  if (HasSA_Data(platform, currentData->GetSimTime()))
                  {
                     mDataContainer.AddEngagementToDataList(ExtractEngagementSummary(*platform, currentData->GetSimTime()));

                     // This platform has air combat data, so create attachments for it.
                     if (!mDisplayInterfacePtr->IsPlatformDecorated(*selectedPlatform))
                     {
                        if (selectedPlatform != mSourcePlatformPtr)
                        {
                           mTargetPlatforms.emplace_back(selectedPlatform);
                        }

                        mDisplayInterfacePtr->CreateDecoratorAttachment(*selectedPlatform);

                        if (selectedPlatform != mSourcePlatformPtr && mSourcePlatformPtr != nullptr)
                        {
                           mDisplayInterfacePtr->CreateLineAttachment(*mSourcePlatformPtr,
                                                                      *selectedPlatform,
                                                                      wkf::AirCombatDisplayInterface::InteractionType::cBASE);
                           mDisplayInterfacePtr->CreateLineAttachment(*mSourcePlatformPtr,
                                                                      *selectedPlatform,
                                                                      wkf::AirCombatDisplayInterface::InteractionType::cWEZ);
                           mDisplayInterfacePtr->CreateLineAttachment(*selectedPlatform,
                                                                      *mSourcePlatformPtr,
                                                                      wkf::AirCombatDisplayInterface::InteractionType::cWEZ);
                           mDisplayInterfacePtr->CreateLineAttachment(*mSourcePlatformPtr,
                                                                      *selectedPlatform,
                                                                      wkf::AirCombatDisplayInterface::InteractionType::cDETECTION);
                           mDisplayInterfacePtr->CreateLineAttachment(*selectedPlatform,
                                                                      *mSourcePlatformPtr,
                                                                      wkf::AirCombatDisplayInterface::InteractionType::cDETECTION);

                           // Add the selected platform to the source overlay's targets.
                           mDisplayInterfacePtr->AddManualTargetToOverlay(*mSourcePlatformPtr, *selectedPlatform);
                        }
                     }
                  }
               }
            }
         }
         // Attempt to update the data.
         if (rv::Environment::Exists())
         {
            rv::ResultData* currentData = rvEnv.GetData();
            if (currentData != nullptr)
            {
               AdvanceTimeRead(*currentData);
            }
         }
      });
}

void RvAirCombatVisualization::Plugin::BuildViewerContextMenu(QMenu* aMenu, vespa::VaViewer* aViewerPtr)
{
   UtVariant v = aViewerPtr->GetData("type");
   if (!v.IsUnset())
   {
      std::string type = v.GetString();
      if (type == "immersive")
      {
         vespa::VaCameraMotionTethered* tether =
            dynamic_cast<vespa::VaCameraMotionTethered*>(aViewerPtr->GetCamera()->GetCameraMotion());
         if (tether != nullptr)
         {
            wkf::Platform* platform = dynamic_cast<wkf::Platform*>(tether->GetEntity());
            if (platform != nullptr)
            {
               if (aViewerPtr->FindFirstOverlayOfType<wkf::AirCombatOverlay>() == nullptr)
               {
                  auto addOverlay = new wkf::Action("Add Air Combat Overlay", aMenu);
                  aMenu->addAction(addOverlay);
                  connect(addOverlay,
                          &QAction::triggered,
                          [aViewerPtr, platform, this]()
                          {
                             mDisplayInterfacePtr->CreateOverlay(*platform, *aViewerPtr);

                             if (mSourcePlatformPtr != nullptr)
                             {
                                for (const auto& target : mTargetPlatforms)
                                {
                                   mDisplayInterfacePtr->AddManualTargetToOverlay(*mSourcePlatformPtr, *target);
                                }
                             }
                          });
               }
               else
               {
                  auto addOverlay = new wkf::Action("Remove Air Combat Overlay", aMenu);
                  aMenu->addAction(addOverlay);
                  connect(addOverlay,
                          &QAction::triggered,
                          [aViewerPtr, platform, this]() { mDisplayInterfacePtr->RemoveOverlay(*platform, *aViewerPtr); });
               }
            }
         }
      }
   }
}

void RvAirCombatVisualization::Plugin::GuiUpdate()
{
   mDisplayInterfacePtr->Update(mDataContainer);
}

QList<wkf::PrefWidget*> RvAirCombatVisualization::Plugin::GetPreferencesWidgets() const
{
   QList<wkf::PrefWidget*> prefs;
   prefs << mPrefWidgetPtr;
   return prefs;
}

void RvAirCombatVisualization::Plugin::AdvanceTimeRead(const rv::ResultData& aData)
{
   std::vector<wkf::AirCombatDataEngagementSummary> tempEngagementDataList;

   // Fill in the source platform data, then use the prioritized targets and threats from it
   wkf::AirCombatDataEngagementSummary sourcePlatformData;
   if (mSourcePlatformPtr != nullptr)
   {
      rv::ResultPlatform* sourcePlatform = aData.FindPlatform(mSourcePlatformPtr->GetIndex());
      if (sourcePlatform != nullptr)
      {
         PopulateData(aData, sourcePlatformData, sourcePlatform);
         tempEngagementDataList.emplace_back(sourcePlatformData);
      }
   }

   // Target platforms
   for (auto& targetPlatformPtr : mTargetPlatforms)
   {
      if (targetPlatformPtr != nullptr)
      {
         wkf::AirCombatDataEngagementSummary tempData;
         rv::ResultPlatform*                 targetPlatform = aData.FindPlatform(targetPlatformPtr->GetIndex());
         if (targetPlatform != nullptr)
         {
            PopulateData(aData, tempData, targetPlatform);
            tempEngagementDataList.emplace_back(tempData);
         }
      }
   }

   // Prioritized Targets
   for (auto targetName : sourcePlatformData.engagementData.mPrioritizedTargetNames)
   {
      wkf::AirCombatDataEngagementSummary tempData;
      rv::ResultPlatform*                 targetPlatform = aData.FindPlatform(aData.FindPlatformIndex(targetName));
      if (targetPlatform != nullptr)
      {
         PopulateData(aData, tempData, targetPlatform);
         tempEngagementDataList.emplace_back(tempData);
      }
   }

   // Prioritized Threats
   for (auto threatName : sourcePlatformData.engagementData.mPrioritizedThreatNames)
   {
      wkf::AirCombatDataEngagementSummary tempData;
      rv::ResultPlatform*                 threatPlatform = aData.FindPlatform(aData.FindPlatformIndex(threatName));
      if (threatPlatform != nullptr)
      {
         PopulateData(aData, tempData, threatPlatform);
         tempEngagementDataList.emplace_back(tempData);
      }
   }

   mDataContainer.SetEngagementDataList(tempEngagementDataList);
}

void RvAirCombatVisualization::Plugin::PopulateData(const rv::ResultData&                aData,
                                                    wkf::AirCombatDataEngagementSummary& aTempData,
                                                    rv::ResultPlatform*                  aPlatform)
{
   // Gather platform name, index, and type
   aTempData.platformData.mPlatformName  = aPlatform->GetName();
   aTempData.platformData.mPlatformIndex = aPlatform->GetPlatformIndex();

   auto* platformInfo = aPlatform->FindFirstAfter<rv::MsgPlatformInfo>(aData.GetSimTime());
   if (platformInfo != nullptr)
   {
      aTempData.platformData.mPlatformType = platformInfo->types().front();
   }

   // Gather flight kinematics data
   const rv::MsgSA_FlightKinematicsData* flightData =
      aPlatform->FindFirstBefore<rv::MsgSA_FlightKinematicsData>(aData.GetSimTime());
   if (flightData == nullptr)
   {
      flightData = aPlatform->FindFirstAfter<rv::MsgSA_FlightKinematicsData>(aData.GetSimTime());
   }
   if (flightData != nullptr)
   {
      aTempData.kinematicData.mAltitude_ft       = flightData->altBaro_ft();
      aTempData.kinematicData.mVerticalSpeed_fpm = flightData->vertSpd_fpm();
      aTempData.kinematicData.mKCAS              = flightData->kcas();
      aTempData.kinematicData.mKTAS              = flightData->ktas();
      aTempData.kinematicData.mMach              = flightData->mach();
      aTempData.kinematicData.mNx                = flightData->gX();
      aTempData.kinematicData.mAlpha_deg         = flightData->alpha_deg();
      aTempData.kinematicData.mGLoad             = flightData->gLoad();
      aTempData.kinematicData.mAvailG            = flightData->gAvail();
      aTempData.kinematicData.mHeading_deg       = flightData->heading_deg();
      aTempData.kinematicData.mPitch_deg         = flightData->pitch_deg();
      aTempData.kinematicData.mRoll_deg          = flightData->roll_deg();
      aTempData.signatureData.mAfterburnerOn     = flightData->afterburnerOn();
      aTempData.signatureData.mContrailing       = flightData->contrailing();
   }

   // ..........................................................................................................................

   // Gather fuel data
   const rv::MsgSA_FuelData* fuelData = aPlatform->FindFirstBefore<rv::MsgSA_FuelData>(aData.GetSimTime());
   if (fuelData == nullptr)
   {
      fuelData = aPlatform->FindFirstAfter<rv::MsgSA_FuelData>(aData.GetSimTime());
   }
   if (fuelData != nullptr)
   {
      aTempData.fuelData.mCurrentInternalFuel_lbs  = fuelData->fuelInternal_lbs();
      aTempData.fuelData.mCurrentExternalFuel_lbs  = fuelData->fuelExternal_lbs();
      aTempData.fuelData.mInternalFuelCapacity_lbs = fuelData->fuelCapInt_lbs();
      aTempData.fuelData.mExternalFuelCapacity_lbs = fuelData->fuelCapExt_lbs();
      aTempData.fuelData.mTotalFuel_lbs =
         aTempData.fuelData.mCurrentInternalFuel_lbs + aTempData.fuelData.mCurrentExternalFuel_lbs;
      aTempData.fuelData.mNormalizedFuelLevel = fuelData->normalizedFuelQty();
      aTempData.fuelData.mJokerFuel_lbs       = fuelData->joker_lbs();
      aTempData.fuelData.mBingoFuel_lbs       = fuelData->bingo_lbs();

      if (aTempData.fuelData.mTotalFuel_lbs < aTempData.fuelData.mJokerFuel_lbs)
      {
         aTempData.fuelData.mJokerReached = true;
      }
      else
      {
         aTempData.fuelData.mJokerReached = false;
      }

      if (aTempData.fuelData.mTotalFuel_lbs < aTempData.fuelData.mBingoFuel_lbs)
      {
         aTempData.fuelData.mBingoReached = true;
      }
      else
      {
         aTempData.fuelData.mBingoReached = false;
      }
   }

   // ..........................................................................................................................

   // Gather weapon data
   const rv::MsgSA_WeaponsData* weaponData = aPlatform->FindFirstBefore<rv::MsgSA_WeaponsData>(aData.GetSimTime());
   if (weaponData == nullptr)
   {
      weaponData = aPlatform->FindFirstAfter<rv::MsgSA_WeaponsData>(aData.GetSimTime());
   }
   if (weaponData != nullptr)
   {
      // Note -- This may be used later: weaponData->jammingDetected();

      aTempData.weaponsData.mInitialTotalWeaponCount = weaponData->initialNumWeapons();
      aTempData.signatureData.mWeaponBayDoorsOpen    = weaponData->weaponBayDoorsOpen();
      aTempData.weaponsData.mSelectedWeapon          = weaponData->selectedWeapon();
      aTempData.weaponsData.mSupportingWeapon        = weaponData->supportingWeapon();

      aTempData.weaponsData.mWeaponsQtyMap.clear();
      for (auto& wpn : weaponData->weaponList())
      {
         // Add weapon to the list
         aTempData.weaponsData.mWeaponsQtyMap.emplace(wpn.weaponName(), wpn.quantity());
      }
   }

   // ..........................................................................................................................

   // Gather engagement data
   const rv::MsgSA_EngagementSummaryData* airCombatData =
      aPlatform->FindFirstBefore<rv::MsgSA_EngagementSummaryData>(aData.GetSimTime());
   if (airCombatData == nullptr)
   {
      airCombatData = aPlatform->FindFirstAfter<rv::MsgSA_EngagementSummaryData>(aData.GetSimTime());
   }
   if (airCombatData != nullptr)
   {
      // Emissions
      aTempData.emissionsData.mRadarEmitting        = airCombatData->radarEmitting();
      aTempData.emissionsData.mJammerEmitting       = airCombatData->jammerEmitting();
      aTempData.emissionsData.mOtherSystemsEmitting = airCombatData->otherEmitting();

      // Overall parameters
      aTempData.saData.mMissionTask    = airCombatData->missionTask();
      aTempData.saData.mRiskAcceptable = airCombatData->riskAcceptable();
      aTempData.saData.mRiskCurrent    = airCombatData->riskCurrent();
      aTempData.saData.mSelfRisk       = airCombatData->selfRisk();
      aTempData.saData.mFlightRisk     = airCombatData->flightRisk();
      aTempData.saData.mPackageRisk    = airCombatData->packageRisk();
      aTempData.saData.mMissionRisk    = airCombatData->missionRisk();
      aTempData.saData.mDefensiveness  = airCombatData->defensiveness();
      aTempData.saData.mUrgency        = airCombatData->urgency();

      // Get the list of prioritized target names
      aTempData.engagementData.mPrioritizedTargetNames.clear();
      for (const auto& target : airCombatData->prioritizedTargetList())
      {
         aTempData.engagementData.mPrioritizedTargetNames.emplace_back(target);
      }

      // Get the list of prioritized threat names
      aTempData.engagementData.mPrioritizedThreatNames.clear();
      for (const auto& threat : airCombatData->prioritizedThreatList())
      {
         aTempData.engagementData.mPrioritizedThreatNames.emplace_back(threat);
      }

      // Get the list of tracked platform names
      aTempData.engagementData.mTrackedPlatformNames.clear();
      for (const auto& trackedPlatform : airCombatData->trackedPlatformList())
      {
         aTempData.engagementData.mTrackedPlatformNames.emplace_back(trackedPlatform);
      }

      // Clear any old engagement data
      aTempData.engagementData.mAngleOffMap_deg.clear();
      aTempData.engagementData.mAspectAngleMap_deg.clear();
      aTempData.engagementData.mCurrentRangeMap_nm.clear();
      aTempData.engagementData.mCurrentDeltaAltMap_ft.clear();
      aTempData.engagementData.mTrackQuality.clear();
      aTempData.engagementData.mTargetCID.clear();
      aTempData.engagementData.mTargetThreatLevel.clear();
      aTempData.engagementData.mRawDetectionAgainstTargetMap_nm.clear();
      aTempData.engagementData.mRawWEZAgainstTargetMap_nm.clear();
      aTempData.engagementData.mWEZMinRangeMap_nm.clear();
      aTempData.engagementData.mWEZMaxRangeMap_nm.clear();
      aTempData.engagementData.mWEZNoEscapeMap_nm.clear();
      aTempData.engagementData.mRiskMap.clear();
      aTempData.engagementData.mDefensivenessMap.clear();
      aTempData.engagementData.mUrgencyMap.clear();
      aTempData.engagementData.mTargetLocked.clear();
      aTempData.engagementData.mAcceptableWEZ.clear();

      // Set the data for each engagement
      for (const auto& data : airCombatData->engagementDataList())
      {
         aTempData.engagementData.mAngleOffMap_deg.emplace(data.targetName(), data.angleOff_deg());
         aTempData.engagementData.mAspectAngleMap_deg.emplace(data.targetName(), data.aspectAngle_deg());
         aTempData.engagementData.mCurrentRangeMap_nm.emplace(data.targetName(), data.currentRange_nm());
         aTempData.engagementData.mCurrentDeltaAltMap_ft.emplace(data.targetName(), data.currentDeltaAlt_ft());
         aTempData.engagementData.mTrackQuality.emplace(data.targetName(), data.trackQuality());
         aTempData.engagementData.mTargetCID.emplace(data.targetName(), data.targetCID());
         aTempData.engagementData.mTargetThreatLevel.emplace(data.targetName(), data.threatLevel());
         aTempData.engagementData.mRawDetectionAgainstTargetMap_nm.emplace(data.targetName(), data.detectionRange_nm());
         aTempData.engagementData.mRawWEZAgainstTargetMap_nm.emplace(data.targetName(), data.wezDesiredRange_nm());
         aTempData.engagementData.mWEZMinRangeMap_nm.emplace(data.targetName(), data.wezMinimum_nm());
         aTempData.engagementData.mWEZMaxRangeMap_nm.emplace(data.targetName(), data.wezMaximum_nm());
         aTempData.engagementData.mWEZNoEscapeMap_nm.emplace(data.targetName(), data.wezNoEscape_nm());
         aTempData.engagementData.mRiskMap.emplace(data.targetName(), data.risk());
         aTempData.engagementData.mDefensivenessMap.emplace(data.targetName(), data.defensiveness());
         aTempData.engagementData.mUrgencyMap.emplace(data.targetName(), data.urgency());
         aTempData.engagementData.mTargetLocked.emplace(data.targetName(), data.tgtTracked());
         aTempData.engagementData.mAcceptableWEZ.emplace(data.targetName(), data.acceptableWez());
      }
   }
}

const bool RvAirCombatVisualization::Plugin::HasSA_Data(rv::ResultPlatform* aPlatformPtr, const double aSimTime)
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
