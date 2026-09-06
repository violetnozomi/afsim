// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSINGLESENSOROBSERVER_HPP
#define WSFSINGLESENSOROBSERVER_HPP

#include "wsf_export.h"
class WsfPlatform;
class WsfSensor;
class WsfSensorResult;
class WsfTrack;

//! A light-weight observer of a single sensor.
//!
//! A object may register an instance of this class with a particular sensor in order to receive
//! notification of the events defined by the virtual methods. Unlike the simulation observers which
//! receive notification of ALL occurrences of a subscribed event, this is directed at observing
//! a SINGLE sensor. It is lighter weight and faster, but the user must take care to follow the
//! process described below.
//!
//! The typical use is to subclass this, define one or more of the virtual methods and attach to the
//! platform using WsfSensor::AttachObserver(). Once an object attaches, it MUST detach from the
//! sensor using WsfSensor::DetachObserver prior to its destruction.
class WSF_EXPORT WsfSingleSensorObserver
{
public:
   virtual ~WsfSingleSensorObserver() = default;

   //! Called just after the sensor is turned off.
   virtual void OnSensorTurnedOff(double aSimTime, WsfSensor* aSensorPtr) {}

   //! Called just after the sensor is turned on.
   virtual void OnSensorTurnedOn(double aSimTime, WsfSensor* aSensorPtr) {}

   //! Called before the sensor is actually deleted.
   virtual void OnSensorDeleting(WsfSensor* aSensorPtr) {}

   //! Called just before the sensor is removed from the simulation.
   virtual void OnSensorDeleted(WsfSensor* aSensorPtr) {}

   //! Called after each sensor update.
   virtual void OnSensorTargetUpdated(double aSimTime, WsfSensor* aSensorPtr, WsfPlatform* aTargetPtr, WsfSensorResult& aResult)
   {
   }

   //! Called after each sensor detection attempt, per mode/beam for multi-mode/beam sensors.
   virtual void
   OnSensorDetectionAttempted(double aSimTime, WsfSensor* aSensorPtr, WsfPlatform* aTargetPtr, WsfSensorResult& aResult)
   {
   }

   //! Called after initial sensor track formation by a sensor (similar to Observer::SensorTrackInitiated).
   virtual void OnSensorTrackInitiated(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack& aTrack) {}

   //! Called after each subsequent update of sensor track by a sensor (similar to Observer::SensorTrackUpdated).
   virtual void OnSensorTrackUpdated(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack& aTrack) {}

   //! Called after a sensor track is dropped by a sensor (similar to Observer::SensorTrackDropped).
   virtual void OnSensorTrackDropped(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack& aTrack) {}

   ////! Called after the sensor detection for a target is completed just before target is updated.
   // virtual void OnSensorDetectionCompleted(double           aSimTime,
   //                                         WsfSensor*       aSensorPtr,
   //                                         WsfPlatform*     aTargetPtr,
   //                                         WsfSensorResult& aResult)
   //{}
};

#endif
