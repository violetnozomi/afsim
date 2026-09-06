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

#include "WsfFalseTargetScreener.hpp"

#include <algorithm>
#include <iostream>
#include <limits>
#include <memory>

#include "UtCast.hpp"
#include "UtEntity.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "WsfAntennaPattern.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Interaction.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfEW_EffectManager.hpp"
#include "WsfEW_FalseTargetEffect.hpp"
#include "WsfEW_Result.hpp"
#include "WsfEW_SensorComponent.hpp"
#include "WsfFalseTarget.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfPlatform.hpp"
#include "WsfRF_Jammer.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackManager.hpp"

namespace
{
template<class RandomIt>
void TrackShuffle(RandomIt first, RandomIt last, ut::Random& aRandom)
{
   for (auto i = (last - first) - 1; i > 0; --i)
   {
      std::swap(first[i], first[aRandom.Uniform<ptrdiff_t>(0, i)]);
   }
}
} // namespace

// =================================================================================================
//! Return the pointer to the false target screener associated with the specified mode.
//! @returns Pointer to the false target screener. This may be null.
// static
WsfFalseTargetScreener* WsfFalseTargetScreener::Find(const WsfSensorMode& aSensorMode)
{
   WsfFalseTargetScreener* screenerPtr(nullptr);
   WsfSensor*              sensorPtr(aSensorMode.GetSensor());
   if (sensorPtr != nullptr)
   {
      WsfEW_SensorComponent* ewComponentPtr(WsfEW_SensorComponent::Find(*sensorPtr));
      if (ewComponentPtr != nullptr)
      {
         screenerPtr = ewComponentPtr->GetScreener(aSensorMode);
      }
   }
   return screenerPtr;
}

// =================================================================================================
WsfFalseTargetScreener::WsfFalseTargetScreener(const WsfScenario& aScenario)
   : WsfObject()
   , mReportingTrackIds()
   , mFalseTargetsJammers()
   , mActiveTargetTrackIds()
   , mBlockedTargetTrackIds()
   , mForcedTargetTrackIds()
   , mNullTargetTrackIds()
   , mTargetsAdjusted(false)
   , mTargetsInvalid(true)
   , mLastUpdate(-std::numeric_limits<double>::max())
   , mBlockedBlipIds()
   , mUndetectedBlipIds()
   , mTrackableBlipIds()
   , mBlipsAdjusted(false)
   , mBlipsInvalid(true)
   , mSensorModePtr(nullptr)
   , mReportsSignalToNoise(false)
   , mDebugEnabled(false)
   , mScreenerValid(false)
   , mRandom()
   , mBlipsAllocation()
   , mInitialRejectionRatios()
   , mRejectionRatios()
   , mOutputType(cPLOT)
   , mTrackReportingWhenFlooded(true)
   , mProduceOutputPlot(false)
   , mRequestBasedTracking(false)
   , mOnlyScreenFT_Requests(false)
   , mPicture(cPERFECT)
   , mPlotCapacity(1000)
   , mTrackCapacity(1000)
   , mTransferCapacity(250)
   , mFT_JammerStatusChange(false)
   , mLastBlipUpdate(-std::numeric_limits<double>::max())
   , mForceCountPerRangeSector(0)
   , mForceJtoSDeltaThreshold(std::numeric_limits<double>::max())
   , mForceSectorArc(UtMath::cTWO_PI)
   , mForceRange(std::numeric_limits<double>::max())
   , mRangeForceType(cFORCE_NEITHER)
   , mMinimumBlipRange(0.0)
   , mMaximumBlipRange(std::numeric_limits<double>::max())
   , mBlipIdToForceDataMap()
   , mPassedFalseTargetNumber(0)
   , mPassedRealTargetNumber(0)
   , mAllowedFalseTargetNumber(0)
   , mAllowedRealTargetNumber(0)
   , mFalseTargetTrackIds()
   , mTempPlatformPtr(ut::make_unique<WsfPlatform>(aScenario))
{
   mTempPlatformPtr->SetIsFalseTarget(true);
}

WsfFalseTargetScreener::WsfFalseTargetScreener(const WsfFalseTargetScreener& aSrc)
   : WsfObject(aSrc)
   , mReportingTrackIds(aSrc.mReportingTrackIds)
   , mFalseTargetsJammers(aSrc.mFalseTargetsJammers)
   , mActiveTargetTrackIds(aSrc.mActiveTargetTrackIds)
   , mBlockedTargetTrackIds(aSrc.mBlockedTargetTrackIds)
   , mForcedTargetTrackIds(aSrc.mForcedTargetTrackIds)
   , mNullTargetTrackIds(aSrc.mNullTargetTrackIds)
   , mTargetsAdjusted(aSrc.mTargetsAdjusted)
   , mTargetsInvalid(aSrc.mTargetsInvalid)
   , mLastUpdate(aSrc.mLastUpdate)
   , mBlockedBlipIds(aSrc.mBlockedBlipIds)
   , mUndetectedBlipIds(aSrc.mUndetectedBlipIds)
   , mTrackableBlipIds(aSrc.mTrackableBlipIds)
   , mBlipsAdjusted(aSrc.mBlipsAdjusted)
   , mBlipsInvalid(aSrc.mBlipsInvalid)
   , mSensorModePtr(aSrc.mSensorModePtr)
   , mReportsSignalToNoise(aSrc.mReportsSignalToNoise)
   , mDebugEnabled(aSrc.mDebugEnabled)
   , mScreenerValid(aSrc.mScreenerValid)
   , mRandom()
   , mBlipsAllocation(aSrc.mBlipsAllocation)
   , mInitialRejectionRatios(aSrc.mInitialRejectionRatios)
   , mRejectionRatios(aSrc.mRejectionRatios)
   , mOutputType(aSrc.mOutputType)
   , mTrackReportingWhenFlooded(aSrc.mTrackReportingWhenFlooded)
   , mProduceOutputPlot(aSrc.mProduceOutputPlot)
   , mRequestBasedTracking(aSrc.mRequestBasedTracking)
   , mOnlyScreenFT_Requests(aSrc.mOnlyScreenFT_Requests)
   , mPicture(aSrc.mPicture)
   , mPlotCapacity(aSrc.mPlotCapacity)
   , mTrackCapacity(aSrc.mTrackCapacity)
   , mTransferCapacity(aSrc.mTransferCapacity)
   , mFT_JammerStatusChange(aSrc.mFT_JammerStatusChange)
   , mLastBlipUpdate(aSrc.mLastBlipUpdate)
   , mForceCountPerRangeSector(aSrc.mForceCountPerRangeSector)
   , mForceJtoSDeltaThreshold(aSrc.mForceJtoSDeltaThreshold)
   , mForceSectorArc(aSrc.mForceSectorArc)
   , mForceRange(aSrc.mForceRange)
   , mRangeForceType(aSrc.mRangeForceType)
   , mMinimumBlipRange(aSrc.mMinimumBlipRange)
   , mMaximumBlipRange(aSrc.mMaximumBlipRange)
   , mBlipIdToForceDataMap(aSrc.mBlipIdToForceDataMap)
   , mPassedFalseTargetNumber(aSrc.mPassedFalseTargetNumber)
   , mPassedRealTargetNumber(aSrc.mPassedRealTargetNumber)
   , mAllowedFalseTargetNumber(aSrc.mAllowedFalseTargetNumber)
   , mAllowedRealTargetNumber(aSrc.mAllowedRealTargetNumber)
   , mFalseTargetTrackIds(aSrc.mFalseTargetTrackIds)
   , mTempPlatformPtr(aSrc.mTempPlatformPtr)
{
}

// virtual
WsfFalseTargetScreener::~WsfFalseTargetScreener()
{
   if (GetSimulation() != nullptr)
   {
      WsfEW_EffectManager* effectManager(WsfEW_EffectManager::Find(*GetSimulation()));
      if (effectManager != nullptr)
      {
         effectManager->RemoveFalseTargetScreener(this);
      }
   }
}

// virtual
WsfFalseTargetScreener* WsfFalseTargetScreener::Clone() const
{
   return new WsfFalseTargetScreener(*this);
}

// virtual
bool WsfFalseTargetScreener::Initialize(double aSimTime, WsfSensorMode* aSensorModePtr, bool aReportsSignalToNoise)
{
   bool success = true;

   mSensorModePtr = dynamic_cast<WsfSensorMode*>(aSensorModePtr);
   assert(mSensorModePtr != nullptr);

   mTempPlatformPtr->AssignToSimulation(GetSimulation());

   // Initialize the random number stream.  Each screener has a different seed.
   unsigned int seed = GetSimulation()->GetRandom().Uniform<unsigned>();
   mRandom.SetSeed(seed);

   mReportsSignalToNoise = aReportsSignalToNoise;

   for (auto& falseTargetsJammer : mFalseTargetsJammers)
   {
      WsfEM_Xmtr* jammerXmtrPtr = falseTargetsJammer.second;
      success &= falseTargetsJammer.first->Initialize(aSimTime, jammerXmtrPtr);
   }

   return success;
}

// virtual
bool WsfFalseTargetScreener::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "plot_capacity")
   {
      aInput.ReadValue(mPlotCapacity);
      aInput.ValueGreater(mPlotCapacity, 0);
   }
   else if (command == "track_capacity")
   {
      aInput.ReadValue(mTrackCapacity);
      aInput.ValueGreater(mTrackCapacity, 0);
   }
   else if (command == "transfer_capacity")
   {
      aInput.ReadValue(mTransferCapacity);
      aInput.ValueGreater(mTransferCapacity, 0);
   }
   else if (command == "random_scan_to_scan")
   {
      double FTRejectionRatio;
      aInput.ReadValue(FTRejectionRatio);
      mInitialRejectionRatios.insert(RR_MapPair(WsfFalseTarget::cRANDOM_SCAN_TO_SCAN, FTRejectionRatio));
      auto out = ut::log::warning() << "WSF_FALSE_TARGET_SCREENER: 'random_scan_to_scan' has been deprecated, use "
                                       "'initial_reject' or 'reject' depending on your needs.";
      out.AddNote() << "See the WSF documentation for more information.";
   }
   else if (command == "consistent_scan_to_scan")
   {
      double FTRejectionRatio;
      aInput.ReadValue(FTRejectionRatio);
      mInitialRejectionRatios.insert(RR_MapPair(WsfFalseTarget::cCONSISTENT_SCAN_TO_SCAN, FTRejectionRatio));
      auto out = ut::log::warning() << "WSF_FALSE_TARGET_SCREENER: 'consistent_scan_to_scan' has been deprecated, use "
                                       "'initial_reject' or 'reject' depending on your needs.";
      out.AddNote() << "See the WSF documentation for more information.";
   }
   else if (command == "initial_reject")
   {
      std::string behavior;
      aInput.ReadValue(behavior);
      double FTRejectionRatio;
      if (behavior == "random_scan_to_scan")
      {
         aInput.ReadValue(FTRejectionRatio);
         mInitialRejectionRatios.insert(RR_MapPair(WsfFalseTarget::cRANDOM_SCAN_TO_SCAN, FTRejectionRatio));
      }
      else if (behavior == "consistent_scan_to_scan")
      {
         aInput.ReadValue(FTRejectionRatio);
         mInitialRejectionRatios.insert(RR_MapPair(WsfFalseTarget::cCONSISTENT_SCAN_TO_SCAN, FTRejectionRatio));
      }
      else
      {
         throw UtInput::BadValue(aInput);
      }
   }
   else if (command == "reject")
   {
      std::string behavior;
      aInput.ReadValue(behavior);
      double FTRejectionRatio;
      if (behavior == "random_scan_to_scan")
      {
         aInput.ReadValue(FTRejectionRatio);
         mRejectionRatios.insert(RR_MapPair(WsfFalseTarget::cRANDOM_SCAN_TO_SCAN, FTRejectionRatio));
      }
      else if (behavior == "consistent_scan_to_scan")
      {
         aInput.ReadValue(FTRejectionRatio);
         mRejectionRatios.insert(RR_MapPair(WsfFalseTarget::cCONSISTENT_SCAN_TO_SCAN, FTRejectionRatio));
      }
      else
      {
         throw UtInput::BadValue(aInput);
      }
   }
   else if (command == "allow_track_reporting_when_flooded")
   {
      aInput.ReadValue(mTrackReportingWhenFlooded);
   }
   else if (command == "request_based_tracking")
   {
      aInput.ReadValue(mRequestBasedTracking);
   }
   else if (command == "only_screen_false_target_track_requests")
   {
      aInput.ReadValue(mOnlyScreenFT_Requests);
   }
   else if (command == "debug")
   {
      mDebugEnabled = true;
   }
   else if (command == "force_target_tracks")
   {
      UtInputBlock block(aInput, "end_force_target_tracks");
      while (block.ReadCommand())
      {
         command = block.GetCommand();
         if (command == "range_force")
         {
            std::string slewMode;
            aInput.ReadValue(slewMode);
            if (slewMode == "none")
            {
               mRangeForceType = cFORCE_NEITHER;
            }
            else if (slewMode == "inside_blip_range")
            {
               mRangeForceType = cFORCE_INSIDE;
            }
            else if (slewMode == "outside_blip_range")
            {
               mRangeForceType = cFORCE_OUTSIDE;
            }
            else if ((slewMode == "both") || (slewMode == "inside_outside_blip_range") ||
                     (slewMode == "outside_inside_blip_range"))
            {
               mRangeForceType = cFORCE_IN_OUT;
            }
            else
            {
               throw UtInput::BadValue(aInput);
            }
         }
         else if (command == "sector_force")
         {
            aInput.ReadValueOfType(mForceSectorArc, UtInput::cANGLE);
            aInput.ValueGreater(mForceSectorArc, 0.0);
            aInput.ValueLessOrEqual(mForceSectorArc, UtMath::cTWO_PI);

            aInput.ReadValue(mForceCountPerRangeSector);
            aInput.ValueGreater(mForceCountPerRangeSector, 0);
         }
         else if (command == "range_sector_force")
         {
            aInput.ReadValueOfType(mForceRange, UtInput::cLENGTH);
            aInput.ValueGreater(mForceRange, 0.0);

            aInput.ReadValueOfType(mForceSectorArc, UtInput::cANGLE);
            aInput.ValueGreater(mForceSectorArc, 0.0);
            aInput.ValueLessOrEqual(mForceSectorArc, UtMath::cTWO_PI);

            aInput.ReadValue(mForceCountPerRangeSector);
            aInput.ValueGreater(mForceCountPerRangeSector, 0);
         }
         else if (command == "j_to_s_delta_threshold")
         {
            aInput.ReadValueOfType(mForceJtoSDeltaThreshold, UtInput::cRATIO);
            aInput.ValueGreater(mForceJtoSDeltaThreshold, 0.0);
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

//! A convenience method to check for and process a 'false_target_screener/end_false_target_screener' block.
void WsfFalseTargetScreener::ProcessInputBlock(UtInputBlock& aInputBlock)
{
   while (aInputBlock.ReadCommand())
   {
      UtInput& input = aInputBlock.GetInput();
      if (!ProcessInput(input))
      {
         throw UtInput::UnknownCommand(input);
      }
   }
}

bool WsfFalseTargetScreener::AddFalseTarget(FT_JammerPair aFT_JammerPair)
{
   bool added = false;

   auto ftJammerVecIter = find(mFalseTargetsJammers.begin(), mFalseTargetsJammers.end(), aFT_JammerPair);
   if (ftJammerVecIter == mFalseTargetsJammers.end())
   {
      mFalseTargetsJammers.push_back(aFT_JammerPair);
      aFT_JammerPair.first->Initialize(GetSimulation()->GetSimTime(), aFT_JammerPair.second);
      SetScreenerValid(false);
      WsfEW_EffectManager* effectManager(WsfEW_EffectManager::Find(*GetSimulation()));
      if (effectManager != nullptr)
      {
         effectManager->AddFalseTargetScreener(aFT_JammerPair.first, this);
      }
      added = true;
   }
   return added;
}

bool WsfFalseTargetScreener::RemoveFalseTarget(FT_JammerPair aFT_JammerPair)
{
   bool removed = false;

   auto ftJammerVecIter = find(mFalseTargetsJammers.begin(), mFalseTargetsJammers.end(), aFT_JammerPair);
   if (ftJammerVecIter != mFalseTargetsJammers.end())
   {
      aFT_JammerPair.first->RemoveJammer(aFT_JammerPair.second);
      RemoveBlips(aFT_JammerPair.first, aFT_JammerPair.second);
      SetScreenerValid(false);
      WsfEW_EffectManager* effectManager(WsfEW_EffectManager::Find(*GetSimulation()));
      if (effectManager != nullptr)
      {
         effectManager->RemoveFalseTargetScreener(aFT_JammerPair.first, this);
      }
      mFalseTargetsJammers.erase(ftJammerVecIter);
      removed = true;
   }
   return removed;
}

bool WsfFalseTargetScreener::FindFalseTarget(FT_JammerPair aFT_JammerPair)
{
   bool found = false;

   auto ftJammerVecIter = find(mFalseTargetsJammers.begin(), mFalseTargetsJammers.end(), aFT_JammerPair);
   if (ftJammerVecIter != mFalseTargetsJammers.end())
   {
      found = true;
   }
   return found;
}

bool WsfFalseTargetScreener::RemoveJammerXmtr(WsfEM_Xmtr* aJammerXmtrPtr)
{
   bool removed = false;

   auto ftJammerVecIter = mFalseTargetsJammers.begin();
   while (ftJammerVecIter != mFalseTargetsJammers.end())
   {
      WsfEM_Xmtr* jammerXmtrPtr = (*ftJammerVecIter).second;
      if (jammerXmtrPtr == aJammerXmtrPtr)
      {
         WsfFalseTarget* ftPtr = (*ftJammerVecIter).first;
         ftPtr->RemoveJammer(aJammerXmtrPtr);
         RemoveBlips(ftPtr, aJammerXmtrPtr);
         SetScreenerValid(false);
         WsfEW_EffectManager* effectManager(WsfEW_EffectManager::Find(*GetSimulation()));
         if (effectManager != nullptr)
         {
            effectManager->RemoveFalseTargetScreener(ftPtr, this);
         }
         ftJammerVecIter = mFalseTargetsJammers.erase(ftJammerVecIter);
         removed         = true;
      }
      else
      {
         ++ftJammerVecIter;
      }
   }
   return removed;
}

bool WsfFalseTargetScreener::RemoveFalseTargets(WsfFalseTarget* aFalseTargetPtr)
{
   bool removed = false;

   auto ftJammerVecIter = mFalseTargetsJammers.begin();
   while (ftJammerVecIter != mFalseTargetsJammers.end())
   {
      WsfFalseTarget* ftPtr = (*ftJammerVecIter).first;
      if (ftPtr == aFalseTargetPtr)
      {
         WsfEM_Xmtr* jammerXmtrPtr = (*ftJammerVecIter).second;
         aFalseTargetPtr->RemoveJammer(jammerXmtrPtr);
         RemoveBlips(aFalseTargetPtr, jammerXmtrPtr);
         SetScreenerValid(false);
         WsfEW_EffectManager* effectManager(WsfEW_EffectManager::Find(*GetSimulation()));
         if (effectManager != nullptr)
         {
            effectManager->RemoveFalseTargetScreener(ftPtr, this);
         }
         ftJammerVecIter = mFalseTargetsJammers.erase(ftJammerVecIter);
         removed         = true;
      }
      else
      {
         ++ftJammerVecIter;
      }
   }
   return removed;
}

WsfFalseTarget* WsfFalseTargetScreener::GetFalseTarget(const std::string& aName, WsfEM_Xmtr* aJammerXmtrPtr) const
{
   WsfFalseTarget* ftPtr = nullptr;

   for (const auto& falseTargetsJammer : mFalseTargetsJammers)
   {
      ftPtr                     = falseTargetsJammer.first;
      WsfEM_Xmtr* jammerXmtrPtr = falseTargetsJammer.second;
      if ((ftPtr->GetName() == aName) && (jammerXmtrPtr == aJammerXmtrPtr))
      {
         ftPtr = falseTargetsJammer.first;
         break;
      }
   }
   return ftPtr;
}

WsfEM_Xmtr* WsfFalseTargetScreener::GetJammerXmtr(WsfFalseTarget* aFalseTargetPtr) const
{
   WsfEM_Xmtr* jammerXmtrPtr = nullptr;

   for (const auto& falseTargetsJammer : mFalseTargetsJammers)
   {
      WsfFalseTarget* ftPtr = falseTargetsJammer.first;
      if (ftPtr == aFalseTargetPtr)
      {
         jammerXmtrPtr = falseTargetsJammer.second;
         break;
      }
   }
   return jammerXmtrPtr;
}

void WsfFalseTargetScreener::AddFalseTargetTrack(size_t aBlipId, WsfTrackId aTrackId)
{
   mFalseTargetTrackIds[aBlipId] = aTrackId;
}

void WsfFalseTargetScreener::DropFalseTargetTrack(size_t aBlipId)
{
   mFalseTargetTrackIds.erase(aBlipId);
   mReportingTrackIds.erase(aBlipId);
}

bool WsfFalseTargetScreener::FindFalseTargetTrack(size_t aBlipId)
{
   if (mFalseTargetTrackIds.find(aBlipId) != mFalseTargetTrackIds.end())
   {
      return true;
   }
   return false;
}

void WsfFalseTargetScreener::UpdateTargetTrack(size_t aObjectId, WsfTrack* aValidTrackPtr)
{
   auto forcedTrackIter = mForcedTargetTrackIds.find(aObjectId);
   auto activeTrackIter = mActiveTargetTrackIds.find(aObjectId);

   // check to see if real target is not in the active or forced track list
   if ((activeTrackIter == mActiveTargetTrackIds.end()) && (forcedTrackIter == mForcedTargetTrackIds.end()))
   {
      WsfTrackId validTrackId(nullptr, 0);

      auto blockedTrackIter = mBlockedTargetTrackIds.find(aObjectId);
      auto nullTrackIter    = mNullTargetTrackIds.find(aObjectId);

      // not in active or forced track list, check to see if the track pointer is valid
      if (aValidTrackPtr != nullptr)
      {
         if (aValidTrackPtr->IsFalseTarget())
         {
            return;
         }

         validTrackId = aValidTrackPtr->GetTrackId();
         // string targetName = aValidTrackPtr->GetTargetName();
         //  valid track pointer, check if this target has a null track
         if (nullTrackIter != mNullTargetTrackIds.end())
         {
            // no longer a null track, attempt to add to the active or forced track list
            if (ForceTargetTrack(aValidTrackPtr->GetRange(), aValidTrackPtr->GetBearing(), aValidTrackPtr->GetSignalToNoise()))
            {
               // have capacity, so move the track from the null track list to the active track list
               mNullTargetTrackIds.erase(nullTrackIter);
               mForcedTargetTrackIds.emplace(aObjectId, validTrackId);
            }
            else if ((int)mActiveTargetTrackIds.size() < mAllowedRealTargetNumber)
            {
               // have capacity, so move the track from the null track list to the active track list
               mNullTargetTrackIds.erase(nullTrackIter);
               mActiveTargetTrackIds.emplace(aObjectId, validTrackId);
            }
            else
            {
               // don't have capacity to add the track, move from null list to blocked track list
               mNullTargetTrackIds.erase(nullTrackIter);
               mBlockedTargetTrackIds.emplace(aObjectId, validTrackId);
            }
            mTargetsInvalid = true;
         }
         // not a null track, check to see if it is in the blocked track list
         else if (blockedTrackIter != mBlockedTargetTrackIds.end())
         {
            // this target is being blocked, check to see if it can be added to the force track list
            if (ForceTargetTrack(aValidTrackPtr->GetRange(), aValidTrackPtr->GetBearing(), aValidTrackPtr->GetSignalToNoise()))
            {
               mBlockedTargetTrackIds.erase(blockedTrackIter);
               mForcedTargetTrackIds.emplace(aObjectId, validTrackId);
            }
            mTargetsInvalid = true;
         }
         else // it is not in the blocked list
         {
            // this target is not being blocked, attempt to add to the active track list
            if (ForceTargetTrack(aValidTrackPtr->GetRange(), aValidTrackPtr->GetBearing(), aValidTrackPtr->GetSignalToNoise()))
            {
               mForcedTargetTrackIds.emplace(aObjectId, validTrackId);
            }
            else if ((int)mActiveTargetTrackIds.size() < mAllowedRealTargetNumber)
            {
               mActiveTargetTrackIds.emplace(aObjectId, validTrackId);
            }
            else // track is not forced and don't have capacity to add the track, add to the blocked track list
            {
               mBlockedTargetTrackIds.emplace(aObjectId, validTrackId);
            }
            mTargetsInvalid = true;
         }
      }
      else // track pointer is null
      {
         // remove from blocked track list and add to null track list
         if (blockedTrackIter != mBlockedTargetTrackIds.end())
         {
            mBlockedTargetTrackIds.erase(blockedTrackIter);
            mNullTargetTrackIds.emplace(aObjectId, validTrackId);
         }
         // add to null track list
         else if (nullTrackIter == mNullTargetTrackIds.end())
         {
            mNullTargetTrackIds.emplace(aObjectId, validTrackId);
            mTargetsInvalid = true;
         }
      }
   }
   else // it is in the active or forced track list
   {
      // check to see if it needs to be switched between the active or forced track lists
      if (aValidTrackPtr != nullptr)
      {
         if (aValidTrackPtr->IsFalseTarget())
         {
            return;
         }

         WsfTrackId validTrackId = aValidTrackPtr->GetTrackId();

         if (activeTrackIter != mActiveTargetTrackIds.end())
         {
            if (ForceTargetTrack(aValidTrackPtr->GetRange(), aValidTrackPtr->GetBearing(), aValidTrackPtr->GetSignalToNoise()))
            {
               // move the track from the active track list to the forced track list
               mActiveTargetTrackIds.erase(activeTrackIter);
               mForcedTargetTrackIds.emplace(aObjectId, validTrackId);
               mTargetsInvalid = true;
            }
         }
         else if (forcedTrackIter != mForcedTargetTrackIds.end())
         {
            if (!ForceTargetTrack(aValidTrackPtr->GetRange(),
                                  aValidTrackPtr->GetBearing(),
                                  aValidTrackPtr->GetSignalToNoise()))
            {
               // move the track from the forced track list to the active track list
               mForcedTargetTrackIds.erase(forcedTrackIter);
               mActiveTargetTrackIds.emplace(aObjectId, validTrackId);
               mTargetsInvalid = true;
            }
         }
      }
   }
}

// public
WsfFalseTargetScreener::TrackIdMap WsfFalseTargetScreener::GetActiveTargetTracks() const
{
   // Have to merge the forced and active list a they are both considered to be active.
   TrackIdMap tempTrackIdMap = mActiveTargetTrackIds;
   tempTrackIdMap.insert(mForcedTargetTrackIds.begin(), mForcedTargetTrackIds.end());
   return tempTrackIdMap;
}

// public
void WsfFalseTargetScreener::DropTargetTrack(size_t aObjectId)
{
   mActiveTargetTrackIds.erase(aObjectId);
   mForcedTargetTrackIds.erase(aObjectId);
   mBlockedTargetTrackIds.erase(aObjectId); // Remove from blocked list in case the object went away in the simulation
   mReportingTrackIds.erase(aObjectId);
}

void WsfFalseTargetScreener::RemoveBlips(WsfFalseTarget* aFalseTargetPtr, WsfEM_Xmtr* aJammerXmtrPtr)
{
   RemoveBlockedBlips(aFalseTargetPtr, aJammerXmtrPtr);
   RemoveUndetectedBlips(aFalseTargetPtr, aJammerXmtrPtr);
   RemoveTrackableBlips(aFalseTargetPtr, aJammerXmtrPtr);
}

void WsfFalseTargetScreener::RemoveTrackableBlips(WsfFalseTarget* aFalseTargetPtr, WsfEM_Xmtr* aJammerXmtr)
{
   auto blipIter = mTrackableBlipIds.begin();
   while (blipIter != mTrackableBlipIds.end())
   {
      WsfFalseTarget::Blip* blipPtr = aFalseTargetPtr->GetBlipPtr(blipIter->first, blipIter->second.second->GetUniqueId());
      if (blipPtr == nullptr)
      {
         blipIter      = mTrackableBlipIds.erase(blipIter);
         mBlipsInvalid = true;
      }
      else if ((blipPtr->GetFalseTarget() == aFalseTargetPtr) && (blipPtr->GetJammerXmtr() == aJammerXmtr))
      {
         blipIter      = mTrackableBlipIds.erase(blipIter);
         mBlipsInvalid = true;
      }
      else
      {
         ++blipIter;
      }
   }
}

void WsfFalseTargetScreener::RemoveBlockedBlips(WsfFalseTarget* aFalseTargetPtr, WsfEM_Xmtr* aJammerXmtr)
{
   auto blipIter = mBlockedBlipIds.begin();
   while (blipIter != mBlockedBlipIds.end())
   {
      WsfFalseTarget::Blip* blipPtr = aFalseTargetPtr->GetBlipPtr(blipIter->first, blipIter->second.second->GetUniqueId());
      if (blipPtr == nullptr)
      {
         blipIter = mBlockedBlipIds.erase(blipIter);
      }
      else if ((blipPtr->GetFalseTarget() == aFalseTargetPtr) && (blipPtr->GetJammerXmtr() == aJammerXmtr))
      {
         blipIter = mBlockedBlipIds.erase(blipIter);
      }
      else
      {
         ++blipIter;
      }
   }
}

void WsfFalseTargetScreener::RemoveUndetectedBlips(WsfFalseTarget* aFalseTargetPtr, WsfEM_Xmtr* aJammerXmtr)
{
   auto blipIter = mUndetectedBlipIds.begin();
   while (blipIter != mUndetectedBlipIds.end())
   {
      WsfFalseTarget::Blip* blipPtr =
         blipIter->second.first->GetBlipPtr(blipIter->first, blipIter->second.second->GetUniqueId());
      if (blipPtr == nullptr)
      {
         blipIter = mUndetectedBlipIds.erase(blipIter);
      }
      else if ((blipPtr->GetFalseTarget() == aFalseTargetPtr) && (blipPtr->GetJammerXmtr() == aJammerXmtr))
      {
         blipIter = mUndetectedBlipIds.erase(blipIter);
      }
      else
      {
         ++blipIter;
      }
   }
}

WsfFalseTarget::Blip* WsfFalseTargetScreener::GetTrackableBlip(size_t aUniqueId)
{
   WsfFalseTarget::Blip* blipPtr = nullptr;
   auto                  it      = std::find_if(mTrackableBlipIds.begin(),
                          mTrackableBlipIds.end(),
                          [aUniqueId](const std::pair<size_t, FT_JammerPair>& aTrackableBlipId)
                          { return aTrackableBlipId.first == aUniqueId; });
   if (it != mTrackableBlipIds.end())
   {
      blipPtr = it->second.first->GetBlipPtr(it->first, it->second.second->GetUniqueId());
   }
   return blipPtr;
}

bool WsfFalseTargetScreener::FindTrackableBlip(size_t aUniqueId)
{
   return std::any_of(mTrackableBlipIds.begin(),
                      mTrackableBlipIds.end(),
                      [aUniqueId](const std::pair<size_t, FT_JammerPair>& aTrackableBlipId)
                      { return aTrackableBlipId.first == aUniqueId; });
}

bool WsfFalseTargetScreener::FindUndetectedBlip(size_t aUniqueId)
{
   return std::any_of(mUndetectedBlipIds.begin(),
                      mUndetectedBlipIds.end(),
                      [aUniqueId](const std::pair<size_t, FT_JammerPair>& aUndetectedBlipId)
                      { return aUndetectedBlipId.first == aUniqueId; });
}

WsfFalseTarget::Blip* WsfFalseTargetScreener::GetUndetectedBlip(size_t aUniqueId)
{
   WsfFalseTarget::Blip* blipPtr = nullptr;
   auto                  it      = std::find_if(mUndetectedBlipIds.begin(),
                          mUndetectedBlipIds.end(),
                          [aUniqueId](const std::pair<size_t, FT_JammerPair>& aUndetectedBlipId)
                          { return aUndetectedBlipId.first == aUniqueId; });
   if (it != mUndetectedBlipIds.end())
   {
      blipPtr = it->second.first->GetBlipPtr(it->first, it->second.second->GetUniqueId());
   }
   return blipPtr;
}

bool WsfFalseTargetScreener::FindBlockedBlip(size_t aUniqueId)
{
   return std::any_of(mBlockedBlipIds.begin(),
                      mBlockedBlipIds.end(),
                      [aUniqueId](const std::pair<size_t, FT_JammerPair>& aBlockedBlipId)
                      { return aBlockedBlipId.first == aUniqueId; });
}

WsfFalseTarget::Blip* WsfFalseTargetScreener::GetBlockedBlip(size_t aUniqueId)
{
   WsfFalseTarget::Blip* blipPtr = nullptr;
   auto                  it      = std::find_if(mBlockedBlipIds.begin(),
                          mBlockedBlipIds.end(),
                          [aUniqueId](const std::pair<size_t, FT_JammerPair>& aBlockedBlipId)
                          { return aBlockedBlipId.first == aUniqueId; });
   if (it != mBlockedBlipIds.end())
   {
      blipPtr = it->second.first->GetBlipPtr(it->first, it->second.second->GetUniqueId());
   }
   return blipPtr;
}

void WsfFalseTargetScreener::UpdateBlips(double aSimTime, WsfSensor::Settings& aSettings)
{
   if ((fabs(aSimTime - mLastBlipUpdate) < 1.0E-5) && (!mFT_JammerStatusChange))
   {
      return;
   }

   // TODO_JAJ Do the settings need to be updated to set the mode index of the mode to which
   // TODO_JAJ this is attached?
   WsfEW_SensorComponent* ewComponentPtr = WsfEW_SensorComponent::Find(*mSensorModePtr->GetSensor());

   if (mDebugEnabled)
   {
      auto out = ut::log::debug() << "Sensor Platform's Mode False Target Screener updating blips.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Sensor Platform: " << mSensorModePtr->GetPlatform()->GetName();
      out.AddNote() << "Mode: " << mSensorModePtr->GetName();
      out.AddNote() << "Last Update: " << mLastBlipUpdate;
   }

   mBlipIdToForceDataMap.clear();

   double                       locationWCS[3];
   double                       velocityWCS[3];
   double                       orientationWCS[3];
   WsfSensorResult              result;
   WsfSensorResult*             resultptr;
   WsfFalseTarget::Blip*        blipPtr;
   BlipIdFTJammerList::iterator blipIter;

   WsfFalseTarget::RadarSnsrPair radarIdKey =
      WsfFalseTarget::RadarSnsrPair(mSensorModePtr->GetPlatform()->GetNameId(), mSensorModePtr->GetSensor()->GetNameId());

   // Update blocked blips
   blipIter = mBlockedBlipIds.begin();
   while (blipIter != mBlockedBlipIds.end())
   {
      blipPtr = blipIter->second.first->GetBlipPtr(blipIter->first, blipIter->second.second->GetUniqueId());
      if (blipPtr == nullptr)
      {
         blipIter = mBlockedBlipIds.erase(blipIter);
      }
      else
      {
         mBlipsAllocation[blipPtr->GetBehavior()] = 1;

         if (!blipPtr->IsValidTargetedRadarId(radarIdKey))
         {
            blipPtr->AddTargetedRadarId(radarIdKey);
         }
         else
         {
            blipPtr->ResetBlipData(radarIdKey);
         }

         resultptr = static_cast<WsfSensorResult*>(blipPtr->GetInteraction(radarIdKey));
         if (resultptr == nullptr)
         {
            resultptr = new WsfSensorResult;
            blipPtr->SetInteraction(radarIdKey, resultptr);
         }

         blipPtr->GetOrientationWCS(orientationWCS[0], orientationWCS[1], orientationWCS[2]);
         blipPtr->GetLocationWCS(locationWCS);
         blipPtr->GetVelocityWCS(velocityWCS);
         mTempPlatformPtr->SetOrientationNED(orientationWCS[0], orientationWCS[1], orientationWCS[2]);
         mTempPlatformPtr->SetLocationWCS(locationWCS);
         mTempPlatformPtr->SetVelocityWCS(velocityWCS);
         mTempPlatformPtr->SetName(blipPtr->GetNameId());

         if (!ewComponentPtr->AttemptToDetectFalseTarget(aSimTime, blipPtr, aSettings, *resultptr))
         {
            mUndetectedBlipIds.push_back(*blipIter);
            blipIter      = mBlockedBlipIds.erase(blipIter);
            mBlipsInvalid = true;
         }
         else
         {
            ForceData forceData(resultptr->mRcvrToTgt.mAz, resultptr->mRcvrToTgt.mRange, resultptr->mSignalToNoise);
            mBlipIdToForceDataMap.emplace(blipIter->first, forceData);
            ++blipIter;
         }
      }
   }

   // Update undetected blips
   blipIter = mUndetectedBlipIds.begin();
   while (blipIter != mUndetectedBlipIds.end())
   {
      blipPtr = blipIter->second.first->GetBlipPtr(blipIter->first, blipIter->second.second->GetUniqueId());
      if (blipPtr == nullptr)
      {
         blipIter = mUndetectedBlipIds.erase(blipIter);
      }
      else
      {
         mBlipsAllocation[blipPtr->GetBehavior()] = 1;

         if (!blipPtr->IsValidTargetedRadarId(radarIdKey))
         {
            blipPtr->AddTargetedRadarId(radarIdKey);
         }
         else
         {
            blipPtr->ResetBlipData(radarIdKey);
         }

         resultptr = static_cast<WsfSensorResult*>(blipPtr->GetInteraction(radarIdKey));
         if (resultptr == nullptr)
         {
            resultptr = new WsfSensorResult;
            blipPtr->SetInteraction(radarIdKey, resultptr);
         }

         blipPtr->GetOrientationWCS(orientationWCS[0], orientationWCS[1], orientationWCS[2]);
         blipPtr->GetLocationWCS(locationWCS);
         blipPtr->GetVelocityWCS(velocityWCS);
         mTempPlatformPtr->SetOrientationNED(orientationWCS[0], orientationWCS[1], orientationWCS[2]);
         mTempPlatformPtr->SetLocationWCS(locationWCS);
         mTempPlatformPtr->SetVelocityWCS(velocityWCS);
         mTempPlatformPtr->SetName(blipPtr->GetNameId());

         if (ewComponentPtr->AttemptToDetectFalseTarget(aSimTime, blipPtr, aSettings, *resultptr))
         {
            mBlockedBlipIds.push_back(*blipIter);
            blipIter      = mUndetectedBlipIds.erase(blipIter);
            mBlipsInvalid = true;
         }
         else
         {
            ++blipIter;
         }
      }
   }

   // Update track-able blips
   double tempMinRange(std::numeric_limits<double>::max());
   double tempMaxRange(0.0);
   blipIter = mTrackableBlipIds.begin();
   while (blipIter != mTrackableBlipIds.end())
   {
      blipPtr = blipIter->second.first->GetBlipPtr(blipIter->first, blipIter->second.second->GetUniqueId());
      if (blipPtr == nullptr)
      {
         blipIter      = mTrackableBlipIds.erase(blipIter);
         mBlipsInvalid = true;
      }
      else
      {
         mBlipsAllocation[blipPtr->GetBehavior()] = 1;

         if (!blipPtr->IsValidTargetedRadarId(radarIdKey))
         {
            blipPtr->AddTargetedRadarId(radarIdKey);
         }
         else
         {
            blipPtr->ResetBlipData(radarIdKey);
         }

         resultptr = static_cast<WsfSensorResult*>(blipPtr->GetInteraction(radarIdKey));
         if (resultptr == nullptr)
         {
            resultptr = new WsfSensorResult;
            blipPtr->SetInteraction(radarIdKey, resultptr);
         }

         blipPtr->GetOrientationWCS(orientationWCS[0], orientationWCS[1], orientationWCS[2]);
         blipPtr->GetLocationWCS(locationWCS);
         blipPtr->GetVelocityWCS(velocityWCS);
         mTempPlatformPtr->SetOrientationNED(orientationWCS[0], orientationWCS[1], orientationWCS[2]);
         mTempPlatformPtr->SetLocationWCS(locationWCS);
         mTempPlatformPtr->SetVelocityWCS(velocityWCS);
         mTempPlatformPtr->SetName(blipPtr->GetNameId());

         if (!ewComponentPtr->AttemptToDetectFalseTarget(aSimTime, blipPtr, aSettings, *resultptr))
         {
            mUndetectedBlipIds.push_back(*blipIter);
            blipIter      = mTrackableBlipIds.erase(blipIter);
            mBlipsInvalid = true;
         }
         else
         {
            tempMinRange = std::min(tempMinRange, resultptr->mRcvrToTgt.mRange);
            tempMaxRange = std::max(tempMaxRange, resultptr->mRcvrToTgt.mRange);

            ForceData forceData(resultptr->mRcvrToTgt.mAz, resultptr->mRcvrToTgt.mRange, resultptr->mSignalToNoise);
            mBlipIdToForceDataMap.emplace(blipIter->first, forceData);

            ++blipIter;
         }
      }
   }

   mMinimumBlipRange = 0.0;
   mMaximumBlipRange = std::numeric_limits<double>::max();
   if (!mTrackableBlipIds.empty())
   {
      mMinimumBlipRange = tempMinRange;
      mMaximumBlipRange = tempMaxRange;
   }

   RejectBlips(aSimTime);

   mLastBlipUpdate = aSimTime;
}

void WsfFalseTargetScreener::CheckFT_Toggle()
{
   for (auto& falseTargetsJammer : mFalseTargetsJammers)
   {
      WsfFalseTarget* ftPtr = falseTargetsJammer.first;
      if (ftPtr->GetToggled())
      {
         mScreenerValid = false;
         ftPtr->SetToggled(false);
      }
   }
}

// private
bool WsfFalseTargetScreener::CreateBlips(WsfFalseTarget* aFalseTargetPtr, WsfEM_Xmtr* aJammerXmtrPtr)
{
   if (!aFalseTargetPtr->GetNewBlipsState(this, aJammerXmtrPtr))
   {
      return false;
   } // No new false target blips to process
   aFalseTargetPtr->SetNewBlipsState(this, aJammerXmtrPtr, false);

   // Get rejection information for initial rejection of false-targets
   double ftRejectionRatio = 0.00; // default is to reject no false targets
   auto   rrIter           = mInitialRejectionRatios.find(aFalseTargetPtr->GetBlipBehavior());
   if (rrIter != mInitialRejectionRatios.end())
   {
      ftRejectionRatio = (*rrIter).second;
   }
   size_t falseTargetNumber = static_cast<size_t>(aFalseTargetPtr->GetInitialNumberOfFTs() * (1.0 - ftRejectionRatio));

   WsfFalseTarget::Blips tempBlips = aFalseTargetPtr->GetBlips(aJammerXmtrPtr->GetUniqueId());

   RemoveBlips(aFalseTargetPtr, aJammerXmtrPtr);
   FT_JammerPair ftJammerPair = std::make_pair(aFalseTargetPtr, aJammerXmtrPtr);

   size_t falseTargetsAdded = 0;
   for (WsfFalseTarget::Blip* tempBlipPtr : tempBlips)
   {
      if (tempBlipPtr != nullptr)
      {
         if ((!FindUndetectedBlip(tempBlipPtr->GetUniqueId())) && (aFalseTargetPtr->GetState() == WsfFalseTarget::cOn))
         {
            mUndetectedBlipIds.emplace_back(make_pair(tempBlipPtr->GetUniqueId(), ftJammerPair));
            ++falseTargetsAdded;
         }
      }

      if (falseTargetsAdded >= falseTargetNumber)
      {
         break;
      }
   }

   if (mDebugEnabled)
   {
      auto out = ut::log::debug() << "Platform's Sensor Mode collected False Target blips.";
      out.AddNote() << "Platform: " << mSensorModePtr->GetPlatform()->GetName();
      out.AddNote() << "Sensor: " << mSensorModePtr->GetSensor()->GetName();
      out.AddNote() << "Mode: " << mSensorModePtr->GetName();
      out.AddNote() << "False Target Screener: " << GetName();
      out.AddNote() << "Jammer Articulated Part Platform: "
                    << aJammerXmtrPtr->GetArticulatedPart()->GetPlatform()->GetName();
      out.AddNote() << "Jammer Articulated Part: " << aJammerXmtrPtr->GetArticulatedPart()->GetName();
      out.AddNote() << "False Target: " << aFalseTargetPtr->GetName();
   }

   return (falseTargetsAdded > 0);
}

bool WsfFalseTargetScreener::IsRadarModeTurnedOn()
{
   return (mSensorModePtr->GetSensor()->IsTurnedOn() && mSensorModePtr->IsSelected());
}

void WsfFalseTargetScreener::PrintScreenerState(double aSimTime)
{
   auto out = ut::log::info() << "Printing screener debug state for Platform.Sensor.Mode:FalseTarget.";
   out.AddNote() << "Platform: " << mSensorModePtr->GetPlatform()->GetName();
   out.AddNote() << "Sensor: " << mSensorModePtr->GetSensor()->GetName();
   out.AddNote() << "Mode: " << mSensorModePtr->GetName();
   out.AddNote() << "False Target: " << GetName();
   out.AddNote() << "Track Capacity: " << mTrackCapacity;
   out.AddNote() << "Transfer Capacity: " << mTransferCapacity;
   out.AddNote() << "Forced Target Tracks: " << mForcedTargetTrackIds.size();
   out.AddNote() << "Active Target Tracks: " << mActiveTargetTrackIds.size();
   out.AddNote() << "Blocked Target Tracks: " << mBlockedTargetTrackIds.size();
   out.AddNote() << "Null Target Tracks: " << mNullTargetTrackIds.size();
   out.AddNote() << "Reported Tracks: " << mReportingTrackIds.size();
   out.AddNote() << "False Target Tracks: " << mTrackableBlipIds.size();
}

bool WsfFalseTargetScreener::AllowTargetTrack(double                 aSimTime,
                                              size_t                 aObjectId,
                                              WsfTrack*              aTrackPtr,
                                              const WsfSensorResult& aResult)
{
   WsfSensor::Settings settings;
   if (!mScreenerValid)
   {
      settings.mModeIndex  = aResult.mModeIndex;
      settings.mRequiredPd = aResult.mRequiredPd;
      Update(aSimTime, settings);
   }

   // will be added to the forced, active or blocked track list if aTrackPtr is not null,
   // will be added to the null track list if aTrackPtr is null, will be updated if it is
   // already in any of the lists.
   UpdateTargetTrack(aObjectId, aTrackPtr);

   if (NeedsUpdated(aSimTime))
   {
      settings.mModeIndex  = aResult.mModeIndex;
      settings.mRequiredPd = aResult.mRequiredPd;
      Update(aSimTime, settings);
   }

   // A target is being presented to see if the presence of false targets
   // precludes or disrupts its detection
   if (mFalseTargetsJammers.empty() || // false target are not present
       (mAllowedRealTargetNumber > (int)(mForcedTargetTrackIds.size() + mActiveTargetTrackIds.size())) ||
       (mActiveTargetTrackIds.find(aObjectId) != mActiveTargetTrackIds.end()) ||
       (mForcedTargetTrackIds.find(aObjectId) != mForcedTargetTrackIds.end()) ||
       ((mNullTargetTrackIds.find(aObjectId) != mNullTargetTrackIds.end()) &&
        ForceTargetTrack(aResult.mTgtToRcvr.mRange, aResult.mRcvrToTgt.mAz, aResult.mSignalToNoise)))

   {
      return true;
   }

   return false;
}

bool WsfFalseTargetScreener::AllowTrackReporting(size_t aObjectId, const WsfTrack* aTrackPtr)
{
   bool allowTrackReporting = false;

   // A track is being presented to see if the presence of false targets
   // precludes or disrupts its reporting status
   if (mFalseTargetsJammers.empty()) // false target are not present
   {
      allowTrackReporting = true;
   }
   else if (mTrackReportingWhenFlooded)
   {
      if (((int)mReportingTrackIds.size() < mTransferCapacity) ||
          (mReportingTrackIds.find(aObjectId) != mReportingTrackIds.end()))
      {
         allowTrackReporting = true;
      }
   }
   else if ((int)(mForcedTargetTrackIds.size() + mActiveTargetTrackIds.size() + mFalseTargetTrackIds.size()) <
            mTransferCapacity)
   {
      allowTrackReporting = true;
   }

   if (allowTrackReporting)
   {
      mReportingTrackIds[aObjectId] = aTrackPtr->GetTrackId();
   }
   else
   {
      mReportingTrackIds.erase(aObjectId);
   }

   return allowTrackReporting;
}

bool WsfFalseTargetScreener::ForceTargetTrack(double aRadar2TargetRange, double aRadar2TargetAzimuth, double aTargetSNR)
{
   bool forceTrack = false;

   double minBlipRange = mMinimumBlipRange;
   double maxBlipRange = mMaximumBlipRange;
   int    sectorCount  = 0;

   if (((UtMath::cTWO_PI - mForceSectorArc) > 0.001) || ((std::numeric_limits<double>::max() - mForceRange) > 0.001))
   {
      double azMin(0.0);
      double azMax(UtMath::cTWO_PI);
      if ((UtMath::cTWO_PI - mForceSectorArc) > 0.001)
      {
         azMin = UtMath::NormalizeAngle0_TwoPi(aRadar2TargetAzimuth - (mForceSectorArc / 2));
         azMax = UtMath::NormalizeAngle0_TwoPi(aRadar2TargetAzimuth + (mForceSectorArc / 2));
      }

      double rangeMin = std::max(0.0, aRadar2TargetRange - mForceRange);
      double rangeMax(std::numeric_limits<double>::max());
      if ((std::numeric_limits<double>::max() - mForceRange) > aRadar2TargetRange)
      {
         rangeMax = aRadar2TargetRange + mForceRange;
      }

      double tempMinRange(std::numeric_limits<double>::max());
      double tempMaxRange(0.0);
      for (auto& falseTargetTrackId : mFalseTargetTrackIds)
      {
         auto brai = mBlipIdToForceDataMap.find(falseTargetTrackId.first);
         if (brai != mBlipIdToForceDataMap.end())
         {
            double range   = brai->second.mRange;
            double azimuth = UtMath::NormalizeAngle0_TwoPi(brai->second.mAzimuth);

            // watch for case where the SNR is not reported by the sensor
            double JtoS = ((aTargetSNR > 0.0) && (mReportsSignalToNoise)) ? (brai->second.mSNR / aTargetSNR) : 0.0;

            if ((azimuth >= azMin) && (azimuth <= azMax) && (range >= rangeMin) && (range <= rangeMax) &&
                ((JtoS <= 0.0) ||
                 ((JtoS >=
                   (mForceJtoSDeltaThreshold < std::numeric_limits<double>::max() ? (1 / mForceJtoSDeltaThreshold) : 0.0)) &&
                  (JtoS <= mForceJtoSDeltaThreshold))))
            {
               tempMinRange = std::min(tempMinRange, range);
               tempMaxRange = std::max(tempMaxRange, range);
               ++sectorCount;
            }
         }
      }

      if (sectorCount > 0)
      {
         minBlipRange = tempMinRange;
         maxBlipRange = tempMaxRange;
      }
   }

   int rangeType = static_cast<int>(mRangeForceType);

   if (((aRadar2TargetRange <= minBlipRange) && ((rangeType & cFORCE_INSIDE) != 0)) ||
       ((aRadar2TargetRange >= maxBlipRange) && ((rangeType & cFORCE_OUTSIDE) != 0)) ||
       ((mForceCountPerRangeSector > 0) && (sectorCount <= mForceCountPerRangeSector)))
   {
      forceTrack = true;
   }

   return forceTrack;
}

// private
void WsfFalseTargetScreener::AdjustTargetsAndBlips(double aSimTime)
{
   mBlipsAdjusted   = false;
   mTargetsAdjusted = false;

   // if (RejectBlips(aSimTime)) { mBlipsAdjusted = true; }

   if (mBlipsInvalid || mTargetsInvalid)
   {
      int realTargets  = static_cast<int>(mActiveTargetTrackIds.size() + mBlockedTargetTrackIds.size() +
                                         mForcedTargetTrackIds.size() + mNullTargetTrackIds.size());
      int falseTargets = static_cast<int>(mTrackableBlipIds.size() + mBlockedBlipIds.size());
      int totalTargets = realTargets + falseTargets;

      if (totalTargets <= mTrackCapacity)
      {
         mAllowedRealTargetNumber = std::max(0, mTrackCapacity - falseTargets);
      }
      else
      {
         mAllowedRealTargetNumber = std::max((int)mForcedTargetTrackIds.size(),
                                             (int)((mTrackCapacity * ((float)realTargets / (float)totalTargets))));
      }

      // Adjust target track quantities
      // only need to worry about pruning as the AddTargetTrack(...) method
      // categorizes the target tracks in the correct lists
      std::ptrdiff_t deltaQuantity =
         static_cast<int>(mForcedTargetTrackIds.size() + mActiveTargetTrackIds.size()) - mAllowedRealTargetNumber;
      if (deltaQuantity > 0)
      {
         PruneTargetTracks(deltaQuantity);
      }

      // Adjust the blip quantities, must be after the targets have been adjusted
      mAllowedFalseTargetNumber =
         mTrackCapacity - static_cast<int>(mForcedTargetTrackIds.size() + mActiveTargetTrackIds.size());
      deltaQuantity = mAllowedFalseTargetNumber - static_cast<int>(mTrackableBlipIds.size());
      if (deltaQuantity > 0)
      {
         AddTrackableBlips(deltaQuantity);
      }
      else if (deltaQuantity < 0)
      {
         PruneTrackableBlips(-deltaQuantity);
      }
   }

   // reset the invalid flags
   mBlipsInvalid   = false;
   mTargetsInvalid = false;
}

// Rejects false-target blips based rejection ratios entered in the input.
// Care must be taken not to call this more than once per UpdateBlips(...) call,
// as it moves the blips from the track-able and blocked lists into the undetected list.
// private
bool WsfFalseTargetScreener::RejectBlips(double aSimTime)
{
   bool blipsRejected(false);

   if (fabs(aSimTime - mLastBlipUpdate) <= 0.001)
   {
      return false;
   }

   if ((mRejectionRatios[WsfFalseTarget::cRANDOM_SCAN_TO_SCAN] > 0.0) ||
       (mRejectionRatios[WsfFalseTarget::cCONSISTENT_SCAN_TO_SCAN] > 0.0))
   {
      // Count blips in each behavior for all track-able and blocked blips lists
      unsigned int randomBlipCount(0);
      unsigned int consistentBlipCount(0);

      for (auto& trackableBlipId : mTrackableBlipIds)
      {
         WsfFalseTarget::Blip* tempBlipPtr =
            trackableBlipId.second.first->GetBlipPtr(trackableBlipId.first, trackableBlipId.second.second->GetUniqueId());
         if (tempBlipPtr != nullptr)
         {
            if (tempBlipPtr->GetBehavior() == WsfFalseTarget::cRANDOM_SCAN_TO_SCAN)
            {
               ++randomBlipCount;
            }

            if (tempBlipPtr->GetBehavior() == WsfFalseTarget::cCONSISTENT_SCAN_TO_SCAN)
            {
               ++consistentBlipCount;
            }
         }
      }

      for (auto& blockedBlipId : mBlockedBlipIds)
      {
         WsfFalseTarget::Blip* tempBlipPtr =
            blockedBlipId.second.first->GetBlipPtr(blockedBlipId.first, blockedBlipId.second.second->GetUniqueId());
         if (tempBlipPtr != nullptr)
         {
            if (tempBlipPtr->GetBehavior() == WsfFalseTarget::cRANDOM_SCAN_TO_SCAN)
            {
               ++randomBlipCount;
            }

            if (tempBlipPtr->GetBehavior() == WsfFalseTarget::cCONSISTENT_SCAN_TO_SCAN)
            {
               ++consistentBlipCount;
            }
         }
      }

      // Check counts and reject random_scan_to_scan blips
      int numRandomToReject     = (int)(randomBlipCount * mRejectionRatios[WsfFalseTarget::cRANDOM_SCAN_TO_SCAN]);
      int numConsistentToReject = (int)(consistentBlipCount * mRejectionRatios[WsfFalseTarget::cCONSISTENT_SCAN_TO_SCAN]);
      if ((numRandomToReject > 0.0) || (numConsistentToReject > 0.0))
      {
         TrackShuffle(mBlockedBlipIds.begin(), mBlockedBlipIds.end(), mRandom);
         TrackShuffle(mTrackableBlipIds.begin(), mTrackableBlipIds.end(), mRandom);

         int numRandomRejected(0);
         int numConsistentRejected(0);
         while (((numRandomRejected != numRandomToReject) || (numConsistentRejected != numConsistentToReject)) &&
                (!mBlockedBlipIds.empty()))
         {
            auto                  blipIter = mBlockedBlipIds.begin();
            WsfFalseTarget::Blip* tempBlipPtr =
               blipIter->second.first->GetBlipPtr(blipIter->first, blipIter->second.second->GetUniqueId());
            if (tempBlipPtr != nullptr)
            {
               if ((tempBlipPtr->GetBehavior() == WsfFalseTarget::cRANDOM_SCAN_TO_SCAN) &&
                   (numRandomRejected != numRandomToReject))
               {
                  mUndetectedBlipIds.push_back(*blipIter);
                  mBlockedBlipIds.erase(blipIter);
                  blipsRejected = true;
                  ++numRandomRejected;
               }

               if ((tempBlipPtr->GetBehavior() == WsfFalseTarget::cCONSISTENT_SCAN_TO_SCAN) &&
                   (numConsistentRejected != numConsistentToReject))
               {
                  mUndetectedBlipIds.push_back(*blipIter);
                  mBlockedBlipIds.erase(blipIter);
                  blipsRejected = true;
                  ++numConsistentRejected;
               }
            }
         }

         while (((numRandomRejected != numRandomToReject) || (numConsistentRejected != numConsistentToReject)) &&
                (!mTrackableBlipIds.empty()))
         {
            auto                  blipIter = mTrackableBlipIds.begin();
            WsfFalseTarget::Blip* tempBlipPtr =
               blipIter->second.first->GetBlipPtr(blipIter->first, blipIter->second.second->GetUniqueId());
            if (tempBlipPtr != nullptr)
            {
               if ((tempBlipPtr->GetBehavior() == WsfFalseTarget::cRANDOM_SCAN_TO_SCAN) &&
                   (numRandomRejected != numRandomToReject))
               {
                  mUndetectedBlipIds.push_back(*blipIter);
                  mTrackableBlipIds.erase(blipIter);
                  blipsRejected = true;
                  ++numRandomRejected;
               }

               if ((tempBlipPtr->GetBehavior() == WsfFalseTarget::cCONSISTENT_SCAN_TO_SCAN) &&
                   (numConsistentRejected != numConsistentToReject))
               {
                  mUndetectedBlipIds.push_back(*blipIter);
                  mTrackableBlipIds.erase(blipIter);
                  blipsRejected = true;
                  ++numConsistentRejected;
               }
            }
         }
      }

      mBlipsInvalid |= blipsRejected;
   }
   return blipsRejected;
}

size_t WsfFalseTargetScreener::PruneTargetTracks(size_t aPruneQuantity)
{
   if (mActiveTargetTrackIds.empty())
   {
      return 0;
   }

   size_t prunedQuantity = 0;

   if (aPruneQuantity > 0) // need to prune some real targets
   {
      TargetIds tempRT_Ids;
      for (auto& activeTargetTrackId : mActiveTargetTrackIds)
      {
         size_t tempId = activeTargetTrackId.first;
         tempRT_Ids.push_back(tempId);
      }

      TrackShuffle(tempRT_Ids.begin(), tempRT_Ids.end(), mRandom);

      if (mActiveTargetTrackIds.size() > aPruneQuantity)
      {
         // prune targets from active list and move to blocked list
         // Note: numberToPrune already guaranteed to be greater than 0  from check above
         prunedQuantity = aPruneQuantity;
         for (size_t pruneIndex = 0; pruneIndex < aPruneQuantity; ++pruneIndex)
         {
            size_t     tempId      = tempRT_Ids[pruneIndex];
            auto       rtIter      = mActiveTargetTrackIds.find(tempId);
            size_t     id          = (*rtIter).first;
            WsfTrackId tempTrackId = (*rtIter).second;
            mActiveTargetTrackIds.erase(rtIter);
            mBlockedTargetTrackIds.emplace(id, tempTrackId);
         }
      }
      else
      {
         // prune targets from active list and move to blocked list
         prunedQuantity = mActiveTargetTrackIds.size();
         mBlockedTargetTrackIds.insert(mActiveTargetTrackIds.begin(), mActiveTargetTrackIds.end());
         mActiveTargetTrackIds.clear();
      }
   }

   if (prunedQuantity > 0)
   {
      mTargetsAdjusted = true;
   }

   return aPruneQuantity;
}

// private
size_t WsfFalseTargetScreener::AddTrackableBlips(size_t aAddQuantity)
{
   if (mBlockedBlipIds.empty())
   {
      return 0;
   }

   size_t addedQuantity = 0;

   WsfFalseTarget::RadarSnsrPair radarIdKey =
      std::make_pair(mSensorModePtr->GetPlatform()->GetNameId(), mSensorModePtr->GetSensor()->GetNameId());

   TrackShuffle(mBlockedBlipIds.begin(), mBlockedBlipIds.end(), mRandom);

   auto blipIter = mBlockedBlipIds.begin();
   while ((addedQuantity < aAddQuantity) && (blipIter != mBlockedBlipIds.end()))
   {
      WsfFalseTarget::Blip* tempBlipPtr =
         blipIter->second.first->GetBlipPtr(blipIter->first, blipIter->second.second->GetUniqueId());
      if (tempBlipPtr == nullptr)
      {
         blipIter = mBlockedBlipIds.erase(blipIter);
      }
      else if (tempBlipPtr->GetStrength(radarIdKey) != WsfFalseTargetBlip::cUndetected)
      {
         mTrackableBlipIds.push_back(*blipIter);
         blipIter = mBlockedBlipIds.erase(blipIter);
         ;
         ++addedQuantity;
      }
      else
      {
         ++blipIter;
      }
   }

   if (addedQuantity > 0)
   {
      mBlipsAdjusted = true;
   }

   return addedQuantity;
}

// private
size_t WsfFalseTargetScreener::PruneTrackableBlips(size_t aPruneQuantity)
{
   if (mTrackableBlipIds.empty())
   {
      return 0;
   }

   std::ptrdiff_t prunedQuantity = 0;
   if (aPruneQuantity > 0)
   {
      TrackShuffle(mTrackableBlipIds.begin(), mTrackableBlipIds.end(), mRandom);

      if (mTrackableBlipIds.size() > aPruneQuantity)
      {
         prunedQuantity            = aPruneQuantity;
         std::ptrdiff_t pruneStart = mTrackableBlipIds.size() - aPruneQuantity;
         auto           blipIter   = mTrackableBlipIds.begin() + pruneStart;
         mBlockedBlipIds.insert(mBlockedBlipIds.end(), blipIter, mTrackableBlipIds.end());
         mTrackableBlipIds.erase(blipIter, mTrackableBlipIds.end());
      }
      else
      {
         prunedQuantity = mTrackableBlipIds.size();
         mBlockedBlipIds.insert(mBlockedBlipIds.end(), mTrackableBlipIds.begin(), mTrackableBlipIds.end());
         mTrackableBlipIds.clear();
      }
   }

   if (prunedQuantity > 0)
   {
      mBlipsAdjusted = true;
   }

   return ut::safe_cast<size_t, ptrdiff_t>(prunedQuantity);
}

bool WsfFalseTargetScreener::TrackExists(size_t aObjectId)
{
   auto rtIter = mActiveTargetTrackIds.find(aObjectId);
   if (rtIter != mActiveTargetTrackIds.end())
   {
      return true;
   }

   rtIter = mForcedTargetTrackIds.find(aObjectId);
   if (rtIter != mForcedTargetTrackIds.end())
   {
      return true;
   }
   return false;
}

void WsfFalseTargetScreener::TracksTransfered()
{
   int inputRTs   = static_cast<int>(mActiveTargetTrackIds.size() + mForcedTargetTrackIds.size());
   int inputFTs   = static_cast<int>(mTrackableBlipIds.size());
   int inputTotal = inputRTs + inputFTs;

   if (inputTotal <= mTransferCapacity)
   {
      mPassedRealTargetNumber  = inputRTs;
      mPassedFalseTargetNumber = inputFTs;
   }
   else
   {
      mPassedRealTargetNumber  = (int)((mTransferCapacity * ((float)inputRTs / (float)inputTotal)));
      mPassedFalseTargetNumber = mTransferCapacity - mPassedRealTargetNumber;
   }
}

bool WsfFalseTargetScreener::NeedsUpdated(double aSimTime)
{
   double updateThreshold = 1.0E-5;
   if (!IsRequestBasedTracking())
   {
      updateThreshold = mSensorModePtr->GetFrameTime() - 1.0E-5;
   }
   return (mTargetsInvalid || (!mScreenerValid) || (fabs(aSimTime - mLastUpdate) >= updateThreshold));
}

void WsfFalseTargetScreener::Update(double aSimTime, WsfSensor::Settings& aSettings)
{
   if ((fabs(aSimTime - mLastUpdate) < 1.0E-5) && (!mTargetsInvalid) && mScreenerValid)
   {
      return;
   }

   CheckFT_Toggle(); // sets mScreenerValid if true

   // Update the false target jammer interactors, gather blips and update them as required.
   UpdateFalseTargetJammers(aSimTime); // updates the mFT_StatusChange flag based on blip creation/modification changes
   UpdateBlips(aSimTime,
               aSettings); // updates the blips (SNR and list location for undetected blips), sets the mBlipsInvalid flag

   mScreenerValid = true; // screener is valid after the blips have been collected and updated

   // Adjust and modify the target and blip numbers and location amongst the lists based on the
   // updated false target jammers and blips correlated with the latest target tracks collected.
   AdjustTargetsAndBlips(aSimTime); // sets the mBlipsAdjusted and mBlipsAdjusted flags and resets the
   // mBlipsInvalid and mTargetsInvalid flags.

   if (mBlipsAdjusted || mTargetsAdjusted)
   {
      if (mDebugEnabled)
      {
         PrintScreenerState(aSimTime);
      }
   }

   mReportingTrackIds.clear();

   // Set the latest screener update time
   mLastUpdate = aSimTime;
}

void WsfFalseTargetScreener::UpdateFalseTargetJammers(double aSimTime)
{
   mFT_JammerStatusChange = false; // must reset the flag before returning for any reason

   if ((fabs(aSimTime - mLastUpdate) <= 1.0E-5) && mScreenerValid)
   {
      return;
   }

   // iterate through all the false targets screening per rejection ratio
   for (auto& falseTargetsJammer : mFalseTargetsJammers)
   {
      WsfFalseTarget* ftPtr         = falseTargetsJammer.first;
      WsfEM_Xmtr*     jammerXmtrPtr = falseTargetsJammer.second;

      ftPtr->Update(aSimTime, jammerXmtrPtr);

      // add this false target's blips to the screener's aggregate vector of blips
      mFT_JammerStatusChange = CreateBlips(ftPtr, jammerXmtrPtr);
      mFT_JammerStatusChange |= (!mScreenerValid);
   }
}

void WsfFalseTargetScreener::Reset()
{
   mAllowedFalseTargetNumber = 0;
   mAllowedRealTargetNumber  = 0;
   mPassedFalseTargetNumber  = 0;
   mPassedRealTargetNumber   = 0;
   mMinimumBlipRange         = 0.0;
   mMaximumBlipRange         = 0.0;
   mBlipsAdjusted            = false;
   mBlipsInvalid             = true;
   mTargetsAdjusted          = false;
   mTargetsInvalid           = true;
   mScreenerValid            = false;
   mBlipsAllocation.clear();
   mBlipIdToForceDataMap.clear();
   mActiveTargetTrackIds.clear();
   mBlockedTargetTrackIds.clear();
   mForcedTargetTrackIds.clear();
   mNullTargetTrackIds.clear();
   mBlockedBlipIds.clear();
   mUndetectedBlipIds.clear();
   mTrackableBlipIds.clear();
   // mFalseTargetTrackIds.clear(); // this is managed by the tracker and should be cleared there.
   mReportingTrackIds.clear();
}

WsfSimulation* WsfFalseTargetScreener::GetSimulation() const
{
   return ((mSensorModePtr != nullptr) ? mSensorModePtr->GetSimulation() : nullptr);
}
