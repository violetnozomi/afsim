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

#ifndef WSFSENSORCOMPONENT_HPP
#define WSFSENSORCOMPONENT_HPP

class UtInput;
#include "WsfComponent.hpp"
class WsfPlatform;
class WsfSensor;
class WsfSensorBeam;
class WsfSensorMode;
class WsfSensorResult;
class WsfSensorTrackerSettings;
class WsfSimulation;
#include "WsfStandardSensorErrorModel.hpp"
class WsfTrack;
class WsfTrackId;

//! A abstract base class for sensor extension components.
//! Components can be added to implement additional functionality beyond what is provided
//! by a specific model. The responsibilities of a derived class include:
//! -) Implementing the 'required interface' methods as defined in WsfComponent.
//! -) Implementing any desired 'common framework' methods defined in WsfComponent.
//! -) Implementing any desired methods as defined below.
//! -) Implementing the specific capabilities required by the derived class.
class WSF_EXPORT WsfSensorComponent : public WsfComponentT<WsfSensor>
{
public:
   // Makes things a little shorter.
   using TrackerSettings = WsfSensorTrackerSettings;

   //! @name Utility methods.
   //@{
   WsfSensor*     GetSensor() const { return GetComponentParent(); }
   WsfPlatform*   GetPlatform() const;
   WsfSimulation* GetSimulation() const;
   //@}

   //! @name Generic methods.
   //!
   //! Actual sensor model implementations do not need to worry about calling these directly
   //! because they are required to call base class methods that end up calling these methods.
   //! (e.g. WsfRadarSensor::ProcessInput calls WsfSensor::ProcessInput).
   //!
   //! These always have the same argument lists as the routine from which they are called.
   //@{
   using WsfComponent::Initialize;
   using WsfComponent::Initialize2;
   using WsfComponent::PreInitialize;
   using WsfComponent::ProcessInput;

   virtual bool ProcessModeInput(UtInput& aInput, WsfSensorMode& aSensorMode) { return false; }
   virtual bool ProcessBeamInput(UtInput& aInput, WsfSensorBeam& aSensorBeam) { return false; }

   virtual void PlatformAdded(double aSimTime, WsfPlatform* aPlatformPtr) {}
   virtual void PlatformDeleted(WsfPlatform* aPlatformPtr) {}

   virtual void SelectMode(double aSimTime, WsfStringId aModeNameId) {}
   virtual void DeselectMode(double aSimTime, WsfStringId aModeNameId) {}
   virtual void TurnOn(double aSimTime) {}
   virtual void TurnOff(double aSimTime) {}
   //@}

   //! @name Model-specific methods.
   //!
   //! Actual sensor models (e.g. WsfRadarSensor, WsfGeometricSensor, etc.) must place calls in
   //! their code because there is no common location from where these methods could be called.
   //! See the static wrapper methods below.
   //@{
   //! Extends the sensor-model implementation of AttemptToDetect.
   virtual void AttemptToDetect(double aSimTime, WsfSensorResult& aResult) {}

   //! Extends the sensor-model implementation of AttemptToDetect with post detection by the sensor.
   //! @note Called after the sensor detection for a target is completed just before target is updated.
   //! @note Must modify the aResult to modify detection criteria
   virtual void PostAttemptToDetect(double aSimTime, WsfPlatform* aTargetPtr, WsfSensorResult& aResult) {}

   //! Modify or compute spherical measurement errors for any sensor error model that produces spherical sensor errors
   //! (see WsfStandardSensorErrorModel).
   virtual void ComputeSphericalMeasurementErrors(const WsfSensorResult&           aResult,
                                                  wsf::SphericalMeasurementErrors& aErrors) const
   {
   }

   // NOTE The callbacks from PerformScheduledDetections were broken up to pre and post to
   //      enable the same order of operations as was present in WSF 1.x.

   //! Extends WsfSensor::PerformScheduledDetections.
   //! Called by models just before entering the loop.
   virtual void PrePerformScheduledDetections(double aSimTime) {}

   //! Extends WsfSensor::PerformScheduledDetections
   //! Called by models just after exiting the loop.
   virtual void PostPerformScheduledDetections(double aSimTime) {}

   // Extends WsfSensorMode::UpdateTrack
   virtual void UpdateTrack(double aSimTime, WsfTrack* aTrackPtr, WsfPlatform* aTargetPtr, WsfSensorResult& aResult) {}
   //@}

   //! @name Track maintenance.
   //! These are called by derived implementations of WsfSensorTracker prior to performing the
   //! indicated function. They are more or less in order of call.
   //@{

   //! Called when WsfSensorTracker::TargetDetected receives a detection.
   //! @param aSimTime   The current simulation time.
   //! @param aSettings  The 'aSettings' argument from the calling 'Target...' method.
   //! @param aRequestId The 'aRequestId' argument from the calling 'Target...' method.
   //! @param aObjectId  The 'aObjectId' argument from the calling 'Target...' method.
   //! @param aTrackPtr  Pointer to the current track if one exists (can be null).
   //! @param aResult    The results from the sensor detection.
   //! Return true to allow track update/creation to proceed with the current detection.
   //! If false the detection will be ignored and any existing track will be dropped.
   virtual bool TrackerAllowTracking(double                 aSimTime,
                                     const TrackerSettings& aSettings,
                                     const WsfTrackId&      aRequestId,
                                     size_t                 aObjectId,
                                     WsfTrack*              aTrackPtr,
                                     WsfSensorResult&       aResult)
   {
      return true;
   }

   //! Called from an implementation of the WsfSensorTracker 'Target...' methods when they want to initiate a track.
   //! @param aSimTime   The current simulation time.
   //! @param aSettings  The 'aSettings' argument from the calling 'Target...' method.
   //! @param aRequestId The 'aRequestId' argument from the calling 'Target...' method.
   //! @param aObjectId  The 'aObjectId' argument from the calling 'Target...' method.
   //! @param aModePtr   The sensor mode used in creating the track.
   //! @param aTrackPtr  Updated with the pointer to the created track.
   virtual void TrackerInitializeTrack(double                 aSimTime,
                                       const TrackerSettings& aSettings,
                                       const WsfTrackId&      aRequestId,
                                       size_t                 aObjectId,
                                       WsfSensorMode*         aModePtr,
                                       WsfTrack*&             aTrackPtr)
   {
   }

   //! Called from an implementation of the WsfSensorTracker 'Target...' methods when they want to update a track.
   //! @param aSimTime   The current simulation time.
   //! @param aSettings  The 'aSettings' argument from the calling 'Target...' method.
   //! @param aRequestId The 'aRequestId' argument from the calling 'Target...' method.
   //! @param aObjectId  The 'aObjectId' argument from the calling 'Target...' method.
   //! @param aModePtr   The sensor mode used in updating the track.
   //! @param aTrackPtr  The track to be updated.
   //! @param aTargetPtr The platform representing the updated track.
   //! @param aResult    The results from the sensor detection causing the track update.
   virtual void TrackerUpdateTrack(double                 aSimTime,
                                   const TrackerSettings& aSettings,
                                   const WsfTrackId&      aRequestId,
                                   size_t                 aObjectId,
                                   WsfSensorMode*         aModePtr,
                                   WsfTrack*              aTrackPtr,
                                   WsfPlatform*           aTargetPtr,
                                   WsfSensorResult&       aResult)
   {
   }

   //! Called from an implementation of the WsfSensorTracker 'Target...' methods when they want to drop a track.
   //! @param aSimTime   The current simulation time.
   //! @param aSettings  The 'aSettings' argument from the calling 'Target...' method.
   //! @param aRequestId The 'aRequestId' argument from the calling 'Target...' method.
   //! @param aObjectId  The 'aObjectId' argument from the calling 'Target...' method.
   //! @param aModePtr   The sensor mode last used in updating the track.
   //! @param aTrackPtr  The track to be dropped.
   virtual void TrackerDropTrack(double                 aSimTime,
                                 const TrackerSettings& aSettings,
                                 const WsfTrackId&      aRequestId,
                                 size_t                 aObjectId,
                                 WsfSensorMode*         aModePtr,
                                 WsfTrack*              aTrackPtr)
   {
   }
   //@}

   //! @name Model-specific wrapper methods.
   //! These wrap the model-specific methods so specific models only need to have one line
   //! inserted into their code where component effects need to be evaluated.
   //@{
   static void AttemptToDetect(WsfSensor& aSensor, double aSimTime, WsfSensorResult& aResult);

   static bool PostAttemptToDetect(WsfSensor& aSensor, double aSimTime, WsfPlatform* aTargetPtr, WsfSensorResult& aResult);

   WSF_DEPRECATED static void ComputeMeasurementErrors(WsfSensor&       aSensor,
                                                       WsfSensorResult& aResult,
                                                       double&          aAzError,
                                                       double&          aElError,
                                                       double&          aRangeError,
                                                       double&          aRangeRateError);

   static void ComputeSphericalMeasurementErrors(WsfSensor&                       aSensor,
                                                 const WsfSensorResult&           aResult,
                                                 wsf::SphericalMeasurementErrors& aErrors);

   static void PrePerformScheduledDetections(WsfSensor& aSensor, double aSimTime);

   static void PostPerformScheduledDetections(WsfSensor& aSensor, double aSimTime);

   static void UpdateTrack(WsfSensor&       aSensor,
                           double           aSimTime,
                           WsfTrack*        aTrackPtr,
                           WsfPlatform*     aTargetPtr,
                           WsfSensorResult& aResult);
   //@}
};

#endif
