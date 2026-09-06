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

#include "Filter.hpp"

#include <memory>

#include "UtAttributeContainer.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMeasurementData.hpp"
#include "UtVec3.hpp"

namespace TrackingFilters
{

// =================================================================================================
Filter::Filter()
   : mFrameTime(0.0)
{
}

// =================================================================================================
//! Initialize the filter at the specified time, with an optional initial state.
//! @param aSimTime The simulation time at which the initial state (if any) is valid;
//! otherwise, the current simulation time.
//! @param aInitialStatePtr An optional initial state (6x1) vector, consisting of
//! a WCS location vector (first three values), and a WCS velocity vector
//! (second three values).
// virtual
bool Filter::Initialize(double           aSimTime,
                        const UtMatrixd* aInitialStatePtr) // = nullptr
{
   // By default, this just calls reset.
   Reset(aSimTime, aInitialStatePtr);
   return true;
}

// =================================================================================================
//! Reset the filter back to an initialized state.
//! @param aSimTime The simulation time at which the initial state (if any) is valid;
//! otherwise, the current simulation time.
//! @param aInitialStatePtr An optional initial state (6x1) vector, consisting of
//! a WCS location vector (first three values), and a WCS velocity vector
//! (second three values).
// virtual
void Filter::Reset(double aSimTime, const UtMatrixd* aInitialStatePtr /*= nullptr*/) {}


// ==================================================================================================
//! Returns the State Covariance.
//!
//! @returns A pointer to the state covariance matrix. This will be zero if the filter does not compute
//! a covariance matrix.
// virtual
const UtCovariance* Filter::GetStateCovariance()
{
   return nullptr;
}

// =================================================================================================
//! If the filter has a state covariance matrix, return the predicted state covariance.
//!@param aSimTime    The current simulation time, used to extrapolate the covariance.
//!@param aCovariance [output] A reference to the predicted covariance.
//!@returns true if the filter is able to provide the estimate.
// virtual
bool Filter::GetPredictedStateCovariance(double aSimTime, UtCovariance& aCovariance)
{
   return false;
}

// =================================================================================================
//! If the filter has a residual covariance matrix, return the predicted residual covariance.
//!@param aSimTime    The current simulation time, used to extrapolate the covariance.
//!@param aCovariance [output] A reference to the predicted covariance.
//!@returns true if the filter is able to provide the estimate.
// virtual
bool Filter::GetPredictedResidualCovariance(double aSimTime, UtCovariance& aCovariance)
{
   return false;
}

// =================================================================================================
//! Is the filter stable?
//! @returns true if the filter is stable and producing 'accurate' state estimates.
//! @returns false if the filter is simply absorbing measurements.
// virtual
bool Filter::IsStable() const
{
   return true;
}

// =================================================================================================
//! Returns the Normalized Residual Square Produced by the Filter
//! Also called Distance Function, or Track Score
//! @param aCurrentScore [output]
//! @returns true if this is received from a KalmanFilter
//! If the filter is not a Kalman filter this data is not valid and should not be used.
// virtual
bool Filter::GetCurrentMeasurementTrackScore(double& aCurrentScore)
{
   return false;
}

// =================================================================================================
//! Returns the AVERAGE Normalized Residual Square Produced by the Filter
//! Done by means of computing a weighted average of the last several individual
//! Distance Functions produced by the filter
//! @param aAverageScore [output]
//! @returns true if this is received from a KalmanFilter
//! If the filter is not a Kalman filter this data is not valid and should not be used.
// virtual
bool Filter::GetAverageTrackScore(double& aAverageScore)
{
   return false;
}

} // namespace TrackingFilters
