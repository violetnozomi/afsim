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

#ifndef TRACKING_FILTERS_FILTER_HPP
#define TRACKING_FILTERS_FILTER_HPP

#include "tracking_filters_export.h"

class UtCalendar;
class UtCovariance;
class UtInput;
#include "UtMatrix.hpp"
class UtMeasurementData;

namespace TrackingFilters
{
//! An abstract base class for filters (alpha-beta, Kalman, etc).
//! Derived classes must re-implement Clone, Update, NoDetectUpdate and GetKinematicEstimate.
//!
//! Filters are used by sensors and trackers to produce estimates of target location and velocity
//! based on a history of measured values.
class TRACKING_FILTERS_EXPORT Filter
{
public:
   Filter();
   virtual ~Filter() = default;

   virtual Filter* Clone() const = 0;

   virtual bool Initialize(double aSimTime, const UtMatrixd* aInitialStatePtr = nullptr);

   virtual void Reset(double aSimTime, const UtMatrixd* aInitialStatePtr = nullptr);

   //! Update the filter's state with a measurement.
   //!@param aSimTime The current simulation time.
   //!@param aDataIn Structure containing valid measurement data.
   //!@param aDataOut Structure containing output data from the filter
   //! (output data will be marked valid).
   virtual void Update(double aSimTime, const UtMeasurementData& aDataIn, UtMeasurementData& aDataOut) = 0;

   //! Update the filter without a measurement.
   //!@param aSimTime The current simulation time.
   //!@param aDataOut Structure containing output data from the filter
   //! (output data will be marked valid).
   virtual void NoDetectUpdate(double aSimTime, UtMeasurementData& aDataOut) = 0;

   //! Provide a kinematic estimate of location and velocity (if available).
   //!@param aDataOut Structure containing the kinematic estimate
   // (output data will be marked valid).
   virtual void GetKinematicEstimate(double aSimTime, UtMeasurementData& aDataOut) = 0;

   virtual bool ProcessInput(UtInput& aInput) { return false; }

   virtual const UtCovariance* GetStateCovariance();

   virtual const UtCovariance* GetResidualCovariance() { return nullptr; }

   virtual bool GetPredictedStateCovariance(double aSimTime, UtCovariance& aCovariance);

   virtual bool GetPredictedResidualCovariance(double aSimTime, UtCovariance& aCovariance);

   virtual bool GetCurrentMeasurementTrackScore(double& aCurrentScore);

   virtual bool GetAverageTrackScore(double& aAverageScore);

   virtual bool IsStable() const;

   //! Set the 'frame time' for the sensor, which is the typical time to complete a scan.
   void SetFrameTime(double aFrameTime) { mFrameTime = aFrameTime; }

protected:
   Filter(const Filter& aSrc) = default;

   double mFrameTime;
};
} // namespace TrackingFilters
#endif
