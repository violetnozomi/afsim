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

#include "WsfSensorScheduler.hpp"

#include <cassert>
#include <string>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "WsfDefaultSensorScheduler.hpp"
#include "WsfPhysicalScanSensorScheduler.hpp"
#include "WsfSectorScanSensorScheduler.hpp"
#include "WsfSensorTracker.hpp"

// =================================================================================================
WsfSensorScheduler::WsfSensorScheduler()
   : mSensorPtr(nullptr)
   , mTrackerPtr(nullptr)
   , mSearchList()
   , mDebugEnabled(false)
{
}

// =================================================================================================
WsfSensorScheduler::WsfSensorScheduler(const WsfSensorScheduler& aSrc)
   : mSensorPtr(nullptr)
   , mTrackerPtr(nullptr)
   , mSearchList()
   , mDebugEnabled(aSrc.mDebugEnabled)
{
}

// =================================================================================================
// virtual
WsfSensorScheduler* WsfSensorScheduler::Clone() const
{
   return new WsfSensorScheduler(*this);
}

// =================================================================================================
//! A notification call to indicate the frame time for the specified mode has changed.
//! @param aSimTime    The current simulation time.
//! @param aModeNameId The name (string ID) of the mode.
// virtual
void WsfSensorScheduler::FrameTimeChanged(double aSimTime, WsfStringId aModeNameId) {}

// =================================================================================================
//! Return the number of requests that are active in a specified mode.
//! @param aModeNameId The name (string ID) of the mode.
// virtual
int WsfSensorScheduler::GetActiveRequestCount(WsfStringId aModeNameId) const
{
   return 0;
}

// =================================================================================================
//! Return the maximum number of requests that can be processed by the indicated mode.
//! @param aModeNameId The name (string ID) of the mode.
// virtual
int WsfSensorScheduler::GetMaximumRequestCount(WsfStringId aModeNameId) const
{
   return 0;
}

// =================================================================================================
//! Does the scheduler have a request for the indicated track ID?
//! @param aRequestId The track ID. This should be the track ID of a track submitted to StartTracking.
// virtual
bool WsfSensorScheduler::HaveRequestFor(const WsfTrackId& aRequestId) const
{
   return false;
}

// =================================================================================================
//! Initialize the scheduler.
//! @param aSimTime    The current simulation time.
//! @param aSensorPtr  The sensor with which the scheduler is associated.
//! @param aTrackerPtr The tracker that is associated with the sensor. This can be zero if there is no tracker.
//! @returns 'true' if successful or 'false' if not.
// virtual
bool WsfSensorScheduler::Initialize(double aSimTime, WsfSensor* aSensorPtr, WsfSensorTracker* aTrackerPtr)
{
   assert(aSensorPtr != nullptr);
   mSensorPtr  = aSensorPtr;
   mTrackerPtr = aTrackerPtr;
   return true;
}

// =================================================================================================
//! A mode has been explicitly deselected.
//! This is called from WsfSensor::DeselectMode to indicate that a mode has been deselected.
//! @param aSimTime    The current simulation time.
//! @param aModeNameId The name (string ID) of the mode that has been deselected.
//! @note This is not called for implicit mode deselections that occur with tracking requests.
// virtual
void WsfSensorScheduler::ModeDeselected(double aSimTime, WsfStringId aModeNameId) {}

// =================================================================================================
//! A mode has been explicitly selected.
//! This is called from WsfSensor::SelectMode to indicate that a mode has been selected.
//! @param aSimTime    The current simulation time.
//! @param aModeNameId The name (string ID) of the mode that has been selected.
//! @note This is not called for implicit mode selections that occur with tracking requests.
// virtual
void WsfSensorScheduler::ModeSelected(double aSimTime, WsfStringId aModeNameId) {}

// =================================================================================================
//! A platform has been added to the simulation.
//! @param aSimTime     The current simulation time.
//! @param aPlatformPtr A pointer to the platform that was added.
// virtual
void WsfSensorScheduler::PlatformAdded(double aSimTime, WsfPlatform* aPlatformPtr) {}

// =================================================================================================
//! A platform has been removed from the simulation.
//! @param aPlatformPtr A pointer to the platform that was deleted.
// virtual
void WsfSensorScheduler::PlatformDeleted(WsfPlatform* aPlatformPtr) {}

// =================================================================================================
// virtual
bool WsfSensorScheduler::ProcessInput(UtInput& aInput)
{
   bool        myCommand = false;
   std::string command(aInput.GetCommand());
   if (command == "debug_scheduler")
   {
      mDebugEnabled = true;
      myCommand     = true;
   }
   return myCommand;
}

// =================================================================================================
//! Indicate that the specified target should be removed from further consideration.
//! This is called from the scheduler to indicate that the target should be removed from any scheduling list.
//! @param aSimTime     The current simulation time.
//! @param aTargetIndex The object ID of the target to be removed.
// virtual
void WsfSensorScheduler::RemoveTarget(double aSimTime, size_t aTargetIndex) {}

// =================================================================================================
//! Returns the track data for the request, this return is of a constant type to prevent modification.
//! @param aRequestId The track ID. This should be the track ID of a track submitted to StartTracking.
//! @returns The track that corresponds to the request.
// virtual
const WsfTrack& WsfSensorScheduler::GetTrackForRequest(const WsfTrackId& aRequestId) const
{
   static WsfTrack track;
   return track;
}

// =================================================================================================
//! Select a target for a detection attempt.
//! @param aSimTime     [input]   The current simulation time.
//! @param aNextSimTime [output]  The simulation time when the scheduler should next be called.
//! @param aTargetIndex [output]  The object ID of the target to be detected (typically a platform index or some other
//!                               unique ID). This will not be valid if the return value from the method is false.
//! @param aRequestId   [output]  The request ID (original track ID from StartTracking) associated with the target.
//!                               aRequest.IsNull() will be true if the target is not associated with a track request.
//!                               This will not be valid if the return value from the method is false.
//! @param aSettings    [updated] The sensor settings to be used for the detection attempt.
//!                               This will not be valid if the return value from the method is false.
//! @returns true if a target was selected or false if no target selected.
// virtual
bool WsfSensorScheduler::SelectTarget(double               aSimTime,
                                      double&              aNextSimTime,
                                      size_t&              aTargetIndex,
                                      WsfTrackId&          aRequestId,
                                      WsfSensor::Settings& aSettings)
{
   return false;
}

// =================================================================================================
//! Start tracking a target specified by a track.
//! @param aSimTime     The current simulation time.
//! @param aTrack       The target track cue.
//! @param aModeNameId The mode name (string ID) of the mode to be used.
//! If zero then the implementation will choose an appropriate mode.
//! @returns 'true' if the tracking request was accepted or 'false' if not.
// virtual
bool WsfSensorScheduler::StartTracking(double aSimTime, const WsfTrack& aTrack, WsfStringId aModeNameId)
{
   return false;
}

// =================================================================================================
//! Stop tracking a target.
//! @param aSimTime   The current simulation time.
//! @param aRequestId The track ID from the track that was provided to StartTracking.
//! @returns 'true' if the request was stopped.
// virtual
bool WsfSensorScheduler::StopTracking(double aSimTime, const WsfTrackId& aRequestId)
{
   return false;
}

// =================================================================================================
//! The sensor is being turned off.
//! @param aSimTime The current simulation time.
// virtual
void WsfSensorScheduler::TurnOff(double aSimTime) {}

// =================================================================================================
//! The sensor is being turned on.
//! @param aSimTime The current simulation time.
// virtual
void WsfSensorScheduler::TurnOn(double aSimTime) {}

// =================================================================================================
//! The tracker has changed the mode for the requested target.
//! @param aSimTime     The current simulation time.
//! @param aRequestId   If non-zero, the associated sensor scheduler request ID.
//! @param aTargetIndex The index of the target of interest.
//!                     For a platform this should be its platform ID and for other
//!                     objects it should be their unique ID.
//! @param aTargetPtr   The pointer of the target of interest.
//! @param aTrackPtr    Pointer to the current track associated with the target (may be zero).
//! @param aModeNameId  The name (ID) of the new mode.
// virtual
void WsfSensorScheduler::TrackerModeChange(double            aSimTime,
                                           const WsfTrackId& aRequestId,
                                           size_t            aTargetIndex,
                                           WsfPlatform*      aTargetPtr,
                                           const WsfTrack*   aTrackPtr,
                                           WsfStringId       aModeNameId)
{
}

WsfSimulation* WsfSensorScheduler::GetSimulation() const
{
   return mSensorPtr ? mSensorPtr->GetSimulation() : nullptr;
}
