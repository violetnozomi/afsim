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

#include "KalmanFilter.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "UtEllipsoidalEarth.hpp"
#include "UtEntity.hpp"
#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMat3.hpp"
#include "UtMath.hpp"
#include "UtMatrix.hpp"
#include "UtMeasurementData.hpp"
#include "UtMeasurementUtil.hpp"
#include "UtVec3.hpp"


namespace
{
int aDim = 6;
};

namespace TrackingFilters
{


//! Static Definitions
int       KalmanFilter::sFilterNumber = 0;
UtMatrixd KalmanFilter::sIdentity6x6(6, 6);
double    KalmanFilter::sDefaultMeasurementNoiseR = 1000.0;
double    KalmanFilter::sDefaultMeasurementNoiseB = 1.0 * UtMath::cRAD_PER_DEG; // 1 degree.
double    KalmanFilter::sDefaultMeasurementNoiseE = 1.0 * UtMath::cRAD_PER_DEG; // 1 degree.

// =================================================================================================
KalmanFilter::KalmanFilter()
   : Filter()
   , mPass(0)
   , mNoiseXSqrd(0.0)
   , mNoiseYSqrd(0.0)
   , mNoiseZSqrd(0.0)
   , mNoiseModel(UtCovariance::cCONST_VEL)
   , mMeasurementNoiseR(0.0)
   , mMeasurementNoiseE(0.0)
   , mMeasurementNoiseB(0.0)
   , mStateTransitionMatrix(aDim, aDim)
   , // 6,6),
   mStateTransitionMatrixTranspose(aDim, aDim)
   , // 6,6),
   mMeasurementNoiseCovariance(3, 3)
   , mObservationMatrix(3, aDim)
   , mObservationMatrixTranspose(aDim, 3)
   , mProcessNoiseCovariance(aDim, aDim)
   , mFilteredState(aDim, 1)
   , mLastFilteredState(aDim, 1)
   , mInitialStatePtr(nullptr)
   , mNormalizedDistance(1, 1)
   , mAverageNormalizedDistance(0.0)
   , mPredictedState(aDim, 1)
   , mPredictedStateCovariance(aDim, aDim)
   , mStateCovariance(aDim, aDim)
   , mMeasurement(3, 1)
   , mResidual(3, 1)
   , mResidualCovariance(3, 3)
   , mResidualCovarianceInverse(3, 3)
   , mKalmanGains(aDim, 3)
   , mInitialized(false)
   , mIssueWarningMessage(true)
   , mDebugFlag(false)
   , mLastUpdateTime(0.0)
   , mFilterNumber(0)
   , mTemp6x6(aDim, aDim)
   , mTemp3x6(3, aDim)
   , mTemp6x3(aDim, 3)
   , mTemp6x1(aDim, 1)
   , mDistanceFunctionWindow()
   , mDistanceFunctionWeights()
{
   static bool once = true;
   if (once)
   {
      sIdentity6x6.LoadIdentity();
      once = false;
   }
   mObservationMatrix(0, 0) = 1.0;
   mObservationMatrix(1, 1) = 1.0;
   mObservationMatrix(2, 2) = 1.0;
   mObservationMatrixTranspose.Transpose(mObservationMatrix);

   // Off-diagonal elements are defined in UpdateTransitionMatrix().
   mStateTransitionMatrix.LoadIdentity();
   mStateTransitionMatrixTranspose.LoadIdentity();

   // Diagonal elements are defined in ComputeMeasurementCovarianceMatrix().
   // All off-diagonal elements are zero.
   mMeasurementNoiseCovariance.LoadIdentity();

   UtMat3d::Set(mTargetWCS_ToNED_Transform, 0.0);
   UtMat3d::Set(mTargetWCS_ToECS_Transform, 0.0);

   // TODO: For right now, use a sliding window of 5 elements
   // May want to get the M of N criteria for track continuation and use an
   // N element sliding window
   double alpha = 0.5;

   std::vector<double> weights;
   double              weightSum = 0.0;
   for (unsigned i = 0; i < 5; ++i)
   {
      // This value of alpha, combined with the window size of 5, computes a max value of
      // 21.954,
      // A value of 15.36 in a CHI-Square distribution with 3 DoF implies that 99.5 percent of
      // computed residual distances should be less than 12.837
      // Initialize with what we HOPE will be the worst values...
      mDistanceFunctionWindow.push_front(0.0);

      double weight = pow(alpha, double(i));
      weights.push_back(weight);
      weightSum += weight;
   }

   for (double weight : weights)
   {
      mDistanceFunctionWeights.push_back(weight / weightSum);
   }
}

// =================================================================================================
KalmanFilter::KalmanFilter(const KalmanFilter& aSrc)
   : Filter(aSrc)
   , mPass(0)
   , mNoiseXSqrd(aSrc.mNoiseXSqrd)
   , mNoiseYSqrd(aSrc.mNoiseYSqrd)
   , mNoiseZSqrd(aSrc.mNoiseZSqrd)
   , mNoiseModel(aSrc.mNoiseModel)
   , mMeasurementNoiseR(aSrc.mMeasurementNoiseR)
   , mMeasurementNoiseE(aSrc.mMeasurementNoiseE)
   , mMeasurementNoiseB(aSrc.mMeasurementNoiseB)
   , mStateTransitionMatrix(aSrc.mStateTransitionMatrix)
   , mStateTransitionMatrixTranspose(aSrc.mStateTransitionMatrixTranspose)
   , mMeasurementNoiseCovariance(aSrc.mMeasurementNoiseCovariance)
   , mObservationMatrix(aSrc.mObservationMatrix)
   , mObservationMatrixTranspose(aSrc.mObservationMatrixTranspose)
   , mProcessNoiseCovariance(aSrc.mProcessNoiseCovariance)
   , mFilteredState(aSrc.mFilteredState)
   , mLastFilteredState(aSrc.mLastFilteredState)
   , mInitialStatePtr(nullptr)
   , mNormalizedDistance(aSrc.mNormalizedDistance)
   , mAverageNormalizedDistance(aSrc.mAverageNormalizedDistance)
   , mPredictedState(aSrc.mPredictedState)
   , mPredictedStateCovariance(aSrc.mPredictedStateCovariance)
   , mStateCovariance(aSrc.mStateCovariance)
   , mMeasurement(aSrc.mMeasurement)
   , mResidual(aSrc.mResidual)
   , mResidualCovariance(aSrc.mResidualCovariance)
   , mResidualCovarianceInverse(aSrc.mResidualCovarianceInverse)
   , mKalmanGains(aSrc.mKalmanGains)
   , mInitialized(aSrc.mInitialized)
   , mIssueWarningMessage(true)
   , mDebugFlag(aSrc.mDebugFlag)
   , mLastUpdateTime(aSrc.mLastUpdateTime)
   , mFilterNumber(aSrc.mFilterNumber)
   , mTemp6x6(aSrc.mTemp6x6)
   , mTemp3x6(aSrc.mTemp3x6)
   , mTemp6x3(aSrc.mTemp6x3)
   , mDistanceFunctionWindow(aSrc.mDistanceFunctionWindow)
   , mDistanceFunctionWeights(aSrc.mDistanceFunctionWeights)
{
   UtMat3d::Set(mTargetWCS_ToNED_Transform, aSrc.mTargetWCS_ToNED_Transform);
   UtMat3d::Set(mTargetWCS_ToECS_Transform, aSrc.mTargetWCS_ToECS_Transform);

   if (aSrc.mInitialStatePtr != nullptr)
   {
      mInitialStatePtr = new UtMatrixd(*aSrc.mInitialStatePtr);
   }
}

// =================================================================================================
// virtual
KalmanFilter::~KalmanFilter()
{
   delete mInitialStatePtr;
}

// =================================================================================================
// virtual
Filter* KalmanFilter::Clone() const
{
   return new KalmanFilter(*this);
}

// =================================================================================================
// virtual
bool KalmanFilter::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "range_measurement_sigma")
   {
      aInput.ReadValueOfType(mMeasurementNoiseR, UtInput::cLENGTH);
   }
   else if (command == "elevation_measurement_sigma")
   {
      aInput.ReadValueOfType(mMeasurementNoiseE, UtInput::cANGLE);
   }
   else if (command == "bearing_measurement_sigma")
   {
      aInput.ReadValueOfType(mMeasurementNoiseB, UtInput::cANGLE);
   }
   else if (command == "process_noise_model")
   {
      std::string type;
      aInput.ReadValue(type);
      if (type == "constant_velocity")
      {
         mNoiseModel = UtCovariance::cCONST_VEL;
      }
      else if (type == "constant_acceleration")
      {
         mNoiseModel = UtCovariance::cCONST_ACC;
      }
      else
      {
         throw UtInput::BadValue(aInput);
      }
   }
   else if (command == "process_noise_sigmas_XYZ")
   {
      aInput.ReadValue(mNoiseXSqrd);
      aInput.ReadValue(mNoiseYSqrd);
      aInput.ReadValue(mNoiseZSqrd);
   }
   else if (command == "process_noise_sigmas")
   {
      aInput.ReadValueOfType(mNoiseXSqrd, UtInput::cACCELERATION);
      aInput.ReadValueOfType(mNoiseYSqrd, UtInput::cACCELERATION);
      aInput.ReadValueOfType(mNoiseZSqrd, UtInput::cACCELERATION);
   }
   else if (command == "debug") // Turn on the debug
   {
      if (!mDebugFlag)
      {
         InitializeDebug();
      }
      mDebugFlag = true;
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

void KalmanFilter::InitializeDebug()
{
   std::ofstream ofs("KFILT_DEBUG.out", std::ios::out);
   ofs << "% KALMAN FILTER GENERAL DEBUG" << std::endl;
   ofs << "% Time        Filt#          R              B              E              MX             MY             MZ  "
          "           ";
   ofs << "PX             PY             PZ             FX             FY             FZ             ";
   ofs << "FXD            FYD            FZD            ";
   ofs << "RESX           RESY           RESZ           NormDist       AvgNormDist    Pass";
   ofs << std::endl;
}

void TrackingFilters::KalmanFilter::InitializeFilter(double aSimTime)
{
   ++sFilterNumber;
   mFilterNumber = sFilterNumber;

   // Set the process noise sigmas in the process noise covariance.
   double variances[3] = {mNoiseXSqrd, mNoiseYSqrd, mNoiseZSqrd};
   mProcessNoiseCovariance.SetProcessNoiseVariancesECS(variances);
   mStateCovariance.SetProcessNoiseVariancesECS(variances);

   if (mInitialStatePtr != nullptr)
   {
      mFilteredState = *mInitialStatePtr;
   }
   else
   {
      mFilteredState(0) = mMeasurement(0);
      mFilteredState(1) = mMeasurement(1);
      mFilteredState(2) = mMeasurement(2);
      mFilteredState(3) = 0.0;
      mFilteredState(4) = 0.0;
      mFilteredState(5) = 0.0;
   }

   // Set the predicted position to the measurement and the predicted velocity to zero
   // This will be overridden the next filter pass
   mPredictedState = mFilteredState;
   mProcessNoiseCovariance.LoadIdentity();

   mStateCovariance.Fill(0);
   mStateCovariance.CopyFrom(mMeasurementNoiseCovariance, 0, 0, 3, 3);
   static const double cINITIAL_VELOCITY_VARIANCE = 1.0e+6;
   for (unsigned i = 3; i < 6; ++i)
   {
      mStateCovariance(i, i) = cINITIAL_VELOCITY_VARIANCE;
   }

   // Save new filtered data as old filter data for next pass
   mLastFilteredState = mFilteredState;
   mLastUpdateTime    = aSimTime;

   mInitialized = true;
}

void TrackingFilters::KalmanFilter::ComputeProcessNoiseCovariance(double aDt)
{
   if (IsStable())
   {
      mProcessNoiseCovariance.ComputeProcessNoise(aDt, mTargetWCS_ToECS_Transform, mNoiseModel);
   }
   else
   {
      mProcessNoiseCovariance.ComputeProcessNoise(aDt, mTargetWCS_ToNED_Transform, mNoiseModel);
   }
}

// =================================================================================================
//! Reset the filter.  The internal values will be re-initialized,
//! and the filter will have to re-stabilize based on new measurements.
// virtual
void KalmanFilter::Reset(double aSimTime, const UtMatrixd* aInitialStatePtr)
{
   Filter::Reset(aSimTime, aInitialStatePtr);
   mPass = 0;
   delete mInitialStatePtr;
   mInitialStatePtr = nullptr;
   if (aInitialStatePtr != nullptr)
   {
      mInitialStatePtr = new UtMatrixd(*aInitialStatePtr);
   }
   mLastUpdateTime = aSimTime;
   mInitialized    = false;
}

//! Compute the state transition matrix to advance the state by aDt
//! @param aDt The elapsed time since the last update of the filter.
// virtual
void TrackingFilters::KalmanFilter::ComputeTransitionMatrix(double aDt)
{
   // Form the transition matrix.
   mStateTransitionMatrix(0, 3) = aDt;
   mStateTransitionMatrix(1, 4) = aDt;
   mStateTransitionMatrix(2, 5) = aDt;
}

// =================================================================================================
// virtual
void KalmanFilter::Update(double aSimTime, const UtMeasurementData& aDataIn, UtMeasurementData& aDataOut)
{
   ++mPass;

   UpdateMeasurementInputs(aSimTime, aDataIn);

   if (!mInitialized)
   {
      InitializeFilter(aSimTime);
      if (mInitialStatePtr != nullptr)
      {
         // We have already provided valid data for initialization,
         // so we can also use the current input.
         UpdateFilter(aSimTime);
      }
   }
   else
   {
      UpdateFilter(aSimTime);
   }

   UpdateFilteredOutputs(aSimTime, aDataOut);

   if (mDebugFlag)
   {
      OutputDebug(aSimTime, aDataIn, aDataOut);
   }
}

// virtual
void KalmanFilter::OutputDebug(double aSimTime, const UtMeasurementData& aDataIn, UtMeasurementData& aDataOut)
{
   std::ofstream ofs("KFILT_DEBUG.out", std::ios::out | std::ios::app);
   ofs.setf(std::ios::left | std::ios::fixed | std::ios::showpoint);
   // clang-format off
   ofs << "  "
       << std::setw(12) << std::setprecision(3) << aSimTime
       << std::setw(15) << std::setprecision(0) << mFilterNumber
       << std::setw(15) << std::setprecision(5) << aDataIn.GetRange()
       << std::setw(15) << std::setprecision(6) << aDataIn.GetSensorAzimuth()   * UtMath::cDEG_PER_RAD
       << std::setw(15) << std::setprecision(6) << aDataIn.GetSensorElevation() * UtMath::cDEG_PER_RAD
       << std::setw(15) << std::setprecision(3) << mMeasurement(0)
       << std::setw(15) << std::setprecision(3) << mMeasurement(1)
       << std::setw(15) << std::setprecision(3) << mMeasurement(2)
       << std::setw(15) << std::setprecision(3) << mPredictedState(0)
       << std::setw(15) << std::setprecision(3) << mPredictedState(1)
       << std::setw(15) << std::setprecision(3) << mPredictedState(2)
       << std::setw(15) << std::setprecision(3) << mFilteredState(0)
       << std::setw(15) << std::setprecision(3) << mFilteredState(1)
       << std::setw(15) << std::setprecision(3) << mFilteredState(2)
       << std::setw(15) << std::setprecision(5) << mFilteredState(3)
       << std::setw(15) << std::setprecision(5) << mFilteredState(4)
       << std::setw(15) << std::setprecision(5) << mFilteredState(5)
       << std::setw(15) << std::setprecision(4) << mResidual(0)
       << std::setw(15) << std::setprecision(4) << mResidual(1)
       << std::setw(15) << std::setprecision(4) << mResidual(2)
       << std::setw(15) << std::setprecision(5) << mNormalizedDistance(0, 0)
       << std::setw(15) << std::setprecision(5) << mAverageNormalizedDistance
       << std::setw(15) << std::setprecision(0) << mPass << std::endl;
   // clang-format on
}

// virtual
//! Update the filter without measurement (see NoDetectUpdateFilter).
//! If the filter is not stable, GetKinematicEstimate is called.
//!@param aSimTime The current simulation time.
//!@param aDataOut Output data from the filter, including
//! location, velocity, and state covariances. The filter must be
//! stable in order to obtain valid location and covariance data in the output.
void KalmanFilter::NoDetectUpdate(double aSimTime, UtMeasurementData& aDataOut)
{
   if (mPass > 0)
   {
      if (IsStable())
      {
         NoDetectUpdateFilter(aSimTime);
         UpdateFilteredOutputs(aSimTime, aDataOut);
      }
      else
      {
         // Return a coasted Track Estimate.
         GetKinematicEstimate(aSimTime, aDataOut);
      }
   }
}

// virtual
void KalmanFilter::GetKinematicEstimate(double aSimTime, UtMeasurementData& aDataOut)
{
   // Update the State Transition Matrix
   double dt = aSimTime - mLastUpdateTime;
   if (dt > 0.0)
   {
      UpdateTransitionMatrix(dt);
   }

   // Extrapolate to current time state estimate based on the last
   // filtered state estimate
   mTemp6x1.Multiply(mStateTransitionMatrix, mLastFilteredState);
   double locationEstimateWCS[3];
   double velocityEstimateWCS[3];
   for (int i = 0; i < 3; ++i)
   {
      locationEstimateWCS[i] = mTemp6x1(i);
      velocityEstimateWCS[i] = mTemp6x1(i + 3);
   }

   aDataOut.SetLocationWCS(locationEstimateWCS);

   if (IsStable())
   {
      aDataOut.SetVelocityWCS(velocityEstimateWCS);

      UtCovariance predictedStateCovariance;
      GetPredictedStateCovariance(aSimTime, predictedStateCovariance);
      predictedStateCovariance.CopyTrackData(aDataOut);
      aDataOut.SetStateCovariance(aSimTime, predictedStateCovariance);
   }
}

// =================================================================================================
// virtual
void KalmanFilter::SetSphericalMeasurementErrors(const UtMeasurementData& aDataIn)
{
   // Must have nonzero measurement errors for all range, bearing, and elevation values.
   bool rbeErrorsValid = aDataIn.RangeValid() && (aDataIn.GetRangeError() > 0.0) &&
                         ((aDataIn.SensorAzimuthValid() && (aDataIn.GetSensorAzimuthError() > 0.0)) ||
                          (aDataIn.BearingValid() && (aDataIn.GetBearingError() > 0.0))) &&
                         ((aDataIn.SensorElevationValid() && (aDataIn.GetSensorElevationError() > 0.0)) ||
                          (aDataIn.ElevationValid() && (aDataIn.GetElevationError() > 0.0)));
   if (rbeErrorsValid)
   {
      mMeasurementNoiseR = aDataIn.GetRangeError();
      mMeasurementNoiseB = (aDataIn.SensorAzimuthValid() ? aDataIn.GetSensorAzimuthError() : aDataIn.GetBearingError());
      mMeasurementNoiseE =
         (aDataIn.SensorElevationValid() ? aDataIn.GetSensorElevationError() : aDataIn.GetElevationError());
   }
   CheckSphericalMeasurementErrors();
}

// =================================================================================================
void TrackingFilters::KalmanFilter::UpdateMeasurementInputs(double aSimTime, const UtMeasurementData& aDataIn)
{
   // Make a copy of the data so we can change it as necessary.
   // TODO rework so we don't have to copy (mjm).
   UtMeasurementData dataIn = aDataIn;

   //! If no measurement covariance is computed the filter will attempt to use
   //! range, bearing, elevation errors to compute one itself.
   if (!aDataIn.MeasurementCovarianceValid())
   {
      SetSphericalMeasurementErrors(dataIn);
      dataIn.SetRangeError(mMeasurementNoiseR);
      dataIn.SetSensorAzimuthError(mMeasurementNoiseB);
      dataIn.SetSensorElevationError(mMeasurementNoiseE);
   }

   if (dataIn.LocationValid())
   {
      const double* measuredLocationWCS = dataIn.GetLocationWCS().GetData();

      // Put measurement array into measurement vector
      mMeasurement(0) = measuredLocationWCS[0];
      mMeasurement(1) = measuredLocationWCS[1];
      mMeasurement(2) = measuredLocationWCS[2];
   }
   else
   {
      // rbe should be valid
      double locWCS[3];
      UtMeasurementUtil::LocationWCS(aDataIn.GetOriginatorLocationWCS().GetData(),
                                     aDataIn.GetRange(),
                                     aDataIn.GetBearing(),
                                     aDataIn.GetElevation(),
                                     locWCS);

      mMeasurement(0) = locWCS[0];
      mMeasurement(1) = locWCS[1];
      mMeasurement(2) = locWCS[2];
   }
   if (dataIn.MeasurementCovarianceValid() &&
       (dataIn.GetMeasurementCovariance() != nullptr)) // these checks should be redundant
   {
      mMeasurementNoiseCovariance = *dataIn.GetMeasurementCovariance();
   }
   else
   {
      // Set the initial state covariance equal to the measurement covariance, with high velocity variances.
      mMeasurementNoiseCovariance.ComputeMeasurementCovarianceWCS(dataIn);
   }
}

// =================================================================================================
void TrackingFilters::KalmanFilter::UpdateFilter(double aSimTime)
{
   double dt = aSimTime - mLastUpdateTime;
   if (dt >= 0.0) // only constraint on an update (with data) is to not go backwards in time.
   {
      // Update the State Transition Matrix
      UpdateTransitionMatrix(dt);

      ////Update the new PREDICTED state estmate, x_hat(k-1)
      //// x_hat(k) = PHI * x_hat(k-1)
      // mPredictedState.Multiply(mStateTransitionMatrix, mLastFilteredState);
      ComputePredictedState(dt);

      // v = measurement - H*x_hat(k)
      mResidual.Multiply(mObservationMatrix, mPredictedState); // H*x_hat(k)
      mResidual.Subtract(mMeasurement, mResidual);             // measurement - H*x_hat(k)

      // Update the PREDICTED State Covariance
      //  P(k) = PHI * P(k-1) * PHI' + Q
      ComputePredictedStateCovariance(dt);

      // Compute the Residual Covariance
      // S = H * P * H' + R
      mTemp3x6.Multiply(mObservationMatrix, mPredictedStateCovariance);    // H * P
      mResidualCovariance.Multiply(mTemp3x6, mObservationMatrixTranspose); // H * P * H'
      mResidualCovariance += mMeasurementNoiseCovariance;                  // H * P * H' + R

      // Compute Residual Covariance Inverse, S^(-1)
      mResidualCovarianceInverse.Invert(mResidualCovariance);

      // Compute the CHI-Square Track Score   d^2 = v' * S^(-1) * v
      ComputeTrackScore();

      // Update the Kalman Gains
      //  K = P * H' * S_inverse
      mTemp6x3.Multiply(mPredictedStateCovariance, mObservationMatrixTranspose); // P * H'
      mKalmanGains.Multiply(mTemp6x3, mResidualCovarianceInverse);               // P * H' * S_inverse

      // Compute the CORRECTED state estimate
      //  x_hat(k) = x_hat(k) + K * v
      mFilteredState.Multiply(mKalmanGains, mResidual); // K * v
      mFilteredState += mPredictedState;                // x_hat(k) + K * v

      // Compute the CORRECTED state covariance
      // P(k) = (I - K*H) * P(k)
      mTemp6x6.Multiply(mKalmanGains, mObservationMatrix);            // K*H
      mTemp6x6.Subtract(sIdentity6x6, mTemp6x6);                      // (I - K*H)
      mStateCovariance.Multiply(mTemp6x6, mPredictedStateCovariance); // (I - K*H) * P(k)

      // Save new filtered data as old filter data for next pass
      mLastFilteredState = mFilteredState;
      mLastUpdateTime    = aSimTime;
   }
}

//! Update the filter without observation.  The filtered state is set to the predicted state,
//! and the state covariance is set to be the predicted state covariance.
//! The filter is not permanently updated to aSimTime; rather, a subsequent call to
//! UpdateFilter() (from Update()) will have the same effect regardless of wether this method is called
//! (i.e., the output state is temporary).
void TrackingFilters::KalmanFilter::NoDetectUpdateFilter(double aSimTime)
{
   double dt = aSimTime - mLastUpdateTime;
   if (dt > 1.0E-8)
   {
      // Update the State Transition Matrix
      UpdateTransitionMatrix(dt);

      ////Update the new PREDICTED state estmate, x_hat(k-1)
      //// x_hat(k) = PHI * x_hat(k-1)
      ComputePredictedState(dt);

      // Set this to the filtered state.
      mFilteredState = mPredictedState;

      // Update the PREDICTED State Covariance
      //  P(k) = PHI * P(k-1) * PHI' + Q
      ComputePredictedStateCovariance(dt);

      // Set this to the state covariance.
      mStateCovariance = mPredictedStateCovariance;
   }
}

// =================================================================================================
void TrackingFilters::KalmanFilter::UpdateFilteredOutputs(double aSimTime, UtMeasurementData& aDataOut)
{
   double filteredLocation[3];

   // Copy the filtered state data to the output arguments
   filteredLocation[0] = mFilteredState(0);
   filteredLocation[1] = mFilteredState(1);
   filteredLocation[2] = mFilteredState(2);

   //-----------------------------------
   aDataOut.SetLocationWCS(filteredLocation);
   aDataOut.SetMeasurementCovariance(aSimTime, mMeasurementNoiseCovariance);
   aDataOut.SetUpdateTime(aSimTime);

   if (IsStable())
   {
      double filteredVelocity[3];
      filteredVelocity[0] = mFilteredState(3);
      filteredVelocity[1] = mFilteredState(4);
      filteredVelocity[2] = mFilteredState(5);

      aDataOut.SetVelocityWCS(filteredVelocity);
      aDataOut.SetStateCovariance(aSimTime, mStateCovariance);
   }
}

// =================================================================================================
void KalmanFilter::CheckSphericalMeasurementErrors()
{
   // Prevent low and no errors, or else P can drive towards zero and matrix invert can fail.
   static constexpr double THRESHOLD_MEASUREMENT_R = 0.001;
   static constexpr double THRESHOLD_MEASUREMENT_B = 1E-7;
   static constexpr double THRESHOLD_MEASUREMENT_E = 1E-7;

   // Must have nonzero measurement errors for all range, bearing, and elevation values.
   // Issue a warning message once if either range or bearing are zero.
   if (mMeasurementNoiseR < THRESHOLD_MEASUREMENT_R)
   {
      if (mMeasurementNoiseR == 0.0)
      {
         mMeasurementNoiseR = sDefaultMeasurementNoiseR;
         if (mIssueWarningMessage)
         {
            auto out = ut::log::warning() << "Kalman Filter: Range measurement error is zero.";
            out.AddNote() << "Did you set range_error_sigma / azimuth_error_sigma in sensor definition?";
            out.AddNote() << "Using default value.";
            mIssueWarningMessage = false;
         }
      }
      else
      {
         mMeasurementNoiseR = THRESHOLD_MEASUREMENT_R;
         if (mIssueWarningMessage)
         {
            auto out = ut::log::warning() << "Kalman Filter: Range measurement error below minimum threshold of "
                                          << THRESHOLD_MEASUREMENT_R << ".";
            out.AddNote() << "Using minimum threshold value.";
            mIssueWarningMessage = false;
         }
      }
   }

   if (mMeasurementNoiseB < THRESHOLD_MEASUREMENT_B)
   {
      if (mMeasurementNoiseB == 0.0)
      {
         mMeasurementNoiseB = sDefaultMeasurementNoiseB;
         if (mIssueWarningMessage)
         {
            auto out = ut::log::warning() << "Kalman Filter: Azimuth measurement error is zero.";
            out.AddNote() << "Did you set azimuth_error_sigma in sensor definition?";
            out.AddNote() << "Using default value.";
            mIssueWarningMessage = false;
         }
      }
      else
      {
         mMeasurementNoiseB = THRESHOLD_MEASUREMENT_B;
         if (mIssueWarningMessage)
         {
            auto out = ut::log::warning() << "Kalman Filter: Azimuth measurement error below minimum threshold of "
                                          << THRESHOLD_MEASUREMENT_B << ".";
            out.AddNote() << "Using minimum threshold value.";
            mIssueWarningMessage = false;
         }
      }
   }

   if (mMeasurementNoiseE < THRESHOLD_MEASUREMENT_E)
   {
      // Assume this is a range-bearing report;
      // use the bearing value as representative of the elevation sigma.
      mMeasurementNoiseE = sDefaultMeasurementNoiseE;
   }
}

// =================================================================================================
// virtual
bool KalmanFilter::IsStable() const
{
   // This is an arbitrary rule; might want to make this value an input (todo-mjm).
   return (mPass >= 3);
}

// =================================================================================================
// virtual
const UtMatrixd* KalmanFilter::GetStateCovarianceMatrix()
{
   return &mStateCovariance;
}

// =================================================================================================
// virtual
const UtMatrixd* KalmanFilter::GetResidualCovarianceMatrix()
{
   return &mResidualCovariance;
}

// =================================================================================================
// virtual
bool KalmanFilter::GetPredictedStateCovariance(double aSimTime, UtCovariance& aCovariance)
{
   double dt      = aSimTime - mLastUpdateTime;
   bool   success = (dt >= 0.0) && IsStable();
   if (success)
   {
      if (dt > 0.0)
      {
         ////Update the PREDICTED State Covariance
         //// P(k) = PHI * P(k-1) * PHI' + Q
         ComputePredictedStateCovariance(dt);
      }
      aCovariance = mPredictedStateCovariance;
   }
   return success;
}

// =================================================================================================
// virtual
bool KalmanFilter::GetPredictedStateCovarianceMatrix(double aSimTime, UtMatrixd& aMatrix)
{
   bool success = false;
   if (IsStable())
   {
      success   = true;
      double dt = aSimTime - mLastUpdateTime;
      if (dt > 0.0)
      {
         ComputePredictedStateCovariance(dt);
      }
      aMatrix = mPredictedStateCovariance;
   }
   return success;
}

// =================================================================================================
// virtual
bool KalmanFilter::GetPredictedResidualCovarianceMatrix(double aSimTime, UtMatrixd& aMatrix)
{
   bool success = false;
   if (IsStable())
   {
      success   = true;
      double dt = aSimTime - mLastUpdateTime;
      if (dt > 0.0)
      {
         // Compute the Residual Covariance
         // S = H * P * H' + R
         mTemp3x6.Multiply(mObservationMatrix, mPredictedStateCovariance);    // H * P
         mResidualCovariance.Multiply(mTemp3x6, mObservationMatrixTranspose); // H * P * H'
         mResidualCovariance += mMeasurementNoiseCovariance;                  // H * P * H' + R
      }
      aMatrix = mResidualCovariance;
   }
   return success;
}

// =================================================================================================
// virtual
bool KalmanFilter::GetCurrentMeasurementTrackScore(double& aCurrentScore)
{
   aCurrentScore = mNormalizedDistance(0, 0);
   return true;
}

// =================================================================================================
// virtual
bool KalmanFilter::GetAverageTrackScore(double& aAverageScore)
{
   aAverageScore = mAverageNormalizedDistance;
   return true;
}

// =================================================================================================
void KalmanFilter::SetFilterProcessNoise(const double aXYZ[3])
{
   mNoiseXSqrd = aXYZ[0];
   mNoiseYSqrd = aXYZ[1];
   mNoiseZSqrd = aXYZ[2];
}

// =================================================================================================
//! Compute the CHI-Square Track Score (Normalized Residual Squared)
//! Used to compute track quality and for adjusting filter process noise in the
//! case of a maneuvering target (MDAR)
// private
void KalmanFilter::ComputeTrackScore()
{
   // d^2 = residual_transpose * residual_covariance_inverse * residual
   // d^2 = v' * S^(-1) * v
   UtMatrixd temp1x3(1, 3);
   UtMatrixd residualTranspose(1, 3);
   residualTranspose.Transpose(mResidual);

   temp1x3.Multiply(residualTranspose, mResidualCovarianceInverse);
   mNormalizedDistance.Multiply(temp1x3, mResidual);

   // Add the track score to the sliding window
   // Make room for the new one by removing the one at the end
   mDistanceFunctionWindow.pop_back();

   // Add the new one.
   mDistanceFunctionWindow.push_front(mNormalizedDistance(0, 0));

   mAverageNormalizedDistance = ComputeAverageTrackScore();
}

// =================================================================================================
// Weighted average of the current mNormalizedDistance values in the window.
// private
double KalmanFilter::ComputeAverageTrackScore()
{
   // Compute the weighted average of the 5 most recent normalized distances

   size_t windowSize = mDistanceFunctionWindow.size();
   double sum        = 0.0;
   for (size_t i = 0; i < windowSize; ++i)
   {
      sum += (mDistanceFunctionWeights[i] * mDistanceFunctionWindow[i]);
   }
   return sum;
}

// =================================================================================================
//! Update the State Transition Matrix of the filter based on the given time increment.
//! Calling this method will not affect subsequent updates or no detect updates.
// private
void KalmanFilter::UpdateTransitionMatrix(double aDt)
{
   ComputeTransitionMatrix(aDt);

   // Compute the transpose of the state transition matrix.
   mStateTransitionMatrixTranspose.Transpose(mStateTransitionMatrix);
}

// =================================================================================================
// protected virtual
//! Compute necessary coordinate transforms necessary to compute filter states and covariances.
void KalmanFilter::ComputeCoordinateTransforms()
{
   assert(mPass >= 1);
   double locWCS[3];
   mPredictedState.CopyTo(locWCS, 3, 0);
   UtCovariance::ComputeWCS_ToNED_Transform(locWCS, mTargetWCS_ToNED_Transform);

   if (IsStable()) // We will have a valid velocity estimate.
   {
      double velWCS[3];
      mPredictedState.CopyTo(velWCS, 3, 3);
      UtCovariance::ComputeWCS_ToECS_Transform(velWCS, mTargetWCS_ToNED_Transform, mTargetWCS_ToECS_Transform);
   }
}

// protected virtual
//! Compute the predicted state of the filter at some time in the future.
//! @param aDt The change in time from mLastUpdateTime until the time of the predicted state.
void TrackingFilters::KalmanFilter::ComputePredictedState(double aDt)
{
   // Update the new PREDICTED state estmate, x_hat(k)
   //  x_hat(k) = PHI * x_hat(k-1)
   mPredictedState.Multiply(mStateTransitionMatrix, mLastFilteredState);
}

// protected virtual
//! Compute the predicted state covariance of the filter at some time in the future.
//! @param aDt The change in time from mLastUpdateTime until the time of the predicted state.
void TrackingFilters::KalmanFilter::ComputePredictedStateCovariance(double aDt)
{
   // The coordinate transforms must be updated before the predicted state covariance matrix
   // is computed (but, after the predicted state is computed).
   // It uses the predicted state to best approximate where to compute the
   // process noise covariance.
   ComputeCoordinateTransforms();

   // Update the ProcessNoiseCovariance, Q
   ComputeProcessNoiseCovariance(aDt);

   // Update the PREDICTED State Covariance
   //  P(k) = PHI * P(k-1) * PHI' + Q
   mTemp6x6.Multiply(mStateTransitionMatrix, mStateCovariance);                   // PHI * P(k-1)
   mPredictedStateCovariance.Multiply(mTemp6x6, mStateTransitionMatrixTranspose); // PHI * P(k-1) * PHI'
   mPredictedStateCovariance += mProcessNoiseCovariance;
}

} // namespace TrackingFilters
