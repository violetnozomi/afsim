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

// References:
//
// 1) "Direct-Detection LADAR Systems"; SPIE Press Tutorial Text in Optical Engineering,
//    Volume TT85; Richard D. Richmond and Stephen C. Cain.

#include "WsfLADAR_Sensor.hpp"

#include <algorithm>
#include <cmath>
#include <complex>
#include <iomanip>
#include <iostream>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtVec3.hpp"
#include "WsfDefaultSensorScheduler.hpp"
#include "WsfDefaultSensorTracker.hpp"
#include "WsfOpticalReflectivity.hpp"
#include "WsfOpticalSignature.hpp"
#include "WsfPlatform.hpp"
#include "WsfSensorModeList.hpp"
#include "WsfSensorObserver.hpp"
#include "WsfSimulation.hpp"

// =================================================================================================
WsfLADAR_Sensor::WsfLADAR_Sensor(WsfScenario& aScenario)
   : WsfSensor(aScenario)
   , mLADAR_ModeList()
{
   SetClass(cACTIVE | cINFRARED);

   // Create the mode list with the sensor-specific mode template.
   SetModeList(ut::make_unique<WsfSensorModeList>(new LADAR_Mode()));

   // Assign the default sensor scheduler and tracker
   SetScheduler(ut::make_unique<WsfDefaultSensorScheduler>());
   SetTracker(ut::make_unique<WsfDefaultSensorTracker>(aScenario));
}

// =================================================================================================
WsfLADAR_Sensor::WsfLADAR_Sensor(const WsfLADAR_Sensor& aSrc)
   : WsfSensor(aSrc)
   , mLADAR_ModeList()
{
}

// =================================================================================================
// virtual
WsfSensor* WsfLADAR_Sensor::Clone() const
{
   return new WsfLADAR_Sensor(*this);
}

// =================================================================================================
// virtual
bool WsfLADAR_Sensor::Initialize(double aSimTime)
{
   bool ok = WsfSensor::Initialize(aSimTime);

   // Reduce future dynamic casting by extracting derived class mode pointers.
   mModeListPtr->GetDerivedModeList(mLADAR_ModeList);
   return ok;
}

// =================================================================================================
// virtual
bool WsfLADAR_Sensor::ProcessInput(UtInput& aInput)
{
   return WsfSensor::ProcessInput(aInput);
}

// =================================================================================================
// virtual
void WsfLADAR_Sensor::Update(double aSimTime)
{
   // Bypass updates if not time for an update.  This avoids unnecessary device updates.
   // (A little slop is allowed to make sure event-driven chances occur as scheduled)
   if (mNextUpdateTime <= (aSimTime + 1.0E-5))
   {
      WsfSensor::Update(aSimTime);
      PerformScheduledDetections(aSimTime); // Perform any required detection attempts
   }
}

// =================================================================================================
// virtual
size_t WsfLADAR_Sensor::GetEM_RcvrCount() const
{
   // Returns 0 prior to initialization
   return mLADAR_ModeList.empty() ? 0U : 1U;
}

// =================================================================================================
// virtual
WsfEM_Rcvr& WsfLADAR_Sensor::GetEM_Rcvr(size_t aIndex) const
{
   return mLADAR_ModeList[mModeListPtr->GetCurrentMode()]->mRcvr;
}

// =================================================================================================
// virtual
size_t WsfLADAR_Sensor::GetEM_XmtrCount() const
{
   // Returns 0 prior to initialization
   return mLADAR_ModeList.empty() ? 0U : 1U;
}

// =================================================================================================
// virtual
WsfEM_Xmtr& WsfLADAR_Sensor::GetEM_Xmtr(size_t aIndex) const
{
   return mLADAR_ModeList[mModeListPtr->GetCurrentMode()]->mXmtr;
}


// =================================================================================================
// Definition for the nested class WsfLADAR_Sensor::LADAR_Result.
// =================================================================================================

// virtual
void WsfLADAR_Sensor::LADAR_Result::Print(ut::log::MessageStream& aMsgStream)
{
   WsfSensorResult::PrintGeometry(aMsgStream);


   // Everything enclosed is computed only if the signal-to-noise ratio is computed.
   if (mSignalToNoise > 0.0)
   {
      auto note = aMsgStream.AddNote() << "LADAR_Result:";
      note.AddNote() << "Optical_Sig: " << mOpticalSig << " m^2";
      note.AddNote() << "Az: " << PrintableAngle(mOpticalSigAz) << " deg";
      note.AddNote() << "El: " << PrintableAngle(mOpticalSigEl) << " deg";

      bool printMaskingFactor = ((mMaskingFactor >= 0.0) && (mMaskingFactor < 1.0));
      if ((mAbsorptionFactor > 0.0) || (mPropagationFactor > 0.0) || printMaskingFactor)
      {
         if (mAbsorptionFactor > 0.0)
         {
            note.AddNote() << "Absorption_factor: " << mAbsorptionFactor;
         }
         if (printMaskingFactor)
         {
            note.AddNote() << "Masking_Factor: " << mMaskingFactor;
         }
      }

      note.AddNote() << "Xmtd_Power: " << mXmtdPower << " W";
      note.AddNote() << "Rcvd_Power: " << mRcvdPower << " W";
      note.AddNote() << "Rcvr_Noise: " << mRcvrNoisePower << " W";
      note.AddNote() << "S/N: " << mSignalToNoise;
      if (mDetectionThreshold > 0.0)
      {
         note.AddNote() << "Threshold: " << mDetectionThreshold;
      }

      { // RAII block
         auto noiseRates = note.AddNote() << "Noise_Rates:";
         noiseRates.AddNote() << "Background: "
                              << (mDetectionData.mEnvironmentalNoiseRate + mDetectionData.mDarkNoiseRate) * 1.0E-6
                              << " MHz";
         noiseRates.AddNote() << "Solar: " << mDetectionData.mEnvironmentalNoiseRate * 1.0E-6 << " MHz";
         noiseRates.AddNote() << "Dark: " << mDetectionData.mDarkNoiseRate * 1.0E-6 << " MHz";
      }
      { // RAII block
         auto photon = note.AddNote() << "Photon_Counts:";
         photon.AddNote() << "Signal: " << mDetectionData.mSignalCount;
         photon.AddNote() << "Background: "
                          << (mDetectionData.mEnvironmentalNoiseCount + mDetectionData.mDarkNoiseCount +
                              mDetectionData.mSignalShotNoiseCount);
         photon.AddNote() << "Solar: " << mDetectionData.mEnvironmentalNoiseCount;
         photon.AddNote() << "Signal: " << mDetectionData.mSignalShotNoiseCount;
         photon.AddNote() << "Dark: " << mDetectionData.mDarkNoiseCount;
         photon.AddNote() << "Thermal: " << mDetectionData.mThermalNoiseCount;
      }
   }

   PrintStatus(aMsgStream);
}

// =================================================================================================
// private
//! A utility method to compute the spectral radiant exitance for a specified wavelength.
//!
//! @param aTemperature The body temperature (deg-K)
//! @param aWavelength  The wavelength of interest (um)
//! @returns The spectral radiant exitance (W / (cm^2 * um)).
// static
double WsfLADAR_Sensor::SpectralRadiantEmittance(double aTemperature, double aWavelength)
{
   // Reference: "Infrared System Engineering"; Richard D. Hudson, Jr.
   //            2006, John Wiley and Sons, Inc.

   //       (2*pi*h*c^2)            1
   // W_a = ------------ -------------------------   (Eq. 2.7)
   //          lam^5 *   exp((c*h)/(lam*k*T)) - 1)
   //
   //        c_1          1
   //     = ----- --------------------               (Eq. 2.8)
   //       lam*5 exp(c_2/(lam*T)) - 1

   // c1 - First radiation constant = 2 * pi * h * c^2
   static const double cC1 = 3.74177153E+4; // W / cm^2 (NIST-2010 CODATA)
   //                      = 3.74177153E-16;                      // W * m^2 (NIST-2010 CODATA)

   // c2 - Second radiation constant = c * h / k
   static const double cC2 = 1.4387770E+4; // um * deg-K (NIST-2010 CODATA)
   //                      = 1.4387770E-2;                        // m * deg-K (NIST-2010 CODATA)

   double lambda   = aWavelength;
   double lambda_2 = lambda * lambda;
   double t1       = cC1 / (lambda * lambda_2 * lambda_2);
   // double t2 = 1.0 / (exp(cC2 / (lambda * aTemperature)) - 1.0);
   double t2 = 1.0 / expm1(cC2 / (lambda * aTemperature));
   return t1 * t2;
}

void WsfLADAR_Sensor::LADAR_Mode::ComputeBackgroundRadiance()
{
   // If the background_temperature was specified use it to calculate the background_irradiance using
   // the blackbody equations.
   if (mBackgroundTemperature > 0.0)
   {
      double wavelength_um          = mRcvr.GetWavelength() * 1.0E6;
      double background_per_um      = SpectralRadiantEmittance(mBackgroundTemperature, wavelength_um);
      mBackgroundSpectralIrradiance = background_per_um * 1.0E6;
   }
}

// =================================================================================================
//! Compute the power per square meter (per meter; spectral irradiance) at the detector due to solar
//! background noise on the target.
//! The solar background photocount rate is computed using the first term of eq 1.27 from Ref 1.
//! @param aTargetArea         The area of the targt seen by the receiver. This may be less than the actual
//!                            area if the receivers IFOV is smaller.
//! @param aTargetReflectivity The reflectivity of the target.
//! @param aRange              The range from the target to the receiver.
//! @param aTransmittance      The atmospheric transmittance between the target and the receiver.
//! @returns The photocount rate (Hz or counts/sec) due to background solar radiation.
//! @note This count is NOT reduced by the structural masking factor. That is the responsibility of
//! the caller if it is desired.
// private
double WsfLADAR_Sensor::LADAR_Mode::ComputeTargetSolarIrradiance(double aTargetArea,
                                                                 double aTargetReflectivity,
                                                                 double aRange,
                                                                 double aTransmittance)
{
   // Solar irradiance
   double S_irr = mBackgroundSpectralIrradiance; // W/m2/m

   // Compute the Labertian reflected power off the target
   double P_sr = S_irr * aTargetArea * aTargetReflectivity; // W/m2/m * m2 -> W/m

   // Account for atmospheric losses from target to receiver.
   P_sr *= aTransmittance; // W/sr/m

   // Now compute the solid angle subtended by the receiver aperture.
   // double omega_r = UtMath::cPI_OVER_4 * (D_r * D_r) / (aRange * aRange);   // sr
   double omega_r_m2 = 1.0 / (aRange * aRange); // sr/m^2

   // Compute the energy at the front of the receiver optics
   double P_m2 = P_sr * omega_r_m2; // W/sr/m * sr/m^2 -> W/m^2/m

   return (P_m2);
}

// =================================================================================================
// Definition for the nested class WsfLADAR_Sensor::LADAR_Mode.
// =================================================================================================

// =================================================================================================
WsfLADAR_Sensor::LADAR_Mode::LADAR_Mode()
   : WsfSensorMode()
   , mAntenna()
   , mXmtr(WsfEM_Xmtr::cXF_SENSOR, &mAntenna)
   , mRcvr(WsfEM_Rcvr::cRF_SENSOR, &mAntenna)
   , mLASER_XmtrComponentPtr(nullptr)
   , mLASER_RcvrComponentPtr(nullptr)
   , mBackgroundTemperature(5778.0)
   , mBackgroundSpectralIrradiance(0.0)
   , mIntegrationGain(1.0)
   , mDetectionThreshold(0.0)
   , mDetectionProbabilityPtr(nullptr)
{
   mXmtr.GetComponents().AddComponent(new WsfLASER_XmtrComponent());
   mRcvr.GetComponents().AddComponent(new WsfLASER_RcvrComponent());

   // Indicate the receiver is 'linked' with the transmitter.
   mXmtr.SetLinkedReceiver(&mRcvr);

   SetCapabilities(cALL ^ cPULSEWIDTH ^ cFREQUENCY ^ cPULSE_REPITITION_INTERVAL);
}

// =================================================================================================
WsfLADAR_Sensor::LADAR_Mode::LADAR_Mode(const LADAR_Mode& aSrc)
   : WsfSensorMode(aSrc)
   , mAntenna(aSrc.mAntenna)
   , mXmtr(aSrc.mXmtr, &mAntenna)
   , mRcvr(aSrc.mRcvr, &mAntenna)
   , mLASER_XmtrComponentPtr(nullptr)
   , mLASER_RcvrComponentPtr(nullptr)
   , mBackgroundTemperature(aSrc.mBackgroundTemperature)
   , mBackgroundSpectralIrradiance(aSrc.mBackgroundSpectralIrradiance)
   , mIntegrationGain(aSrc.mIntegrationGain)
   , mDetectionThreshold(aSrc.mDetectionThreshold)
   , mDetectionProbabilityPtr(aSrc.mDetectionProbabilityPtr)
{
   // Indicate the receiver is 'linked' with the transmitter.
   mXmtr.SetLinkedReceiver(&mRcvr);
}

// =================================================================================================
// virtual
WsfMode* WsfLADAR_Sensor::LADAR_Mode::Clone() const
{
   return new LADAR_Mode(*this);
}

// =================================================================================================
// virtual
bool WsfLADAR_Sensor::LADAR_Mode::Initialize(double aSimTime)
{
   bool ok = WsfSensorMode::Initialize(aSimTime);

   if (mXmtr.GetPulseRepetitionInterval() <= 0.0)
   {
      ut::log::error()
         << "Either transmitter 'pulse_repetition_frequency' or 'pulse_repetition_interval' must be specified.";
      ok = false;
   }

   if ((mDetectionThreshold <= 0.0) && (mDetectionProbabilityPtr == nullptr))
   {
      ut::log::error() << "'detection_threshold' or 'detection_probability' must be specified.";
      ok = false;
   }

   mLASER_XmtrComponentPtr = mXmtr.GetComponents().GetComponent<WsfLASER_XmtrComponent>();
   mLASER_RcvrComponentPtr = mRcvr.GetComponents().GetComponent<WsfLASER_RcvrComponent>();

   ok &= mAntenna.Initialize(mSensorPtr);
   mXmtr.SetMode(this); // Identify the xmtr with the mode.
   mXmtr.SetIndex(0);
   ok &= mXmtr.Initialize(*GetSimulation());
   mXmtr.SetDebugEnabled(mSensorPtr->DebugEnabled());
   mRcvr.SetMode(this); // Identify the rcvr with the mode.

   // For automatic propagation to WsfEM_Interaction by BeginTwoWayInteraction
   mRcvr.SetDetectionThreshold(mDetectionThreshold);
   ok &= mRcvr.Initialize(*GetSimulation());
   mRcvr.SetDebugEnabled(mSensorPtr->DebugEnabled());

   ComputeBackgroundRadiance();

   return ok;
}

// =================================================================================================
// virtual
bool WsfLADAR_Sensor::LADAR_Mode::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (mAntenna.ProcessInput(aInput) || mXmtr.ProcessInputBlock(aInput) || mRcvr.ProcessInputBlock(aInput))
   {
   }
   else if (command == "background_temperature")
   {
      aInput.ReadValueOfType(mBackgroundTemperature, UtInput::cTEMPERATURE);
      aInput.ValueGreater(mBackgroundTemperature, 0.0);
      mBackgroundSpectralIrradiance = 0.0;
   }
   else if (command == "background_irradiance")
   {
      aInput.ReadValueOfType(mBackgroundSpectralIrradiance, UtInput::cSPECTRAL_IRRADIANCE);
      aInput.ValueGreaterOrEqual(mBackgroundSpectralIrradiance, 0.0);
      mBackgroundTemperature = 0.0;
   }
   else if (command == "integration_gain")
   {
      aInput.ReadValue(mIntegrationGain);
      aInput.ValueGreater(mIntegrationGain, 0.0);
   }
   else if (command == "detection_threshold")
   {
      aInput.ReadValue(mDetectionThreshold);
      aInput.ValueGreater(mDetectionThreshold, 0.0);
   }
   else if (command == "detection_probability")
   {
      // Load a Pd vs. S/N ratio
      mDetectionProbabilityPtr = UtTable::LoadCurve(aInput,
                                                    UtInput::cNON_DIMENSIONAL,
                                                    "",
                                                    UtTable::ValueGE(0.0), // SNR
                                                    UtInput::cNON_DIMENSIONAL,
                                                    "",
                                                    UtTable::ValueGE_LE(0.0, 1.0)); // Pd
      mDetectionThreshold      = 0.0;
   }
   else
   {
      myCommand = WsfSensorMode::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
// virtual
bool WsfLADAR_Sensor::LADAR_Mode::AttemptToDetect(double           aSimTime,
                                                  WsfPlatform*     aTargetPtr,
                                                  Settings&        aSettings,
                                                  WsfSensorResult& aResult)
{
   LADAR_Result result;
   bool         detected = AttemptToDetect(aSimTime, aTargetPtr, mXmtr, aSettings, result);
   aResult               = result;
   detected &= WsfSensorComponent::PostAttemptToDetect(*GetSensor(), aSimTime, aTargetPtr, aResult);
   return detected;
}

// =================================================================================================
bool WsfLADAR_Sensor::LADAR_Mode::AttemptToDetect(double        aSimTime,
                                                  WsfPlatform*  aTargetPtr,
                                                  WsfEM_Xmtr&   aXmtr,
                                                  Settings&     aSettings,
                                                  LADAR_Result& aResult)
{
   aResult.Reset(aSettings);
   aResult.SetCategory(GetSensor()->GetZoneAttenuationModifier());
   GetSensor()->UpdatePosition(aSimTime); // Ensure my position is current
   aTargetPtr->Update(aSimTime);          // Ensure the target position is current
   if (GetSensor()->DebugEnabled())
   {
      auto out = ut::log::debug() << "Attempting to detect target.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Sensor: " << GetSensor()->GetName();
      out.AddNote() << "Mode: " << GetName();
      out.AddNote() << "Target: " << aTargetPtr->GetName();
   }

   // Determine if concealed (like in a building).
   if (aResult.mFailedStatus == 0)
   {
      aResult.mCheckedStatus |= WsfSensorResult::cCONCEALMENT;
      if (aTargetPtr->GetConcealmentFactor() > 0.99F)
      {
         // We can't detect if it's in a building (or something like that)
         aResult.mFailedStatus |= WsfSensorResult::cCONCEALMENT;
         // Must have object pointers so event_output and debug output show locations.
         aResult.BeginGenericInteraction(&aXmtr, aTargetPtr, &mRcvr);
      }
   }

   if ((aResult.mFailedStatus == 0) && (aResult.BeginTwoWayInteraction(&aXmtr, aTargetPtr, &mRcvr) == 0))
   {
      // Set the position of the antenna beam(s).
      aResult.SetTransmitterBeamPosition();
      aResult.SetReceiverBeamPosition();

      // Determine the target optical cross section.
      aResult.ComputeOpticalSigAzEl();
      aResult.mOpticalSig =
         WsfOpticalSignature::GetValue(aSimTime, aTargetPtr, aResult.mOpticalSigAz, aResult.mOpticalSigEl);

      // Determine the target reflectivity.
      aResult.mOpticalReflectivity = WsfOpticalReflectivity::GetValue(aSimTime,
                                                                      aTargetPtr,
                                                                      aXmtr.GetWavelength(),
                                                                      aResult.mOpticalSigAz,
                                                                      aResult.mOpticalSigEl,
                                                                      aResult.mOpticalSigAz,
                                                                      aResult.mOpticalSigEl);

      // Compute the two-way atmospheric transmission factor.
      double T_a                = aResult.ComputeAttenuationFactor(WsfEM_Interaction::cXMTR_TO_TARGET);
      aResult.mAbsorptionFactor = T_a * T_a;

      // Use the LADAR range equation to compute the number of photons received

      // Compute the peak power of the pulse from the laser (W), using average power
      double P = aXmtr.GetAveragePower() * aXmtr.GetPulseRepetitionInterval() / aXmtr.GetPulseWidth(); // P_t, W

      // Propagate through the transmit optics
      P *= mLASER_XmtrComponentPtr->GetOpticsTransmissionFactor();

      // Convert back to Watts for interaction output (peak, not average)
      aResult.mXmtdPower = P; // W

      // Account for atmospheric losses from the transmitter to the target.
      P *= T_a;

      // Compute the power per unit area at the target plane (W/m^2)
      double theta  = mLASER_XmtrComponentPtr->GetBeamwidth();
      double R      = std::max(aResult.mXmtrToTgt.mRange, 1.0);
      double D_beam = theta * R;
      double A_beam = UtMath::cPI_OVER_4 * D_beam * D_beam;
      double P_m2   = P / A_beam; // W / m2 -> W/m2

      // The effective area from which the beam is reflected is the minimum of the beam area at the target
      // and the actual projected area of the target.
      double A_ref = std::min(aResult.mOpticalSig, A_beam); // m2

      // If the focal length and detector size of the receiver are specified, compute the instantaneous field-
      // of-view (IFOV) of the receiver and the area of the IFOV at the target plane. If this is computed then
      // limit reflected area to this value if the currently computed value from above exceeds this value.
      double A_ifov = mLASER_RcvrComponentPtr->GetInstantaneousFieldOfView() * R * R;
      A_ref         = std::min(A_ref, A_ifov);

      // Compute the intensity of reflected power
      double P_sr = P_m2 * A_ref * aResult.mOpticalReflectivity; // W/m2 * m2 * 1/sr -> W/sr

      double solarNoiseIrradiance = ComputeTargetSolarIrradiance(A_ref, aResult.mOpticalReflectivity, R, T_a);

      // Account for atmospheric losses from target to receiver.
      P_sr *= T_a; // W/sr

      // double omega_r = UtMath::cPI_OVPR_4 * (D_r * D_r) / (R * R);   // sr
      double omega_r_m2 = 1.0 / (R * R); // sr/m^2

      // Compute the spectral irradiance at the front of the receiver
      P_m2 = P_sr * omega_r_m2; // W/sr * sr/m^2 -> W/m^2

      aResult.mDetectionData =
         mLASER_RcvrComponentPtr->ComputeDetectionData(0.0, P_m2, 0.0, solarNoiseIrradiance, aXmtr.GetPulseWidth());

      // Convert back to Watts for interaction output (peak, not average)
      aResult.mRcvdPower = aResult.mDetectionData.mPower;

      // Compute the probability of detection.
      ComputeProbabilityOfDetection(aXmtr, aResult);

      // Determine if the Pd exceeds the detection threshold.
      aResult.mCheckedStatus |= WsfSensorResult::cSIGNAL_LEVEL;
      if (aResult.mPd >= aSettings.mRequiredPd)
      {
         // Check terrain masking only if all other checks succeed
         aResult.MaskedByTerrain();
      }
      else
      {
         aResult.mFailedStatus |= WsfSensorResult::cSIGNAL_LEVEL;
      }
   }

   // Determine the impact (if any) that an external script should have on detection status
   bool detected = GetSensor()->ScriptAllowDetection(aSimTime, aTargetPtr, aResult);

   if (GetSensor()->DebugEnabled())
   {
      auto logDebug = ut::log::debug() << "Sensor Data:";
      aResult.Print(logDebug);
   }
   GetSensor()->NotifySensorDetectionAttempted(aSimTime, aTargetPtr, aResult);
   return detected;
}

// =================================================================================================
// virtual
void WsfLADAR_Sensor::LADAR_Mode::Deselect(double aSimTime)
{
   mRcvr.Deactivate();
   mXmtr.Deactivate();
}

// =================================================================================================
// virtual
void WsfLADAR_Sensor::LADAR_Mode::Select(double aSimTime)
{
   mRcvr.Activate();
   mXmtr.Activate();
}

// =================================================================================================
//! Computes the probability of detection using the detector model that has been configured.
//!
//! @param aXmtr   The transmitter that is involved in the interaction.
//! @param aResult The sensor interaction data.
//!
//! @note This routine does NOT expect ANY of the 'aResult' data to be valid except a few variables,
//! and the transmitter is only used to get wavelength, pulse width and repetition interval.
// private
void WsfLADAR_Sensor::LADAR_Mode::ComputeProbabilityOfDetection(const WsfEM_Xmtr& aXmtr, LADAR_Result& aResult)
{
   aResult.mPd = 0.0;

   // Compute the signal to noise ratio (Ref. 1, eq. 1.30)
   //
   // NOTE: The signal to noise ratio is NOT used for the advanced detector model.
   //       This is ONLY for the simple detectors.

   WsfLASER_RcvrComponent::DetectionData& dd = aResult.mDetectionData;
   aResult.mSignalToNoise                    = dd.mSignalCount / dd.mNoiseCount;

   // Convert the noise back to watts for interaction output.
   double noiseEnergy = dd.mNoiseCount * ((UtMath::cPLANCK_CONSTANT * UtMath::cLIGHT_SPEED) / aXmtr.GetWavelength());
   aResult.mRcvrNoisePower = noiseEnergy / aXmtr.GetPulseWidth();

   if (mDetectionProbabilityPtr != nullptr)
   {
      // Computed with the Pd vs. S/N table
      aResult.mPd = mDetectionProbabilityPtr->Lookup(aResult.mSignalToNoise);
   }
   else
   {
      // Compute using the Gaussian curve.
      aResult.mPd = ComputeGaussianDetectionProbability(dd.mSignalCount, dd.mNoiseCount, mDetectionThreshold);
   }
}

// =================================================================================================
//! Compute the probability of detection using the simple Gaussian model.
//!
//! @param aSignal [input] The returned signal
//! @param aNoise  [input] The noise
//! @param aThreshold [input] The signal-to-noise ratio required to generate a Pd = 0.5.
//!
//! @returns The probability of detection in the range [0, 1]
// private
double WsfLADAR_Sensor::LADAR_Mode::ComputeGaussianDetectionProbability(double aSignal, double aNoise, double aThreshold) const
{
   if (aSignal <= 0.0)
   {
      return 0.0;
   }

   double signalToNoise = aSignal / aNoise;
   double beta          = signalToNoise - aThreshold;

   // The following refers to MDC report B1368 (15 Jan 1989).
   //
   // Equation 5.3-12 defines Pd = Q(-beta) + Q(beta + 2*alpha)
   // where Q is the Gaussian Q function.
   //
   // The second term is very small compared to the first so:
   //
   // Pd = Q(-beta) = 1 - Q(beta)
   //
   // THe 'Handbook of Mathematical Functions' (Abramowitz and Stegun) say:
   //
   // P(x) + Q(x) = 1         (equation 26.2.5)
   // P(x) = 1 - Q(x)
   //
   // Pd = P(x)
   //
   // The following is the approximation for P(x) as defined in 26.2.16.

   static const double cCONST = 0.39894228; // 1.0 / sqrt(2*pi)
   double              pd     = 0.5;
   double              x      = beta;
   double              z      = cCONST * exp(-0.5 * x * x);
   if (x > 1.0E-5)
   {
      double t = 1.0 / (1.0 + 0.33267 * x);
      pd       = 1.0 - z * (0.4361836 * t - 0.1201676 * t * t + .9372980 * t * t * t);
   }
   else if (x < -1.0E-5)
   {
      double t = 1.0 / (1.0 - 0.33267 * x);
      pd       = z * (0.4361836 * t - 0.1201676 * t * t + .9372980 * t * t * t);
   }
   return pd;
}
