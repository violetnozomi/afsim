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

#include "UtMeasurementData.hpp"

#include "UtEllipsoidalEarth.hpp"
#include "UtMeasurementUtil.hpp"
#include "UtVec3.hpp"

// =================================================================================================
UtMeasurementData::UtMeasurementData()
{
   mTypeIds.emplace_back();
}

void UtMeasurementData::Reset()
{
   // Set all data to invalid and set the values to zero.
   mLocationWCS.SetValid(false);
   mLocationLLA.SetValid(false);
   mMeasurementCovariancePtr.SetValid(false);
   mStateCovariancePtr.SetValid(false);
   mVelocityWCS.SetValid(false);
   mRange.SetValid(false);
   mBearing.SetValid(false);
   mElevation.SetValid(false);
   mSensorAzimuth.SetValid(false);
   mSensorElevation.SetValid(false);
   mRangeRate.SetValid(false);
   for (UtStringIdInformation& i : mTypeIds)
   {
      i.SetValid(false);
   }
   mSideId.SetValid(false);
   mOriginatorLocationWCS.SetValid(false);
   mOriginatorTransformWCS.SetValid(false);
   SetRange(0.0);
   SetBearing(0.0);
   SetElevation(0.0);
   SetSensorAzimuth(0.0);
   SetSensorElevation(0.0);
   SetRangeError(0.0);
   SetBearingError(0.0);
   SetElevationError(0.0);
   SetSensorAzimuthError(0.0);
   SetSensorElevationError(0.0);
   SetSideId(nullptr);
   SetTypeId(nullptr);
}

//! Get the state covariance matrix as a pointer to a UtMatrixd object.
//! @returns A pointer to the UtMatrixd object representing the state covariance.
//! This will be zero if there is no state covariance.
const UtMatrixd* UtMeasurementData::GetStateCovarianceMatrix() const
{
   return mStateCovariancePtr.Get();
}

//! Define state whether or not the covariance matrix attribute is valid.
void UtMeasurementData::SetStateCovarianceValid(bool aStateCovarianceMatrixValid)
{
   mStateCovariancePtr.SetValid(aStateCovarianceMatrixValid);
}

// =================================================================================================
//! Age out an existing state covariance matrix to the given simulation time.
//! @param aSimTime The time to which we expect to age out the covariance.
//! @param aStateCovarianceMatrix The returned covariance matrix.
//! @return true, if the state covariance matrix exists and aSimTime is greater than the last
//!  update time of the track; false otherwise.
bool UtMeasurementData::GetStateCovarianceMatrix(double aSimTime, UtCovariance& aStateCovarianceMatrix)
{
   double deltaT = aSimTime - mUpdateTime.Get();
   bool   ok     = ((deltaT >= 0.0) && (mStateCovariancePtr.Get() != nullptr) && LocationValid());
   if (ok)
   {
      UtCovariance predictedCovariance;
      // Note: the following must be called with *this, not with *mStateCovariancePtr.Get().
      // Need to look into inserting all the data into the covariance (todo mjm).
      ok = predictedCovariance.ComputePredictedState(aSimTime, *this);
      if (ok)
      {
         aStateCovarianceMatrix = predictedCovariance;
      }
   }
   return ok;
}

// =================================================================================================
//! Get the state covariance matrix as a pointer to a UtCovariance object.
//! @returns A pointer to the UtCovariance object representing the state covariance.
//! This will be zero if there is no state covariance.
const UtCovariance* UtMeasurementData::GetStateCovariance() const
{
   return mStateCovariancePtr.Get();
}

// =================================================================================================
//! Set the state covariance matrix.
void UtMeasurementData::SetStateCovariance(double aSimTime, const UtCovariance& aStateCovariance)
{
   if (mStateCovariancePtr.Get() != nullptr)
   {
      delete mStateCovariancePtr.Get();
   }
   mStateCovariancePtr.Get() = new UtCovariance(aStateCovariance);
   mStateCovariancePtr.SetQuality(
      UtMeasurementUtil::GetQuantitativeMeasurementQuality(aSimTime, aSimTime, *mStateCovariancePtr.Get()));
   mStateCovariancePtr.SetValid(true);
}

// =================================================================================================
//! Set the state covariance matrix.
//! The track instance owns the parameter.
void UtMeasurementData::SetStateCovariance(double aSimTime, UtCovariance* aStateCovariancePtr)
{
   if (mStateCovariancePtr.Get() != nullptr)
   {
      delete mStateCovariancePtr.Get();
   }
   mStateCovariancePtr.Get() = aStateCovariancePtr;
   if (mStateCovariancePtr.Get() == nullptr)
   {
      mStateCovariancePtr.SetValid(false);
   }
   else
   {
      mStateCovariancePtr.SetQuality(
         UtMeasurementUtil::GetQuantitativeMeasurementQuality(aSimTime, aSimTime, *mStateCovariancePtr.Get()));
      mStateCovariancePtr.SetValid(true);
   }
}

//! Is the state covariance matrix attribute valid?
bool UtMeasurementData::StateCovarianceValid() const
{
   return (mStateCovariancePtr.IsValid());
}

// =================================================================================================
//! Get the measurement covariance matrix as a pointer to a UtCovariance object.
//! @returns A pointer to the UtCovariance object representing the measurement covariance.
//! This will be zero if there is no state covariance.
const UtCovariance* UtMeasurementData::GetMeasurementCovariance() const
{
   return mMeasurementCovariancePtr.Get();
}

// =================================================================================================
//! Set the measurement covariance matrix.
void UtMeasurementData::SetMeasurementCovariance(double aSimTime, const UtCovariance& aMeasurementCovariance)
{
   if (mMeasurementCovariancePtr.Get() != nullptr)
   {
      delete mMeasurementCovariancePtr.Get();
   }
   mMeasurementCovariancePtr.Get() = new UtCovariance(aMeasurementCovariance);
   mStateCovariancePtr.SetQuality(
      UtMeasurementUtil::GetQuantitativeMeasurementQuality(aSimTime, aSimTime, *mMeasurementCovariancePtr.Get()));
   mMeasurementCovariancePtr.SetValid(true);
}

// =================================================================================================
//! Set the measurement covariance matrix.
//! The track instance owns the parameter.
void UtMeasurementData::SetMeasurementCovariance(double aSimTime, UtCovariance* aMeasurementCovariancePtr)
{
   if (mMeasurementCovariancePtr.Get() != nullptr)
   {
      delete mMeasurementCovariancePtr.Get();
   }
   mMeasurementCovariancePtr.Get() = aMeasurementCovariancePtr;
   mStateCovariancePtr.SetQuality(
      UtMeasurementUtil::GetQuantitativeMeasurementQuality(aSimTime, aSimTime, *mMeasurementCovariancePtr.Get()));
   mMeasurementCovariancePtr.SetValid(true);
}

//! Is the measurement covariance matrix attribute valid?
bool UtMeasurementData::MeasurementCovarianceValid() const
{
   return (mMeasurementCovariancePtr.IsValid());
}

void UtMeasurementData::SetMeasurementCovarianceValid(bool aMeasurementCovarianceValid)
{
   mMeasurementCovariancePtr.SetValid(aMeasurementCovarianceValid);
}

// =================================================================================================
void UtMeasurementData::GetVelocityWCS(double aVelocityWCS[3]) const
{
   UtVec3d::Set(aVelocityWCS, mVelocityWCS.Get().GetData());
}

void UtMeasurementData::SetVelocityWCS(const double aVelocityWCS[3])
{
   UtVec3d::Set(mVelocityWCS.Get().GetData(), aVelocityWCS);
   mVelocityWCS.SetValid(true);
}

//! Get the location of the platform that originated the track.
void UtMeasurementData::GetOriginatorLocationWCS(double aOriginatorLocationWCS[3]) const
{
   // UtVec3d::Set(aOriginatorLocationWCS, mOriginatorLocationWCS);
   mOriginatorLocationWCS.Get().Get(aOriginatorLocationWCS);
}

// =================================================================================================
//! Set the location of the platform that originated the track.
void UtMeasurementData::SetOriginatorLocationWCS(const double aOriginatorLocationWCS[3])
{
   // UtVec3d::Set(mOriginatorLocationWCS, aOriginatorLocationWCS);
   mOriginatorLocationWCS.Get().Set(aOriginatorLocationWCS);
   mOriginatorLocationWCS.SetValid(true);
}

// =================================================================================================
//! Set the originator transform as a 3x3 matrix.
void UtMeasurementData::SetOriginatorTransformWCS(const double aOriginatorTransformWCS[3][3])
{
   UtQuaternion& xform = mOriginatorTransformWCS.Get();
   xform.Set(aOriginatorTransformWCS);
   mOriginatorTransformWCS.SetValid(true);
}

//! Set the originator transform using a UtQuaternion object.
void UtMeasurementData::SetOriginatorTransformWCS(const UtQuaternion& aDCM)
{
   mOriginatorTransformWCS.Set(aDCM);
}

void UtMeasurementData::SetOriginatorTransformWCS(const double aOriginatorLocationWCS[3])
{
   double aOriginatorTransformWCS[3][3];
   UtCovariance::ComputeWCS_ToNED_Transform(aOriginatorLocationWCS, aOriginatorTransformWCS);
   SetOriginatorTransformWCS(aOriginatorTransformWCS);
}

void UtMeasurementData::CopyOriginatorFrom(const UtMeasurementData* aMeasurementSrc)
{
   double originatorLocWCS[3];
   aMeasurementSrc->GetOriginatorLocationWCS(originatorLocWCS);
   SetOriginatorLocationWCS(originatorLocWCS);

   double originatorTransformWCS[3][3];
   aMeasurementSrc->GetOriginatorTransformWCS(originatorTransformWCS);
   SetOriginatorTransformWCS(originatorTransformWCS);
}

void UtMeasurementData::GetOriginatorTransformWCS(double aOriginatorTransformWCS[3][3]) const
{
   const UtQuaternion& xform = mOriginatorTransformWCS.Get();
   xform.Get(aOriginatorTransformWCS);
}

//! Set the perceived type ID of the target.
void UtMeasurementData::SetTypeId(UtStringId aTypeId)
{
   if (mTypeIds.size() > 1)
   {
      // Previously set with the SetTypeIds method.
      // In this case we only want to keep the value passed in,
      // And make sure the quality is 1.0.
      mTypeIds.clear();
      mTypeIds.emplace_back();
   }

   // Set the first value, which is guaranteed to exist.
   mTypeIds[0].Set(aTypeId);
}

//! Define whether or not the target type IDs attributes are valid.
void UtMeasurementData::SetTypeIdsValid(bool aTypeIdValid)
{
   for (UtStringIdInformation& i : mTypeIds)
   {
      i.SetValid(aTypeIdValid);
   }
}

// =================================================================================================
// private
void UtMeasurementData::UpdateLocationWCS() const
{
   if ((!mLocationWCS.IsValid()) && mLocationLLA.IsValid())
   {
      double lat = mLocationLLA.Get()[0];
      double lon = mLocationLLA.Get()[1];
      double alt = mLocationLLA.Get()[2];
      UtEllipsoidalEarth::ConvertLLAToECEF(lat, lon, alt, mLocationWCS.Get().GetData());
      mLocationWCS.SetValid(true);
   }
}

// =================================================================================================
//! Get the perceived WCS location of the target.
void UtMeasurementData::GetLocationWCS(double aLocationWCS[3]) const
{
   UpdateLocationWCS();
   UtVec3d::Set(aLocationWCS, mLocationWCS.Get().GetData());
}

// =================================================================================================
//! Set the perceived WCS location of the target.
void UtMeasurementData::SetLocationWCS(const double aLocationWCS[3])
{
   UtVec3d::Set(mLocationWCS.Get().GetData(), aLocationWCS);
   mLocationWCS.SetValid(true);
   mLocationLLA.SetValid(false);
}

// =================================================================================================
// private
void UtMeasurementData::UpdateLocationLLA() const
{
   if ((!mLocationLLA.IsValid()) && mLocationWCS.IsValid())
   {
      double lat;
      double lon;
      double alt;
      UtEllipsoidalEarth::ConvertECEFToLLA(mLocationWCS.Get().GetData(), lat, lon, alt);
      mLocationLLA.Get().Set(lat, lon, alt);
      mLocationLLA.SetValid(true);
   }
}

// =================================================================================================
//! Get the perceived WCS location of the target.
const UtVec3d& UtMeasurementData::GetLocationWCS() const
{
   UpdateLocationWCS();
   return mLocationWCS.Get();
}

// =================================================================================================
//! Get the perceived latitude, longitude and altitude of the target.
void UtMeasurementData::GetLocationLLA(double& aLat, double& aLon, double& aAlt) const
{
   UpdateLocationLLA();

   aLat = mLocationLLA.Get()[0];
   aLon = mLocationLLA.Get()[1];
   aAlt = mLocationLLA.Get()[2];
}

// =================================================================================================
void UtMeasurementData::SetLocationLLA(double aLat, double aLon, double aAlt)
{
   mLocationLLA.Get().Set(aLat, aLon, aAlt);
   mLocationLLA.SetValid(true);
   mLocationWCS.SetValid(false);
}
