// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfAbsoluteSensorErrorModel.hpp"

#include <string>

#include "UtEllipsoidalCentralBody.hpp"
#include "UtEntity.hpp"
#include "UtInput.hpp"
#include "UtMeasurementUtil.hpp"
#include "UtMemory.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSimulation.hpp"

namespace wsf
{

SensorErrorModelBase* AbsoluteSensorErrorModel::Clone() const
{
   return new AbsoluteSensorErrorModel(*this);
}

// static
std::unique_ptr<SensorErrorModelBase> AbsoluteSensorErrorModel::ObjectFactory(const std::string& aTypeName)
{
   if (aTypeName == "absolute_sensor_error")
   {
      return ut::make_unique<AbsoluteSensorErrorModel>();
   }
   return nullptr;
}

// override
std::unique_ptr<AbsoluteMeasurementError> AbsoluteSensorErrorModel::ComputeMeasurementErrors(WsfSensorResult& aResult) const
{
   // The absolute error is along a radius from the target location with a length that is a normal deviate.
   double radius = GetSensorMode()->GetSensor()->GetRandom().Gaussian() * mSigma;

   // The azimuth is a uniform deviate over the full azimuth range.
   double azimuth = GetSensorMode()->GetSensor()->GetRandom().Uniform(0.0, UtMath::cTWO_PI);

   if (mIs3D)
   {
      // The elevation is a uniform deviate over the full elevation range.
      double elevation = GetSensorMode()->GetSensor()->GetRandom().Uniform(-UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
      double r_xy      = radius * cos(elevation);

      // Convert spherical to 3-D Cartesian.
      return ut::make_unique<AbsoluteMeasurementError>(r_xy * cos(azimuth), r_xy * sin(azimuth), radius * sin(elevation));
   }
   else // 2D
   {
      // Convert circular to 2-D Cartesian.
      return ut::make_unique<AbsoluteMeasurementError>(radius * cos(azimuth), radius * sin(azimuth), 0.0);
   }
}

// override
bool AbsoluteSensorErrorModel::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();
   if (command == "2d_position_error_sigma")
   {
      mIs3D = false;
      aInput.ReadValueOfType(mSigma, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(mSigma, 0.0);
   }
   else if (command == "3d_position_error_sigma")
   {
      mIs3D = true;
      aInput.ReadValueOfType(mSigma, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(mSigma, 0.0);
   }
   else
   {
      myCommand = SensorErrorModelBase::ProcessInput(aInput);
   }

   return myCommand;
}

// override
void AbsoluteSensorErrorModel::ApplyMeasurementErrors(const AbsoluteMeasurementError& aErrors, WsfSensorResult& aResult)
{
   const WsfEM_Interaction::LocationData* truthLocPtr = &aResult.mTgtLoc;
   if ((aResult.mRcvrToTgt.mRange < 0.0) && (aResult.mRcvrToXmtr.mRange >= 0.0))
   {
      // Passive receiver detection.
      truthLocPtr = &aResult.mXmtrLoc;
   }
   double transECEF_NED[3][3];
   ut::EllipsoidalCentralBody::ComputeNEDTransform(truthLocPtr->mLat, truthLocPtr->mLon, truthLocPtr->mAlt, transECEF_NED);
   UtVec3d errorOffsetWCS;
   UtMat3d::InverseTransform(errorOffsetWCS.GetData(), transECEF_NED, aErrors.GetData());
   UtVec3d measuredLocWCS;
   UtVec3d::Add(measuredLocWCS.GetData(), truthLocPtr->mLocWCS, errorOffsetWCS.GetData());
   if (!mIs3D)
   {
      // Provide the exact target altitude, correcting the measured location for a curved Earth.
      double lat, lon, alt;
      GetSensorMode()->GetSensor()->GetPlatform()->ConvertWCSToLLA(measuredLocWCS.GetData(), lat, lon, alt);
      alt = truthLocPtr->mAlt;
      GetSensorMode()->GetSensor()->GetPlatform()->ConvertLLAToWCS(lat, lon, alt, measuredLocWCS.GetData());
   }
   UtVec3d         targetVecWCS = measuredLocWCS - UtVec3d(aResult.mRcvrLoc.mLocWCS);
   WsfMeasurement& measurement  = aResult.mMeasurement;
   UtVec3d         targetVecBCS;
   measurement.GetOriginatorTransformWCS().Rotate(targetVecWCS.GetData(), targetVecBCS.GetData());
   double azimuth, elevation;
   UtEntity::ComputeAzimuthAndElevation(targetVecBCS.GetData(), azimuth, elevation);
   double range = targetVecBCS.Magnitude();

   measurement.SetRangeError(mSigma);
   double azErrorSigma = 0.0;
   if (range >= 0.0)
   {
      azErrorSigma = asin(mSigma / range);
   }
   measurement.SetSensorAzimuthError(azErrorSigma);

   double elErrorSigma = 0.0;
   if (mIs3D)
   {
      elErrorSigma = azErrorSigma;
   }
   else if (mSigma > 0.0)
   {
      // Set the 2D elevation error to a very small nonzero number in order
      // for a measurement covariance to be produced.
      static constexpr double c2D_ELEVATION_ERROR_SIGMA = 1.0E-7;
      elErrorSigma                                      = c2D_ELEVATION_ERROR_SIGMA;
   }

   measurement.SetSensorElevationError(elErrorSigma);
   aResult.mMeasurement.SetSphericalMeasurement(GetSensorMode()->GetSimulation()->GetSimTime(), azimuth, elevation, range, 0.0);
   measurement.SetLocationWCS(measuredLocWCS.GetData());
}

} // namespace wsf
