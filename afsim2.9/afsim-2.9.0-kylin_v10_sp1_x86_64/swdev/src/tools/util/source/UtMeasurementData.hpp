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
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#ifndef UTMEASUREMENTDATA_HPP
#define UTMEASUREMENTDATA_HPP

#include "ut_export.h"

#include <vector>

#include "UtCovariance.hpp"
#include "UtInformation.hpp"
#include "UtMat3.hpp"
#include "UtQuaternion.hpp"
#include "UtStringId.hpp"
#include "UtVec3.hpp"

//! Information typedefs (not included from UtInformation.hpp)
using UtVec3dInformation          = UtInformation<UtVec3d>;
using UtQuaternion_Information    = UtInformation<UtQuaternion>;
using UtCovariancePtrInformation  = UtInformation<UtCovariance*>;
using UtStringIdInformation       = UtInformation<UtStringId>;
using UtStringIdInformationVector = std::vector<UtStringIdInformation>;

/**
   UtMeasurementData is a general interface for information that are commonly the
   results of sensor measurements.  They are meant to be filtered and fused with
   sensor measurements.
   UtMeasurementData is composed of atomic pieces of information (see UtInformation).
   As each datum is an instance of a class, the attributes are exposed as public members.
   Additionally, a standard accessor / mutator interface is provided.  An auxiliary data
   capability will soon be added, which will allow users to add their own additional
   data / information.
*/
class UT_EXPORT UtMeasurementData
{
public:
   UtMeasurementData();
   UtMeasurementData(const UtMeasurementData& aSrc) = default;
   virtual ~UtMeasurementData()                     = default;

   virtual void Reset();

   double GetUpdateTime() const { return mUpdateTime.Get(); }
   void   SetUpdateTime(double aUpdateTime) { mUpdateTime.SetAndValidate(aUpdateTime); }

   //! @name Perceived target WCS and LLA location.
   //! Get/Set the target location in WCS and / or LLA coordinates
   //@{
   //! Accessors and mutators using double[3]
   void GetLocationWCS(double aTargetLocationWCS[3]) const;
   void SetLocationWCS(const double aTargetLocationWCS[3]);

   //! Accessors using UtVec3d.
   const UtVec3d& GetLocationWCS() const;

   void GetLocationLLA(double& aLat, double& aLon, double& aAlt) const;

   void SetLocationLLA(double aLat, double aLon, double aAlt);

   //! Is the target WCS location attribute valid?
   bool LocationValid() const { return (mLocationWCS.IsValid() || mLocationLLA.IsValid()); }

   //! Define whether or not the target WCS location attribute is valid.
   void SetWCS_LocationValid(bool aLocationValid) { mLocationWCS.SetValid(aLocationValid); }

   //! Define whether or not the target LLA location attribute is valid.
   void SetLLA_LocationValid(bool aLocationValid) { mLocationLLA.SetValid(aLocationValid); }

   //@}

   //! @name Covariance matrix.
   //! Get/Set the measurement covariance and validity flags.
   //!
   //! The covariance matrix size and contents vary based on the specific filter being used.
   //@{

   //-----------------------------------------------------------------------
   // Note: UtMatrix-based methods are deprecated.
   // Set as a UtMatrix.
   const UtMatrixd* GetStateCovarianceMatrix() const;
   bool             GetStateCovarianceMatrix(double aSimTime, UtCovariance& aStateCovarianceMatrix);
   bool             StateCovarianceMatrixValid() const { return mStateCovariancePtr.IsValid(); }
   // end deprecated methods.
   //-----------------------------------------------------------------------

   // Set as a covariance object.
   const UtCovariance* GetStateCovariance() const;
   void                SetStateCovariance(double aSimTime, const UtCovariance& aStateCovariance);
   void                SetStateCovariance(double aSimTime, UtCovariance* aStateCovariancePtr);

   //! Is the state covariance matrix attribute valid?
   bool StateCovarianceValid() const;

   //! Define state whether or not the covariance matrix attribute is valid.
   void SetStateCovarianceValid(bool aStateCovarianceValid);

   //@}

   //! @name Measurement Covariance.
   //! Get/Set the measurement covariance and validity flags.
   //!
   //! This covariance is meant to be set based on the az, el, range
   //! errors of a given measurement.
   //! See UtCovariance::CreateMeasurementCovariance
   //@{

   // Get / Set as a covariance object.
   const UtCovariance* GetMeasurementCovariance() const;
   void                SetMeasurementCovariance(double aSimTime, const UtCovariance& aMeasurementCovariance);
   void                SetMeasurementCovariance(double aSimTime, UtCovariance* aMeasurementCovariancePtr);

   //! Is the state covariance matrix attribute valid?
   bool MeasurementCovarianceValid() const;

   //! Define state whether or not the covariance matrix attribute is valid.
   void SetMeasurementCovarianceValid(bool aMeasurementCovarianceValid);

   //@}

   //! @name Perceived target WCS velocity.
   //! Get/Set the target velocity vector
   //@{

   void GetVelocityWCS(double aTargetVelocityWCS[3]) const;
   void SetVelocityWCS(const double aTargetVelocityWCS[3]);

   //! Get the speed (magnitude of the velocity) of the target.
   //! The speed is valid only if the velocity is valid.
   double GetSpeed() const { return mVelocityWCS.Get().Magnitude(); }

   //! Get the speed squared (magnitude squared of the velocity) of the target.
   //! The speed squared is valid only if the velocity is valid.
   double GetSpeedSquared() const { return mVelocityWCS.Get().MagnitudeSquared(); }

   //! Accessors using UtVec3d
   const UtVec3d& GetVelocityWCS() const { return mVelocityWCS.Get(); }
   UtVec3d&       GetVelocityWCS() { return mVelocityWCS.Get(); }

   //! Define whether or not the target WCS Velocity attribute is valid.
   void SetVelocityValid(bool aVelocityValid) { mVelocityWCS.SetValid(aVelocityValid); }
   void SetWCS_VelocityValid(bool aVelocityValid) { mVelocityWCS.SetValid(aVelocityValid); } // same as above
   bool VelocityValid() const { return (mVelocityWCS.IsValid()); }
   //@}

   void GetOriginatorLocationWCS(double aOriginatorLocationWCS[3]) const;
   void SetOriginatorLocationWCS(const double aOriginatorLocationWCS[3]);

   //! Accessors using UtVec3d
   const UtVec3d& GetOriginatorLocationWCS() const { return mOriginatorLocationWCS.Get(); }
   UtVec3d&       GetOriginatorLocationWCS() { return mOriginatorLocationWCS.Get(); }

   //! Set the originator transform if no sensor and/or platform roll
   //! is possible or accessible.  This version will only set the WCS->NED transform, so
   //! will produce a poor result for non-level platforms.  The other methods are preferred.
   void SetOriginatorTransformWCS(const double aOriginatorLocationWCS[3]);

   //! Get/Set the WCS->PCS transform.
   void SetOriginatorTransformWCS(const UtQuaternion& aDCM);
   void SetOriginatorTransformWCS(const double aOriginatorTransformWCS[3][3]);
   void GetOriginatorTransformWCS(double aOriginatorTransformWCS[3][3]) const;

   //! Get both the originator location and transform at the same time.
   void CopyOriginatorFrom(const UtMeasurementData* aMeasurementSrc);

   //! Accessors using UtQuaternion
   const UtQuaternion& GetOriginatorTransformWCS() const { return mOriginatorTransformWCS.Get(); }
   UtQuaternion&       GetOriginatorTransformWCS() { return mOriginatorTransformWCS.Get(); }

   //! @name Perceived range and range error methods.
   //! Get/Set the target range and range error and validity flags
   //@{

   //! Get the range from the originator to the target.
   double GetRange() const { return mRange.Get(); }
   //! Set the range from the originator to the target.
   void SetRange(double aRange) { mRange.Set(aRange); }

   //! Is the target range attribute valid?
   bool RangeValid() const { return mRange.IsValid(); }
   //! Define whether or not the target range attribute is valid.
   void SetRangeValid(bool aRangeValid) { mRange.SetValid(aRangeValid); }

   //! Get the standard deviation of the error in the range measurement.
   double GetRangeError() const { return mRange.GetError(); }
   //! Set the standard deviation of the error in the range measurement.
   void SetRangeError(double aRangeError) { mRange.SetError(aRangeError); }

   //@}

   //! @name Perceived target bearing from true north.
   //! Get/Set the target bearing and validity flags
   //@{

   //! Get the bearing of the target with respect to true north at the originators location.
   //! @return The bearing angle (radians)
   double GetBearing() const { return mBearing.Get(); }

   //! Set the bearing of the target with respect to true north at the originators location.
   //! @param aBearing The bearing angle (radians)
   void SetBearing(double aBearing) { mBearing.Set(aBearing); }

   //! Is the target bearing attribute valid?
   bool BearingValid() const { return mBearing.IsValid(); }
   //! Define whether or not the target bearing attribute is valid.
   void SetBearingValid(bool aBearingValid) { mBearing.SetValid(aBearingValid); }

   //! Get the standard deviation of the error in the bearing measurement.
   double GetBearingError() const { return mBearing.GetError(); }

   //! Set the standard deviation of the error in the bearing measurement.
   void SetBearingError(double aBearingError) { mBearing.SetError(aBearingError); }

   //@}

   //! @name Perceived target elevation angle from local horizontal.
   //! Get/Set the target elevation and validity flags
   //@{

   //! Get the elevation angle of the target.
   //! Return the elevation angle of the target with respect to plane that is tangent to
   //! the Earth ellipsoid at the originators location. Positive angles are above the plane
   //! and negative angles are below the plane.
   //! @returns The elevation angle (radians).
   double GetElevation() const { return mElevation.Get(); }

   //! Set the elevation angle of the target.
   //! Return the elevation angle of the target with respect to plane that is tangent to
   //! the Earth ellipsoid at the originators location. Positive angles are above the plane
   //! and negative angles are below the plane.
   //! @param aElevation The angle of the target above/below the horizontal.
   void SetElevation(double aElevation) { mElevation.Set(aElevation); }

   //! Is the target elevation angle attribute valid?
   bool ElevationValid() const { return mElevation.IsValid(); }

   //! Define whether or not the target elevation angle attribute is valid.
   void SetElevationValid(bool aElevationValid) { mElevation.SetValid(aElevationValid); }

   //! Get the standard deviation of the error in the elevation measurement.
   double GetElevationError() const { return mElevation.GetError(); }

   //! Set the standard deviation of the error in the elevation measurement.
   void SetElevationError(double aElevationError) { mElevation.SetError(aElevationError); }

   //@}

   //! @name Perceived target azimuth the detecting sensor's frame of reference.
   //! Get/Set the target azimuth and validity flags
   //@{

   //! Get the sensor azimuth of the target with respect to true north at the originators location.
   //! @return The sensor azimuth angle (radians)
   double GetSensorAzimuth() const { return mSensorAzimuth.Get(); }

   //! Set the sensor azimuth of the target with respect to true north at the originators location.
   //! @param aSensorAzimuth The sensor azimuth angle (radians)
   void SetSensorAzimuth(double aSensorAzimuth) { mSensorAzimuth.Set(aSensorAzimuth); }

   //! Is the target sensor azimuth attribute valid?
   bool SensorAzimuthValid() const { return mSensorAzimuth.IsValid(); }

   //! Define whether or not the target sensor azimuth attribute is valid.
   void SetSensorAzimuthValid(bool aSensorAzimuthValid) { mSensorAzimuth.SetValid(aSensorAzimuthValid); }

   //! Get the standard deviation of the error in the sensor azimuth measurement.
   double GetSensorAzimuthError() const { return mSensorAzimuth.GetError(); }

   //! Set the standard deviation of the error in the sensor azimuth measurement.
   void SetSensorAzimuthError(double aSensorAzimuthError) { mSensorAzimuth.SetError(aSensorAzimuthError); }

   //@}

   //! @name Perceived target elevation angle from local horizontal.
   //! Get/Set the target elevation and validity flags
   //@{

   //! Get the sensor elevation angle of the target.
   //! Return the elevation angle of the target with respect the sensor's beam or center of FOV.
   //! @returns The sensor elevation angle (radians).
   double GetSensorElevation() const { return mSensorElevation.Get(); }

   //! Set the elevation angle of the target.
   //! Return the elevation angle of the target with respect to plane that is tangent to
   //! the Earth ellipsoid at the originators location. Positive angles are above the plane
   //! and negative angles are below the plane.
   //! @param aSensorElevation The angle of the target above/below the horizontal.
   void SetSensorElevation(double aSensorElevation) { mSensorElevation.Set(aSensorElevation); }

   //! Is the target elevation angle attribute valid?
   bool SensorElevationValid() const { return mSensorElevation.IsValid(); }
   //! Define whether or not the target elevation angle attribute is valid.
   void SetSensorElevationValid(bool aSensorElevationValid) { mSensorElevation.SetValid(aSensorElevationValid); }

   //! Get the standard deviation of the error in the elevation measurement.
   double GetSensorElevationError() const { return mSensorElevation.GetError(); }

   //! Set the standard deviation of the error in the elevation measurement.
   void SetSensorElevationError(double aSensorElevationError) { mSensorElevation.SetError(aSensorElevationError); }

   //@}

   //! @name Perceived range rate and range rate error methods.
   //! Get/Set the target range rate and range rate error and validity flags.
   //@{

   //! Get the range rate from the originator to the target.
   double GetRangeRate() const { return mRangeRate.Get(); }
   //! Set the range rate from the originator to the target.
   void SetRangeRate(double aRangeRate) { mRangeRate.Set(aRangeRate); }

   //! Is the target range rate attribute valid?
   bool RangeRateValid() const { return mRangeRate.IsValid(); }
   //! Define whether or not the target range rate attribute is valid.
   void SetRangeRateValid(bool aRangeRateValid) { mRangeRate.SetValid(aRangeRateValid); }

   //! Get the standard deviation of the error in the range rate measurement.
   double GetRangeRateError() const { return mRangeRate.GetError(); }
   //! Set the standard deviation of the error in the range rate measurement.
   void SetRangeRateError(double aRangeRateError) { mRangeRate.SetError(aRangeRateError); }

   //@}

   //! @name Perceived target type ID
   //! (single type ID assumed; see below for get / set for multiple possible types)
   //! Get/Set the target type ID and validity flags
   //@{

   //! Get the perceived type ID of the target.
   UtStringId GetTypeId() const { return mTypeIds[0].Get(); }
   void       SetTypeId(UtStringId aTypeId);

   //! Is the target type ID attribute valid?
   bool TypeIdValid() const { return mTypeIds[0].IsValid(); }
   //! Define whether or not the target type ID attribute is valid.
   void SetTypeIdValid(bool aTypeIdValid) { mTypeIds[0].SetValid(aTypeIdValid); }
   //@}

   //! @name Perceived target type ID
   //! (multiple possible type IDs assumed; see above for get / set for a single assumed type)
   //! Get/Set the target type IDs and validity flags
   //@{

   //! Get the perceived type IDs of the target.
   //!@note If multiple possible type Ids exist, the associated quality for each type is the probability
   //! that the target is of that type.  A multinomial distribution is assumed.
   const UtStringIdInformationVector& GetTypeIds() const { return mTypeIds; }

   //! Set the perceived type IDs of the target.
   //!@note If multiple possible type Ids exist, the associated quality should be the probability
   //! according to a multinomial distribution that the target is of that type.  Probabilities for all types should sum to 1.
   void SetTypeIds(const UtStringIdInformationVector& aTypeIds) { mTypeIds = aTypeIds; }

   //! Get the perceived type IDs of the target (non-const version).  Use this version to directly manipulate the type
   //! id vector.
   //!@note If multiple possible type Ids exist, the associated quality for each type is the probability
   //! that the target is of that type.  A multinomial distribution is assumed.
   //!@note Make sure that if manipulating the vector, make sure that the sum of the probabilities (qualities) of all
   //! entries equals one.
   UtStringIdInformationVector& GetTypeIds() { return mTypeIds; }

   //! Are the target type IDs valid?
   //!@note It is assumed that if one ID is valid, all are valid.
   bool TypeIdsValid() const { return mTypeIds[0].IsValid(); }
   void SetTypeIdsValid(bool aTypeIdValid);
   //@}

   //! @name Perceived target side ID.
   //! Get/Set the target side ID and validity flags
   //@{

   //! Get the perceived side ID of the target.
   UtStringId GetSideId() const { return mSideId.Get(); }
   //! Set the perceived side ID of the target.
   void SetSideId(UtStringId aSideId) { mSideId.Set(UtStringId(aSideId)); }

   //! Is the target side ID attribute valid?
   bool SideIdValid() const { return mSideId.IsValid(); }
   //! Define whether or not the target side ID attribute is valid.
   void SetSideIdValid(bool aSideIdValid) { mSideId.SetValid(aSideIdValid); }

   //@}

   //! @name Information attributes.
   //! Direct access of these attributes is *only* intended for
   //! access from sensor detection and fusion algorithms.
   //@{
   UtDoubleInformation mUpdateTime; //! Time at which the measurement is valid (note that this will be valid,
   //! but may also have uncertainty)
   mutable UtVec3dInformation mLocationWCS;              //! Perceived location of the target at the time of the report
   mutable UtVec3dInformation mLocationLLA;              //! Perceived location of the target at the time of the report
   UtVec3dInformation         mVelocityWCS;              //! Perceived velocity of the target at the time of the report
   UtCovariancePtrInformation mMeasurementCovariancePtr; //! The measurement covariance produced from sensor errors of a
                                                         //! single measurement.
   UtCovariancePtrInformation mStateCovariancePtr; //! The state covariance, normally set as the product of a Kalman filter.
   UtVec3dInformation         mOriginatorLocationWCS; //! The originator location
   UtQuaternion_Information mOriginatorTransformWCS; //! The originator WCS to sensor FOV or beam coordinate frame transform
   UtDoubleInformation      mRange;
   UtDoubleInformation      mBearing;
   UtDoubleInformation      mElevation;
   UtDoubleInformation      mSensorAzimuth;
   UtDoubleInformation      mSensorElevation;
   UtDoubleInformation      mRangeRate;
   UtStringIdInformationVector mTypeIds;
   UtStringIdInformation       mSideId;
   //@}

protected:
   void UpdateLocationWCS() const;
   void UpdateLocationLLA() const;
};

#endif
