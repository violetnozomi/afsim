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

#ifndef WSFSENSORSCHEDULER_HPP
#define WSFSENSORSCHEDULER_HPP

#include "wsf_export.h"

class UtInput;
class WsfPlatform;
#include "WsfSensor.hpp"
class WsfSensorTracker;
#include "WsfTrackId.hpp"

//! Schedules detection chances for sensors.
//!
//! The main task of the sensor scheduler is to schedule detection chances for a sensor.
//! WsfSensorScheduler is a virtual interface for sensor scheduler implementations.
//! See WsfDefaultSensorScheduler for the default scheduler implementation.
class WSF_EXPORT WsfSensorScheduler
{
public:
   WsfSensorScheduler();
   virtual ~WsfSensorScheduler() = default;

   virtual WsfSensorScheduler* Clone() const;

   //! Returns 'true' if debugging is enabled for the scheduler.
   bool DebugEnabled() const { return mDebugEnabled; }
   void SetDebugEnabled(bool aDebug) { mDebugEnabled = aDebug; }

   virtual void FrameTimeChanged(double aSimTime, WsfStringId aModeNameId);

   virtual int GetActiveRequestCount(WsfStringId aModeNameId) const;

   virtual int GetMaximumRequestCount(WsfStringId aModeNameId) const;

   virtual bool HaveRequestFor(const WsfTrackId& aRequestId) const;

   virtual bool Initialize(double aSimTime, WsfSensor* aSensorPtr, WsfSensorTracker* aTrackerPtr);

   virtual void ModeDeselected(double aSimTime, WsfStringId aModeNameId);

   virtual void ModeSelected(double aSimTime, WsfStringId aModeNameId);

   virtual void PlatformAdded(double aSimTime, WsfPlatform* aPlatformPtr);

   virtual void PlatformDeleted(WsfPlatform* aPlatformPtr);

   virtual bool ProcessInput(UtInput& aInput);

   virtual void RemoveTarget(double aSimTime, size_t aTargetIndex);

   virtual const WsfTrack& GetTrackForRequest(const WsfTrackId& aRequestId) const;

   virtual bool SelectTarget(double               aSimTime,
                             double&              aNextSimTime,
                             size_t&              aTargetIndex,
                             WsfTrackId&          aRequestId,
                             WsfSensor::Settings& aSettings);

   virtual bool StartTracking(double aSimTime, const WsfTrack& aTrack, WsfStringId aModeNameId);

   virtual bool StopTracking(double aSimTime, const WsfTrackId& aTrackId);

   virtual void TrackerModeChange(double            aSimTime,
                                  const WsfTrackId& aRequestId,
                                  size_t            aTargetIndex,
                                  WsfPlatform*      aTargetPtr,
                                  const WsfTrack*   aTrackPtr,
                                  WsfStringId       aModeNameId);

   virtual void TurnOff(double aSimTime);

   virtual void TurnOn(double aSimTime);

   WsfSimulation* GetSimulation() const;

protected:
   WsfSensorScheduler(const WsfSensorScheduler& aSrc);
   WsfSensorScheduler& operator=(const WsfSensorScheduler& aSrc) = delete;

   using SearchList      = std::vector<size_t>;
   using SearchListIndex = SearchList::size_type;
   const SearchList& GetSearchList() const { return mSearchList; }

   WsfSensor*        mSensorPtr;
   WsfSensorTracker* mTrackerPtr;

   //! A list of platform indices representing search chances.
   //! See the notes at the top of the implementation file for more information.
   SearchList mSearchList;

private:
   //! 'true' if 'debug_scheduler' was specified.
   bool mDebugEnabled;
};

#endif
