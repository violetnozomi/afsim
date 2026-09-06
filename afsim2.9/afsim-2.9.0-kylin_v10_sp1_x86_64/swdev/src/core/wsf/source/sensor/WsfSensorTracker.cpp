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

#include "WsfSensorTracker.hpp"

#include <cassert>

#include "UtInput.hpp"
#include "UtVec3.hpp"
#include "WsfPlatform.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSensorScheduler.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrackId.hpp"

// =================================================================================================
WsfSensorTracker::WsfSensorTracker()
   : mSensorPtr(nullptr)
   , mSchedulerPtr(nullptr)
   , mDebugEnabled(false)
{
}

// =================================================================================================
//! Copy constructor (for Clone())
// protected
WsfSensorTracker::WsfSensorTracker(const WsfSensorTracker& aSrc)
   : mSensorPtr(nullptr)
   , mSchedulerPtr(nullptr)
   , mDebugEnabled(aSrc.mDebugEnabled)
{
}

// =================================================================================================
//! Clone this tracker.
// virtual
WsfSensorTracker* WsfSensorTracker::Clone() const
{
   return new WsfSensorTracker(*this);
}

// =================================================================================================
//! Return the number of tracks being maintained by the sensor.
// virtual
unsigned int WsfSensorTracker::GetActiveTrackCount() const
{
   return 0;
}

// =================================================================================================
//! Return the active tracks the sensor is maintaining.
// virtual
void WsfSensorTracker::GetActiveTrackList(WsfTrackList& aActiveTrackList) {}

// =================================================================================================
//! Return the maximum number of tracks that can be maintained by the sensor.
// virtual
unsigned int WsfSensorTracker::GetMaximumTrackCount() const
{
   return 0;
}

// =================================================================================================
//! Get the current perceived state of a particular target.
//!
//! @param aSimTime     The current simulation time.
//! @param aRequestId   The scheduler request ID.
//! @param aTargetState The returned target state data (valid only if the function return value is 'true').
//! @returns 'true' if the tracker was able to provide target state data or 'false' if no data is available.
// virtual
bool WsfSensorTracker::GetTargetState(double aSimTime, const WsfTrackId& aRequestId, TargetState& aTargetState)
{
   return false;
}

// =================================================================================================
//! Get the request ID particular target.
//!
//! @param aObjectId  The index of the target of interest.
//! @param aRequestId The scheduler request ID for the target of interest.
//! @param aModeIndex The mode index of the target request.
//! @param aTrackId   The track Id of the request, 'null' if not found.
// virtual
void WsfSensorTracker::GetRequestDataForTarget(size_t      aObjectId,
                                               WsfTrackId& aRequestId,
                                               size_t&     aModeIndex,
                                               WsfTrackId& aTrackId) const
{
   aRequestId.Null(); // return null as this is just a placeholder.
   aModeIndex = 0;
   aTrackId.Null();
}

// =================================================================================================
//! Initialize the tracker.
//!
//! @param aSimTime      The current simulation time.
//! @param aSensorPtr    The pointer to the sensor that owns this tracker.
//! @param aSchedulerPtr The pointer to the sensor scheduler that is associated with the sensor.
//!
//! @note A tracker may be associated with only one sensor.
// virtual
bool WsfSensorTracker::Initialize(double aSimTime, WsfSensor* aSensorPtr, WsfSensorScheduler* aSchedulerPtr)
{
   assert(aSensorPtr != nullptr);
   mSensorPtr    = aSensorPtr;
   mSchedulerPtr = aSchedulerPtr;
   return true;
}

// =================================================================================================
//! Process tracker input.
//! @see WsfObject::ProcessInput() for a description of this method.
// virtual
bool WsfSensorTracker::ProcessInput(UtInput& aInput)
{
   bool        myCommand = false;
   std::string command(aInput.GetCommand());
   if (command == "debug_tracker")
   {
      mDebugEnabled = true;
      myCommand     = true;
   }
   return myCommand;
}

// =================================================================================================
//! The scheduler is changing the mode for the indicated object.
//!
//! This method is called by the scheduler to indicate it is changing the mode.
//!
//! @param aSimTime     The current simulation time.
//! @param aRequestId   If non-zero, the associated sensor scheduler request ID.
//! @param aObjectId    A unique identifier of the object that is trying to be detected.
//!                     For a platform this should be its platform ID and for other
//!                     objects it should be their unique ID.
//! @param aTargetPtr   The platform that owns the object that is trying to be detected.
//! @param aNewModeName The name (ID) of the new mode.
// virtual
void WsfSensorTracker::SchedulerModeChange(double            aSimTime,
                                           const WsfTrackId& aRequestId,
                                           size_t            aObjectId,
                                           WsfPlatform*      aTargetPtr,
                                           WsfStringId       aNewModeName)
{
}

// =================================================================================================
//! A request has been made to stop a tracking request.
//! @param aSimTime   The current simulation time.
//! @param aRequestId The request Id.
// virtual
void WsfSensorTracker::StopTracking(double aSimTime, const WsfTrackId& aRequestId) {}

// =================================================================================================
//! Request that a target be 'blocked'.
//!
//! An external component may call this to indicate that a target is 'block'. Its function is to
//! immediately drop any track associated with the target.
//!
//! @param aSimTime   The current simulation time.
//! @param aSettings  The settings/options for the tracker update.
//! @param aRequestId If non-zero, the associated sensor scheduler request ID.
//! @param aObjectId  A unique identifier of the object that is trying to be blocked. For a platform this should
//! be its platform ID. For other objects it should be their unique ID.
//!
//! @note This routine may call WsfSensor::DropTrack() (or its derived class implementation) to drop a track.
// virtual
void WsfSensorTracker::TargetBlocked(double aSimTime, const Settings& aSettings, const WsfTrackId& aRequestId, size_t aObjectId)
{
}

// =================================================================================================
//! Process a target object that has been deleted (or turned off)
//!
//! @param aSimTime   The current simulation time.
//! @param aSettings  The settings/options for the tracker update.
//! @param aRequestId If non-zero, the associated sensor scheduler request ID.
//! @param aObjectId  A unique identifier of the object that is trying to be blocked. For a platform this should
//! be its platform ID. For other objects it should be their unique ID.
//!
//! @returns 'true' if the target is no longer being tracked or 'false' if it is still being tracked.
//! The caller should continue to call this method until the track is dropped.
//!
//! @note This routine may call WsfSensor::DropTrack() (or its derived class implementation) to drop a track.
// virtual
bool WsfSensorTracker::TargetDeleted(double aSimTime, const Settings& aSettings, const WsfTrackId& aRequestId, size_t aObjectId)
{
   return true;
}

// =================================================================================================
//! Process a successful detection attempt.
//!
//! This routine should be called whenever a detection attempt is successful.
//! The tracker can do things like correlation and track maintenance.
//!
//! @param aSimTime   The current simulation time.
//! @param aSettings  The settings/options for the tracker update.
//! @param aRequestId If non-zero, the associated sensor scheduler request ID.
//! @param aObjectId  A unique identifier of the object that is trying to be blocked. For a platform this should
//! be its platform ID. For other objects it should be their unique ID.
//! @param aTargetPtr The platform that owns the object that is trying to be detected.
//! @param aResult    The detection result structure that contains the data about the detection attempt.
//!
//! @note This routine may call WsfSensor::InitializeTrack() and WsfSensor::UpdateTrack()
//! (or their derived class implementations) to initialize or update a track.
// virtual
void WsfSensorTracker::TargetDetected(double            aSimTime,
                                      const Settings&   aSettings,
                                      const WsfTrackId& aRequestId,
                                      size_t            aObjectId,
                                      WsfPlatform*      aTargetPtr,
                                      WsfSensorResult&  aResult)
{
}

// =================================================================================================
//! Process a skipped detection chance.
//!
//! This is called when a detection chance is bypassed. This is most likely caused when a tracking
//! mode is activated that precludes searching.
//!
//! @param aSimTime   The current simulation time.
//! @param aSettings  The settings/options for the tracker update.
//! @param aRequestId If non-zero, the associated sensor scheduler request ID.
//! @param aObjectId  A unique identifier of the object that is trying to be blocked. For a platform this should
//! be its platform ID. For other objects it should be their unique ID.
//!
//! @returns 'true' if the target is no longer being tracked and 'false' if the target is still being tracked.
//! The caller should continue to call this method until the track is dropped.
//!
//! @note This routine may call WsfSensor::DropTrack() (or its derived class implementation) to drop a track.
// virtual
bool WsfSensorTracker::TargetSkipped(double aSimTime, const Settings& aSettings, const WsfTrackId& aRequestId, size_t aObjectId)
{
   return true;
}

// =================================================================================================
//! Process an unsuccessful detection attempt.
//!
//! This routine should be called whenever a detection attempt is unsuccessful.
//! The tracker can do things like update a detection history and drop tracks.
//!
//! @param aSimTime   The current simulation time.
//! @param aSettings  The settings/options for the tracker update.
//! @param aRequestId If non-zero, the associated sensor scheduler request ID.
//! @param aObjectId  A unique identifier of the object that is trying to be blocked. For a platform this should
//! be its platform ID. For other objects it should be their unique ID.
//! @param aTargetPtr The platform that owns the object that is trying to be detected.
//! @param aResult    The detection result structure that contains the data about the detection attempt.
//!
//! @note This routine may call WsfSensor::DropTrack() (or its derived class implementation) to drop a track.
// virtual
void WsfSensorTracker::TargetUndetected(double            aSimTime,
                                        const Settings&   aSettings,
                                        const WsfTrackId& aRequestId,
                                        size_t            aObjectId,
                                        WsfPlatform*      aTargetPtr,
                                        WsfSensorResult&  aResult)
{
}

// =================================================================================================
//! The associated sensor is being turned off.
//! @param aSimTime The current simulation time.
// virtual
void WsfSensorTracker::TurnOff(double aSimTime) {}

// =================================================================================================
//! The associated sensor is being turned on.
//! @param aSimTime The current simulation time.
// virtual
void WsfSensorTracker::TurnOn(double aSimTime) {}

// =================================================================================================
WsfSimulation* WsfSensorTracker::GetSimulation() const
{
   return (mSensorPtr != nullptr) ? mSensorPtr->GetSimulation() : nullptr;
}

// =================================================================================================
ut::Random& WsfSensorTracker::GetRandom() const
{
   return GetSimulation()->GetRandom();
}

// =================================================================================================
//! A convenience method for tracker implementations to ask components if an object should be allowed to tracked.
bool WsfSensorTracker::AllowTrackingP(double            aSimTime,
                                      const Settings&   aSettings,
                                      const WsfTrackId& aRequestId,
                                      size_t            aObjectId,
                                      WsfTrack*         aTrackPtr,
                                      WsfSensorResult&  aResult)
{
   // This check is not made if the call is indirectly from something that is injecting a track and they
   // don't want this check.
   if ((aSettings.mOptions & Settings::cSUPPRESS_ALLOW_TRACKING_CHECK) == 0)
   {
      const WsfSensor::ComponentList& components(mSensorPtr->GetComponents());
      for (auto component : components)
      {
         if (!component->TrackerAllowTracking(aSimTime, aSettings, aRequestId, aObjectId, aTrackPtr, aResult))
         {
            return false;
         }
      }
   }
   return true;
}

// =================================================================================================
//! A convenience method for tracker implementations for dropping a track.
//! This will invoke either WsfSensor::DropTrack or the component represented by the settings.
//! Note that this is purely a notification. The track must be dropped by the caller.
void WsfSensorTracker::DropTrackP(double            aSimTime,
                                  const Settings&   aSettings,
                                  const WsfTrackId& aRequestId,
                                  size_t            aObjectId,
                                  WsfSensorMode*    aModePtr,
                                  WsfTrack*&        aTrackPtr)
{
   // If specified, direct the request to a specific component.
   if (aSettings.mComponentRole != 0)
   {
      const WsfSensor::ComponentList& components(mSensorPtr->GetComponents());
      WsfSensorComponent*             componentPtr = components.FindComponentByRole(aSettings.mComponentRole);
      if (componentPtr != nullptr)
      {
         componentPtr->TrackerDropTrack(aSimTime, aSettings, aRequestId, aObjectId, aModePtr, aTrackPtr);
         return;
      }
   }

   // Not directed to a specific component, so notify ALL components.
   const WsfSensor::ComponentList& components(mSensorPtr->GetComponents());
   for (auto component : components)
   {
      component->TrackerDropTrack(aSimTime, aSettings, aRequestId, aObjectId, aModePtr, aTrackPtr);
   }
}

// =================================================================================================
//! A convenience method for tracker implementations for initializing a track.
//! This will invoke either WsfSensorMode::InitializeTrack or the component represented by the settings.
void WsfSensorTracker::InitializeTrackP(double            aSimTime,
                                        const Settings&   aSettings,
                                        const WsfTrackId& aRequestId,
                                        size_t            aObjectId,
                                        WsfSensorMode*    aModePtr,
                                        WsfTrack*&        aTrackPtr)
{
   // If specified, direct the request to a specific component.
   if (aSettings.mComponentRole != 0)
   {
      const WsfSensor::ComponentList& components(mSensorPtr->GetComponents());
      WsfSensorComponent*             componentPtr = components.FindComponentByRole(aSettings.mComponentRole);
      if (componentPtr != nullptr)
      {
         componentPtr->TrackerInitializeTrack(aSimTime, aSettings, aRequestId, aObjectId, aModePtr, aTrackPtr);
         return;
      }
   }

   // The request is not directed to a specific component OR the requested component does not exist.
   // (The latter case should not occur because this is indirectly being called by the component to
   // whom it should be directed.
   aModePtr->InitializeTrack(aSimTime, aTrackPtr);
}

// =================================================================================================
//! A convenience method for tracker implementations for updating a track.
//! This will invoke either WsfSensorMode::UpdateTrack or the component represented by the settings.
void WsfSensorTracker::UpdateTrackP(double            aSimTime,
                                    const Settings&   aSettings,
                                    const WsfTrackId& aRequestId,
                                    size_t            aObjectId,
                                    WsfSensorMode*    aModePtr,
                                    WsfTrack*         aTrackPtr,
                                    WsfPlatform*      aTargetPtr,
                                    WsfSensorResult&  aResult)
{
   // If specified, direct the request to a specific component.
   if (aSettings.mComponentRole != 0)
   {
      const WsfSensor::ComponentList& components(mSensorPtr->GetComponents());
      WsfSensorComponent*             componentPtr = components.FindComponentByRole(aSettings.mComponentRole);
      if (componentPtr)
      {
         componentPtr->TrackerUpdateTrack(aSimTime, aSettings, aRequestId, aObjectId, aModePtr, aTrackPtr, aTargetPtr, aResult);
         return;
      }
   }

   // The request is not directed to a specific component OR the requested component does not exist.
   // (The latter case should not occur because this is indirectly being called by the component to
   // whom it should be directed.
   aModePtr->UpdateTrack(aSimTime, aTrackPtr, aTargetPtr, aResult);
}

// =================================================================================================
//! Constructor for nested class TargetState
WsfSensorTracker::TargetState::TargetState()
   : mLocationWCS()
   , mVelocityWCS()
   , mLockonTime(-1.0)
{
}
