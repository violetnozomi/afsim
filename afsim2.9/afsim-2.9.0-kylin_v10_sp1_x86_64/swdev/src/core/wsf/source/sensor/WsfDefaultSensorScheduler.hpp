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

#ifndef WSFDEFAULTSENSORSCHEDULER_HPP
#define WSFDEFAULTSENSORSCHEDULER_HPP

#include "wsf_export.h"

#include <vector>

#include "WsfSensorScheduler.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackId.hpp"

//! The default sensor scheduler given to sensors.
//!
//! In search mode, this schedules detection chances for a sensor using a statistical scan.
//! Times of detection chances are random and independent of the target's location, but still
//! constrained by the 'frame_time'.
class WSF_EXPORT WsfDefaultSensorScheduler : public WsfSensorScheduler
{
public:
   WsfDefaultSensorScheduler();

   ~WsfDefaultSensorScheduler() override = default;

   static std::unique_ptr<WsfSensorScheduler> ObjectFactory(const std::string& aTypeName);

   WsfSensorScheduler* Clone() const override;

   void FrameTimeChanged(double aSimTime, WsfStringId aModeNameId) override;

   int GetActiveRequestCount(WsfStringId aModeNameId) const override;

   int GetMaximumRequestCount(WsfStringId aModeNameId) const override;

   bool HaveRequestFor(const WsfTrackId& aRequestId) const override;

   bool Initialize(double aSimTime, WsfSensor* aSensorPtr, WsfSensorTracker* aTrackerPtr) override;

   void ModeDeselected(double aSimTime, WsfStringId aModeNameId) override;

   void ModeSelected(double aSimTime, WsfStringId aModeNameId) override;

   void PlatformAdded(double aSimTime, WsfPlatform* aPlatformPtr) override;

   void PlatformDeleted(WsfPlatform* aPlatformPtr) override;

   bool ProcessInput(UtInput& aInput) override;

   void RemoveTarget(double aSimTime, size_t aTargetIndex) override;

   const WsfTrack& GetTrackForRequest(const WsfTrackId& aRequestId) const override;

   bool SelectTarget(double               aSimTime,
                     double&              aNextSimTime,
                     size_t&              aTargetIndex,
                     WsfTrackId&          aRequestId,
                     WsfSensor::Settings& aSettings) override;

   bool StartTracking(double aSimTime, const WsfTrack& aTrack, WsfStringId aModeNameId) override;

   bool StopTracking(double aSimTime, const WsfTrackId& aRequestId) override;

   void TrackerModeChange(double            aSimTime,
                          const WsfTrackId& aRequestId,
                          size_t            aTargetIndex,
                          WsfPlatform*      aTargetPtr,
                          const WsfTrack*   aTrackPtr,
                          WsfStringId       aModeNameId) override;

   void TurnOff(double aSimTime) override;

   void TurnOn(double aSimTime) override;

   //! A class that represents an external cue request.
   //! This must be public (even though the actual list is private) because
   //! it must be accessible to a functor.
   class Request
   {
   public:
      Request() = default;
      Request(double aSimTime, const WsfTrack& aTrack, size_t aModeIndex)
         : mRequestId(aTrack.GetTrackId())
         , mTrack(aTrack)
         , mStartTime(aSimTime)
         , mNextVisitTime(aSimTime)
         , mLastVisitTime(-1.0)
         , mTargetIndex(0)
         , mModeIndex(aModeIndex)
      {
      }

      //! The track ID from the original request.
      WsfTrackId mRequestId = WsfTrackId();

      //! The track to be used for the setting the sensor position.
      //! This starts out with the requested track and then gets updated as the sensor establishes
      //! its own track.
      WsfTrack mTrack = WsfTrack();

      //! The time when the request was initiated.
      double mStartTime = -1.0;

      //! The simulation time of the next visit (sensing chance)
      double mNextVisitTime = -1.0;

      //! The simulation time of the last visit (sensing chance)
      double mLastVisitTime = -1.0;

      //! The target index to be used for this track request.
      size_t mTargetIndex = 0;

      //! The index of the sensor mode to be used for this request.
      size_t mModeIndex = 0;
   };
   using RequestList      = std::vector<Request>;
   using RequestListIndex = RequestList::size_type;

   const RequestList& GetRequestList() const { return mRequestList; }

protected:
   enum ScanSchedulingMethod
   {
      cSSM_RANDOM,
      cSSM_INPUT_ORDER,
      cSSM_REVERSE_INPUT_ORDER
   };

   WsfDefaultSensorScheduler(const WsfDefaultSensorScheduler& aSrc);
   WsfDefaultSensorScheduler& operator=(const WsfDefaultSensorScheduler&) = delete;

   void CheckSearchModeAvailability();

   void DeselectMode(double aSimTime, Request& aRequest);

   void SelectMode(double aSimTime, Request& aRequest);

   void ResetTrackList();

   void ResetSearchList();

   size_t SelectTargetForRequest(Request& aRequest);

   bool TargetHasActiveRequest(size_t aTargetIndex) const;

   void UpdateNextTrackVisitTime();

   void UpdateSearchChanceInterval();

   void UpdateSearchFrameTime(double aSimTime);


   //! A Functor for locating a track request.
   class CompareRequestId
   {
   public:
      CompareRequestId(const WsfTrackId& aRequestId)
         : mRequestId(aRequestId)
      {
      }

      bool operator()(const Request& aRequest) const { return (aRequest.mRequestId == mRequestId); }

   private:
      WsfTrackId mRequestId;
   };

   //! The pointers to the sensor modes, indexed by mode index.
   std::vector<WsfSensorMode*> mModeList;

   double mDwellEndTime;
   double mNextSearchVisitTime;
   double mNextTrackVisitTime;
   double mSearchChanceInterval;
   double mSearchFrameTime;

   //! The vector index of the next search chance to be performed.
   SearchListIndex mSearchIndex;

   //! A list of active track requests
   RequestList mRequestList;

   //! The index of the next request to be performed.
   RequestListIndex mRequestIndex;

   //! The last mode explicitly selected (i.e.: via WsfSensor::SelectMode).
   size_t mLastExplicitModeIndex;

   //! The mode index to be used for searching.
   //! If greater than the mode count then searching is disabled.
   size_t mSearchModeIndex;

   //! 'true' if search chances can be processed.
   bool mSearchAllowed;

   //! 'true' if SelectTarget should check the search list for tracks or targets that need to be delete.
   //! This is done only when a search mode is not active.
   bool mCheckSearchList;

   //! How scan chances are added to the search list
   ScanSchedulingMethod mScanSchedulingMethod;
};

#endif
