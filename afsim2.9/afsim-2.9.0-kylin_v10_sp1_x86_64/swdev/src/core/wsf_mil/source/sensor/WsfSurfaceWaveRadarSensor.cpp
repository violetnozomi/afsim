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

// WsfSurfaceWaveRadarSensor is a simple surface-wave radar sensor implementation.
// Documents referenced in this file:
//
// Ref 1. Milsom, J.D., "HF Groundwave Radar Equations", 'HF Radio Systems
//        and Techniques', 7-10 Jul 1997, Conference Publication No. 411

#include "WsfSurfaceWaveRadarSensor.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>

#include "UtEarth.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtRandom.hpp"
#include "UtStringUtil.hpp"
#include "UtVec3.hpp"
#include "WsfDefaultSensorScheduler.hpp"
#include "WsfDefaultSensorTracker.hpp"
#include "WsfPlatform.hpp"
#include "WsfRadarSignature.hpp"
#include "WsfSensorModeList.hpp"
#include "WsfSensorObserver.hpp"
#include "WsfSensorResult.hpp"
#include "WsfStringId.hpp"
#include "WsfSurfaceWaveRadarSensorErrorModel.hpp"

WsfSurfaceWaveRadarSensor::WsfSurfaceWaveRadarSensor(WsfScenario& aScenario)
   : WsfSensor(aScenario)
   , mRadarModeList()
{
   SetClass(cACTIVE | cRADIO); // This is an active RF sensor.
   // Create the mode list with the sensor-specific mode template.
   SetModeList(ut::make_unique<WsfSensorModeList>(new WsfSurfaceWaveRadarSensor::RadarMode()));

   // Assign the default sensor scheduler and tracker
   SetScheduler(ut::make_unique<WsfDefaultSensorScheduler>());
   SetTracker(ut::make_unique<WsfDefaultSensorTracker>(aScenario));
}

WsfSurfaceWaveRadarSensor::WsfSurfaceWaveRadarSensor(const WsfSurfaceWaveRadarSensor& aSrc)
   : WsfSensor(aSrc)
   , mRadarModeList()
{
}

// virtual
WsfSensor* WsfSurfaceWaveRadarSensor::Clone() const
{
   return new WsfSurfaceWaveRadarSensor(*this);
}

// virtual
size_t WsfSurfaceWaveRadarSensor::GetEM_RcvrCount() const
{
   // Returns 0 prior to initialization
   return mRadarModeList.empty() ? 0U : 1U;
}

// virtual
WsfEM_Rcvr& WsfSurfaceWaveRadarSensor::GetEM_Rcvr(size_t aIndex) const
{
   RadarMode* modePtr = mRadarModeList[mModeListPtr->GetCurrentMode()];
   return *modePtr->GetEM_Rcvr();
}

// virtual
size_t WsfSurfaceWaveRadarSensor::GetEM_XmtrCount() const
{
   // Returns 0 prior to initialization
   return mRadarModeList.empty() ? 0U : 1U;
}

// virtual
WsfEM_Xmtr& WsfSurfaceWaveRadarSensor::GetEM_Xmtr(size_t aIndex) const
{
   RadarMode* modePtr = mRadarModeList[mModeListPtr->GetCurrentMode()];
   return *modePtr->GetEM_Xmtr();
}

// virtual
bool WsfSurfaceWaveRadarSensor::Initialize(double aSimTime)
{
   bool ok = WsfSensor::Initialize(aSimTime);

   // Reduce future dynamic casting by extracting derived class mode pointers.
   mModeListPtr->GetDerivedModeList(mRadarModeList);

   return ok;
}

// virtual
void WsfSurfaceWaveRadarSensor::PlatformAdded(double aSimTime, WsfPlatform* aPlatformPtr)
{
   WsfSensor::PlatformAdded(aSimTime, aPlatformPtr);
}

// virtual
bool WsfSurfaceWaveRadarSensor::ProcessInput(UtInput& aInput)
{
   return WsfSensor::ProcessInput(aInput);
}

// virtual
void WsfSurfaceWaveRadarSensor::Update(double aSimTime)
{
   // Bypass updates if not time for an update.  This avoids unnecessary device updates.
   // (A little slop is allowed to make sure event-driven chances occur as scheduled)
   if (mNextUpdateTime <= (aSimTime + 1.0E-5))
   {
      WsfSensor::Update(aSimTime);
      PerformScheduledDetections(aSimTime); // Perform any required detection attempts
   }
}

// ***************************************************************************
// Nested class WsfSurfaceWaveRadarSensor::RadarMode.
WsfSurfaceWaveRadarSensor::RadarMode::RadarMode()
   : WsfSensorMode()
   , WsfSensorBeam()
   , mDopplerResolution(0.0)
   , mAdjustmentFactor(1.0)
   , mIntegrationGain(1.0)
   , mDetector()
   , mUseDetector(false)
   , mAntenna()
   , mXmtr(WsfEM_Xmtr::cXF_SENSOR, &mAntenna)
   , mRcvr(WsfEM_Rcvr::cRF_SENSOR, &mAntenna)
   , mNoise()
{
   // indicate the receives is 'linked' with the transmitter
   mXmtr.SetLinkedReceiver(&mRcvr);

   // disable masking checks for OTH transmitters and receivers
   mXmtr.DisableMaskingCheck();
   mRcvr.DisableMaskingCheck();
}

WsfSurfaceWaveRadarSensor::RadarMode::RadarMode(const RadarMode& aSrc)
   : WsfSensorMode(aSrc)
   , WsfSensorBeam(aSrc)
   , mDopplerResolution(aSrc.mDopplerResolution)
   , mAdjustmentFactor(aSrc.mAdjustmentFactor)
   , mIntegrationGain(aSrc.mIntegrationGain)
   , mDetector(aSrc.mDetector)
   , mUseDetector(aSrc.mUseDetector)
   , mAntenna(aSrc.mAntenna)
   , mXmtr(aSrc.mXmtr, &mAntenna)
   , mRcvr(aSrc.mRcvr, &mAntenna)
   , mNoise(aSrc.mNoise)
{
   // indicate the receives is 'linked' with the transmitter
   mXmtr.SetLinkedReceiver(&mRcvr);

   // disable masking checks for OTH transmitters and receivers
   mXmtr.DisableMaskingCheck();
   mRcvr.DisableMaskingCheck();
}

// virtual
WsfMode* WsfSurfaceWaveRadarSensor::RadarMode::Clone() const
{
   return new RadarMode(*this);
}

// virtual
bool WsfSurfaceWaveRadarSensor::RadarMode::Initialize(double aSimTime)
{
   bool ok = WsfSensorMode::Initialize(aSimTime);

   mMaximumRange = 0.0;
   ok &= mAntenna.Initialize(mSensorPtr);
   mMaximumRange = std::max(mMaximumRange, mAntenna.GetMaximumRange());
   mXmtr.SetMode(this);
   ok &= mXmtr.Initialize(*GetSimulation());
   mRcvr.SetMode(this);
   ok &= mRcvr.Initialize(*GetSimulation());
   ok &= mNoise.Initialize();
   if (ok)
   {
      if (mSensorPtr->ShowCalibrationData())
      {
         { // RAII block
            auto out = ut::log::info() << "Sensor calibration data:";
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
            out.AddNote() << "Sensor: " << mSensorPtr->GetName();
            out.AddNote() << "Mode: " << GetName();
         }
         Calibrate();
      }
   }

   // Set the debug flag
   mRcvr.SetDebugEnabled(mSensorPtr->DebugEnabled());
   mXmtr.SetDebugEnabled(mSensorPtr->DebugEnabled());

   return ok;
}

// virtual
bool WsfSurfaceWaveRadarSensor::RadarMode::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (mAntenna.ProcessInput(aInput) || mXmtr.ProcessInputBlock(aInput) || mRcvr.ProcessInputBlock(aInput) ||
       mNoise.ProcessInput(aInput))
   {
   }
   else if (command == "doppler_resolution")
   {
      aInput.ReadValueOfType(mDopplerResolution, UtInput::cSPEED);
      aInput.ValueGreater(mDopplerResolution, 0.0);
   }
   else if (command == "integration_gain")
   {
      aInput.ReadValueOfType(mIntegrationGain, UtInput::cRATIO);
      aInput.ValueGreaterOrEqual(mIntegrationGain, 1.0);
      mUseDetector = false;
   }
   else if (command == "adjustment_factor")
   {
      aInput.ReadValueOfType(mAdjustmentFactor, UtInput::cRATIO);
      aInput.ValueGreater(mAdjustmentFactor, 0.0);
   }
   else if (command == "operating_loss")
   {
      // For compatibility with SUPPRESSOR. Just another name for the reciprocal of the 'adjustment_factor'
      double operatingLoss;
      aInput.ReadValueOfType(operatingLoss, UtInput::cRATIO);
      aInput.ValueGreaterOrEqual(operatingLoss, 1.0);
      mAdjustmentFactor = 1.0 / operatingLoss;
   }
   else if (command == "detection_threshold")
   {
      // The receiver also has a detection threshold. It is also put here so the
      // 'integration_gain' and 'detection_threshold' can be grouped together by the user
      // in a logical fashion.
      double detectionThreshold;
      aInput.ReadValueOfType(detectionThreshold, UtInput::cRATIO);
      aInput.ValueGreater(detectionThreshold, 0.0);
      mRcvr.SetDetectionThreshold(detectionThreshold);
      mUseDetector = false;
   }
   else if (command == "number_of_pulses_integrated")
   {
      int numberOfPulsesIntegrated;
      aInput.ReadValue(numberOfPulsesIntegrated);
      aInput.ValueGreater(numberOfPulsesIntegrated, 0);
      mDetector.SetNumberOfPulsesIntegrated(numberOfPulsesIntegrated);
   }
   else if (command == "detector_law")
   {
      std::string word;
      aInput.ReadValue(word);
      if (word == "linear")
      {
         mDetector.SetDetectorLaw(wsf::MarcumSwerling::cDL_LINEAR);
      }
      else if (word == "square")
      {
         mDetector.SetDetectorLaw(wsf::MarcumSwerling::cDL_SQUARE);
      }
      else if (word == "log")
      {
         mDetector.SetDetectorLaw(wsf::MarcumSwerling::cDL_LOG);
      }
      else
      {
         throw UtInput::BadValue(aInput);
      }
   }
   else if (command == "probability_of_false_alarm")
   {
      double probabilityOfFalseAlarm;
      aInput.ReadValue(probabilityOfFalseAlarm);
      aInput.ValueInClosedRange(probabilityOfFalseAlarm, 0.0, 1.0);
      mDetector.SetProbabilityOfFalseAlarm(probabilityOfFalseAlarm);
   }
   else if (command == "swerling_case")
   {
      int swerlingCase;
      aInput.ReadValue(swerlingCase);
      aInput.ValueInClosedRange(swerlingCase, 0, 4);
      mDetector.SetCase(swerlingCase);
      mUseDetector = true;
   }
   else if (command == "no_swerling_case")
   {
      mUseDetector = false;
   }
   else if (command == "compute_measurement_errors")
   {
      bool computeMeasurementErrors;
      aInput.ReadValue(computeMeasurementErrors);
      if (computeMeasurementErrors)
      {
         mErrorModelPtr = ut::make_unique<wsf::SurfaceWaveRadarSensorErrorModel>();
      }
      else
      {
         mErrorModelPtr = ut::make_unique<wsf::StandardSensorErrorModel>();
      }
   }
   else
   {
      myCommand = (WsfSensorMode::ProcessInput(aInput) || WsfSensorBeam::ProcessInput(aInput));
   }

   return myCommand;
}

bool WsfSurfaceWaveRadarSensor::RadarMode::AttemptToDetect(double           aSimTime,
                                                           WsfPlatform*     aTargetPtr,
                                                           Settings&        aSettings,
                                                           WsfSensorResult& aResult)
{
   bool detected = false;
   aResult.Reset(aSettings);

   GetSensor()->UpdatePosition(aSimTime); // Ensure my position is current
   aTargetPtr->Update(aSimTime);          // Ensure the target position is current

   if (GetSensor()->DebugEnabled())
   {
      auto out = ut::log::debug() << "Attempting to detect.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Sensor: " << GetSensor()->GetName();
      out.AddNote() << "Mode: " << GetName();
      out.AddNote() << "Target: " << aTargetPtr->GetName();
   }

   // Determine if concealed (like in a building).
   if (aResult.mFailedStatus == 0)
   {
      aResult.mCheckedStatus |= Result::cCONCEALMENT;
      if (aTargetPtr->GetConcealmentFactor() > 0.99F)
      {
         // We can't detect if it's in a building (or something like that)
         aResult.mFailedStatus |= WsfSensorResult::cCONCEALMENT;
         // Must have object pointers so event_output and debug output show locations.
         aResult.BeginGenericInteraction(nullptr, aTargetPtr, &mRcvr);
      }
   }

   if (aResult.mFailedStatus == 0)
   {
      AttemptToDetect(aSimTime, aTargetPtr, &mXmtr, aResult);

      // See if the resulting Pd results in detection.
      if (aResult.mFailedStatus == 0)
      {
         aResult.mCheckedStatus |= WsfSensorResult::cSIGNAL_LEVEL;
         if (aResult.mPd >= aSettings.mRequiredPd)
         {
            detected = true;
         }
         else
         {
            aResult.mFailedStatus |= WsfSensorResult::cSIGNAL_LEVEL;
         }
      }
   }

   detected &= WsfSensorComponent::PostAttemptToDetect(*GetSensor(), aSimTime, aTargetPtr, aResult);
   detected &= GetSensor()->ScriptAllowDetection(aSimTime, aTargetPtr, aResult);
   if (GetSensor()->DebugEnabled())
   {
      auto logDebug = ut::log::debug() << "Sensor Data.";
      aResult.Print(logDebug);
   }
   GetSensor()->NotifySensorDetectionAttempted(aSimTime, aTargetPtr, aResult);
   return detected;
}

// virtual
void WsfSurfaceWaveRadarSensor::RadarMode::AttemptToDetect(double           aSimTime,
                                                           WsfPlatform*     aTargetPtr,
                                                           WsfEM_Xmtr*      aXmtrPtr,
                                                           WsfSensorResult& aResult)
{
   if (aResult.BeginTwoWayInteraction(aXmtrPtr, aTargetPtr, &mRcvr) == 0)
   {
      // Set the position of the antenna beam(s).
      aResult.SetTransmitterBeamPosition();
      aResult.SetReceiverBeamPosition();

      // Determine the radar cross section of the target.
      aResult.ComputeRadarSigAzEl();
      aResult.mRadarSig = WsfRadarSignature::GetValue(aTargetPtr,
                                                      aXmtrPtr,
                                                      &mRcvr,
                                                      aResult.mRadarSigAz,
                                                      aResult.mRadarSigEl,
                                                      aResult.mRadarSigAz,
                                                      aResult.mRadarSigEl);

      // the sea surface acts as a reflector of the transmitted energy.
      // the fully developed sea cross section is 0.005 m2 / m2
      // we will treat the sea surface as a decrement of the true targets
      // RCS to ensure the sea surface return passes through the integration gain
      // However, these systems fundamentally detect targets based upon
      // Doppler-gate processing of long-duration integration
      // therefore, the sea surface return will only be used for targets
      // whose radial velocity is less than the equivalent Bragg line frequency
      //
      // compute the maximum radial velocity that corresponds to the Bragg Doppler frequency
      double braggRadialVelocity =
         sqrt(UtEarth::cACCEL_OF_GRAVITY * UtMath::cLIGHT_SPEED / (4.0 * UtMath::cPI * aXmtrPtr->GetFrequency()));

      // compute sea-clutter if target Doppler is less than the Bragg Doppler
      double effectiveRCS = aResult.mRadarSig;
      if (fabs(aResult.ComputeTargetDopplerSpeed(true)) < braggRadialVelocity)
      {
         double seaSurfaceArea = UtMath::cLIGHT_SPEED * aXmtrPtr->GetPulseWidth() * aResult.mXmtrToTgt.mRange *
                                 aResult.GetReceiver()->GetAzimuthBeamwidth(0.0, 0.0);
         effectiveRCS -= 0.005 * seaSurfaceArea;
      }

      // compute received power
      aResult.ComputeRF_TwoWayPower(effectiveRCS);

      // because the surface-wave propagation routine takes into account
      // free-space propagation, we take those terms out of the computation
      // that aResult just performed
      double xmtrRange = std::max(aResult.mXmtrToTgt.mRange, 1.0);
      double rcvrRange = xmtrRange;
      if (aResult.mBistatic)
      {
         rcvrRange = std::max(aResult.mRcvrToTgt.mRange, 1.0);
      }

      aResult.mRcvdPower *= UtMath::cFOUR_PI * xmtrRange * xmtrRange * UtMath::cFOUR_PI * rcvrRange * rcvrRange;

      // apply aperture function
      aResult.mRcvdPower /= pow(UtMath::cLIGHT_SPEED / aXmtrPtr->GetFrequency(), 2.0) / UtMath::cFOUR_PI;

      // Account for the gain due to pulse compression.
      aResult.mRcvdPower *= aXmtrPtr->GetPulseCompressionRatio();

      // Allow for direct input of the integration gain (ala SUPPRESSOR)
      if (!mUseDetector)
      {
         aResult.mRcvdPower *= mIntegrationGain;
      }

      // Allow for other general post-reception adjustments (for compatibility with SUPPRESSOR OPERATING-LOSSES).
      aResult.mRcvdPower *= mAdjustmentFactor;

      // Compute effects from components.
      WsfSensorComponent::AttemptToDetect(*GetSensor(), aSimTime, aResult);

      // sum galactic, man-made and atmospheric noise power
      // and put into result's clutter power
      aResult.mClutterPower = mNoise.ComputeNoise(aResult);

      // Compute the total effective signal-to-noise ratio at the output of the receiver.
      aResult.mSignalToNoise =
         mRcvr.ComputeSignalToNoise(aResult.mRcvdPower, aResult.mClutterPower, aResult.mInterferencePower);

      // Compute the probability of detection.
      if (mUseDetector)
      {
         // Marcum-Swerling detector selected
         aResult.mPd = mDetector.ComputeProbabilityOfDetection(aResult.mSignalToNoise);
      }
      else
      {
         // Swerling case not specified... use simple binary test.
         aResult.mPd = 1.0;
         if (aResult.mSignalToNoise < mRcvr.GetDetectionThreshold())
         {
            aResult.mPd = 0.0;
         }
      }

      // Apply component effects
      aResult.mPd *= (1.0 - aResult.mInterferenceFactor);
   }
}

// virtual
void WsfSurfaceWaveRadarSensor::RadarMode::Deselect(double aSimTime)
{
   mRcvr.Deactivate();
   mXmtr.Deactivate();
}

// virtual
void WsfSurfaceWaveRadarSensor::RadarMode::Select(double aSimTime)
{
   mRcvr.Activate();
   mXmtr.Activate();
}

void WsfSurfaceWaveRadarSensor::RadarMode::Calibrate()
{
   WsfEM_Xmtr& xmtr = mXmtr;
   WsfEM_Rcvr& rcvr = mRcvr;

   double p_avg      = xmtr.GetAveragePower();
   double dutyCycle  = xmtr.GetPulseWidth() * xmtr.GetPulseRepetitionFrequency();
   double wavelength = UtMath::cLIGHT_SPEED / xmtr.GetFrequency();

   bool print = mSensorPtr->ShowCalibrationData();
   auto out   = ut::log::info();
   if (print)
   {
      out << "WsfSurfaceWaveRadarSensor::Calibrate():";
      out.AddNote() << "Peak power output: " << UtMath::LinearToDB(xmtr.GetPeakPower()) << " dbW ("
                    << xmtr.GetPeakPower() << " W)";
      out.AddNote() << "Pulse repetition frequency: " << xmtr.GetPulseRepetitionFrequency() << " Hz";
      out.AddNote() << "Pulse width: " << xmtr.GetPulseWidth() << " seconds";
      out.AddNote() << "Duty cycle: " << dutyCycle;
      double pcr = xmtr.GetPulseCompressionRatio();
      out.AddNote() << "Pulse compression ratio: " << UtMath::LinearToDB(pcr) << " dB (" << pcr << ")";
      out.AddNote() << "Average power output: " << UtMath::LinearToDB(p_avg) << " dBW (" << p_avg << " W)";

      out.AddNote() << "Frequency: " << xmtr.GetFrequency() << " Hertz";
      out.AddNote() << "Wavelength: " << wavelength << " meters";

      double peakGain;
      peakGain = xmtr.GetPeakAntennaGain();
      out.AddNote() << "Transmitter Antenna Gain: " << UtMath::LinearToDB(peakGain) << " dB (" << peakGain << ")";
      peakGain = rcvr.GetPeakAntennaGain();
      out.AddNote() << "Receiver Antenna Gain: " << UtMath::LinearToDB(peakGain) << " dB (" << peakGain << ")";

      double internalLoss;
      internalLoss = xmtr.GetInternalLoss();
      out.AddNote() << "Transmitter Internal Loss: " << UtMath::LinearToDB(internalLoss) << " dB (" << internalLoss << ")";
      internalLoss = rcvr.GetInternalLoss();
      out.AddNote() << "Receiver Internal Loss: " << UtMath::LinearToDB(internalLoss) << " dB (" << internalLoss << ")";
      if (mIntegrationGain != 1.0)
      {
         out.AddNote() << "Integration Gain: " << UtMath::LinearToDB(mIntegrationGain) << " dB (" << mIntegrationGain
                       << ")";
      }
      if (mAdjustmentFactor != 1.0)
      {
         out.AddNote() << "Adjustment Factor: " << UtMath::LinearToDB(mAdjustmentFactor) << " dB (" << mAdjustmentFactor
                       << ")";
      }

      out.AddNote() << "Receiver Noise Power: " << UtMath::LinearToDB(rcvr.GetNoisePower()) << " dBW ("
                    << rcvr.GetNoisePower() << " W)";
   }

   double detectionThreshold = rcvr.GetDetectionThreshold();
   if (mUseDetector)
   {
      // Find the signal to noise the generates a Pd = 0.5.
      double loSignalToNoise = 0.0;
      double hiSignalToNoise = 1000.0;
      while (fabs(hiSignalToNoise - loSignalToNoise) > 0.001)
      {
         detectionThreshold = 0.5 * (loSignalToNoise + hiSignalToNoise);
         double pd          = mDetector.ComputeProbabilityOfDetection(detectionThreshold);
         if (fabs(pd - 0.5) < 0.001)
         {
            break;
         }
         else if (pd < 0.5)
         {
            loSignalToNoise = detectionThreshold;
         }
         else
         {
            hiSignalToNoise = detectionThreshold;
         }
      }
   }
   if (print)
   {
      double thresholdWatts = detectionThreshold * rcvr.GetNoisePower();
      out.AddNote() << "Minimum Detectable Signal: " << UtMath::LinearToDB(thresholdWatts) << " dBW (" << thresholdWatts
                    << " W)";
      out.AddNote() << "Minimum Detectable S/N: " << UtMath::LinearToDB(detectionThreshold) << " dB ("
                    << detectionThreshold << ")";
   }

   double temp1 = (wavelength * wavelength) / (UtMath::cFOUR_PI * UtMath::cFOUR_PI * UtMath::cFOUR_PI);
   temp1 *= xmtr.GetPower() * xmtr.GetPeakAntennaGain() * rcvr.GetPeakAntennaGain();
   temp1 /= (xmtr.GetInternalLoss() * rcvr.GetInternalLoss());

   // Account for the gain due to pulse compression.
   temp1 *= xmtr.GetPulseCompressionRatio();

   // Allow for direct input of the integration gain (ala SUPPRESSOR)
   if (!mUseDetector)
   {
      temp1 *= mIntegrationGain;
   }

   // Allow for other general post-reception adjustments (for compatibility with SUPPRESSOR OPERATING-LOSSES).
   temp1 *= mAdjustmentFactor;
   double r        = pow(temp1 / (rcvr.GetNoisePower() * detectionThreshold), 0.25);
   double loopGain = temp1 / rcvr.GetNoisePower();
   if (print)
   {
      out.AddNote() << "1 m^2 Detection Range: " << r << " meters (calibrated - free space)";
      out.AddNote() << "Loop Gain: " << UtMath::LinearToDB(loopGain) << " dB (calibrated)";
   }
}
