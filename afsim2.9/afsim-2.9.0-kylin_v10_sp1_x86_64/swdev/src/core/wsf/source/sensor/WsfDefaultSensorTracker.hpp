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

#ifndef WSFDEFAULTSENSORTRACKER_HPP
#define WSFDEFAULTSENSORTRACKER_HPP

#include "wsf_export.h"

#include <map>

class WsfFilter;
#include "WsfSensor.hpp"
#include "WsfSensorTracker.hpp"
#include "WsfTrackId.hpp"
#include "WsfUtil.hpp"

//! A 'perfect' sensor tracker.
//!
//! This class provides the implementation for the default sensor tracker.
//! It assumes perfect correlation (the track for a given platform is
//! a direct product of only detections for that platform).

class WSF_EXPORT WsfDefaultSensorTracker : public WsfSensorTracker
{
public:
   WsfDefaultSensorTracker(WsfScenario& aScenario);

   ~WsfDefaultSensorTracker() override;

   WsfSensorTracker* Clone() const override;

   unsigned int GetActiveTrackCount() const override;

   void GetActiveTrackList(WsfTrackList& aActiveTrackList) override;

   unsigned int GetMaximumTrackCount() const override;

   bool GetTargetState(double aSimTime, const WsfTrackId& aRequestId, TargetState& aTargetState) override;

   void GetRequestDataForTarget(size_t aObjectId, WsfTrackId& aRequestId, size_t& aModeIndex, WsfTrackId& aTrackId) const override;

   bool Initialize(double aSimTime, WsfSensor* aSensorPtr, WsfSensorScheduler* aSchedulerPtr) override;

   bool ProcessInput(UtInput& aInput) override;

   void SchedulerModeChange(double            aSimTime,
                            const WsfTrackId& aRequestId,
                            size_t            aObjectId,
                            WsfPlatform*      aTargetPtr,
                            WsfStringId       aNewModeName) override;

   void StopTracking(double aSimTime, const WsfTrackId& aRequestId) override;

   virtual void TargetBlocked(double aSimTime, const Settings& aSettings, const WsfTrackId& aRequestId, unsigned int aObjectId);

   bool TargetDeleted(double aSimTime, const Settings& aSettings, const WsfTrackId& aRequestId, size_t aObjectId) override;

   void TargetDetected(double            aSimTime,
                       const Settings&   aSettings,
                       const WsfTrackId& aRequestId,
                       size_t            aObjectId,
                       WsfPlatform*      aTargetPtr,
                       WsfSensorResult&  aResult) override;

   bool TargetSkipped(double aSimTime, const Settings& aSettings, const WsfTrackId& aRequestId, size_t aObjectId) override;

   void TargetUndetected(double            aSimTime,
                         const Settings&   aSettings,
                         const WsfTrackId& aRequestId,
                         size_t            aObjectId,
                         WsfPlatform*      aTargetPtr,
                         WsfSensorResult&  aResult) override;

   void TurnOff(double aSimTime) override;

   WsfScenario* GetScenario() const { return mScenarioPtr; }

   //! Used to store state data about the interaction between a sensor and a target
   class WSF_EXPORT State
   {
   public:
      State(const WsfTrackId& aRequestId, const WsfSensor::Result& aResult);
      ~State();

      //! The sensor scheduler request ID associated with the interaction
      WsfTrackId mRequestId;

      //! If non-zero, a pointer to the filter.
      WsfFilter* mFilterPtr{nullptr};

      //! If non-zero, a pointer to the track.
      WsfTrack* mTrackPtr{nullptr};

      //! The time when 'locked-on' in the current mode.
      //! This is typically the time when the M/N establish track criteria is met for the current mode.
      //! The value will be less than zero if not locked-on in the current mode.
      double mLockonTime{-1.0};

      static constexpr size_t cINVALID_TARGET_INDEX = ut::npos;

      //! The target platform index
      size_t mTargetIndex{cINVALID_TARGET_INDEX};

      //! The mode index.
      size_t mModeIndex{0};

      //! The transmitter number (if any).
      size_t mXmtrIndex{0};

      //! The receiver number.
      size_t mRcvrIndex{0};

      //! A bit stream that represents the success/failure of recent detection attempts.
      //! Each recent detection attempt is represented as a bit within this variable, with an 'on' bit
      //! representing a successful detection attempt and 'off' bit representing a failed detection
      //! attempt. Bit 2^0 is the most recent attempt.
      unsigned int mDetectionHistory{0};

      //! The remaining number of detection failures before a track is to be dropped.
      //! A value of -1 indicates it should be computed.
      int mFailuresUntilDrop{-1};

      //! True if a mode switch is in progress.
      bool mModeSwitchActive{false};

      //! True if state id for a false-target track.
      bool mFalseTargetTrack{false};
   };

protected:
   WsfDefaultSensorTracker(const WsfDefaultSensorTracker& aSrc);
   WsfDefaultSensorTracker& operator=(const WsfDefaultSensorTracker&) = delete;

   void CheckForAutoModeSwitch(double aSimTime, size_t aObjectId, WsfPlatform* aTargetPtr, size_t aNewModeIndex);

   virtual void DropTrack(double aSimTime, State* aStatePtr);

   bool SwitchMode(double aSimTime, State* aStatePtr, size_t aNewModeIndex);

   void ProcessSensorDetectionChanged(double aSimTime, const State& aState, unsigned int aStatus);

   WsfScenario* mScenarioPtr;

   //! The maximum number of tracks that can be maintained by the tracker.
   unsigned int mMaximumTrackCount;

   //! The current number of tracks that are being maintained by the tracker.
   unsigned int mActiveTrackCount;

   //! If non-zero, a pointer to the prototype filter.
   //! This objects Clone() method is call to create a filter object for each sensor-target interaction.
   WsfFilter* mFilterPtr;

   //! The pointers to the sensor modes, indexed by mode index.
   std::vector<WsfSensorMode*> mModeList;

   //! The state data for active sensor-target interactions.
   //! The key value is the object index of the target (which may be a platform index or any other
   //! index that is unique among all objects of that type.
   using StateList = std::map<size_t, State*>;
   StateList mStateList;

   //! An iterator for accessing state data.
   using StateListIter = StateList::iterator;

   //! 'true' if 'Track Drop' messages should be sent when the sensor is turned off.
   bool mSendTrackDropOnTurnOff;

   //! 'true' if the sensor is being turned off.
   bool mTurnOffInProgress;
};

#endif
