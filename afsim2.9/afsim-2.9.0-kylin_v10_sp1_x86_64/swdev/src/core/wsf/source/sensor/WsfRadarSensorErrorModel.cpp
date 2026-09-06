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

#include "WsfRadarSensorErrorModel.hpp"

#include <string>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "WsfRadarSensor.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSimulation.hpp"

namespace wsf
{

RadarSensorErrorModel* RadarSensorErrorModel::Clone() const
{
   return new RadarSensorErrorModel(*this);
}

//! Factory method for SensorErrorModelTypes.
//! This is called by SensorErrorModelTypes::LoadType to determine if a pattern
//! represented by this class is being requested.
// static
std::unique_ptr<SensorErrorModelBase> RadarSensorErrorModel::ObjectFactory(const std::string& aTypeName)
{
   if ((aTypeName == "WSF_RADAR_SENSOR_ERROR") || // WSF_RADAR_SENSOR_ERROR is undocumented and will be removed in the future.
       (aTypeName == "RADAR_SENSOR_ERROR") || // RADAR_SENSOR_ERROR is undocumented and will be removed in the future.
       (aTypeName == "radar_sensor_error"))
   {
      return ut::make_unique<RadarSensorErrorModel>();
   }
   return nullptr;
}

bool RadarSensorErrorModel::Initialize(WsfSensorMode* aSensorModePtr)
{
   bool ok = SensorErrorModel::Initialize(aSensorModePtr);
   if (dynamic_cast<WsfRadarSensor*>(aSensorModePtr->GetSensor()) == nullptr)
   {
      auto out = ut::log::error() << "Unable to use 'error_model' on sensor types that are note WSF_RADAR_SENSORs.";
      out.AddNote() << "Model: " << GetName();
      ok = false;
   }
   return ok;
}

void RadarSensorErrorModel::ApplyMeasurementErrors(const SphericalMeasurementErrors& aErrors, WsfSensorResult& aResult)
{
   // Apply the error
   // Use the true Az/El to avoid propagating wave bending effects to the position calculation.

   WsfMeasurement& measurement = aResult.mMeasurement;
   measurement.SetUpdateTime(GetSensorMode()->GetSimulation()->GetSimTime());
   double rcvrToTgtLocWCS[3];
   measurement.SetRange(aResult.mRcvrToTgt.mRange + aErrors.mRangeError);
   measurement.SetRangeValid(true);
   measurement.SetSensorAzimuth(aResult.mRcvrToTgt.mTrueAz + aErrors.mAzError);
   measurement.SetSensorAzimuthValid(true);
   measurement.SetSensorElevation(aResult.mRcvrToTgt.mTrueEl + aErrors.mElError);
   measurement.SetSensorElevationValid(true);

   aResult.GetReceiver()->GetAntenna()->GetRelativeLocationWCS(measurement.GetSensorAzimuth(),
                                                               measurement.GetSensorElevation(),
                                                               measurement.GetRange(),
                                                               rcvrToTgtLocWCS);
   double reportedLocWCS[3];
   aResult.GetReceiver()->GetAntenna()->GetLocationWCS(rcvrToTgtLocWCS, reportedLocWCS);
   measurement.SetLocationWCS(reportedLocWCS);

   if (measurement.RangeRateValid())
   {
      measurement.SetRangeRate(measurement.GetRangeRate() + aErrors.mRangeRateError);
   }
}

bool RadarSensorErrorModel::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "override_measurement_with_truth")
   {
      aInput.ReadValue(mOverrideMeasurementWithTruth);
   }
   else if (command == "compute_measurement_errors")
   {
      aInput.ReadValue(mComputeMeasurementErrors);
   }
   else
   {
      myCommand = StandardSensorErrorModel::ProcessInput(aInput);
   }

   return myCommand;
}

SphericalMeasurementErrors RadarSensorErrorModel::GetSphericalErrorStdDev(ut::Random&      aRandom,
                                                                          WsfSensorResult& aResult,
                                                                          const Sigmas&    aSigmas) const
{
   if (mComputeMeasurementErrors)
   {
      return GetRadarSphericalErrorStdDev(aRandom, aResult);
   }
   if (mOverrideMeasurementWithTruth)
   {
      return SphericalMeasurementErrors(); // All errors zero.
   }
   return StandardSensorErrorModel::GetSphericalErrorStdDev(aRandom, aResult, aSigmas);
}

SphericalMeasurementErrors RadarSensorErrorModel::GetRadarSphericalErrorStdDev(ut::Random&      aRandom,
                                                                               WsfSensorResult& aResult) const
{
   // Compute the measurement errors using techniques that are specific to a radar sensor.

   int n = 1; // Could be > 1 for non-coherent integration....

   double temp = sqrt(2.0 * n * aResult.mSignalToNoise);

   WsfSensorMode*                   snsrModePtr = GetSensorMode();
   const WsfRadarSensor::RadarBeam* beamPtr =
      dynamic_cast<WsfRadarSensor::RadarBeam*>(snsrModePtr->GetBeamEntry(aResult.mBeamIndex));
   const WsfEM_Rcvr* rcvrPtr = aResult.GetReceiver();
   const WsfEM_Xmtr* xmtrPtr = aResult.GetTransmitter();

   // Compute the standard deviation of the azimuth error.
   double azBeamwidth = beamPtr->mErrorModelAzBeamwidth;
   if (azBeamwidth < 0.0)
   {
      azBeamwidth = rcvrPtr->GetAzimuthBeamwidth(aResult.mRcvrBeam.mEBS_Az, aResult.mRcvrBeam.mEBS_El);
   }
   double azErrorSigma = azBeamwidth / temp;

   // Compute the standard deviation of the elevation error.
   double elBeamwidth = beamPtr->mErrorModelElBeamwidth;
   if (elBeamwidth < 0.0)
   {
      elBeamwidth = rcvrPtr->GetElevationBeamwidth(aResult.mRcvrBeam.mEBS_Az, aResult.mRcvrBeam.mEBS_El);
   }
   double elErrorSigma = elBeamwidth / temp;

   // Compute the standard deviation of the range error.
   double pulseWidth = beamPtr->mErrorModelPulseWidth;
   if (pulseWidth < 0.0)
   {
      pulseWidth = xmtrPtr->GetPulseWidth();
      if (pulseWidth > 0.0)
      {
         // Pulse width is defined.
      }
      else if (rcvrPtr->GetBandwidth() > 0.0)
      {
         // Assume a matched filter.
         pulseWidth = 1.0 / rcvrPtr->GetBandwidth();
      }
      else
      {
         pulseWidth = 0.0;
      }
      // Account for processing gains due to pulse compression.
      pulseWidth /= xmtrPtr->GetPulseCompressionRatio();
   }
   double rangeErrorSigma = 0.0;
   if (pulseWidth > 0.0)
   {
      rangeErrorSigma = (pulseWidth * UtMath::cLIGHT_SPEED) / (2.0 * temp);
      // equation: c / (2 * B * sqrt(2(S/N))) from "Radar System Performance Modeling" - G. Richard Curry Page 168
      // Note that pulseWidth is the inverse of B, B = waveform bandwidth
   }

   // Compute the standard deviation of the range rate error
   double dopplerResolution = beamPtr->mErrorModelDopplerResolution;
   if (dopplerResolution < 0.0)
   {
      dopplerResolution = beamPtr->GetDopplerResolution();
   }
   double rangeRateErrorSigma = 0.0;
   if (dopplerResolution > 0.0)
   {
      rangeRateErrorSigma = dopplerResolution / temp;
      // equation: delta V / sqrt(2(S/N)) from "Radar System Performance Modeling" - G. Richard Curry Page 172
   }

   WsfMeasurement& measurement = aResult.mMeasurement;

   SphericalMeasurementErrors errors;
   measurement.SetSensorAzimuthError(0.0);
   if ((snsrModePtr->ReportsBearing() || snsrModePtr->ReportsLocation()) && (azErrorSigma != 0.0))
   {
      measurement.SetSensorAzimuthError(azErrorSigma);
      errors.mAzError = aRandom.Gaussian() * azErrorSigma;
   }
   measurement.SetSensorElevationError(0.0);
   if ((snsrModePtr->ReportsElevation() || snsrModePtr->ReportsLocation()) && (elErrorSigma != 0.0))
   {
      measurement.SetSensorElevationError(elErrorSigma);
      errors.mElError = aRandom.Gaussian() * elErrorSigma;
   }
   measurement.SetRangeError(0.0);
   if ((snsrModePtr->ReportsRange() || snsrModePtr->ReportsLocation()) && (rangeErrorSigma != 0.0))
   {
      measurement.SetRangeError(rangeErrorSigma);
      errors.mRangeError = aRandom.Gaussian() * rangeErrorSigma;
   }
   measurement.SetRangeRateError(0.0);
   if (snsrModePtr->ReportsRangeRate() && (rangeRateErrorSigma != 0.0))
   {
      measurement.SetRangeRateError(rangeRateErrorSigma);
      errors.mRangeRateError = aRandom.Gaussian() * rangeRateErrorSigma;
   }

   return errors;
}

} // namespace wsf
