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
#include "WsfSA_Perceive.hpp"

#include <numeric>

#include "UtScriptEnvironment.hpp"
#include "UtSpeed.hpp"
#include "WsfAirCombatObserver.hpp"
#include "WsfCommandChain.hpp"
#include "WsfExplicitWeapon.hpp"
#include "WsfSA_Assess.hpp"
#include "WsfSA_EntityMessage.hpp"
#include "WsfSA_EntityPerception.hpp"
#include "WsfSA_GroupUtils.hpp"
#include "WsfSA_Processor.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrackList.hpp"
#include "WsfTrackListFwd.hpp"
#include "WsfTrackManager.hpp"
#include "WsfTrackProcessor.hpp"
#include "script/WsfScriptMessageHandler.hpp"

WsfSA_Perceive::WsfSA_Perceive(WsfSA_Processor* aSA_ProcPtr, WsfSA_Processor::PerceiveData* aDataPtr)
   : WsfSA_Module(aSA_ProcPtr, aDataPtr)
   , mGroupManager()
{
}

// mDeferredForDeletion and mRandom are intentionally not copied.
WsfSA_Perceive::WsfSA_Perceive(const WsfSA_Perceive& aSrc)
   : WsfSA_Module(aSrc)
   , mConsiderationRandomness(aSrc.mConsiderationRandomness)
   , mGroupManager()
   , mMaxGroupingDistanceCentroid(aSrc.mMaxGroupingDistanceCentroid)
   , mMaxGroupingDistanceNeighbor(aSrc.mMaxGroupingDistanceNeighbor)
   , mMaxGroupingHeadingDifference(aSrc.mMaxGroupingHeadingDifference)
   , mMaxGroupingSpeedDifference(aSrc.mMaxGroupingSpeedDifference)
   , mMinGroupRadius(aSrc.mMinGroupRadius)
   , mUseCentroidGrouping(aSrc.mUseCentroidGrouping)
   , mUseNeighborGrouping(aSrc.mUseNeighborGrouping)
   , mUseHeadingGrouping(aSrc.mUseHeadingGrouping)
   , mUseSpeedGrouping(aSrc.mUseSpeedGrouping)
   , mUseTypeGrouping(aSrc.mUseTypeGrouping)
   , mIgnoreBogiesWhenGrouping(aSrc.mIgnoreBogiesWhenGrouping)
{
}

WsfSA_Perceive::~WsfSA_Perceive()
{
   // Make sure that everything that was supposed to be deleted, is
   DeferredDelete();
}

void WsfSA_Perceive::AddExtraCognitiveLoading(double aTimeDelay)
{
   mDataPtr->mExtraCognitiveDelaysThreats.push_back(aTimeDelay);
   mDataPtr->mExtraCognitiveDelaysAssets.push_back(aTimeDelay);
}

bool WsfSA_Perceive::AssetScore(double aSimTime, WsfSA_EntityPerception* aAssetPtr, double& aScore)
{
   // Return false if we lack a script
   if (mDataPtr->mAssetConsiderationScoringScriptPtr == nullptr)
   {
      return false;
   }

   try
   {
      UtScriptData     scriptRetVal;
      UtScriptDataList scriptArgs;
      scriptArgs.emplace_back(
         new UtScriptRef(const_cast<WsfSA_EntityPerception*>(aAssetPtr), mDataPtr->mSA_EntityPerceptionClassPtr));

      GetScriptContext().ExecuteScript(aSimTime, mDataPtr->mAssetConsiderationScoringScriptPtr, scriptRetVal, scriptArgs);
      aScore = scriptRetVal.GetDouble();
      return true;
   }
   catch (std::exception& e)
   {
      auto out = ut::log::error() << "Exception thrown in WsfSA_Perceive::AssetScore.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Type: " << ut::TypeNameOf(e);
      out.AddNote() << "What: " << e.what();
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      return false;
   }
}

bool WsfSA_Perceive::EntityIsInAssetGroup(const WsfSA_EntityPerception* aEntity) const
{
   for (auto& entity : mDataPtr->mPerceivedAssets)
   {
      if (entity->GetPerceivedIndex() == aEntity->GetPerceivedIndex())
      {
         return true;
      }
   }

   return false;
}

bool WsfSA_Perceive::EntityIsInAssetGroup(size_t aEntityIndex) const
{
   for (auto& entity : mDataPtr->mPerceivedAssets)
   {
      if (entity->GetPerceivedIndex() == aEntityIndex)
      {
         return true;
      }
   }

   return false;
}

bool WsfSA_Perceive::Important(WsfLocalTrack* aThreatPtr)
{
   if (aThreatPtr == nullptr)
   {
      return false;
   }

   // Check if this specific track has been marked as important
   if (mDataPtr->mImportantThreats.find(aThreatPtr->GetTargetIndex()) != mDataPtr->mImportantThreats.end())
   {
      return true;
   }

   // Check if the track's type has been marked as important, use perceived type if possible
   WsfStringId tgtType = (aThreatPtr->TypeIdValid()) ? (aThreatPtr->GetTypeId()) : (aThreatPtr->GetTargetType());
   if (mDataPtr->mImportantThreatTypes.find(tgtType) != mDataPtr->mImportantThreatTypes.end())
   {
      return true;
   }

   return false;
}

bool WsfSA_Perceive::Important(WsfPlatform* aAssetPtr)
{
   if (aAssetPtr == nullptr)
   {
      return false;
   }

   // Check if this specific platform has been marked as important
   if (mDataPtr->mImportantAssets.find(aAssetPtr->GetIndex()) != mDataPtr->mImportantAssets.end())
   {
      return true;
   }

   // Check if the platform's type has been marked as important
   WsfStringId key = aAssetPtr->GetType();
   if (mDataPtr->mImportantAssetTypes.find(key) != mDataPtr->mImportantAssetTypes.end())
   {
      return true;
   }

   return false;
}

bool WsfSA_Perceive::Important(size_t aPlatformIndex)
{
   return Important(GetSimulation()->GetPlatformByIndex(aPlatformIndex));
}

void WsfSA_Perceive::LocalTrackDropped(double aSimTime, const WsfLocalTrack* aLocalTrackPtr)
{
   // Remove track from mTrackDelayQueue (perception delay in progress)
   auto it = mTrackDelayQueue.find(aLocalTrackPtr->GetTrackId());
   if (it != mTrackDelayQueue.end())
   {
      mTrackDelayQueue.erase(it);
   }

   bool entityFound = false;

   // Check assets first.
   for (auto& asset : mDataPtr->mPerceivedAssets)
   {
      if (asset->GetTrackPtr() == aLocalTrackPtr)
      {
         entityFound = true;
         if (mDataPtr->mAssetCoastTime > 0)
         {
            if (!asset->GetIsMissile() && !asset->GetIsNonTrackAsset())
            {
               asset->SetIsCoasting(true);
            }
         }
         asset->SetTrackPtr(nullptr);
         break;
      }
   }

   if (!entityFound)
   {
      // The tracked entity wasn't an asset, so check bandits next.
      for (auto& bandit : mDataPtr->mPerceivedBandits)
      {
         if (bandit->GetTrackPtr() == aLocalTrackPtr)
         {
            entityFound = true;
            if (mDataPtr->mBanditCoastTime > 0)
            {
               bandit->SetIsCoasting(true);
            }
            bandit->SetTrackPtr(nullptr);
            break;
         }
      }

      if (!entityFound)
      {
         // The tracked entity wasn't an asset or a bandit, so check bogies.
         for (auto& bogey : mDataPtr->mPerceivedBogies)
         {
            if (bogey->GetTrackPtr() == aLocalTrackPtr)
            {
               if (mDataPtr->mBogeyCoastTime > 0)
               {
                  bogey->SetIsCoasting(true);
               }
               bogey->SetTrackPtr(nullptr);
               break;
            }
         }
      }
   }
}

void WsfSA_Perceive::MarkAssetAsImportant(WsfPlatform& aAsset)
{
   MarkAssetAsImportant(aAsset.GetIndex());
}

void WsfSA_Perceive::MarkAssetAsImportant(size_t aIndex)
{
   mDataPtr->mImportantAssets[aIndex] = 1;     // Add to important list
   mDataPtr->mUnimportantAssets.erase(aIndex); // Remove from unimportant list
}

void WsfSA_Perceive::MarkAssetAsUnimportant(WsfPlatform& aAsset)
{
   MarkAssetAsUnimportant(aAsset.GetIndex());
}

void WsfSA_Perceive::MarkAssetAsUnimportant(size_t aIndex)
{
   mDataPtr->mUnimportantAssets[aIndex] = 1; // Add to unimportant list
   mDataPtr->mImportantAssets.erase(aIndex); // Remove from important list
}

void WsfSA_Perceive::MarkAssetTypeAsImportant(const std::string& aType)
{
   mDataPtr->mImportantAssetTypes[aType] = 1;
   mDataPtr->mUnimportantAssetTypes.erase(aType);
}

void WsfSA_Perceive::MarkAssetTypeAsUnimportant(const std::string& aType)
{
   mDataPtr->mUnimportantAssetTypes[aType] = 1;
   mDataPtr->mImportantAssetTypes.erase(aType);
}

void WsfSA_Perceive::MarkThreatAsImportant(WsfLocalTrack& aThreat)
{
   mDataPtr->mImportantThreats[aThreat.GetTargetIndex()] = 1;     // Add to important list
   mDataPtr->mUnimportantThreats.erase(aThreat.GetTargetIndex()); // Remove from unimportant list
}

void WsfSA_Perceive::MarkThreatAsUnimportant(WsfLocalTrack& aThreat)
{
   mDataPtr->mUnimportantThreats[aThreat.GetTargetIndex()] = 1; // Add to unimportant list
   mDataPtr->mImportantThreats.erase(aThreat.GetTargetIndex()); // Remove from important list
}

void WsfSA_Perceive::MarkThreatTypeAsImportant(const std::string& aType)
{
   mDataPtr->mImportantThreatTypes[aType] = 1;
   mDataPtr->mUnimportantThreatTypes.erase(aType);
}

void WsfSA_Perceive::MarkThreatTypeAsUnimportant(const std::string& aType)
{
   mDataPtr->mUnimportantThreatTypes[aType] = 1;
   mDataPtr->mImportantThreatTypes.erase(aType);
}

const WsfSA_EntityPerception* WsfSA_Perceive::PerceivedAsset(size_t aPlatformIndex) const
{
   for (auto& perceivedAsset : mDataPtr->mPerceivedAssets)
   {
      if (perceivedAsset->GetEntityPlatformIndex() == aPlatformIndex)
      {
         return perceivedAsset;
      }
   }
   return nullptr;
}

const std::vector<WsfSA_EntityPerception*>& WsfSA_Perceive::PerceivedAssets(double aSimTime)
{
   if (GetSAP()->GetUpdateInterval(WsfSA_Processor::UpdateType::cASSETS_DATA) <= 0)
   {
      // No events are updating the 'mPerceivedAssets', we need to fill it each query here
      UpdateAssets(aSimTime);
   }
   return mDataPtr->mPerceivedAssets;
}

const std::vector<WsfSA_EntityPerception*>& WsfSA_Perceive::PerceivedAircraftAssets(double aSimTime)
{
   if (GetSAP()->GetUpdateInterval(WsfSA_Processor::UpdateType::cASSETS_DATA) <= 0)
   {
      // No events are updating the 'mPerceivedAircraftAssets', we need to fill it each query here
      UpdateAssets(aSimTime);
   }
   return mDataPtr->mPerceivedAircraftAssets;
}

const std::vector<WsfSA_EntityPerception*>& WsfSA_Perceive::PerceivedMissileAssets(double aSimTime)
{
   if (GetSAP()->GetUpdateInterval(WsfSA_Processor::UpdateType::cASSETS_DATA) <= 0)
   {
      // No events are updating the 'mPerceivedMissileAssets', we need to fill it each query here
      UpdateAssets(aSimTime);
   }
   return mDataPtr->mPerceivedMissileAssets;
}

const std::vector<WsfSA_EntityPerception*>& WsfSA_Perceive::PerceivedBogies(double aSimTime)
{
   return mDataPtr->mPerceivedBogies;
}

const std::vector<WsfSA_EntityPerception*>& WsfSA_Perceive::PerceivedAircraftBogies(double aSimTime)
{
   return mDataPtr->mPerceivedAircraftBogies;
}

const std::vector<WsfSA_EntityPerception*>& WsfSA_Perceive::PerceivedMissileBogies(double aSimTime)
{
   return mDataPtr->mPerceivedMissileBogies;
}

const std::vector<WsfSA_EntityPerception*>& WsfSA_Perceive::PerceivedBandits(double aSimTime)
{
   return mDataPtr->mPerceivedBandits;
}

const std::vector<WsfSA_EntityPerception*>& WsfSA_Perceive::PerceivedAircraftBandits(double aSimTime)
{
   return mDataPtr->mPerceivedAircraftBandits;
}

const std::vector<WsfSA_EntityPerception*>& WsfSA_Perceive::PerceivedMissileBandits(double aSimTime)
{
   return mDataPtr->mPerceivedMissileBandits;
}

WsfLocalTrack* WsfSA_Perceive::PerceivedThreat(WsfStringId aTargetNameId)
{
   auto it = mDataPtr->mPerceivedBandits.begin();
   for (; it != mDataPtr->mPerceivedBandits.end(); ++it)
   {
      if ((*it)->GetTrackPtr()->GetTargetName() == aTargetNameId)
      {
         return (*it)->GetTrackPtr();
      }
   }
   return nullptr;
}

WsfLocalTrack* WsfSA_Perceive::PerceivedThreat(const WsfTrackId& aTrackId)
{
   auto it = mDataPtr->mPerceivedBandits.begin();
   for (; it != mDataPtr->mPerceivedBandits.end(); ++it)
   {
      if ((*it)->GetTrackPtr()->GetTrackId() == aTrackId)
      {
         return (*it)->GetTrackPtr();
      }
   }
   return nullptr;
}

bool WsfSA_Perceive::PossibleEnemy(WsfLocalTrack* aTrackPtr)
{
   if (aTrackPtr->TypeIdValid())
   {
      if (GetSAP()->IsEnemyType(aTrackPtr->GetTypeId().GetString()))
      {
         return true;
      }
      if (GetSAP()->IsFriendlyType(aTrackPtr->GetTypeId().GetString()))
      {
         return false;
      }
      if (GetSAP()->IsNeutralType(aTrackPtr->GetTypeId().GetString()))
      {
         return false;
      }
   }

   if (aTrackPtr->SideIdValid())
   {
      if (GetSAP()->IsEnemySide(aTrackPtr->GetSideId().GetString()))
      {
         return true;
      }
      if (GetSAP()->IsFriendlySide(aTrackPtr->GetSideId().GetString()))
      {
         return false;
      }
      if (GetSAP()->IsNeutralSide(aTrackPtr->GetSideId().GetString()))
      {
         return false;
      }
   }

   if (GetSAP()->UseIFF() && aTrackPtr->IFF_StatusValid())
   {
      if (aTrackPtr->GetIFF_Status() == WsfTrack::cIFF_FOE)
      {
         return true;
      }
      if (aTrackPtr->GetIFF_Status() == WsfTrack::cIFF_FRIEND)
      {
         return false;
      }
      if (aTrackPtr->GetIFF_Status() == WsfTrack::cIFF_NEUTRAL)
      {
         return false;
      }
   }

   return true;
}

bool WsfSA_Perceive::ProcessInput(UtInput& aInput)
{
   bool        myCommand(true);
   std::string command = aInput.GetCommand();

   if (command == "reporting_self" || command == "reports_self")
   {
      aInput.ReadValue(mDataPtr->mReportingSelfStatus);
   }
   else if (command == "reporting_others" || command == "reports_others")
   {
      aInput.ReadValue(mDataPtr->mReportingOthersStatus);
   }
   else if (command == "asset_perception")
   {
      std::string pType;
      aInput.ReadValue(pType);
      if (pType == "status_messages")
      {
         mDataPtr->mAssetPerception = GetSAP()->STATUS_MESSAGES;
      }
      else if (pType == "truth")
      {
         mDataPtr->mAssetPerception = GetSAP()->TRUTH;
         std::string setString;
         aInput.ReadValue(
            setString); // Colon separated lists of which sets to include in perception (commander:peers:subordinates)
         UtStringUtil::ToLower(setString); // Shouldn't mess with the colons???
         std::vector<std::string> sets;
         UtStringUtil::Parse(setString, sets, ':');
         while (!sets.empty())
         {
            std::string set = sets.back();
            if (set == "commander")
            {
               mDataPtr->mAssetTruthPerceptionMask |= cCOMMANDER;
            }
            else if (set == "peers")
            {
               mDataPtr->mAssetTruthPerceptionMask |= cPEERS;
            }
            else if (set == "subordinates")
            {
               mDataPtr->mAssetTruthPerceptionMask |= cSUBORDINATES;
            }
            else if (set == "all_commanders")
            {
               mDataPtr->mAssetTruthPerceptionMask |= cALLCOMMANDERS;
            }
            else if (set == "all_subordinates")
            {
               mDataPtr->mAssetTruthPerceptionMask |= cALLSUBORDINATES;
            }
            sets.pop_back();
         }
      }
      else
      {
         throw UtInput::BadValue(aInput, "unknown asset_perception: " + pType);
      }
   }
   else if (command == "asset_ignore")
   {
      std::string category;
      aInput.ReadValue(category);
      mDataPtr->mIgnoredAssetCategories.JoinCategory(WsfStringId(category));
   }
   else if (command == "perceive_self")
   {
      aInput.ReadValue(mDataPtr->mPerceiveSelfStatus);
   }
   else if (command == "max_threat_load")
   {
      aInput.ReadValue(mDataPtr->mMaxThreatLoadInitial);
      mDataPtr->mMaxThreatLoad = mDataPtr->mMaxThreatLoadInitial;
   }
   else if (command == "max_asset_load")
   {
      aInput.ReadValue(mDataPtr->mMaxAssetLoadInitial);
      mDataPtr->mMaxAssetLoad = mDataPtr->mMaxAssetLoadInitial;
   }
   else if (command == "asset_coast_time")
   {
      aInput.ReadValueOfType(mDataPtr->mAssetCoastTime, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mDataPtr->mAssetCoastTime, 0.0);
   }
   else if (command == "bandit_coast_time")
   {
      aInput.ReadValueOfType(mDataPtr->mBanditCoastTime, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mDataPtr->mBanditCoastTime, 0.0);
   }
   else if (command == "bogey_coast_time")
   {
      aInput.ReadValueOfType(mDataPtr->mBogeyCoastTime, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mDataPtr->mBogeyCoastTime, 0.0);
   }
   else if (command == "use_simple_countermeasures")
   {
      bool value = aInput.ReadBool();
      if (value == true)
      {
         mDataPtr->mUseSimpleCM = true;
      }
      else
      {
         mDataPtr->mUseSimpleCM = false;
      }
   }
   else if (command == "num_chaff")
   {
      int num = 0;
      aInput.ReadValue(num);
      mDataPtr->mNumSimpleChaff = static_cast<size_t>(num);
   }
   else if (command == "num_flares")
   {
      int num = 0;
      aInput.ReadValue(num);
      mDataPtr->mNumSimpleFlare = static_cast<size_t>(num);
   }
   else if (command == "num_decoys")
   {
      int num = 0;
      aInput.ReadValue(num);
      mDataPtr->mNumSimpleDecoy = static_cast<size_t>(num);
   }
   else if (command == "filter_assets_from_bogies")
   {
      aInput.ReadValue(mDataPtr->mFilterAssetsFromBogies);
   }
   else if (command == "consideration_score_randomness")
   {
      aInput.ReadValue(mConsiderationRandomness);
   }
   else if (command == "ignore_bogies_when_grouping")
   {
      aInput.ReadValue(mIgnoreBogiesWhenGrouping);
   }
   else if (command == "max_grouping_distance_centroid")
   {
      aInput.ReadValueOfType(mMaxGroupingDistanceCentroid, UtInput::ValueType::cLENGTH);
   }
   else if (command == "max_grouping_distance_neighbor")
   {
      aInput.ReadValueOfType(mMaxGroupingDistanceNeighbor, UtInput::ValueType::cLENGTH);
   }
   else if (command == "max_grouping_heading_difference")
   {
      aInput.ReadValueOfType(mMaxGroupingHeadingDifference, UtInput::ValueType::cANGLE);
   }
   else if (command == "max_grouping_speed_difference")
   {
      aInput.ReadValueOfType(mMaxGroupingSpeedDifference, UtInput::ValueType::cSPEED);
      mMaxGroupingSpeedDifference *= UtMath::cNMPH_PER_MPS;
   }
   else if (command == "min_group_radius")
   {
      aInput.ReadValueOfType(mMinGroupRadius, UtInput::ValueType::cLENGTH);
   }
   else if (command == "use_centroid_grouping")
   {
      aInput.ReadValue(mUseCentroidGrouping);
   }
   else if (command == "use_neighbor_grouping")
   {
      aInput.ReadValue(mUseNeighborGrouping);
   }
   else if (command == "use_heading_grouping")
   {
      aInput.ReadValue(mUseHeadingGrouping);
   }
   else if (command == "use_speed_grouping")
   {
      aInput.ReadValue(mUseSpeedGrouping);
   }
   else if (command == "use_type_grouping")
   {
      aInput.ReadValue(mUseTypeGrouping);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

bool WsfSA_Perceive::ProcessMessage(double aSimTime, const WsfMessage& aMessage)
{
   bool processed = false;
   if (aMessage.GetType() == WsfSA_EntityMessage::GetTypeId())
   {
      const WsfSA_EntityMessage& message            = static_cast<const WsfSA_EntityMessage&>(aMessage);
      WsfSA_EntityPerception*    assetPerceptionPtr = message.GetEntity();

      // Save off info into our buffer of received status messages (asset perception)

      // Do not save perception of the "self" platform
      if (assetPerceptionPtr->GetEntityPlatformIndex() != GetPlatform()->GetIndex())
      {
         WsfPlatform* assetPlatform = GetSimulation()->GetPlatformByIndex(assetPerceptionPtr->GetEntityPlatformIndex());

         // Only proceed if the platform is valid (not null)
         if (assetPlatform != nullptr)
         {
            auto it = mDataPtr->mReceivedAssetStatus.find(assetPerceptionPtr->GetEntityPlatformIndex());
            if (it == mDataPtr->mReceivedAssetStatus.end())
            {
               // Add a perception to the map for this asset the first time
               WsfSA_EntityPerception* assetEntity = assetPerceptionPtr->Clone();
               assetEntity->Time(aSimTime); // Explicitly set the current time
               mDataPtr->mReceivedAssetStatus[assetPerceptionPtr->GetEntityPlatformIndex()] = assetEntity;
            }
            else if (it->second->Time() < assetPerceptionPtr->Time())
            {
               // Overwrite the existing asset
               delete mDataPtr->mReceivedAssetStatus[assetPerceptionPtr->GetEntityPlatformIndex()];
               WsfSA_EntityPerception* assetEntity = assetPerceptionPtr->Clone();
               assetEntity->Time(aSimTime); // Explicitly set the current time
               mDataPtr->mReceivedAssetStatus[assetPerceptionPtr->GetEntityPlatformIndex()] = assetEntity;
            }
         }
      }
      processed = true;
   }

   // Purge assets based on lifetime since last update
   PurgeOldAssets(aSimTime);

   // Perform any local processing (script on_message callback)
   processed |= GetSAP()->GetMessageHandler()->ProcessMessage(aSimTime, aMessage);
   return processed;
}

void WsfSA_Perceive::PurgeOldAssets(double aSimTime)
{
   // Remove old (timed-out) entities
   if (GetSAP()->GetAssetDataPurgeLifetime() > 0)
   {
      for (auto it = mDataPtr->mReceivedAssetStatus.begin(); it != mDataPtr->mReceivedAssetStatus.end();)
      {
         WsfSA_EntityPerception* entity = (*it).second;

         // The allowable time is the entity's timestamp plus the lifetime
         double allowableTime = entity->Time() + GetSAP()->GetAssetDataPurgeLifetime();

         if (aSimTime > allowableTime)
         {
            // Remove the timed-out asset
            it = mDataPtr->mReceivedAssetStatus.erase(it);
            delete entity;
         }
         else
         {
            // Increment to next asset entity
            ++it;
         }
      }
   }
}

WsfSA_EntityPerception* WsfSA_Perceive::NearestAsset(double aSimTime)
{
   return NearestEntity(mDataPtr->mPerceivedAssets);
}

WsfSA_EntityPerception* WsfSA_Perceive::NearestAircraftAsset(double aSimTime)
{
   return NearestEntity(mDataPtr->mPerceivedAircraftAssets);
}

WsfSA_EntityPerception* WsfSA_Perceive::NearestMissileAsset(double aSimTime)
{
   return NearestEntity(mDataPtr->mPerceivedMissileAssets);
}

void WsfSA_Perceive::Initialize(double aSimTime)
{
   // Locate countermeasure "weapons" if appropriate
   if (!mDataPtr->mUseSimpleCM)
   {
      FindExpenableCountermeasures();
   }

   // Locate explicit weapons (other than countermeasure "weapons")
   FindWeapons();

   // Locate custom scripts, if they exist

   mDataPtr->mAssetConsiderationScoringScriptPtr = GetSAP()->GetScriptContext().FindScript("AssetConsiderationScoring");
   bool ok = GetSAP()->GetScriptContext().ValidateScript(mDataPtr->mAssetConsiderationScoringScriptPtr,
                                                         "double",
                                                         "WsfSA_EntityPerception");
   if (!ok)
   {
      mDataPtr->mAssetConsiderationScoringScriptPtr = nullptr;
   }

   mDataPtr->mBogieBanditConsiderationScoringScriptPtr =
      GetSAP()->GetScriptContext().FindScript("BogieBanditConsiderationScoring");
   ok = GetSAP()->GetScriptContext().ValidateScript(mDataPtr->mBogieBanditConsiderationScoringScriptPtr,
                                                    "double",
                                                    "WsfLocalTrack");
   if (!ok)
   {
      mDataPtr->mBogieBanditConsiderationScoringScriptPtr = nullptr;
   }

   mDataPtr->mMissileConsiderationScoringScriptPtr =
      GetSAP()->GetScriptContext().FindScript("MissileConsiderationScoring");
   ok = GetSAP()->GetScriptContext().ValidateScript(mDataPtr->mMissileConsiderationScoringScriptPtr,
                                                    "double",
                                                    "WsfLocalTrack");
   if (!ok)
   {
      mDataPtr->mMissileConsiderationScoringScriptPtr = nullptr;
   }

   mDataPtr->mUnfocusedGroupConsiderationScoringScriptPtr =
      GetSAP()->GetScriptContext().FindScript("UnfocusedGroupConsiderationScoring");
   ok = GetSAP()->GetScriptContext().ValidateScript(mDataPtr->mUnfocusedGroupConsiderationScoringScriptPtr,
                                                    "double",
                                                    "WsfSA_Group");
   if (!ok)
   {
      mDataPtr->mUnfocusedGroupConsiderationScoringScriptPtr = nullptr;
   }

   mDataPtr->mPruningScriptPtr = GetSAP()->GetScriptContext().FindScript("CreatePerceivedItemPruningArray");
   ok                          = GetSAP()->GetScriptContext().ValidateScript(mDataPtr->mPruningScriptPtr,
                                                    "Array<WsfSA_PerceivedItem>",
                                                    "Array<WsfSA_PerceivedItem>");
   if (!ok)
   {
      mDataPtr->mPruningScriptPtr = nullptr;
   }

   // Locate other script classes
   mDataPtr->mSA_EntityPerceptionClassPtr = GetSAP()->GetScriptContext().GetClass("WsfSA_EntityPerception");
   mDataPtr->mSA_GroupClassPtr            = GetSAP()->GetScriptContext().GetClass("WsfSA_Group");
   mDataPtr->mSA_PerceivedItemClassPtr    = GetSAP()->GetScriptContext().GetClass("WsfSA_PerceivedItem");
   mDataPtr->mTrackClassPtr               = GetSAP()->GetScriptContext().GetClass("WsfTrack");

   // Set the group manager data
   mGroupManager.InitializeData(GetPlatform(),
                                mMaxGroupingDistanceCentroid,
                                mMaxGroupingDistanceNeighbor,
                                mMaxGroupingHeadingDifference,
                                mMaxGroupingSpeedDifference,
                                0.0, // Not using grouping angle for now
                                mMinGroupRadius,
                                mUseCentroidGrouping,
                                mUseNeighborGrouping,
                                mUseHeadingGrouping,
                                mUseSpeedGrouping,
                                mUseTypeGrouping);
}

double WsfSA_Perceive::TryUpdateStatus(double aSimTime)
{
   // Perform the update
   UpdateStatus(aSimTime);

   return (aSimTime + GetSAP()->GetUpdateInterval(WsfSA_Processor::UpdateType::cSTATUS));
}

double WsfSA_Perceive::UpdateStatus(double aSimTime)
{
   if (GetSAP()->HasExternalLinks())
   {
      WsfPlatform* platformPtr = GetPlatform();

      if (mDataPtr->mReportingSelfStatus == true)
      {
         WsfSA_EntityPerception* perception =
            new WsfSA_EntityPerception(GetSimulation(), aSimTime, GetPlatform(), *platformPtr, true);
         perception->SetFlightId(GetSAP()->FlightId());

         WsfSA_EntityMessage myStatusMessage(platformPtr, perception);
#undef SendMessage
         GetSAP()->SendMessage(aSimTime, myStatusMessage);
      }

      if (mDataPtr->mReportingOthersStatus == true)
      {
         // Also send all asset status knowledge available
         for (auto& receivedAssetStatus : mDataPtr->mReceivedAssetStatus)
         {
            // TODO: should we store the WsfObject mNameId in an asset perception?
            // We could exclude sending his own status back to him
            // Don't want to do a platform lookup just to find his name each time we send
            WsfSA_EntityMessage aStatus(platformPtr, receivedAssetStatus.second->Clone());
#undef SendMessage
            GetSAP()->SendMessage(aSimTime, aStatus);
         }
      }
   }

   return aSimTime + GetSAP()->GetUpdateInterval(WsfSA_Processor::UpdateType::cSTATUS);
}

bool WsfSA_Perceive::ThreatScore(double aSimTime, WsfLocalTrack* aThreatPtr, bool aIsMissile, double& aScore)
{
   // First check for being perceived as a missile
   if (aIsMissile)
   {
      if (mDataPtr->mMissileConsiderationScoringScriptPtr != nullptr)
      {
         try
         {
            UtScriptData     scriptRetVal;
            UtScriptDataList scriptArgs;
            scriptArgs.emplace_back(new UtScriptRef(const_cast<WsfLocalTrack*>(aThreatPtr), mDataPtr->mTrackClassPtr));

            GetScriptContext().ExecuteScript(aSimTime, mDataPtr->mMissileConsiderationScoringScriptPtr, scriptRetVal, scriptArgs);
            aScore = scriptRetVal.GetDouble();
            return true;
         }
         catch (std::exception& e)
         {
            auto out = ut::log::error() << "Exception thrown in WsfSA_Perceive::ThreatScore.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Type: " << ut::TypeNameOf(e);
            out.AddNote() << "What: " << e.what();
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
            return false;
         }
      }
   }

   // We are here if the track is not a missile or if it is a missile, but lacks the missile consideration script
   if (mDataPtr->mBogieBanditConsiderationScoringScriptPtr == nullptr)
   {
      // No consideration script available, so return false
      return false;
   }

   try
   {
      UtScriptData     scriptRetVal;
      UtScriptDataList scriptArgs;
      scriptArgs.emplace_back(new UtScriptRef(const_cast<WsfLocalTrack*>(aThreatPtr), mDataPtr->mTrackClassPtr));

      GetScriptContext().ExecuteScript(aSimTime, mDataPtr->mBogieBanditConsiderationScoringScriptPtr, scriptRetVal, scriptArgs);
      aScore = scriptRetVal.GetDouble();
      return true;
   }
   catch (std::exception& e)
   {
      auto out = ut::log::error() << "Exception thrown in WsfSA_Perceive::ThreatScore.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Type: " << ut::TypeNameOf(e);
      out.AddNote() << "What: " << e.what();
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      return false;
   }
}

double WsfSA_Perceive::DefaultThreatScore(double aSimTime, WsfSA_PerceivedItem* aThreatPtr)
{
   double range = aThreatPtr->CalcRangeForPlatform(GetPlatform()) * 0.001; // range is in km
   return 1.0 / (1.0 + range);
}

WsfSA_EntityPerception* WsfSA_Perceive::NearestEntity(const std::vector<WsfSA_EntityPerception*>& aDataList) const
{
   WsfSA_EntityPerception* nearestEntity = nullptr;
   WsfPlatform*            platform      = GetPlatform();
   double                  AU            = 150000000000; // Roughly 1 astronomical unit (AU), 150M km

   double bestRange_m = AU;
   for (auto& entity : aDataList)
   {
      // Only consider non-angle-only entities
      if (!entity->GetAngleOnly())
      {
         double range_m = entity->CalcRangeForPlatform(platform);
         if (range_m < bestRange_m)
         {
            bestRange_m   = range_m;
            nearestEntity = entity;
         }
      }
   }

   return nearestEntity;
}

WsfSA_EntityPerception* WsfSA_Perceive::NearestBogie() const
{
   return NearestEntity(mDataPtr->mPerceivedBogies);
}

WsfSA_EntityPerception* WsfSA_Perceive::NearestAircraftBogie() const
{
   return NearestEntity(mDataPtr->mPerceivedAircraftBogies);
}

WsfSA_EntityPerception* WsfSA_Perceive::NearestMissileBogie() const
{
   return NearestEntity(mDataPtr->mPerceivedMissileBogies);
}

WsfSA_EntityPerception* WsfSA_Perceive::NearestBandit() const
{
   return NearestEntity(mDataPtr->mPerceivedBandits);
}

WsfSA_EntityPerception* WsfSA_Perceive::NearestAircraftBandit() const
{
   return NearestEntity(mDataPtr->mPerceivedAircraftBandits);
}

WsfSA_EntityPerception* WsfSA_Perceive::NearestMissileBandit() const
{
   return NearestEntity(mDataPtr->mPerceivedMissileBandits);
}

std::set<WsfPlatform*> WsfSA_Perceive::TruthAssets()
{
   // TODO - create this asset list on init once? (edit it as platforms are deleted) or create the list fresh each time?
   std::set<WsfPlatform*> assets;

   if (mDataPtr->mAssetTruthPerceptionMask == 0)
   {
      return assets; // empty
   }

   // Loop through multiple command chains, if they exist
   unsigned int commandChainCount = GetPlatform()->GetComponentCount<WsfCommandChain>();
   for (unsigned int chainNum = 0; chainNum < commandChainCount; ++chainNum)
   {
      WsfCommandChain* chainPtr = GetPlatform()->GetComponentEntry<WsfCommandChain>(chainNum);

      if (chainPtr != nullptr)
      {
         // Do subordinates first, to avoid missing out on climbing down the entire command tree,
         // if you would have already happen to have added somebody
         if ((mDataPtr->mAssetTruthPerceptionMask & cALLSUBORDINATES) != 0u)
         {
            std::pair<std::set<WsfPlatform*>::iterator, bool> ret;
            // Add all subordinates down the chain, start with first level subordinates
            std::vector<WsfPlatform*> subs;
            subs.insert(subs.end(), chainPtr->GetSubordinates().begin(), chainPtr->GetSubordinates().end());
            for (unsigned int i = 0; i < subs.size(); ++i)
            {
               WsfPlatform* pPtr = subs[i];
               ret               = assets.insert(pPtr);
               if (ret.second == true)
               {
                  // Haven't added this subordinate yet or climbed down it's chain either
                  // keep climbing down the command chain (tree) adding all subordinates
                  WsfCommandChain* tempChainPtr = pPtr->GetComponent<WsfCommandChain>(WsfCommandChain::GetDefaultNameId());
                  if (tempChainPtr != nullptr)
                  {
                     subs.insert(subs.end(), tempChainPtr->GetSubordinates().begin(), tempChainPtr->GetSubordinates().end());
                  }
               }
            }
         }
         else if ((mDataPtr->mAssetTruthPerceptionMask & cSUBORDINATES) != 0u)
         {
            WsfCommandChain::PlatformList           subs = chainPtr->GetSubordinates();
            WsfCommandChain::PlatformList::iterator it;
            for (it = subs.begin(); it != subs.end(); ++it)
            {
               assets.insert(*it);
            }
         }

         // Do commanders second
         WsfPlatform* cmdrPtr = chainPtr->GetCommander();
         if ((mDataPtr->mAssetTruthPerceptionMask & cALLCOMMANDERS) != 0u)
         {
            while (cmdrPtr != nullptr)
            {
               // Save off this commander
               assets.insert(cmdrPtr);
               // Keep climbing upwards
               WsfCommandChain* tempChainPtr = cmdrPtr->GetComponent<WsfCommandChain>(WsfCommandChain::GetDefaultNameId());
               if (tempChainPtr == nullptr)
               {
                  break;
               }
               cmdrPtr = tempChainPtr->GetCommander();
            }
         }
         else if (((mDataPtr->mAssetTruthPerceptionMask & cCOMMANDER) != 0u) && cmdrPtr != nullptr)
         {
            assets.insert(cmdrPtr);
         }

         // Do peers last, no traversing a command chain here
         if ((mDataPtr->mAssetTruthPerceptionMask & cPEERS) != 0u)
         {
            WsfCommandChain::PlatformList           peers = chainPtr->GetPeers();
            WsfCommandChain::PlatformList::iterator it;
            for (it = peers.begin(); it != peers.end(); ++it)
            {
               assets.insert(*it);
            }
         }
      }
   }

   // Add in self perception if configured to do so
   if (mDataPtr->mPerceiveSelfStatus == true)
   {
      assets.insert(GetPlatform());
   }

   // Remove any invalid platforms
   for (auto it = assets.begin(); it != assets.end();)
   {
      WsfPlatform* plat = *it;

      // Remove any item that is not valid
      if (plat == nullptr)
      {
         // Remove this item
         it = assets.erase(it);
      }
      else if (plat->IsDeleted() || plat->IsBroken())
      {
         // Remove this item
         it = assets.erase(it);
      }
      else
      {
         ++it;
      }
   }

   return assets;
}

std::string WsfSA_Perceive::SelectedWeaponName() const
{
   if (mDataPtr->mSelectedWeapon == nullptr)
   {
      return std::string();
   }

   return mDataPtr->mSelectedWeapon->GetName();
}

const WsfExplicitWeapon* WsfSA_Perceive::SelectedWeapon() const
{
   return mDataPtr->mSelectedWeapon;
}

int WsfSA_Perceive::QuantityOfSelectedWeapon() const
{
   if (mDataPtr->mSelectedWeapon == nullptr)
   {
      return 0;
   }

   // We add a slight amount extra to avoid a round-down
   return static_cast<int>(mDataPtr->mSelectedWeapon->GetQuantityRemaining() + 0.01);
}

bool WsfSA_Perceive::SetSelectedWeapon(const std::string& aWeaponName)
{
   // This is similar to the function HasOperationalWeapon()
   // Loop through components that are weapons
   for (WsfComponentList::RoleIterator<WsfWeapon> iter(*GetPlatform()); !iter.AtEnd(); ++iter)
   {
      if (iter->GetName() == aWeaponName)
      {
         if ((*iter)->IsA_TypeOf("WSF_EXPLICIT_WEAPON"))
         {
            mDataPtr->mSelectedWeapon = static_cast<WsfExplicitWeapon*>(*iter);
            return true;
         }
      }
   }

   // No match found, so fail
   return false;
}

void WsfSA_Perceive::SetMasterArm(bool aArmed)
{
   mDataPtr->mMasterArm = aArmed;
}

bool WsfSA_Perceive::GetMasterArm() const
{
   return mDataPtr->mMasterArm;
}

bool WsfSA_Perceive::MasterWarningActive() const
{
   return mDataPtr->mMasterWarningActive;
}

bool WsfSA_Perceive::MasterCautionActive() const
{
   return mDataPtr->mMasterCautionActive;
}

bool WsfSA_Perceive::WeaponBayDoorsAreOpen() const
{
   return mDataPtr->mWeaponBayDoorsAreOpen;
}

bool WsfSA_Perceive::ShootCueActive() const
{
   return mDataPtr->mShootCueActive;
}

bool WsfSA_Perceive::ShootCueBlinking() const
{
   return mDataPtr->mShootCueBlinking;
}

bool WsfSA_Perceive::JammingDetected() const
{
   return mDataPtr->mJammingDetected;
}

void WsfSA_Perceive::SetMasterWarningActive(bool aActive)
{
   mDataPtr->mMasterWarningActive = aActive;
}

void WsfSA_Perceive::SetMasterCautionActive(bool aActive)
{
   mDataPtr->mMasterCautionActive = aActive;
}

void WsfSA_Perceive::SetWeaponBayDoorsAreOpen(bool aOpen)
{
   mDataPtr->mWeaponBayDoorsAreOpen = aOpen;
}

void WsfSA_Perceive::SetShootCueActive(bool aActive)
{
   mDataPtr->mShootCueActive = aActive;
}

void WsfSA_Perceive::SetShootCueBlinking(bool aBlinking)
{
   mDataPtr->mShootCueBlinking = aBlinking;
}

void WsfSA_Perceive::SetJammingDetected(bool aJammingDetected)
{
   mDataPtr->mJammingDetected = aJammingDetected;
}

int WsfSA_Perceive::QuantityOfChaff() const
{
   if (!mDataPtr->mUseSimpleCM)
   {
      if (mDataPtr->mChaffDispenser != nullptr)
      {
         // We add a slight amount extra to avoid a round-down
         return static_cast<int>(mDataPtr->mChaffDispenser->GetQuantityRemaining() + 0.01);
      }
      else
      {
         // No chaff dispenser, so return zero
         return 0;
      }
   }

   return static_cast<int>(mDataPtr->mNumSimpleChaff);
}

int WsfSA_Perceive::QuantityOfFlares() const
{
   if (!mDataPtr->mUseSimpleCM)
   {
      // No flare dispenser, so return zero
      return 0;
   }

   return static_cast<int>(mDataPtr->mNumSimpleFlare);
}

int WsfSA_Perceive::QuantityOfDecoys() const
{
   if (!mDataPtr->mUseSimpleCM)
   {
      // No decoy dispenser, so return zero
      return 0;
   }

   return static_cast<int>(mDataPtr->mNumSimpleDecoy);
}

bool WsfSA_Perceive::DispenseChaff(double aSimTime)
{
   if (mDataPtr->mUseSimpleCM)
   {
      if (mDataPtr->mNumSimpleChaff > 0)
      {
         --mDataPtr->mNumSimpleChaff;
         return true;
      }
      else
      {
         return false;
      }
   }
   else
   {
      if ((mDataPtr->mChaffDispenser != nullptr) && (QuantityOfChaff() > 0))
      {
         mDataPtr->mChaffDispenser->Fire(aSimTime, WsfWeapon::FireTarget(), WsfWeapon::FireOptions());
         return true;
      }
      else
      {
         return false;
      }
   }
}

bool WsfSA_Perceive::DispenseFlare(double aSimTime)
{
   if (mDataPtr->mUseSimpleCM)
   {
      if (mDataPtr->mNumSimpleFlare > 0)
      {
         --mDataPtr->mNumSimpleFlare;
         return true;
      }
      else
      {
         return false;
      }
   }

   return false;
}

bool WsfSA_Perceive::DispenseDecoy(double aSimTime)
{
   if (mDataPtr->mUseSimpleCM)
   {
      if (mDataPtr->mNumSimpleDecoy > 0)
      {
         --mDataPtr->mNumSimpleDecoy;
         return true;
      }
      else
      {
         return false;
      }
   }

   return false;
}

bool WsfSA_Perceive::RenameGroup(WsfSA_Group* aGroup, const std::string& aName)
{
   return WsfSA_GroupUtils::RenameGroup(mDataPtr->mCurrentlyPerceivedGroups, aGroup, aName);
}

WsfSA_Group* WsfSA_Perceive::CreateGroup(const std::list<WsfSA_EntityPerception*>& aElements, bool aProtect)
{
   double minGroupRadius_nm = mMinGroupRadius * UtMath::cNM_PER_M;
   return WsfSA_GroupUtils::CreateGroup(mDataPtr->mMaxThreatLoad,
                                        WsfSA_GroupUtils::CalculateNumPIs(mDataPtr->mCurrentlyPerceivedGroups),
                                        mDataPtr->mCurrentlyPerceivedGroups,
                                        GetPlatform(),
                                        aElements,
                                        mGroupManager.GenerateNewGroupName(),
                                        minGroupRadius_nm,
                                        aProtect);
}

bool WsfSA_Perceive::DisbandGroup(WsfSA_Group* aGroup, bool aProtect)
{
   double minGroupRadius_nm = mMinGroupRadius * UtMath::cNM_PER_M;
   auto&  gm                = mGroupManager;
   auto   generateGroupName = [&gm]() -> std::string { return gm.GenerateNewGroupName(); };
   return WsfSA_GroupUtils::DisbandGroup(mDataPtr->mMaxThreatLoad,
                                         WsfSA_GroupUtils::CalculateNumPIs(mDataPtr->mCurrentlyPerceivedGroups),
                                         mDataPtr->mCurrentlyPerceivedGroups,
                                         aGroup,
                                         generateGroupName,
                                         minGroupRadius_nm,
                                         aProtect);
}

WsfSA_Group* WsfSA_Perceive::MergeGroups(const std::list<WsfSA_Group*>& aGroups, bool aProtect)
{
   double minGroupRadius_nm = mMinGroupRadius * UtMath::cNM_PER_M;
   return WsfSA_GroupUtils::MergeGroups(mDataPtr->mCurrentlyPerceivedGroups, aGroups, GetPlatform(), minGroupRadius_nm, aProtect);
}

std::pair<WsfSA_Group*, WsfSA_Group*>
WsfSA_Perceive::SplitGroup(WsfSA_Group* aGroup, int aNumElements, int aCriterion, bool aProtect)
{
   double minGroupRadius_nm = mMinGroupRadius * UtMath::cNM_PER_M;
   return WsfSA_GroupUtils::SplitGroup(mDataPtr->mMaxThreatLoad,
                                       WsfSA_GroupUtils::CalculateNumPIs(mDataPtr->mCurrentlyPerceivedGroups),
                                       mDataPtr->mCurrentlyPerceivedGroups,
                                       aGroup,
                                       aNumElements,
                                       aCriterion,
                                       GetPlatform(),
                                       mGroupManager.GenerateNewGroupName(),
                                       minGroupRadius_nm,
                                       aProtect);
}

bool WsfSA_Perceive::TransferEntity(WsfSA_EntityPerception* aEntity, WsfSA_Group* aGroup, bool aProtect)
{
   double minGroupRadius_nm = mMinGroupRadius * UtMath::cNM_PER_M;
   return WsfSA_GroupUtils::TransferEntity(mDataPtr->mMaxThreatLoad,
                                           WsfSA_GroupUtils::CalculateNumPIs(mDataPtr->mCurrentlyPerceivedGroups),
                                           mDataPtr->mCurrentlyPerceivedGroups,
                                           aEntity,
                                           aGroup,
                                           minGroupRadius_nm,
                                           aProtect);
}

bool WsfSA_Perceive::RemoveEntityFromGroup(WsfSA_EntityPerception* aEntity, bool aProtect)
{
   double minGroupRadius_nm = mMinGroupRadius * UtMath::cNM_PER_M;
   return WsfSA_GroupUtils::RemoveEntityFromGroup(mDataPtr->mMaxThreatLoad,
                                                  WsfSA_GroupUtils::CalculateNumPIs(mDataPtr->mCurrentlyPerceivedGroups),
                                                  mDataPtr->mCurrentlyPerceivedGroups,
                                                  aEntity,
                                                  mGroupManager.GenerateNewGroupName(),
                                                  minGroupRadius_nm,
                                                  aProtect);
}

bool WsfSA_Perceive::SetFocus(WsfSA_Group* aGroup, bool aFocus, bool aProtect)
{
   auto ret = WsfSA_GroupUtils::SetFocus(mDataPtr->mMaxThreatLoad,
                                         WsfSA_GroupUtils::CalculateNumPIs(mDataPtr->mCurrentlyPerceivedGroups),
                                         aGroup,
                                         aFocus,
                                         aProtect);
   if (ret)
   {
      GetSAP()->Assess()->UpdateGroupFocusLists();
   }
   return ret;
}

void WsfSA_Perceive::SetImportant(WsfSA_PerceivedItem* aItem, bool aIsImportant)
{
   if (aIsImportant)
   {
      aItem->SetImportance(WsfSA_PerceivedItem::IMPORTANT);
   }
   else
   {
      aItem->SetImportance(WsfSA_PerceivedItem::NOMINAL);
   }
}

bool WsfSA_Perceive::GetFocus(WsfSA_Group* aGroup)
{
   if (aGroup)
   {
      return aGroup->GetFocus();
   }
   return false;
}

bool WsfSA_Perceive::GetImportant(WsfSA_PerceivedItem* aItem)
{
   if (aItem)
   {
      return aItem->GetImportance() == WsfSA_PerceivedItem::IMPORTANT;
   }
   return false;
}

double WsfSA_Perceive::TryUpdateAssets(double aSimTime)
{
   if (mDataPtr->mExtraCognitiveDelaysAssets.empty())
   {
      // Perform the update
      UpdateAssets(aSimTime);

      // Send the data
      WsfObserver::AirCombatSendPerceivedAssetsData(GetSimulation())(aSimTime, *GetPlatform(), *GetSAP());

      // Update the last update time
      GetSAP()->SetLastUpdateTime(WsfSA_Processor::UpdateType::cASSETS_DATA, aSimTime);

      return (aSimTime + GetSAP()->GetUpdateInterval(WsfSA_Processor::UpdateType::cASSETS_DATA));
   }
   else
   {
      // Delay the update even longer
      double extraDelay = 0.0;
      while (!mDataPtr->mExtraCognitiveDelaysAssets.empty())
      {
         extraDelay += mDataPtr->mExtraCognitiveDelaysAssets.back();
         mDataPtr->mExtraCognitiveDelaysAssets.pop_back();
      }
      return (aSimTime + extraDelay);
   }
}

double WsfSA_Perceive::TryUpdatePerceivedItemData(double aSimTime)
{
   // Send the data
   WsfObserver::AirCombatSendPerceivedItemsData(GetSimulation())(aSimTime, *GetPlatform(), *GetSAP());

   // Update the last update time
   GetSAP()->SetLastUpdateTime(WsfSA_Processor::UpdateType::cPERCEIVED_ITEM_DATA, aSimTime);

   return (aSimTime + GetSAP()->GetUpdateInterval(WsfSA_Processor::UpdateType::cPERCEIVED_ITEM_DATA));
}

double WsfSA_Perceive::TryUpdatePerceivedItemCalculation(double aSimTime)
{
   if (mDataPtr->mExtraCognitiveDelaysThreats.empty())
   {
      // Perform the update
      UpdatePerceivedItemsCalculation(aSimTime);
      UpdateGroupsData(aSimTime);

      // Update the last update time
      GetSAP()->SetLastUpdateTime(WsfSA_Processor::UpdateType::cPERCEIVED_ITEM_CALCULATION, aSimTime);

      return (aSimTime + GetSAP()->GetUpdateInterval(WsfSA_Processor::UpdateType::cPERCEIVED_ITEM_CALCULATION));
   }
   else
   {
      // Delay the update even longer
      double extraDelay = 0.0;
      while (!mDataPtr->mExtraCognitiveDelaysThreats.empty())
      {
         extraDelay += mDataPtr->mExtraCognitiveDelaysThreats.back();
         mDataPtr->mExtraCognitiveDelaysThreats.pop_back();
      }
      return (aSimTime + extraDelay);
   }
}

bool WsfSA_Perceive::Unimportant(WsfLocalTrack* aThreatPtr)
{
   if (aThreatPtr == nullptr)
   {
      return false;
   }
   // Check if this specific track has been marked as unimportant
   if (mDataPtr->mUnimportantThreats.find(aThreatPtr->GetTargetIndex()) != mDataPtr->mUnimportantThreats.end())
   {
      return true;
   }
   // Check if the track's type has been marked as unimportant, use perceived type if possible
   WsfStringId tgtType = (aThreatPtr->TypeIdValid()) ? (aThreatPtr->GetTypeId()) : (aThreatPtr->GetTargetType());
   if (mDataPtr->mUnimportantThreatTypes.find(tgtType) != mDataPtr->mUnimportantThreatTypes.end())
   {
      return true;
   }
   return false;
}

bool WsfSA_Perceive::Unimportant(WsfPlatform* aAssetPtr)
{
   if (aAssetPtr == nullptr)
   {
      return false;
   }
   // Check if this specific platform has been marked as unimportant
   if (mDataPtr->mUnimportantAssets.find(aAssetPtr->GetIndex()) != mDataPtr->mUnimportantAssets.end())
   {
      return true;
   }
   // Check if the platform's type has been marked as unimportant
   WsfStringId key = aAssetPtr->GetType();
   if (mDataPtr->mUnimportantAssetTypes.find(key) != mDataPtr->mUnimportantAssetTypes.end())
   {
      return true;
   }
   return false;
}

bool WsfSA_Perceive::Unimportant(size_t aPlatformIndex)
{
   return Unimportant(GetSimulation()->GetPlatformByIndex(aPlatformIndex));
}

void WsfSA_Perceive::UpdateAssetsData(double aSimTime)
{
   // These lists are based on the main assets list and determined in CreateAssetSubLists().
   // Since they consist of copies of the perceptions in mData.mPerceivedAssets, it's safe to clear them out fully.
   for (auto& perceivedAircraftAsset : mDataPtr->mPerceivedAircraftAssets)
   {
      delete perceivedAircraftAsset;
   }
   mDataPtr->mPerceivedAircraftAssets.clear();
   for (auto& perceivedMissileAsset : mDataPtr->mPerceivedMissileAssets)
   {
      delete perceivedMissileAsset;
   }
   mDataPtr->mPerceivedMissileAssets.clear();

   if (mDataPtr->mMaxAssetLoad == 0) // No assets
   {
      // Clear out previous perceptions and return
      for (auto& perceivedAsset : mDataPtr->mPerceivedAssets)
      {
         delete perceivedAsset;
      }
      mDataPtr->mPerceivedAssets.clear();

      return;
   }

   // Check for unlimited assets (negative value of mData.mMaxAssetLoad) and set a flag
   bool unlimitedAssets = false;
   if (mDataPtr->mMaxAssetLoad < 0)
   {
      unlimitedAssets = true;
   }

   // There are two types of asset perception -- TRUTH and STATUS_MESSAGES -- each is handled below

   // This temporary list is used below
   std::list<WsfSA_EntityPerception*> tmpAssetsList;

   if (mDataPtr->mAssetPerception == GetSAP()->TRUTH)
   {
      // First, get the platforms for the assets
      std::set<WsfPlatform*> assetPlatforms = TruthAssets();

      // Next, create a temporary list of asset SA entity perceptions
      for (auto assetPlatform : assetPlatforms)
      {
         auto tempAssetPerception =
            new WsfSA_EntityPerception(GetSimulation(), aSimTime, GetPlatform(), *assetPlatform, true);
         tempAssetPerception->SetIsNonTrackAsset(true);
         tmpAssetsList.push_back(tempAssetPerception);
      }
   }
   else // STATUS_MESSAGES
   {
      // Note:  mData.mReceivedAssetStatus provides the list of assets that generated STATUS_MESSAGES

      // First, add in self-perception if appropriate
      if (mDataPtr->mPerceiveSelfStatus == true)
      {
         WsfPlatform* self = GetPlatform();
         mDataPtr->mReceivedAssetStatus[self->GetIndex()] =
            new WsfSA_EntityPerception(GetSimulation(), aSimTime, GetPlatform(), *self, true);
      }

      // Purge assets based on lifetime since last update
      PurgeOldAssets(aSimTime);

      // Next, copy the received assets into a temporary list of asset SA entity perceptions
      for (auto asset : mDataPtr->mReceivedAssetStatus)
      {
         auto tempAssetPerception = new WsfSA_EntityPerception(aSimTime, GetPlatform(), *(asset.second));
         tempAssetPerception->SetIsNonTrackAsset(true);
         tmpAssetsList.push_back(tempAssetPerception);
      }
   }

   // Clear out any previous assets if they've outlived the coast time.
   RemoveExpiredCoastingPerceptions(mDataPtr->mPerceivedAssets, mDataPtr->mAssetCoastTime, aSimTime);

   // For each asset that isn't getting an update, set it to coasting.
   if (mDataPtr->mAssetCoastTime > 0)
   {
      UpdateCoastingPerceptions(mDataPtr->mPerceivedAssets, tmpAssetsList, aSimTime);
   }

   // Now, use the temporary assets list and the unlimited flag to create the assets list (mPerceivedAssets)
   CreateAssetsList(aSimTime, tmpAssetsList, unlimitedAssets);

   // Create sublists for aircraft and missiles
   CreateAssetSublists();
}

void WsfSA_Perceive::UpdateGroupsData(double aSimTime)
{
   // Update and assess the groups using perceived bogies/threats data
   mGroupManager.UpdateAndAssessGroups(aSimTime,
                                       GetPlatform(),
                                       mDataPtr->mCurrentlyPerceivedGroups,
                                       mDataPtr->mPerceivedBogies,
                                       mDataPtr->mPerceivedBandits,
                                       mIgnoreBogiesWhenGrouping);

   GetSAP()->Assess()->UpdateGroupFocusLists();
}

void WsfSA_Perceive::UpdatePerceivedItemsCalculation(double aSimTime)
{
   // This function updates the lists of perceived bogies and bandits

   // These lists are based on the main bandits list and determined in CreateSubLists().
   // Since they consist of copies of the perceptions in mData.mPerceivedBandits, it's safe to clear them out fully.
   for (auto& perceivedAircraftBandit : mDataPtr->mPerceivedAircraftBandits)
   {
      delete perceivedAircraftBandit;
   }
   mDataPtr->mPerceivedAircraftBandits.clear();
   for (auto& perceivedMissileBandit : mDataPtr->mPerceivedMissileBandits)
   {
      delete perceivedMissileBandit;
   }
   mDataPtr->mPerceivedMissileBandits.clear();

   // These lists are based on the main bogies list and determined in CreateSubLists().
   // Since they consist of copies of the perceptions in mData.mPerceivedBogies, it's safe to clear them out fully.
   for (auto& perceivedAircraftBogie : mDataPtr->mPerceivedAircraftBogies)
   {
      delete perceivedAircraftBogie;
   }
   mDataPtr->mPerceivedAircraftBogies.clear();
   for (auto& perceivedMissileBogie : mDataPtr->mPerceivedMissileBogies)
   {
      delete perceivedMissileBogie;
   }
   mDataPtr->mPerceivedMissileBogies.clear();

   if (mDataPtr->mMaxThreatLoad == 0) // No threats (or bogies/bandits)
   {
      // Clear out previous perceptions and return
      for (auto& perceivedBogie : mDataPtr->mPerceivedBogies)
      {
         delete perceivedBogie;
      }
      mDataPtr->mPerceivedBogies.clear();

      for (auto& perceivedBandit : mDataPtr->mPerceivedBandits)
      {
         delete perceivedBandit;
      }
      mDataPtr->mPerceivedBandits.clear();

      return;
   }

   // First, get the master track list -- we loop through these to gather bogies and bandits
   WsfLocalTrackList& tmpTrkList = GetPlatform()->GetMasterTrackList();
   WsfLocalTrackList* trackList  = &tmpTrkList;

   // Note: The track list will either use the MTP list (default) or the PMTP (optional)
   if (GetSAP()->UsingPerceptionMasterTrackProcessor())
   {
      // Use PMTP instead of MTP
      trackList = GetSAP()->GetPerceptionMasterTrackProcessorTrackList();
   }

   // Empty mTrackDelayQueue container if trackList is empty or display_perception_delay and visual_perception_delay are not set
   if (trackList->GetTrackCount() == 0)
   {
      mTrackDelayQueue.clear();
   }
   else if (GetSAP()->GetVisualPerceptionDelay() == 0.0 && GetSAP()->GetDisplayPerceptionDelay() == 0.0)
   {
      mTrackDelayQueue.clear();
   }

   // If display_perception_delay or visual_perception_delay is set, apply it to each track:
   //    For each track in trackList, update perception delay container:
   //       If track is new, start its perception delay (add to mTrackDelayQueue, isDone = false)
   //          Check to see if isVisual needs to be true or false by checking the eyes_track_processor
   //       If perception delay is now complete, change isDone to true
   //       If perception delay is still in progress or has already completed
   //          do nothing to perception delay container
   //    For each track in mTrackDelayQueue, make sure it still appears in trackList
   //       Prune dropped tracks from mTrackDelayQueue
   if (GetSAP()->GetDisplayPerceptionDelay() > 0 || GetSAP()->GetVisualPerceptionDelay() > 0)
   {
      for (unsigned a = 0; a < trackList->GetTrackCount(); ++a)
      {
         if (trackList->GetTrackEntry(a) != nullptr)
         {
            WsfTrackId id         = trackList->GetTrackEntry(a)->GetTrackId();
            auto       foundTrack = mTrackDelayQueue.find(id);

            if (foundTrack == mTrackDelayQueue.end())
            {
               // Track not found in queue. Needs to be added. isDone = false. Default mIsVisual = false.
               TrackDelayData tmpData;
               tmpData.mIsDone    = false;
               tmpData.mIsVisual  = false;
               tmpData.mTimeAdded = aSimTime;

               // Check if track is in eyes_track_processor. If it is mIsVisual = true.
               if (trackList->GetTrackEntry(a)->GetTargetIndex() != 0)
               {
                  WsfTrackProcessor* trkProcessor = GetSAP()->GetTrackProcessor(GetSAP()->EyesTrackProcessor());

                  if (trkProcessor != nullptr)
                  {
                     WsfLocalTrackList trkProcessorTrkList = trkProcessor->GetTrackManager().GetTrackList();
                     for (unsigned procEntry = 0; procEntry < trkProcessorTrkList.GetTrackCount(); ++procEntry)
                     {
                        if (trkProcessorTrkList.GetTrackEntry(procEntry)->GetTargetIndex() ==
                            trackList->GetTrackEntry(a)->GetTargetIndex())
                        {
                           tmpData.mIsVisual = true;
                           if (GetSAP()->GetVisualPerceptionDelay() == 0.0)
                           {
                              tmpData.mIsDone = true;
                           }
                        }
                     }
                  }
               }

               if (GetSAP()->GetDisplayPerceptionDelay() == 0.0 && !tmpData.mIsVisual)
               {
                  tmpData.mIsDone = true;
               }

               mTrackDelayQueue.emplace(id, tmpData);
            }
            else
            {
               // Track found in queue.
               // Check if isDone = false. Check to see if it needs set to true.
               if (!foundTrack->second.mIsDone)
               {
                  if (foundTrack->second.mIsVisual)
                  {
                     // Perception delay is in progress - is it now complete?
                     if (foundTrack->second.mTimeAdded + GetSAP()->GetVisualPerceptionDelay() <= aSimTime)
                     {
                        // This track has completed its perception delay - change mIsDone to true
                        foundTrack->second.mIsDone = true;
                     }
                  }
                  else
                  {
                     // Perception delay is in progress - is it now complete?
                     if (foundTrack->second.mTimeAdded + GetSAP()->GetDisplayPerceptionDelay() <= aSimTime)
                     {
                        // This track has completed its perception delay - change mIsDone to true
                        foundTrack->second.mIsDone = true;
                     }
                  }
               }
            }
         }
      }

      // Prune dropped tracks from mTrackDelayQueue
      for (auto elem = mTrackDelayQueue.begin(); elem != mTrackDelayQueue.end();)
      {
         auto found = trackList->FindTrack(elem->first);

         if (found == nullptr)
         {
            elem = mTrackDelayQueue.erase(elem);
         }
         else
         {
            ++elem;
         }
      }
   }

   bool unlimitedThreats = true;

   // Set the desired range to assume for angle-only targets
   WsfSA_EntityPerception::SetAssumedRangeForAngleOnlyTargets(GetSAP()->AssumedRangeForAngleOnlyTargets());

   // Create a temporary list of bogie/threat entities, based on the tracks in the master track list
   std::list<WsfSA_EntityPerception*> tmpBogiesAndBanditsList;
   unsigned int                       numTracks = trackList->GetTrackCount();

   for (unsigned int i = 0; i < numTracks; ++i)
   {
      WsfLocalTrack* track = trackList->GetTrackEntry(i);
      if (track != nullptr)
      {
         // If display_perception_delay or visual_perception_delay is set, skip tracks that haven't completed the delay
         if (GetSAP()->GetDisplayPerceptionDelay() > 0 || GetSAP()->GetVisualPerceptionDelay() > 0)
         {
            auto it = mTrackDelayQueue.find(track->GetTrackId());
            if (!it->second.mIsDone)
            {
               continue;
            }
         }

         size_t targetIndex = track->GetTargetIndex();

         // Target platform must be valid (non-null) or it must be a false target
         WsfPlatform* tgtPlatform = GetSimulation()->GetPlatformByIndex(targetIndex);

         if ((tgtPlatform != nullptr) || (track->IsFalseTarget()))
         {
            // Must not be the current (ownship) platform -- this filters-out ownship
            if (GetPlatform()->GetIndex() != targetIndex)
            {
               // If using mFilterAssetsFromBogies, must not be an asset to proceed -- this may filter friends
               if (!(mDataPtr->mFilterAssetsFromBogies && EntityIsInAssetGroup(targetIndex)) && !Unimportant(track))
               {
                  // Create an entity and add it to the list
                  WsfSA_EntityPerception* entity =
                     new WsfSA_EntityPerception(GetSimulation(), aSimTime, GetPlatform(), track);
                  tmpBogiesAndBanditsList.push_back(entity);
               }
            }
         }
      }
   }

   // Clear out previous bandits/bogies if they've outlived the coast time.
   RemoveExpiredCoastingPerceptions(mDataPtr->mPerceivedBandits, mDataPtr->mBanditCoastTime, aSimTime);
   RemoveExpiredCoastingPerceptions(mDataPtr->mPerceivedBogies, mDataPtr->mBogeyCoastTime, aSimTime);

   // For each bogie/bandit that isn't getting an update, set it to coasting.
   UpdateCoastingPerceptions(mDataPtr->mPerceivedBandits, tmpBogiesAndBanditsList, aSimTime);
   UpdateCoastingPerceptions(mDataPtr->mPerceivedBogies, tmpBogiesAndBanditsList, aSimTime);

   // We can end early if there are no new entities.
   if (tmpBogiesAndBanditsList.empty())
   {
      // Even though there are no *new* entities,
      // we need to make sure that any old threats still recognized are set to coast.
      for (auto& threat : mDataPtr->mPerceivedBandits)
      {
         threat->SetIsCoasting(true);
      }
      for (auto& threat : mDataPtr->mPerceivedBogies)
      {
         threat->SetIsCoasting(true);
      }
      SetOldPerceptionsAsCoasting(mDataPtr->mPerceivedBandits, aSimTime);
      SetOldPerceptionsAsCoasting(mDataPtr->mPerceivedBogies, aSimTime);
      return;
   }

   // Known bandits are more important than bogies, so we first assemble the perceived bandits
   CreateBanditsList(aSimTime, tmpBogiesAndBanditsList, unlimitedThreats);

   // Now, we assemble the perceived bogies
   CreateBogiesList(aSimTime, tmpBogiesAndBanditsList, unlimitedThreats);

   // Filter out any invalid entities
   RemoveAnyInvalidEntitiesInBogiesAndBandits();

   // Ensure that all entities have an initial group
   JoinInitialGroups(mDataPtr->mPerceivedBandits, mDataPtr->mCurrentlyPerceivedGroups, aSimTime);
   JoinInitialGroups(mDataPtr->mPerceivedBogies, mDataPtr->mCurrentlyPerceivedGroups, aSimTime);

   // Filter according to PI limit
   Prune(aSimTime);

   // Get an assessment of the data
   GetSAP()->Assess()->AssessBogiesBandits(aSimTime);

   // Create bogie/bandit sublists for aircraft and missiles
   CreateSublists();

   // Give Assess time to make sure that there are no old pointers to deleted entities before we actually delete them.
   DeferredDelete();
}

void WsfSA_Perceive::RemoveAnyInvalidEntitiesInBogiesAndBandits()
{
   // First, filter-out bogies that are not valid, meaning they lack a perceived index
   for (auto it = mDataPtr->mPerceivedBogies.begin(); it != mDataPtr->mPerceivedBogies.end();)
   {
      WsfSA_EntityPerception* entity = *it;

      // Remove any item that is not valid
      if (entity->GetPerceivedIndex() < 1)
      {
         // Remove this item
         it = mDataPtr->mPerceivedBogies.erase(it);

         // Delete the entity
         DeleteLater(entity);
      }
      else
      {
         ++it;
      }
   }

   // Next, filter-out bandits that are not valid, meaning they lack a perceived index
   for (auto it = mDataPtr->mPerceivedBandits.begin(); it != mDataPtr->mPerceivedBandits.end();)
   {
      WsfSA_EntityPerception* entity = *it;

      // Remove any item that is not valid
      if (entity->GetPerceivedIndex() < 1)
      {
         // Remove this item
         it = mDataPtr->mPerceivedBandits.erase(it);

         // Delete the entity
         DeleteLater(entity);
      }
      else
      {
         ++it;
      }
   }
}

void WsfSA_Perceive::Prune(double aSimTime)
{
   // Ensure that all important entities are internally set as IMPORTANT.
   std::vector<WsfSA_PerceivedItem*> PIs{};
   for (auto& entity : mDataPtr->mPerceivedBogies)
   {
      if (Important(entity->GetTrackPtr()))
      {
         entity->SetImportance(WsfSA_PerceivedItem::IMPORTANT);
      }
   }
   for (auto& entity : mDataPtr->mPerceivedBandits)
   {
      if (Important(entity->GetTrackPtr()))
      {
         entity->SetImportance(WsfSA_PerceivedItem::IMPORTANT);
      }
   }

   // Add all groups to the PI list.
   PIs.insert(PIs.end(), mDataPtr->mCurrentlyPerceivedGroups.begin(), mDataPtr->mCurrentlyPerceivedGroups.end());

   auto maxScore = [](double aMax, const WsfSA_EntityPerception* aEntity)
   { return std::max(aMax, aEntity->GetSelectionScore()); };

   // Set each group's selection score.
   for (auto& group : mDataPtr->mCurrentlyPerceivedGroups)
   {
      if (!(group->GetFocus()))
      {
         // Unfocused groups use either the user-defined script, or else fall back to the same scoring as an individual entity.
         if (mDataPtr->mUnfocusedGroupConsiderationScoringScriptPtr != nullptr)
         {
            try
            {
               UtScriptData     scriptRetVal;
               UtScriptDataList scriptArgs;
               scriptArgs.emplace_back(new UtScriptRef(const_cast<WsfSA_Group*>(group), mDataPtr->mSA_GroupClassPtr));
               auto dist = std::uniform_real_distribution<double>(-mConsiderationRandomness, mConsiderationRandomness);
               GetScriptContext().ExecuteScript(aSimTime,
                                                mDataPtr->mUnfocusedGroupConsiderationScoringScriptPtr,
                                                scriptRetVal,
                                                scriptArgs);
               group->SetSelectionScore(scriptRetVal.GetDouble() + dist(mRandom));
            }
            catch (std::exception& e)
            {
               auto out = ut::log::error() << "Exception thrown in WsfSA_Perceive::Prune.";
               out.AddNote() << "T = " << aSimTime;
               out.AddNote() << "Type: " << ut::TypeNameOf(e);
               out.AddNote() << "What: " << e.what();
               out.AddNote() << "Platform: " << GetPlatform()->GetName();
            }
         }
         else
         {
            group->SetSelectionScore(DefaultThreatScore(aSimTime, group));
         }
      }
      else
      {
         // Focused groups use the score of their highest-scoring entity.
         auto elements = group->GetElementList();
         group->SetSelectionScore(std::accumulate(elements.begin(), elements.end(), 0.0, maxScore));
      }
   }

   // Sort them by their scores.
   // Use a stable sort, because this will put newer PIs first.
   std::stable_sort(PIs.begin(), PIs.end(), WsfSA_PerceivedItem::CompareScoreLowGood);

   // This is the list of PIs that need to be removed in order to keep within the PI limit.
   std::list<WsfSA_PerceivedItem*> toRemove{};

   if (mDataPtr->mPruningScriptPtr != nullptr)
   {
      // Try for a user-defined pruning function first.
      try
      {
         UtScriptData              scriptRetVal;
         UtScriptDataList          scriptArgs;
         std::vector<UtScriptData> argPIs{};
         for (auto& PI : PIs)
         {
            argPIs.push_back(UtScriptRef::Ref(PI, mDataPtr->mSA_PerceivedItemClassPtr));
         }
         scriptArgs.emplace_back(
            new UtScriptRef(&argPIs, GetSAP()->GetScriptContext().GetClass("Array<WsfSA_PerceivedItem>")));
         GetScriptContext().ExecuteScript(aSimTime, mDataPtr->mPruningScriptPtr, scriptRetVal, scriptArgs);
         auto ret = static_cast<std::vector<UtScriptData>*>(scriptRetVal.GetPointer()->GetAppObject());
         for (auto& entity : *ret)
         {
            toRemove.push_back(static_cast<WsfSA_EntityPerception*>(entity.GetPointer()->GetAppObject()));
         }
      }
      catch (std::exception& e)
      {
         auto out = ut::log::error() << "Exception thrown in WsfSA_Perceive::Prune.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Type: " << ut::TypeNameOf(e);
         out.AddNote() << "What: " << e.what();
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
      }
   }
   else
   {
      // Otherwise, use the default.
      std::list<WsfSA_PerceivedItem*> PI_List{PIs.begin(), PIs.end()};
      WsfSA_GroupUtils::Prune(mDataPtr->mMaxThreatLoad, PI_List, toRemove);
   }

   // Delete everything in toRemove.
   for (auto& item : toRemove)
   {
      if (item->GetItemType() == WsfSA_PerceivedItem::ENTITY)
      {
         auto entity = dynamic_cast<WsfSA_EntityPerception*>(item);
         DropEntity(entity);
      }
      else
      {
         // For groups, that means deleting the entities *and* the groups.
         auto group = dynamic_cast<WsfSA_Group*>(item);
         group->SetFocus(true);
         auto elements = group->ModifiableElementList();
         while (!elements->empty())
         {
            DropEntity(elements->back());
            elements->pop_back();
         }
      }
   }
}

void WsfSA_Perceive::UpdateAssets(double aSimTime)
{
   if (aSimTime > GetSAP()->GetLastUpdateTime(WsfSA_Processor::UpdateType::cASSETS_DATA))
   {
      // Update the data
      UpdateAssetsData(aSimTime);
      GetSAP()->SetLastUpdateTime(WsfSA_Processor::UpdateType::cASSETS_DATA, aSimTime);
   }
}

void WsfSA_Perceive::FindExpenableCountermeasures()
{
   // Loop through components that are weapons
   for (WsfComponentList::RoleIterator<WsfWeapon> iter(*GetPlatform()); !iter.AtEnd(); ++iter)
   {
      if ((*iter)->IsA_TypeOf("WSF_EXPLICIT_WEAPON"))
      {
         WsfExplicitWeapon* countermeasure = dynamic_cast<WsfExplicitWeapon*>(*iter);

         if (countermeasure != nullptr)
         {
            if (countermeasure->IsA_TypeOf("WSF_CHAFF_WEAPON"))
            {
               if (mDataPtr->mChaffDispenser == nullptr)
               {
                  mDataPtr->mChaffDispenser = countermeasure;
               }
            }

            // TODO -- In the future, there may be other CM types, such as WSF_FLARE_WEAPON
            // and WSF_DECOY_WEAPON. If those are ever introduced, additional tests
            // for those types should be performed here, similar to the WSF_CHAFF_WEAPON above.
         }
      }
   }
}

void WsfSA_Perceive::FindWeapons()
{
   // Loop through components that are weapons
   for (WsfComponentList::RoleIterator<WsfWeapon> iter(*GetPlatform()); !iter.AtEnd(); ++iter)
   {
      if ((*iter)->IsA_TypeOf("WSF_EXPLICIT_WEAPON"))
      {
         // Get explicit weapons only
         WsfExplicitWeapon* weapon = dynamic_cast<WsfExplicitWeapon*>(*iter);

         if (weapon != nullptr)
         {
            // We need to ignore countermeasures that are modeled as explicit weapons

            // TODO -- At the present time, only WSF_CHAFF_WEAPON expendable countermeasures are
            // available. In the future, there may be other CM types, such as WSF_FLARE_WEAPON
            // and WSF_DECOY_WEAPON. If those are ever introduced, additional tests
            // for those types should be performed here, similar to the WSF_CHAFF_WEAPON below.

            if (!weapon->IsA_TypeOf("WSF_CHAFF_WEAPON"))
            {
               mDataPtr->mWeaponList.push_back(weapon);
            }
         }
      }
   }
}

void WsfSA_Perceive::CreateAssetsList(double aSimTime, std::list<WsfSA_EntityPerception*>& aTempAssetsList, bool aUnlimitedAssets)
{
   // First, filter-out assets that are not valid, meaning they lack a perceived index
   for (auto it = aTempAssetsList.begin(); it != aTempAssetsList.end();)
   {
      WsfSA_EntityPerception* entity = *it;

      // Remove any asset that is not valid
      if (entity->GetPerceivedIndex() < 1)
      {
         // Remove this asset
         it = aTempAssetsList.erase(it);

         // Delete the entity
         delete entity;
      }
      else
      {
         ++it;
      }
   }

   // Then filter out any ignored assets based on platform category
   if (mDataPtr->mIgnoredAssetCategories.GetCategoryList().size() > 0)
   {
      for (auto it = aTempAssetsList.begin(); it != aTempAssetsList.end();)
      {
         WsfSA_EntityPerception* entity = *it;

         WsfPlatform* platformPtr = GetSimulation()->GetPlatformByIndex(entity->GetPerceivedIndex());
         if (platformPtr != nullptr)
         {
            if (mDataPtr->mIgnoredAssetCategories.Intersects(platformPtr->GetCategories()))
            {
               // Remove this asset
               it = aTempAssetsList.erase(it);

               // Delete the entity
               delete entity;
            }
            else
            {
               ++it;
            }
         }
      }
   }

   // Now, ensure that "self" is not included, if not perceiving self
   if (mDataPtr->mPerceiveSelfStatus != true)
   {
      size_t platIndex = GetPlatform()->GetIndex();
      for (auto it = aTempAssetsList.begin(); it != aTempAssetsList.end();)
      {
         WsfSA_EntityPerception* entity = *it;

         if ((entity->GetEntityPlatformIndex() == platIndex) || (entity->GetPerceivedIndex() == platIndex) ||
             (entity->GetTargetPlatformIndex() == platIndex))
         {
            // Remove this asset
            it = aTempAssetsList.erase(it);

            // Delete the entity
            delete entity;
         }
         else
         {
            ++it;
         }
      }
   }

   // Next, filter-out assets by range, if appropriate
   if (GetSAP()->UseMaxRangeFilterForPerceivedAssets())
   {
      for (auto it = aTempAssetsList.begin(); it != aTempAssetsList.end();)
      {
         WsfSA_EntityPerception* entity = *it;

         // Remove any assets that are out of range
         if (entity->CalcRangeForPlatform(GetPlatform()) > GetSAP()->MaxRangeForPerceivedAssets())
         {
            // Remove this asset
            it = aTempAssetsList.erase(it);

            // Delete the entity
            delete entity;
         }
         else
         {
            ++it;
         }
      }
   }

   // Next, ensure that this is marked as an asset
   for (auto& assetPtr : aTempAssetsList)
   {
      assetPtr->SetFriendlyAsset(true);
   }

   for (auto& asset : mDataPtr->mPerceivedAssets)
   {
      if (!asset->GetIsMissile())
      {
         asset->SetIsCoasting(true);
      }
   }

   // If we are not limiting assets, simply add the full list
   if (aUnlimitedAssets)
   {
      for (auto it = aTempAssetsList.begin(); it != aTempAssetsList.end();)
      {
         WsfSA_EntityPerception* entity = *it;

         // Remove this asset
         it = aTempAssetsList.erase(it);

         // Add it to the list of perceived assets
         AddOrUpdatePerception(mDataPtr->mPerceivedAssets, *entity, aSimTime);
      }

      SetOldPerceptionsAsCoasting(mDataPtr->mPerceivedAssets, aSimTime);

      // Clear out any remaining items
      for (auto& remainingAsset : aTempAssetsList)
      {
         delete remainingAsset;
      }
      aTempAssetsList.clear();

      return;
   }

   // This is where we will place assets that require further consideration
   std::list<WsfSA_EntityPerception*> assetsToConsider{};

   bool customScriptScoringInUse = false;
   for (auto it = aTempAssetsList.begin(); it != aTempAssetsList.end();)
   {
      WsfSA_EntityPerception* entity = *it;

      double score = 0.0;
      if (AssetScore(aSimTime, entity, score)) // Custom script scoring
      {
         // Use custom user script for asset scoring (do not use important/unimportant feature)
         if (score > 0)
         {
            customScriptScoringInUse = true;

            // Remove this asset
            it = aTempAssetsList.erase(it);

            // Add it to the list of assets to consider
            entity->SetSelectionScore(score);
            assetsToConsider.emplace_back(entity);
         }
         else
         {
            // Skip it - zero or negative user score means threat should be ignored
            ++it;
         }
      }
      else if (Important(entity->GetEntityPlatformIndex()))
      {
         // Remove this asset
         it = aTempAssetsList.erase(it);

         AddOrUpdatePerception(mDataPtr->mPerceivedAssets, *entity, aSimTime);

         if (mDataPtr->mMaxAssetLoad > 0 && (int)mDataPtr->mPerceivedAssets.size() >= mDataPtr->mMaxAssetLoad)
         {
            break; // Break out of this loop, since we've reached the maximum
         }
      }
      else if (Unimportant(entity->GetEntityPlatformIndex()))
      {
         // Skip it -- we want to ignore this one
         ++it;
      }
      else
      {
         // Remove this asset
         it = aTempAssetsList.erase(it);

         // This asset is something to consider further
         assetsToConsider.emplace_back(entity);
      }
   }

   // Clear out any remaining items
   for (auto& remainingAsset : aTempAssetsList)
   {
      delete remainingAsset;
   }
   aTempAssetsList.clear();

   if (customScriptScoringInUse)
   {
      // If we are using custom script scoring, sort the list, based on high score
      assetsToConsider.sort(WsfSA_EntityPerception::CompareScoreHighGood);
   }
   else
   {
      // In this case, we use the range (calculated above) to sort the list, based on range scores.
      // We want low range, so use the "low good" variant.
      assetsToConsider.sort(WsfSA_EntityPerception::CompareScoreLowGood);
   }

   // Now add assets to the list until we hit the max
   for (auto it = assetsToConsider.begin(); it != assetsToConsider.end();)
   {
      WsfSA_EntityPerception* entity = *it;

      if (static_cast<int>(mDataPtr->mPerceivedAssets.size()) < mDataPtr->mMaxAssetLoad)
      {
         // Remove this asset
         it = assetsToConsider.erase(it);

         // Add the asset
         AddOrUpdatePerception(mDataPtr->mPerceivedAssets, *entity, aSimTime);
      }
      else
      {
         break; // Hit the limit, break out
      }
   }

   SetOldPerceptionsAsCoasting(mDataPtr->mPerceivedAssets, aSimTime);

   // Clear out any remaining items
   for (auto& remainingAsset : assetsToConsider)
   {
      delete remainingAsset;
   }
   assetsToConsider.clear();
}

void WsfSA_Perceive::CreateBanditsList(double                              aSimTime,
                                       std::list<WsfSA_EntityPerception*>& aTrackedEntitiesList,
                                       bool                                aUnlimitedBandits)
{
   // This is where we will place threats that were removed from the combined bogie/threat list
   std::list<WsfSA_EntityPerception*> threatSublist{};

   // Now, we want to pull the *bandits* from the combined bogie/threat list (aTrackedEntitiesList)
   // and add the bandits to a bandit-only sublist. Any remaining items in aTrackedEntitiesList
   // will be bogies and will be left in the list. They will be used in a later call to CreateBogiesList().

   for (auto it = aTrackedEntitiesList.begin(); it != aTrackedEntitiesList.end();)
   {
      WsfSA_EntityPerception* entity = *it;

      bool addedBandit = false;

      // Track is a bandit if the type is known to be an enemy type
      if (entity->GetTrackPtr() != nullptr)
      {
         if (entity->GetTrackPtr()->TypeIdValid())
         {
            std::string trackType = entity->GetTrackPtr()->GetTypeId().GetString();

            // Check for simple mode
            if (GetSAP()->UseSimpleIdByType())
            {
               // This provides a simulated, simple identification by type. Basically, when using
               // the "simple" approach, we confirm that the track supports type ID, and then we
               // check the target platform truth "side", even if the side data is not necessarily
               // provided in the track.

               size_t       tgtIndex    = entity->GetTrackPtr()->GetTargetIndex();
               WsfPlatform* tgtPlatform = GetSimulation()->GetPlatformByIndex(tgtIndex);
               if (tgtPlatform != nullptr)
               {
                  std::string truthSide = tgtPlatform->GetSide();

                  if (GetSAP()->IsEnemySide(truthSide))
                  {
                     // This is a bandit, set the identification to BANDIT, and mark it for addition
                     // to the temporary threat list
                     entity->SetIdentification(WsfSA_EntityPerception::BANDIT);
                     entity->SetIdTypeFoe();
                     addedBandit = true;
                  }
                  else if (GetSAP()->IsFriendlySide(truthSide))
                  {
                     // This is a friend, set the identification to FRIENDLY
                     entity->SetIdentification(WsfSA_EntityPerception::FRIENDLY);
                     entity->SetIdTypeFriend();
                     // Note that we do not set addedBandit
                  }
                  else if (GetSAP()->IsNeutralSide(truthSide))
                  {
                     // This is a neutral, set the identification to NEUTRAL
                     entity->SetIdentification(WsfSA_EntityPerception::NEUTRAL);
                     entity->SetIdTypeNeutral();
                     // Note that we do not set addedBandit
                  }

                  // Set missile flag, if appropriate
                  if (GetSAP()->IsMissileType(trackType))
                  {
                     // This is a missile
                     entity->SetIsMissile(true);
                  }
               }
            }
            else
            {
               if (GetSAP()->IsEnemyType(trackType))
               {
                  // This is a bandit, set the identification to BANDIT, and mark it for addition
                  // to the temporary threat list
                  entity->SetIdentification(WsfSA_EntityPerception::BANDIT);
                  entity->SetIdTypeFoe();
                  addedBandit = true;
               }
               else if (GetSAP()->IsFriendlyType(trackType))
               {
                  // This is a friend, set the identification to FRIENDLY
                  entity->SetIdentification(WsfSA_EntityPerception::FRIENDLY);
                  entity->SetIdTypeFriend();
                  // Note that we do not set addedBandit
               }
               else if (GetSAP()->IsNeutralType(trackType))
               {
                  // This is a neutral, set the identification to NEUTRAL
                  entity->SetIdentification(WsfSA_EntityPerception::NEUTRAL);
                  entity->SetIdTypeNeutral();
                  // Note that we do not set addedBandit
               }

               // Set missile flag, if appropriate
               if (GetSAP()->IsMissileType(trackType))
               {
                  // This is a missile
                  entity->SetIsMissile(true);
               }
            }

            // Check for missile. Note -- If the list of missile types is not populated (possibly
            // because the simple type ID is being used), we will not be able to identify missiles
            // by type.
            if (GetSAP()->IsMissileType(trackType))
            {
               entity->SetIsMissile(true);
            }
         }
      }

      // Track is a bandit if the side is known and is the enemy
      if (!addedBandit)
      {
         if (entity->GetTrackPtr() != nullptr)
         {
            if (entity->GetTrackPtr()->SideIdValid())
            {
               std::string sideString = entity->GetTrackPtr()->GetSideId().GetString();

               if (GetSAP()->IsEnemySide(sideString))
               {
                  // This is a bandit, set the identification to BANDIT, and
                  // mark it for addition to the temporary threat list
                  entity->SetIdentification(WsfSA_EntityPerception::BANDIT);
                  entity->SetIdSideFoe();
                  addedBandit = true;
               }
               else if (GetSAP()->IsFriendlySide(sideString))
               {
                  // This is a friend, set the identification to FRIENDLY
                  entity->SetIdentification(WsfSA_EntityPerception::FRIENDLY);
                  entity->SetIdSideFriend();
                  // Note that we do not set addedBandit
               }
               else if (GetSAP()->IsNeutralSide(sideString))
               {
                  // This is a neutral, set the identification to NEUTRAL
                  entity->SetIdentification(WsfSA_EntityPerception::NEUTRAL);
                  entity->SetIdSideNeutral();
                  // Note that we do not set addedBandit
               }
            }
         }
      }

      // Track is a bandit if the IFF is FOE
      if (!addedBandit)
      {
         if (entity->GetTrackPtr() != nullptr)
         {
            if (GetSAP()->UseIFF() && entity->GetTrackPtr()->IFF_StatusValid())
            {
               if (entity->GetTrackPtr()->GetIFF_Status() == WsfTrack::cIFF_FOE)
               {
                  // This is a bandit, set the identification to BANDIT, and mark it
                  // for addition to the temporary threat list
                  entity->SetIdentification(WsfSA_EntityPerception::BANDIT);
                  entity->SetIdIffFoe();
                  addedBandit = true;
               }
               else if (entity->GetTrackPtr()->GetIFF_Status() == WsfTrack::cIFF_FRIEND)
               {
                  // This is a friend, set the identification to FRIENDLY
                  entity->SetIdentification(WsfSA_EntityPerception::FRIENDLY);
                  entity->SetIdIffFriend();
                  // Note that we do not set addedBandit
               }
               else if (entity->GetTrackPtr()->GetIFF_Status() == WsfTrack::cIFF_NEUTRAL)
               {
                  // This is a neutral, set the identification to NEUTRAL
                  entity->SetIdentification(WsfSA_EntityPerception::NEUTRAL);
                  entity->SetIdIffNeutral();
                  // Note that we do not set addedBandit
               }
            }
         }
      }

      // If the track is not perceived as a missile, check the MWS track processor.
      // Any track reported by the MWS is assumed to be a missile.
      WsfPlatform* tgtPlatform = GetSimulation()->GetPlatformByIndex(entity->GetEntityPlatformIndex());
      if (!entity->GetIsMissile() && (tgtPlatform != nullptr))
      {
         if (GetSAP()->PlatformInTrackListMWS(tgtPlatform))
         {
            entity->SetIsMissile(true);
         }
      }

      // Check for multiple, differing identifications. Id priority is as follows:  Type, Side, IFF, and Aux

      // First, check if anything has marked this a friend
      bool somethingMarkedThisAsFriend = false;
      if (entity->GetIdTypeFriend() || entity->GetIdSideFriend() || entity->GetIdIffFriend() || entity->GetIdAuxFriend())
      {
         somethingMarkedThisAsFriend = true;
      }

      // Next, check if anything has marked this a foe
      bool somethingMarkedThisAsFoe = false;
      if (entity->GetIdTypeFoe() || entity->GetIdSideFoe() || entity->GetIdIffFoe() || entity->GetIdAuxFoe())
      {
         somethingMarkedThisAsFoe = true;
      }

      // Check for only friend
      if (somethingMarkedThisAsFriend && !somethingMarkedThisAsFoe)
      {
         // This should be identified as a friend
         entity->SetIdentification(WsfSA_EntityPerception::FRIENDLY);
      }
      else
      {
         // Check for only foe
         if (!somethingMarkedThisAsFriend && somethingMarkedThisAsFoe)
         {
            // This should be identified as a foe
            entity->SetIdentification(WsfSA_EntityPerception::BANDIT);
         }
         else if (somethingMarkedThisAsFriend && somethingMarkedThisAsFoe) // Both
         {
            // Check priority
            if (entity->GetIdTypeFriend())
            {
               entity->SetIdentification(WsfSA_EntityPerception::FRIENDLY);
            }
            else if (entity->GetIdTypeFoe())
            {
               entity->SetIdentification(WsfSA_EntityPerception::BANDIT);
            }

            if (entity->GetIdSideFriend())
            {
               entity->SetIdentification(WsfSA_EntityPerception::FRIENDLY);
            }
            else if (entity->GetIdSideFoe())
            {
               entity->SetIdentification(WsfSA_EntityPerception::BANDIT);
            }

            if (entity->GetIdIffFriend())
            {
               entity->SetIdentification(WsfSA_EntityPerception::FRIENDLY);
            }
            else if (entity->GetIdIffFoe())
            {
               entity->SetIdentification(WsfSA_EntityPerception::BANDIT);
            }

            if (entity->GetIdAuxFriend())
            {
               entity->SetIdentification(WsfSA_EntityPerception::FRIENDLY);
            }
            else if (entity->GetIdAuxFoe())
            {
               entity->SetIdentification(WsfSA_EntityPerception::BANDIT);
            }
         }
         else if (!somethingMarkedThisAsFriend && !somethingMarkedThisAsFoe) // Neither
         {
            // Check for neutral
            if (entity->GetIdTypeNeutral() || entity->GetIdSideNeutral() || entity->GetIdIffNeutral() ||
                entity->GetIdAuxNeutral())
            {
               entity->SetIdentification(WsfSA_EntityPerception::NEUTRAL);
            }
         }
      }

      // Completed the tests for bandits.
      // Now, if this entity is a bandit, add it to the special threat list (threatSublist) and
      // remove it from aTrackedEntitiesList.
      if (addedBandit)
      {
         // Remove threat from list
         it = aTrackedEntitiesList.erase(it);

         // Add this threat to the temporary threat list
         threatSublist.emplace_back(entity);
      }
      else
      {
         ++it;
      }
   }

   for (auto& threat : mDataPtr->mPerceivedBandits)
   {
      threat->SetIsCoasting(true);
   }

   // Check if we have any threats in the special list, if not, we're done
   if (threatSublist.empty())
   {
      SetOldPerceptionsAsCoasting(mDataPtr->mPerceivedBandits, aSimTime);
      return;
   }

   // We now have a list of bandits (threatSublist) and now use that list to
   // create the perceived bandit list

   // First, set the hostile flag, if appropriate
   for (auto& threatPtr : threatSublist)
   {
      // Use aux data to determine if the track has been marked as hostile
      if (threatPtr->GetTrackPtr()->HasAuxData())
      {
         WsfAttributeContainer& attributes = threatPtr->GetTrackPtr()->GetAuxData();
         if (attributes.AttributeExists("HOSTILE"))
         {
            // Mark as hostile
            threatPtr->SetIsHostile(true);
         }
      }
   }

   // If we are not limiting threats, simply add to the perceived threat list
   if (aUnlimitedBandits && mDataPtr->mMaxThreatLoad < 0)
   {
      for (auto it = threatSublist.begin(); it != threatSublist.end();)
      {
         WsfSA_EntityPerception* entity = *it;

         // Remove this threat
         it = threatSublist.erase(it);

         // Add it to the list of perceived threats
         AddOrUpdatePerception(mDataPtr->mPerceivedBandits, *entity, aSimTime);
      }

      SetOldPerceptionsAsCoasting(mDataPtr->mPerceivedBandits, aSimTime);

      // Clear out any remaining items
      for (auto& remainingThreat : threatSublist)
      {
         delete remainingThreat;
      }
      threatSublist.clear();

      return;
   }

   // This is where we will place threats that require further consideration
   std::list<WsfSA_EntityPerception*> threatsToConsider{};

   for (auto it = threatSublist.begin(); it != threatSublist.end();)
   {
      WsfSA_EntityPerception* entity    = *it;
      bool                    isMissile = entity->GetIsMissile();

      entity->SetSelectionScore(GetThreatScore(aSimTime, entity, isMissile));
      if (Important(entity->GetTrackPtr()))
      {
         entity->SetImportance(WsfSA_PerceivedItem::IMPORTANT);
         // Remove this threat
         it = threatSublist.erase(it);

         // Add to the perceived threats -- it uses the range score calculated above
         // using CalcRangeForPlatform
         AddOrUpdatePerception(mDataPtr->mPerceivedBandits, *entity, aSimTime);
      }
      else if (Unimportant(entity->GetTrackPtr()) || entity->GetSelectionScore() <= 0.0)
      {
         // Skip it -- we want to ignore this one
         entity->SetImportance(WsfSA_PerceivedItem::IGNORED);
         ++it;
      }
      else
      {
         // Remove this threat
         it = threatSublist.erase(it);

         // This threat is something to consider further
         threatsToConsider.emplace_back(entity);
      }
   }

   // Clear out any remaining items
   for (auto& remainingThreat : threatSublist)
   {
      delete remainingThreat;
   }
   threatSublist.clear();

   if (mConsiderationRandomness > 0.0)
   {
      auto dist = std::uniform_real_distribution<double>(-mConsiderationRandomness, mConsiderationRandomness);
      for (auto& threat : threatsToConsider)
      {
         threat->SetSelectionScore(threat->GetSelectionScore() + dist(mRandom));
      }
   }

   // Update all bandits with the new data
   for (auto it = threatsToConsider.begin(); it != threatsToConsider.end();)
   {
      WsfSA_EntityPerception* entity = *it;
      // Remove this bandit
      it = threatsToConsider.erase(it);

      // Add the bandit to the list
      AddOrUpdatePerception(mDataPtr->mPerceivedBandits, *entity, aSimTime);
   }

   // Because we inverted the selection scores earlier, this line works for
   // both custom scoring and the default scoring (based on range).
   std::stable_sort(mDataPtr->mPerceivedBandits.begin(),
                    mDataPtr->mPerceivedBandits.end(),
                    WsfSA_EntityPerception::CompareScoreHighGood);

   // Update those that need it, set others to coast
   SetOldPerceptionsAsCoasting(mDataPtr->mPerceivedBandits, aSimTime);

   // Clear out any remaining items
   for (auto& remainingThreat : threatsToConsider)
   {
      delete remainingThreat;
   }
   threatsToConsider.clear();
}

void WsfSA_Perceive::CreateBogiesList(double                              aSimTime,
                                      std::list<WsfSA_EntityPerception*>& aTrackedEntitiesList,
                                      bool                                aUnlimitedBogies)
{
   // Note: In CreateThreatsList (which should be called prior to this function) has already:
   //        -- Filtered out bad tracks
   //        -- Filtered out tracks based on range
   //        -- Filtered out "threat" tracks
   //
   //        As a result, the aTrackedEntitiesList contains bogies that can be evaluated further.

   for (auto& threat : mDataPtr->mPerceivedBogies)
   {
      threat->SetIsCoasting(true);
   }

   // Check if we have any tracks remaining in the list, if not, we're done
   if (aTrackedEntitiesList.empty())
   {
      SetOldPerceptionsAsCoasting(mDataPtr->mPerceivedBogies, aSimTime);
      return;
   }

   // We now use the list of bogies (aTrackedEntitiesList) to create the perceived bogie list
   // If we are not limiting bogies, simply add to the perceived bogie list
   if (aUnlimitedBogies && mDataPtr->mMaxThreatLoad < 0)
   {
      for (auto it = aTrackedEntitiesList.begin(); it != aTrackedEntitiesList.end();)
      {
         WsfSA_EntityPerception* entity = *it;

         // Remove this bogie
         it = aTrackedEntitiesList.erase(it);

         // Add it to the list of perceived bogies
         AddOrUpdatePerception(mDataPtr->mPerceivedBogies, *entity, aSimTime);
      }

      SetOldPerceptionsAsCoasting(mDataPtr->mPerceivedBogies, aSimTime);

      // Clear out any remaining items
      for (auto& remainingBogie : aTrackedEntitiesList)
      {
         delete remainingBogie;
      }
      aTrackedEntitiesList.clear();

      return;
   }

   // This is where we will place bogies that require further consideration
   std::list<WsfSA_EntityPerception*> bogiesToConsider{};

   for (auto it = aTrackedEntitiesList.begin(); it != aTrackedEntitiesList.end();)
   {
      WsfSA_EntityPerception* entity    = *it;
      bool                    isMissile = entity->GetIsMissile();

      // This is used with the custom script scoring (below)
      // double score = 0.0;
      entity->SetSelectionScore(GetThreatScore(aSimTime, entity, isMissile));
      if (entity->GetPerceivedIndex() == GetPlatform()->GetIndex())
      {
         // Remove the ownship/self bogie, we always filter-out the platform itself),
         // but do *not* add it to the list of bogies to consider
         it = aTrackedEntitiesList.erase(it);
      }
      else if (mDataPtr->mFilterAssetsFromBogies && EntityIsInAssetGroup(entity))
      {
         // Remove this asset bogie (we want to filter-out assets), but do *not* add it
         // to the list of bogies to consider
         it = aTrackedEntitiesList.erase(it);
      }
      else if (Important(entity->GetTrackPtr()))
      {
         // Remove this bogie
         it = aTrackedEntitiesList.erase(it);

         // Add to the perceived threats -- it uses the default score calculated above
         // using CalcRangeForPlatform
         // Add it to the list of perceived bogies
         AddOrUpdatePerception(mDataPtr->mPerceivedBogies, *entity, aSimTime);
      }
      else if (Unimportant(entity->GetTrackPtr()) || entity->GetSelectionScore() <= 0.0)
      {
         // Skip it -- we want to ignore this one
         ++it;
      }
      else
      {
         // Remove this bogie
         it = aTrackedEntitiesList.erase(it);

         // Add it to the list of bogies to consider
         bogiesToConsider.emplace_back(entity);
      }
   }

   // Clear out any remaining items
   for (auto& remainingBogie : aTrackedEntitiesList)
   {
      delete remainingBogie;
   }
   aTrackedEntitiesList.clear();

   if (mConsiderationRandomness > 0.0)
   {
      auto dist = std::uniform_real_distribution<double>(-mConsiderationRandomness, mConsiderationRandomness);
      for (auto& threat : bogiesToConsider)
      {
         threat->SetSelectionScore(threat->GetSelectionScore() + dist(mRandom));
      }
   }

   // Update all bandits with the new data
   for (auto it = bogiesToConsider.begin(); it != bogiesToConsider.end();)
   {
      WsfSA_EntityPerception* entity = *it;
      // Remove this bogie
      it = bogiesToConsider.erase(it);

      // Add the bandit to the list
      AddOrUpdatePerception(mDataPtr->mPerceivedBogies, *entity, aSimTime);
   }

   // Because we inverted the selection scores earlier, this line works for
   // both custom scoring and the default scoring (based on range).
   std::stable_sort(mDataPtr->mPerceivedBogies.begin(),
                    mDataPtr->mPerceivedBogies.end(),
                    WsfSA_EntityPerception::CompareScoreHighGood);

   SetOldPerceptionsAsCoasting(mDataPtr->mPerceivedBogies, aSimTime);
   // Clear out any remaining items
   for (auto& remainingBogie : bogiesToConsider)
   {
      delete remainingBogie;
   }
   bogiesToConsider.clear();
}

double WsfSA_Perceive::GetThreatScore(double aSimTime, WsfSA_EntityPerception* aThreatPtr, bool aIsMissile)
{
   double score = 0.0;
   if (!ThreatScore(aSimTime, aThreatPtr->GetTrackPtr(), aIsMissile, score))
   {
      return DefaultThreatScore(aSimTime, aThreatPtr);
   }
   return score;
}

void WsfSA_Perceive::CreateAssetSublists()
{
   for (auto& asset : mDataPtr->mPerceivedAssets)
   {
      if (asset->GetIsMissile())
      {
         // Add to missile list
         mDataPtr->mPerceivedMissileAssets.push_back(new WsfSA_EntityPerception(*asset));
      }
      else
      {
         // Add to aircraft list
         mDataPtr->mPerceivedAircraftAssets.push_back(new WsfSA_EntityPerception(*asset));
      }
   }
}

void WsfSA_Perceive::CreateSublists()
{
   for (auto& bogie : mDataPtr->mPerceivedBogies)
   {
      if (bogie->GetIsMissile())
      {
         // Add to missile list
         mDataPtr->mPerceivedMissileBogies.push_back(new WsfSA_EntityPerception(*bogie));
      }
      else
      {
         // Add to aircraft list
         mDataPtr->mPerceivedAircraftBogies.push_back(new WsfSA_EntityPerception(*bogie));
      }
   }

   for (auto& bandit : mDataPtr->mPerceivedBandits)
   {
      if (bandit->GetIsMissile())
      {
         // Add to missile list
         mDataPtr->mPerceivedMissileBandits.push_back(new WsfSA_EntityPerception(*bandit));
      }
      else
      {
         // Add to aircraft list
         mDataPtr->mPerceivedAircraftBandits.push_back(new WsfSA_EntityPerception(*bandit));
      }
   }
}

void WsfSA_Perceive::RemoveExpiredCoastingPerceptions(std::vector<WsfSA_EntityPerception*>& aPerceptionList,
                                                      double                                aCoastTime,
                                                      double                                aSimTime)
{
   double timeSpentCoasting = 0.0;
   for (auto it = aPerceptionList.begin(); it != aPerceptionList.end();)
   {
      if ((*it)->GetIsCoasting())
      {
         // Use the last update time to determine if this perception
         // has been coasting for too long.
         timeSpentCoasting = aSimTime - (*it)->GetLastUpdateTime();
         if (timeSpentCoasting >= aCoastTime)
         {
            DeleteLater(*it);
            it = aPerceptionList.erase(it);
         }
         else
         {
            ++it;
         }
      }
      else
      {
         ++it;
      }
   }
}

void WsfSA_Perceive::SetOldPerceptionsAsCoasting(const std::vector<WsfSA_EntityPerception*>& aPerceptionList, double aSimTime)
{
   for (auto& perception : aPerceptionList)
   {
      // Assets
      if (perception->GetEntityPlatformIndex() > 0)
      {
         if (perception->GetIsCoasting())
         {
            perception->SetLastExtrapolationTime(aSimTime);
         }
      }
      // Threats (and everything else)
      else
      {
         // exclusive OR
         if (perception->GetIsCoasting() && perception->GetTrackPtr() != nullptr)
         {
            perception->SetTrackPtr(nullptr);
            perception->SetLastExtrapolationTime(aSimTime);
         }
         else if (!perception->GetIsCoasting() && perception->GetTrackPtr() == nullptr)
         {
            perception->SetIsCoasting(true);
            perception->SetLastExtrapolationTime(aSimTime);
         }
      }
   }
}

void WsfSA_Perceive::UpdateCoastingPerceptions(const std::vector<WsfSA_EntityPerception*>& aPerceptionList,
                                               const std::list<WsfSA_EntityPerception*>&   aNewPerceptionList,
                                               double                                      aSimTime)
{
   // For each currently perceived entity, check if there's an update for it in the
   // new perception list. If there isn't, mark the currently perceived entity as coasting.
   bool setCoasting;
   for (auto& perceivedEntity : aPerceptionList)
   {
      // Prevent friendly missiles from coasting.
      if (perceivedEntity->GetFriendlyAsset() && perceivedEntity->GetIsMissile())
      {
         setCoasting = false;
      }
      else
      {
         setCoasting = true;
      }

      if (setCoasting)
      {
         if (perceivedEntity->GetIsCoasting())
         {
            double newLat, newLon = 0.0;
            double timeSpentMoving = aSimTime - perceivedEntity->GetLastExtrapolationTime();
            double distance        = UtSpeed(perceivedEntity->GetSpeed_kts(), UtSpeed::FmtKTS) * timeSpentMoving;

            UtEllipsoidalEarth::ExtrapolateGreatEllipsePosition(perceivedEntity->GetLat_deg(),
                                                                perceivedEntity->GetLon_deg(),
                                                                perceivedEntity->GetHeading_deg(),
                                                                distance,
                                                                newLat,
                                                                newLon);
            perceivedEntity->SetLat_deg(newLat);
            perceivedEntity->SetLon_deg(newLon);

            perceivedEntity->SetLastExtrapolationTime(aSimTime);
         }
      }
   }
}

void WsfSA_Perceive::AddOrUpdatePerception(std::vector<WsfSA_EntityPerception*>& aPerceptionList,
                                           WsfSA_EntityPerception&               aPerception,
                                           double                                aSimTime)
{
   // If a matching entity perception exists in the given list, then update that perception's
   // data and set the last update time to now. Otherwise, just add the new perception to the list.
   auto sameEntity = [aPerception](const WsfSA_EntityPerception* aPerceptionToCompare)
   { return aPerception.IsSame(aPerceptionToCompare); };

   auto foundPerception = std::find_if(aPerceptionList.begin(), aPerceptionList.end(), sameEntity);
   if (foundPerception != aPerceptionList.end())
   {
      // Update the perception in the list.
      *(*foundPerception) = aPerception;
      (*foundPerception)->SetLastUpdateTime(aSimTime);

      // The temporary perception is no longer needed.
      delete &aPerception;
   }
   else
   {
      aPerceptionList.emplace_back(&aPerception);
   }
}

void WsfSA_Perceive::JoinInitialGroups(std::vector<WsfSA_EntityPerception*>& aPerceptionList,
                                       std::list<WsfSA_Group*>&              aGroupList,
                                       double                                aSimTime)
{
   // First, try to join an existing group.
   for (auto& entity : aPerceptionList)
   {
      if (!entity->GetParentGroup())
      {
         if (!mGroupManager.AttemptToJoinBestGroup(entity, aGroupList))
         {
            // Failing that, create a degenerate group.
            mGroupManager.FormDegenerateGroup(entity, aGroupList);
         }
      }
   }
}

void WsfSA_Perceive::DropEntity(WsfSA_EntityPerception* aOldEntity)
{
   if (std::find(mDeferredForDeletion.begin(), mDeferredForDeletion.end(), aOldEntity) != mDeferredForDeletion.end())
   {
      // Don't double up on deleting entities.
      return;
   }

   auto sameEntity = [aOldEntity](const WsfSA_EntityPerception* aPerceptionToCompare)
   { return aOldEntity->IsSame(aPerceptionToCompare); };

   // Depending on which type of entity it is, remove it from its respective list.
   switch (aOldEntity->GetIdentification())
   {
   case WsfSA_PerceivedItem::BANDIT:
   {
      auto it = std::find_if(mDataPtr->mPerceivedBandits.begin(), mDataPtr->mPerceivedBandits.end(), sameEntity);
      if (it != mDataPtr->mPerceivedBandits.end())
      {
         DeleteLater(*it);
         mDataPtr->mPerceivedBandits.erase(it);
      }
      break;
   }
   case WsfSA_PerceivedItem::BOGIE:
   {
      auto it = std::find_if(mDataPtr->mPerceivedBogies.begin(), mDataPtr->mPerceivedBogies.end(), sameEntity);
      if (it != mDataPtr->mPerceivedBogies.end())
      {
         DeleteLater(*it);
         mDataPtr->mPerceivedBogies.erase(it);
      }
      break;
   }
   case WsfSA_PerceivedItem::FRIENDLY:
   {
      auto it = std::find_if(mDataPtr->mPerceivedAssets.begin(), mDataPtr->mPerceivedAssets.end(), sameEntity);
      if (it != mDataPtr->mPerceivedAssets.end())
      {
         DeleteLater(*it);
         mDataPtr->mPerceivedAssets.erase(it);
      }
      break;
   }
   default:
      break;
   }
}

void WsfSA_Perceive::DeleteLater(WsfSA_EntityPerception* aOldEntity)
{
   mDeferredForDeletion.push_back(aOldEntity);
}

void WsfSA_Perceive::DeferredDelete()
{
   for (auto i : mDeferredForDeletion)
   {
      delete i;
   }
   mDeferredForDeletion.clear();
}
