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

#ifndef UTCOVARIANCE_HPP
#define UTCOVARIANCE_HPP

#include "ut_export.h"

class UtInput;
#include "UtMatrix.hpp"
class UtMeasurementData;
#include "UtVec3.hpp"

//! A representation of a 3x3 or 6x6 covariance matrix.
//! The diagonal elements of this implementation correspond with
//! variances in the estimated location and velocity (x, y, z, vx, vy, vz),
//! Where the coordinate frame of reference varies depending on the context.
//! Process noise covariance is expressed in ECS coordinates initially, and
//! measurement noise covariance in NED coordinates.  These are typically
//! "converted" to a WCS frame for site-independent evaluation.
//! Note: The Compute... methods compute the desired result for the given
//! object.  For example, ComputeMeasurementCovariance(...) fills the calling
//! object with the measurement noise covariance, based on the parameters.
//! Note that in order to compute a measurement covariance, the range,
//! bearing and elevation information (along with associated errors) must
//! be valid.
class UT_EXPORT UtCovariance : public UtMatrixd
{
public:
   //! Enumerated Types to Control the Amount of Process Noise Injected into the Estimates.
   //! May be expanded in the future to arbitrary models.
   enum NOISE_MODEL
   {
      cCONST_VEL,
      cCONST_ACC
   };

   UtCovariance();
   ~UtCovariance() override = default;

   UtCovariance(unsigned int aRows, unsigned int aCols);
   UtCovariance(const UtMatrixd& aMatrix);
   UtCovariance(const UtCovariance& aCovariance);
   UtCovariance& operator=(const UtCovariance&) = default;

   //! @name Track data methods.
   //! A subset of the track data can be stored in the covariance object so the script methods do not
   //! require the user to provide a track as input to the script methods.
   //@{
   void CopyTrackData(const UtMeasurementData& aMeasurement) const;
   void GetOriginLocationWCS(double aOriginLocationWCS[3]) const
   {
      UtVec3d::Set(aOriginLocationWCS, mOriginLocationWCS);
   }
   void SetOriginLocationWCS(const double aOriginLocationWCS[3])
   {
      UtVec3d::Set(mOriginLocationWCS, aOriginLocationWCS);
   }

   void GetTargetLocationWCS(double aTargetLocationWCS[3]) const
   {
      UtVec3d::Set(aTargetLocationWCS, mTargetLocationWCS);
   }
   void SetTargetLocationWCS(const double aTargetLocationWCS[3])
   {
      UtVec3d::Set(mTargetLocationWCS, aTargetLocationWCS);
   }
   //@}

   void GetProcessNoiseVariancesECS(double aProcessNoiseVariances[3]) const;

   void SetProcessNoiseVariancesECS(const double aProcessNoiseVariances[3]);

   int GetProcessNoiseModel() const { return mNoiseModel; }

   //! @name Measurement covariance methods
   //!@{
   static UtCovariance* CreateMeasurementCovarianceWCS(const UtMeasurementData& aData);

   void ComputeMeasurementCovarianceWCS(const UtMeasurementData& aDataIn);

   static UtCovariance* CreateMeasurementCovarianceWCS(double       aRange,
                                                       double       aRangeError,
                                                       double       aBearing,
                                                       double       aBearingError,
                                                       double       aElevation,
                                                       double       aElevationError,
                                                       const double aSourceLocWCS[3]);

   static UtCovariance*
   CreateMeasurementCovariance(double aRange, double aRangeError, double aAzimuthError, double aElevationError);

   void ComputeMeasurementCovariance(double aRange, double aRangeError, double aAzimuthError, double aElevationError);
   //@}

   void ComputeProcessNoiseECS(double aTimeInterval, int aNoiseModelType = cCONST_VEL);

   void ComputeProcessNoise(double aTimeInterval, const double aFrame_ToNED_Transform[3][3], int aNoiseModelType = cCONST_VEL);

   void ComputeProcessNoise(double       aTimeInterval,
                            const double aSensorLocationWCS[3],
                            const double aTargetVelocityWCS[3], // estimated.
                            int          aNoiseModelType = cCONST_VEL);

   bool ComputePredictedState(double                   aSimTime,
                              const UtCovariance&      aCurrentStateCovariance,
                              const UtMeasurementData& aMeasurement);

   bool ComputePredictedState(double aSimTime, const UtMeasurementData& aMeasurement);

   bool ComputePredictedState(double aSimTime, const UtCovariance& aCurrentStateCovariance);

   void ResizeToOrder(unsigned int aOrder);

   void ComputeNED_FromWCS(const UtCovariance& aWCS_Covar);

   void ComputeNED_FromWCS(const UtCovariance& aWCS_Covar, const double aRefLocationWCS[3]);

   static void ConvertFromFrameA_ToFrameB(UtMatrixd& aFrameA_Matrix, const double aFrameB_ToFrameA_Transform[3][3]);

   static void ConvertFromFrameB_ToFrameA(UtMatrixd& aFrameB_Matrix, const double aFrameB_ToFrameA_Transform[3][3]);

   static void ComputeRotationMatrices(const double aSensorLocationWCS[3],
                                       const double aTargetVelocityWCS[3],
                                       double       aWCS_ToNED_Transform[3][3],
                                       double       aWCS_ToECS_Transform[3][3]);

   static void ComputeWCS_ToNED_Transform(const double aSensorLocationWCS[3], double aWCS_ToNED_Transform[3][3]);

   static void ComputeWCS_ToECS_Transform(const double aTargetVelocityWCS[3],
                                          const double aWCS_ToNED_Transform[3][3],
                                          double       aWCS_ToECS_Transform[3][3]);

private:
   bool ComputePredictedState(double              aSimTime,
                              const UtCovariance& aCurrentStateCovariance,
                              double              aMeasurementUpdateTime,
                              const double        aOriginLocationWCS[3],
                              const double        aTargetLocationWCS[3],
                              const double        aTargetVelocityWCS[3]);

   static void ComputeMeasurementCovarianceSensorFrame(UtMatrixd& aMeasurementCovariance,
                                                       double     aRange,
                                                       double     aBearing,
                                                       double     aElevation,
                                                       double     aRangeError,
                                                       double     aBearingError,
                                                       double     aElevationError);

   static void ConvertFromFrameA_ToFrameB_3x3(int          aStartRow,
                                              int          aStartCol,
                                              UtMatrixd&   aFrameA_Matrix,
                                              const double aFrameB_ToFrameA_Transform[3][3]);

   static void ConvertFromFrameB_ToFrameA_3x3(int          aStartRow,
                                              int          aStartCol,
                                              UtMatrixd&   aFrameA_Matrix,
                                              const double aFrameB_ToFrameA_Transform[3][3]);

   mutable double mTrackUpdateTime;
   mutable double mOriginLocationWCS[3];
   mutable double mTargetLocationWCS[3];

   mutable double mTargetVelocityWCS[3];

   double mProcessNoiseVariancesECS[3];
   int    mNoiseModel;
};

#endif
