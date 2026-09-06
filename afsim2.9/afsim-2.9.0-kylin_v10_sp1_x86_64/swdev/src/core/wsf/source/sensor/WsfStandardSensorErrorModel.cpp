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

#include "WsfStandardSensorErrorModel.hpp"

#include "WsfEM_Antenna.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorComponent.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSimulation.hpp"

namespace wsf
{

// =================================================================================================
StandardSensorErrorModel* StandardSensorErrorModel::Clone() const
{
   return new StandardSensorErrorModel(*this);
}

// =================================================================================================
//! Factory method for WsfSensorErrorModelTypes.
//! This is called by WsfSensorErrorModelTypes::LoadType to determine if a pattern
//! represented by this class is being requested.
// static
std::unique_ptr<SensorErrorModelBase> StandardSensorErrorModel::ObjectFactory(const std::string& aTypeName)
{
   if ((aTypeName ==
        "WSF_STANDARD_SENSOR_ERROR") || // WSF_STANDARD_SENSOR_ERROR is undocumented and will be removed in the future.
       (aTypeName == "STANDARD_SENSOR_ERROR") || // STANDARD_SENSOR_ERROR is undocumented and will be removed in the future.
       (aTypeName == "standard_sensor_error"))
   {
      return ut::make_unique<StandardSensorErrorModel>();
   }
   return nullptr;
}

// =================================================================================================
//! Compute the 'standard' measurement errors.
//!
//! @param aResult The sensing chance result data.
//! @return The computed errors.
// virtual
std::unique_ptr<SphericalMeasurementErrors> StandardSensorErrorModel::ComputeMeasurementErrors(WsfSensorResult& aResult) const
{
   SphericalMeasurementErrors errors =
      GetSphericalErrorStdDev(GetSensorMode()->GetSensor()->GetRandom(), aResult, mSigmas);

   // Invoke optional component models for this action.
   WsfSensorComponent::ComputeSphericalMeasurementErrors(*GetSensorMode()->GetSensor(), aResult, errors);
   return ut::make_unique<SphericalMeasurementErrors>(errors);
}

bool StandardSensorErrorModel::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if ((command == "azimuth_error_sigma") || (command == "fixed_azimuth_error_sigma"))
   {
      double      value;
      std::string units;
      aInput.ReadValue(value);
      aInput.ReadValue(units);
      if (units == "percent_of_true_range")
      {
         aInput.ValueInClosedRange(value, 0.0, 100.0);
         value *= -0.01;
      }
      else
      {
         value = aInput.ConvertValueFrom(value, units, UtInput::cANGLE);
         aInput.ValueGreaterOrEqual(value, 0.0);
      }
      mSigmas.mAzErrorStdDev = value;
   }
   else if ((command == "elevation_error_sigma") || (command == "fixed_elevation_error_sigma"))
   {
      double      value;
      std::string units;
      aInput.ReadValue(value);
      aInput.ReadValue(units);
      if (units == "percent_of_true_range")
      {
         aInput.ValueInClosedRange(value, 0.0, 100.0);
         value *= -0.01;
      }
      else
      {
         value = aInput.ConvertValueFrom(value, units, UtInput::cANGLE);
         aInput.ValueGreaterOrEqual(value, 0.0);
      }
      mSigmas.mElErrorStdDev = value;
   }
   else if ((command == "range_error_sigma") || (command == "fixed_range_error_sigma"))
   {
      double      value;
      std::string units;
      aInput.ReadValue(value);
      aInput.ReadValue(units);
      if (units == "percent_of_true_range")
      {
         aInput.ValueInClosedRange(value, 0.0, 100.0);
         value *= -0.01;
      }
      else
      {
         value = aInput.ConvertValueFrom(value, units, UtInput::cLENGTH);
         aInput.ValueGreaterOrEqual(value, 0.0);
      }
      mSigmas.mRangeErrorStdDev = value;
   }
   else if ((command == "range_rate_error_sigma") || (command == "fixed_range_rate_error_sigma"))
   {
      double value;
      aInput.ReadValueOfType(value, UtInput::cSPEED);
      aInput.ValueGreaterOrEqual(value, 0.0);
      mSigmas.mRangeRateErrorStdDev = value;
   }
   else
   {
      myCommand = SensorErrorModelBase::ProcessInput(aInput);
   }

   return myCommand;
}

// =================================================================================================
//! Specifically compute normally distributed polar measurement errors based on the given sensor result and set of
//! standard deviations.
//! @param aRandom The random number generator used to generate the errors.
//! @param aResult The sensing chance result that may be used to help compute the sensor errors.
//! @param aSigmas The set of standard deviations used to compute the sensor errors.
//! @note The sensor result may be modified by this method.
//! @note Once the deprecated WsfSensorMode::ComputeMeasurementErrors is removed, this method can be made protected.
SphericalMeasurementErrors StandardSensorErrorModel::GetSphericalErrorStdDev(ut::Random&      aRandom,
                                                                             WsfSensorResult& aResult,
                                                                             const Sigmas&    aSigmas) const
{
   SphericalMeasurementErrors errors;

   // This is the original calculation
   // For cases where standard deviation was given as a percent of the true range ...
   double trueRange = aResult.mRcvrToTgt.mRange;
   if ((trueRange < 0.0) && (aResult.mRcvrToXmtr.mRange >= 0.0))
   {
      trueRange = aResult.mRcvrToXmtr.mRange;
   }
   WsfMeasurement&      measurement = aResult.mMeasurement;
   const WsfSensorMode* snsrModePtr = GetSensorMode();

   errors.mAzError = 0.0;
   measurement.SetSensorAzimuthError(0.0);
   if (snsrModePtr->ReportsBearing() || snsrModePtr->ReportsLocation())
   {
      double sigma = aSigmas.mAzErrorStdDev;
      if (sigma < 0.0) // error sigma is a percent of the true range.
      {
         sigma = atan2(-sigma * trueRange, trueRange);
      }
      measurement.SetSensorAzimuthError(sigma);
      if (sigma != 0.0)
      {
         errors.mAzError = aRandom.Gaussian() * sigma;
      }
   }

   errors.mElError = 0.0;
   measurement.SetSensorElevationError(0.0);
   if (snsrModePtr->ReportsElevation() || snsrModePtr->ReportsLocation())
   {
      double sigma = aSigmas.mElErrorStdDev;
      if (sigma < 0.0) // error sigma is a percent of the true range.
      {
         sigma = atan2(-sigma * trueRange, trueRange);
      }
      measurement.SetSensorElevationError(sigma);
      if (sigma != 0.0)
      {
         errors.mElError = aRandom.Gaussian() * sigma;
      }
   }

   errors.mRangeError = 0.0;
   measurement.SetRangeError(0.0);
   if (snsrModePtr->ReportsRange() || snsrModePtr->ReportsLocation())
   {
      double sigma = aSigmas.mRangeErrorStdDev;
      if (sigma < 0.0) // error sigma is a percent of the true range.
      {
         sigma = -sigma * trueRange;
      }
      measurement.SetRangeError(sigma);
      if (sigma != 0.0)
      {
         errors.mRangeError = aRandom.Gaussian() * sigma;
      }
   }

   errors.mRangeRateError = 0.0;
   measurement.SetRangeRateError(0.0);
   if (snsrModePtr->ReportsRangeRate())
   {
      double sigma = aSigmas.mRangeRateErrorStdDev;
      measurement.SetRangeRateError(sigma);
      if (sigma != 0.0)
      {
         errors.mRangeRateError = aRandom.Gaussian() * sigma;
      }
   }

   return errors;
}

// =================================================================================================
//! Apply measurement errors computed with ComputeMeasurementErrors to a sensor result.
//! @param aErrors The sensor errors as returned from ComputeMeasurementErrors.
//! @param aResult The sensing chance result to which the error is applied.
void StandardSensorErrorModel::ApplyMeasurementErrors(const SphericalMeasurementErrors& aErrors, WsfSensorResult& aResult)
{
   // Apply the error.
   // Use the true Az/El to avoid propagating wave bending effects to the position calculation.

   double az    = 0.0;
   double el    = 0.0;
   double range = 1.0;

   if (aResult.mRcvrToTgt.mRange >= 0.0)
   {
      az    = UtMath::NormalizeAngleMinusPi_Pi(aResult.mRcvrToTgt.mTrueAz + aErrors.mAzError);
      el    = UtMath::Limit(aResult.mRcvrToTgt.mTrueEl + aErrors.mElError, 0.9999 * UtMath::cPI_OVER_2);
      range = std::max(aResult.mRcvrToTgt.mRange + aErrors.mRangeError, 1.0);
   }
   else if (aResult.mRcvrToXmtr.mRange >= 0.0)
   {
      // Passive receiver detection.
      az    = UtMath::NormalizeAngleMinusPi_Pi(aResult.mRcvrToXmtr.mTrueAz + aErrors.mAzError);
      el    = UtMath::Limit(aResult.mRcvrToXmtr.mTrueEl + aErrors.mElError, 0.9999 * UtMath::cPI_OVER_2);
      range = std::max(aResult.mRcvrToXmtr.mRange + aErrors.mRangeError, 1.0);
   }

   // Apparently it is expected that range rate would already be set in the measurement,
   // so if it is valid, simply add the error to the value already set.
   if (aResult.mMeasurement.RangeRateValid()) // Range rate previously set
   {
      aResult.mMeasurement.SetSphericalMeasurement(GetSensorMode()->GetSimulation()->GetSimTime(),
                                                   az,
                                                   el,
                                                   range,
                                                   aResult.mMeasurement.GetRangeRate() + aErrors.mRangeRateError);
   }
   else
   {
      aResult.mMeasurement.SetSphericalMeasurement(GetSensorMode()->GetSimulation()->GetSimTime(), az, el, range, 0.0);
   }

   // Set the WCS location consistently, depending on whether the az/el is relative to the antenna or the sensor.
   double relativeTgtLocWCS[3];
   double reportedLocWCS[3];
   if ((aResult.GetReceiver() != nullptr) && (aResult.GetReceiver()->GetAntenna() != nullptr))
   {
      auto* antennaPtr = aResult.GetReceiver()->GetAntenna();
      // Get the WCS vector to the reported location.
      antennaPtr->GetRelativeLocationWCS(az, el, range, relativeTgtLocWCS);
      // Update the reported location in WCS
      antennaPtr->GetLocationWCS(relativeTgtLocWCS, reportedLocWCS);
   }
   else
   {
      // Get the WCS vector to the reported location.
      GetSensorMode()->GetSensor()->GetRelativeLocationWCS(az, el, range, relativeTgtLocWCS);
      // Update the reported location in WCS
      GetSensorMode()->GetSensor()->GetLocationWCS(relativeTgtLocWCS, reportedLocWCS);
   }

   aResult.mMeasurement.SetLocationWCS(reportedLocWCS);
}

StandardSensorErrorModel::Sigmas::Sigmas(double aAzErrorStdDev,
                                         double aElErrorStdDev,
                                         double aRangeErrorStdDev,
                                         double aRangeRateErrorStdDev)
   : mAzErrorStdDev(aAzErrorStdDev)
   , mElErrorStdDev(aElErrorStdDev)
   , mRangeErrorStdDev(aRangeErrorStdDev)
   , mRangeRateErrorStdDev(aRangeRateErrorStdDev)
{
}

} // namespace wsf
