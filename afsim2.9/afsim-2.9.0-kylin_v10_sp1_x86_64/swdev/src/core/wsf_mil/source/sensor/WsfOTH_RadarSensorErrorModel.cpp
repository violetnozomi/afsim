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

#include "WsfOTH_RadarSensorErrorModel.hpp"

#include <string>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "WsfOTH_RadarSensor.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSensorResult.hpp"

namespace wsf
{

OTH_RadarSensorErrorModel* OTH_RadarSensorErrorModel::Clone() const
{
   return new OTH_RadarSensorErrorModel(*this);
}

//! Factory method for WsfSensorErrorModelTypes.
//! This is called by WsfSensorErrorModelTypes::LoadType to determine if a pattern
//! represented by this class is being requested.
// static
std::unique_ptr<SensorErrorModelBase> OTH_RadarSensorErrorModel::ObjectFactory(const std::string& aTypeName)
{
   if ((aTypeName ==
        "WSF_OTH_RADAR_SENSOR_ERROR") || // WSF_OTH_RADAR_SENSOR_ERROR is undocumented and will be removed in the future.
       (aTypeName == "OTH_RADAR_SENSOR_ERROR") || // OTH_RADAR_SENSOR_ERROR is undocumented and will be removed in the future.
       (aTypeName == "oth_radar_sensor_error"))
   {
      return ut::make_unique<OTH_RadarSensorErrorModel>();
   }
   return nullptr;
}

bool OTH_RadarSensorErrorModel::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "compute_measurement_errors")
   {
      aInput.ReadValue(mComputeMeasurementErrors);
   }
   else
   {
      myCommand = StandardSensorErrorModel::ProcessInput(aInput);
   }

   return myCommand;
}

bool OTH_RadarSensorErrorModel::Initialize(WsfSensorMode* aSensorModePtr)
{
   bool ok = wsf::StandardSensorErrorModel::Initialize(aSensorModePtr);
   if (dynamic_cast<WsfOTH_RadarSensor*>(aSensorModePtr->GetSensor()) == nullptr)
   {
      auto out =
         ut::log::error() << "error_model: Not able to be used on sensor types that are not WSF_OTH_RADAR_SENSOR.";
      out.AddNote() << "Model Name: " << GetName();
      ok = false;
   }
   return ok;
}

SphericalMeasurementErrors OTH_RadarSensorErrorModel::GetSphericalErrorStdDev(ut::Random&      aRandom,
                                                                              WsfSensorResult& aResult,
                                                                              const Sigmas&    aSigmas) const
{
   if (mComputeMeasurementErrors)
   {
      return GetOTH_RadarSphericalErrorStdDev(aRandom, aResult, aSigmas);
   }
   return StandardSensorErrorModel::GetSphericalErrorStdDev(aRandom, aResult, aSigmas);
}

wsf::SphericalMeasurementErrors OTH_RadarSensorErrorModel::GetOTH_RadarSphericalErrorStdDev(ut::Random&      aRandom,
                                                                                            WsfSensorResult& aResult,
                                                                                            const Sigmas& aSigmas) const
{
   // Compute the measurement errors using techniques that are specific to a radar sensor.

   int n = 1; // Could be > 1 for non-coherent integration....

   double temp = sqrt(2.0 * n * aResult.mSignalToNoise);

   WsfSensorMode*                snsrModePtr = GetSensorMode();
   WsfOTH_RadarSensor::OTH_Beam* beamPtr =
      dynamic_cast<WsfOTH_RadarSensor::OTH_Beam*>(snsrModePtr->GetBeamEntry(aResult.mBeamIndex));
   WsfEM_Rcvr* rcvrPtr = aResult.GetReceiver();
   WsfEM_Xmtr* xmtrPtr = aResult.GetTransmitter();

   // Compute the standard deviation of the azimuth error.
   double azBeamwidth = beamPtr->mErrorModelAzBeamwidth;
   if (azBeamwidth < 0.0)
   {
      azBeamwidth = rcvrPtr->GetAzimuthBeamwidth(0.0, 0.0);
   }
   double azErrorSigma = azBeamwidth / temp;

   // Compute the standard deviation of the elevation error.
   double elBeamwidth = beamPtr->mErrorModelElBeamwidth;
   if (elBeamwidth < 0.0)
   {
      elBeamwidth = rcvrPtr->GetElevationBeamwidth(0.0, 0.0);
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
      // equation: c / (2 * B * sqrt(2(S / N))) from "Radar System Performance Modeling" - G. Richard Curry Page 168
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
      // equation: delta V / sqrt(2(S / N)) from "Radar System Performance Modeling" - G. Richard Curry Page 172
   }

   WsfMeasurement&                 measurement = aResult.mMeasurement;
   wsf::SphericalMeasurementErrors errors;

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
