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

#ifndef WSFSensorOBSERVER_HPP
#define WSFSensorOBSERVER_HPP

#include "wsf_export.h"

#include "UtCallback.hpp"
#include "WsfSensor.hpp"
class WsfImage;
class WsfMessage;
class WsfSimulation;

namespace WsfObserver
{
using ImageCreatedCallback           = UtCallbackListN<void(double, WsfSensor*, WsfImage*)>;
using SensorDetectionAttemptCallback = UtCallbackListN<void(double, WsfSensor*, WsfPlatform*, WsfSensorResult&)>;
using SensorDetectionChangedCallback = UtCallbackListN<void(double, WsfSensor*, size_t, WsfSensorResult&)>;
using SensorFrequencyChangedCallback = UtCallbackListN<void(double, WsfSensor*, WsfSensorMode*)>;
using SensorBrokenCallback           = UtCallbackListN<void(double, WsfSensor*)>;
using SensorModeActivatedCallback    = UtCallbackListN<void(double, WsfSensor*, WsfSensorMode*)>;
using SensorModeDeactivatedCallback  = UtCallbackListN<void(double, WsfSensor*, WsfSensorMode*)>;
using SensorNonOperationalCallback   = UtCallbackListN<void(double, WsfSensor*)>;
using SensorOperationalCallback      = UtCallbackListN<void(double, WsfSensor*)>;
using SensorRequestCanceledCallback  = UtCallbackListN<void(double, WsfSensor*, const WsfTrack*)>;
using SensorRequestInitiatedCallback = UtCallbackListN<void(double, WsfSensor*, WsfSensorMode*, const WsfTrack*)>;
using SensorRequestUpdatedCallback   = UtCallbackListN<void(double, WsfSensor*, WsfSensorMode*, const WsfTrack*)>;
using SensorScanUpdatedCallback      = UtCallbackListN<void(double, WsfSensor*)>;
using SensorTurnedOffCallback        = UtCallbackListN<void(double, WsfSensor*)>;
using SensorTurnedOnCallback         = UtCallbackListN<void(double, WsfSensor*)>;

WSF_EXPORT ImageCreatedCallback&           ImageCreated(const WsfSimulation* aSimulationPtr);
WSF_EXPORT SensorDetectionAttemptCallback& SensorDetectionAttempt(const WsfSimulation* aSimulationPtr);
WSF_EXPORT SensorDetectionChangedCallback& SensorDetectionChanged(const WsfSimulation* aSimulationPtr);
WSF_EXPORT SensorFrequencyChangedCallback& SensorFrequencyChanged(const WsfSimulation* aSimulationPtr);
WSF_EXPORT SensorModeActivatedCallback&    SensorModeActivated(const WsfSimulation* aSimulationPtr);
WSF_EXPORT SensorModeDeactivatedCallback&  SensorModeDeactivated(const WsfSimulation* aSimulationPtr);
WSF_EXPORT SensorRequestCanceledCallback&  SensorRequestCanceled(const WsfSimulation* aSimulationPtr);
WSF_EXPORT SensorRequestInitiatedCallback& SensorRequestInitiated(const WsfSimulation* aSimulationPtr);
WSF_EXPORT SensorRequestUpdatedCallback&   SensorRequestUpdated(const WsfSimulation* aSimulationPtr);
WSF_EXPORT SensorScanUpdatedCallback&      SensorScanUpdated(const WsfSimulation* aSimulationPtr);
WSF_EXPORT SensorTurnedOffCallback&        SensorTurnedOff(const WsfSimulation* aSimulationPtr);
WSF_EXPORT SensorTurnedOnCallback&         SensorTurnedOn(const WsfSimulation* aSimulationPtr);
WSF_EXPORT SensorNonOperationalCallback&   SensorNonOperational(const WsfSimulation* aSimulationPtr);
WSF_EXPORT SensorOperationalCallback&      SensorOperational(const WsfSimulation* aSimulationPtr);
WSF_EXPORT SensorBrokenCallback&           SensorBroken(const WsfSimulation* aSimulationPtr);
} // namespace WsfObserver

//! The implementation of the Sensor observer objects.
//! An instance of this class is maintained by the simulation.
struct WSF_EXPORT WsfSensorObserver
{
   //-----------------------------------------------------------------------
   // Callback attributes
   //-----------------------------------------------------------------------

   //! An image has been created.
   //! @param aSimTime   The current simulation time.
   //! @param aSensorPtr The sensor that created the image.
   //! @param aImagePtr  The image created.
   WsfObserver::ImageCreatedCallback ImageCreated;

   //! A sensor has attempted to detect a target platform.
   //! @param aSimTime   The current simulation time.
   //! @param aSensorPtr The sensor that is performing the detection attempt.
   //! @param aTargetPtr The target that the sensor is attempting to detect.
   //! @param aResult    The result data from the detection attempt.
   //! @note This method is not currently called by any of the core sensors.
   //! It is provided for use by some high-fidelity add-on sensors.
   WsfObserver::SensorDetectionAttemptCallback SensorDetectionAttempt;

   //! A sensor detection of a target platform has changed is some way.
   //! @param aSimTime     The current simulation time.
   //! @param aSensorPtr   The sensor that is performing the detection.
   //! @param aTargetIndex The platform index of the target to which the detection status is changing.
   //! @param aResult      The result data from the detection.
   WsfObserver::SensorDetectionChangedCallback SensorDetectionChanged;

   //! A sensor mode has been activated.
   //! @param aSimTime   The current simulation time.
   //! @param aSensorPtr The sensor.
   //! @param aModePtr   The sensor mode that contains the beams in which the frequency was changed.
   WsfObserver::SensorFrequencyChangedCallback SensorFrequencyChanged;

   //! A sensor mode has been activated.
   //! @param aSimTime   The current simulation time.
   //! @param aSensorPtr The sensor.
   //! @param aModePtr   The sensor mode that has been activated.
   WsfObserver::SensorModeActivatedCallback SensorModeActivated;

   //! A sensor mode has been deactivated.
   //! @param aSimTime   The current simulation time.
   //! @param aSensorPtr The sensor.
   //! @param aModePtr   The sensor mode that has been deactivated.
   WsfObserver::SensorModeDeactivatedCallback SensorModeDeactivated;

   //! A sensor target tracking request has been canceled.
   //! @param aSimTime   The current simulation time.
   //! @param aSensorPtr The sensor being requested.
   //! @param aTrackPtr  The track that represents the target.
   WsfObserver::SensorRequestCanceledCallback SensorRequestCanceled;

   //! A sensor has been requested to track a target.
   //! @param aSimTime   The current simulation time.
   //! @param aSensorPtr The sensor being requested.
   //! @param aModePtr   The sensor mode being requested (can be zero).
   //! @param aTrackPtr  The track that represents the target.
   WsfObserver::SensorRequestInitiatedCallback SensorRequestInitiated;

   //! A sensor target tracking request has been updated (mode switch, track update, etc).
   //! @param aSimTime   The current simulation time.
   //! @param aSensorPtr The sensor being requested.
   //! @param aModePtr   The sensor mode being requested (can be zero).
   //! @param aTrackPtr  The track that represents the target.
   WsfObserver::SensorRequestUpdatedCallback SensorRequestUpdated;

   //! A sensor has changed its scan volume or scan center
   //! @param aSimTime   The current simulation time.
   //! @param aSensorPtr The sensor whose scan volume or center is changing.
   //! @note This method is not currently called by any of the core sensors.
   //! It is provided for use by some high-fidelity add-on sensors.
   WsfObserver::SensorScanUpdatedCallback SensorScanUpdated;

   //! A sensor has been turned off.
   //! @param aSimTime   The current simulation time.
   //! @param aSensorPtr The sensor that is being turned off.
   WsfObserver::SensorTurnedOffCallback SensorTurnedOff;

   //! A sensor has been turned on.
   //! @param aSimTime   The current simulation time.
   //! @param aSensorPtr The sensor that has been turned on.
   WsfObserver::SensorTurnedOnCallback SensorTurnedOn;

   //! A sensor has gone non-Operational.
   //! @param aSimTime   The current simulation time.
   //! @param aSensorPtr The sensor that is going non-Operational.
   WsfObserver::SensorNonOperationalCallback SensorNonOperational;

   //! A sensor has gone Operational.
   //! @param aSimTime   The current simulation time.
   //! @param aSensorPtr The sensor that is going Operational.
   WsfObserver::SensorOperationalCallback SensorOperational;

   //! A sensor has gone terminal.
   //! @param aSimTime   The current simulation time.
   //! @param aSensorPtr The sensor that is terminated.
   WsfObserver::SensorBrokenCallback SensorBroken;
};

#endif
