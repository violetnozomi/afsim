// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "SOSM_Sensor.hpp"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>

#include "SOSM_Atmosphere.hpp"
#include "SOSM_Interaction.hpp"
#include "SOSM_Manager.hpp"
#include "SOSM_TableVar.hpp"
#include "SOSM_Target.hpp"
#include "SOSM_Utility.hpp"
#include "TblLookup.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"

namespace
{
const double cMEAN_EARTH_RADIUS = 6.371E+6; // m (Same as in SOSM_SimpleAtmosphere)
}

// =================================================================================================
SOSM_Sensor::SOSM_Sensor(SOSM_Manager* aManagerPtr)
   : SOSM_CoreBase()
   , mManagerPtr(aManagerPtr)
   , mSharedDataPtr(new SharedData())
   , mAtmospherePtr(nullptr)
   , mAtmosphereSelector()
{
}

// =================================================================================================
SOSM_Sensor::SOSM_Sensor(const SOSM_Sensor& aSrc)
   : SOSM_CoreBase(aSrc)
   , mManagerPtr(aSrc.mManagerPtr)
   , mSharedDataPtr(aSrc.mSharedDataPtr)
   , mAtmospherePtr(nullptr)
   , mAtmosphereSelector()
{
}

// =================================================================================================
// virtual
SOSM_Sensor::~SOSM_Sensor()
{
   delete mAtmospherePtr;
}

// =================================================================================================
// virtual
SOSM_Sensor* SOSM_Sensor::Clone() const
{
   return new SOSM_Sensor(*this);
}

// =================================================================================================
// virtual
bool SOSM_Sensor::Initialize()
{
   bool ok = true;

   // Attach an instance of the atmosphere object.
   if (!mSharedDataPtr->mAtmosphereType.empty())
   {
      mAtmospherePtr = mManagerPtr->AllocateAtmosphere(mSharedDataPtr->mAtmosphereType);
      if (mAtmospherePtr == nullptr)
      {
         auto logError = ut::log::error() << "Unable to instantiate atmosphere_type.";
         logError.AddNote() << "atmosphere_type: " << mSharedDataPtr->mAtmosphereType;
         return false;
      }
   }
   else if (mAtmospherePtr == nullptr)
   {
      mAtmospherePtr = mManagerPtr->AllocateDefaultAtmosphere();
      if (mAtmospherePtr == nullptr)
      {
         ut::log::error() << "Unable to allocate the default atmosphere.";
         return false;
      }
   }

   // Compute the intersection between the sensor and the atmosphere objects
   SOSM_Selector::Status status = mAtmosphereSelector.Intersect(*mAtmospherePtr, *this);
   if (status != SOSM_Selector::cINTERSECTS)
   {
      auto logError = ut::log::error() << "Atmosphere/Sensor intersection error. ";
      logError.AddNote() << "Status: " << SOSM_Selector::StatusString(status);
      logError.AddNote() << "Atmosphere: Origin: " << mAtmospherePtr->Origin();
      logError.AddNote() << "Increment: " << mAtmospherePtr->Increment();
      logError.AddNote() << "Count: " << mAtmospherePtr->Count();
      logError.AddNote() << "Sensor.Origin: " << Origin();
      logError.AddNote() << "Increment: " << Increment();
      logError.AddNote() << "Count: " << Count();
      ok = false;
   }
   return ok;
}

// =================================================================================================
// virtual
void SOSM_Sensor::InputComplete()
{
   mSharedDataPtr->InputComplete(*this);
   DefineSpectralLimits(*mSharedDataPtr);
}

// =================================================================================================
// virtual
bool SOSM_Sensor::ProcessInput(UtInput& aInput)
{
   return mSharedDataPtr->ProcessInput(aInput, *this);
}

// =================================================================================================
//! Compute the integrated atmospheric data for the 'fast' detection model.
//!
//! This method computes the response-biased transmittance and radiance for the specified interaction.
//!
//! @param aInteraction           [input]  The interaction object representing the sensor and target state.
//! @param aBackgroundRadiance_xR [output] The integrated background radiance.
//! @param aForegroundRadiance_xR [output] The integrated foreground radiance.
//! @param aTransmittance_xR      [output] The integrated foreground transmittance.
//!
void SOSM_Sensor::ComputeIntegratedAtmosphericData(SOSM_Interaction& aInteraction,
                                                   float&            aBackgroundRadiance_xR,
                                                   float&            aForegroundRadiance_xR,
                                                   float&            aTransmittance_xR)
{
   // Compute the atmosphere data (to background and to target);
   std::vector<float> backgroundRadiance(Count()); // L_B (W/cm^2/sr/um)
   std::vector<float> foregroundRadiance(Count()); // L_F (W/cm^2/sr/um)
   std::vector<float> foregroundTransmittance(Count());
   mAtmospherePtr->ComputeAtmosphereData(mAtmosphereSelector,
                                         aInteraction,
                                         backgroundRadiance,
                                         foregroundRadiance,
                                         foregroundTransmittance);

   // **************************************************************************
   // NOTE: See ComputeTargetIrradiance for comments on the integration method.
   // **************************************************************************

   int   i;
   float f;
   int   n = Count() - 1;

   f                          = mSharedDataPtr->mFirstIntervalFraction;
   backgroundRadiance[0]      = backgroundRadiance[1] + f * (backgroundRadiance[0] - backgroundRadiance[1]);
   foregroundRadiance[0]      = foregroundRadiance[1] + f * (foregroundRadiance[0] - foregroundRadiance[1]);
   foregroundTransmittance[0] = foregroundTransmittance[1] + f * (foregroundTransmittance[0] - foregroundTransmittance[1]);

   f                     = mSharedDataPtr->mLastIntervalFraction;
   backgroundRadiance[n] = backgroundRadiance[n - 1] + f * (backgroundRadiance[n] - backgroundRadiance[n - 1]);
   foregroundRadiance[n] = foregroundRadiance[n - 1] + f * (foregroundRadiance[n] - foregroundRadiance[n - 1]);
   foregroundTransmittance[n] =
      foregroundTransmittance[n - 1] + f * (foregroundTransmittance[n] - foregroundTransmittance[n - 1]);

   double backgroundRadiance_xR = 0.0;
   double foregroundRadiance_xR = 0.0;
   double transmittance_xR      = 0.0;

   for (i = 0; i < n; ++i)
   {
      f         = mSharedDataPtr->mIntegFactor[i];
      float L_B = f * (backgroundRadiance[i] + backgroundRadiance[i + 1]);
      float L_F = f * (foregroundRadiance[i] + foregroundRadiance[i + 1]);
      f         = mSharedDataPtr->mTransFactor[i];
      float tau = f * (foregroundTransmittance[i] + foregroundTransmittance[i + 1]);

      backgroundRadiance_xR += L_B;
      foregroundRadiance_xR += L_F;
      transmittance_xR += tau;
   }

   aBackgroundRadiance_xR = static_cast<float>(backgroundRadiance_xR);
   aForegroundRadiance_xR = static_cast<float>(foregroundRadiance_xR);
   aTransmittance_xR      = static_cast<float>(transmittance_xR / mSharedDataPtr->mSumIntervalSize);
}

// =================================================================================================
//! Compute the integrated target data for the 'fast' detection model.
//! @param aTargetPtr          [input]  The target under consideration.
//! @param aInteraction        [input]  The interaction object representing the sensor and target state.
//! @param aTargetIntensity_xR [output] The integrated, response-biased radiant intensity. (W/sr)
//! @param aProjectedArea      [output] The projected area (cm^2)
//! @note The caller must call the targets 'SelectState' function prior to calling this method.
void SOSM_Sensor::ComputeIntegratedTargetData(SOSM_Target*      aTargetPtr,
                                              SOSM_Interaction& aInteraction,
                                              float&            aTargetIntensity_xR,
                                              float&            aProjectedArea)
{
   // Determine the intensity and projected area of the target.

   std::vector<float> bodyIntensity(Count());  // I_AF (W/sr/um)
   float              bodyArea;                // cm^2
   std::vector<float> plumeIntensity(Count()); // I_PL (W/sr/um)
   float              plumeArea;               // cm^2
   aTargetPtr->ComputeRadiantIntensity(*this, aInteraction, bodyIntensity, bodyArea, plumeIntensity, plumeArea);

   // **************************************************************************
   // NOTE: See ComputeTargetIrradiance for comments on the integration method.
   // **************************************************************************

   int   i;
   float f;
   int   n = Count() - 1;

   f                 = mSharedDataPtr->mFirstIntervalFraction;
   bodyIntensity[0]  = bodyIntensity[1] + f * (bodyIntensity[0] - bodyIntensity[1]);
   plumeIntensity[0] = plumeIntensity[1] + f * (plumeIntensity[0] - plumeIntensity[1]);

   f                 = mSharedDataPtr->mLastIntervalFraction;
   bodyIntensity[n]  = bodyIntensity[n - 1] + f * (bodyIntensity[n] - bodyIntensity[n - 1]);
   plumeIntensity[n] = plumeIntensity[n - 1] + f * (plumeIntensity[n] - plumeIntensity[n - 1]);

   double bodyIntensity_xR = 0.0;
   for (i = 0; i < n; ++i)
   {
      f         = mSharedDataPtr->mIntegFactor[i];
      float I_S = f * (bodyIntensity[i] + bodyIntensity[i + 1] + plumeIntensity[i] + plumeIntensity[i + 1]);

      bodyIntensity_xR += I_S;
   }
   aTargetIntensity_xR = static_cast<float>(bodyIntensity_xR);
   aProjectedArea      = bodyArea + plumeArea;
}

// =================================================================================================
//! Compute the probability of detection for a specified target irradiance.
//! @param aTargetIrradiance [input] The irradiance on the sensor while looking at the target of interest. (W/cm^2)
//! @param aInteraction      [input] The interaction object representing the sensor and target state.
//! @returns The probability of detection in the range [0..1].
float SOSM_Sensor::ComputeProbabilityOfDetection(float aTargetIrradiance, SOSM_Interaction& aInteraction) const
{
   aInteraction.mNoiseEquivalentIrradiance = mSharedDataPtr->mNoiseEquivalentIrradiance;

   // Determine the detection threshold to use (above horizon or below horizon)

   aInteraction.mDetectionThreshold = mSharedDataPtr->mDetectionThresholdAboveHorizon;
   if (mSharedDataPtr->mDetectionThresholdAboveHorizon != mSharedDataPtr->mDetectionThresholdBelowHorizon)
   {
      // Determine the depression angle of the horizon.
      double earthRadius           = cMEAN_EARTH_RADIUS;
      double cosTheta              = earthRadius / (earthRadius + aInteraction.GetSensorAltitude());
      double horizonElevationAngle = -acos(cosTheta);
      if (aInteraction.GetAbsoluteTargetElevation() < horizonElevationAngle)
      {
         aInteraction.mDetectionThreshold = mSharedDataPtr->mDetectionThresholdBelowHorizon;
      }
   }

   // Determine any installation effects (windows, structure, etc).

   aInteraction.mInstallationAdjustment = 1.0F;
   if (mSharedDataPtr->mInstallationAdjustmentTablePtr)
   {
      float snrToTgtAz;
      float snrToTgtEl;
      aInteraction.GetSensorToTargetAspect(snrToTgtAz, snrToTgtEl);
      snrToTgtAz = static_cast<float>(UtMath::NormalizeAngleMinusPi_Pi(snrToTgtAz));
      TblLookupLU<float> azLookup;
      TblLookupLU<float> elLookup;
      mSharedDataPtr->mInstallationAdjustmentTablePtr->PolarLookup(azLookup, elLookup, snrToTgtAz, snrToTgtEl);
      aInteraction.mInstallationAdjustment =
         mSharedDataPtr->mInstallationAdjustmentTablePtr->Interpolate(azLookup, elLookup);
   }
   double pd = ComputeProbabilityOfDetection(aTargetIrradiance * aInteraction.mInstallationAdjustment,
                                             aInteraction.mNoiseEquivalentIrradiance,
                                             aInteraction.mDetectionThreshold);
   if (mManagerPtr->DebugLevel() > 0)
   {
      ut::log::debug() << "Pd: " << pd;
   }
   return static_cast<float>(pd);
}

// =================================================================================================
//! Compute the irradiance on the sensor when looking at the specified target.
//! @param aTargetPtr   [input] The target under consideration.
//! @param aInteraction [input] The interaction object representing the sensor and target state.
//! @returns The irradiance on the sensor (W/cm^2)
float SOSM_Sensor::ComputeTargetIrradiance(SOSM_Target* aTargetPtr, SOSM_Interaction& aInteraction)
{
   // Select the target state based on the current target conditions.
   aTargetPtr->SelectState(aInteraction);

   // Determine the intensity and projected area of the target.

   std::vector<float> bodyIntensity(Count());  // I_S (W/sr/um)
   float              bodyArea;                // cm^2
   std::vector<float> plumeIntensity(Count()); // I_PL (W/sr/um)
   float              plumeArea;               // cm^2
   aTargetPtr->ComputeRadiantIntensity(*this, aInteraction, bodyIntensity, bodyArea, plumeIntensity, plumeArea);

   // Compute the atmosphere data (to background and to target).

   std::vector<float> backgroundRadiance(Count());      // L_B (W/cm^2/sr/um)
   std::vector<float> foregroundRadiance(Count());      // L_F (W/cm^2/sr/um)
   std::vector<float> foregroundTransmittance(Count()); // Unitless [0..1]
   mAtmospherePtr->ComputeAtmosphereData(mAtmosphereSelector,
                                         aInteraction,
                                         backgroundRadiance,
                                         foregroundRadiance,
                                         foregroundTransmittance);

   // **************************************************************************
   // NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE
   //
   // If you change this integration code here, you will also have to change
   // ComputeIntegratedAtmosphereData and ComputeIntegratedTargetData!
   // **************************************************************************

   int   i;
   float f;
   int   n = Count() - 1;

   // Overwrite the first and last values for the target radiant intensity, background radiance
   // and foreground radiance to only include the portion that is in the band of the sensor.

   f                          = mSharedDataPtr->mFirstIntervalFraction;
   bodyIntensity[0]           = bodyIntensity[1] + f * (bodyIntensity[0] - bodyIntensity[1]);
   plumeIntensity[0]          = plumeIntensity[1] + f * (plumeIntensity[0] - plumeIntensity[1]);
   backgroundRadiance[0]      = backgroundRadiance[1] + f * (backgroundRadiance[0] - backgroundRadiance[1]);
   foregroundRadiance[0]      = foregroundRadiance[1] + f * (foregroundRadiance[0] - foregroundRadiance[1]);
   foregroundTransmittance[0] = foregroundTransmittance[1] + f * (foregroundTransmittance[0] - foregroundTransmittance[1]);

   f                     = mSharedDataPtr->mLastIntervalFraction;
   bodyIntensity[n]      = bodyIntensity[n - 1] + f * (bodyIntensity[n] - bodyIntensity[n - 1]);
   plumeIntensity[n]     = plumeIntensity[n - 1] + f * (plumeIntensity[n] - plumeIntensity[n - 1]);
   backgroundRadiance[n] = backgroundRadiance[n - 1] + f * (backgroundRadiance[n] - backgroundRadiance[n - 1]);
   foregroundRadiance[n] = foregroundRadiance[n - 1] + f * (foregroundRadiance[n] - foregroundRadiance[n - 1]);
   foregroundTransmittance[n] =
      foregroundTransmittance[n - 1] + f * (foregroundTransmittance[n] - foregroundTransmittance[n - 1]);

   // Loop through spectral radiance and transmittance data and apply any user provided scale factors
   for (i = 0; i < n; ++i)
   {
      backgroundRadiance[i] *= aInteraction.mBackgroundScaleFactor;         // Default 1.0
      foregroundRadiance[i] *= aInteraction.mForegroundScaleFactor;         // Default 1.0
      foregroundTransmittance[i] *= aInteraction.mTransmittanceScaleFactor; // Default 1.0
      if (aInteraction.mTransmittanceScaleFactor < 0.01F)
      {
         backgroundRadiance[i] = foregroundRadiance[i];
      }
   }

   // Compute the effective target contrast radiant intensity (I_c) at the sensor

   double bodyIntensity_xR      = 0.0; // sum(I_AF(i) * R(i))
   double bodyIntensity_xRxT    = 0.0; // sum(I_AF(i) * R(i) * tau(i))
   double plumeIntensity_xR     = 0.0; // sum(I_PL(i) * R(i))
   double plumeIntensity_xRxT   = 0.0; // sum(I_PL(i) * R(i) * tau(i))
   double backgroundRadiance_xR = 0.0; // sum(L_B(i) * R(i))
   double foregroundRadiance_xR = 0.0; // sum(L_F(i) * R(i))
   double contrastIntensity_xR  = 0.0;

   // Integrate the intensity and radiance using trapezoidal integration.
   //
   //                  wl(i) - wl(i-1)
   // Note: f = R(i) * ---------------
   //                       2.0
   //
   // Where: R(i)    is the response between sample points i and i+1.
   //        wl(i)   is the wavelength for sample point i.

   float targetArea = bodyArea + plumeArea;

   for (i = 0; i < n; ++i)
   {
      f          = mSharedDataPtr->mIntegFactor[i];
      float I_AF = f * (bodyIntensity[i] + bodyIntensity[i + 1]);
      float I_PL = f * (plumeIntensity[i] + plumeIntensity[i + 1]);
      float L_B  = f * (backgroundRadiance[i] + backgroundRadiance[i + 1]);
      float L_F  = f * (foregroundRadiance[i] + foregroundRadiance[i + 1]);
      float tau  = 0.5F * (foregroundTransmittance[i] + foregroundTransmittance[i + 1]);

      bodyIntensity_xR += I_AF;          // body intensity at source
      bodyIntensity_xRxT += I_AF * tau;  // body intensity at sensor
      plumeIntensity_xR += I_PL;         // plume intensity at source
      plumeIntensity_xRxT += I_PL * tau; // plume intensity at sensor
      backgroundRadiance_xR += L_B;
      foregroundRadiance_xR += L_F;

      float I_S = (I_AF + I_PL) * tau; // target intensity at sensor
      float I_B = L_B * targetArea;    // background intensity at sensor
      float I_F = L_F * targetArea;    // foreground intensity at sensor
      contrastIntensity_xR += (I_S + I_F - I_B);
   }

   // And FINALLY, compute the effective target contrast irradiance.

   double slantRange = std::max(static_cast<double>(aInteraction.GetSlantRange()), 1.0);
   slantRange *= 1.0E+2F; // m^2 -> cm->2
   // fabs() allows for positive or negative contrast
   float targetIrradiance = static_cast<float>(fabs(contrastIntensity_xR) / (slantRange * slantRange)); // W/cm^2

   // Propagate internal values to the interaction interface.

   double temp;
   aInteraction.mBodyArea           = static_cast<float>(bodyArea);
   aInteraction.mBodyIntensity      = static_cast<float>(bodyIntensity_xRxT);
   aInteraction.mPlumeArea          = static_cast<float>(plumeArea);
   aInteraction.mPlumeIntensity     = static_cast<float>(plumeIntensity_xRxT);
   temp                             = std::max(bodyIntensity_xR, 1.0E-17);
   aInteraction.mTransmittance      = static_cast<float>(bodyIntensity_xRxT / temp);
   aInteraction.mForegroundRadiance = static_cast<float>(foregroundRadiance_xR);
   aInteraction.mBackgroundRadiance = static_cast<float>(backgroundRadiance_xR);
   aInteraction.mContrastIntensity  = static_cast<float>(contrastIntensity_xR);

   if (mManagerPtr->DebugLevel() > 0)
   {
      auto logDebug = ut::log::debug() << "SOSM_Sensor::ComputeTargetIrradiance Data Report";
      aInteraction.Print(logDebug, mManagerPtr->GetUtAtmosphere(), this, aTargetPtr);
      logDebug.AddNote() << "Target Irradiance: " << targetIrradiance << " W/cm^2";

      if (mManagerPtr->DebugLevel() >= 2)
      {
         auto logNote = logDebug.AddNote() << "Data by Wavenumber:";
         SOSM_Utility::PrintSpectralVector(logNote, *this, bodyIntensity, "Body Radiant Intensity (I_AF) (W/sr/um)");
         SOSM_Utility::PrintSpectralVector(logNote, *this, plumeIntensity, "Plume Radiant Intensity (I_AF) (W/sr/um)");
         SOSM_Utility::PrintSpectralVector(logNote, *this, backgroundRadiance, "Background Radiance (L_B) (W/cm^2/sr/um)");
         SOSM_Utility::PrintSpectralVector(logNote, *this, foregroundRadiance, "Foreground Radiance (L_F) (W/cm^2/sr/um)");
         SOSM_Utility::PrintSpectralVector(logNote, *this, foregroundTransmittance, "Foreground Transmittance (tau_F)");
      }
   }

   if (mManagerPtr->ShowIRIPP_Data())
   {
      auto   logData       = ut::log::info() << "IRIPP_DATA:";
      double targetSum     = 0.0;
      double backgroundSum = 0.0;
      double foregroundSum = 0.0;
      double contrastSum   = 0.0;
      for (int j = 0; j < Count(); ++j)
      {
         i                = Count() - j - 1;
         float wavenumber = Origin() + (i * Increment());
         float wavelength = 1.0E+4F / wavenumber;
         float tau        = foregroundTransmittance[i];
         float I_S        = (bodyIntensity[i] + plumeIntensity[i]) * tau;
         float I_B        = backgroundRadiance[i] * targetArea;
         float I_F        = foregroundRadiance[i] * targetArea;
         float cI         = I_S + I_F - I_B;

         auto line = logData.AddNote() << j << ":";
         line.AddNote() << "Wavelength: " << std::fixed << std::setprecision(3) << wavelength << " um";
         { // RAII block
            auto intensityNote = line.AddNote() << "Intensity:" << std::scientific << std::setprecision(4);
            intensityNote.AddNote() << "Contrast: " << cI << " W/sr/um";
            intensityNote.AddNote() << "Target: " << I_S << " W/sr/um";
            intensityNote.AddNote() << "Background: " << I_B << " W/sr/um";
            intensityNote.AddNote() << "Foreground: " << I_F << " W/sr/um";
         }
         line.AddNote() << "Atmospheric Transmittance: " << std::fixed << std::setprecision(7) << tau;
         line.AddNote() << "Wave Number: " << std::fixed << std::setprecision(1) << wavenumber;


         if (i < (Count() - 1))
         {
            // Compute the integration term using trapezoidal integration.
            f         = mSharedDataPtr->mIntegFactor[i];
            I_S       = f * (bodyIntensity[i] + bodyIntensity[i + 1] + plumeIntensity[i] + plumeIntensity[i + 1]);
            float L_B = f * (backgroundRadiance[i] + backgroundRadiance[i + 1]);
            float L_F = f * (foregroundRadiance[i] + foregroundRadiance[i + 1]);
            tau       = 0.5F * (foregroundTransmittance[i] + foregroundTransmittance[i + 1]);
            I_S       = I_S * tau;
            I_B       = L_B * targetArea;
            I_F       = L_F * targetArea;
            cI        = I_S + I_F - I_B;
            targetSum += I_S;
            backgroundSum += I_B;
            foregroundSum += I_F;
            contrastSum += cI;
         }
      }
      auto line = logData.AddNote() << "Totals:" << std::scientific << std::setprecision(4);
      line.AddNote() << "Contrast: " << contrastSum << " W/sr";
      line.AddNote() << "Target: " << targetSum << " W/sr";
      line.AddNote() << "Background: " << backgroundSum << " W/sr";
      line.AddNote() << "Foreground: " << foregroundSum << " W/sr";
   }
   return targetIrradiance; // W/cm^2
}

// =================================================================================================
//! Compute the probability of detection.
//!
//! @param aSignal [input] The effective target irradiance (CEI) (W/cm^2)
//! @param aNoise  [input] The noise equivalent irradiance (NEI) (W/cm^2)
//! @param aThreshold [input] The signal-to-noise ratio required to generate a Pd = 0.5.
//!
//! @returns The probability of detection in the range [0, 1]
// static
double SOSM_Sensor::ComputeProbabilityOfDetection(double aSignal, double aNoise, double aThreshold)
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
   // where Q is the Guassian Q function.
   //
   // The second term is very small compared to the first so:
   //
   // Pd = Q(-beta) = 1 - Q(beta)
   //
   // THe 'Handbook of Mathematical Functions' (Abramowitz and Stegun) says:
   //
   // P(x) + Q(x) = 1         (equation 26.2.5)
   // P(x) = 1 - Q(x)
   //
   // Pd = P(x)
   //
   // The following is the approximation for P(x) as defined in 26.2.17

   const double cCONST = 0.39894228; // 1.0 / sqrt(2*pi)
   const double cP     = 0.2316419;
   const double cB1    = 0.319381530;
   const double cB2    = -0.356563782;
   const double cB3    = 1.781477937;
   const double cB4    = -1.821255978;
   const double cB5    = 1.330274429;

   double x  = beta;
   double z  = cCONST * exp(-0.5 * x * x);
   double pd = 0.5;
   if (x > 7.5E-8)
   {
      double t  = 1.0 / (1.0 + cP * x);
      double t2 = t * t;
      double t3 = t * t2;
      pd        = 1.0 - z * ((cB1 * t) + (cB2 * t2) + (cB3 * t3) + (cB4 * t2 * t2) + (cB5 * t2 * t3));
   }
   else if (x < -7.5E-8)
   {
      double t  = 1.0 / (1.0 - cP * x);
      double t2 = t * t;
      double t3 = t * t2;
      pd        = z * ((cB1 * t) + (cB2 * t2) + (cB3 * t3) + (cB4 * t2 * t2) + (cB5 * t2 * t3));
   }
   return pd;
}

// =================================================================================================
//! Return the response curve of the sensor.
//! @param aWavelength [output] The vector of wavelengths (in microns).
//! @param aResponse   [output] The vector of unitless response [0..1] for each wavelength
void SOSM_Sensor::GetResponseCurve(std::vector<float>& aWavelength, std::vector<float>& aResponse) const
{
   size_t pointCount = mSharedDataPtr->mResponseCurve.size();
   aWavelength.resize(pointCount);
   aResponse.resize(pointCount);
   for (size_t i = 0; i < pointCount; ++i)
   {
      const ResponsePoint& pt = mSharedDataPtr->mResponseCurve[i];
      aWavelength[i]          = pt.mWavelength;
      aResponse[i]            = pt.mResponse;
   }
}

// =================================================================================================
// Nested class 'SharedData'.
// =================================================================================================

// =================================================================================================
SOSM_Sensor::SharedData::SharedData()
   : mResponseCurve()
   , mLowerWavelength(0.0F)
   , mUpperWavelength(0.0F)
   , mNoiseEquivalentIrradiance(0.0F)
   , mDetectionThresholdAboveHorizon(0.0F)
   , mDetectionThresholdBelowHorizon(0.0F)
   , mInstallationAdjustmentTablePtr()
   , mAtmosphereType()
   , mShowResponse(false)
   , mIntegFactor()
   , mTransFactor()
   , mFirstIntervalFraction(0.0F)
   , mLastIntervalFraction(0.0F)
   , mSumIntervalSize(0.0F)
   , mAverageResponse(1.0F)
{
   DefineSpectralLimits(0.0F, 20.0F, 0);
}

// =================================================================================================
float SOSM_Sensor::SharedData::ComputeAverageResponse(const TblIndVarU<float>& aWavelengthTbl,
                                                      const TblDepVar1<float>& aResponseTbl,
                                                      float                    aLowerWavelength,
                                                      float                    aUpperWavelength)
{
   TblLookupLU<float> wavelengthLookup;

   // Sample and sum the response at increments of 10% of the difference between
   // the lower and upper wavelengths.

   double responseSum = 0.0;
   for (int i = 0; i < 11; ++i)
   {
      float wavelength = aLowerWavelength + (i * 0.1F * (aUpperWavelength - aLowerWavelength));
      wavelengthLookup.Lookup(aWavelengthTbl, wavelength);
      float response = TblEvaluate(aResponseTbl, wavelengthLookup);
      responseSum += response;
   }

   return static_cast<float>(responseSum / 11.0);
}

// =================================================================================================
void SOSM_Sensor::SharedData::InputComplete(SOSM_Sensor& aSensor)
{
   if ((mLowerWavelength <= 0.0F) || (mUpperWavelength <= 0.0F))
   {
      throw UtException("response_limits must be provided");
   }

   if (mNoiseEquivalentIrradiance <= 0.0F)
   {
      throw UtException("noise_equivalent_irradiance must be provided");
   }

   if ((mDetectionThresholdAboveHorizon <= 0.0F) || (mDetectionThresholdBelowHorizon <= 0.0F))
   {
      throw UtException("detection_threshold must be provided");
   }

   // Determine the wavenumber sample points that totally encompass the range.
   // Note that the sample size is a fixed number of wavenumbers (typically 20 cm-1))

   float lowerWavenumber = 1.0E+4F / mUpperWavelength; // um -> cm-1
   float upperWavenumber = 1.0E+4F / mLowerWavelength; // um -> cm-1

   int iTableStep = static_cast<int>(Increment() + 0.5F);

   int iTableMin = static_cast<int>(lowerWavenumber);
   iTableMin     = ((iTableMin / iTableStep) * iTableStep) + iTableStep;
   while (iTableMin > lowerWavenumber)
   {
      iTableMin -= iTableStep;
   }
   int iTableMax = static_cast<int>(upperWavenumber);
   iTableMax     = ((iTableMax / iTableStep) * iTableStep) - iTableStep;
   while (iTableMax < upperWavenumber)
   {
      iTableMax += iTableStep;
   }

   int count = ((iTableMax - iTableMin + iTableStep - 1) / iTableStep) + 1;
   DefineSpectralLimits(static_cast<float>(iTableMin), static_cast<float>(iTableStep), count);

   // Create the response table as function of wavenumber. If input points were provided then the
   // table is built using the provided points. If no points were provided then the response is
   // assumed to be a constant 1.0

   if (mResponseCurve.size() < 2)
   {
      // Either 0 or 1 response points given. Create a uniform response curve for all wavelengths.
      ResponsePoint rp;
      rp.mResponse   = 1.0F;
      rp.mWavelength = 0.1F; // 0.1 um = 100000 cm-1
      if (!mResponseCurve.empty())
      {
         rp.mResponse = mResponseCurve.front().mResponse;
      }
      mResponseCurve.clear();
      mResponseCurve.push_back(rp);
      rp.mWavelength = 10000.0F; // 10000.0 um = 1 cm-1
      mResponseCurve.push_back(rp);
   }

   // Compute the wavelengths that correspond to the required sample points.

   int                i;
   std::vector<float> wavelengths(count);
   for (i = 0; i < count; ++i)
   {
      wavelengths[i] = 1.0E+4F / (Origin() + (i * Increment()));
   }

   // Compute the fractions of the first and last intervals that participate in the integration.

   int n                  = count - 1;
   mFirstIntervalFraction = (mUpperWavelength - wavelengths[1]) / (wavelengths[0] - wavelengths[1]);
   mLastIntervalFraction  = (mLowerWavelength - wavelengths[n - 1]) / (wavelengths[n] - wavelengths[n - 1]);

   // Set the first and last wavelength equal to actual terminal endpoints defined by the user.

   wavelengths[0] = mUpperWavelength;
   wavelengths[n] = mLowerWavelength;

   // Compute the response for each interval.

   std::sort(mResponseCurve.begin(), mResponseCurve.end());
   int               responseCount = static_cast<int>(mResponseCurve.size());
   TblIndVarU<float> wavelengthTbl(responseCount);
   TblDepVar1<float> responseTbl(responseCount);
   for (i = 0; i < responseCount; ++i)
   {
      wavelengthTbl.Set(mResponseCurve[i].mWavelength, i);
      responseTbl.Set(mResponseCurve[i].mResponse, i);
   }

   int                intervalCount = Count() - 1;
   std::vector<float> response(intervalCount);
   for (i = 0; i < intervalCount; ++i)
   {
      lowerWavenumber       = Origin() + (i * Increment());
      upperWavenumber       = lowerWavenumber + Increment();
      float lowerWavelength = 1.0E+4F / upperWavenumber;
      float upperWavelength = 1.0E+4F / lowerWavenumber;
      response[i]           = ComputeAverageResponse(wavelengthTbl, responseTbl, lowerWavelength, upperWavelength);
   }

   // Eliminate intervals at the front and back that have 0 response as they don't contribute
   // to the results. This may be the result of user error (specifying response_limits
   // that are beyond the bounds of the response curve).

   for (i = 0; i < intervalCount; ++i)
   {
      if (response[i] != 0.0F)
      {
         break;
      }
   }
   int firstInterval = i;
   for (i = (intervalCount - 1); i > firstInterval; --i)
   {
      if (response[i] != 0.0F)
      {
         break;
      }
   }
   int lastInterval = i;

   bool fixedFirstIndex = (firstInterval != 0);
   bool fixedLastIndex  = (lastInterval != (intervalCount - 1));
   if (fixedFirstIndex || fixedLastIndex)
   {
      auto logWarning = ut::log::warning() << "Zero response entries have been eliminated.";
      logWarning.AddNote() << "Sensor Type: " << aSensor.GetTypeName();

      // Rebuild the response table, eliminating the zero entries from the end.

      intervalCount = lastInterval - firstInterval + 1;
      std::vector<float> newResponse(intervalCount);
      for (i = 0; i < intervalCount; ++i)
      {
         newResponse[i] = response[firstInterval + i];
      }
      response = newResponse;

      // Redefine the spectral limits to eliminate the points that resulted in no response.

      count           = intervalCount + 1;
      float newOrigin = Origin() + (firstInterval * Increment());
      DefineSpectralLimits(newOrigin, Increment(), count);

      // Rebuild the wavelength table.

      std::vector<float> newWavelengths(count);
      for (i = 0; i < count; ++i)
      {
         newWavelengths[i] = 1.0E+4F / (Origin() + (i * Increment()));
      }
      if (!fixedFirstIndex)
      {
         newWavelengths[0] = mUpperWavelength;
      }
      if (!fixedLastIndex)
      {
         newWavelengths[count - 1] = mLowerWavelength;
      }
      wavelengths = newWavelengths;

      // If an endpoint has been eliminated, then the fraction has been eliminated.
      if (fixedFirstIndex)
      {
         mFirstIntervalFraction = 1.0F;
      }
      if (fixedLastIndex)
      {
         mLastIntervalFraction = 1.0F;
      }
   }

   // Set the absolute limits of the spectral object (needed by by the constant intensity mode
   // of the simple target model)
   SetWavelengthLimits(wavelengths.back(), wavelengths.front());

   // The algorithm as it currently exists requires the existence of two intervals.
   // (the first and last interval fractions cannot refer to the same interval).
   if (intervalCount < 2)
   {
      throw UtException("response_limits are too narrow");
   }

   // Compute the factors used during spectral integration:
   //
   //               wl(i) - wl(i+1)
   // f(i) = R(i) * ---------------
   //                    2.0

   mIntegFactor.resize(intervalCount);
   for (i = 0; i < intervalCount; ++i)
   {
      mIntegFactor[i] = response[i] * (wavelengths[i] - wavelengths[i + 1]) * 0.5F;
   }

   // Compute the factors used when computing integrated transmittance for 'fast_detection_mode'.
   //
   //        R(i)
   // f(i) = ----
   //         2.0
   //
   // This also computes the 'average' response of the sensor.

   mTransFactor.resize(intervalCount);
   mTransFactor[0]  = response[0] * mFirstIntervalFraction / 2.0F;
   mSumIntervalSize = mFirstIntervalFraction;
   for (i = 1; i < (intervalCount - 1); ++i)
   {
      mTransFactor[i] = response[i] / 2.0F;
      mSumIntervalSize += 1.0F;
   }
   mTransFactor[intervalCount - 1] = response[intervalCount - 1] * mLastIntervalFraction / 2.0F;
   mSumIntervalSize += mLastIntervalFraction;

   double responseSum = 0.0;
   for (i = 0; i < intervalCount; ++i)
   {
      responseSum += mTransFactor[i];
   }
   mAverageResponse = static_cast<float>(2.0 * responseSum / mSumIntervalSize);

   if (mShowResponse)
   {
      ShowResponse(aSensor, response);
   }
}

// =================================================================================================
// virtual
bool SOSM_Sensor::SharedData::ProcessInput(UtInput& aInput, SOSM_Sensor& aSensor)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "response_limits")
   {
      double lowerWavelength;
      double upperWavelength;
      aInput.ReadValueOfType(lowerWavelength, UtInput::cLENGTH);
      aInput.ReadValueOfType(upperWavelength, UtInput::cLENGTH);
      aInput.ValueGreater(lowerWavelength, 0.0);
      aInput.ValueGreater(upperWavelength, lowerWavelength);
      mLowerWavelength = static_cast<float>(lowerWavelength * 1.0E+6); // m -> um
      mUpperWavelength = static_cast<float>(upperWavelength * 1.0E+6); // m -> um
   }
   else if ((command == "response_curve") || (command == "response_points"))
   {
      mResponseCurve.clear(); // Clear any existing definition
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         if (command == "wavelength")
         {
            double wavelength;
            double response;
            aInput.ReadValueOfType(wavelength, UtInput::cLENGTH);
            aInput.ValueGreater(wavelength, 0.0);
            aInput.ReadValue(response);
            aInput.ValueInClosedRange(response, 0.0, 1.0);

            ResponsePoint point;
            point.mWavelength                        = static_cast<float>(wavelength * 1.0E6);
            point.mResponse                          = static_cast<float>(response);
            std::vector<ResponsePoint>::iterator rpi = std::find(mResponseCurve.begin(), mResponseCurve.end(), point);
            if (rpi == mResponseCurve.end())
            {
               mResponseCurve.push_back(point);
            }
            else
            {
               (*rpi) = point;
            }
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   else if ((command == "nei") || (command == "NEI") || (command == "noise_equivalent_irradiance"))
   {
      std::string units;
      aInput.ReadValue(mNoiseEquivalentIrradiance);
      aInput.ValueGreater(mNoiseEquivalentIrradiance, 0.0F);
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
         mNoiseEquivalentIrradiance *= static_cast<float>(multiplier);
      }
      else
      {
         throw UtInput::BadValue(aInput, "Unknown irradiance units " + units);
      }
      mNoiseEquivalentIrradiance *= 1.0E-4F; // W/m^2 -> W/cm^2
   }
   else if (command == "detection_threshold")
   {
      aInput.ReadValueOfType(mDetectionThresholdAboveHorizon, UtInput::cRATIO);
      aInput.ValueGreater(mDetectionThresholdAboveHorizon, 0.0F);
      mDetectionThresholdBelowHorizon = mDetectionThresholdAboveHorizon;
   }
   else if (command == "detection_threshold_above_horizon")
   {
      aInput.ReadValueOfType(mDetectionThresholdAboveHorizon, UtInput::cRATIO);
      aInput.ValueGreater(mDetectionThresholdAboveHorizon, 0.0F);
   }
   else if (command == "detection_threshold_below_horizon")
   {
      aInput.ReadValueOfType(mDetectionThresholdBelowHorizon, UtInput::cRATIO);
      aInput.ValueGreater(mDetectionThresholdBelowHorizon, 0.0F);
   }
   else if (command == "installation_adjustment_table")
   {
      std::string fileName;
      aInput.ReadValueQuoted(fileName);
      fileName = aInput.LocateFile(fileName);
      SOSM_TableVar dvInfo("factor", 0.0F, 1.0F, 1.0F);
      mInstallationAdjustmentTablePtr =
         SOSM_ScalarTable2D::LoadSharedPolarTable(aSensor.GetManager(), fileName, aSensor.GetCachePrefix(), dvInfo);
   }
   else if (command == "atmosphere")
   {
      aInput.ReadValue(mAtmosphereType);
   }
   else if (command == "resolution")
   {
      double resolution    = ReadWavenumber(aInput);
      int    intResolution = static_cast<int>(resolution + 1.0E-3);
      if (fabs(resolution - intResolution) > 1.0E-3)
      {
         throw UtInput::BadValue(aInput, "resolution must be an integer");
      }
      aInput.ValueGreater(intResolution, 0);
      DefineSpectralLimits(0.0F, static_cast<float>(intResolution), 1);
   }
   else if (command == "show_response")
   {
      mShowResponse = true;
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
float SOSM_Sensor::SharedData::ReadWavenumber(UtInput& aInput)
{
   double wavelength;
   double value;
   aInput.ReadValue(value);
   aInput.ValueGreater(value, 0.0);
   std::string units;
   aInput.ReadValue(units);
   size_t unitsLength = units.length();
   if ((unitsLength > 2) && // 1/cm
       (units.substr(0, 2) == "1/"))
   {
      wavelength = UtInput::ConvertValueFrom(1.0 / value, units.substr(2), UtInput::cLENGTH);
   }
   else if ((unitsLength > 4) && // per-cm
            (units.substr(0, 4) == "per-"))
   {
      wavelength = UtInput::ConvertValueFrom(1.0 / value, units.substr(4), UtInput::cLENGTH);
   }
   else if ((unitsLength > 2) && // cm-1
            (units.substr(unitsLength - 2, 2) == "-1"))
   {
      wavelength = UtInput::ConvertValueFrom(1.0 / value, units.substr(0, unitsLength - 2), UtInput::cLENGTH);
   }
   else
   {
      // Value is being expressed as a wavelength.
      wavelength = UtInput::ConvertValueFrom(value, units, UtInput::cLENGTH);
   }
   // Convert wavelength (in meters) to centimeters and take reciprocal to get wavenumber (cm-1).
   float wavenumber = static_cast<float>(1.0 / (wavelength * 1.0E+2));
   return wavenumber;
}

// =================================================================================================
void SOSM_Sensor::SharedData::ShowResponse(SOSM_Sensor& aSensor, const std::vector<float>& aResponse)
{
   auto out = ut::log::info() << "Response data for Sensor Type.";
   out.AddNote() << "Sensor Type: " << aSensor.GetTypeName();
   { // RAII block
      auto note = out.AddNote() << "Sensor response limits:";
      note.AddNote() << "Lower wavelength: " << mLowerWavelength << " um";
      note.AddNote() << " (" << 1.0E+4F / mLowerWavelength << " cm-1)";
      note.AddNote() << "Upper wavelength: " << mUpperWavelength << " um";
      note.AddNote() << " (" << 1.0E+4F / mUpperWavelength << " cm-1)";
   }
   if (!mResponseCurve.empty())
   {
      auto curveNote = out.AddNote() << "Input response curve:";
      for (auto rpi : mResponseCurve)
      {
         float wavelength = rpi.mWavelength;
         auto  note       = curveNote.AddNote() << "Wavelength: " << wavelength << " um";

         float wavenumber = 1.0E+4F / wavelength;
         note.AddNote() << "Wavenumber: " << wavenumber << " cm^-1";
         note.AddNote() << "Response: " << rpi.mResponse;
      }
   }

   auto responseNote = out.AddNote() << "Generated response:";
   for (int index = 0; index < Count() - 1; ++index)
   {
      auto note = responseNote.AddNote() << "Index: " << index;

      float lowerWavenumber = Origin() + (index * Increment());
      float upperWavenumber = lowerWavenumber + Increment();

      float lowerWavelength = 1.0E+4F / upperWavenumber;
      float upperWavelength = 1.0E+4F / lowerWavenumber;

      note.AddNote() << "Wavenumber: " << std::setprecision(0) << lowerWavenumber << " - " << upperWavenumber << " cm^-1";
      note.AddNote() << "Wavelength: " << std::setprecision(6) << lowerWavelength << " - " << upperWavelength << " um";
      float response = aResponse[index];
      if (index == 0)
      {
         note.AddNote() << "Response: " << std::setprecision(6) << response << " (" << mFirstIntervalFraction << ")";
      }
      else if (index == (Count() - 2))
      {
         note.AddNote() << "Response: " << std::setprecision(6) << response << " (" << mLastIntervalFraction << ")";
      }
      else
      {
         note.AddNote() << "Response: " << std::setprecision(6) << response;
      }
   }
   responseNote.AddNote() << "Average Response: " << mAverageResponse;
}
