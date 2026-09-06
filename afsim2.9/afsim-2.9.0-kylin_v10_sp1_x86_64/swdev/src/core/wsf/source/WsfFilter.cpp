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

#include "WsfFilter.hpp"

#include "WsfSimulation.hpp"

WsfFilter::WsfFilter(TrackingFilters::Filter* aFilterPtr)
   : WsfObject()
   , mFilterPtr(aFilterPtr)
{
}

WsfFilter::WsfFilter(const WsfFilter& aSrc)
   : WsfObject(aSrc)
   , mFilterPtr(aSrc.mFilterPtr->Clone())
{
}

WsfFilter& WsfFilter::operator=(const WsfFilter& aRhs)
{
   if (this != &aRhs)
   {
      WsfObject::operator=(aRhs);
      mFilterPtr         = aRhs.mFilterPtr->Clone();
   }
   return *this;
}

// virtual
WsfFilter::~WsfFilter()
{
   delete mFilterPtr;
}

// virtual
bool WsfFilter::ProcessInput(UtInput& aInput)
{
   return mFilterPtr->ProcessInput(aInput);
}

//! Initialize the filter with the option of providing simulation and / or scenario specific data,
//! and an initial state.
//! @param aSimTime         The simulation time at which the initial state (if any) is valid;
//!                         otherwise, the current simulation time.
//! @param aSimulationPtr   The simulation object pointer.
//! @param aInitialStatePtr An optional initial state (6x1) vector, consisting of
//!                         a WCS location vector (first three values), and a WCS velocity vector
//!                        (second three values).
// virtual
bool WsfFilter::Initialize(double aSimTime, WsfSimulation* aSimulationPtr, const UtMatrixd* aInitialStatePtr /*=nullptr*/)
{
   return mFilterPtr->Initialize(aSimTime, aInitialStatePtr);
}

//! Reset the filter back to an initialized state.
//! @param aSimTime         The simulation time at which the initial state (if any) is valid;
//!                         otherwise, the current simulation time.
//! @param aInitialStatePtr An optional initial state (6x1) vector, consisting of
//!                         a WCS location vector (first three values), and a WCS velocity vector
//!                         (second three values).
void WsfFilter::Reset(double aSimTime, const UtMatrixd* aInitialStatePtr /*= nullptr*/)
{
   mFilterPtr->Reset(aSimTime, aInitialStatePtr);
}

void WsfFilter::Update(double aSimTime, const UtMeasurementData& aDataIn, UtMeasurementData& aDataOut)
{
   mFilterPtr->Update(aSimTime, aDataIn, aDataOut);
}

void WsfFilter::NoDetectUpdate(double aSimTime, UtMeasurementData& aDataOut)
{
   mFilterPtr->NoDetectUpdate(aSimTime, aDataOut);
}

void WsfFilter::GetKinematicEstimate(double aSimTime, UtMeasurementData& aDataOut)
{
   mFilterPtr->GetKinematicEstimate(aSimTime, aDataOut);
}

//! Returns the State Covariance.
//! @returns A pointer to the state covariance matrix. This will be zero if the filter does not compute
//! a covariance matrix.
const UtCovariance* WsfFilter::GetStateCovariance()
{
   return mFilterPtr->GetStateCovariance();
}

const UtCovariance* WsfFilter::GetResidualCovariance()
{
   return mFilterPtr->GetResidualCovariance();
}

//! If the filter has a state covariance matrix, return the predicted state covariance.
//!@param aSimTime    The current simulation time, used to extrapolate the covariance.
//!@param aCovariance [output] A reference to the predicted covariance.
//!@returns true if the filter is able to provide the estimate.
bool WsfFilter::GetPredictedStateCovariance(double aSimTime, UtCovariance& aCovariance)
{
   return mFilterPtr->GetPredictedStateCovariance(aSimTime, aCovariance);
}

//! If the filter has a residual covariance matrix, return the predicted residual covariance.
//!@param aSimTime    The current simulation time, used to extrapolate the covariance.
//!@param aCovariance [output] A reference to the predicted covariance.
//!@returns true if the filter is able to provide the estimate.
bool WsfFilter::GetPredictedResidualCovariance(double aSimTime, UtCovariance& aCovariance)
{
   return mFilterPtr->GetPredictedResidualCovariance(aSimTime, aCovariance);
}

//! Returns the Normalized Residual Square Produced by the Filter
//! Also called Distance Function, or Track Score
//! @param aCurrentScore [output]
//! @returns true if this is received from a KalmanFilter
//! If the filter is not a Kalman filter this data is not valid and should not be used.
bool WsfFilter::GetCurrentMeasurementTrackScore(double& aCurrentScore)
{
   return mFilterPtr->GetCurrentMeasurementTrackScore(aCurrentScore);
}

//! Returns the AVERAGE Normalized Residual Square Produced by the Filter
//! Done by means of computing a weighted average of the last several individual
//! Distance Functions produced by the filter
//! @param aAverageScore [output]
//! @returns true if this is received from a KalmanFilter
//! If the filter is not a Kalman filter this data is not valid and should not be used.
bool WsfFilter::GetAverageTrackScore(double& aAverageScore)
{
   return mFilterPtr->GetAverageTrackScore(aAverageScore);
}

//! Is the filter stable?
//! @returns true if the filter is stable and producing 'accurate' state estimates.
//! @returns false if the filter is simply absorbing measurements.
//! @note This method should be checked as true before attempting to retrieve or use
//! filter state information.  A stable filter must be able to provide valid WCS location
//! and velocity estimates.
bool WsfFilter::IsStable() const
{
   return mFilterPtr->IsStable();
}

//! Set the 'frame time' for the sensor, which is the typical time to complete a scan.
void WsfFilter::SetFrameTime(double aFrameTime)
{
   mFilterPtr->SetFrameTime(aFrameTime);
}
