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

#include "WsfRF_Repeater.hpp"

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "WsfAntennaPattern.hpp"
#include "WsfEM_Interaction.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfESM_Sensor.hpp"
#include "WsfEW_Effect.hpp"
#include "WsfEW_Result.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfPlatform.hpp"
#include "WsfSensorResult.hpp"
#include "WsfStringId.hpp"
#include "WsfTaskManager.hpp"
#include "WsfTrackList.hpp"
#include "WsfTrackManager.hpp"
#include "WsfWeaponTaskResource.hpp"

WsfRF_Repeater::WsfRF_Repeater()
   : mWeaponPtr(nullptr)
   , mOperatingMode(cOM_MANUAL)
   , mSignalFollowing(false)
   , mDebug(false)
   , mTrackIdToSignalMap()
   , mTargetIndexToSensorNameId()
   , mMergedTrackIdMap()
   , mReverseMergedTrackIdMap()
{
}

WsfRF_Repeater::WsfRF_Repeater(const WsfRF_Repeater& aSrc)
   : mWeaponPtr(nullptr)
   , mOperatingMode(aSrc.mOperatingMode)
   , mSignalFollowing(aSrc.mSignalFollowing)
   , mDebug(aSrc.mDebug)
   , mTrackIdToSignalMap()
   , // do not copy registered tracks
   mTargetIndexToSensorNameId()
   , // do not copy registered targets
   mMergedTrackIdMap()
   ,                          // do not copy merged tracks
   mReverseMergedTrackIdMap() // do not copy merged tracks
{
}

// virtual
WsfRF_Repeater* WsfRF_Repeater::Clone() const
{
   return new WsfRF_Repeater(*this);
}

// virtual
bool WsfRF_Repeater::Initialize(double aSimTime, WsfWeapon* aWeaponPtr)
{
   mWeaponPtr = aWeaponPtr;

   if (mWeaponPtr != nullptr)
   {
      Reset(aSimTime);
      mDebug |= mWeaponPtr->DebugEnabled();
   }

   return (mWeaponPtr != nullptr);
}

// virtual
bool WsfRF_Repeater::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if ((command == "frequency_following") || (command == "signal_following"))
   {
      aInput.ReadValue(mSignalFollowing);
   }
   else if (command == "operating_mode")
   {
      std::string operatingMode;
      aInput.ReadValue(operatingMode);
      if (operatingMode == "manual")
      {
         mOperatingMode = cOM_MANUAL;
      }
      else if ((operatingMode == "semi_auto") || (operatingMode == "semi-auto"))
      {
         mOperatingMode = cOM_SEMIAUTO;
      }
      else if (operatingMode == "auto")
      {
         mOperatingMode = cOM_AUTO;
      }
      else
      {
         throw UtInput::BadValue(aInput, "Unrecognized 'operating_mode input' " + operatingMode);
      }
   }
   else if (command == "debug_repeater")
   {
      mDebug = true;
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

// virtual
bool WsfRF_Repeater::ProcessInputBlock(UtInput& aInput)
{
   bool myCommand = false;
   if (aInput.GetCommand() == "repeater")
   {
      myCommand = true;
      UtInputBlock inputBlock(aInput);
      std::string  command;
      while (inputBlock.ReadCommand(command))
      {
         if (command == "debug")
         {
            mDebug = true;
         }
         else if (!ProcessInput(aInput))
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   return myCommand;
}

void WsfRF_Repeater::Reset(double aSimTime)
{
   mTrackIdToSignalMap.clear();
   mTargetIndexToSensorNameId.clear();
   mMergedTrackIdMap.clear();
   mReverseMergedTrackIdMap.clear();
}

void WsfRF_Repeater::RegisterTrack(const WsfTrack* aTrackPtr)
{
   if (!aTrackPtr->GetTrackId().IsNull())
   {
      AddUpdateSignalList(aTrackPtr);
   }

   if (aTrackPtr->GetTargetIndex() > 0)
   {
      mTargetIndexToSensorNameId[aTrackPtr->GetTargetIndex()] = aTrackPtr->GetSensorNameId();
   }
}

void WsfRF_Repeater::RegisterTrackId(WsfTrackId aTrackId, size_t aTargetIndex)
{
   if (mSignalFollowing)
   {
      if ((!aTrackId.IsNull()) && (mTrackIdToSignalMap.find(aTrackId) == mTrackIdToSignalMap.end()))
      {
         std::vector<WsfTrack::Signal> signalList;
         signalList.clear();
         mTrackIdToSignalMap[aTrackId] = signalList;
      }

      if ((aTargetIndex > 0) && (mTargetIndexToSensorNameId.find(aTargetIndex) == mTargetIndexToSensorNameId.end()))
      {
         mTargetIndexToSensorNameId[aTargetIndex] = nullptr;
      }
   }
}

void WsfRF_Repeater::RemoveTrackId(WsfTrackId aTrackId, size_t aTargetIndex)
{
   if (!aTrackId.IsNull())
   {
      mTrackIdToSignalMap.erase(aTrackId);

      if (mMergedTrackIdMap.find(aTrackId) != mMergedTrackIdMap.end())
      {
         if (mTrackIdToSignalMap.find(mMergedTrackIdMap[aTrackId]) != mTrackIdToSignalMap.end())
         {
            mTrackIdToSignalMap.erase(mMergedTrackIdMap[aTrackId]);
         }

         if (mReverseMergedTrackIdMap.find(mMergedTrackIdMap[aTrackId]) != mReverseMergedTrackIdMap.end())
         {
            mReverseMergedTrackIdMap.erase(mMergedTrackIdMap[aTrackId]);
         }
         mMergedTrackIdMap.erase(aTrackId);
      }
   }

   if ((aTargetIndex > 0) && (mTargetIndexToSensorNameId.find(aTargetIndex) != mTargetIndexToSensorNameId.end()))
   {
      mTargetIndexToSensorNameId.erase(aTargetIndex);
   }
}

WsfTrackId WsfRF_Repeater::MergedTrackId(WsfTrackId aProcessedTrackId)
{
   WsfTrackId rawTrackId;
   if (!aProcessedTrackId.IsNull())
   {
      if (mMergedTrackIdMap.find(aProcessedTrackId) != mMergedTrackIdMap.end())
      {
         rawTrackId = mMergedTrackIdMap[aProcessedTrackId];
      }
   }
   return rawTrackId;
}

bool WsfRF_Repeater::CheckDetectAndUpdate(double                   aSimTime,
                                          WsfEM_Xmtr*              aXmtrPtr,
                                          WsfSensorResult&         aResult,
                                          const WsfEM_Interaction& aTargetInteraction)
{
   // default is to return true if an ESM/RWR was not specified.
   bool detected = true;

   // Get the victim transmitter
   WsfEM_Xmtr* victimXmtrPtr = aTargetInteraction.GetTransmitter();

   WsfESM_Sensor* esmSensorPtr = nullptr;
   size_t         targetIndex  = victimXmtrPtr->GetPlatform()->GetIndex();
   if (mTargetIndexToSensorNameId.find(targetIndex) != mTargetIndexToSensorNameId.end())
   {
      esmSensorPtr = dynamic_cast<WsfESM_Sensor*>(
         mWeaponPtr->GetPlatform()->GetComponent<WsfSensor>(mTargetIndexToSensorNameId[targetIndex]));
   }

   if (esmSensorPtr != nullptr)
   {
      // Need to check if the sensor is on
      if (esmSensorPtr->IsTurnedOn())
      {
         WsfESM_Sensor::ESM_Mode* ESM_ModePtr = static_cast<WsfESM_Sensor::ESM_Mode*>(esmSensorPtr->GetCurrentMode());
         detected =
            ESM_ModePtr->CanDetect(aSimTime, victimXmtrPtr, aResult, const_cast<WsfEM_Interaction&>(aTargetInteraction));

         WsfEW_Result* ewResult                               = WsfEW_Result::Find(aResult);
         ewResult->mEW_Effects.mCurrentRepeaterData.mGain     = aResult.mXmtrBeam.mGain;
         ewResult->mEW_Effects.mCurrentRepeaterData.mMinGain  = victimXmtrPtr->GetMinimumAntennaGain();
         ewResult->mEW_Effects.mCurrentRepeaterData.mPeakGain = victimXmtrPtr->GetPeakAntennaGain();
         ewResult->mEW_Effects.mMask |= WsfEW_Effect::cEB_REPEATER_UPDATE;
      }

      if (mDebug)
      {
         auto out = ut::log::debug() << "Repeater updated detection interaction.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Jammer: " << mWeaponPtr->GetName();
         out.AddNote() << "Platform(victim): " << victimXmtrPtr->GetPlatform()->GetName();
         out.AddNote() << "Result: " << detected;
      }
   }

   return detected;
}

void WsfRF_Repeater::SensorTrackDropped(double aSimTime, WsfTrackId aTrackId, size_t aTargetIndex)
{
   if (mOperatingMode == cOM_MANUAL)
   {
      return;
   }

   bool trackDropProcessed(false);

   if (mOperatingMode == cOM_SEMIAUTO)
   {
      for (WsfComponentList::RoleIterator<WsfProcessor> iter(*mWeaponPtr->GetPlatform()); !iter.AtEnd(); ++iter)
      {
         WsfTaskManager* taskManagerPtr = dynamic_cast<WsfTaskManager*>(*iter);
         if (taskManagerPtr != nullptr)
         {
            WsfJammerTaskResource jammerResource;
            jammerResource.mNameId = mWeaponPtr->GetNameId();
            if (taskManagerPtr->TasksAssignedFor(aTrackId, nullptr, mWeaponPtr->GetNameId()) != 0)
            {
               taskManagerPtr->CancelTask(aSimTime, aTrackId, nullptr, jammerResource, 0);
               trackDropProcessed = true;

               if (mDebug)
               {
                  auto out = ut::log::debug() << "Repeater issued a cancel jamming task for raw track.";
                  out.AddNote() << "T = " << aSimTime;
                  out.AddNote() << "Jammer: " << mWeaponPtr->GetName();
                  out.AddNote() << "Track Id: " << aTrackId;
               }
            }

            if (mReverseMergedTrackIdMap.find(aTrackId) != mReverseMergedTrackIdMap.end())
            {
               if (taskManagerPtr->TasksAssignedFor(mReverseMergedTrackIdMap[aTrackId], nullptr, mWeaponPtr->GetNameId()) !=
                   0)
               {
                  taskManagerPtr->CancelTask(aSimTime, mReverseMergedTrackIdMap[aTrackId], nullptr, jammerResource, 0);
                  trackDropProcessed = true;

                  if (mDebug)
                  {
                     auto out = ut::log::debug() << "Repeater issued a cancel jamming task for local track.";
                     out.AddNote() << "T = " << aSimTime;
                     out.AddNote() << "Jammer: " << mWeaponPtr->GetName();
                     out.AddNote() << "Track Id: " << mReverseMergedTrackIdMap[aTrackId];
                     out.AddNote() << "AKA Raw Track Id: " << aTrackId;
                  }
               }
            }
         }
      }
   }
   else // if (mOperatingMode == cOM_AUTO)
   {
      if (mWeaponPtr->StopJamming(aSimTime, 0.0, 0.0, 0, aTrackId))
      {
         if (mDebug)
         {
            auto out = ut::log::debug() << "Repeater stopped jamming track.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Jammer: " << mWeaponPtr->GetName();
            out.AddNote() << "Track Id: " << aTrackId;
         }
         trackDropProcessed = true;
      }
   }

   if (trackDropProcessed)
   {
      if (mDebug)
      {
         auto out = ut::log::debug() << "Repeater dropped track.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Jammer: " << mWeaponPtr->GetName();
         out.AddNote() << "Track Id: " << aTrackId;
      }
      RemoveTrackId(aTrackId, aTargetIndex);
   }
}

void WsfRF_Repeater::SensorTrackInitiated(double aSimTime, const WsfTrack* aTrackPtr)
{
   if (mOperatingMode != cOM_AUTO)
   {
      return;
   }

   if (mWeaponPtr->StartJammingTrack(aSimTime, nullptr, aTrackPtr))
   {
      if (mDebug)
      {
         auto out = ut::log::debug() << "Repeater started jamming track.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Jammer: " << mWeaponPtr->GetName();
         out.AddNote() << "Track Id: " << aTrackPtr->GetTrackId();
         out.AddNote() << "Target: " << aTrackPtr->GetTargetName();
      }
      RegisterTrack(aTrackPtr);
   }
}

void WsfRF_Repeater::SensorTrackUpdated(double aSimTime, const WsfTrack* aTrackPtr)
{
   if (!mSignalFollowing)
   {
      return;
   }

   if ((mTrackIdToSignalMap.find(aTrackPtr->GetTrackId()) == mTrackIdToSignalMap.end())/* &&
       (mOperatingMode != cOM_AUTO)*/)
   {
      if ((mTargetIndexToSensorNameId.find(aTrackPtr->GetTargetIndex()) != mTargetIndexToSensorNameId.end()))
      {
         if (mDebug)
         {
            auto out = ut::log::debug() << "Repeater trying to merge Raw Track.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Jammer: " << mWeaponPtr->GetName();
            out.AddNote() << "Track Id: " << aTrackPtr->GetTrackId();
            out.AddNote() << "Target: " << aTrackPtr->GetTargetName();
         }

         if (!MergeTrack(aTrackPtr))
         {
            if (mOperatingMode != cOM_AUTO)
            {
               if (mDebug)
               {
                  auto out = ut::log::debug() << "Repeater track merge failed for Raw Track.";
                  out.AddNote() << "T = " << aSimTime;
                  out.AddNote() << "Jammer: " << mWeaponPtr->GetName();
                  out.AddNote() << "Track Id: " << aTrackPtr->GetTrackId();
                  out.AddNote() << "Target: " << aTrackPtr->GetTargetName();
               }
               auto out = ut::log::warning() << "FAILED Raw Track merge.";
               out.AddNote() << "Will result in incorrect track drop while operating the repeater in this mode.";
            }
         }
      }
   }

   WsfTrackId processedTrackId = aTrackPtr->GetTrackId();
   if (mReverseMergedTrackIdMap.find(processedTrackId) != mReverseMergedTrackIdMap.end())
   {
      processedTrackId = mReverseMergedTrackIdMap[aTrackPtr->GetTrackId()];
   }

   if (mTrackIdToSignalMap.find(aTrackPtr->GetTrackId()) != mTrackIdToSignalMap.end())
   {
      if (aTrackPtr->GetSignalCount() != mTrackIdToSignalMap[aTrackPtr->GetTrackId()].size())
      {
         // Stop the current assignment (note it may be from the processed and not the raw track initially)
         if (mWeaponPtr->StopJamming(aSimTime, 0.0, 0.0, 0, processedTrackId) ||
             mWeaponPtr->StopJamming(aSimTime, 0.0, 0.0, 0, aTrackPtr->GetTrackId()))
         {
            if (mWeaponPtr->StartJammingTrack(aSimTime, nullptr, aTrackPtr))
            {
               UpdateTrack(aTrackPtr);

               if (mDebug)
               {
                  auto out = ut::log::debug() << "Repeater updating track for target due to track signal count "
                                                 "changing, jamming assignments updated.";
                  out.AddNote() << "T = " << aSimTime;
                  out.AddNote() << "Jammer: " << mWeaponPtr->GetName();
                  out.AddNote() << "TrackId: " << aTrackPtr->GetTrackId();
                  out.AddNote() << "Target: " << aTrackPtr->GetTargetName();
               }
            }
         }
      }
      else // check each signal
      {
         unsigned int     trackSignalCount = aTrackPtr->GetSignalCount();
         WsfTrack::Signal signal;
         for (unsigned int i = 0; i < trackSignalCount; ++i)
         {
            aTrackPtr->GetSignalEntry(i, signal);
            if (!(signal == mTrackIdToSignalMap[aTrackPtr->GetTrackId()].at(i)))
            {
               // Stop the current assignment (note it may be from the processed and not the raw track initially)
               if (mWeaponPtr->StopJamming(aSimTime, 0.0, 0.0, 0, processedTrackId) ||
                   mWeaponPtr->StopJamming(aSimTime, 0.0, 0.0, 0, aTrackPtr->GetTrackId()))
               {
                  if (mWeaponPtr->StartJammingTrack(aSimTime, nullptr, aTrackPtr))
                  {
                     UpdateTrack(aTrackPtr);

                     if (mDebug)
                     {
                        auto out = ut::log::debug() << "Repeater updating track for target due to track signal "
                                                       "parameters changing, jamming assignments updated.";
                        out.AddNote() << "T = " << aSimTime;
                        out.AddNote() << "Jammer: " << mWeaponPtr->GetName();
                        out.AddNote() << "TrackId: " << aTrackPtr->GetTrackId();
                        out.AddNote() << "Target: " << aTrackPtr->GetTargetName();
                     }
                     break;
                  }
               }
            }
         }
      }
   }
   else if (mOperatingMode == cOM_AUTO)
   {
      SensorTrackInitiated(aSimTime, aTrackPtr);
   }
}

void WsfRF_Repeater::AddUpdateSignalList(const WsfTrack* aTrackPtr)
{
   mTrackIdToSignalMap[aTrackPtr->GetTrackId()].clear();

   WsfTrack::Signal signal;
   unsigned int     signalCount = aTrackPtr->GetSignalCount();
   for (unsigned int i = 0; i < signalCount; ++i)
   {
      aTrackPtr->GetSignalEntry(i, signal);
      mTrackIdToSignalMap[aTrackPtr->GetTrackId()].push_back(signal);
   }
}

bool WsfRF_Repeater::MergeTrack(const WsfTrack* aRawTrackPtr)
{
   if ((!mSignalFollowing) /* || (mOperatingMode == cOM_AUTO)*/)
   {
      return false;
   }

   const WsfTrackId& rawTrackId = aRawTrackPtr->GetTrackId();
   if (rawTrackId.IsNull())
   {
      return false;
   }

   WsfLocalTrack* localTrackPtr =
      mWeaponPtr->GetPlatform()->GetTrackManager().FindCorrelatedTrack(aRawTrackPtr->GetTrackId());
   if (localTrackPtr != nullptr)
   {
      const WsfTrackId& localTrackId = localTrackPtr->GetTrackId();
      if (mTrackIdToSignalMap.find(localTrackId) != mTrackIdToSignalMap.end())
      {
         mMergedTrackIdMap[localTrackId]      = rawTrackId;
         mReverseMergedTrackIdMap[rawTrackId] = localTrackId;
         mTrackIdToSignalMap[rawTrackId]      = mTrackIdToSignalMap[localTrackId];
         mTrackIdToSignalMap.erase(localTrackId);

         if (mDebug)
         {
            auto out = ut::log::debug() << "Track merge success from local track to raw track.";
            out.AddNote() << "Local Track Id: " << localTrackId;
            out.AddNote() << "Raw Track Id: " << rawTrackId;
            out.AddNote() << "Target: " << aRawTrackPtr->GetTargetName();
         }

         return true;
      }
      else if (mDebug)
      {
         auto out = ut::log::debug() << "Track merge failed from local track to raw track due to not finding the local "
                                        "track registration in the jammer repeater.";
         out.AddNote() << "Local Track Id: " << localTrackId;
         out.AddNote() << "Raw Track Id: " << rawTrackId;
         out.AddNote() << "Target: " << aRawTrackPtr->GetTargetName();
      }
   }
   else if (mDebug)
   {
      auto out =
         ut::log::debug() << "Track merge failed for raw track due to not finding a correlated track in track manager.";
      out.AddNote() << "Track Id: " << aRawTrackPtr->GetTrackId();
      out.AddNote() << "Target: " << aRawTrackPtr->GetTargetName();
   }
   return false;
}

void WsfRF_Repeater::UpdateTrack(const WsfTrack* aTrackPtr)
{
   if ((!aTrackPtr->GetTrackId().IsNull()) &&
       (mTrackIdToSignalMap.find(aTrackPtr->GetTrackId()) != mTrackIdToSignalMap.end()))
   {
      AddUpdateSignalList(aTrackPtr);
   }

   if ((aTrackPtr->GetTargetIndex() > 0) &&
       (mTargetIndexToSensorNameId.find(aTrackPtr->GetTargetIndex()) != mTargetIndexToSensorNameId.end()))
   {
      mTargetIndexToSensorNameId[aTrackPtr->GetTargetIndex()] = aTrackPtr->GetSensorNameId();
   }
}
