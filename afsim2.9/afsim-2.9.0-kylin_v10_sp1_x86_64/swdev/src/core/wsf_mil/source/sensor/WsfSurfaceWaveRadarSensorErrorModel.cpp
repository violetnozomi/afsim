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

#include "WsfSurfaceWaveRadarSensorErrorModel.hpp"

#include <string>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSimulation.hpp"

namespace wsf
{

SurfaceWaveRadarSensorErrorModel* SurfaceWaveRadarSensorErrorModel::Clone() const
{
   return new SurfaceWaveRadarSensorErrorModel(*this);
}

//! Factory method for WsfSensorErrorModelTypes.
//! This is called by WsfSensorErrorModelTypes::LoadType to determine if a pattern
//! represented by this class is being requested.
// static
std::unique_ptr<SensorErrorModelBase> SurfaceWaveRadarSensorErrorModel::ObjectFactory(const std::string& aTypeName)
{
   if ((aTypeName == "WSF_SURFACE_WAVE_RADAR_SENSOR_ERROR") || // WSF_SURFACE_WAVE_RADAR_SENSOR_ERROR is undocumented
                                                               // and will be removed in the future.
       (aTypeName == "SURFACE_WAVE_RADAR_SENSOR_ERROR") || // SURFACE_WAVE_RADAR_SENSOR_ERROR is undocumented and will
                                                           // be removed in the future.
       (aTypeName == "surface_wave_radar_sensor_error"))
   {
      return ut::make_unique<SurfaceWaveRadarSensorErrorModel>();
   }
   return nullptr;
}

bool SurfaceWaveRadarSensorErrorModel::Initialize(WsfSensorMode* aSensorModePtr)
{
   bool ok = wsf::StandardSensorErrorModel::Initialize(aSensorModePtr);
   if (dynamic_cast<WsfSurfaceWaveRadarSensor*>(aSensorModePtr->GetSensor()) == nullptr)
   {
      auto out =
         ut::log::error()
         << "Specified 'error_model' not able to be used on sensor types that are not WSF_SURFACE_WAVE_RADAR_SENSOR.";
      out.AddNote() << "error_model: " << GetName();
      ok = false;
   }
   else
   {
      mSurfaceWaveRadarModePtr = static_cast<WsfSurfaceWaveRadarSensor::RadarMode*>(aSensorModePtr);
   }
   return ok;
}

bool SurfaceWaveRadarSensorErrorModel::ProcessInput(UtInput& aInput)
{
   bool        myCommand = false;
   std::string command(aInput.GetCommand());

   if (command == "error_model_parameters")
   {
      myCommand = true;
      UtInputBlock block(aInput);
      while (block.ReadCommand(command))
      {
         if (command == "azimuth_beamwidth")
         {
            aInput.ReadValueOfType(mErrorModelAzBeamwidth, UtInput::cANGLE);
            aInput.ValueGreater(mErrorModelAzBeamwidth, 0.0);
         }
         else if (command == "elevation_beamwidth")
         {
            aInput.ReadValueOfType(mErrorModelElBeamwidth, UtInput::cANGLE);
            aInput.ValueGreater(mErrorModelElBeamwidth, 0.0);
         }
         else if (command == "pulse_width")
         {
            aInput.ReadValueOfType(mErrorModelPulseWidth, UtInput::cTIME);
            aInput.ValueGreater(mErrorModelPulseWidth, 0.0);
         }
         else if (command == "receiver_bandwidth")
         {
            // Derive the pulse width assuming a matched filter.
            double bandwidth;
            aInput.ReadValueOfType(bandwidth, UtInput::cFREQUENCY);
            aInput.ValueGreater(bandwidth, 0.0);
            mErrorModelPulseWidth = 1.0 / bandwidth;
         }
         else if (command == "doppler_resolution")
         {
            aInput.ReadValueOfType(mErrorModelDopplerResolution, UtInput::cSPEED);
            aInput.ValueGreater(mErrorModelDopplerResolution, 0.0);
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }

   return myCommand;
}

SphericalMeasurementErrors SurfaceWaveRadarSensorErrorModel::GetSphericalErrorStdDev(ut::Random&      aRandom,
                                                                                     WsfSensorResult& aResult,
                                                                                     const Sigmas&    aSigmas) const
{
   // Compute the measurement errors using techniques that are specific to a radar sensor.
   int n = 1; // Could be > 1 for non-coherent integration....

   double temp = sqrt(2.0 * n * aResult.mSignalToNoise);

   const WsfSurfaceWaveRadarSensor::RadarMode* snsrModePtr =
      dynamic_cast<WsfSurfaceWaveRadarSensor::RadarMode*>(GetSensorMode());
   const WsfEM_Rcvr* rcvrPtr = aResult.GetReceiver();
   const WsfEM_Xmtr* xmtrPtr = aResult.GetTransmitter();

   // Compute the standard deviation of the azimuth error.
   double azBeamwidth = mErrorModelAzBeamwidth;
   if (azBeamwidth < 0.0)
   {
      azBeamwidth = xmtrPtr->GetAzimuthBeamwidth(0.0, 0.0);
   }
   double azErrorSigma = azBeamwidth / temp;

   // Compute the standard deviation of the elevation error.
   double elBeamwidth = mErrorModelElBeamwidth;
   if (elBeamwidth < 0.0)
   {
      elBeamwidth = xmtrPtr->GetElevationBeamwidth(0.0, 0.0);
   }
   double elErrorSigma = elBeamwidth / temp;

   // Compute the standard deviation of the range error.
   double pulseWidth = mErrorModelPulseWidth;
   if (pulseWidth < 0.0)
   {
      pulseWidth = xmtrPtr->GetPulseWidth();
      if ((pulseWidth > 0.0) && (pulseWidth < 0.99))
      {
         // Pulse width is defined and isn't a continuous wave.
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
   double dopplerResolution = mErrorModelDopplerResolution;
   if (dopplerResolution < 0.0)
   {
      dopplerResolution = snsrModePtr->GetDopplerResolution();
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

void SurfaceWaveRadarSensorErrorModel::ApplyMeasurementErrors(const wsf::SphericalMeasurementErrors& aErrors,
                                                              WsfSensorResult&                       aResult)
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
   mSurfaceWaveRadarModePtr->GetEM_Rcvr()->GetAntenna()->GetRelativeLocationWCS(measurement.GetSensorAzimuth(),
                                                                                measurement.GetSensorElevation(),
                                                                                measurement.GetRange(),
                                                                                rcvrToTgtLocWCS);
   double reportedLocWCS[3];
   mSurfaceWaveRadarModePtr->GetEM_Rcvr()->GetAntenna()->GetLocationWCS(rcvrToTgtLocWCS, reportedLocWCS);
   measurement.SetLocationWCS(reportedLocWCS);

   if (measurement.RangeRateValid())
   {
      measurement.SetRangeRate(measurement.GetRangeRate() + aErrors.mRangeRateError);
   }
}

} // namespace wsf
