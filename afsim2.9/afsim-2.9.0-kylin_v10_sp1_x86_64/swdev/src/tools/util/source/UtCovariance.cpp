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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "UtCovariance.hpp"

#include <algorithm>
#include <cstring>

#include "UtEllipsoidalEarth.hpp"
#include "UtEntity.hpp"
#include "UtMat3.hpp"
#include "UtMath.hpp"
#include "UtMeasurementData.hpp"
#include "UtMeasurementUtil.hpp"

// =================================================================================================
UtCovariance::UtCovariance(unsigned int aRows,
                           unsigned int aCols) // = 1)
   : UtMatrixd(aRows, aCols)
   , mTrackUpdateTime(0.0)
   , mNoiseModel(cCONST_VEL)
{
   UtVec3d::Set(mOriginLocationWCS, 0.0);
   UtVec3d::Set(mTargetLocationWCS, 0.0);
   UtVec3d::Set(mTargetVelocityWCS, 0.0);
   UtVec3d::Set(mProcessNoiseVariancesECS, 10.0); // Set to a default value
}

// =================================================================================================
UtCovariance::UtCovariance(const UtCovariance& aCovariance)
   : UtMatrixd(aCovariance)
   , mTrackUpdateTime(aCovariance.mTrackUpdateTime)
   , mNoiseModel(aCovariance.mNoiseModel)
{
   UtVec3d::Set(mOriginLocationWCS, aCovariance.mOriginLocationWCS);
   UtVec3d::Set(mTargetLocationWCS, aCovariance.mTargetLocationWCS);
   UtVec3d::Set(mTargetVelocityWCS, aCovariance.mTargetVelocityWCS);
   UtVec3d::Set(mProcessNoiseVariancesECS, aCovariance.mProcessNoiseVariancesECS);
}

// =================================================================================================
UtCovariance::UtCovariance(const UtMatrixd& aMatrix)
   : UtMatrixd(aMatrix)
   , mTrackUpdateTime(0.0)
   , mNoiseModel(cCONST_VEL)
{
   UtVec3d::Set(mOriginLocationWCS, 0.0);
   UtVec3d::Set(mTargetLocationWCS, 0.0);
   UtVec3d::Set(mTargetVelocityWCS, 0.0);
   UtVec3d::Set(mProcessNoiseVariancesECS, 10.0); // Set to a default value
}

// =================================================================================================
UtCovariance::UtCovariance()
   : UtMatrixd()
   , mTrackUpdateTime(0.0)
   , mNoiseModel(cCONST_VEL)
{
   UtVec3d::Set(mOriginLocationWCS, 0.0);
   UtVec3d::Set(mTargetLocationWCS, 0.0);
   UtVec3d::Set(mTargetVelocityWCS, 0.0);
   UtVec3d::Set(mProcessNoiseVariancesECS, 10.0); // Set to a default value
}

// =================================================================================================
//! Copy track data needed by WsfScriptCovarianceClass methods.
//!
//! @note This method is declared 'const' even though it modifies the cached track data. This is
//! necessary because UtMeasurementData::GetStateCovariance() returns a pointer to a const object.
void UtCovariance::CopyTrackData(const UtMeasurementData& aMeasurement) const
{
   mTrackUpdateTime = aMeasurement.GetUpdateTime();
   aMeasurement.GetOriginatorLocationWCS(mOriginLocationWCS);
   if (aMeasurement.LocationValid())
   {
      aMeasurement.GetLocationWCS(mTargetLocationWCS);
   }
   else if (aMeasurement.RangeValid() && aMeasurement.BearingValid() && aMeasurement.ElevationValid())
   {
      UtMeasurementUtil::LocationWCS(mOriginLocationWCS,
                                     aMeasurement.GetRange(),
                                     aMeasurement.GetBearing(),
                                     aMeasurement.GetElevation(),
                                     mTargetLocationWCS);
   }
   else
   {
      UtVec3d::Set(mTargetLocationWCS, 0.0);
   }

   if (aMeasurement.VelocityValid())
   {
      aMeasurement.GetVelocityWCS(mTargetVelocityWCS);
   }
   else
   {
      UtVec3d::Set(mTargetVelocityWCS, 0.0);
   }
}

// =================================================================================================
//! Get the process noise variances in the ECS frame.
void UtCovariance::GetProcessNoiseVariancesECS(double aProcessNoiseVariancesECS[3]) const
{
   UtVec3d::Set(aProcessNoiseVariancesECS, mProcessNoiseVariancesECS);
}

// =================================================================================================
//! Set the process noise variances in the ECS frame.
void UtCovariance::SetProcessNoiseVariancesECS(const double aProcessNoiseVariancesECS[3])
{
   UtVec3d::Set(mProcessNoiseVariancesECS, aProcessNoiseVariancesECS);
}


// =================================================================================================
//! Create an "uncoverted" measurement covariance.  This can be thought of as computed in a sensor's,
//! natural coordinate system, with the x-axis along the range vector, and azimuth and elevation errors
//! defining uncertainty in the tangential coordinates.
//! Note: The errors are clamped to minimum values to guarantee that subsequent mathematical operations
//! will succeed.
//! @param aRange The range from the origin to the center of the associated error ellipsoid.
//! @param aRangeError The error in range to be used in computing the covariance's extents.
//! @param aAzimuthError The error in azimuth to be used in computing the covariance's extents.
//! @param aElevationError The error in elevation to be used in computing the covariance's extents.
//! @return The created UtCovariance object.
UtCovariance* UtCovariance::CreateMeasurementCovariance(double aRange,
                                                        double aRangeError,
                                                        double aAzimuthError,
                                                        double aElevationError)
{
   UtCovariance* covarPtr = new UtCovariance(3, 3);
   covarPtr->ComputeMeasurementCovariance(aRange, aRangeError, aAzimuthError, aElevationError);
   return covarPtr;
}

// =================================================================================================
//! Computes the measurement noise covariance matrix, expressed in a local sensor frame.
//! This algorithm uses the Jacobian of the converted measurement noise
//! (alternatively, evaluate the chain rule for changes in n, e, d, expressed in polar coordinates).
//! e = r cos(theta) cos(psi)
//! n = r cos(theta) sin(psi)
//! u = r sin(theta)
//! R = J * SphericalNoise * transpose(J)
// static private
void UtCovariance::ComputeMeasurementCovarianceSensorFrame(UtMatrixd& aMeasurementNoiseCovariance,
                                                           double     aRange,
                                                           double     aAzimuth,
                                                           double     aElevation,
                                                           double     aRangeError,
                                                           double     aAzimuthError,
                                                           double     aElevationError)
{
   // todo-mjm Print out warning message in a verbose mode if thresholds not met
   aRangeError     = std::max(aRangeError, 0.001);
   aAzimuthError   = std::max(aAzimuthError, 1.0e-7);
   aElevationError = std::max(aElevationError, 1.0e-7);

   double sine = sin(aElevation);
   double cose = cos(aElevation);
   double sinb = sin(aAzimuth);
   double cosb = cos(aAzimuth);

   double sinSQe = sine * sine;
   double cosSQe = cose * cose;
   double sinSQb = sinb * sinb;
   double cosSQb = cosb * cosb;

   double sigmaRsq = aRangeError * aRangeError;
   double sigmaEsq = aElevationError * aElevationError;
   double sigmaBsq = aAzimuthError * aAzimuthError;

   double Rsq = aRange * aRange;
   aMeasurementNoiseCovariance(0, 0) =
      cosSQe * cosSQb * sigmaRsq + Rsq * sinSQe * cosSQb * sigmaEsq + Rsq * cosSQe * sinSQb * sigmaBsq;

   aMeasurementNoiseCovariance(1, 1) =
      cosSQe * sinSQb * sigmaRsq + Rsq * sinSQe * sinSQb * sigmaEsq + Rsq * cosSQe * cosSQb * sigmaBsq;

   aMeasurementNoiseCovariance(2, 2) = sinSQe * sigmaRsq + Rsq * cosSQe * sigmaEsq;

   aMeasurementNoiseCovariance(0, 1) =
      (sinb * cosb) * (sigmaRsq * cosSQe - Rsq * cosSQe * sigmaBsq + Rsq * sinSQe * sigmaEsq);
   aMeasurementNoiseCovariance(1, 0) = aMeasurementNoiseCovariance(0, 1); // This is a symmetric matrix.

   aMeasurementNoiseCovariance(1, 2) = sinb * (sine * cose) * (-sigmaRsq + Rsq * sigmaEsq);
   aMeasurementNoiseCovariance(2, 1) = aMeasurementNoiseCovariance(1, 2);

   aMeasurementNoiseCovariance(0, 2) = cosb * (sine * cose) * (-sigmaRsq + Rsq * sigmaEsq);
   aMeasurementNoiseCovariance(2, 0) = aMeasurementNoiseCovariance(0, 2);

   // Do we need to include estimates for velocity?
   if (aMeasurementNoiseCovariance.GetSize() == 36)
   {
      // TODO Use range rate to compute velocity values.
      // For now, velocity errors are set very high so they will not be considered in averaging
      static const double INVALID_COVAR_VALUE = 1.0e10;
      double              velocityCovar       = INVALID_COVAR_VALUE;
      aMeasurementNoiseCovariance(3, 3)       = velocityCovar;
      aMeasurementNoiseCovariance(4, 4)       = velocityCovar;
      aMeasurementNoiseCovariance(5, 5)       = velocityCovar;
      // ConvertNED_MatrixToWCS(aMeasurementNoiseCovariance);
   }
}

// =================================================================================================
//! "Convert" a covariance matrix in one frame to be oriented in another frame.
//! This conversion is accomplished through multiplications of the transformation matrix,
//! so that for a transformation matrix, TM, and a covariance matrix, C, and rotated covariance, C',
//!\code
//!                 t
//! C' = TM * C * TM
//!\endcode
//!@param aMatrix The covariance, oriented in frame "A", that is converted to frame "B" (in/out).
//!@param aFrameB_ToFrameA_Transform The transformation matrix that converts from frame "B" to frame "A".
void UtCovariance::ConvertFromFrameA_ToFrameB(UtMatrixd&   aMatrix, // oriented in frame "A",
                                              const double aFrameB_ToFrameA_Transform[3][3])
{
   ConvertFromFrameA_ToFrameB_3x3(0, 0, aMatrix, aFrameB_ToFrameA_Transform);
   if (aMatrix.GetSize() == 36)
   {
      ConvertFromFrameA_ToFrameB_3x3(0, 3, aMatrix, aFrameB_ToFrameA_Transform);
      ConvertFromFrameA_ToFrameB_3x3(3, 0, aMatrix, aFrameB_ToFrameA_Transform);
      ConvertFromFrameA_ToFrameB_3x3(3, 3, aMatrix, aFrameB_ToFrameA_Transform);
   }
}

// =================================================================================================
// static
void UtCovariance::ConvertFromFrameA_ToFrameB_3x3(int          aStartRow,
                                                  int          aStartCol,
                                                  UtMatrixd&   aMatrix, // oriented in frame "A",
                                                  const double aFrameB_ToFrameA_Transform[3][3])
{
   int    i;
   double FrameB_Matrix[3][3];
   double FrameA_Matrix[3][3];
   for (i = aStartRow; i < (aStartRow + 3); ++i)
   {
      for (int j = aStartCol; j < (aStartCol + 3); ++j)
      {
         FrameA_Matrix[i - aStartRow][j - aStartCol] = aMatrix(i, j);
      }
   }

   UtMat3d::InverseTransform(FrameB_Matrix, aFrameB_ToFrameA_Transform, FrameA_Matrix);
   for (i = aStartRow; i < (aStartRow + 3); ++i)
   {
      for (int j = aStartCol; j < (aStartCol + 3); ++j)
      {
         aMatrix(i, j) = FrameB_Matrix[i - aStartRow][j - aStartCol];
      }
   }
}

// =================================================================================================
void UtCovariance::ConvertFromFrameB_ToFrameA_3x3(int          aStartRow,
                                                  int          aStartCol,
                                                  UtMatrixd&   aMatrix, // oriented in frame "A",
                                                  const double aFrameB_ToFrameA_Transform[3][3])
{
   int    i;
   double FrameB_Matrix[3][3];
   double FrameA_Matrix[3][3];
   for (i = aStartRow; i < (aStartRow + 3); ++i)
   {
      for (int j = aStartCol; j < (aStartCol + 3); ++j)
      {
         FrameA_Matrix[i - aStartRow][j - aStartCol] = aMatrix(i, j);
      }
   }

   UtMat3d::Transform(FrameB_Matrix, aFrameB_ToFrameA_Transform, FrameA_Matrix);
   for (i = aStartRow; i < (aStartRow + 3); ++i)
   {
      for (int j = aStartCol; j < (aStartCol + 3); ++j)
      {
         aMatrix(i, j) = FrameB_Matrix[i - aStartRow][j - aStartCol];
      }
   }
}

// =================================================================================================
//! "Convert" a covariance matrix in one frame to be oriented in another frame.
//! This conversion is accomplished through multiplications of the transformation matrix,
//! so that for a transformation matrix, TM, and a covariance matrix, C, and rotated covariance, C',
//!\code
//!        t
//! C' = TM * C * TM
//!\endcode
//!@param aMatrix The covariance, oriented in frame "B", that is converted to frame "A" (in/out).
//!@param aFrameB_ToFrameA_Transform The transformation matrix that converts from frame "B" to frame "A".
void UtCovariance::ConvertFromFrameB_ToFrameA(UtMatrixd&   aMatrix, // oriented in frame "B",
                                              const double aFrameB_ToFrameA_Transform[3][3])
{
   ConvertFromFrameB_ToFrameA_3x3(0, 0, aMatrix, aFrameB_ToFrameA_Transform);
   if (aMatrix.GetSize() == 36)
   {
      ConvertFromFrameB_ToFrameA_3x3(0, 3, aMatrix, aFrameB_ToFrameA_Transform);
      ConvertFromFrameB_ToFrameA_3x3(3, 0, aMatrix, aFrameB_ToFrameA_Transform);
      ConvertFromFrameB_ToFrameA_3x3(3, 3, aMatrix, aFrameB_ToFrameA_Transform);
   }
}

// =================================================================================================
//! Calculate the process noise model.
//! Currently the only choices for noise model are constant velocity and constant acceleration.
//! A future generalization would be to store functions in a "types" database, so that
//! any model could be used.
void UtCovariance::ComputeProcessNoiseECS(double aTimeInterval, int aNoiseModelType /* = cCONST_VEL*/)
{
   (*this).Fill(0.0);
   double       noiseXSqrd = mProcessNoiseVariancesECS[0];
   double       noiseYSqrd = mProcessNoiseVariancesECS[1];
   double       noiseZSqrd = mProcessNoiseVariancesECS[2];
   unsigned int size       = (*this).GetSize();
   mNoiseModel             = aNoiseModelType;

   if (aNoiseModelType == cCONST_VEL)
   {
      double timeIntervalSquared = aTimeInterval * aTimeInterval / 2.0;

      // This covariance represents a constant velocity.
      (*this)(0, 0) = noiseXSqrd * timeIntervalSquared;
      (*this)(1, 1) = noiseYSqrd * timeIntervalSquared;
      (*this)(2, 2) = noiseZSqrd * timeIntervalSquared;

      if (size == 36)
      {
         (*this)(3, 3) = noiseXSqrd;
         (*this)(4, 4) = noiseYSqrd;
         (*this)(5, 5) = noiseZSqrd;
      }
   }
   else if (aNoiseModelType == cCONST_ACC)
   {
      // This covariance represents a 3-d "jerk," or change in the acceleration,
      // and a constant acceleration.
      double mult2 = aTimeInterval * aTimeInterval * aTimeInterval / 3.0;

      (*this)(0, 0) = noiseXSqrd * mult2;
      (*this)(1, 1) = noiseYSqrd * mult2;
      (*this)(2, 2) = noiseZSqrd * mult2;

      if (size == 36)
      {
         double mult1 = aTimeInterval * aTimeInterval / 2.0;

         (*this)(3, 3) = noiseXSqrd * aTimeInterval;
         (*this)(4, 4) = noiseYSqrd * aTimeInterval;
         (*this)(5, 5) = noiseZSqrd * aTimeInterval;

         (*this)(0, 3) = noiseXSqrd * mult1;
         (*this)(1, 4) = noiseYSqrd * mult1;
         (*this)(2, 5) = noiseZSqrd * mult1;

         (*this)(3, 0) = (*this)(0, 3);
         (*this)(4, 1) = (*this)(1, 4);
         (*this)(5, 2) = (*this)(2, 5);
      }
   }
   else
   {
      // invalid model type; use the constant velocity model as a default.
      ComputeProcessNoiseECS(aTimeInterval, cCONST_VEL);
   }
}

// =================================================================================================
//! Compute the process noise covariance, expressed in an desired frame of reference.
//! The transformation matrix from the desired frame of reference to the local NED frame
//! must be provided as an argument.
void UtCovariance::ComputeProcessNoise(double aTimeInterval, const double aFrame_ToNED_Transform[3][3], int aNoiseModelType)
{
   ComputeProcessNoiseECS(aTimeInterval, aNoiseModelType);
   ConvertFromFrameA_ToFrameB(*this, aFrame_ToNED_Transform);
}

//! Given a valid UtMeasurementData object, compute a measurement covariance.
void UtCovariance::ComputeMeasurementCovarianceWCS(const UtMeasurementData& aDataIn)
{
   double WCSToPCSTransform[3][3];
   aDataIn.GetOriginatorTransformWCS(WCSToPCSTransform);
   assert(!(WCSToPCSTransform[0][0] == 0 && WCSToPCSTransform[0][1] == 0 && WCSToPCSTransform[0][2] == 0));
   double range;
   double azimuth;
   double elevation;
   double rangeError;
   double azimuthError;
   double elevationError;
   if (aDataIn.RangeValid() && (aDataIn.SensorAzimuthValid() || aDataIn.BearingValid()) &&
       (aDataIn.SensorElevationValid() || aDataIn.ElevationValid()))
   {
      range          = aDataIn.GetRange();
      azimuth        = (aDataIn.SensorAzimuthValid() ? aDataIn.GetSensorAzimuth() : aDataIn.GetBearing());
      elevation      = (aDataIn.SensorElevationValid() ? aDataIn.GetSensorElevation() : aDataIn.GetElevation());
      rangeError     = aDataIn.GetRangeError();
      azimuthError   = (aDataIn.SensorAzimuthValid() ? aDataIn.GetSensorAzimuthError() : aDataIn.GetBearingError());
      elevationError = (aDataIn.SensorElevationValid() ? aDataIn.GetSensorElevationError() : aDataIn.GetElevationError());
   }
   else // Determine range, azimuth, elevation if not known.
   {
      assert(aDataIn.LocationValid());
      aDataIn.GetOriginatorLocationWCS(mOriginLocationWCS);
      double trackLocWCS[3];
      aDataIn.GetLocationWCS(trackLocWCS);
      UtMeasurementUtil::RangeBearingElevation(mOriginLocationWCS, trackLocWCS, range, azimuth, elevation);
      // revisit this (TODO)
      rangeError = aDataIn.GetRangeError();
      azimuthError = (aDataIn.GetSensorAzimuthError() > 0.0 ? aDataIn.GetSensorAzimuthError() : aDataIn.GetBearingError());
      elevationError =
         (aDataIn.GetSensorElevationError() > 0.0 ? aDataIn.GetSensorElevationError() : aDataIn.GetElevationError());
   }

   UtCovariance::ComputeMeasurementCovarianceSensorFrame(*this, range, azimuth, elevation, rangeError, azimuthError, elevationError);

   // Below, the input xform is always B_To_A_Transform, so A = PCS and B = WCS
   UtCovariance::ConvertFromFrameA_ToFrameB(*this, WCSToPCSTransform);
}

// =================================================================================================
//! Compute an "uncoverted" measurement covariance.  This can be thought of as computed in a sensor's,
//! natural coordinate system, with the x-axis along the range vector, and azimuth and elevation errors
//! defining uncertainty in the tangential coordinates.
//! Note: The errors are clamped to minimum values to guarantee that subsequent mathematical operations
//! will succeed.
//! @param aRange The range from the origin to the center of the associated error ellipsoid.
//! @param aRangeError The error in range to be used in computing the covariance's extents.
//! @param aAzimuthError The error in azimuth to be used in computing the covariance's extents.
//! @param aElevationError The error in elevation to be used in computing the covariance's extents.
void UtCovariance::ComputeMeasurementCovariance(double aRange, double aRangeError, double aAzimuthError, double aElevationError)
{
   // todo-mjm Print out warning message in a verbose mode if thresholds not met
   aRangeError     = std::max(aRangeError, 0.001);
   aAzimuthError   = std::max(aAzimuthError, 1.0e-7);
   aElevationError = std::max(aElevationError, 1.0e-7);

   double sigmaRsq = aRangeError * aRangeError;
   double sigmaEsq = aElevationError * aElevationError;
   double sigmaBsq = aAzimuthError * aAzimuthError;

   double Rsq = aRange * aRange;

   (*this)(0, 0) = sigmaRsq;
   (*this)(1, 1) = Rsq * sigmaBsq;
   (*this)(2, 2) = Rsq * sigmaEsq;
}

// =================================================================================================
// static
//! Create a pseudo-covariance matrix, for tracks that do not already have a covariance matrix.
//! @param aMeasurement The track to use to create the covariance.
//! @returns Pointer to the output covariance matrix (may be zero).
UtCovariance* UtCovariance::CreateMeasurementCovarianceWCS(const UtMeasurementData& aMeasurement)
{
   UtCovariance* measurementCovariancePtr = nullptr;
   if (aMeasurement.RangeValid() && (aMeasurement.SensorAzimuthValid() || aMeasurement.BearingValid()) &&
       (aMeasurement.SensorElevationValid() || aMeasurement.ElevationValid()) && (aMeasurement.GetRange() > 0.0) &&
       (aMeasurement.GetRangeError() >= 0.001) &&
       ((aMeasurement.GetSensorAzimuthError() >= 1.0e-7) || (aMeasurement.GetBearingError() >= 1.0e-7)) &&
       ((aMeasurement.GetSensorElevationError() >= 1.0e-7) || (aMeasurement.GetElevationError() >= 1.0e-7)))
   {
      measurementCovariancePtr = new UtCovariance(3, 3);
      measurementCovariancePtr->CopyTrackData(aMeasurement);
      measurementCovariancePtr->ComputeMeasurementCovarianceWCS(aMeasurement);
   }
   return measurementCovariancePtr;
}

// =================================================================================================
//! Given a valid state covariance matrix, compute the predicted state covariance at the given time.
bool UtCovariance::ComputePredictedState(double                   aSimTime,
                                         const UtCovariance&      aCurrentStateCovariance,
                                         const UtMeasurementData& aMeasurement)
{
   bool computed = false;
   if (aMeasurement.LocationValid())
   {
      double trackUpdateTime = aMeasurement.GetUpdateTime();
      double originLocationWCS[3];
      double targetLocationWCS[3] = {0.0, 0.0, 0.0};
      double targetVelocityWCS[3] = {0.0, 0.0, 0.0};
      aMeasurement.GetOriginatorLocationWCS(originLocationWCS);
      aMeasurement.GetLocationWCS(targetLocationWCS);
      if (aMeasurement.VelocityValid())
      {
         aMeasurement.GetVelocityWCS(targetVelocityWCS);
      }
      computed = ComputePredictedState(aSimTime,
                                       aCurrentStateCovariance,
                                       trackUpdateTime,
                                       originLocationWCS,
                                       targetLocationWCS,
                                       targetVelocityWCS);
   }
   return computed;
}

// =================================================================================================
//! Given a valid state covariance matrix, compute the predicted state covariance at the given time.
//!
//! This form uses the track data that was cached into the current state covariance at time it was
//! last retrieved from its associated track.
bool UtCovariance::ComputePredictedState(double aSimTime, const UtCovariance& aCurrentStateCovariance)
{
   return ComputePredictedState(aSimTime,
                                aCurrentStateCovariance,
                                mTrackUpdateTime,
                                mOriginLocationWCS,
                                mTargetLocationWCS,
                                mTargetVelocityWCS);
}

// =================================================================================================
//! Given a valid measurement, compute the predicted state covariance at the given time.
//!
//! This form uses the track measurement data at time it was last retrieved.
bool UtCovariance::ComputePredictedState(double aSimTime, const UtMeasurementData& aMeasurement)
{
   assert(aMeasurement.StateCovarianceValid());
   const UtCovariance* currentCovar = aMeasurement.GetStateCovariance();
   return ComputePredictedState(aSimTime, *currentCovar, aMeasurement);
}

// =================================================================================================
//! Resize the covariance to a new size (aOrder x aOrder).  If expanding, initializing the
//! new cross covariances to zero and the new variances to very large values.
//! @param aOrder The new order of the matrix (size aOrder x aOrder).
void UtCovariance::ResizeToOrder(unsigned int aOrder)
{
   unsigned int aOldOrder = GetRows();
   if (aOldOrder != aOrder)
   {
      UtMatrixd::Resize(aOrder, aOrder);
      if (aOrder > aOldOrder)
      {
         for (unsigned i = aOldOrder; i < aOrder; ++i)
         {
            (*this)(i, i) = 1.0e+10;
         }
      }
   }
}

// =================================================================================================
//! Internal method to compute a predicted state covariance at a given time.
// private
bool UtCovariance::ComputePredictedState(double              aSimTime,
                                         const UtCovariance& aCurrentStateCovariance,
                                         double              aMeasurementUpdateTime,
                                         const double        aOriginLocationWCS[3],
                                         const double        aTargetLocationWCS[3],
                                         const double        aTargetVelocityWCS[3])
{
   if (UtVec3d::Equals(aTargetLocationWCS, 0.0))
   {
      return false;
   }

   *this = aCurrentStateCovariance;

   // Set the information needed to use the new predicted state covariance without track input.

   mTrackUpdateTime = aMeasurementUpdateTime;
   UtVec3d::Set(mOriginLocationWCS, aOriginLocationWCS);
   UtVec3d::Set(mTargetLocationWCS, aTargetLocationWCS);
   UtVec3d::Set(mTargetVelocityWCS, aTargetVelocityWCS);

   double timeInterval = aSimTime - aMeasurementUpdateTime;
   if (timeInterval > 1.0e-6) // Use current value if time interval below a certain limit.
   {
      double WCS_ToNED_Transform[3][3];
      ComputeWCS_ToNED_Transform(aTargetLocationWCS, WCS_ToNED_Transform);

      // This measurement interval should be the time between updates,
      // but if it is too low, clamp it to the time interval since last update.

      double transform[3][3];
      if (!UtVec3d::Equals(aTargetVelocityWCS, 0.0))
      {
         ComputeWCS_ToECS_Transform(aTargetVelocityWCS, WCS_ToNED_Transform, transform);
      }
      else
      {
         UtMat3d::Set(transform, WCS_ToNED_Transform);
      }

      UtCovariance processNoiseCovariance(aCurrentStateCovariance);
      processNoiseCovariance.ComputeProcessNoise(timeInterval, transform);

      if (aCurrentStateCovariance.GetSize() == 36) // 6x6
      {
         // temp- optimize this algorithm...
         UtMatrixd stateTransition(6, 6);
         stateTransition.LoadIdentity();
         stateTransition(0, 3) = timeInterval;
         stateTransition(1, 4) = timeInterval;
         stateTransition(2, 5) = timeInterval;
         UtMatrixd temp6x6(6, 6);
         temp6x6.Multiply(stateTransition, *this); // PHI * P(k-1)
         stateTransition(0, 3) = 0.0;
         stateTransition(1, 4) = 0.0;
         stateTransition(2, 5) = 0.0;
         stateTransition(3, 0) = timeInterval;
         stateTransition(4, 1) = timeInterval;
         stateTransition(5, 2) = timeInterval;
         this->Multiply(temp6x6, stateTransition); // PHI * P(k-1) * PHI'
         *this += processNoiseCovariance;
      }
      else
      {
         *this += processNoiseCovariance;
      }
   }
   return true;
}

// =================================================================================================
//! Construct a process noise covariance based on the parameters, including
//! the sensor location and the estimated target velocity.  Taken together, these
//! give the target's ECS coordinate system that is needed.
void UtCovariance::ComputeProcessNoise(double       aTimeInterval,
                                       const double aTargetLocationWCS[3],
                                       const double aTargetVelocityWCS[3], // estimated.
                                       int          aNoiseModelType /* = cCONST_VEL*/)
{
   double WCS_ToNED_Transform[3][3];
   double WCS_ToECS_Transform[3][3];

   ComputeRotationMatrices(aTargetLocationWCS, aTargetVelocityWCS, WCS_ToNED_Transform, WCS_ToECS_Transform);
   UtCovariance::ComputeProcessNoise(aTimeInterval, WCS_ToECS_Transform, aNoiseModelType);
}

// =================================================================================================
// static
void UtCovariance::ComputeRotationMatrices(const double aTargetLocationWCS[3],
                                           const double aTargetVelocityWCS[3],
                                           double       aWCS_ToNED_Transform[3][3],
                                           double       aWCS_ToECS_Transform[3][3])
{
   double lat;
   double lon;
   double alt;
   UtEllipsoidalEarth::ConvertECEFToLLA(aTargetLocationWCS, lat, lon, alt);
   double sensorLocWCS[3];
   UtEllipsoidalEarth::ComputeNEDTransform(lat, lon, alt, aWCS_ToNED_Transform, sensorLocWCS);
   double velNED[3];
   UtMat3d::Transform(velNED, aWCS_ToNED_Transform, aTargetVelocityWCS);
   double n       = velNED[0];
   double e       = velNED[1];
   double d       = velNED[2];
   double heading = atan2(e, n);
   double pitch   = UtMath::cPI_OVER_2;
   double ne      = sqrt((n * n) + (e * e));
   if (ne != 0.0)
   {
      pitch = -atan2(d, ne);
   }
   else if (d > 0.0)
   {
      pitch = -UtMath::cPI_OVER_2;
   }

   // Finally, we can compute the local transform
   double NED_ToECS_Transform[3][3];
   UtEntity::ComputeRotationalTransform(heading, pitch, 0.0, NED_ToECS_Transform);

   // And the WCS->ECS is the product of the other two:
   UtMat3d::Multiply(aWCS_ToECS_Transform, NED_ToECS_Transform, aWCS_ToNED_Transform);
}

// =================================================================================================
// static
void UtCovariance::ComputeWCS_ToNED_Transform(const double aLocationWCS[3], double aWCS_ToNED_Transform[3][3])
{
   double lat;
   double lon;
   double alt;
   UtEllipsoidalEarth::ConvertECEFToLLA(aLocationWCS, lat, lon, alt);
   double locWCS[3];
   UtEllipsoidalEarth::ComputeNEDTransform(lat, lon, alt, aWCS_ToNED_Transform, locWCS);
}

// =================================================================================================
// static
void UtCovariance::ComputeWCS_ToECS_Transform(const double aVelocityWCS[3],
                                              const double aWCS_ToNED_Transform[3][3],
                                              double       aWCS_ToECS_Transform[3][3])
{
   double velNED[3];
   UtMat3d::Transform(velNED, aWCS_ToNED_Transform, aVelocityWCS);
   double n       = velNED[0];
   double e       = velNED[1];
   double d       = velNED[2];
   double heading = atan2(e, n);
   double pitch   = UtMath::cPI_OVER_2;
   double ne      = sqrt((n * n) + (e * e));
   if (ne != 0.0)
   {
      pitch = -atan2(d, ne);
   }
   else if (d > 0.0)
   {
      pitch = -UtMath::cPI_OVER_2;
   }

   // Finally, we can compute the local transform
   double NED_ToECS_Transform[3][3];
   UtEntity::ComputeRotationalTransform(heading, pitch, 0.0, NED_ToECS_Transform);

   // And the WCS->ECS is the product of the other two:
   UtMat3d::Multiply(aWCS_ToECS_Transform, NED_ToECS_Transform, aWCS_ToNED_Transform);
}

// =================================================================================================
void UtCovariance::ComputeNED_FromWCS(const UtCovariance& aWCS_Covar)
{
   ComputeNED_FromWCS(aWCS_Covar, aWCS_Covar.mTargetLocationWCS);
}

// =================================================================================================
void UtCovariance::ComputeNED_FromWCS(const UtCovariance& aWCS_Covar, const double aRefLocationWCS[3])
{
   *this = aWCS_Covar;
   double lat;
   double lon;
   double alt;
   UtEllipsoidalEarth::ConvertECEFToLLA(aRefLocationWCS, lat, lon, alt);
   double locWCS[3];
   double WCS_ToNED_Transform[3][3];
   UtEllipsoidalEarth::ComputeNEDTransform(lat, lon, alt, WCS_ToNED_Transform, locWCS);

   // Below, the input xform is B_To_A_Transform, so A = NED and B = WCS
   // convert WCS (frame B) to NED (frame A)
   UtCovariance::ConvertFromFrameB_ToFrameA(*this, WCS_ToNED_Transform);
}

// =================================================================================================
// static
//! Creates the measurement covariance matrix, given a measurement and set of nonzero errors.
//! @returns Pointer to the output covariance matrix (may be zero).
//! @note This routine checks for some sanity conditions.  The errors must be >= 1mm (range),
//! >= 1 uRad (azimuth and elevation).  These checks are to improve the conditioning of the matrix
//! to make sure it is invertible.
UtCovariance* UtCovariance::CreateMeasurementCovarianceWCS(const double aRange,
                                                           const double aRangeError,
                                                           const double aAzimuth,
                                                           const double aAzimuthError,
                                                           const double aElevation,
                                                           const double aElevationError,
                                                           const double aSourceLocWCS[3])
{
   UtCovariance* measurementCovarianceMatrixPtr = nullptr;
   if ((aRange > 0.0) && (aRangeError >= 0.001) && (aAzimuthError >= 1.0e-7) && (aElevationError >= 1.0e-7))
   {
      UtMeasurementData measurement;
      measurement.SetRange(aRange);
      measurement.SetRangeError(aRangeError);
      measurement.SetRangeValid(true);
      measurement.SetSensorAzimuth(aAzimuth);
      measurement.SetSensorAzimuthError(aAzimuthError);
      measurement.SetSensorAzimuthValid(true);
      measurement.SetSensorElevation(aElevation);
      measurement.SetSensorElevationError(aElevationError);
      measurement.SetSensorElevationValid(true);
      measurement.SetOriginatorLocationWCS(aSourceLocWCS);
      measurementCovarianceMatrixPtr = CreateMeasurementCovarianceWCS(measurement);
   }
   return measurementCovarianceMatrixPtr;
}
