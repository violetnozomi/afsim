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

#include "KalmanFilter2D_RB.hpp"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "UtEntity.hpp"
#include "UtInput.hpp"
#include "UtMath.hpp"
#include "UtMatrix.hpp"
#include "UtMeasurementData.hpp"
#include "UtVec3.hpp"

namespace TrackingFilters
{

int KalmanFilter2D_RB::mStaticFilterNumber = 0;

UtMatrixd KalmanFilter2D_RB::sIdentity4x4(4, 4);

KalmanFilter2D_RB::KalmanFilter2D_RB()
   : mPass(0)
   , mNoiseXSqrd(0.0)
   , mNoiseYSqrd(0.0)
   , mNoiseModel(cCONST_VEL)
   , mMeasurementNoiseR(0.0)
   , mMeasurementNoiseB(0.0)
   , mStateTransitionMatrix(4, 4)
   , mStateTransitionMatrixTranspose(4, 4)
   , mMeasurementNoiseCovariance(2, 2)
   , mObservationMatrix(2, 4)
   , mObservationMatrixTranspose(4, 2)
   , mProcessNoiseCovariance(4, 4)
   , mFilteredStateNE(4, 1)
   , mFilteredStateWCS(6, 1)
   , mLastFilteredStateNE(4, 1)
   , mPredictedStateNE(4, 1)
   , mPredictedStateCovariance(4, 4)
   , mStateCovariance(4, 4)
   , mMeasurement(2, 1)
   , mResidual(2, 1)
   , mResidualCovariance(2, 2)
   , mResidualCovarianceInverse(2, 2)
   , mKalmanGains(4, 2)
   , mInitialized(false)
   , mRange(0.0)
   , mBearing(0.0)
   , mLastUpdateTime(0.0)
   , mFilterNumber(0)
   , mTemp4x4(4, 4)
   , mTemp2x4(2, 4)
   , mTemp4x2(4, 2)
{
   sIdentity4x4.LoadIdentity();
   mObservationMatrix(0, 0) = 1.0;
   mObservationMatrix(1, 1) = 1.0;
   mObservationMatrixTranspose.Transpose(mObservationMatrix);

   // Off-diagonal elements are defined in UpdateTransitionMatrix().
   mStateTransitionMatrix.LoadIdentity();
   mStateTransitionMatrixTranspose.LoadIdentity();

   // Diagonal elements are defined in ComputeMeasurementCovarianceMatrix().
   // All off-diagonal elements are zero.
   mMeasurementNoiseCovariance.LoadIdentity();
}

KalmanFilter2D_RB::KalmanFilter2D_RB(const KalmanFilter2D_RB& aSrc)
   : Filter(aSrc)
   , mPass(0)
   , mNoiseXSqrd(aSrc.mNoiseXSqrd)
   , mNoiseYSqrd(aSrc.mNoiseYSqrd)
   , mNoiseModel(cCONST_VEL)
   , mMeasurementNoiseR(aSrc.mMeasurementNoiseR)
   , mMeasurementNoiseB(aSrc.mMeasurementNoiseB)
   , mStateTransitionMatrix(aSrc.mStateTransitionMatrix)
   , mStateTransitionMatrixTranspose(aSrc.mStateTransitionMatrixTranspose)
   , mMeasurementNoiseCovariance(aSrc.mMeasurementNoiseCovariance)
   , mObservationMatrix(aSrc.mObservationMatrix)
   , mObservationMatrixTranspose(aSrc.mObservationMatrixTranspose)
   , mProcessNoiseCovariance(aSrc.mProcessNoiseCovariance)
   , mFilteredStateNE(aSrc.mFilteredStateNE)
   , mFilteredStateWCS(aSrc.mFilteredStateWCS)
   , mLastFilteredStateNE(aSrc.mLastFilteredStateNE)
   , mPredictedStateNE(aSrc.mPredictedStateNE)
   , mPredictedStateCovariance(aSrc.mPredictedStateCovariance)
   , mStateCovariance(aSrc.mStateCovariance)
   , mMeasurement(aSrc.mMeasurement)
   , mResidual(aSrc.mResidual)
   , mResidualCovariance(aSrc.mResidualCovariance)
   , mResidualCovarianceInverse(aSrc.mResidualCovarianceInverse)
   , mKalmanGains(aSrc.mKalmanGains)
   , mInitialized(aSrc.mInitialized)
   , mDebugFlag(aSrc.mDebugFlag)
   , mRange(aSrc.mRange)
   , mBearing(aSrc.mBearing)
   , mLastUpdateTime(aSrc.mLastUpdateTime)
   , mFilterNumber(aSrc.mFilterNumber)
   , mTemp4x4(aSrc.mTemp4x4)
   , mTemp2x4(aSrc.mTemp2x4)
   , mTemp4x2(aSrc.mTemp4x2)
{
}

Filter* KalmanFilter2D_RB::Clone() const
{
   return new KalmanFilter2D_RB(*this);
}

bool KalmanFilter2D_RB::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "range_measurement_sigma")
   {
      aInput.ReadValueOfType(mMeasurementNoiseR, UtInput::cLENGTH);
   }
   else if (command == "bearing_measurement_sigma")
   {
      aInput.ReadValueOfType(mMeasurementNoiseB, UtInput::cANGLE);
   }
   else if (command == "process_noise_sigmas_XY")
   {
      aInput.ReadValue(mNoiseXSqrd);
      aInput.ReadValue(mNoiseYSqrd);
   }
   else if (command == "debug")
   {
      if (!mDebugFlag)
      {
         InitializeDebug();
      }
      mDebugFlag = true;
   }
   else
   {
      myCommand = Filter::ProcessInput(aInput);
   }

   return myCommand;
}

// virtual
void KalmanFilter2D_RB::InitializeDebug()
{
   std::ofstream ofs("KFILT_DEBUG.out", std::ios::out);
   ofs << "% KALMAN FILTER GENERAL DEBUG" << std::endl;
   ofs << "% Time        Filt#          R              B              E              ME             MN             MU  "
          "           ";
   ofs << "PE             PN             PU             FE             FN             FU             ";
   ofs << "FXD            FYD            FZD            ";
   ofs << "RESX           RESY           RESZ           ";
   ofs << "sqD            RESRng         RESel          RESbgy         sqDSpherical   Pass";
   ofs << std::endl;
}

void KalmanFilter2D_RB::Reset(double aSimTime, const UtMatrixd* aInitialStatePtr)
{
   Filter::Reset(aSimTime, aInitialStatePtr);
   mLastUpdateTime = aSimTime;
   mPass           = 0;
}

// virtual
void KalmanFilter2D_RB::OutputDebug(double aSimTime, const UtMeasurementData& aDataIn, UtMeasurementData& aDataOut)
{
   std::ofstream ofs("KFILT_DEBUG.out", std::ios::out | std::ios::app);
   ofs.setf(std::ios::left | std::ios::fixed | std::ios::showpoint);
   double RB[3];
   double sensorLocation[3];
   aDataIn.GetOriginatorLocationWCS(sensorLocation);

   const double* measuredLocationWCS = aDataIn.GetLocationWCS().GetData();
   const double* filteredLocation    = aDataOut.GetLocationWCS().GetData();
   ComputeRB(sensorLocation, filteredLocation, RB);
   // clang-format off
   ofs << "  "
       << std::setw(12) << std::setprecision(3) << aSimTime
       << std::setw(15) << std::setprecision(0) << mFilterNumber
       << std::setw(15) << std::setprecision(5) << mRange
       << std::setw(15) << std::setprecision(5) << mBearing
       << std::setw(15) << std::setprecision(5) << RB[0]
       << std::setw(15) << std::setprecision(5) << RB[1]
       << std::setw(15) << std::setprecision(4) << measuredLocationWCS[0]
       << std::setw(15) << std::setprecision(4) << measuredLocationWCS[1]
       << std::setw(15) << std::setprecision(4) << measuredLocationWCS[2]
       << std::setw(15) << std::setprecision(4) << filteredLocation[0]
       << std::setw(15) << std::setprecision(4) << filteredLocation[1]
       << std::setw(15) << std::setprecision(4) << filteredLocation[2]
       << std::setw(15) << std::setprecision(3) << mMeasurement(0, 0)
       << std::setw(15) << std::setprecision(3) << mMeasurement(1, 0)
       << std::setw(15) << std::setprecision(3) << mPredictedStateNE(0, 0)
       << std::setw(15) << std::setprecision(3) << mPredictedStateNE(1, 0)
       << std::setw(15) << std::setprecision(3) << mFilteredStateNE(0, 0)
       << std::setw(15) << std::setprecision(3) << mFilteredStateNE(1, 0)
       << std::setw(15) << std::setprecision(3) << mFilteredStateNE(2, 0)
       << std::setw(15) << std::setprecision(5) << mFilteredStateNE(3, 0)
       << std::setw(15) << std::setprecision(0) << mPass << std::endl;
   // clang-format on
}

// =================================================================================================
// virtual
void KalmanFilter2D_RB::Update(double aSimTime, const UtMeasurementData& aDataIn, UtMeasurementData& aDataOut)
{
   SetSphericalMeasurementErrors(aDataIn.GetRangeError(), aDataIn.GetBearingError(), aDataIn.GetElevationError());
   double sensorLocationWCS[3];
   aDataIn.GetOriginatorLocationWCS(sensorLocationWCS);
   double filteredLocationWCS[3];
   double filteredVelocityWCS[3];
   Update(aSimTime, sensorLocationWCS, aDataIn.GetLocationWCS().GetData(), filteredLocationWCS, filteredVelocityWCS);
   aDataOut.SetLocationWCS(filteredLocationWCS);
   aDataOut.SetVelocityWCS(filteredVelocityWCS);
   aDataOut.SetUpdateTime(aSimTime);

   if (mDebugFlag)
   {
      OutputDebug(aSimTime, aDataIn, aDataOut);
   }

   aDataOut.SetWCS_LocationValid(true);
   if (IsStable())
   {
      aDataOut.SetWCS_VelocityValid(true);
   }
}

// Update the track when new measurement data are received
void KalmanFilter2D_RB::Update(double       aSimTime,
                               const double aSensorLocation[3],
                               const double aMeasuredLocationWCS[3],
                               double       aFilteredLocation[3],
                               double       aFilteredVelocity[3])
{
   ++mPass;
   // Use the WCS Measurement to compute a RBE
   // Measurement.
   // Dumb-down the measurement so that only
   // Range & Bearing are used
   double tempRB[2];
   ComputeRB(aSensorLocation, aMeasuredLocationWCS, tempRB);

   mRange   = tempRB[0];
   mBearing = tempRB[1];

   // Compute North and East Components
   mMeasurement(0, 0) = mRange * cos(mBearing);
   mMeasurement(1, 0) = mRange * sin(mBearing);

   if (mPass == 1) // First Pass Through the Filter
   {
      ++mStaticFilterNumber;
      mFilterNumber = mStaticFilterNumber;

      mFilteredStateNE(0, 0) = mMeasurement(0, 0);
      mFilteredStateNE(1, 0) = mMeasurement(1, 0);
      mFilteredStateNE(2, 0) = 0.0;
      mFilteredStateNE(3, 0) = 0.0;

      // Set the predicted position to the measurement and the predicted velocity to zero
      // This will be overridden the next filter pass
      mPredictedStateNE = mFilteredStateNE;

      // Initialize Q
      InitializeProcessNoiseCovariance();

      // P = Q
      mStateCovariance = mProcessNoiseCovariance;
   }
   else // Filter is initialized
   {
      double dt = aSimTime - mLastUpdateTime;

      // Update the State Transition Matrix
      UpdateTransitionMatrix(dt);

      // Update the new PREDICTED state estimate, x_hat(k-1)
      //  x_hat(k) = PHI * x_hat(k-1)
      mPredictedStateNE.Multiply(mStateTransitionMatrix, mLastFilteredStateNE);

      // v = measurement - H*x_hat(k)
      mResidual.Multiply(mObservationMatrix, mPredictedStateNE); // H*x_hat(k)
      mResidual.Subtract(mMeasurement, mResidual);               // measurement - H*x_hat(k)

      // Update the ProcessNoiseCovariance, Q
      UpdateProcessNoiseCovariance(dt);

      // Update the PREDICTED State Covariance
      //  P(k) = PHI * P(k-1) * PHI' + Q
      mTemp4x4.Multiply(mStateTransitionMatrix, mStateCovariance);                   // PHI * P(k-1)
      mPredictedStateCovariance.Multiply(mTemp4x4, mStateTransitionMatrixTranspose); // PHI * P(k-1) * PHI'
      mPredictedStateCovariance += mProcessNoiseCovariance;

      // Update the Measurement Noise Covariance R
      ComputeMeasurementCovarianceMatrix();

      // Compute the Residual Covariance
      // S = H * P * H' + R
      mTemp2x4.Multiply(mObservationMatrix, mPredictedStateCovariance);    // H * P
      mResidualCovariance.Multiply(mTemp2x4, mObservationMatrixTranspose); // H * P * H'
      mResidualCovariance += mMeasurementNoiseCovariance;                  // H * P * H' + R

      // Compute Residual Covariance Inverse, S^(-1)
      mResidualCovarianceInverse.Invert(mResidualCovariance);

      // Update the Kalman Gains
      //  K = P * H' * S_inverse
      mTemp4x2.Multiply(mPredictedStateCovariance, mObservationMatrixTranspose); // P * H'
      mKalmanGains.Multiply(mTemp4x2, mResidualCovarianceInverse);               // P * H' * S_inverse

      // Compute the CORRECTED state estimate
      //  x_hat(k) = x_hat(k) + K * v
      mFilteredStateNE.Multiply(mKalmanGains, mResidual); // K * v
      mFilteredStateNE += mPredictedStateNE;              // x_hat(k) + K * v

      // Compute the CORRECTED state covariance
      // P(k) = (I - K*H) * P(k)
      mTemp4x4.Multiply(mKalmanGains, mObservationMatrix);            // K*H
      mTemp4x4.Subtract(sIdentity4x4, mTemp4x4);                      // (I - K*H)
      mStateCovariance.Multiply(mTemp4x4, mPredictedStateCovariance); // (I - K*H) * P(k)
   }

   // Save new filtered data as old filter data for next pass
   mLastFilteredStateNE = mFilteredStateNE;
   mLastUpdateTime      = aSimTime;

   // Copy the filtered state data to the output arguments
   double locNED[3];
   double locWCS[3];
   double velNED[3];
   double velWCS[3];
   locNED[0] = mFilteredStateNE(0, 0);
   locNED[1] = mFilteredStateNE(1, 0);
   locNED[2] = 0.0;

   velNED[0] = mFilteredStateNE(2, 0);
   velNED[1] = mFilteredStateNE(3, 0);
   velNED[2] = 0.0;

   // Update the 3-D WCS Location based on a 2-D Estimate.
   // Not surprisingly, each axis is "off" compared to the measurement
   // due to the non-existence of a elevation channel measurement input.
   UtEntity entity;
   entity.SetLocationWCS(aSensorLocation);
   entity.ConvertNEDToWCS(locNED, locWCS);
   entity.ConvertNEDVectorToWCS(velWCS, velNED);

   mFilteredStateWCS(0, 0) = locWCS[0];
   mFilteredStateWCS(1, 0) = locWCS[1];
   mFilteredStateWCS(2, 0) = locWCS[2];
   mFilteredStateWCS(3, 0) = velWCS[0];
   mFilteredStateWCS(4, 0) = velWCS[1];
   mFilteredStateWCS(5, 0) = velWCS[2];

   // Copy the filtered state data to the output arguments
   aFilteredLocation[0] = mFilteredStateWCS(0, 0);
   aFilteredLocation[1] = mFilteredStateWCS(1, 0);
   aFilteredLocation[2] = mFilteredStateWCS(2, 0);
   aFilteredVelocity[0] = mFilteredStateWCS(3, 0);
   aFilteredVelocity[1] = mFilteredStateWCS(4, 0);
   aFilteredVelocity[2] = mFilteredStateWCS(5, 0);
}

// Computes the measurement covariance matrix
// Uses the Jacobian of the converted measurement noise
// e = r sin(psi)
// n = r cos(psi)
//
// R = J * SphericalNoise * transpose(J)
void KalmanFilter2D_RB::ComputeMeasurementCovarianceMatrix()
{
   double sinb = sin(mBearing);
   double cosb = cos(mBearing);

   double sinSQb = sinb * sinb;
   double cosSQb = cosb * cosb;

   double sigmaRsq = mMeasurementNoiseR * mMeasurementNoiseR;
   double sigmaBsq = mMeasurementNoiseB * mMeasurementNoiseB;

   double Rsq = mRange * mRange;

   // Sigma^2 - N
   mMeasurementNoiseCovariance(0, 0) = cosSQb * sigmaRsq + Rsq * sinSQb * sigmaBsq;

   // Sigma^2 - E
   mMeasurementNoiseCovariance(1, 1) = sinSQb * sigmaRsq + Rsq * cosSQb * sigmaBsq;

   // The Cross Correlation terms are zero since the assumed target dynamics model is independent in X-Y
   // These non-diagonal terms are initialized to zero in the constructor, so there is no need for it here.
   // mMeasurementNoiseCovariance(0, 1) = 0.0;
   // mMeasurementNoiseCovariance(1, 0) = 0.0;
}

void KalmanFilter2D_RB::ComputeRB(const double aSensorLocation[3], const double aMeasuredLocation[3], double aRB[2])
{
   UtEntity entity;
   entity.SetLocationWCS(aSensorLocation);
   double NED[3];
   entity.ConvertWCSToNED(aMeasuredLocation, NED);

   double bgy = atan2(NED[1], NED[0]);
   if (bgy > UtMath::cPI)
   {
      bgy -= UtMath::cTWO_PI;
   }
   else if (bgy < -UtMath::cPI)
   {
      bgy += UtMath::cTWO_PI;
   }

   double rng = sqrt((NED[0] * NED[0]) + (NED[1] * NED[1]) + (NED[2] * NED[2]));

   aRB[0] = rng;
   aRB[1] = bgy;
}

// virtual
void KalmanFilter2D_RB::NoDetectUpdate(double aSimTime, UtMeasurementData& aDataOut)
{
   // todo-mjm propagate and update covariance in output
   double locationWCS[3];
   double velocityWCS[3];
   NoDetectUpdate(aSimTime, locationWCS, velocityWCS);
   aDataOut.SetLocationWCS(locationWCS);
   aDataOut.SetVelocityWCS(velocityWCS);
}

void KalmanFilter2D_RB::NoDetectUpdate(double aSimTime, double aFilteredLocation[3], double aFilteredVelocity[3])
{
   // This function is used to return a current kinematic state estimate
   // when a scheduled detection failed forcing the track to be coasted.
   // This needs to be investigated further. Should this be considered a valid
   // update or should only valid updates be considered? In which case, one
   // only has to use the GetKinematicEstimate. I THINK that a coast is considered a
   // valid estimate in order to minimize the possibility of a very long time between
   // two valid updates causing an instability within the filter because of overestimating
   // target speed/acceleration.

   // Update the State Transition Matrix
   double dt = aSimTime - mLastUpdateTime;
   UpdateTransitionMatrix(dt); // Used for the North-East State Only

   // Extrapolate to current time state estimate based on the last
   // filtered state estimate
   // Update the North-East State Vector
   UtMatrixd temp(4, 1);
   temp.Multiply(mStateTransitionMatrix, mFilteredStateNE);

   mFilteredStateNE = temp;

   // Update and return the WCS State Vector
   UtMatrixd WCSStateTransition(6, 6);
   UtMatrixd tempWCS(6, 1);
   WCSStateTransition.LoadIdentity();
   WCSStateTransition(0, 3) = dt;
   WCSStateTransition(1, 4) = dt;
   WCSStateTransition(2, 5) = dt;
   tempWCS.Multiply(WCSStateTransition, mFilteredStateWCS);

   mFilteredStateWCS = tempWCS;

   // Save to the arguments to return
   aFilteredLocation[0] = tempWCS(0, 0);
   aFilteredLocation[1] = tempWCS(1, 0);
   aFilteredLocation[2] = tempWCS(2, 0);
   aFilteredVelocity[0] = tempWCS(3, 0);
   aFilteredVelocity[1] = tempWCS(4, 0);
   aFilteredVelocity[2] = tempWCS(5, 0);

   // Save the LastUpdateTime
   mLastUpdateTime = aSimTime;
}

// virtual
void KalmanFilter2D_RB::GetKinematicEstimate(double aSimTime, UtMeasurementData& aDataOut)
{
   double locationWCS[3];
   double velocityWCS[3];
   GetKinematicEstimate(aSimTime, locationWCS, velocityWCS);
   aDataOut.SetLocationWCS(locationWCS);
   aDataOut.SetVelocityWCS(velocityWCS);
}

// Can be called at anytime to get an up-to-date state estimate
void KalmanFilter2D_RB::GetKinematicEstimate(double aSimTime, double aLocationEstimate[3], double aVelocityEstimate[3])
{
   // This function is used to return a current kinematic state estimate
   // BETWEEN calls to the filter update.

   // Update the State Transition Matrix
   double dt = aSimTime - mLastUpdateTime;
   UpdateTransitionMatrix(dt);

   // Extrapolate to current time state estimate based on the last
   // filtered state estimate
   // Update the North-East State Vector
   UtMatrixd temp(4, 1);
   temp.Multiply(mStateTransitionMatrix, mFilteredStateNE);

   // Update and return the WCS State Vector
   UtMatrixd WCSStateTransition(6, 6);
   UtMatrixd tempWCS(6, 1);
   WCSStateTransition.LoadIdentity();
   WCSStateTransition(0, 3) = dt;
   WCSStateTransition(1, 4) = dt;
   WCSStateTransition(2, 5) = dt;
   tempWCS.Multiply(WCSStateTransition, mFilteredStateWCS);

   // Save to the arguments to return
   aLocationEstimate[0] = tempWCS(0, 0);
   aLocationEstimate[1] = tempWCS(1, 0);
   aLocationEstimate[2] = tempWCS(2, 0);
   aVelocityEstimate[0] = tempWCS(3, 0);
   aVelocityEstimate[1] = tempWCS(4, 0);
   aVelocityEstimate[2] = tempWCS(5, 0);
}

// State Transition Matrix
void KalmanFilter2D_RB::UpdateTransitionMatrix(double aDt)
{
   // Form the transition matrix.
   // Note: diagonal elements have already been set to 1 in the constructor.

   mStateTransitionMatrix(0, 2) = aDt;
   mStateTransitionMatrix(1, 3) = aDt;

   // Explicitly form the transpose of the transition matrix.
   // Note: diagonal elements have already been set to 1 in the constructor.

   mStateTransitionMatrixTranspose(2, 0) = aDt;
   mStateTransitionMatrixTranspose(3, 1) = aDt;
}

// Process Noise Covariance matrix
void KalmanFilter2D_RB::UpdateProcessNoiseCovariance(double aDt)
{
   if (mNoiseModel == cCONST_VEL)
   {
      mProcessNoiseCovariance(0, 0) = mNoiseXSqrd * aDt;
      mProcessNoiseCovariance(1, 1) = mNoiseYSqrd * aDt;
      mProcessNoiseCovariance(2, 2) = mNoiseXSqrd;
      mProcessNoiseCovariance(3, 3) = mNoiseYSqrd;

      mProcessNoiseCovariance(0, 2) = 0.0;
      mProcessNoiseCovariance(1, 3) = 0.0;

      mProcessNoiseCovariance(2, 0) = mProcessNoiseCovariance(0, 2);
      mProcessNoiseCovariance(3, 1) = mProcessNoiseCovariance(1, 3);
   }
   else // Default to the Const_Acc model if the input model type is incorrect
   {
      double mult1 = aDt * aDt / 2.0;
      double mult2 = aDt * aDt * aDt / 3.0;

      mProcessNoiseCovariance(0, 0) = mNoiseXSqrd * mult2;
      mProcessNoiseCovariance(1, 1) = mNoiseYSqrd * mult2;
      mProcessNoiseCovariance(2, 2) = mNoiseXSqrd * aDt;
      mProcessNoiseCovariance(3, 3) = mNoiseYSqrd * aDt;

      mProcessNoiseCovariance(0, 2) = mNoiseXSqrd * mult1;
      mProcessNoiseCovariance(1, 3) = mNoiseYSqrd * mult1;

      mProcessNoiseCovariance(2, 0) = mProcessNoiseCovariance(0, 2);
      mProcessNoiseCovariance(3, 1) = mProcessNoiseCovariance(1, 3);
   }
}

// Initialize the process noise covariance to something large
void KalmanFilter2D_RB::InitializeProcessNoiseCovariance()
{
   mProcessNoiseCovariance.LoadIdentity();
   double acc                    = 100.0;
   mProcessNoiseCovariance(0, 0) = acc;
   mProcessNoiseCovariance(1, 1) = acc;
   mProcessNoiseCovariance(2, 2) = mNoiseXSqrd * mNoiseXSqrd;
   mProcessNoiseCovariance(3, 3) = mNoiseYSqrd * mNoiseYSqrd;
}

// Return the State Covariance Matrix
// virtual
const UtMatrixd* KalmanFilter2D_RB::GetStateCovarianceMatrix()
{
   return &mStateCovariance;
}

// virtual
void KalmanFilter2D_RB::SetSphericalMeasurementErrors(double aRangeMeasurementError,
                                                      double aBearingMeasurementError,
                                                      double aElevationMeasurementError)
{
   mInitialized       = true;
   mMeasurementNoiseR = aRangeMeasurementError;
   mMeasurementNoiseB = aBearingMeasurementError;
}

void KalmanFilter2D_RB::SetFilterProcessNoise(const double aXYZ[3])
{
   mNoiseXSqrd = aXYZ[0];
   mNoiseYSqrd = aXYZ[1];
}

// virtual
bool KalmanFilter2D_RB::IsStable() const
{
   return (mPass > 2); // TODO-TEMP
}

} // namespace TrackingFilters
