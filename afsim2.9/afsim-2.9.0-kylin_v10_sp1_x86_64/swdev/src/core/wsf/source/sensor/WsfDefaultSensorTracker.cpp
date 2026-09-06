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

#include "WsfDefaultSensorTracker.hpp"

#include <algorithm>
#include <cassert>
#include <climits>
#include <iomanip>

#include "UtException.hpp"
#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtMatrix.hpp"
#include "UtRandom.hpp"
#include "UtVec3.hpp"
#include "WsfAssociationMessage.hpp"
#include "WsfCovariance.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfFilter.hpp"
#include "WsfFilterTypes.hpp"
#include "WsfPlatform.hpp"
#include "WsfRandomVariable.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSensorModeList.hpp"
#include "WsfSensorObserver.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSensorScheduler.hpp"
#include "WsfSimulation.hpp"
#include "WsfStatusMessage.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackList.hpp"

// =================================================================================================
WsfDefaultSensorTracker::WsfDefaultSensorTracker(WsfScenario& aScenario)
   : WsfSensorTracker()
   , mScenarioPtr(&aScenario)
   , mMaximumTrackCount(std::numeric_limits<unsigned int>::max())
   , mActiveTrackCount(0)
   , mFilterPtr(nullptr)
   , mModeList()
   , mStateList()
   , mSendTrackDropOnTurnOff(false)
   , mTurnOffInProgress(false)
{
}

// =================================================================================================
WsfDefaultSensorTracker::WsfDefaultSensorTracker(const WsfDefaultSensorTracker& aSrc)
   : WsfSensorTracker(aSrc)
   , mScenarioPtr(aSrc.mScenarioPtr)
   , mMaximumTrackCount(aSrc.mMaximumTrackCount)
   , mActiveTrackCount(0)
   , mFilterPtr(nullptr)
   , mModeList()
   , mStateList()
   , mSendTrackDropOnTurnOff(aSrc.mSendTrackDropOnTurnOff)
   , mTurnOffInProgress(false)
{
   if (aSrc.mFilterPtr != nullptr)
   {
      mFilterPtr = aSrc.mFilterPtr->Clone();
   }
}

// =================================================================================================
// virtual
WsfDefaultSensorTracker::~WsfDefaultSensorTracker()
{
   delete mFilterPtr;
   for (auto& sli : mStateList)
   {
      delete sli.second;
   }
}

// =================================================================================================
// virtual
WsfSensorTracker* WsfDefaultSensorTracker::Clone() const
{
   return new WsfDefaultSensorTracker(*this);
}

// =================================================================================================
// virtual
unsigned int WsfDefaultSensorTracker::GetActiveTrackCount() const
{
   return mActiveTrackCount;
}

// =================================================================================================
// virtual
void WsfDefaultSensorTracker::GetActiveTrackList(WsfTrackList& aActiveTrackList)
{
   // Loop through the state list and find the active tracks
   for (auto& sli : mStateList)
   {
      State* statePtr = sli.second;
      if (statePtr->mTrackPtr != nullptr)
      {
         // Clone the track and return
         aActiveTrackList.AddTrack(ut::clone(statePtr->mTrackPtr));
      }
   }
}

// =================================================================================================
// virtual
unsigned int WsfDefaultSensorTracker::GetMaximumTrackCount() const
{
   return mMaximumTrackCount;
}

// =================================================================================================
// virtual
bool WsfDefaultSensorTracker::GetTargetState(double aSimTime, const WsfTrackId& aRequestId, TargetState& aTargetState)
{
   bool targetStateValid = false;
   aTargetState.mLocationWCS.Set(0.0);
   aTargetState.mVelocityWCS.Set(0.0);

   if (!aRequestId.IsNull())
   {
      for (auto& sli : mStateList)
      {
         State* statePtr = sli.second;
         if (statePtr->mRequestId == aRequestId)
         {
            // We return a target state if and only if there is a track. We don't use the filter data without
            // a track because it is probably very bad. The caller should simply use it's external track data
            // in that case.
            if (statePtr->mTrackPtr != nullptr)
            {
               if ((statePtr->mFilterPtr != nullptr) && (statePtr->mFilterPtr->IsStable()))
               {
                  UtMeasurementData estimate;
                  statePtr->mFilterPtr->GetKinematicEstimate(aSimTime, estimate);
                  aTargetState.mLocationWCS = estimate.GetLocationWCS();
                  targetStateValid          = true;
               }
               else
               {
                  targetStateValid =
                     statePtr->mTrackPtr->GetExtrapolatedLocationWCS(aSimTime, aTargetState.mLocationWCS.GetData());
                  if (targetStateValid && statePtr->mTrackPtr->VelocityValid())
                  {
                     statePtr->mTrackPtr->GetVelocityWCS(aTargetState.mVelocityWCS.GetData());
                  }
               }
               if (targetStateValid)
               {
                  aTargetState.mLockonTime = statePtr->mLockonTime;
               }
            }
            break;
         }
      }
   }
   return targetStateValid;
}

// =================================================================================================
// virtual
void WsfDefaultSensorTracker::GetRequestDataForTarget(size_t      aObjectId,
                                                      WsfTrackId& aRequestId,
                                                      size_t&     aModeIndex,
                                                      WsfTrackId& aTrackId) const
{
   auto sli = mStateList.find(aObjectId);
   if (sli != mStateList.end())
   {
      State* statePtr = sli->second;
      aRequestId      = statePtr->mRequestId;
      aModeIndex      = statePtr->mModeIndex;
      aTrackId.Null();
      if (statePtr->mTrackPtr != nullptr)
      {
         aTrackId = statePtr->mTrackPtr->GetTrackId();
      }
   }
}

// =================================================================================================
// virtual
bool WsfDefaultSensorTracker::Initialize(double aSimTime, WsfSensor* aSensorPtr, WsfSensorScheduler* aSchedulerPtr)
{
   if (aSensorPtr->GetModeList() == nullptr)
   {
      ut::log::error() << "A mode list is required for WsfDefaultSensorTracker.";
      assert(aSensorPtr->GetModeList() != nullptr);
      return false;
   }

   bool ok = WsfSensorTracker::Initialize(aSimTime, aSensorPtr, aSchedulerPtr);

   // Reduce future dynamic casting by extracting derived class mode pointers.
   aSensorPtr->GetModeList()->GetDerivedModeList(mModeList);
   return ok;
}

// =================================================================================================
// virtual
bool WsfDefaultSensorTracker::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   WsfFilter* filterPtr = nullptr;
   if (command == "maximum_track_count")
   {
      aInput.ReadValue(mMaximumTrackCount);
      aInput.ValueGreater(mMaximumTrackCount, 0U);
   }
   else if (command == "send_track_drop_on_turn_off")
   {
      aInput.ReadValue(mSendTrackDropOnTurnOff);
   }
   else if (WsfFilterTypes::Get(*GetScenario()).LoadInstance(aInput, filterPtr))
   {
      delete mFilterPtr;
      mFilterPtr = filterPtr;
   }
   else
   {
      myCommand = WsfSensorTracker::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
// virtual
void WsfDefaultSensorTracker::SchedulerModeChange(double            aSimTime,
                                                  const WsfTrackId& aRequestId,
                                                  size_t            aObjectId,
                                                  WsfPlatform*      aTargetPtr,
                                                  WsfStringId       aNewModeName)
{
   auto sli = mStateList.find(aObjectId);
   if (sli != mStateList.end())
   {
      State* statePtr     = sli->second;
      size_t newModeIndex = mSensorPtr->GetModeList()->GetModeByName(aNewModeName);
      if (newModeIndex < mSensorPtr->GetModeList()->GetModeCount())
      {
         SwitchMode(aSimTime, statePtr, newModeIndex);
      }
   }
}

// =================================================================================================
// virtual
void WsfDefaultSensorTracker::StopTracking(double aSimTime, const WsfTrackId& aRequestId)
{
   // TODO The current implementation simply drops all knowledge of the tracked object.
   // TODO This is probably the desired behavior for a pure tracking sensor, but for a
   // TODO sensor that can scan-while-track it will force the sensor to reacquire the
   // TODO target. For some systems this may be the desired behavior while for others it
   // TODO may not. An option will probably need to be added, but that will get done later.

   // Locate the state(s) with the requested Id. In theory there may be more than one physical object associated
   // with a given request, so we must construct a list of the targets and then process them at the end.
   // We can't process them inside the following loop because the iterator would get invalidated by 'erase'.
   std::vector<size_t> objectKeys;
   for (auto& sli : mStateList)
   {
      State* statePtr = sli.second;
      if (statePtr->mRequestId == aRequestId)
      {
         objectKeys.push_back(sli.first);
      }
   }

   // Now go back and process the applicable targets.
   for (size_t objectKey : objectKeys)
   {
      auto sli = mStateList.find(objectKey);
      if (sli != mStateList.end())
      {
         // NOTE - the order of operations is important to avoid problems with callbacks.
         State* statePtr = (*sli).second;
         mStateList.erase(sli);         // 1...
         DropTrack(aSimTime, statePtr); // 2...
         delete statePtr;               // 3...
      }
   }
}

// =================================================================================================
// virtual
void WsfDefaultSensorTracker::TargetBlocked(double            aSimTime,
                                            const Settings&   aSettings,
                                            const WsfTrackId& aRequestId,
                                            unsigned int      aObjectId)
{
   auto sli = mStateList.find((int)aObjectId);
   if (sli != mStateList.end())
   {
      State* statePtr = (*sli).second;
      if (statePtr->mTrackPtr != nullptr)
      {
         WsfSensorMode* modePtr = GetSensor()->GetModeEntry(statePtr->mModeIndex);
         DropTrackP(aSimTime, aSettings, aRequestId, aObjectId, modePtr, statePtr->mTrackPtr);
         DropTrack(aSimTime, statePtr);
         // StopTracking(aSimTime,tempState->mTrackPtr->GetTrackId()); // this is eating processor time
      }
   }
}

// =================================================================================================
// virtual
bool WsfDefaultSensorTracker::TargetDeleted(double            aSimTime,
                                            const Settings&   aSettings,
                                            const WsfTrackId& aRequestId,
                                            size_t            aObjectId)
{
   bool trackDeleted = true;

   // Attempt to locate the state data.  If there is state data and it indicates that
   // a track is being maintained, then we must not delete the track until it has
   // failed the M/N criteria.

   auto sli = mStateList.find(aObjectId);
   if (sli != mStateList.end())
   {
      State* statePtr = sli->second;
      assert(statePtr->mModeIndex < mSensorPtr->GetModeCount());

      // Unless suppressed, inform observers of change in detection status.
      if (((aSettings.mOptions & Settings::cSUPPRESS_DETECTION_CHANGE_MESSAGES) == 0) &&
          (statePtr->mDetectionHistory & 1)) // currently detected
      {
         ProcessSensorDetectionChanged(aSimTime, *statePtr, WsfSensorResult::cTARGET_DELETED);
      }

      statePtr->mDetectionHistory = (statePtr->mDetectionHistory << 1);

      if (statePtr->mTrackPtr != nullptr)
      {
         WsfSensorMode* modePtr = mModeList[statePtr->mModeIndex];

         bool dropTrack(true);
         // A caller can suppress the M/N maintenance check.
         if ((aSettings.mOptions & Settings::cSUPPRESS_TRACKING_CRITERIA) == 0)
         {
            int bitsOn = WsfUtil::CountBitsOn(statePtr->mDetectionHistory & modePtr->MaintainTrackMask());
            dropTrack  = (bitsOn < modePtr->HitsToMaintainTrack());
         }
         if (dropTrack)
         {
            // NOTE - The order of operations is important to avoid problems with callbacks.
            mStateList.erase(sli); // 1...
            DropTrackP(aSimTime, aSettings, aRequestId, aObjectId, modePtr, statePtr->mTrackPtr);
            DropTrack(aSimTime, statePtr); // 2...
            delete statePtr;               // 3...
         }
         else
         {
            trackDeleted = false;
            mSensorPtr->TrackCoasted(aSimTime, statePtr->mTrackPtr);
         }
      }
   }

   return trackDeleted;
}

// =================================================================================================
// virtual
void WsfDefaultSensorTracker::TargetDetected(double            aSimTime,
                                             const Settings&   aSettings,
                                             const WsfTrackId& aRequestId,
                                             size_t            aObjectId,
                                             WsfPlatform*      aTargetPtr,
                                             WsfSensorResult&  aResult)
{
   // Locate the state data for the requested object ID.  If no data exists then create
   // and initialize state data for the object.

   State* statePtr = nullptr;
   auto   sli      = mStateList.find(aObjectId);
   if (sli == mStateList.end())
   {
      // State data does not exist for this target.  Create and initialize new state data.
      statePtr = new State(aRequestId, aResult);
      mStateList.insert(std::make_pair(aObjectId, statePtr));
   }
   else
   {
      statePtr = sli->second;
   }

   // Check for a possible mode switch.
   // TODO - should this be for tracking requests only??? What about simple mode switches for things like
   // TODO - PRF changes???

   assert(aResult.mModeIndex < mSensorPtr->GetModeCount());
   if (statePtr->mModeIndex != aResult.mModeIndex)
   {
      SwitchMode(aSimTime, statePtr, aResult.mModeIndex);
   }

   statePtr->mModeIndex = aResult.mModeIndex;
   assert(statePtr->mModeIndex < mSensorPtr->GetModeCount());
   WsfSensorMode* modePtr = mModeList[statePtr->mModeIndex];

   // Allow a component to reject the detection.
   if (!AllowTrackingP(aSimTime, aSettings, aRequestId, aObjectId, statePtr->mTrackPtr, aResult))
   {
      sli = mStateList.find(aObjectId);
      DropTrackP(aSimTime, aSettings, aRequestId, aObjectId, modePtr, statePtr->mTrackPtr);
      DropTrack(aSimTime, statePtr);
      mStateList.erase(sli);
      delete statePtr;
      return;
   }

   // Update the detection history to indicate we have a detection.
   statePtr->mDetectionHistory = (statePtr->mDetectionHistory << 1) | 1;

   // Indicate the 'failures until drop' needs to be calculated.
   statePtr->mFailuresUntilDrop = -1;

   // Unless suppressed by the caller, call the observers if a change in detection status
   // (detected this time, but not last time).
   if (((statePtr->mDetectionHistory & 2) == 0) &&
       ((aSettings.mOptions & Settings::cSUPPRESS_DETECTION_CHANGE_MESSAGES) == 0))
   {
      WsfObserver::SensorDetectionChanged(GetSimulation())(aSimTime, modePtr->GetSensor(), aTargetPtr->GetIndex(), aResult);
   }

   // If a filter has been defined then pass the detection to the filter.
   if (mFilterPtr != nullptr)
   {
      // Allocate the filter object for this target if not yet allocated
      if (statePtr->mFilterPtr == nullptr)
      {
         statePtr->mFilterPtr = mFilterPtr->Clone();
         statePtr->mFilterPtr->Initialize(aSimTime, GetSimulation());
      }
      else if (statePtr->mTrackPtr == nullptr)
      {
         // Nothing is being tracked.  If this is the first hit in the track establishment
         // window then we need to reinitialize the filter.

         if ((statePtr->mDetectionHistory & modePtr->EstablishTrackMask()) == 1)
         {
            statePtr->mFilterPtr->Reset(aSimTime);
            statePtr->mDetectionHistory = 1; // Restart the detection history
         }
      }

      //! Update the filter and report the result in aResult.mMeasurement.
      WsfMeasurement dataIn(aResult.mMeasurement);
      statePtr->mFilterPtr->Update(aSimTime, dataIn, aResult.mMeasurement);
   }

   // Initialize the track if necessary.

   bool modeSwitchRequired       = false;
   int  hits                     = WsfUtil::CountBitsOn(statePtr->mDetectionHistory & modePtr->EstablishTrackMask());
   bool suppressTrackingCriteria = ((aSettings.mOptions & Settings::cSUPPRESS_TRACKING_CRITERIA) != 0);

   bool createdTrack = false;
   if (statePtr->mTrackPtr != nullptr)
   {
      // Track already exists. Determine if a mode switch is complete.
      bool modeSwitchComplete(false);
      if (statePtr->mModeSwitchActive)
      {
         // A mode switch is declared to be complete if the basic M/N criteria is met.
         // The mode switch can still be delayed by randomness later in the processing.
         modeSwitchComplete = true;
         if (!suppressTrackingCriteria)
         {
            modeSwitchComplete = (hits >= modePtr->HitsToEstablishTrack());
         }
      }
      if (modeSwitchComplete)
      {
         if (!suppressTrackingCriteria)
         {
            // If requested by user input, lockon can be randomly delayed.
            double establishTrackProbability = modePtr->EstablishTrackProbability();
            double randomDraw                = (establishTrackProbability < 1.0) ? GetRandom().Uniform<double>() : 0.0;
            modeSwitchComplete               = (randomDraw <= establishTrackProbability);
         }
         if (modeSwitchComplete)
         {
            statePtr->mModeSwitchActive = false;
            modeSwitchRequired          = (statePtr->mModeIndex != modePtr->GetOnSuccessModeIndex());
            statePtr->mLockonTime       = aSimTime;
         }
      }
      else
      {
         // The track has not yet met the tracking criteria for the new mode.
         // Determine if the track should randomly be dropped.
         bool shouldDropTrack(false);
         if (!suppressTrackingCriteria)
         {
            // If requested by user input, the track could still be randomly maintained.
            double maintainTrackProbability = modePtr->MaintainTrackProbability();
            double randomDraw               = (maintainTrackProbability < 1.0) ? GetRandom().Uniform<double>() : 0.0;
            shouldDropTrack                 = (randomDraw > maintainTrackProbability);
         }
         if (shouldDropTrack)
         {
            statePtr->mDetectionHistory  = 0;
            statePtr->mFailuresUntilDrop = -1;
            CheckForAutoModeSwitch(aSimTime, aObjectId, aTargetPtr, modePtr->GetOnFailureModeIndex());
            DropTrackP(aSimTime, aSettings, aRequestId, aObjectId, modePtr, statePtr->mTrackPtr);
            DropTrack(aSimTime, statePtr);
            return; // Do not continue as statePtr may no longer be valid
         }
      }
   }
   else
   {
      // Track does not exist. Determine if a track should be established.
      bool shouldEstablishTrack(true);
      if (!suppressTrackingCriteria)
      {
         shouldEstablishTrack = (hits >= modePtr->HitsToEstablishTrack());
         if (shouldEstablishTrack)
         {
            // If requested by user input, track establishment can be delayed.
            double establishTrackProbability = modePtr->EstablishTrackProbability();
            double randomDraw                = (establishTrackProbability < 1.0) ? GetRandom().Uniform<double>() : 0.0;
            shouldEstablishTrack             = (randomDraw <= establishTrackProbability);
         }
      }
      if (shouldEstablishTrack)
      {
         InitializeTrackP(aSimTime, aSettings, aRequestId, aObjectId, modePtr, statePtr->mTrackPtr);
         statePtr->mLockonTime = aSimTime;
         createdTrack          = true;
         // Indicate if this will be a filtered or unfiltered track.
         if (mFilterPtr == nullptr)
         {
            statePtr->mTrackPtr->SetTrackType(WsfTrack::cUNFILTERED_SENSOR);
         }
         else
         {
            statePtr->mTrackPtr->SetTrackType(WsfTrack::cFILTERED_SENSOR);
         }
         ++mActiveTrackCount;
         modeSwitchRequired = (statePtr->mModeIndex != modePtr->GetOnSuccessModeIndex());
      }
   }

   // If we have a track then update it.

   if (statePtr->mTrackPtr != nullptr)
   {
      // Update the track's state covariance matrix, if it's available
      // from the filter and the track reports it.
      if ((statePtr->mFilterPtr != nullptr) && (statePtr->mFilterPtr->GetStateCovariance() != nullptr))
      {
         statePtr->mTrackPtr->SetStateCovariance(aSimTime, *(statePtr->mFilterPtr->GetStateCovariance()));

         if (statePtr->mFilterPtr->GetResidualCovariance() != nullptr)
         {
            statePtr->mTrackPtr->SetResidualCovariance(*(statePtr->mFilterPtr->GetResidualCovariance()));
         }
      }

      // If associated with a tracking request, send the association message if either of the following is true:
      // -) This is the creation of the track.
      // -) The request ID has changed (some sort of track swap occurred).
      if ((!aRequestId.IsNull()) && (createdTrack || (statePtr->mRequestId != aRequestId)))
      {
         WsfAssociationMessage message(mSensorPtr);
         message.SetSubjectId(aRequestId);
         message.SetAssociatedId(statePtr->mTrackPtr->GetTrackId());
         mSensorPtr->SendMessage(aSimTime, message);
         statePtr->mRequestId = aRequestId;
      }

      // The track quality is not updated during a mode switch because if the new mode is of higher quality,
      // we don't want to report the higher quality until we're locked on.
      if (!statePtr->mModeSwitchActive)
      {
         statePtr->mTrackPtr->SetTrackQuality(modePtr->GetTrackQuality());
      }

      UpdateTrackP(aSimTime, aSettings, aRequestId, aObjectId, modePtr, statePtr->mTrackPtr, aTargetPtr, aResult);

      mSensorPtr->TrackUpdated(aSimTime, statePtr->mTrackPtr);

      // If a track was newly established or a mode switch was completed then check to see if an automatic
      // transition to another state has been defined.

      if (modeSwitchRequired)
      {
         CheckForAutoModeSwitch(aSimTime, aObjectId, aTargetPtr, modePtr->GetOnSuccessModeIndex());
      }
   }
}

// =================================================================================================
// virtual
bool WsfDefaultSensorTracker::TargetSkipped(double            aSimTime,
                                            const Settings&   aSettings,
                                            const WsfTrackId& aRequestId,
                                            size_t            aObjectId)
{
   // This is a lot like TargetDeleted...

   bool trackDeleted = true;

   // Attempt to locate the state data.  If there is state data and it indicates that
   // a track is being maintained, then we must not delete the track until it has
   // failed the M/N criteria.

   auto sli = mStateList.find(aObjectId);
   if (sli != mStateList.end())
   {
      State* statePtr = sli->second;
      assert(statePtr->mModeIndex < mSensorPtr->GetModeCount());
      WsfSensorMode* modePtr      = mModeList[statePtr->mModeIndex];
      statePtr->mDetectionHistory = (statePtr->mDetectionHistory << 1);

      if (statePtr->mTrackPtr != nullptr)
      {
         bool shouldDropTrack(true);
         if ((aSettings.mOptions & Settings::cSUPPRESS_TRACKING_CRITERIA) == 0)
         {
            int bitsOn      = WsfUtil::CountBitsOn(statePtr->mDetectionHistory & modePtr->MaintainTrackMask());
            shouldDropTrack = (bitsOn < modePtr->HitsToMaintainTrack());
         }
         if (shouldDropTrack)
         {
            // NOTE - The order of operations is important to avoid problems with callbacks.
            mStateList.erase(sli); // 1...
            DropTrackP(aSimTime, aSettings, aRequestId, aObjectId, modePtr, statePtr->mTrackPtr);
            DropTrack(aSimTime, statePtr); // 2...
            delete statePtr;               // 3...
         }
         else
         {
            trackDeleted = false;
         }
      }
      else
      {
         mStateList.erase(sli);
         delete statePtr;
      }
   }

   return trackDeleted;
}

// =================================================================================================
// virtual
void WsfDefaultSensorTracker::TargetUndetected(double            aSimTime,
                                               const Settings&   aSettings,
                                               const WsfTrackId& aRequestId,
                                               size_t            aObjectId,
                                               WsfPlatform*      aTargetPtr,
                                               WsfSensorResult&  aResult)
{
   auto sli = mStateList.find(aObjectId);
   if (sli == mStateList.end())
   {
      return;
   }
   State* statePtr = sli->second;

   // Check for a possible mode switch.
   // TODO - should this be for tracking requests only??? What about simple mode switches for things like
   // TODO - PRF changes???

   bool detectionChanged = false;

   assert(aResult.mModeIndex < mSensorPtr->GetModeCount());
   if (statePtr->mModeIndex != aResult.mModeIndex)
   {
      // If change in detection status (was detected last time, but not this time)
      // Need to capture before mode switch due to clearing of detection history
      if ((statePtr->mDetectionHistory & 1) != 0)
      {
         detectionChanged = true;
      }
      SwitchMode(aSimTime, statePtr, aResult.mModeIndex);
   }

   assert(statePtr->mModeIndex < mSensorPtr->GetModeCount());
   WsfSensorMode* modePtr = mModeList[statePtr->mModeIndex];

   // Initialize the 'failures until drop' if requested.
   if (statePtr->mFailuresUntilDrop < 0)
   {
      if (statePtr->mModeSwitchActive || (statePtr->mTrackPtr == nullptr))
      {
         statePtr->mFailuresUntilDrop = modePtr->EstablishTrackWindow() - modePtr->HitsToEstablishTrack();
      }
      else
      {
         statePtr->mFailuresUntilDrop = modePtr->MaintainTrackWindow() - modePtr->HitsToMaintainTrack();
      }
   }

   // Update the detection history to indicate that no detection occurred.
   statePtr->mDetectionHistory = (statePtr->mDetectionHistory << 1);

   // Decrement the number of failures until drop.
   --(statePtr->mFailuresUntilDrop);

   detectionChanged |= ((statePtr->mDetectionHistory & 2) != 0);

   // If change in detection status (was detected last time, but not this time)
   if (detectionChanged && ((aSettings.mOptions & Settings::cSUPPRESS_DETECTION_CHANGE_MESSAGES) == 0))
   {
      WsfObserver::SensorDetectionChanged(GetSimulation())(aSimTime, modePtr->GetSensor(), aTargetPtr->GetIndex(), aResult);
   }

   if (statePtr->mTrackPtr != nullptr)
   {
      // The object is currently being tracked.
      if (statePtr->mFailuresUntilDrop < 0)
      {
         // Not enough chances left in the window to maintain the track (or switch modes).
         // Drop the track, reset the history and check for a mode transition.

         // NOTE: Any manipulation of the state data referenced by 'statePtr' MUST be done PRIOR to calling
         // DropTrack() as dropping the track may result in a callback that may cause the state to be deleted!

         statePtr->mDetectionHistory  = 0;
         statePtr->mFailuresUntilDrop = -1;

         // Perform any automatic mode switch prior to dropping the track.
         // This ensures the scheduler will have the last known track for cuing.
         CheckForAutoModeSwitch(aSimTime, aObjectId, aTargetPtr, modePtr->GetOnFailureModeIndex());

         // TODO-This probably doesn't do during the final pseudo-non-detects after a platform has been broken.
         // TODO-(The physical detection attempt is totally bypassed because the platform no longer exists).
         DropTrackP(aSimTime, aSettings, aRequestId, aObjectId, modePtr, statePtr->mTrackPtr);
         DropTrack(aSimTime, statePtr);
      }
      else if (statePtr->mFilterPtr != nullptr)
      {
         // There is still a chance to establish or maintain the track and a filter exists.
         statePtr->mFilterPtr->NoDetectUpdate(aSimTime, aResult.mMeasurement);

         // Update the track's state/residual covariance matrix, if it's available from the filter.
         if ((statePtr->mFilterPtr != nullptr) && (statePtr->mFilterPtr->GetStateCovariance() != nullptr))
         {
            WsfCovariance stateCovariance;
            if (statePtr->mFilterPtr->GetPredictedStateCovariance(aSimTime, stateCovariance))
            {
               statePtr->mTrackPtr->SetStateCovariance(aSimTime, stateCovariance);
            }

            if (statePtr->mFilterPtr->GetResidualCovariance() != nullptr)
            {
               WsfCovariance residualCovariance;
               if (statePtr->mFilterPtr->GetPredictedResidualCovariance(aSimTime, residualCovariance))
               {
                  statePtr->mTrackPtr->SetResidualCovariance(residualCovariance);
               }
            }
         }

         // Send the updated track
         UpdateTrackP(aSimTime, aSettings, aRequestId, aObjectId, modePtr, statePtr->mTrackPtr, aTargetPtr, aResult);
         mSensorPtr->TrackUpdated(aSimTime, statePtr->mTrackPtr);
         mSensorPtr->TrackCoasted(aSimTime, statePtr->mTrackPtr);
      }
      else
      {
         mSensorPtr->TrackCoasted(aSimTime, statePtr->mTrackPtr);
      }
   }
   else
   {
      // The object is not being tracked.
      if ((statePtr->mDetectionHistory & modePtr->EstablishTrackMask()) != 0)
      {
         if (statePtr->mFailuresUntilDrop < 0)
         {
            // No chance to establish a track (or switch modes)
            statePtr->mDetectionHistory  = 0;
            statePtr->mFailuresUntilDrop = -1;
            // Pre-check for the mode switch to avoid unnecessarily re-fetching the state pointer in the called routine.
            if (statePtr->mModeIndex != modePtr->GetOnFailureModeIndex())
            {
               CheckForAutoModeSwitch(aSimTime, aObjectId, aTargetPtr, modePtr->GetOnFailureModeIndex());
            }
         }
         else if (statePtr->mFilterPtr != nullptr)
         {
            // Still a chance to establish a track (or switch modes), filter is updated
            statePtr->mFilterPtr->NoDetectUpdate(aSimTime, aResult.mMeasurement);
         }
      }
   }
}

// =================================================================================================
// virtual
void WsfDefaultSensorTracker::TurnOff(double aSimTime)
{
   // Drop all active tracks.

   // NOTE - A copy of the state data pointer list is made and then the original is cleared. This is
   // necessary to prevent problems associated with the callbacks that result from dropping tracks.
   StateList tempStateData(mStateList);
   mStateList.clear();

   mTurnOffInProgress = true;
   while (!tempStateData.empty())
   {
      auto   sli      = tempStateData.begin();
      State* statePtr = (*sli).second;     // 1...
      tempStateData.erase(sli);            // 2...
      if (statePtr->mDetectionHistory & 1) // currently detected
      {
         ProcessSensorDetectionChanged(aSimTime, *statePtr,
                                       WsfSensorResult::cDETECTION_STOP); // 3...
      }
      DropTrack(aSimTime, statePtr); // 4...
      delete statePtr;               // 5...
   }
   mTurnOffInProgress = false;
   mActiveTrackCount  = 0;
}

// =================================================================================================
// Start of methods that are not part of the public interface
// =================================================================================================

// =================================================================================================
//! Drop the track that may be attached with the specified state.
//! This routine does nothing if there is not a track that is attached to the state.
//! @param aSimTime  The current simulation time.
//! @param aStatePtr The state to be processed.
// private virtual
void WsfDefaultSensorTracker::DropTrack(double aSimTime, State* aStatePtr)
{
   // The order of operations is very important here because invoking WsfSensor::TrackDropped can cause all
   // sorts of callback processing. In particular, we have to make sure that State::mTrackPtr is cleared
   // BEFORE calling so that through the callbacks we don't end up back here again!

   if (aStatePtr->mTrackPtr != nullptr)
   {
      --mActiveTrackCount;
      WsfTrack* trackPtr   = aStatePtr->mTrackPtr; // 1... Get a local copy before clearing.
      aStatePtr->mTrackPtr = nullptr;              // 2... Clear the stored pointer.

      // Notification of a track drop can optionally be sent when a sensor is turned off. This is not the default
      // mode of operation because it may cause a track manager to drop a track simply as the result of
      // EMCON, when in fact it would reasonably be expected to retain some knowledge. The normal purging
      // mechanism should be used to delete obsolete information.
      if ((!mTurnOffInProgress) || mSendTrackDropOnTurnOff)
      {
         mSensorPtr->TrackDropped(aSimTime, trackPtr); // 3... Inform interested parties
      }
      delete trackPtr; // 4... And finally delete the track
   }
}

// =================================================================================================
//! Check for and invoke an automatic mode switch.
//! A user may define automatic mode switches that occur on the acquisition or loss of a track.
//! This method is called when those conditions occur and determine if a mode switch should occur.
// private
void WsfDefaultSensorTracker::CheckForAutoModeSwitch(double aSimTime, size_t aObjectId, WsfPlatform* aTargetPtr, size_t aNewModeIndex)
{
   // NOTE: If this was called as the result of dropping a track, the call to DropTrack may have resulted in
   // callbacks to which may themselves that end up canceling the tracking request and the eventual deleting
   // of the state object. Therefore we must make sure it still exists prior to actually performing any mode switch.
   auto sli = mStateList.find(aObjectId);
   if (sli != mStateList.end())
   {
      State* statePtr = sli->second;
      if (SwitchMode(aSimTime, statePtr, aNewModeIndex))
      {
         // Notify the scheduler of the mode switch.
         if (mSchedulerPtr != nullptr)
         {
            WsfStringId newModeName = mModeList[statePtr->mModeIndex]->GetNameId();
            mSchedulerPtr->TrackerModeChange(aSimTime, statePtr->mRequestId, aObjectId, aTargetPtr, statePtr->mTrackPtr, newModeName);
         }
      }
   }
}

// =================================================================================================
//! Switch to a new mode if the new mode is not the same as the current mode.
//! @param aSimTime      The current simulation time.
//! @param aStatePtr     The state whose mode is to be changed.
//! @param aNewModeIndex The mode index of the desired new mode.
//! @returns 'true' if the desired new mode was not the same as the current mode.
//! @note This routine does not inform the scheduler of the change.
// private
bool WsfDefaultSensorTracker::SwitchMode(double aSimTime, State* aStatePtr, size_t aNewModeIndex)
{
   bool modeChanged = false;
   if (aStatePtr->mModeIndex != aNewModeIndex)
   {
      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "Sensor tracker changing mode for track.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << mSensorPtr->GetPlatform()->GetName();
         out.AddNote() << "Sensor: " << mSensorPtr->GetName();
         out.AddNote() << "Track ID: " << aStatePtr->mRequestId;
         out.AddNote() << "Old Mode: " << mModeList[aStatePtr->mModeIndex]->GetName();
         out.AddNote() << "New Mode: " << mModeList[aNewModeIndex]->GetName();
      }
      modeChanged           = true;
      aStatePtr->mModeIndex = aNewModeIndex;

      // Indicate not locked-on in the current mode.
      aStatePtr->mLockonTime = -1.0;

      // Reset the detection history. The transition into the new state is not complete until it satisfies the
      // M/N establish criteria for the new state.
      aStatePtr->mDetectionHistory = 0;

      // Force the failed detection count to be recomputed.
      aStatePtr->mFailuresUntilDrop = -1;

      // Indicate that a mode switch is in progress.
      aStatePtr->mModeSwitchActive = true;
   }
   return modeChanged;
}

// =================================================================================================
//! Provide a minimal set of data in a SensorDetectionChanged event in the absence of
//! an active WsfSensor::Result.
//! @param aSimTime      The current simulation time.
//! @param aState        The State to be used to populate data provided in the event.
//! @param aStatus       The WsfSensorResult status flag (reason for detection change).
void WsfDefaultSensorTracker::ProcessSensorDetectionChanged(double aSimTime, const State& aState, unsigned int aStatus)
{
   // Set up current interaction geometry and call SensorDetectionChanged.
   WsfSensor::Result result;
   result.mCheckedStatus = aStatus;
   result.mFailedStatus  = aStatus;

   WsfEM_Xmtr* xmtrPtr = nullptr;
   WsfEM_Rcvr* rcvrPtr = nullptr;
   if (GetSensor()->GetEM_XmtrCount() > 0)
   {
      xmtrPtr = &(GetSensor()->GetEM_Xmtr(aState.mXmtrIndex));
   }
   if (GetSensor()->GetEM_RcvrCount() > 0)
   {
      rcvrPtr = &(GetSensor()->GetEM_Rcvr(aState.mXmtrIndex));
      result.BeginGenericInteraction(xmtrPtr, GetSimulation()->GetPlatformByIndex(aState.mTargetIndex), rcvrPtr);
   }
   else
   {
      throw UtException(
         "WsfDefaultSensorTracker::ProcessSensorDetectionChanged: No valid sensor receiver object found.");
   }
   result.mModeIndex = aState.mModeIndex;
   WsfObserver::SensorDetectionChanged(mSensorPtr->GetSimulation())(aSimTime, mSensorPtr, aState.mTargetIndex, result);
}

// =================================================================================================
// Nested class 'State'
// =================================================================================================

WsfDefaultSensorTracker::State::State(const WsfTrackId& aRequestId, const WsfSensor::Result& aResult)
   : mRequestId(aRequestId)
   , mModeIndex(aResult.mModeIndex)
   , mXmtrIndex(aResult.mXmtrIndex)
   , mRcvrIndex(aResult.mRcvrIndex)
{
   const WsfPlatform* targetPtr = aResult.GetTarget();
   if (targetPtr != nullptr)
   {
      mTargetIndex = targetPtr->GetIndex();
   }
}

WsfDefaultSensorTracker::State::~State()
{
   delete mFilterPtr;
   delete mTrackPtr;
}
