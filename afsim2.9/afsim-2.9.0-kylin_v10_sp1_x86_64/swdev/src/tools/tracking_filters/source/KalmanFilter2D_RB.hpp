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

#ifndef TRACKING_FILTERS_KALMANFILTER2D_RB_HPP
#define TRACKING_FILTERS_KALMANFILTER2D_RB_HPP

#include "tracking_filters_export.h"

#include "Filter.hpp"
class UtInput;
#include "UtMatrix.hpp"

namespace TrackingFilters
{
//! Implementation of a Coupled, Linear 6-State Kalman Filter

class TRACKING_FILTERS_EXPORT KalmanFilter2D_RB : public Filter
{
public:
   KalmanFilter2D_RB();
   KalmanFilter2D_RB(const KalmanFilter2D_RB& aSrc);
   ~KalmanFilter2D_RB() override = default;

   Filter* Clone() const override;

   bool ProcessInput(UtInput& aInput) override;

   void Reset(double aSimTime, const UtMatrixd* aInitialStatePtr = nullptr) override;

   void Update(double aSimTime, const UtMeasurementData& aDataIn, UtMeasurementData& aDataOut) override;

   void NoDetectUpdate(double aSimTime, UtMeasurementData& aDataOut) override;

   void GetKinematicEstimate(double aSimTime, UtMeasurementData& aDataOut) override;

   virtual const UtMatrixd* GetStateCovarianceMatrix();

   bool IsInitialized() { return mInitialized; }

   bool IsStable() const override;

   void SetFilterProcessNoise(const double aXYZ[3]);

private:
   virtual void InitializeDebug();
   virtual void OutputDebug(double aSimTime, const UtMeasurementData& aDataIn, UtMeasurementData& aDataOut);

   virtual void Update(double       aSimTime,
                       const double aSensorLocation[3],
                       const double aMeasuredLocationWCS[3],
                       double       aFilteredLocation[3],
                       double       aFilteredVelocity[3]);

   virtual void NoDetectUpdate(double aSimTime, double aFilteredLocation[3], double aFilteredVelocity[3]);

   void GetKinematicEstimate(double aSimTime, double aLocationEstimate[3], double aVelocityEstimate[3]);

   void SetSphericalMeasurementErrors(double aRangeMeasurementError,
                                      double aBearingMeasurementError,
                                      double aElevationMeasurementError);

   //! Spherical to Cartesian Transformation of Sensor Measurement Errors
   void ComputeMeasurementCovarianceMatrix();

   //! Compute the Range, Bearing, and Elevation Measurement from WCS Measurement Input
   void ComputeRB(const double aSensorLocation[3], const double aMeasuredLocation[3], double aRB[2]);

   //! Update the Target Dynamics Model Matrix
   void UpdateTransitionMatrix(double aDt);

   //! Update the Target Dynamics Noise Model Matrix
   void UpdateProcessNoiseCovariance(double aDt);

   //! Initialization for the Target Dynamics Noise Model Matrix
   void InitializeProcessNoiseCovariance();

   //! NOT USED YET
   //! Fading Memory Window used for Maneuver Detection/Maneuver Response within the Filter
   bool UpdateTrackQualList(double aSimTime, double aDistSQ);

   //! Enumerated Types to Control the Amount of Process Noise Injected into the Estimates
   enum NOISE_MODEL
   {
      cCONST_VEL,
      cCONST_ACC
   };

   //! Number of 'Pass' through the Filter
   int mPass;

   //! Process Noise Elements
   double mNoiseXSqrd;
   double mNoiseYSqrd;

   //! Instances of the above enumerated types
   NOISE_MODEL mNoiseModel;

   // Measurement noise elements
   double mMeasurementNoiseR;
   double mMeasurementNoiseB;

   //!
   //! Target State Dynamics Model Matrix - PHI
   UtMatrixd mStateTransitionMatrix;

   //! Transpose of the State Dynamics Model Matrix - transpose(PHI)
   UtMatrixd mStateTransitionMatrixTranspose;

   //! Transformed Spherical -> Cartesian Measurement Noise - R
   UtMatrixd mMeasurementNoiseCovariance;

   //! Sensor Observation Matrix - H
   UtMatrixd mObservationMatrix;

   //! Transpose of Sensor Observation Matrix - transpose(H)
   UtMatrixd mObservationMatrixTranspose;

   //! Covariance Matrix of the Target Dynamics Noise - Q
   UtMatrixd mProcessNoiseCovariance;

   //! 4-Element Filtered State Vector
   UtMatrixd mFilteredStateNE;

   //! 6-Element Filtered State Vector
   UtMatrixd mFilteredStateWCS;

   //! Previous Filtered State Computed on the last valid update
   UtMatrixd mLastFilteredStateNE;

   //! Predicted Target State
   UtMatrixd mPredictedStateNE;

   //! Covariance of the Predicted State - P-hat
   UtMatrixd mPredictedStateCovariance;

   //! Covariance of the Filtered State - P
   UtMatrixd mStateCovariance;

   //! Input WCS Measurement Vector
   UtMatrixd mMeasurement;

   //! Residual Vector (Predicted - Measurement)
   UtMatrixd mResidual;

   //! Covariance of the Residual - S
   UtMatrixd mResidualCovariance;

   //! The inverse of the residual covariance matrix - S^-1
   UtMatrixd mResidualCovarianceInverse;

   //! Kalman Gains Matrix - K
   UtMatrixd mKalmanGains;

   //! Filter Initialized Flag
   bool mInitialized;

   //! Filter Debug Data Output Flag
   bool mDebugFlag;

   //! Measurement Range
   double mRange;

   //! Measurement Bearing
   double mBearing;

   //! Last Filter Update Time
   double mLastUpdateTime;

   //! Filter Instantiation Number (Useful for analyzing debug with multiple targets)
   int mFilterNumber;

   //! Static version of above
   static int mStaticFilterNumber;

   //! A static 6x6 identity matrix
   static UtMatrixd sIdentity4x4;

   //! Temporary matrices used during Update().
   UtMatrixd mTemp4x4;
   UtMatrixd mTemp2x4;
   UtMatrixd mTemp4x2;
};
} // namespace TrackingFilters
#endif
