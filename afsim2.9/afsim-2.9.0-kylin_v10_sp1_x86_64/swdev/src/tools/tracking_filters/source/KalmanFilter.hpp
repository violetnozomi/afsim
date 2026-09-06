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

#ifndef TRACKING_FILTERS_KALMANFILTER_HPP
#define TRACKING_FILTERS_KALMANFILTER_HPP

#include "tracking_filters_export.h"

#include "Filter.hpp"
#include "UtCovariance.hpp"
class UtInput;
#include <deque>
#include <vector>

#include "UtMatrix.hpp"

namespace TrackingFilters
{
//! Implementation of a Coupled, Linear 6-State Kalman Filter.
//! These filtered states are WCS location (3 states)
//! and WCS velocity (3 states).
class TRACKING_FILTERS_EXPORT KalmanFilter : public Filter
{
public:
   KalmanFilter();
   KalmanFilter(const KalmanFilter& aSrc);
   ~KalmanFilter() override;

   Filter* Clone() const override;

   bool ProcessInput(UtInput& aInput) override;

   void Reset(double aSimTime, const UtMatrixd* aInitialStatePtr = nullptr) override;

   void Update(double aSimTime, const UtMeasurementData& aDataIn, UtMeasurementData& aDataOut) override;

   void NoDetectUpdate(double aSimTime, UtMeasurementData& aDataOut) override;

   void GetKinematicEstimate(double aSimTime, UtMeasurementData& aDataOut) override;

   virtual const UtMatrixd* GetStateCovarianceMatrix();

   const UtCovariance* GetStateCovariance() override { return &mStateCovariance; }

   virtual const UtMatrixd* GetResidualCovarianceMatrix();

   const UtCovariance* GetResidualCovariance() override { return &mResidualCovariance; }

   bool IsInitialized() { return mInitialized; }

   bool IsStable() const override;

   bool GetPredictedStateCovariance(double aSimTime, UtCovariance& aCovariance) override;

   virtual bool GetPredictedStateCovarianceMatrix(double aSimTime, UtMatrixd& aMatrix);

   virtual bool GetPredictedResidualCovarianceMatrix(double aSimTime, UtMatrixd& aMatrix);

   bool GetCurrentMeasurementTrackScore(double& aCurrentScore) override;

   bool GetAverageTrackScore(double& aAverageScore) override;

   void SetFilterProcessNoise(const double aXYZ[3]);

   UtCovariance& GetProcessNoiseCovariance() { return mProcessNoiseCovariance; }

protected:
   virtual void InitializeDebug();

   virtual void InitializeFilter(double aSimTime);

   virtual void OutputDebug(double aSimTime, const UtMeasurementData& aDataIn, UtMeasurementData& aDataOut);

   virtual void SetSphericalMeasurementErrors(const UtMeasurementData& aData);

   virtual void ComputeTransitionMatrix(double aDt);

   virtual void ComputeProcessNoiseCovariance(double aDt);

   virtual void ComputeCoordinateTransforms();

   virtual void ComputePredictedState(double aDt);

   virtual void ComputePredictedStateCovariance(double aDt);

   //! Number of 'Pass' thru the Filter
   int mPass;

   //! Process Noise Elements
   double mNoiseXSqrd;
   double mNoiseYSqrd;
   double mNoiseZSqrd;

   //! Instances of the above enumerated types
   UtCovariance::NOISE_MODEL mNoiseModel;

   // Measurement noise elements
   double mMeasurementNoiseR;
   double mMeasurementNoiseE;
   double mMeasurementNoiseB;

   // Transformation Matrices
   double mTargetWCS_ToNED_Transform[3][3];
   double mTargetWCS_ToECS_Transform[3][3];

   //!
   //! Targaet State Dynamics Model Matrix - PHI
   UtMatrixd mStateTransitionMatrix;

   //! Transpose of the State Dynamics Model Matrix - transpose(PHI)
   UtMatrixd mStateTransitionMatrixTranspose;

   //! Transformed Spherical -> Cartesian Measurement Noise - R
   UtCovariance mMeasurementNoiseCovariance;

   //! Sensor Observation Matrix - H
   UtMatrixd mObservationMatrix;

   //! Transpose of Sensor Observation Matrix - transpose(H)
   UtMatrixd mObservationMatrixTranspose;

   //! Covariance Matrix of the Target Dynamics Noise - Q
   UtCovariance mProcessNoiseCovariance;

   //! 6-Element Filtered State Vector
   UtMatrixd mFilteredState;

   //! Previous Filtered State Computed on the last valid update
   UtMatrixd mLastFilteredState;

   //! Initial state if provided.
   UtMatrixd* mInitialStatePtr;

   //! CHI-Squared Track Score. Normalized Residual Squared
   UtMatrixd mNormalizedDistance;

   //! Average CHI-Squared Track Score
   double mAverageNormalizedDistance;

   //! Predicted Target State
   UtMatrixd mPredictedState;

   //! Covariance of the Predicted State - P-hat
   UtMatrixd mPredictedStateCovariance;

   //! Covariance of the Filtered State - P
   UtCovariance mStateCovariance;

   //! Input WCS Measurement Vector
   UtMatrixd mMeasurement;

   //! Residual Vector (Predicted - Measurement)
   UtMatrixd mResidual;

   //! Covariance of the Residual - S
   UtCovariance mResidualCovariance;

   //! The inverse of the residual covariance matrix - S^-1
   UtCovariance mResidualCovarianceInverse;

   //! Kalman Gains Matrix - K
   UtMatrixd mKalmanGains;

   //! Filter Initialized Flag
   bool mInitialized;

   //! Issue a warning message once if either range or bearing measurement
   //! sigmas are zero when SetSphericalMeasurementErrors is called.
   bool mIssueWarningMessage;

   //! Filter Debug Data Output Flag
   bool mDebugFlag;

   //! Last Filter Update Time
   double mLastUpdateTime;

   //! Filter Instantiation Number (Useful for analyzing debug with multiple targets)
   int mFilterNumber;

   //! Static version of above
   static int sFilterNumber;

   //! A static 6x6 identity matrix
   static UtMatrixd sIdentity6x6;

   //! The default measurement noise
   static double sDefaultMeasurementNoiseR;
   static double sDefaultMeasurementNoiseB;
   static double sDefaultMeasurementNoiseE;

   //! Temporary matrices used during Update().
   UtMatrixd mTemp6x6;
   UtMatrixd mTemp3x6;
   UtMatrixd mTemp6x3;
   UtMatrixd mTemp6x1;

   std::deque<double>  mDistanceFunctionWindow;
   std::vector<double> mDistanceFunctionWeights;

   virtual void UpdateMeasurementInputs(double aSimTime, const UtMeasurementData& aDataIn);

   virtual void UpdateFilteredOutputs(double aSimTime, UtMeasurementData& aDataOut);

   void UpdateTransitionMatrix(double aDt);

   void UpdateFilter(double aSimTime);

   void NoDetectUpdateFilter(double aSimTime);

private:
   void CheckSphericalMeasurementErrors();

   double ComputeAverageTrackScore();

   void ComputeTrackScore();
};
} // namespace TrackingFilters
#endif
