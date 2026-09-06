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
//****************************************************************************
// Updated by Infoscitex, a DCS Company
//****************************************************************************

#include "WsfFusionCenter.hpp"

#include <cfloat>

#include "UtEntity.hpp"
#include "UtLog.hpp"
#include "UtRandom.hpp"
#include "WsfDefaultFusion.hpp"
#include "WsfEM_Interaction.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfEW_EP.hpp"
#include "WsfEW_SensorComponent.hpp"
#include "WsfFalseTarget.hpp"
#include "WsfFalseTargetScreener.hpp"
#include "WsfFusionStrategy.hpp"
#include "WsfFusionStrategyTypes.hpp"
#include "WsfMessage.hpp"
#include "WsfPlatform.hpp"
#include "WsfProcessor.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSensorObserver.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrackManager.hpp"
#include "WsfTrackMessage.hpp"
#include "WsfTrackObserver.hpp"

WsfFusionCenter::WsfFusionCenter(WsfScenario& aScenario)
   : WsfScriptProcessor(aScenario)
   , mActiveRT_Tracks()
   , mBlockedRT_Tracks()
   , mScreenersRT_Tracks()
   , mScreenersRT_IdList()
   , mActiveFT_Tracks()
   , mScreenersFT_Tracks()
   , mScreenersFT_IdList()
   , mCallbacks()
   , mConsistencyChanged(true)
   , mConsistencyConstrained(true)
   , mDebugEnabled(false)
   , mFalseTargetConsistency()
   , mLastFalseTargetConsistency()
   , mFrameTime(0.0)
   , mFusedFT_Number(0)
   , mFusedRT_Number(0)
   , mLastAggFT_Number(0)
   , mLastAggRT_Number(0)
   , mLastUpdateTime(0.0)
   , mOutput(cPLOT)
   , mPlotCapacity(2000)
   , mProduceFusedObjects(false)
   , mRealTargetConsistency()
   , mLastRealTargetConsistency()
   , mRegisteredSites()
   , mScreeners()
   , mStateChange(true)
   , mTrackCapacity(500)
   , mValidSites()
   , mFusionStrategyPtr(nullptr)
{
   mRejectionRatios[WsfFalseTarget::cRANDOM_TO_MULTIPLE_RADARS]     = 0.0;
   mRejectionRatios[WsfFalseTarget::cCONSISTENT_TO_MULTIPLE_RADARS] = 0.0;
   WsfFusionStrategyTypes& fusionTypes(WsfFusionStrategyTypes::Get(aScenario));
   mFusionStrategyPtr = fusionTypes.Create(fusionTypes.GetDefaultStrategyName());
}

WsfFusionCenter::WsfFusionCenter(const WsfFusionCenter& aSrc)
   : WsfScriptProcessor(aSrc)
   , mActiveRT_Tracks(aSrc.mActiveRT_Tracks)
   , mBlockedRT_Tracks(aSrc.mBlockedRT_Tracks)
   , mScreenersRT_Tracks(aSrc.mScreenersFT_Tracks)
   , mScreenersRT_IdList(aSrc.mScreenersRT_IdList)
   , mActiveFT_Tracks(aSrc.mActiveFT_Tracks)
   , mScreenersFT_Tracks(aSrc.mScreenersRT_Tracks)
   , mScreenersFT_IdList(aSrc.mScreenersRT_IdList)
   , mCallbacks(aSrc.mCallbacks)
   , mConsistencyChanged(aSrc.mConsistencyChanged)
   , mConsistencyConstrained(aSrc.mConsistencyConstrained)
   , mDebugEnabled(aSrc.mDebugEnabled)
   , mFalseTargetConsistency(aSrc.mFalseTargetConsistency)
   , mLastFalseTargetConsistency(aSrc.mLastFalseTargetConsistency)
   , mFrameTime(aSrc.mFrameTime)
   , mFusedFT_Number(aSrc.mFusedFT_Number)
   , mFusedRT_Number(aSrc.mFusedRT_Number)
   , mLastAggFT_Number(aSrc.mLastAggFT_Number)
   , mLastAggRT_Number(aSrc.mLastAggRT_Number)
   , mLastUpdateTime(aSrc.mLastUpdateTime)
   , mOutput(aSrc.mOutput)
   , mPlotCapacity(aSrc.mPlotCapacity)
   , mProduceFusedObjects(aSrc.mProduceFusedObjects)
   , mRealTargetConsistency(aSrc.mRealTargetConsistency)
   , mLastRealTargetConsistency(aSrc.mLastRealTargetConsistency)
   , mRegisteredSites(aSrc.mRegisteredSites)
   , mRejectionRatios(aSrc.mRejectionRatios)
   , mScreeners(aSrc.mScreeners)
   , mStateChange(aSrc.mStateChange)
   , mTrackCapacity(aSrc.mTrackCapacity)
   , mValidSites(aSrc.mValidSites)
   , mFusionStrategyPtr(aSrc.mFusionStrategyPtr->Clone())
{
}

// virtual
WsfFusionCenter::~WsfFusionCenter()
{
   mCallbacks.Clear();
   delete mFusionStrategyPtr;
}

// virtual
WsfProcessor* WsfFusionCenter::Clone() const
{
   return new WsfFusionCenter(*this);
}

// virtual
bool WsfFusionCenter::Initialize(double aSimTime)
{
   bool success = WsfScriptProcessor::Initialize(aSimTime);

   // Put any other initialize criteria here....

   if (success)
   {
      // Sign up to get notification of any sensor (anywhere) that gets turned on or off...
      mCallbacks.Add(WsfObserver::SensorModeActivated(GetSimulation()).Connect(&WsfFusionCenter::CheckForScreenerAdd, this));
      mCallbacks.Add(
         WsfObserver::SensorModeDeactivated(GetSimulation()).Connect(&WsfFusionCenter::CheckForScreenerReset, this));
   }

   return success;
}

bool WsfFusionCenter::AddScreener(WsfStringId aPlatformId, WsfFalseTargetScreener* aScreenerPtr)
{
   bool added = false;

   PlatformScreenerPair pltfrmScrnrKey = std::make_pair(aPlatformId, aScreenerPtr);
   auto                 screenerIter   = find(mScreeners.begin(), mScreeners.end(), pltfrmScrnrKey);
   if (screenerIter == mScreeners.end())
   {
      // register radar's screeners which are valid for this fusion center
      auto vsi = find(mValidSites.begin(), mValidSites.end(), aPlatformId);
      if (vsi != mValidSites.end())
      {
         mScreeners.push_back(pltfrmScrnrKey);

         WsfFalseTarget::RadarSnsrPair rsKey =
            WsfFalseTarget::RadarSnsrPair(WsfStringInt(aPlatformId),
                                          WsfStringInt(aScreenerPtr->GetSensorModePtr()->GetSensor()->GetNameId()));
         auto rsi = find(mRegisteredSites.begin(), mRegisteredSites.end(), rsKey);
         if (rsi == mRegisteredSites.end())
         {
            mRegisteredSites.push_back(rsKey);
         }

         if (mDebugEnabled)
         {
            auto out = ut::log::debug() << "Added Screener: ";
            out.AddNote() << "Platform: " << aPlatformId;
            out.AddNote() << "Screener: " << aScreenerPtr->GetName();
         }

         added = true;
      }
   }
   return added;
}

bool WsfFusionCenter::FindScreener(WsfStringId aPlatformId, WsfFalseTargetScreener* aScreenerPtr) const
{
   bool found = false;

   PlatformScreenerPair pltfrmScrnrKey = std::make_pair(aPlatformId, aScreenerPtr);
   auto                 screenerIter   = find(mScreeners.begin(), mScreeners.end(), pltfrmScrnrKey);
   if (screenerIter != mScreeners.end())
   {
      found = true;
   }
   return found;
}

bool WsfFusionCenter::DeleteScreener(WsfStringId aPlatformId, WsfFalseTargetScreener* aScreenerPtr)
{
   bool deleted = false;
   // Check to see if we have a screener pointer that is on the
   // specified platform.  If so, remove it.
   PlatformScreenerPair pltfrmScrnrKey = std::make_pair(aPlatformId, aScreenerPtr);
   auto                 screenerIter   = find(mScreeners.begin(), mScreeners.end(), pltfrmScrnrKey);
   if (screenerIter == mScreeners.end())
   {
      mScreeners.erase(screenerIter);

      WsfFalseTarget::RadarSnsrPair rsKey =
         WsfFalseTarget::RadarSnsrPair(WsfStringInt(aPlatformId),
                                       WsfStringInt(aScreenerPtr->GetSensorModePtr()->GetSensor()->GetNameId()));
      auto rsIter = find(mRegisteredSites.begin(), mRegisteredSites.end(), rsKey);
      if (rsIter != mRegisteredSites.end())
      {
         mRegisteredSites.erase(rsIter);
      }

      if (mDebugEnabled)
      {
         auto out = ut::log::debug() << "Deleted Screener: ";
         out.AddNote() << "Platfrom: " << aPlatformId;
         out.AddNote() << "Screener: " << aScreenerPtr->GetName();
      }

      deleted = true;
   }
   return deleted;
}

bool WsfFusionCenter::DeleteAllScreenersOn(WsfStringId aPlatformId)
{
   bool deleted = false;
   // Check to see if we have a screener pointer that is on the
   // specified platform.  If so, remove it.
   for (auto iter = mScreeners.begin(); iter != mScreeners.end();)
   {
      if ((*iter).first == aPlatformId)
      {
         iter = mScreeners.erase(iter);

         for (auto rsIter = mRegisteredSites.begin(); rsIter != mRegisteredSites.end();)
         {
            if ((*rsIter).first == WsfStringInt(aPlatformId))
            {
               mRegisteredSites.erase(rsIter);
               break;
            }
            else
            {
               ++rsIter;
            }
         }

         deleted = true;
      }
      else
      {
         ++iter;
      }
   }
   return deleted;
}

// virtual
bool WsfFusionCenter::ProcessInput(UtInput& aInput)
{
   bool   myCommand = true;
   double FTRejectionRatio;

   std::string command(aInput.GetCommand());

   if (command == "plot_capacity")
   {
      aInput.ReadValue(mPlotCapacity);
   }
   else if (command == "frame_time")
   {
      aInput.ReadValueOfType(mFrameTime, UtInput::cTIME);
   }
   else if (command == "track_capacity")
   {
      aInput.ReadValue(mTrackCapacity);
   }
   else if (command == "random_to_multiple_radars")
   {
      aInput.ReadValue(FTRejectionRatio);
      mRejectionRatios[WsfFalseTarget::cRANDOM_TO_MULTIPLE_RADARS] = FTRejectionRatio;
   }
   else if (command == "consistent_to_multiple_radars")
   {
      aInput.ReadValue(FTRejectionRatio);
      mRejectionRatios[WsfFalseTarget::cCONSISTENT_TO_MULTIPLE_RADARS] = FTRejectionRatio;
   }
   else if (command == "radar_site")
   {
      std::string siteName;
      aInput.ReadValue(siteName);
      WsfStringId siteId(siteName);
      if (!siteId.IsNull())
      {
         auto sidi = find(mValidSites.begin(), mValidSites.end(), siteId);
         if (sidi == mValidSites.end())
         {
            mValidSites.push_back(siteId);
         }
         else
         {
            throw UtInput::BadValue(aInput, "Duplicate radar_site '" + siteName);
         }
      }
   }
   else if (command == "consistency_constrained")
   {
      aInput.ReadValue(mConsistencyConstrained);
   }
   else if (command == "produce_fused_objects")
   {
      aInput.ReadValue(mProduceFusedObjects);
   }
   else if (command == "debug")
   {
      mDebugEnabled = true;
   }
   else
   {
      myCommand = WsfScriptProcessor::ProcessInput(aInput);
   }
   return myCommand;
}

// virtual
void WsfFusionCenter::Update(double aSimTime)
{
   if (aSimTime > mFrameTime + mLastUpdateTime)
   {
      mLastUpdateTime = aSimTime;

      // update the real and false target quantities based upon the
      //  aggregation of the screeners' tracks
      // modifies mStateChange
      UpdateState(aSimTime);

      UpdateFalseTargetTracks(aSimTime);
      UpdateRealTargetTracks(aSimTime);

      if (mStateChange)
      {
         // create/modify numbers of false target and real target tracks
         //   based  on the updated Fusion Center state
         // also create actual false target platforms if necessary
         AdjustTrackQuantities(aSimTime);
      }

      if (mDebugEnabled)
      {
         auto out = ut::log::debug() << "Fuser State Report: ";
         PrintFuserState(out, aSimTime);
      }
   }
}

void WsfFusionCenter::UpdateState(double aSimTime)
{
   mStateChange        = false;
   mConsistencyChanged = false;

   mScreenersFT_Tracks.clear();
   mScreenersRT_Tracks.clear();
   mScreenersFT_IdList.clear();
   mScreenersRT_IdList.clear();
   mLastRealTargetConsistency = mRealTargetConsistency;
   mRealTargetConsistency.clear();
   mLastFalseTargetConsistency = mFalseTargetConsistency;
   mFalseTargetConsistency.clear();

   // Iterate through the list of screeners (radars) associated with this Fusion Center
   //  and aggregate screened tracks (real and false-target).
   for (auto& screenerIdPtr : mScreeners)
   {
      WsfFalseTargetScreener* screenerPtr = screenerIdPtr.second;
      assert(screenerPtr != nullptr);

      if (screenerPtr->GetScreenerValid())
      {
         // Accumulate false target tracks
         screenerPtr->TracksTransfered();
         WsfFalseTargetScreener::TrackIdMap screenedFalseTargets = screenerPtr->GetFalseTargetTracks();
         for (auto& screenedFalseTarget : screenedFalseTargets)
         {
            size_t objectId = screenedFalseTarget.first;
            // WsfTrackId trackId = (*ftIter).second;
            if (find(mScreenersFT_IdList.begin(), mScreenersFT_IdList.end(), objectId) == mScreenersFT_IdList.end())
            {
               mScreenersFT_IdList.push_back(objectId);
            }
            mScreenersFT_Tracks.insert(screenedFalseTarget);

            if (screenerPtr->IsRadarModeTurnedOn())
            {
               auto ftConsistencyIter = mFalseTargetConsistency.find(objectId);
               if (ftConsistencyIter != mFalseTargetConsistency.end())
               {
                  ++(*ftConsistencyIter).second;
               }
               else
               {
                  mFalseTargetConsistency.insert(std::make_pair(objectId, 1));
               }
            } // if (screenerPtr
         }    // for (ftIter = screenedFalseTargets.begin()

         // Accumulate  real target tracks.
         WsfFalseTargetScreener::TrackIdMap screenedRealTargets = screenerPtr->GetActiveTargetTracks();
         for (auto& screenedRealTarget : screenedRealTargets)
         {
            size_t objectId = screenedRealTarget.first;
            // WsfTrackId trackId = (*rtIter).second;
            if (find(mScreenersRT_IdList.begin(), mScreenersRT_IdList.end(), objectId) == mScreenersRT_IdList.end())
            {
               mScreenersRT_IdList.push_back(objectId);
            }
            mScreenersRT_Tracks.insert(screenedRealTarget);

            if (screenerPtr->IsRadarModeTurnedOn())
            {
               auto rtConsistencyIter = mRealTargetConsistency.find(objectId);
               if (rtConsistencyIter != mRealTargetConsistency.end())
               {
                  ++(*rtConsistencyIter).second;
               }
               else
               {
                  mRealTargetConsistency.insert(std::make_pair(objectId, 1));
               }
            } // if (screenerPtr
         }    // for (rtIter
      }       // if (screenerPtr->GetScreenerValid()
   }          // for (scrIter = mScreeners.begin()

   size_t aggScreenedFT_number = 0;
   size_t aggScreenedRT_number = 0;
   if (mConsistencyConstrained)
   {
      size_t tempConsistency;
      for (auto& rtConsistencyIter : mRealTargetConsistency)
      {
         tempConsistency = rtConsistencyIter.second;
         if (tempConsistency == mRegisteredSites.size())
         {
            ++aggScreenedRT_number;
         }
      }

      for (auto& ftConsistencyIter : mFalseTargetConsistency)
      {
         tempConsistency = ftConsistencyIter.second;
         if (tempConsistency == mRegisteredSites.size())
         {
            ++aggScreenedFT_number;
         }
      }

      if ((mFalseTargetConsistency != mLastFalseTargetConsistency) ||
          (mRealTargetConsistency != mLastRealTargetConsistency))
      {
         mConsistencyChanged = true;
      }
   }
   else
   {
      aggScreenedRT_number = mScreenersRT_IdList.size();
      aggScreenedFT_number = mScreenersFT_IdList.size();
   }

   // check to see if fusion center state has changed due to the appearance/disappearance
   // of false targets and reset state
   if ((mLastAggFT_Number != aggScreenedFT_number) || (mLastAggRT_Number != aggScreenedRT_number) || mConsistencyChanged)
   {
      mLastAggFT_Number = aggScreenedFT_number; // save current state
      mLastAggRT_Number = aggScreenedRT_number;
      mStateChange      = true;

      double scrRejectionRatio = mRejectionRatios[WsfFalseTarget::cCONSISTENT_TO_MULTIPLE_RADARS];
      size_t remainingFTs      = static_cast<size_t>(aggScreenedFT_number * (1.0 - scrRejectionRatio));
      size_t totalTargets      = remainingFTs + aggScreenedRT_number;

      if (totalTargets <= mTrackCapacity)
      {
         mFusedRT_Number = aggScreenedRT_number;
         mFusedFT_Number = remainingFTs;
      }
      else
      {
         mFusedRT_Number = static_cast<size_t>((mTrackCapacity * ((float)aggScreenedRT_number / (float)totalTargets)));
         mFusedFT_Number = mTrackCapacity - mFusedRT_Number;
      }
   }
}

void WsfFusionCenter::AdjustTrackQuantities(double aSimTime)
{
   // False target track adjustment
   if (mActiveFT_Tracks.size() < mFusedFT_Number) // then add false-target tracks
   {
      auto idIter = mScreenersFT_IdList.begin();
      while ((idIter != mScreenersFT_IdList.end()) && (mActiveFT_Tracks.size() < mFusedFT_Number))
      {
         size_t tempId = *idIter;
         if (FT_Fusible(tempId)) // check to see if a track can be added
         {
            size_t trackCount = mScreenersFT_Tracks.count(tempId);
            if (trackCount == 0)
            {
               ut::log::error() << "Unknown false-target track in WsfFusionCenter::AdjustTrackQuantities.";
               assert(false);
            }

            auto trackIdIter = mScreenersFT_Tracks.equal_range(tempId).first;
            auto out         = ut::log::info() << "Adjusting track quantities.";
            out.AddNote() << "T = " << aSimTime;
            for (; trackIdIter != mScreenersFT_Tracks.equal_range(tempId).second; ++trackIdIter)
            {
               auto         note             = out.AddNote() << "Track Id: " << (*trackIdIter).second;
               WsfTrackId   tempTrackId      = (*trackIdIter).second;
               WsfPlatform* trackingPlatform = GetSimulation()->GetPlatformByName(tempTrackId.GetOwningPlatformId());
               WsfTrack*    tempTrackPtr     = trackingPlatform->GetMasterRawTrackList().FindTrack(tempTrackId);

               // correlatedTrackPtr =
               //    GetPlatform()->GetTrackManager().FindCorrelatedTrack(tempTrackId,
               //    GetPlatform()->GetTrackManager().GetTrackList());
               // if (correlatedTrackPtr == nullptr)
               //{
               WsfLocalTrack* localTrackPtr = GetPlatform()->GetTrackManager().AddTrackReport(aSimTime, *tempTrackPtr);
               if (localTrackPtr != nullptr) // track has already been correlated
               {
                  IdTrackIdPair idTrackIdPair = std::make_pair(tempId, localTrackPtr->GetTrackId());
                  if ((find(mActiveFT_Tracks.begin(), mActiveFT_Tracks.end(), idTrackIdPair) == mActiveFT_Tracks.end()) &&
                      (mActiveFT_Tracks.size() < mFusedFT_Number)) // see if false-target track is already active and we
                                                                   // can add another one.
                  {
                     // false-target is not active
                     mActiveFT_Tracks.push_back(idTrackIdPair);
                  }
                  else if ((mActiveFT_Tracks.size() > mFusedFT_Number)) // check to see if we went over and delete the
                                                                        // last one added since it didn't correlate.
                  {
                     note.AddNote() << "WsfFusionCenter::AdjustTrackQuantities - Too man false-target tracks due to "
                                       "un-correlated blips in correlation strategy.";
                     GetPlatform()->GetTrackManager().DropTrack(aSimTime, localTrackPtr->GetTrackId());
                  }
               }
               //}
            }
         }
         ++idIter;
      }
   }
   else if (mActiveFT_Tracks.size() > mFusedFT_Number) // purge existing tracks
   {
      size_t numberToPrune = mActiveFT_Tracks.size() - mFusedFT_Number;
      shuffle(mActiveFT_Tracks.begin(),
              mActiveFT_Tracks.end(),
              std::default_random_engine(GetSimulation()->GetRandom().GetSeed()));
      size_t loopIndex = 0;
      auto   out       = ut::log::debug() << "WsfFusionCenter Dropped Track List:";
      while (loopIndex < numberToPrune)
      {
         auto tracksIter = mActiveFT_Tracks.end();
         --tracksIter;
         // id = (*tracksIter).first;
         WsfTrackId trackId = (*tracksIter).second;
         mActiveFT_Tracks.erase(tracksIter);
         WsfLocalTrack* localTrackPtr = GetPlatform()->GetTrackManager().FindTrack(trackId);
         if (localTrackPtr != nullptr)
         {
            if (mDebugEnabled)
            {
               out.AddNote() << "Dropped Track: " << localTrackPtr->GetTargetName();
            }
            WsfObserver::LocalTrackDropped(GetSimulation())(aSimTime, GetPlatform(), localTrackPtr);
            GetPlatform()->GetTrackManager().DropTrack(aSimTime, trackId);
         }
         ++loopIndex;
      }
   }

   // Real target track adjustments
   if (mActiveRT_Tracks.size() < mFusedRT_Number) // then add false-target tracks
   {
      // IdTrackIdList::iterator activeIdIter;
      IdTrackIdList::iterator blockedIdIter;

      auto idIter = mScreenersRT_IdList.begin();
      while ((idIter != mScreenersRT_IdList.end()) && (mActiveRT_Tracks.size() < mFusedRT_Number))
      {
         size_t tempId = *idIter;
         if (RT_Fusible(tempId)) // check to see if a track can be added
         {
            size_t trackCount = mScreenersRT_Tracks.count(tempId);
            if (trackCount == 0)
            {
               ut::log::info() << "Unknown real target track in WsfFusionCenter::AdjustTrackQuantities.";
               assert(false);
            }

            auto trackIdIter = mScreenersRT_Tracks.equal_range(tempId).first;
            auto out         = ut::log::info() << "Adjusting track quantities.";
            out.AddNote() << "T = " << aSimTime;
            for (; trackIdIter != mScreenersRT_Tracks.equal_range(tempId).second; ++trackIdIter)
            {
               auto         note             = out.AddNote() << "Track ID: " << (*trackIdIter).second;
               WsfTrackId   tempTrackId      = (*trackIdIter).second;
               WsfPlatform* trackingPlatform = GetSimulation()->GetPlatformByName(tempTrackId.GetOwningPlatformId());
               WsfTrack*    tempTrackPtr     = trackingPlatform->GetMasterRawTrackList().FindTrack(tempTrackId);

               // correlatedTrackPtr =
               //    GetPlatform()->GetTrackManager().FindCorrelatedTrack(tempTrackId,
               //    GetPlatform()->GetTrackManager().GetTrackList());
               // if (correlatedTrackPtr == 0)
               //{
               WsfLocalTrack* localTrackPtr = GetPlatform()->GetTrackManager().AddTrackReport(aSimTime, *tempTrackPtr);
               if (localTrackPtr != nullptr) // track has already been correlated
               {
                  IdTrackIdPair idTrackIdPair = std::make_pair(tempId, localTrackPtr->GetTrackId());
                  if ((find(mActiveRT_Tracks.begin(), mActiveRT_Tracks.end(), idTrackIdPair) == mActiveRT_Tracks.end()) &&
                      (mActiveRT_Tracks.size() < mFusedRT_Number)) // see if false-target track is already active and we
                                                                   // can add another one.
                  {
                     // false-target is not active
                     mActiveRT_Tracks.push_back(idTrackIdPair);
                     blockedIdIter = find(mBlockedRT_Tracks.begin(), mBlockedRT_Tracks.end(), idTrackIdPair);
                     if (blockedIdIter != mBlockedRT_Tracks.end())
                     {
                        mBlockedRT_Tracks.erase(blockedIdIter);
                     }
                  }
                  else if ((mActiveRT_Tracks.size() > mFusedRT_Number)) // check to see if we went over and delete the
                                                                        // last one added since it didn't correlate.
                  {
                     note.AddNote() << "WsfFusionCenter::AdjustTrackQuantities - Too man false-target tracks due to "
                                       "un-correlated blips in correlation strategy.";
                     GetPlatform()->GetTrackManager().DropTrack(aSimTime, localTrackPtr->GetTrackId());
                  }
               }
               //}
            }
         }
         ++idIter;
      }
   }
   else if (mActiveRT_Tracks.size() > mFusedRT_Number) // purge existing tracks
   {
      size_t numberToPrune = mActiveRT_Tracks.size() - mFusedRT_Number;
      shuffle(mActiveRT_Tracks.begin(),
              mActiveRT_Tracks.end(),
              std::default_random_engine(GetSimulation()->GetRandom().GetSeed()));
      size_t loopIndex = 0;
      while (loopIndex < numberToPrune)
      {
         auto tracksIter = mActiveRT_Tracks.end();
         --tracksIter;
         // id = (*tracksIter).first;
         WsfTrackId trackId = (*tracksIter).second;
         mActiveRT_Tracks.erase(tracksIter);
         WsfLocalTrack* localTrackPtr = GetPlatform()->GetTrackManager().FindTrack(trackId);
         if (localTrackPtr != nullptr)
         {
            if (mDebugEnabled)
            {
               auto out = ut::log::debug() << "WsfFusionCenter: Dropping local track.";
               out.AddNote() << "Track Id: " << localTrackPtr->GetTrackId();
               out.AddNote() << "Target: " << localTrackPtr->GetTargetName();
            }
            WsfObserver::LocalTrackDropped(GetSimulation())(aSimTime, GetPlatform(), localTrackPtr);
            GetPlatform()->GetTrackManager().DropTrack(aSimTime, trackId);
         }
         ++loopIndex;
      }
   }
}

void WsfFusionCenter::DeleteAllTracks(double aSimTime)
{
   if (mDebugEnabled)
   {
      ut::log::debug() << "WsfFusionCenter::DeleteAllTracks: Deleting all tracks.";
   }

   IdTrackIdList::iterator idTrackIdIter;
   for (idTrackIdIter = mActiveRT_Tracks.begin(); idTrackIdIter != mActiveRT_Tracks.end(); ++idTrackIdIter)
   {
      GetPlatform()->GetTrackManager().DropTrack(aSimTime, (*idTrackIdIter).second);
   }
   for (idTrackIdIter = mActiveFT_Tracks.begin(); idTrackIdIter != mActiveFT_Tracks.end(); ++idTrackIdIter)
   {
      GetPlatform()->GetTrackManager().DropTrack(aSimTime, (*idTrackIdIter).second);
   }
   mActiveRT_Tracks.clear();
   mScreenersRT_Tracks.clear();
   mBlockedRT_Tracks.clear();
   mActiveFT_Tracks.clear();
   mRealTargetConsistency.clear();
   mLastAggRT_Number = 0;
   mLastAggFT_Number = 0;
   mStateChange      = true;
}

// virtual
void WsfFusionCenter::PlatformDeleted(WsfPlatform* aPlatformPtr)
{
   DeleteAllScreenersOn(aPlatformPtr->GetNameId());
}

// protected
void WsfFusionCenter::CheckForScreenerAdd(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr)
{
   // A sensor has just been turned on, and we are receiving a callback...
   // see if this is a sensor containing a screener that we might need access to.
   WsfFalseTargetScreener* screenerPtr(WsfFalseTargetScreener::Find(*aModePtr));
   if (screenerPtr != nullptr)
   {
      WsfStringId platformId = aSensorPtr->GetPlatform()->GetNameId();
      AddScreener(platformId, screenerPtr);
      mStateChange = true;
      mRealTargetConsistency.clear();
      mFalseTargetConsistency.clear();
   }
}

// protected
void WsfFusionCenter::CheckForScreenerReset(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr)
{
   // A sensor has just been turned off, and we are receiving a callback...
   // see if this is a sensor containing a screener that we might need to reset.

   WsfFalseTargetScreener* screenerPtr(WsfFalseTargetScreener::Find(*aModePtr));
   if (screenerPtr != nullptr)
   {
      screenerPtr->Reset();
      mStateChange = true;
      if (mConsistencyConstrained)
      {
         DeleteAllTracks(aSimTime);
      }
   }
}

void WsfFusionCenter::DetermineOutputType()
{
   int maxScreenedFTs   = 0;
   int totalScreenedFTs = 0;

   bool screenerPlotFlag  = false;
   bool screenerTrackFlag = false;

   int plotBoundary;
   int overloadBoundary;

   mOutput = cPLOT;

   overloadBoundary = 10 * mPlotCapacity;
   plotBoundary     = 10 * mTrackCapacity;

   for (const auto& idScreener : mScreeners)
   {
      WsfFalseTargetScreener*            screenerPtr = idScreener.second;
      WsfFalseTargetScreener::OutputType outputType  = screenerPtr->GetScreenerOutputType();
      if (outputType == WsfFalseTargetScreener::cPLOT)
      {
         screenerPlotFlag = true;
      }
      else if (outputType == WsfFalseTargetScreener::cTRACKS)
      {
         screenerTrackFlag = true;
      }
      else if (outputType == WsfFalseTargetScreener::cOVERLOAD)
      {
         // screenerOverloadFlag = true;
      }

      if (screenerPtr->GetFalseTargetNumber() > maxScreenedFTs)
      {
         int numScreenedFTs = screenerPtr->GetFalseTargetNumber();
         totalScreenedFTs += numScreenedFTs;
         maxScreenedFTs = (numScreenedFTs > maxScreenedFTs) ? numScreenedFTs : maxScreenedFTs;
      }
   }

   // Don't know the details here, but here's some placeholder code

   if (totalScreenedFTs > overloadBoundary) // too many false targets to process
   {
      mOutput = cOVERLOAD;
   }
   else if (!screenerTrackFlag) // no screeners are producing tracks
   {
      if (totalScreenedFTs > plotBoundary && totalScreenedFTs < overloadBoundary)
      {
         mOutput = cPLOT;
      }
   }
   else if (screenerTrackFlag && screenerPlotFlag) // some of each
   {
   }
}

// virtual
// bool WsfFusionCenter::ProcessMessage(double aSimTime, const WsfMessage& aMessage)
//{
//    bool processed = false;
//    return processed;
//    WsfScriptProcessor::ProcessMessage(aSimTime, aMessage);
// }

// virtual
bool WsfFusionCenter::ReceiveMessage(double aSimTime, const WsfMessage& aMessage)
{
   // If the message is of a type that is of interest to this processor, then attempt
   // to process and react to it.  If we do actually react, respond,  or even value
   // the information in this message, then set the return value to true.
   bool isMeaningful = false;

   WsfStringId messageType = aMessage.GetType();
   if (messageType == WsfTrackMessage::GetTypeId())
   {
      // Incoming track message.
      isMeaningful = true;
   }

   return isMeaningful;
}

void WsfFusionCenter::UpdateFalseTargetTracks(double aSimTime)
{
   if ((mScreenersFT_Tracks.empty()) || (mActiveFT_Tracks.empty()))
   {
      return;
   }
   auto out = ut::log::debug();
   if (mDebugEnabled)
   {
      out << "Updating false target tracks.";
   }
   IdTrackIdList::iterator trackIdIter;
   for (trackIdIter = mActiveFT_Tracks.begin(); trackIdIter != mActiveFT_Tracks.end();)
   {
      size_t         targetId      = (*trackIdIter).first;
      WsfTrackId     localTrackId  = (*trackIdIter).second;
      WsfLocalTrack* localTrackPtr = GetPlatform()->GetTrackManager().FindTrack(localTrackId);
      if (localTrackPtr != nullptr)
      {
         if (FT_Fusible((*trackIdIter).first))
         {
            auto trackNote = out.AddNote();
            if (mDebugEnabled)
            {
               trackNote << "Updating track.";
               trackNote.AddNote() << "Track Id: " << localTrackId;
            }
            auto aggIter = mScreenersFT_Tracks.equal_range(targetId).first;
            for (; aggIter != mScreenersFT_Tracks.equal_range(targetId).second; ++aggIter)
            {
               WsfTrackId   trackId          = (*aggIter).second;
               WsfPlatform* trackingPlatform = GetSimulation()->GetPlatformByName(trackId.GetOwningPlatformId());
               WsfTrack*    trackPtr         = trackingPlatform->GetMasterRawTrackList().FindTrack(trackId);

               WsfLocalTrack* correlatedTrackPtr =
                  GetPlatform()->GetTrackManager().FindCorrelatedTrack(trackId,
                                                                       GetPlatform()->GetTrackManager().GetTrackList());
               if (correlatedTrackPtr != nullptr)
               {
                  if (correlatedTrackPtr->GetTrackId() == localTrackId)
                  {
                     GetPlatform()->GetTrackManager().AddTrackReport(aSimTime, *trackPtr);
                  }
                  else if (mDebugEnabled)
                  {
                     trackNote.AddNote() << "Correlation tracks are different.";
                     trackNote.AddNote() << "Correlation Track Id: " << correlatedTrackPtr->GetTrackId();
                  }
               }
               else if (mDebugEnabled)
               {
                  trackNote.AddNote() << "Correlation track not found.";
               }
            }
         }
         else
         {
            // do not update the track to prevent updates. The track will be dropped by the track processor after the
            //  purge interval time has been met.
            if (mDebugEnabled)
            {
               auto note = out.AddNote() << "Not updating track.";
               note.AddNote() << "Track Id: " << localTrackId;
            }
         }
         ++trackIdIter;
      }
      else
      {
         // has been purged by the track processor due to purge interval or other reasons
         if (mDebugEnabled)
         {
            auto note = out.AddNote() << "Removing track.";
            note.AddNote() << "Track Id: " << localTrackId;
         }
         trackIdIter = mActiveFT_Tracks.erase(trackIdIter);
      }
   }
}

void WsfFusionCenter::UpdateRealTargetTracks(double aSimTime)
{
   if ((mScreenersRT_Tracks.empty()) || (mActiveRT_Tracks.empty()))
   {
      return;
   }

   IdTrackIdList::iterator trackIdIter;
   auto                    out = ut::log::debug();
   if (mDebugEnabled)
   {
      out << "Updating real target tracks.";
   }
   for (trackIdIter = mActiveRT_Tracks.begin(); trackIdIter != mActiveRT_Tracks.end();)
   {
      size_t         targetId      = (*trackIdIter).first;
      WsfTrackId     localTrackId  = (*trackIdIter).second;
      WsfLocalTrack* localTrackPtr = GetPlatform()->GetTrackManager().FindTrack(localTrackId);
      if (localTrackPtr != nullptr)
      {
         if (RT_Fusible((*trackIdIter).first))
         {
            auto trackNote = out.AddNote();
            if (mDebugEnabled)
            {
               trackNote << "Updating track.";
               trackNote.AddNote() << "Track Id: " << localTrackId;
            }
            auto aggIter = mScreenersRT_Tracks.equal_range(targetId).first;
            for (; aggIter != mScreenersRT_Tracks.equal_range(targetId).second; ++aggIter)
            {
               WsfTrackId   trackId          = (*aggIter).second;
               WsfPlatform* trackingPlatform = GetSimulation()->GetPlatformByName(trackId.GetOwningPlatformId());
               WsfTrack*    trackPtr         = trackingPlatform->GetMasterRawTrackList().FindTrack(trackId);

               WsfLocalTrack* correlatedTrackPtr =
                  GetPlatform()->GetTrackManager().FindCorrelatedTrack(trackId,
                                                                       GetPlatform()->GetTrackManager().GetTrackList());
               if (correlatedTrackPtr != nullptr)
               {
                  if (correlatedTrackPtr->GetTrackId() == localTrackId)
                  {
                     GetPlatform()->GetTrackManager().AddTrackReport(aSimTime, *trackPtr);
                  }
                  else if (mDebugEnabled)
                  {
                     auto note = ut::log::debug() << "Correlation tracks are different.";
                     note.AddNote() << "Target: " << targetId;
                     note.AddNote() << "Correlation Track Id: " << correlatedTrackPtr->GetTrackId();
                  }
               }
               else if (mDebugEnabled)
               {
                  auto note = out.AddNote() << "Correlation track not found.";
                  note.AddNote() << "Target.TrackId: " << targetId << "." << localTrackId;
               }
            }
         }
         else
         {
            // do not update the track to prevent updates. The track will be dropped by the track processor after the
            //  purge interval time has been met.
            if (mDebugEnabled)
            {
               auto note = out.AddNote() << "Not updating track.";
               note.AddNote() << "Track Id: " << localTrackId;
            }
         }
         ++trackIdIter;
      }
      else
      {
         // has been purged by the track processor due to purge interval or other reasons
         if (mDebugEnabled)
         {
            auto out = ut::log::debug() << "Removing Target.";
            out.AddNote() << "Target: " << targetId;
            out.AddNote() << "Name Id: " << GetNameId();
            out.AddNote() << "Type Id: " << GetTypeId();
         }
         trackIdIter = mActiveRT_Tracks.erase(trackIdIter);
      }
   }
}
/*
bool WsfFusionCenter::UpdateNonKinematicData(double           aSimTime,
                                             WsfTrackManager& aTrackManager,
                                             WsfLocalTrack&   aLocalTrack,
                                             const WsfTrack&  aNonLocalTrack)
{
   // Now merge 'scalar' data.
   bool localTrackUpdated = false;

   if (aNonLocalTrack.FrequencyValid())
   {
      aLocalTrack.SetFrequency(aNonLocalTrack.GetFrequency());
      localTrackUpdated = true;
   }

   if (aNonLocalTrack.SideIdValid())
   {
      aLocalTrack.SetSideId(aNonLocalTrack.GetSideId());
      aLocalTrack.SetSideIdValid(true);
      localTrackUpdated = true;
   }

   if (aNonLocalTrack.TypeIdValid())
   {
      aLocalTrack.SetTypeId(aNonLocalTrack.GetTypeId());
      aLocalTrack.SetTypeIdValid(true);
      localTrackUpdated = true;
   }

   if (aNonLocalTrack.GetIFF_Status() != WsfTrack::cIFF_UNKNOWN)
   {
      // TODO this should be scriptable based on Rules of Engagement (ROE).
      // Current implementation is to report "ambiguous" if all reports do not agree.
      WsfTrack::IFF_Status nonLocalIFF = aNonLocalTrack.GetIFF_Status();
      WsfTrack::IFF_Status localIFF    = aLocalTrack.GetIFF_Status();

      bool nonLocalFOF = ((nonLocalIFF == WsfTrack::cIFF_FRIEND) || (nonLocalIFF == WsfTrack::cIFF_FOE));
      bool localFOF    = ((localIFF == WsfTrack::cIFF_FRIEND) || (localIFF == WsfTrack::cIFF_FOE));

      if (localIFF == WsfTrack::cIFF_UNKNOWN)
      {
         aLocalTrack.SetIFF_Status(nonLocalIFF);
      }
      else if (nonLocalFOF)
      {
         if ((localFOF) &&
            (nonLocalIFF != localIFF))
         {
            aLocalTrack.SetIFF_Status(WsfTrack::cIFF_AMBIGUOUS);
         }
         // Local track iff should be "ambiguous" in order to be here in the logic.
         else if ((localIFF    == WsfTrack::cIFF_AMBIGUOUS) &&
            (nonLocalIFF != WsfTrack::cIFF_AMBIGUOUS))
         {
            // Does this update resolve the ambiguity?
            // Must examine our raw track inputs to find out.
//            ReviewIFF(aTrackManager, aLocalTrack);                      later djs
         }
      }
      else if (nonLocalIFF == WsfTrack::cIFF_AMBIGUOUS)
      {
         aLocalTrack.SetIFF_Status(WsfTrack::cIFF_AMBIGUOUS);
      }
   }

   // Take the highest track quality as the local track's track quality.
   if (aNonLocalTrack.GetTrackQuality() > aLocalTrack.GetTrackQuality())
   {
      aLocalTrack.SetTrackQuality(aNonLocalTrack.GetTrackQuality());
   }

   if (aNonLocalTrack.HasAuxData())
   {
      aLocalTrack.UpdateAuxData(aNonLocalTrack);
   }

   return localTrackUpdated;
}
*/
size_t WsfFusionCenter::GetValidBlipCount()
{
   size_t validBlipCount = 0;

   IdToTrackIdMultiMap::iterator blipIter;
   for (blipIter = mScreenersFT_Tracks.begin(); blipIter != mScreenersFT_Tracks.end(); ++blipIter)
   {
      if (FT_Fusible((*blipIter).first))
      {
         ++validBlipCount;
      }
   }
   return validBlipCount;
}

bool WsfFusionCenter::RT_Fusible(size_t aTargetId)
{
   // if consistency constrained, true only if all the screeners are tracking the same target
   if (!mConsistencyConstrained)
   {
      return true;
   }

   auto screenerIter = mRealTargetConsistency.find(aTargetId);
   if (screenerIter == mRealTargetConsistency.end())
   {
      return false;
   }

   return ((*screenerIter).second == mRegisteredSites.size());
}

bool WsfFusionCenter::FT_Fusible(size_t aBlipId)
{
   // if consistency constrained, true only if all the screeners are tracking the same blip
   if (!mConsistencyConstrained)
   {
      return true;
   }

   auto screenerIter = mFalseTargetConsistency.find(aBlipId);
   if (screenerIter == mFalseTargetConsistency.end())
   {
      return false;
   }

   return ((*screenerIter).second == mRegisteredSites.size());
}

void WsfFusionCenter::PrintFuserState(ut::log::MessageStream& aMsgStream, double aSimTime)
{
   if (mDebugEnabled) // && mStateChange)
   {
      Screeners::iterator scrIter;
      for (scrIter = mScreeners.begin(); scrIter != mScreeners.end(); ++scrIter)
      {
         WsfFalseTargetScreener* screenerPtr = (*scrIter).second;
         aMsgStream.AddNote() << "Screener: " << screenerPtr->GetSensorModePtr()->GetPlatform()->GetName() << ":"
                              << screenerPtr->GetSensorModePtr()->GetSensor()->GetName();
         aMsgStream.AddNote() << "Screener Sensor: " << screenerPtr->GetSensorModePtr()->GetName();
         aMsgStream.AddNote() << "Screener: " << screenerPtr->GetName();
         aMsgStream.AddNote() << "False Target Tracks: " << screenerPtr->GetPassedFalseTargetNumber();
         aMsgStream.AddNote() << "Real Target Tracks: " << screenerPtr->GetPassedRealTargetNumber();
      }
   }

   auto logNote = aMsgStream.AddNote() << "Fusion Center Processor State for: " << GetPlatform()->GetName() << ":"
                                       << GetNameId();
   logNote.AddNote() << "T = " << aSimTime;
   logNote.AddNote() << "Tracking: " << GetPlatform()->GetTrackManager().GetTrackCount() << " total targets";
   logNote.AddNote() << "Track Capacity: " << mTrackCapacity;
   logNote.AddNote() << "Active Target Tracks: " << mActiveRT_Tracks.size();
   logNote.AddNote() << "Blocked Target Tracks: " << mBlockedRT_Tracks.size();
   logNote.AddNote() << "False Target Tracks: " << mActiveFT_Tracks.size();
}
