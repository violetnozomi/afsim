// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef ORBITDETERMINATIONKALMANFILTER_HPP
#define ORBITDETERMINATIONKALMANFILTER_HPP

#include "tracking_filters_export.h"

#include <memory>
#include <vector>

#include "KalmanFilter.hpp"
#include "UtCalendar.hpp"
#include "UtCloneablePtr.hpp"
#include "UtCovariance.hpp"
#include "UtECI_Conversion.hpp"
#include "UtMatrix.hpp"
#include "UtOrbitalPropagatorBase.hpp"
#include "UtOrbitalState.hpp"

namespace TrackingFilters
{

//! Implementation of an Unscented Kalman Filter (UKF) for orbital determination and tracking.
//! Internally it is a 6-State Kalman Filter (ECI location (3 states) and ECI velocity (3 states).
//! These states and the state covariances are converted to ECI on input and back to WCS on output,
//! in order to maintain proper "canonical" representation in measurements / tracks.
//! The filter stabilizes either by using a linear state transition matrix that approximates orbital motion,
//! during which the predicted state and predicted covariance are computed using it; or, if an initial state is
//! provided, initial orbital motion is computed directly using an orbital propagator during this phase.
//! Once the filter stabilizes, the state covariance is represented as a set of 12 "Sigma Points" in phase space.
//! These are propagated using orbital propagators to the desired time.  The the predicted state covariance is
//! reconstituted from the sigma point propagators, and the predicted state is computed as the average of the
//! sigma point phase space locations.
//!@note It is recommended to provide an initial state  in the call to Initialize(),
//! in order to provide the option to propagate the predicted state nonlinearly while the filter is stabilizing.
class TRACKING_FILTERS_EXPORT OrbitDeterminationKalmanFilter : public KalmanFilter
{
public:
   OrbitDeterminationKalmanFilter();
   OrbitDeterminationKalmanFilter(const OrbitDeterminationKalmanFilter& aSrc);
   OrbitDeterminationKalmanFilter& operator=(const OrbitDeterminationKalmanFilter& aSrc) = delete;
   ~OrbitDeterminationKalmanFilter() override                                            = default;

   Filter* Clone() const override;

   bool ProcessInput(UtInput& aInput) override;

   bool Initialize(double aSimTime, const ut::OrbitalState& aState);

   bool Initialize(double            aSimTime,
                   const UtCalendar& aEpoch,
                   const UtMatrixd*  aInitialStatePtr = nullptr); // TODO remove

   void SetStatePropagator(std::unique_ptr<UtOrbitalPropagatorBase> aStatePropagatorPtr);

   const UtOrbitalPropagatorBase* GetStatePropagator() const { return mStatePropagatorPtr.get(); }

   bool GetPredictedStateCovariance(double aSimTime, UtCovariance& aCovariance) override;

   //! Get the current orbital state from the state propagator.
   const ut::OrbitalState& GetOrbitalState() const { return mStatePropagatorPtr->GetOrbitalState(); }

   bool IsStable() const override;

   bool HyperbolicOrbitAllowed() const;

   void Update(double aSimTime, const UtMeasurementData& aDataIn, UtMeasurementData& aDataOut) override;

   void NoDetectUpdate(double aSimTime, UtMeasurementData& aDataOut) override;

   void GetKinematicEstimate(double aSimTime, UtMeasurementData& aDataOut) override;

   const UtMatrixd* GetResidualCovarianceMatrix() override;

   const UtCovariance* GetResidualCovariance() override;

protected:
   bool InitializePropagators(const ut::OrbitalState& aInitialState);

   void UpdateMeasurementInputs(double aSimTime, const UtMeasurementData& aDataIn) override;

   void UpdateFilteredOutputs(double aSimTime, UtMeasurementData& aDataOut) override;

   void ComputeTransitionMatrix(double aDt) override;

   void ComputeProcessNoiseCovariance(double aDt) override;

   void ComputeCoordinateTransforms() override;

   void ComputePredictedState(double aDt) override;

   void ComputePredictedStateCovariance(double aDt) override;

private:
   void UpdateTime(double aSimTime);
   bool UpdateSigmaPoints(const UtCalendar& aEpoch);
   void PropagateSigmaPoints(const UtCalendar& aEpoch);
   void ComputePredictedFromSigmaPoints();
   bool ComputePredictedFromPropagatedState();

   bool                               mIsStable{false};
   bool                               mPropagatorsInitialized{false};
   UtCalendar                         mEpochSimTime;
   UtCalendar                         mStartSimTime;
   UtCalendar                         mLastEpochSimTime;
   UtVec3d                            mWCS_Location;
   UtVec3d                            mWCS_Velocity;
   UtCovariance                       mWCS_StateCovariance;
   UtCovariance                       mWCS_MeasurementCovariance;
   UtCovariance                       mWCS_ResidualCovariance;
   ut::CloneablePtr<UtECI_Conversion> mECI_ConversionPtr;
   double                             mTargetWCS_ToECI_Transform[3][3];
   double                             mTargetECI_ToNED_Transform[3][3];
   double                             mTargetECI_ToECS_Transform[3][3];

   UtMatrixd                                              mStateCovarianceDecomposition;
   UtMatrixd                                              mCovarianceSigmaPoints;
   UtMatrixd                                              mCovarianceSigmas;
   UtMatrixd                                              mCovarianceSigmasTranspose;
   bool                                                   mSigmaPointsValid;
   ut::CloneablePtr<UtOrbitalPropagatorBase>              mStatePropagatorPtr;
   std::vector<ut::CloneablePtr<UtOrbitalPropagatorBase>> mSigmaPointPropagators;
};

} // namespace TrackingFilters

#endif
