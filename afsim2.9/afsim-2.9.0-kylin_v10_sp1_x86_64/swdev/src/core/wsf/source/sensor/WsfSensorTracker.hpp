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

#ifndef WSFSENSORTRACKER_HPP
#define WSFSENSORTRACKER_HPP

#include "wsf_export.h"

class UtInput;
#include "UtRandom.hpp"
#include "UtVec3.hpp"
class WsfPlatform;
class WsfSensor;
class WsfSensorMode;
class WsfSensorResult;
class WsfSensorScheduler;
class WsfSimulation;
#include "WsfStringId.hpp"
class WsfTrackId;
#include "WsfTrackListFwd.hpp"

//! A class used to pass options to the tracker.
//! Note: This started as a nested class of WsfSensorTracker, but we needed to be able to forward
//!       declare this class to prevent circular references.
class WSF_EXPORT WsfSensorTrackerSettings
{
public:
   //! A bitmask of options that can be passed to the tracker
   enum Options
   {
      cSUPPRESS_DETECTION_CHANGE_MESSAGES = 0x00000001,
      cSUPPRESS_TRACKING_CRITERIA         = 0x00000002,
      cSUPPRESS_ALLOW_TRACKING_CHECK      = 0x00000004
   };

   WsfSensorTrackerSettings()
      : mOptions(0)
      , mComponentRole(0)
      , mComponentFlags(0)
   {
   }

   WsfSensorTrackerSettings(unsigned int aOptions, int aComponentRole, unsigned int aComponentFlags)
      : mOptions(aOptions)
      , mComponentRole(aComponentRole)
      , mComponentFlags(aComponentFlags)
   {
   }

   //!< A bitmask of Settings (from the enum Settings)
   unsigned int mOptions;
   //! The role of a the component calling the tracker.
   //! The tracker will invoke track-related functions for the sensor component with this
   //! role rather than those defined within the tracker. This allows the component to
   //! to create tracks exactly in the manner required.
   int mComponentRole;
   //! Additional flags that are passed to the component when mComponentRole is non-zero.
   unsigned int mComponentFlags;
};

//! A class that takes sensor detections and maintains tracks.
//!
//! The sensor tracker is called by the sensor detection logic to report the
//! results of detection attempts. The tracker will take the result and
//! initiate, update and drop tracks.
class WSF_EXPORT WsfSensorTracker
{
public:
   //! A nested class that defines the data returned by GetTargetState.
   class WSF_EXPORT TargetState
   {
   public:
      TargetState();

      UtVec3d mLocationWCS;
      UtVec3d mVelocityWCS;
      double  mLockonTime;
   };

   // Just to make things shorter.
   using Settings = WsfSensorTrackerSettings;

   WsfSensorTracker();
   virtual ~WsfSensorTracker() = default;

   virtual WsfSensorTracker* Clone() const;

   //! Returns 'true' if debugging is enabled for the tracker.
   bool DebugEnabled() const { return mDebugEnabled; }

   virtual unsigned int GetActiveTrackCount() const;

   virtual void GetActiveTrackList(WsfTrackList& aActiveTrackList);

   virtual unsigned int GetMaximumTrackCount() const;

   virtual bool GetTargetState(double aSimTime, const WsfTrackId& aRequestId, TargetState& aTargetState);

   virtual void GetRequestDataForTarget(size_t aObjectId, WsfTrackId& aRequestId, size_t& aModeIndex, WsfTrackId& aTrackId) const;

   virtual bool Initialize(double aSimTime, WsfSensor* aSensorPtr, WsfSensorScheduler* aSchedulerPtr);

   virtual bool ProcessInput(UtInput& aInput);

   virtual void SchedulerModeChange(double            aSimTime,
                                    const WsfTrackId& aRequestId,
                                    size_t            aObjectId,
                                    WsfPlatform*      aTargetPtr,
                                    WsfStringId       aNewModeName);

   virtual void StopTracking(double aSimTime, const WsfTrackId& aRequestId);

   virtual void TargetBlocked(double aSimTime, const Settings& aSettings, const WsfTrackId& aRequestId, size_t aObjectId);

   virtual bool TargetDeleted(double aSimTime, const Settings& aSettings, const WsfTrackId& aRequestId, size_t aObjectId);

   virtual void TargetDetected(double            aSimTime,
                               const Settings&   aSettings,
                               const WsfTrackId& aRequestId,
                               size_t            aObjectId,
                               WsfPlatform*      aTargetPtr,
                               WsfSensorResult&  aResult);

   virtual bool TargetSkipped(double aSimTime, const Settings& aSettings, const WsfTrackId& aRequestId, size_t aObjectId);

   virtual void TargetUndetected(double            aSimTime,
                                 const Settings&   aSettings,
                                 const WsfTrackId& aRequestId,
                                 size_t            aObjectId,
                                 WsfPlatform*      aTargetPtr,
                                 WsfSensorResult&  aResult);

   virtual void TurnOff(double aSimTime);

   virtual void TurnOn(double aSimTime);

   WsfSensor*     GetSensor() const { return mSensorPtr; }
   WsfSimulation* GetSimulation() const;

protected:
   WsfSensorTracker(const WsfSensorTracker& aSrc);
   WsfSensorTracker& operator=(const WsfSensorTracker& aSrc) = delete;

   ut::Random& GetRandom() const;

   bool AllowTrackingP(double            aSimTime,
                       const Settings&   aSettings,
                       const WsfTrackId& aRequestId,
                       size_t            aObjectId,
                       WsfTrack*         aTrackPtr, // TODO_JAJ - NEEDED?
                       WsfSensorResult&  aResult);

   void DropTrackP(double            aSimTime,
                   const Settings&   aSettings,
                   const WsfTrackId& aRequestId,
                   size_t            aObjectId,
                   WsfSensorMode*    aModePtr,
                   WsfTrack*&        aTrackPtr);

   void InitializeTrackP(double            aSimTime,
                         const Settings&   aSettings,
                         const WsfTrackId& aRequestId,
                         size_t            aObjectId,
                         WsfSensorMode*    aModePtr,
                         WsfTrack*&        aTrackPtr);

   void UpdateTrackP(double            aSimTime,
                     const Settings&   aSettings,
                     const WsfTrackId& aRequestId,
                     size_t            aObjectId,
                     WsfSensorMode*    aModePtr,
                     WsfTrack*         aTrackPtr,
                     WsfPlatform*      aTargetPtr,
                     WsfSensorResult&  aResult);

   //! The sensor to which the tracker is attached.
   WsfSensor* mSensorPtr;

   //! The scheduler that is attached to the sensor.
   //! This scheduler will be notified of any track updates that are associated with a particular request.
   WsfSensorScheduler* mSchedulerPtr;

private:
   //! 'true' if 'debug_tracker' was specified.
   bool mDebugEnabled;
};

#endif
