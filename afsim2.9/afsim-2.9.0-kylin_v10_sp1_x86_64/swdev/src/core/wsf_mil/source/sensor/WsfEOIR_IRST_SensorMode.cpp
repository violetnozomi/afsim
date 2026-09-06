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

#include "WsfEOIR_IRST_SensorMode.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "WsfEM_Attenuation.hpp"
#include "WsfEM_AttenuationTypes.hpp"
#include "WsfEM_Util.hpp"
#include "WsfEnvironment.hpp"
#include "WsfInfraredSignature.hpp"
#include "WsfInherentContrast.hpp"
#include "WsfOpticalAttenuation.hpp"
#include "WsfOpticalSignature.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSensorObserver.hpp"
#include "WsfSimulation.hpp"

// =================================================================================================
WsfEOIR_IRST_SensorMode::WsfEOIR_IRST_SensorMode(WsfScenario& aScenario)
   : WsfSensorMode()
   , mAntenna()
   , mRcvr(WsfEM_Rcvr::cRF_SENSOR, &mAntenna)
   , mEnvironment()
   , mOpticalBands()
   , mOpticalBand(WsfEM_Types::cOPT_IR_COUNT)
   , mInfraredBand(WsfEM_Types::cIRBAND_COUNT)
   , mDetectNegativeContrast(true)
   , mUseSimpleVisualDetector(false)
   , mDetectorGain(1.0)
   , mIntegrationGain(1.0)
   , mNoiseEquivalentIrradiance(0.0)
   , mDetectionThreshold(0.0)
   , mAtmosphericAttenuation(0.0)
   , mAtmosphere(aScenario.GetAtmosphere())
   , mComputeOldInstallationEffects(false)
{
   // Enable attenuation_model input
   mRcvr.SetAttenuationInputEnabled(true);

   SetCapabilities(cALL ^ cPULSEWIDTH ^ cFREQUENCY ^ cPULSE_REPITITION_INTERVAL);
}

// =================================================================================================
WsfEOIR_IRST_SensorMode::WsfEOIR_IRST_SensorMode(const WsfEOIR_IRST_SensorMode& aSrc)
   : WsfSensorMode(aSrc)
   , mAntenna(aSrc.mAntenna)
   , mRcvr(aSrc.mRcvr, &mAntenna)
   , mEnvironment(aSrc.mEnvironment)
   , mOpticalBands(aSrc.mOpticalBands)
   , mOpticalBand(aSrc.mOpticalBand)
   , mInfraredBand(aSrc.mInfraredBand)
   , mDetectNegativeContrast(aSrc.mDetectNegativeContrast)
   , mUseSimpleVisualDetector(aSrc.mUseSimpleVisualDetector)
   , mDetectorGain(aSrc.mDetectorGain)
   , mIntegrationGain(aSrc.mIntegrationGain)
   , mNoiseEquivalentIrradiance(aSrc.mNoiseEquivalentIrradiance)
   , mDetectionThreshold(aSrc.mDetectionThreshold)
   , mAtmosphericAttenuation(aSrc.mAtmosphericAttenuation)
   , mAtmosphere(aSrc.mAtmosphere)
   , mComputeOldInstallationEffects(aSrc.mComputeOldInstallationEffects)
{
}

// ================================================================================================
// virtual
bool WsfEOIR_IRST_SensorMode::Initialize(double aSimTime)
{
   bool ok = WsfSensorMode::Initialize(aSimTime);
   ok &= mEnvironment.Initialize(aSimTime, mPlatformPtr);

   // Set the receiver frequency and bandwidth so it will initialize.
   double shorterWavelength = 0.0;
   double longerWavelength  = 0.0;
   if (mOpticalBand == WsfEM_Types::cOPT_VISUAL)
   {
      shorterWavelength = 380.0E-9; // 380 nm
      longerWavelength  = 760.0E-9; // 760 nm
   }
   else if (mOpticalBand == WsfEM_Types::cOPT_IR_SHORT)
   {
      shorterWavelength = 1.0E-6;
      longerWavelength  = 3.0E-6;
   }
   else if (mOpticalBand == WsfEM_Types::cOPT_IR_MEDIUM)
   {
      shorterWavelength = 3.0E-6;
      longerWavelength  = 5.0E-6;
   }
   else if (mOpticalBand == WsfEM_Types::cOPT_IR_LONG)
   {
      shorterWavelength = 8.0E-6;
      longerWavelength  = 12.0E-6;
   }
   else if (mOpticalBand == WsfEM_Types::cOPT_IR_VERY_LONG)
   {
      shorterWavelength = 15.0E-6;
      longerWavelength  = 30.0E-6;
   }
   else
   {
      ut::log::error() << "Band not defined.";
      ok = false;
   }

   double lowerFrequency = UtMath::cLIGHT_SPEED / longerWavelength;
   double upperFrequency = UtMath::cLIGHT_SPEED / shorterWavelength;
   mRcvr.SetFrequency(0.5 * (lowerFrequency + upperFrequency));
   mRcvr.SetBandwidth(upperFrequency - lowerFrequency);

   // If an infrared band is used make sure the required parameters are defined.
   if (mOpticalBand != WsfEM_Types::cOPT_VISUAL)
   {
      if (mNoiseEquivalentIrradiance <= 0.0)
      {
         ut::log::error() << "'noise_equivalent_irradiance' not defined.";
         ok = false;
      }
      if (mDetectionThreshold <= 0.0)
      {
         ut::log::error() << "'detection_threshold' not defined.";
         ok = false;
      }
   }

   ok &= mAntenna.Initialize(mSensorPtr);
   mMaximumRange = mAntenna.GetMaximumRange();
   ok &= mRcvr.Initialize(*mSensorPtr->GetSimulation());
   mRcvr.SetDebugEnabled(GetSensor()->DebugEnabled());

   // If the user selected the old attenuation model, instantiate it...
   if (mAtmosphericAttenuation > 0.0)
   {
      // Allocate an attenuation model object that implements the old embedded model.
      WsfEM_Attenuation* attenuationPtr =
         new WsfOpticalAttenuation(mSensorPtr->GetScenario(), mAtmosphericAttenuation, mAtmosphere.GetAtmosphereType());
      if (attenuationPtr->Initialize(&mRcvr))
      {
         mRcvr.SetAttenuationModel(attenuationPtr);
      }
      else
      {
         ut::log::error() << "Failed to initialize 'attenuation_model'.";
         delete attenuationPtr;
      }
   }

   // If an antenna_pattern has been defined on the receiver then the old 'installation_effects' computation is performed.
   if (mRcvr.GetAntennaPattern(WsfEM_Types::cPOL_DEFAULT, mRcvr.GetFrequency()) != nullptr)
   {
      mComputeOldInstallationEffects = true;
   }

   return ok;
}

// =================================================================================================
// virtual
bool WsfEOIR_IRST_SensorMode::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (mEnvironment.ProcessInput(aInput))
   {
   }
   else if (command == "band")
   {
      std::string bandName;
      aInput.ReadValue(bandName);
      if (bandName == "visual")
      {
         mOpticalBand             = WsfEM_Types::cOPT_VISUAL;
         mInfraredBand            = WsfEM_Types::cIRBAND_COUNT;
         mUseSimpleVisualDetector = true;
      }
      else if (WsfEM_Util::StringToEnum(mInfraredBand, bandName))
      {
         switch (mInfraredBand)
         {
         case WsfEM_Types::cIRBAND_SHORT:
         case WsfEM_Types::cIRBAND_MEDIUM:
         case WsfEM_Types::cIRBAND_LONG:
         case WsfEM_Types::cIRBAND_VERY_LONG:
            mOpticalBand =
               static_cast<WsfEM_Types::OpticalBand>(mInfraredBand); // Also set the band in the optical spectrum
            break;
         default:
            throw UtInput::BadValue(aInput, "Invalid band: " + bandName);
            break;
         }
         mUseSimpleVisualDetector = false;
      }
      else
      {
         throw UtInput::BadValue(aInput, "Invalid band: " + bandName);
      }
      mOpticalBands.clear();
      mOpticalBands.push_back(mOpticalBand);
   }
   else if (command == "bands")
   {
      mOpticalBands.clear();
      UtInputBlock inputBlock(aInput);
      std::string  bandName;
      while (inputBlock.ReadCommand(bandName))
      {
         WsfEM_Types::OpticalBand opticalBand;
         if (WsfEM_Util::StringToEnum(opticalBand, bandName) && (opticalBand != WsfEM_Types::cOPT_DEFAULT))
         {
            mOpticalBands.push_back(opticalBand);
            // The first IR signature is the default IR signature...
            if ((opticalBand < WsfEM_Types::cOPT_IR_COUNT) && (mInfraredBand == WsfEM_Types::cIRBAND_COUNT))
            {
               mInfraredBand = static_cast<WsfEM_Types::InfraredBand>(opticalBand);
            }
         }
         else
         {
            throw UtInput::BadValue(aInput, "Invalid band: " + bandName);
         }
      }
      if (mOpticalBands.empty())
      {
         throw UtInput::BadValue(aInput, "At least band must be specified");
      }
      mOpticalBand             = mOpticalBands.front();
      mUseSimpleVisualDetector = false;
   }
   else if (command == "detect_negative_contrast")
   {
      aInput.ReadValue(mDetectNegativeContrast);
   }
   else if (command == "detector_model")
   {
      std::string model;
      aInput.ReadValue(model);
      if (model == "detailed")
      {
         mUseSimpleVisualDetector = false;
      }
      else if (model == "simple")
      {
         mUseSimpleVisualDetector = true;
      }
      else
      {
         throw UtInput::BadValue(aInput, "Invalid detector model: " + model);
      }
   }
   else if (command == "detector_gain")
   {
      aInput.ReadValue(mDetectorGain);
      aInput.ValueGreater(mDetectorGain, 0.0);
   }
   else if (command == "integration_gain")
   {
      aInput.ReadValue(mIntegrationGain);
      aInput.ValueGreater(mIntegrationGain, 0.0);
   }
   else if ((command == "nei") || (command == "NEI") || (command == "noise_equivalent_irradiance"))
   {
      std::string units;
      aInput.ReadValue(mNoiseEquivalentIrradiance);
      aInput.ValueGreater(mNoiseEquivalentIrradiance, 0.0);
      aInput.ReadValue(units);

      // Parse the units.
      std::string::size_type slashPos = units.find('/');
      if ((slashPos != std::string::npos) && (slashPos != 0) && ((slashPos + 1) != units.size()))
      {
         std::string powerUnits(units.substr(0, slashPos));
         std::string areaUnits(units.substr(slashPos + 1));
         double      powerFactor = aInput.ConvertValue(1.0, powerUnits, UtInput::cPOWER);
         double      areaFactor  = aInput.ConvertValue(1.0, areaUnits, UtInput::cAREA);
         double      multiplier  = powerFactor / areaFactor;
         mNoiseEquivalentIrradiance *= multiplier;
      }
      else
      {
         throw UtInput::BadValue(aInput, "Unknown irradiance units " + units);
      }
      // For automatic propagation to WsfEM_Interaction by BeginOneWayInteraction
      mRcvr.SetNoisePower(mNoiseEquivalentIrradiance);
   }
   else if (command == "detection_threshold")
   {
      aInput.ReadValue(mDetectionThreshold);
      aInput.ValueGreater(mDetectionThreshold, 0.0);
      // For automatic propagation to WsfEM_Interaction by BeginOneWayInteraction
      mRcvr.SetDetectionThreshold(mDetectionThreshold);
   }

   // Inputs for the embedded simple attenuation model.
   // This has been deprecated in favor of using 'attenuation_model' to reference to user-selectable model.
   else if (command == "atmospheric_attenuation") // NO_DOC - DEPRECATED
   {
      std::string per;
      std::string units;
      aInput.ReadValue(mAtmosphericAttenuation);
      aInput.ValueGreater(mAtmosphericAttenuation, 0.0);
      aInput.ReadValue(per);
      aInput.StringEqual(per, "per");
      aInput.ReadValue(units);
      mAtmosphericAttenuation /= UtInput::ConvertValueFrom(1.0, units, UtInput::cLENGTH);
      mRcvr.SetAttenuationModelType(WsfStringId()); // Disable explicit model
   }
   else if (mAtmosphere.ProcessInput(aInput)) // NO_DOC - DEPRECATED
   {
   }

   // More common parameters (must be last).
   else if (mAntenna.ProcessInput(aInput) || mRcvr.ProcessInput(aInput) || mRcvr.ProcessInputBlock(aInput))
   {
      if (!mRcvr.GetAttenuationModelType().IsNull()) // Disable simple model if explicit model selected
      {
         mAtmosphericAttenuation = 0.0;
      }
   }
   else
   {
      myCommand = WsfSensorMode::ProcessInput(aInput);
   }
   return myCommand;
}

// ================================================================================================
//! Perform common processing to start a detection attempt.
//!
//! This is called by AttemptToDetect to perform the common actions at the start of a detection attempt.
//!
//! @returns true if detection processing should proceed (i.e.: it is POTENTIALLY detectable)
bool WsfEOIR_IRST_SensorMode::BeginDetectionAttempt(double aSimTime, WsfPlatform* aTargetPtr, Settings& aSettings, Result& aResult)
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
      out.AddNote() << "Target: " << aTargetPtr->GetName();
   }

   // Determine if concealed (like in a building).
   if (aResult.mFailedStatus == 0)
   {
      aResult.mCheckedStatus |= Result::cCONCEALMENT;
      if (aTargetPtr->GetConcealmentFactor() > 0.99F)
      {
         // We can't detect if it's in a building (or something like that)
         aResult.mFailedStatus |= Result::cCONCEALMENT;
         // Must have object pointers so event_output and debug output show locations.
         aResult.BeginGenericInteraction(nullptr, aTargetPtr, &mRcvr);
      }
   }

   // Perform the detection attempt if not concealed and within range and angle limits.
   bool detectable = false;
   if ((aResult.mFailedStatus == 0) && (aResult.BeginOneWayInteraction(&mRcvr, aTargetPtr) == 0))
   {
      // Set the position of the antenna.
      aResult.SetReceiverBeamPosition();
      detectable = true;
   }
   return detectable;
}

// ================================================================================================
//! Perform common processing to start a detection attempt.
//!
//! This is called by AttemptToDetect to perform the common actions at the end of a detection attempt.
//!
//! @returns true if the target was detected and false if not.
bool WsfEOIR_IRST_SensorMode::EndDetectionAttempt(double aSimTime, WsfPlatform* aTargetPtr, Settings& aSettings, Result& aResult)
{
   bool detected = WsfSensorComponent::PostAttemptToDetect(*GetSensor(), aSimTime, aTargetPtr, aResult);

   // If all other detection criteria have been met, check if masked by terrain.
   if (detected)
   {
      aResult.MaskedByTerrain();
   }

   // Determine the impact (if any) that an external script should have on detection status
   detected = GetSensor()->ScriptAllowDetection(aSimTime, aTargetPtr, aResult);

   // Write debugging output if requested and invoke an observers of detection attempts.
   if (GetSensor()->DebugEnabled())
   {
      auto logDebug = ut::log::debug() << "Sensor Debug Info:";
      aResult.Print(logDebug);
   }
   GetSensor()->NotifySensorDetectionAttempted(aSimTime, aTargetPtr, aResult);
   return detected;
}

// ================================================================================================
void WsfEOIR_IRST_SensorMode::AttemptToDetectInfrared(double       aSimTime,
                                                      WsfPlatform* aTargetPtr,
                                                      Settings&    aSettings,
                                                      Result&      aResult)
{
   // Determine the infrared radiant intensity of the target.
   // First see if the optical signature will provide this, and if it doesn't then try the standard.
   // (i.e.: The source contrast... I_s)
   aResult.ComputeInfraredSigAzEl();
   if (!WsfOpticalSignature::GetRadiantIntensity(aSimTime,
                                                 aTargetPtr,
                                                 mOpticalBands,
                                                 aResult.mInfraredSigAz,
                                                 aResult.mInfraredSigEl,
                                                 aResult.mInfraredSig))
   {
      aResult.mInfraredSig =
         WsfInfraredSignature::GetValue(aSimTime, aTargetPtr, mInfraredBand, aResult.mInfraredSigAz, aResult.mInfraredSigEl);
   }

   // Determine the projected area of the target. (A_proj)
   if (aResult.mOpticalSig < 0.0) // If not determined by caller
   {
      aResult.ComputeOpticalSigAzEl();
      aResult.mOpticalSig =
         WsfOpticalSignature::GetValue(aSimTime, aTargetPtr, aResult.mOpticalSigAz, aResult.mOpticalSigEl);
   }

   // Determine the contrast signature, I_c = I_s - L_bkg * A_proj
   // First compute the background radiant intensity, L_bkg * A_proj.
   double L_bkg = mEnvironment.ComputeBackgroundRadiance(aSimTime, mOpticalBands, aResult, aResult.mInterferencePower);
   aResult.mBackgroundRadiantIntensity = L_bkg * aResult.mOpticalSig;

   // And then the contrast radiant intensity, I_c = I_s - L_bkg * A_proj
   aResult.mContrastRadiantIntensity = aResult.mInfraredSig - aResult.mBackgroundRadiantIntensity;

   if (mDetectNegativeContrast)
   {
      // If 'detect_negative_contrast' is true then negative contrast is the same as positive contrast.
      aResult.mContrastRadiantIntensity = fabs(aResult.mContrastRadiantIntensity);
   }
   else
   {
      // 'detect_negative_contrast' is false. If the contrast is negative then set it to near zero
      // (it can't be set to zero because that means it hasn't been calculated...)
      aResult.mContrastRadiantIntensity = std::max(aResult.mContrastRadiantIntensity, 1.0E-6);
   }

   // Compute the atmospheric transmittance, tau
   aResult.mAbsorptionFactor = aResult.ComputeAttenuationFactor(WsfEM_Interaction::cTARGET_TO_RCVR);

   // Compute the effective target irradiance
   //
   // E_eff = (tau * I_c) / R^2
   //
   //        tau = atmospheric transmittance
   //        I_c = contrast radiant intensity of target.
   double range                     = aResult.mRcvrToTgt.mRange;
   double effectiveTargetIrradiance = (aResult.mAbsorptionFactor * aResult.mContrastRadiantIntensity) / (range * range);

   // Adjust the received irradiance to account for installation effects (deprecated)
   if (mComputeOldInstallationEffects)
   {
      double installationEffects = ComputeInstallationEffects(aResult);
      effectiveTargetIrradiance *= installationEffects;
   }

   // Account for structural masking.
   effectiveTargetIrradiance *= aResult.mMaskingFactor;

   // Also factor in any 'internal losses', just like is done in the WsfEM_Rcvr::ComputeReceivedPower
   // (which was for RF only).
   effectiveTargetIrradiance /= mRcvr.GetInternalLoss();

   // TODO_JAJ Allow the signal to be gained up. Not sure about the mechanics. Integration gain also integrates noise???
   effectiveTargetIrradiance *= mDetectorGain;
   effectiveTargetIrradiance *= mIntegrationGain;

   aResult.mRcvdPower = effectiveTargetIrradiance;

   // Compute the signal to noise and determine the probability of detection
   aResult.mSignalToNoise = effectiveTargetIrradiance / (mNoiseEquivalentIrradiance + aResult.mInterferencePower);
   aResult.mPd            = ComputeProbabilityOfDetection(effectiveTargetIrradiance,
                                               mNoiseEquivalentIrradiance + aResult.mInterferencePower,
                                               mDetectionThreshold);

   // NOTE: Most other sensors declare success/failure based on comparing aResult.mPd and aSettings.mRequiredPd.
   //       (Failure is defined as aResult.mPd < aSettings.mRequiredPd).
   //       I went back thought the change log and it appears that was never the case for this sensor. (JAJ)

   // Determine if the signal exceeds the detection threshold.
   aResult.mCheckedStatus |= Result::cSIGNAL_LEVEL;
   if (aResult.mSignalToNoise < mDetectionThreshold)
   {
      aResult.mFailedStatus |= Result::cSIGNAL_LEVEL;
   }
}

// ================================================================================================
void WsfEOIR_IRST_SensorMode::AttemptToDetectVisual(double aSimTime, WsfPlatform* aTargetPtr, Settings& aSettings, Result& aResult)
{
   // Determine the projected area of the target. (A_proj)
   if (aResult.mOpticalSig < 0.0) // If not determined by caller
   {
      aResult.ComputeOpticalSigAzEl();
      aResult.mOpticalSig =
         WsfOpticalSignature::GetValue(aSimTime, aTargetPtr, aResult.mOpticalSigAz, aResult.mOpticalSigEl);
   }

   // Compute the background radiance, L_bkg
   double backgroundRadiance =
      mEnvironment.ComputeBackgroundRadiance(aSimTime, mOpticalBands, aResult, aResult.mInterferencePower);

   // Compute the atmospheric transmittance, tau (in aResult.mAbsorptionFactor)
   aResult.mAbsorptionFactor = aResult.ComputeAttenuationFactor(Result::cTARGET_TO_RCVR);

   // Compute the background radiance at the sensor
   double pathRadiance               = mEnvironment.ComputePathRadiance(aSimTime, mOpticalBands, aResult);
   double backgroundRadianceAtSensor = backgroundRadiance * aResult.mAbsorptionFactor + pathRadiance;

   // Compute the inherent contrast of the target. This will use the background radiance and target intensity if they
   // are available, otherwise it will fall back to the old table form...
   double inherentContrast = 0.0;
   if (backgroundRadiance > 0.0)
   {
      double targetRadiantIntensity = 0.0;
      if (WsfOpticalSignature::GetRadiantIntensity(aSimTime,
                                                   aTargetPtr,
                                                   mOpticalBands,
                                                   aResult.mOpticalSigAz,
                                                   aResult.mOpticalSigEl,
                                                   targetRadiantIntensity))
      {
         // First compute the target radiance, L_tgt = I_tgt / A_proj
         double targetRadiance = targetRadiantIntensity / aResult.mOpticalSig;

         // Compute the background radiant intensity for event_output, I_bkg = L_bkg * A_proj.
         aResult.mBackgroundRadiantIntensity = backgroundRadiance * aResult.mOpticalSig;

         // The 'inherent_contrast' is L_tgt / L_bkg
         inherentContrast = targetRadiance / backgroundRadiance;
      }
   }
   // If the inherent contrast was not computed then look it up in the old table.
   if (inherentContrast == 0.0)
   {
      inherentContrast = WsfInherentContrast::GetValue(aTargetPtr,
                                                       aResult.mOpticalSigAz,
                                                       aResult.mOpticalSigEl,
                                                       WsfOpticalSignature::UsesInherentContrast(aTargetPtr));
   }

   // Compute the contrast at the sensor
   double contrastAtSensor = inherentContrast * aResult.mAbsorptionFactor; // worst case without background radiance
   if (backgroundRadianceAtSensor > 0.0)
   {
      contrastAtSensor *= backgroundRadiance / backgroundRadianceAtSensor; // scale it with the background if non-zero
   }

   // Adjust the received irradiance to account for installation effects (deprecated)
   if (mComputeOldInstallationEffects)
   {
      double installationEffects = ComputeInstallationEffects(aResult);
      contrastAtSensor *= installationEffects;
   }

   // Account for structural masking.
   contrastAtSensor *= aResult.mMaskingFactor;

   // Also factor in any 'internal losses', just like is done in the WsfEM_Rcvr::ComputeReceivedPower
   // (which was for RF only).
   contrastAtSensor /= mRcvr.GetInternalLoss();

   // If the Sun is in the line of sight then set the contrast to zero...
   if (aResult.mInterferencePower > 0.0)
   {
      contrastAtSensor = 0.0;
   }
   aResult.mPd = (contrastAtSensor == 0.0) ? 0.0 : 1.0;

#ifdef TODO
   // Compute solid angle of target at sensor
   double range      = aResult.mRcvrToTgt.mRange;
   double targetSize = aResult.mOpticalSig / (range * range);

   // Determine the probability of detection
   aResult.mPd = ComputeProbabilityOfDetection(targetSize, contrastAtSensor, aTargetPtr->GetIndex());
#endif

   // see if the resulting Pd results in detection
   aResult.mCheckedStatus |= Result::cSIGNAL_LEVEL;
   if (aResult.mPd < aSettings.mRequiredPd)
   {
      aResult.mFailedStatus |= Result::cSIGNAL_LEVEL;
   }
}

// =================================================================================================
//! Compute the effects on reception due to the sensors installation on the vehicle.
//! @returns A multiplication factor for the effective target irradiance.
//! @note This method is deprecated. It allows the user to define an antenna_pattern that defines
//! the loss. The preferred method is to use 'masking_pattern'.
double WsfEOIR_IRST_SensorMode::ComputeInstallationEffects(Result& aResult)
{
   // This is a bit of a hack, because an infrared device does not actually have an antenna, but it is used here
   // to account for aspect dependent losses caused by occlusion or vignetting by the aperture window (or any
   // other effects that might be aspect dependent.

   // NOTE: The antenna lookup angles are not relative to the 'beam' as they are in the radar model - they are
   //       relative to the installed orientation of the sensor (/antenna).
   double tgtLocACS[3];
   mAntenna.ConvertWCS_VectorToACS(aResult.mRcvrToTgt.mUnitVecWCS, tgtLocACS);
   UtEntity::ComputeAzimuthAndElevation(tgtLocACS, aResult.mRcvrBeam.mAz, aResult.mRcvrBeam.mEl);
   aResult.mRcvrBeam.mEBS_Az = 0.0;
   aResult.mRcvrBeam.mEBS_El = 0.0;

   aResult.mRcvrBeam.mGain    = mRcvr.GetAntennaGain(mRcvr.GetPolarization(),
                                                  mRcvr.GetFrequency(),
                                                  aResult.mRcvrBeam.mAz,
                                                  aResult.mRcvrBeam.mEl,
                                                  aResult.mRcvrBeam.mEBS_Az,
                                                  aResult.mRcvrBeam.mEBS_El);
   double installationEffects = aResult.mRcvrBeam.mGain;
   return installationEffects;
}

// =================================================================================================
//! Compute the probability of detection.
//!
//! @param aSignal [input] The effective target irradiance (CEI)
//! @param aNoise  [input] The noise equivalent irradiance (NEI)
//! @param aThreshold [input] The signal-to-noise ratio required to generate a Pd = 0.5.
//!
//! @returns The probability of detection in the range [0, 1]
double WsfEOIR_IRST_SensorMode::ComputeProbabilityOfDetection(double aSignal, double aNoise, double aThreshold)
{
   if (aSignal <= 0.0)
   {
      return 0.0;
   }

   double signalToNoise = aSignal / aNoise;
   double beta          = signalToNoise - aThreshold;

#if 1
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

#else

   // The following is a pretty darn close approximation to the above.

   double              pd        = 0.0;
   static const double cBASE     = 10.0;
   static const double cEXPONENT = -0.5;

   // This pretty closely approximates the function
   if (beta >= 0.0)
   {
      // pd = 0.5 + 0.5 * (1.0 - pow(10.0, -0.5 * beta));
      pd = 1.0 - 0.5 * pow(cBASE, cEXPONENT * beta);
   }
   else
   {
      // pd = 0.5 - 0.5 * (1.0 - pow(10.0,  0.5 * beta));
      pd = 0.5 * pow(cBASE, cEXPONENT * (-beta));
   }
#endif

   return pd;
}
