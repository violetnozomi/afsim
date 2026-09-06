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

#include "WsfSensorComponent.hpp"

#include "UtInput.hpp"
#include "WsfComponentRoles.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorBeam.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSensorResult.hpp"

// =================================================================================================
WsfPlatform* WsfSensorComponent::GetPlatform() const
{
   return GetComponentParent()->GetPlatform();
}

// =================================================================================================
WsfSimulation* WsfSensorComponent::GetSimulation() const
{
   return GetComponentParent()->GetSimulation();
}

// =================================================================================================
//! Convenience method to call AttemptToDetect on all the components.
// static
void WsfSensorComponent::AttemptToDetect(WsfSensor& aSensor, double aSimTime, WsfSensorResult& aResult)
{
   const WsfSensor::ComponentList& components(aSensor.GetComponents());
   if (components.HasComponents())
   {
      for (auto component : components)
      {
         component->AttemptToDetect(aSimTime, aResult);
      }
   }
}

// =================================================================================================
//! Convenience method to call PostAttemptToDetect on all the components.
//! @param aSensor    [input] The sensor for which to loop all PostAttemptToDetect calls.
//! @param aSimTime   [input] The current simulation time.
//! @param aTargetPtr [input] The platform under detection.
//! @param aResult    [input/output] The results from the sensor detection causing the track update.
//! @returns bool The aResult.Detected() for all calls to the sensor's components with PostAttemptToDetect.
// static
bool WsfSensorComponent::PostAttemptToDetect(WsfSensor&       aSensor,
                                             double           aSimTime,
                                             WsfPlatform*     aTargetPtr,
                                             WsfSensorResult& aResult)
{
   const WsfSensor::ComponentList& components(aSensor.GetComponents());
   if (components.HasComponents())
   {
      for (auto component : components)
      {
         component->PostAttemptToDetect(aSimTime, aTargetPtr, aResult);
      }
   }
   return aResult.Detected();
}

// =================================================================================================
//! (DEPRECATED) Convenience method to call ComputeMeasurementErrors on all the components.
// static
void WsfSensorComponent::ComputeMeasurementErrors(WsfSensor&       aSensor,
                                                  WsfSensorResult& aResult,
                                                  double&          aAzError,
                                                  double&          aElError,
                                                  double&          aRangeError,
                                                  double&          aRangeRateError)
{
   const WsfSensor::ComponentList& components(aSensor.GetComponents());
   if (components.HasComponents())
   {
      wsf::SphericalMeasurementErrors errors{aAzError, aElError, aRangeError, aRangeRateError};

      for (auto component : components)
      {
         component->ComputeSphericalMeasurementErrors(aResult, errors);
      }

      aAzError        = errors.mAzError;
      aElError        = errors.mElError;
      aRangeError     = errors.mRangeError;
      aRangeRateError = errors.mRangeRateError;
   }
}

// =================================================================================================
//! Convenience method to call ComputeSphericalMeasurementErrors on all the components.
// static
void WsfSensorComponent::ComputeSphericalMeasurementErrors(WsfSensor&                       aSensor,
                                                           const WsfSensorResult&           aResult,
                                                           wsf::SphericalMeasurementErrors& aErrors)
{
   const WsfSensor::ComponentList& components(aSensor.GetComponents());
   if (components.HasComponents())
   {
      for (auto component : components)
      {
         component->ComputeSphericalMeasurementErrors(aResult, aErrors);
      }
   }
}

// =================================================================================================
//! Convenience method to call PrePerformScheduledDetections on all the components.
// static
void WsfSensorComponent::PrePerformScheduledDetections(WsfSensor& aSensor, double aSimTime)
{
   const WsfSensor::ComponentList& components(aSensor.GetComponents());
   if (components.HasComponents())
   {
      for (auto component : components)
      {
         component->PrePerformScheduledDetections(aSimTime);
      }
   }
}

// =================================================================================================
//! Convenience method to call PostPerformScheduledDetections on all the components.
// static
void WsfSensorComponent::PostPerformScheduledDetections(WsfSensor& aSensor, double aSimTime)
{
   const WsfSensor::ComponentList& components(aSensor.GetComponents());
   if (components.HasComponents())
   {
      for (auto component : components)
      {
         component->PostPerformScheduledDetections(aSimTime);
      }
   }
}

// =================================================================================================
//! Convenience method to call UpdateTrack on all the components.
// static
void WsfSensorComponent::UpdateTrack(WsfSensor&       aSensor,
                                     double           aSimTime,
                                     WsfTrack*        aTrackPtr,
                                     WsfPlatform*     aTargetPtr,
                                     WsfSensorResult& aResult)
{
   const WsfSensor::ComponentList& components(aSensor.GetComponents());
   if (components.HasComponents())
   {
      for (auto component : components)
      {
         component->UpdateTrack(aSimTime, aTrackPtr, aTargetPtr, aResult);
      }
   }
}
