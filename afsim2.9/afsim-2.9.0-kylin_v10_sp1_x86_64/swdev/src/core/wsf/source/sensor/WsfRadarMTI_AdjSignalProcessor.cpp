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

#include "WsfRadarMTI_AdjSignalProcessor.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMath.hpp"
#include "WsfEM_Clutter.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfEnvironment.hpp"
#include "WsfPlatform.hpp"
#include "WsfRadarSensor.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"

namespace
{
using MTI_AdjustmentTable = WsfMTI_TableManager::MTI_AdjustmentTable;
}

// ================================================================================================
//! Constructor
WsfRadarMTI_AdjSignalProcessor::WsfRadarMTI_AdjSignalProcessor(MTI_Type aPulseDopplerType)
   : WsfSensorSignalProcessor()
   , mFilterOwnshipFromDoppler(true)
   , mMTI_Type(aPulseDopplerType)
   , mMTI_MinResponse(0.0)
   , mResponseTable()
   , mMTI_AdjustmentTablePtr(nullptr)
   , mMTI_TableMaxRange(1.0e38)
   , mMTI_MaxRange(0.0)
   , mMTI_InterpulsePeriod_1(0.0)
   , mMTI_InterpulsePeriod_2(0.0)
   , mMTI_StabilityConstant(0.0)
   , mMTI_NumberOfStages(1)
   , mMTI_ClutterLock(true)
   , mMTI_UpWind(true)
   , mClutterAttenuationFactor(1.0)
   , mInputClutterAttenuationFactor(1.0)
{
   UtVec3d::Set(mMTI_Weights, 0.0);
}

// ================================================================================================
//! The object factory for the MTI signal processor type.
// static
WsfSensorSignalProcessor* WsfRadarMTI_AdjSignalProcessor::ObjectFactory(const std::string& aTypeName)
{
   WsfSensorSignalProcessor* instancePtr = nullptr;

   if ((aTypeName == "gmti_adjustment") || (aTypeName == "GMTI_ADJUSTMENT"))
   {
      instancePtr = new WsfRadarMTI_AdjSignalProcessor(cMTIT_ADJUSTMENT_TABLE);
   }
   else if ((aTypeName == "mti_processor") || (aTypeName == "MTI_PROCESSOR") || (aTypeName == "dual_delay_canceller"))
   {
      instancePtr = new WsfRadarMTI_AdjSignalProcessor(cMTIT_PROCESSOR);
   }
   // else if ((aTypeName == "mti_table") ||
   //    (aTypeName == "MTI_TABLE"))
   //{
   //    instancePtr = new WsfRadarMTI_AdjSignalProcessor(cMTIT_DATA_TABLE);
   // }
   return instancePtr;
}

// ================================================================================================
// virtual
WsfSensorSignalProcessor* WsfRadarMTI_AdjSignalProcessor::Clone() const
{
   return new WsfRadarMTI_AdjSignalProcessor(*this);
}

// ================================================================================================
// virtual
bool WsfRadarMTI_AdjSignalProcessor::Initialize(double         aSimTime,
                                                WsfSensor*     aSensorPtr,
                                                WsfSensorMode* aModePtr,
                                                size_t         aBeamIndex /* = 0*/)
{
   bool ok = true;

   // Verify input based on MTI response mode
   if (mMTI_Type == cMTIT_DATA_TABLE) // MTI type is Data Table
   {
      // Verify that a data table is present
      if (!mResponseTable.IsDefined())
      {
         ut::log::error() << "Must specify 'mti_data_table' and valid data for MTI type of 'data_table'.";
         ok = false;
      }
   }
   else if (mMTI_Type == cMTIT_PROCESSOR) // MTI type is dual delay canceler
   {
      WsfRadarSensor::RadarMode* radarModePtr = dynamic_cast<WsfRadarSensor::RadarMode*>(aModePtr);
      if (radarModePtr == nullptr)
      {
         ut::log::error() << "Sensor Mode must be of type WsfRadarSensor:RadarMode.";
         return false;
      }

      WsfRadarSensor::RadarBeam* beamPtr = radarModePtr->mBeamList[aBeamIndex];

      mInputClutterAttenuationFactor = beamPtr->GetClutterAttenuationFactor();
      if (mInputClutterAttenuationFactor < 1.0)
      {
         mClutterAttenuationFactor = mInputClutterAttenuationFactor;
      }

      // Ensure a PRF was specified.
      if (beamPtr->mXmtrPtr->GetPulseRepetitionFrequency() <= 0.0)
      {
         ut::log::error() << "Must specify 'pulse_repetition_frequency' with 'mti_processor'.";
         return false;
      }

      // Ensure the maximum MTI range was specified
      if (mMTI_MaxRange <= 0.0)
      {
         ut::log::error() << "Must specify 'maximum_range' in 'mti_processor'";
         return false;
      }

      // Compute the weights based upon the interpulse_periods entered into the block.
      // If no interpulse_periods have been entered, then the use the inverse of the PRF.

      if ((mMTI_InterpulsePeriod_1 == 0.0) && (mMTI_InterpulsePeriod_2 == 0.0))
      {
         // Set the interpulse periods to the inverse of the PRF (Non-staggered MTI system).
         mMTI_InterpulsePeriod_1 = 1.0 / beamPtr->mXmtrPtr->GetPulseRepetitionFrequency();
         mMTI_InterpulsePeriod_2 = mMTI_InterpulsePeriod_1;
      }
      else if ((mMTI_InterpulsePeriod_1 == 0.0) || (mMTI_InterpulsePeriod_2 == 0.0))
      {
         ut::log::error() << "Must specify both 'interpulse_period_1 and 2' in 'mti_processor'";
         return false;
      }

      mMTI_Weights[0] = 1.0;
      mMTI_Weights[1] = -(mMTI_InterpulsePeriod_1 + mMTI_InterpulsePeriod_2) / mMTI_InterpulsePeriod_2;
      mMTI_Weights[2] = mMTI_InterpulsePeriod_1 / mMTI_InterpulsePeriod_2;

      // Compute the clutter attenuation factor if not explicitly specified.
      if (mInputClutterAttenuationFactor >= 1.0)
      {
         double              sigma;
         double              sigmav;
         static const double v0iss[2][7] = {{0.06, 0.36, 0.58, 0.95, 1.34, 1.65, 1.95},
                                            {0.06, 1.22, 1.80, 2.20, 2.46, 2.70, 2.92}};
         static const double sigvis[7]   = {0.01, 0.30, 0.62, 0.94, 1.25, 1.56, 1.80};

         WsfEnvironment& environment = aSensorPtr->GetScenario().GetEnvironment();
         int             seaState    = environment.GetSeaState();
         seaState                    = std::min(std::max(seaState, 0), 6);
         // double integratedPulseCount = beam.ComputeIntegratedPulseCount(radarModePtr);

         // Get the number of pulses being integrated,
         // This is similar to the WsfRadarSensor::RadarBeam::ComputeIntegratedPulseCount(RadarMode& aMode)
         double                     numPulseIntegrated = 1; // Assuming continuous wave...
         WsfRadarSensor::RadarBeam* radarBeamPtr       = radarModePtr->mBeamList[beamPtr->mRcvrPtr->GetIndex()];
         if ((radarBeamPtr != nullptr) && radarBeamPtr->UsingDetector())
         {
            numPulseIntegrated = (double)(radarBeamPtr->GetNumberOfPulsesIntegrated());
         }
         else if (beamPtr->mXmtrPtr->GetPulseRepetitionFrequency() != 0.0)
         {
            // Pulsed radar...
            double timeOnTarget = 0.0;
            if (radarModePtr->GetDwellTime() == 0.0)
            {
               if (beamPtr->mXmtrPtr->GetAntenna()->GetScanMode() == WsfEM_Antenna::cSCAN_EL)
               {
                  double minEl, maxEl;
                  beamPtr->mXmtrPtr->GetAntenna()->GetElevationScanLimits(minEl, maxEl);
                  timeOnTarget =
                     radarModePtr->GetFrameTime() * beamPtr->mXmtrPtr->GetElevationBeamwidth(0.0, 0.0) / (maxEl - minEl);
               }
               else
               {
                  double minAz, maxAz;
                  beamPtr->mXmtrPtr->GetAntenna()->GetAzimuthScanLimits(minAz, maxAz);
                  timeOnTarget =
                     radarModePtr->GetFrameTime() * beamPtr->mXmtrPtr->GetAzimuthBeamwidth(0.0, 0.0) / (maxAz - minAz);
               }
            }
            else
            {
               // Tracker
               timeOnTarget = radarModePtr->GetDwellTime();
            }
            numPulseIntegrated = timeOnTarget * beamPtr->mXmtrPtr->GetPulseRepetitionFrequency();
         }

         if (aSensorPtr->GetPlatform()->GetSpatialDomain() == WSF_SPATIAL_DOMAIN_SURFACE)
         {
            // Use Sea State 1 for now
            sigma = sigvis[seaState];
         }
         else
         {
            double windSpeed = environment.GetWindSpeed();
            double temp      = 1.29 * log10(windSpeed / 0.01);
            sigma            = 0.000018 * pow(10.0, temp);
         }

         double wavelength = UtMath::cLIGHT_SPEED / beamPtr->mXmtrPtr->GetFrequency();
         if (beamPtr->mAntennaPtr->GetScanMode() != WsfEM_Antenna::cSCAN_FIXED)
         {
            // Antenna scanning (not applicable in track mode)

            // Assume antenna scans in elevation and azimuth, assume 360 deg az scan sector.
            double sector = UtMath::cTWO_PI;
            if (beamPtr->mAntennaPtr->GetScanMode() == WsfEM_Antenna::cSCAN_AZ)
            {
               // Azimuth scans in azimuth only.
               double azMin, azMax;
               beamPtr->mAntennaPtr->GetAzimuthScanLimits(azMin, azMax);
               sector = azMax - azMin;
            }
            else
            {
            }
            // ut::log::info() << "sector=" << sector * UtMath::cDEG_PER_RAD;
            double sigsca = ((1.0 / aModePtr->GetFrameTime()) * sector * wavelength) /
                            (10.7 * beamPtr->mRcvrPtr->GetAzimuthBeamwidth(0.0, 0.0));
            sigmav = sqrt((sigma * sigma) + (sigsca * sigsca));
         }
         else
         {
            // Antenna Tracking
            sigmav = sigma;
         }

         double v0 = 0.0;
         if (mMTI_ClutterLock)
         {
            v0 = sigmav / sqrt(numPulseIntegrated);
         }
         else if (mMTI_UpWind)
         {
            int ipol = (beamPtr->mXmtrPtr->GetPolarization() == WsfEM_Types::cPOL_HORIZONTAL) ? 1 : 0;
            v0       = v0iss[ipol][seaState];
         }

         // Compute limit on clutter attenuation resulting from clutter
         // motion and antenna scanning for a 2-stage canceller.
         // Use maximally flat weights (which reduce to binomial weights for
         // unstaggered systems

         // Weights not calculated here, using R. Hackett's results,
         // Weights are calculated in REFRANG, passed into program

         double temp, temp1;
         double x1 = UtMath::cTWO_PI * sigmav / wavelength;
         double x2 = UtMath::cFOUR_PI * v0 / wavelength;

         temp         = x1 * mMTI_InterpulsePeriod_1;
         temp1        = cos(x2 * mMTI_InterpulsePeriod_1);
         double rhot1 = exp(-2.0 * temp * temp) * temp1;

         temp         = x1 * mMTI_InterpulsePeriod_2;
         temp1        = cos(x2 * mMTI_InterpulsePeriod_2);
         double rhot2 = exp(-2.0 * temp * temp) * temp1;

         temp           = x1 * (mMTI_InterpulsePeriod_1 + mMTI_InterpulsePeriod_2);
         temp1          = cos(x2 * (mMTI_InterpulsePeriod_1 + mMTI_InterpulsePeriod_2));
         double rhot1t2 = exp(-2.0 * temp * temp) * temp1;

         // ca is in units of power (volts^2)
         temp         = mMTI_Weights[0] * mMTI_Weights[0];
         temp1        = mMTI_Weights[1] * mMTI_Weights[1];
         double temp2 = mMTI_Weights[2] * mMTI_Weights[2];
         double ca    = 1.0 / ((temp + temp1 + temp2) + (2.0 * mMTI_Weights[0] * mMTI_Weights[1] * rhot1) +
                            (2.0 * mMTI_Weights[1] * mMTI_Weights[2] * rhot2) +
                            (2.0 * mMTI_Weights[0] * mMTI_Weights[2] * rhot1t2));

         // Compute limit on clutter attenuation resulting from  transmitter instability - stabconst
         // *****NOTE: Value is hardwired into present simulation*****
         // Determine overall clutter attenuation - cat & catdb
         //
         // The above NOTE was not correct. The user had to provide it in the input file...
         double stabilityConstant = (mMTI_StabilityConstant > 0.0) ? mMTI_StabilityConstant : 100.0;

         double cati               = (1.0 / sqrt(ca)) + (1.0 / sqrt(stabilityConstant));
         double cat                = 1.0 / cati;
         mClutterAttenuationFactor = 1.0 / (cat * cat);

         // ut::log::info() << "Clutter initialization";
         // ut::log::info() << "sigma=" << sigma;
         // ut::log::info() << "sigmav=" << sigmav;
         // ut::log::info() << "npi=" << integratedPulseCount;
         // ut::log::info() << "v0=" << v0;
         // ut::log::info() << "ca=" << ca;
         // double catdb = UtMath::LinearToDB(cat * cat);           // 20.0 * log10(cat)
         // ut::log::info() << "catdb=" << catdb;
      }
   }

   if (mDebug)
   {
      auto out = ut::log::debug() << "Initializing MTI response for sensor.";
      out.AddNote() << "Response Type: " << mMTI_Type;
      out.AddNote() << "Platform: " << aSensorPtr->GetPlatform()->GetName();
      out.AddNote() << "Sensor: " << aSensorPtr->GetName();
   }

   return ok;
}

// ================================================================================================
// virtual
bool WsfRadarMTI_AdjSignalProcessor::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;

   std::string command = aInput.GetCommand();

   if ((mMTI_Type == cMTIT_DATA_TABLE) && ProcessDataTableInput(aInput))
   {
   }
   else if ((mMTI_Type == cMTIT_ADJUSTMENT_TABLE) && ProcessAdjustmentTableInput(aInput))
   {
   }
   else if ((mMTI_Type == cMTIT_PROCESSOR) && ProcessMTI_ProcessorInput(aInput))
   {
   }
   else if (command == "unfiltered_doppler_speed")
   {
      mFilterOwnshipFromDoppler = false;
   }
   else if (command == "filtered_doppler_speed")
   {
      mFilterOwnshipFromDoppler = true;
   }
   else
   {
      myCommand = WsfSensorSignalProcessor::ProcessInput(aInput);
   }

   return myCommand;
}

bool WsfRadarMTI_AdjSignalProcessor::ProcessDataTableInput(UtInput& aInput)
{
   bool myCommand = true;

   std::string command = aInput.GetCommand();

   if (command == "mti_data_table")
   {
      mResponseTable.ProcessInput(aInput,
                                  UtInput::cFREQUENCY,
                                  "hz",
                                  UtTable::ValueGE(0.0),
                                  UtInput::cRATIO,
                                  "dB",
                                  UtTable::ValueGE(0.0));
   }
   else if (command == "mti_min_response")
   {
      aInput.ReadValueOfType(mMTI_MinResponse, UtInput::cRATIO);
      aInput.ValueLessOrEqual(mMTI_MinResponse, 1.0);
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

bool WsfRadarMTI_AdjSignalProcessor::ProcessAdjustmentTableInput(UtInput& aInput)
{
   bool myCommand = true;

   std::string command = aInput.GetCommand();
   if (command == "mti_adjustment_table" || command == "gmti_adjustment_table")
   {
      if (mMTI_AdjustmentTablePtr)
      {
         ut::log::error()
            << "WsfRadarSensor::RadarBeam::ProcessInput: An MTI adjustment table has already been specified.";
         throw UtInput::BadValue(aInput, "1: An MTI adjustment table has already been specified!");
      }

      mMTI_AdjustmentTablePtr = std::make_shared<MTI_AdjustmentTable>();
      ReadTable(aInput, *mMTI_AdjustmentTablePtr);
   }
   else if (command == "mti_adjustment_file")
   {
      if (mMTI_AdjustmentTablePtr)
      {
         ut::log::error()
            << "WsfRadarSensor::RadarBeam::ProcessInput: An MTI adjustment table has already been specified.";
         throw UtInput::BadValue(aInput, "2: An MTI adjustment table has already been specified!");
      }

      std::string fileName;
      aInput.ReadValueQuoted(fileName);
      fileName                = aInput.LocateFile(fileName);
      mMTI_AdjustmentTablePtr = std::make_shared<MTI_AdjustmentTable>();
      if (!WsfMTI_TableManager::GetManager().ReadCsv(fileName, *mMTI_AdjustmentTablePtr))
      {
         throw UtInput::BadValue(aInput, "Unable to process file " + fileName);
      }
   }
   else if (command == "mti_adjustment_compound_file")
   {
      if (mMTI_AdjustmentTablePtr)
      {
         ut::log::error()
            << "WsfRadarSensor::RadarBeam::ProcessInput: An MTI adjustment table has already been specified.";
         throw UtInput::BadValue(aInput, "2: An MTI adjustment table has already been specified!");
      }

      std::string fileName;
      aInput.ReadValueQuoted(fileName);
      fileName = aInput.LocateFile(fileName);
      // This pattern is different, it is returning a subclass of table.
      MTI_AdjustmentTable* tablePtr = WsfMTI_TableManager::GetManager().ReadCsvCompound(fileName);
      if (tablePtr == nullptr)
      {
         throw UtInput::BadValue(aInput, "Unable to process file " + fileName);
      }
      mMTI_AdjustmentTablePtr = std::shared_ptr<MTI_AdjustmentTable>(tablePtr);
   }
   else if (command == "mti_maximum_range")
   {
      aInput.ReadValueOfType(mMTI_TableMaxRange, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(mMTI_MaxRange, 0.0);
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

bool WsfRadarMTI_AdjSignalProcessor::ProcessMTI_ProcessorInput(UtInput& aInput)
{
   bool myCommand = true;

   std::string command = aInput.GetCommand();
   if (command == "maximum_range")
   {
      aInput.ReadValueOfType(mMTI_MaxRange, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(mMTI_MaxRange, 0.0);
   }
   else if (command == "number_of_stages")
   {
      aInput.ReadValue(mMTI_NumberOfStages);
      aInput.ValueGreaterOrEqual(mMTI_NumberOfStages, 1);
   }
   else if (command == "interpulse_period_1")
   {
      aInput.ReadValueOfType(mMTI_InterpulsePeriod_1, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mMTI_InterpulsePeriod_1, 0.0);
   }
   else if (command == "interpulse_period_2")
   {
      aInput.ReadValueOfType(mMTI_InterpulsePeriod_2, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mMTI_InterpulsePeriod_2, 0.0);
   }
   else if (command == "clutter_lock")
   {
      aInput.ReadValue(mMTI_ClutterLock);
   }
   else if (command == "stability_constant")
   {
      aInput.ReadValue(mMTI_StabilityConstant);
      aInput.ValueGreater(mMTI_StabilityConstant, 0.0);
   }
   else if (command == "upwind")
   {
      aInput.ReadValue(mMTI_UpWind);
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

// ================================================================================================
// virtual
void WsfRadarMTI_AdjSignalProcessor::Execute(double aSimTime, WsfSensorResult& aResult)
{
   // Default MTI level, means no change to the radar's signal to noise ratio
   double TgtMTI_Level     = 1.0;
   double ClutterMTI_Level = 1.0;
   // double InterferenceMTI_Level = 1.0;
   double ReceiverNoiseMTI_Level = 1.0;

   if (mDebug)
   {
      auto out = ut::log::debug() << "Prior to applying MTI response:";
      out.AddNote() << "Received Power: " << aResult.mRcvdPower << " W";
      out.AddNote() << "Received Power: " << aResult.mRcvdPower << " W";
      out.AddNote() << "Receiver Noise Power: " << aResult.mRcvrNoisePower << " W";
      out.AddNote() << "Clutter Power: " << aResult.mClutterPower << " W";
      out.AddNote() << "Interference Power: " << aResult.mInterferencePower << " W";
      ;
   }

   // Make sure the interaction has a transmitter
   if (aResult.GetTransmitter() != nullptr)
   {
      if (aResult.mRcvdPower > 0.0)
      {
         TgtMTI_Level = ComputeMTI_Response(aResult, 1);
      }

      if (aResult.mClutterPower > 0.0)
      {
         ClutterMTI_Level = ComputeMTI_Response(aResult, 0);
      }

      // if (aResult.mInterferencePower > 0.0)
      //{
      //    InterferenceMTI_Level = ComputeMTI_Response(aResult, 0);
      // }

      if ((mMTI_Type == cMTIT_PROCESSOR) && (aResult.mRcvrNoisePower > 0.0))
      {
         ReceiverNoiseMTI_Level = ComputeMTI_Response(aResult, 2);
      }
   }

   // Adjust the parameters used to calculate the signal to noise ratio
   // based on the MTI response
   // Defend ourselves from bad input that results in an invalid calculated MTI level
   // if ((MTI_Level >= 0.0)/* &&
   //    (MTI_Level <= 1.0)*/)
   //{
   aResult.mRcvdPower *= TgtMTI_Level;
   aResult.mClutterPower *= ClutterMTI_Level;
   // aResult.mInterferencePower *= InterferenceMTI_Level;
   aResult.mRcvrNoisePower *= ReceiverNoiseMTI_Level;

   if (mDebug)
   {
      auto out = ut::log::debug() << "Power after applying MTI level:";
      { // RAII block
         auto levelNote = out.AddNote() << "Applied Level:";
         levelNote.AddNote() << "Target Level: " << TgtMTI_Level;
         levelNote.AddNote() << "Receiver Noise Level: " << ReceiverNoiseMTI_Level;
         levelNote.AddNote() << "Clutter Level: " << ClutterMTI_Level;
      }
      auto resultNote = out.AddNote() << "Result:";
      resultNote.AddNote() << "Received Power: " << aResult.mRcvdPower << " W";
      resultNote.AddNote() << "Receiver Noise Power: " << aResult.mRcvrNoisePower << " W";
      resultNote.AddNote() << "Clutter Power: " << aResult.mClutterPower << " W";
      resultNote.AddNote() << "Interference Power: " << aResult.mInterferencePower << " W";
   }
   //}
}

// ================================================================================================
// Computes the MTI response based on range and PRF or provides a value from a table.
// @param aTgtBearing                 The target azimuth wrt receiving sensor.
// @param aTgtSlantRange              The slant range from the receiving sensor to the target.
// @param aTgtDopplerFrequency        The target Doppler frequency of the receiving sensor to the target
// @param aPulseRepetitionFrequencies The PRF list in hertz of the transmitting sensor
// @param aTypeFlag                   The type of signal being passed to the calculation, 0=Clutter Attenuation,
// 1=Target Response, 2=Receiver Noise Factor.

// @return Returns a double value of the MTI Response
// private
double WsfRadarMTI_AdjSignalProcessor::ComputeMTI_Response(WsfSensorResult& aResult, const int aTypeFlag)
{
   double MTI_Response = 1.0; // Or 0 dB

   double tgtDopplerFrequency = aResult.ComputeTargetDopplerFrequency(mFilterOwnshipFromDoppler);

   if (mDebug)
   {
      auto out = ut::log::debug() << "Computed MTI result.";
      out.AddNote() << "Type: " << aTypeFlag;
      out.AddNote() << "Minimum MTI response: " << mMTI_MinResponse;
      out.AddNote() << "Target Bearing:" << aResult.mRcvrToTgt.mTrueAz * UtMath::cDEG_PER_RAD << " deg";
      out.AddNote() << "Slant Range: " << aResult.mRcvrToTgt.mRange / 1000.0 << " km";
      out.AddNote() << "Doppler Frequency: " << tgtDopplerFrequency << " Hz";
   }
   if (mMTI_Type == cMTIT_DATA_TABLE)
   {
      // Get MTI response value as an absolute ratio from table for Doppler frequency
      MTI_Response = GetTableResponse(tgtDopplerFrequency);
   }
   else if (mMTI_Type == cMTIT_ADJUSTMENT_TABLE)
   {
      if (aTypeFlag == 1)
      {
         // Apply the MTI adjustment if it was provided
         if (mMTI_AdjustmentTablePtr)
         {
            // Approximation to ground range.
            double cosTheta    = cos(aResult.mRcvrToTgt.mTrueEl);
            double groundRange = aResult.mRcvrToTgt.mRange * cosTheta;
            if (groundRange < mMTI_TableMaxRange)
            {
               // The return value from ComputeTargetDopplerSpeed is negative if the target is closing. We want it the
               // other way around!
               double closingSpeed = -aResult.ComputeTargetDopplerSpeed(mFilterOwnshipFromDoppler);
               double MTI_AdjustmentFactor =
                  mMTI_AdjustmentTablePtr->ComputeAdjustmentFactor(closingSpeed, aResult.GetTransmitter()->GetFrequency());
               MTI_Response = MTI_AdjustmentFactor;
            }
         }
      }
   }
   else // if (mMTI_Type == cMTIT_PROCESSOR)
   {
      MTI_Response = ComputeMTI_Effects(aResult, aTypeFlag);
   }

   // Limit the minimum MTI response
   MTI_Response = std::max(MTI_Response, mMTI_MinResponse);

   if (mDebug)
   {
      ut::log::debug() << "Final MTI response: " << MTI_Response;
   }

   return MTI_Response;
}

// ================================================================================================
// Looks up the MTI response based on slant range value from a table.
// @param aSlantRange  The range in meters of the receiving sensor to the target
// @return Returns a double value of the MTI Response. Returns 1.0 if the table is not valid.
// private
double WsfRadarMTI_AdjSignalProcessor::GetTableResponse(const double aTargetDopplerFrequency)
{
   double response = 1.0;
   if (mResponseTable.IsDefined())
   {
      response = mResponseTable.Lookup(aTargetDopplerFrequency);
      if (mDebug)
      {
         auto out = ut::log::debug() << "MTI response table lookup for target Doppler frequency:";
         out.AddNote() << "Response: " << response;
         out.AddNote() << "Frequency: " << aTargetDopplerFrequency << " Hz";

         ut::log::debug() << "MTI response table lookup: " << response << " for " << aTargetDopplerFrequency << " Hz.";
      }
   }
   return response;
}

// =================================================================================================
//! Compute the clutter attenuation to be applied to the current interaction
//! @param aResult The current interaction.
//! @param aTypeFlag The type of return to return: 0=Clutter Attenuation, 1=Target Response, 2=Receiver Noise Factor.
// private
double WsfRadarMTI_AdjSignalProcessor::ComputeMTI_Effects(WsfSensorResult& aResult, const int aTypeFlag)
{
   double returnValue = 1.0;

   // Approximation to ground range.
   double cosTheta    = cos(aResult.mRcvrToTgt.mTrueEl);
   double groundRange = aResult.mRcvrToTgt.mRange * cosTheta;
   // Perform MTI processing only if within the range limits for MTI processing.
   if (groundRange > mMTI_MaxRange)
   {
      return returnValue;
   }

   if (aTypeFlag == 0) // return the clutter MTI attenuation factor
   {
      if (aResult.mClutterPower > 0.0)
      {
         // Select the clutter attenuation factor that was calculated/set during intialization,
         // dividing out the already set input attenuation taken care of in clutter power calculation.
         returnValue = mClutterAttenuationFactor / mInputClutterAttenuationFactor;
      }
   }
   else if (aTypeFlag == 1) // return the target MTI response
   {
      // Compute the Doppler frequency
      double fc = fabs(aResult.ComputeTargetDopplerFrequency(mFilterOwnshipFromDoppler));

      double prf[2];
      int    numPRFs = 1;
      if (mMTI_InterpulsePeriod_1 != mMTI_InterpulsePeriod_2)
      {
         numPRFs = 2;
         prf[0]  = 1.0 / mMTI_InterpulsePeriod_1;
         prf[1]  = 1.0 / mMTI_InterpulsePeriod_2;
      }

      // Find target response for pulse radar with MTI turned on
      double targetResponse = 0.0;
      if (numPRFs == 1)
      {
         // Use Nathanson method to compute MTI response for nonstaggered multiple delay line system
         WsfEM_Xmtr* xmtrPtr   = aResult.GetTransmitter();
         double pulse_rep_freq = xmtrPtr->GetPulseRepetitionFrequency() > 0.0 ? xmtrPtr->GetPulseRepetitionFrequency() :
                                                                                1.0 / mMTI_InterpulsePeriod_1;
         double ratio          = fc / pulse_rep_freq;
         double temp1          = 2.0 * mMTI_NumberOfStages;
         double temp2          = sin(UtMath::cPI * ratio);
         targetResponse        = pow(2.0, temp1) * pow(temp2, temp1);
      }
      else
      {
         // Weights not calculated here, using R. Hackett's results, weights
         // are calculated in ReferenceRange
         double omegaD = UtMath::cTWO_PI * fc;
         double sum1   = mMTI_Weights[0];
         double sum2   = 0.0;
         double delay  = 0.0;
         for (int j = 0; j < numPRFs; ++j)
         {
            delay           = delay + (1.0 / prf[j]);
            double angle    = omegaD * delay;
            double sinAngle = sin(angle);
            double cosAngle = cos(angle);
            sum1            = sum1 + mMTI_Weights[j + 1] * cosAngle;
            sum2            = sum2 + mMTI_Weights[j + 1] * sinAngle;
         }
         targetResponse = (sum1 * sum1) + (sum2 * sum2);
      }
      returnValue = std::max(targetResponse, 1.0E-6);
   }
   else if (aTypeFlag == 2) // return the receiver noise response
   {
      // Determine average power gain of the MTI system to account for the system noise
      // passing through the MTI Dual Delay Line Canceller.

      // Calculate noise power gain at output of canceller.
      // NO/NI = W1**2 + W2**2 + W3**2 + ...
      double rcvrNoiseFactor =
         mMTI_Weights[0] * mMTI_Weights[0] + mMTI_Weights[1] * mMTI_Weights[1] + mMTI_Weights[2] * mMTI_Weights[2];
      returnValue = rcvrNoiseFactor;
   }
   return returnValue;
}

// =================================================================================================
void WsfRadarMTI_AdjSignalProcessor::ReadTable(UtInput&                                  aInput,
                                               WsfMTI_TableManager::MTI_AdjustmentTable& aMTI_AdjustmentTable)
{
   UtInputBlock inputBlock(aInput);

   std::string         command;
   std::vector<double> speedFrequencyValues;
   std::vector<double> adjustmentValues;
   bool                usedClosingSpeed     = false;
   bool                usedDopplerFrequency = false;
   while (inputBlock.ReadCommand(command))
   {
      double closingValue = 0.0;
      if (command == "closing_speed")
      {
         if (usedDopplerFrequency)
         {
            throw UtInput::OutOfContext(aInput, "Cannot use closing_speed and doppler_frequency in the same table");
         }
         usedClosingSpeed = true;
         aInput.ReadValueOfType(closingValue, UtInput::cSPEED);
         if ((!speedFrequencyValues.empty()) && (closingValue <= speedFrequencyValues.back()))
         {
            throw UtInput::BadValue(aInput, "closing_speed values must be monotonically ascending");
         }
      }
      else if (command == "doppler_frequency")
      {
         if (usedClosingSpeed)
         {
            throw UtInput::OutOfContext(aInput, "Cannot use closing_speed and doppler_frequency in the same table");
         }
         usedDopplerFrequency = true;
         aInput.ReadValueOfType(closingValue, UtInput::cFREQUENCY);
         if ((!speedFrequencyValues.empty()) && (closingValue <= speedFrequencyValues.back()))
         {
            throw UtInput::BadValue(aInput, "doppler_frequency values must be monotonically ascending");
         }
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }

      // Process the part after 'closing_speed' or 'doppler_frequency'

      std::string adjustmentStr;
      aInput.ReadValue(adjustmentStr);
      aInput.StringEqual(adjustmentStr, "adjustment");

      // UtInput will take the text dB value and by unit returns the linear equivilan,
      // which needs to be converted back to dB before storage.
      double adjustment;
      aInput.ReadValueOfType(adjustment, UtInput::cRATIO);
      aInput.ValueGreater(adjustment, 0.0);
      adjustment = UtMath::LinearToDB(adjustment);

      speedFrequencyValues.push_back(closingValue);
      adjustmentValues.push_back(adjustment);
   }

   if (speedFrequencyValues.size() < 2)
   {
      throw UtInput::BadValue(aInput, "mti_adjustment table must have at least two entries");
   }

   // aMTI_AdjustmentTable.SetDopplerFrequency(usedDopplerFrequency);
   // mSpeedFrequencyTable.SetValues(speedFrequencyValues);
   // mAdjustmentTable.SetValues(adjustmentValues);
   aMTI_AdjustmentTable.SetDopplerFrequency(usedDopplerFrequency);
   aMTI_AdjustmentTable.GetSpeedFrequencyTable().SetValues(speedFrequencyValues);
   aMTI_AdjustmentTable.GetAdjustmentTable().SetValues(adjustmentValues);
}
