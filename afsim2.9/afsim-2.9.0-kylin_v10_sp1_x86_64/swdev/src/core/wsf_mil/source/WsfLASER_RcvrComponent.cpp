// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

// References:
// 1) "Direct-Detection LADAR Systems"; SPIE Press Tutorial Text in Optical Engineering,
//    Volume TT85; Richard D. Richmond and Stephen C. Cain.
// 2) "Laser Communications in Space", Stephen G. Lambert & William L. Casey, Artech House, 1995.
// 3) "Predicting the Performance of Linear Optical Detectors in Free Space Laser Communication Links",
//    Thomas C Farrell, Air Force Research Laboratory, Space Vehicles Directorate 3550 Aberdeen Ave., SE,
//    Kirtland AFB, NM 87117-5776; Unpublished.

#include "WsfLASER_RcvrComponent.hpp"

#include <math.h>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtStringIdLiteral.hpp"
#include "WsfEM_Antenna.hpp"

// =================================================================================================
// Definition for the nested class WsfLASER_RcvrComponent
// =================================================================================================

namespace
{
// Returns the diffraction limited FOV angle in radians.
double ComputeDiffractionLimitedFOV_Angle(double aWavelength, double aAperture)
{
   // Equation per Michael Cates.
   return 1.03 * aWavelength / aAperture;
}
} // namespace

// =================================================================================================
WsfLASER_RcvrComponent::WsfLASER_RcvrComponent()
   : WsfEM_RcvrComponent()
   , mApertureDiameter(0.0)
   , mApertureSolidAngle(0.0)
   , mApertureArea(0.0)
   , mOpticsTransmissionFactor(1.0)
   , mFocalLength(0.0)
   , mDetectorSize(0.0)
   , mPhotonEnergy(0.0)
   , mIFOV(0.0)
   , mQuantumEfficiency(1.0)
   , mResponsivity(0.0)
   , mDetectorGain(1.0)
   , mCircuitTemperature(300.0)
   , mCircuitCapacitance(0.0)
   , mCircuitResistance(0.0)
   , mThermalNoiseCountSq(0.0)
   , mBulkDarkCountRate(0.0)
   , mBandPass(1.0e-9)
   , mSurfaceDarkCountRate(0.0)
   , mExcessNoiseFactor(1.0)
   , mNoiseEquivalentPower(0.0)
   , mNoiseEquivalentCountRate(0.0)
   , mPointingTransmissionFactor(1.0)
{
}

// =================================================================================================
WsfStringId WsfLASER_RcvrComponent::GetComponentName() const
{
   return UtStringIdLiteral("em_component_laser_rcvr");
}

// =================================================================================================
WsfComponent* WsfLASER_RcvrComponent::CloneComponent() const
{
   return new WsfLASER_RcvrComponent(*this);
}

// =================================================================================================
const int* WsfLASER_RcvrComponent::GetComponentRoles() const
{
   static const int roles[] = {cWSF_COMPONENT_LASER_RCVR, 0};
   return roles;
}

// =================================================================================================
void* WsfLASER_RcvrComponent::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_LASER_RCVR)
   {
      return this;
   }
   return nullptr;
}

// =================================================================================================
// virtual
bool WsfLASER_RcvrComponent::Initialize(double aSimTime)
{
   bool ok = WsfEM_RcvrComponent::Initialize(aSimTime);
   if (ok)
   {
      // Compute thermal noise according to Ref. 1, equation 1.26
      if ((mCircuitTemperature > 0.0) && (mCircuitCapacitance > 0.0))
      {
         double e             = UtMath::cELEMENTARY_CHARGE; // Absolute value of the charge of electron/proton
         double k             = UtMath::cBOLTZMANN_CONSTANT;
         mThermalNoiseCountSq = (k * mCircuitTemperature * mCircuitCapacitance) / (e * e);
      }

      // Compute the count rate assoiciated with the noise equivalent power.
      if (mNoiseEquivalentPower > 0.0)
      {
         mNoiseEquivalentCountRate =
            mNoiseEquivalentPower * mResponsivity / UtMath::cELEMENTARY_CHARGE; // W * ((C/s)/W) * (1/C) -> 1/s
      }

      if (mApertureDiameter == 0.0)
      {
         ut::log::error() << "'aperture_diameter' must be specified.";
         ok = false;
      }

      WsfEM_Rcvr& rcvr = *GetComponentParent();

      if (mResponsivity > 0.0)
      {
         // Compute quantum efficiency from input responsivity value.
         mQuantumEfficiency = UtMath::cPLANCK_CONSTANT * UtMath::cLIGHT_SPEED * mResponsivity / rcvr.GetWavelength() /
                              UtMath::cELEMENTARY_CHARGE;

         // Make sure this is <= 1
         if (mQuantumEfficiency > 1.0)
         {
            auto out = ut::log::error() << "Receiver responsivity too high.";
            out.AddNote() << "Actual: " << mResponsivity << " amp/watt";
            out.AddNote() << "Expected: resp < "
                          << rcvr.GetWavelength() / UtMath::cPLANCK_CONSTANT / UtMath::cLIGHT_SPEED * UtMath::cELEMENTARY_CHARGE
                          << " amp/watt";
            ok = false;
         }
      }
      else
      {
         // compute responsivity from the quantum efficiency value.
         mResponsivity = mQuantumEfficiency / UtMath::cPLANCK_CONSTANT / UtMath::cLIGHT_SPEED * rcvr.GetWavelength() *
                         UtMath::cELEMENTARY_CHARGE;
      }

      double wavelength = GetComponentParent()->GetWavelength();
      ok &= (wavelength > 0.0); // Error message will be output by the rcvr.

      if (ok)
      {
         if (GetComponentParent()->GetBandwidth() == 0.0)
         {
            // WsfEM_Rcvr uses the bandwidth value to determine whether transmiitters and receivers can interact.
            // For optical systems we are using the concept of the bandpass filter.  We must translate the bandpass
            // (wavelength) into 'bandwidth' (frequency) in order for the framework (WsfEM_Rcvr) to correctly make
            // interaction determinations.
            double halfBandpass = mBandPass / 2.0;
            double bandwidth =
               UtMath::cLIGHT_SPEED * (1.0 / (wavelength - halfBandpass) - 1.0 / (wavelength + halfBandpass));
            GetComponentParent()->SetBandwidth(bandwidth);
         }

         mPhotonEnergy = (UtMath::cPLANCK_CONSTANT * UtMath::cLIGHT_SPEED) / rcvr.GetWavelength();

         //! If the focal length and detector size of the receiver are specified, compute the instantaneous field-
         //! of-view (IFOV; the FOV of the smallest discrenable feature)
         if ((mFocalLength > 0.0) && (mDetectorSize > 0.0))
         {
            mIFOV = pow(mDetectorSize / mFocalLength, 2);
         }
         else
         {
            double fovAngle = ComputeDiffractionLimitedFOV_Angle(GetComponentParent()->GetWavelength(), mApertureDiameter);
            mIFOV           = fovAngle * fovAngle;
         }

         // Compute the aperture area.
         mApertureArea = UtMath::cPI * pow(mApertureDiameter / 2.0, 2);

         // Compute the aperture solid angle according to a standard small-angle approximation.
         // If a focal length was specified, it will be preferentially used; otherwise the standard FOV
         // from the antenna is used.
         if (mFocalLength > 0.0)
         {
            double fov          = 2.0 * atan2(mApertureDiameter, 2.0 * mFocalLength);
            mApertureSolidAngle = UtMath::cPI_OVER_4 * fov * fov; // sr
         }
         else
         {
            double minAz;
            double maxAz;
            double minEl;
            double maxEl;
            GetComponentParent()->GetAntenna()->GetAzimuthFieldOfView(minAz, maxAz);
            GetComponentParent()->GetAntenna()->GetElevationFieldOfView(minEl, maxEl);
            mApertureSolidAngle = UtMath::cPI_OVER_4 * (maxAz - minAz) *
                                  (maxEl - minEl); // Small angle approximation to compute solid angle.
         }
      }
   }

   return ok;
}

// =================================================================================================
// virtual
bool WsfLASER_RcvrComponent::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "aperture_diameter")
   {
      aInput.ReadValueOfType(mApertureDiameter, UtInput::cLENGTH);
      aInput.ValueGreater(mApertureDiameter, 0.0);
   }
   else if (command == "optics_transmission_factor")
   {
      aInput.ReadValue(mOpticsTransmissionFactor);
      aInput.ValueGreater(mOpticsTransmissionFactor, 0.0);
      aInput.ValueLessOrEqual(mOpticsTransmissionFactor, 1.0);
   }
   else if (command == "optics_loss") // Lasercom convention
   {
      aInput.ReadValueOfType(mOpticsTransmissionFactor, UtInput::cRATIO);
      aInput.ValueGreater(mOpticsTransmissionFactor, 0.0);
      aInput.ValueLessOrEqual(mOpticsTransmissionFactor, 1.0);
   }
   else if (command == "focal_length")
   {
      aInput.ReadValueOfType(mFocalLength, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(mFocalLength, 0.0); // Value of 0 disables computation
   }
   else if (command == "detector_size")
   {
      aInput.ReadValueOfType(mDetectorSize, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(mDetectorSize, 0.0); // Value of 0 disables computation
   }
   else if (command == "quantum_efficiency")
   {
      aInput.ReadValue(mQuantumEfficiency);
      aInput.ValueGreater(mQuantumEfficiency, 0.0);
      aInput.ValueLessOrEqual(mQuantumEfficiency, 1.0);
   }
   else if (command == "responsivity")
   {
      aInput.ReadValueOfType(mResponsivity, UtInput::cRESPONSIVITY);
      aInput.ValueGreater(mResponsivity, 0.0);
   }
   else if (command == "detector_gain")
   {
      aInput.ReadValue(mDetectorGain);
      aInput.ValueGreater(mDetectorGain, 0.0);
   }
   else if (command == "circuit_temperature")
   {
      aInput.ReadValueOfType(mCircuitTemperature, UtInput::cTEMPERATURE);
      aInput.ValueGreaterOrEqual(mCircuitTemperature, 0.0); // Value of 0 disables computation
   }
   else if (command == "circuit_capacitance")
   {
      aInput.ReadValueOfType(mCircuitCapacitance, UtInput::cCAPACITANCE);
      aInput.ValueGreaterOrEqual(mCircuitCapacitance, 0.0); // Value of 0 disables computation
   }
   else if (command == "circuit_resistance")
   {
      aInput.ReadValueOfType(mCircuitCapacitance, UtInput::cRESISTANCE);
      aInput.ValueGreaterOrEqual(mCircuitCapacitance, 0.0); // Value of 0 disables computation
   }
   else if (command == "dark_count_rate")
   {
      aInput.ReadValueOfType(mBulkDarkCountRate, UtInput::cFREQUENCY);
      aInput.ValueGreater(mBulkDarkCountRate, 0.0);
   }
   else if (command == "dark_current")
   {
      double darkCurrent;
      aInput.ReadValueOfType(darkCurrent, UtInput::cCURRENT);
      aInput.ValueGreater(darkCurrent, 0.0);
      mBulkDarkCountRate = darkCurrent / UtMath::cELEMENTARY_CHARGE;
   }
   else if (command == "bandpass")
   {
      aInput.ReadValueOfType(mBandPass, UtInput::cLENGTH);
   }
   else if (command == "noise_equivalent_power")
   {
      aInput.ReadValueOfType(mNoiseEquivalentPower, UtInput::cPOWER);
      aInput.ValueGreater(mNoiseEquivalentPower, 0.0);
   }
   else if (command == "excess_noise_factor")
   {
      aInput.ReadValue(mExcessNoiseFactor);
      aInput.ValueGreater(mExcessNoiseFactor, 1.0);
   }
   else if (command == "pointing_transmission_factor")
   {
      aInput.ReadValue(mPointingTransmissionFactor);
      aInput.ValueInClosedRange(mPointingTransmissionFactor, 0.0, 1.0);
   }
   else if (command == "surface_dark_count_rate")
   {
      aInput.ReadValueOfType(mSurfaceDarkCountRate, UtInput::cFREQUENCY);
      aInput.ValueGreaterOrEqual(mSurfaceDarkCountRate, 0.0);
   }
   else if (command == "surface_dark_current")
   {
      double surfaceDarkCurrent;
      aInput.ReadValueOfType(surfaceDarkCurrent, UtInput::cCURRENT);
      aInput.ValueGreater(surfaceDarkCurrent, 0.0);
      mBulkDarkCountRate = surfaceDarkCurrent / UtMath::cELEMENTARY_CHARGE;
   }
   else if (command == "pointing_loss")
   {
      aInput.ReadValueOfType(mPointingTransmissionFactor, UtInput::cRATIO);
      aInput.ValueInClosedRange(mPointingTransmissionFactor, 0.0, 1.0);
   }
   else
   {
      myCommand = WsfEM_RcvrComponent::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
//! Compute the "antenna gain" of the transmitter (see documentation or (1) eq. 3.80).
//!@param aXmtr_Wavelength The wavelength of the transmitter's laser.
double WsfLASER_RcvrComponent::GetAntennaGain(double aXmtr_Wavelength) const
{
   return pow(UtMath::cPI * mApertureDiameter / aXmtr_Wavelength, 2.0);
}

//! virtual
WsfLASER_RcvrComponent::DetectionData WsfLASER_RcvrComponent::ComputeDetectionData(double aSignalRadiance,
                                                                                   double aSignalIrradiance,
                                                                                   double aBackgroundSpectralRadiance,
                                                                                   double aBackgroundSpectralIrradiance,
                                                                                   double aIntegrationTime)
{
   DetectionData dd;

   // Compute the power at the front of the receiver, accounting for pointing error:
   dd.mPower = (aSignalIrradiance + aSignalRadiance * mApertureSolidAngle) * mApertureArea; // W/m^2 * m^2 -> W

   // Propagate through the receive optics
   dd.mPower *= mOpticsTransmissionFactor; // W

   // Convert to energy
   dd.mEnergy = dd.mPower * aIntegrationTime; // W * s => J

   // Account for other factors.
   dd.mEnergy *= mQuantumEfficiency; // J

   // Compute the number of signal photoelectrons.
   // The energy of a single photon is (h * c) / lambda.
   dd.mSignalCount = dd.mEnergy / mPhotonEnergy; // electrons, before amplification

   double backgroundRadiance =
      (aBackgroundSpectralRadiance * mApertureSolidAngle + aBackgroundSpectralIrradiance) * mBandPass; // W/m^2

   // Compute the power at the front of the receiver
   double backgroundPower = backgroundRadiance * mApertureArea; // W

   // Propagate through the receive optics
   backgroundPower *= mOpticsTransmissionFactor; // W

   // Account for other factors.
   backgroundPower *= mQuantumEfficiency; // J

   // Convert to energy
   double backgroundEnergy = backgroundPower * aIntegrationTime; // W * s => J

   // Compute the number of photoelectrons.
   // The energy of a single photon is (h * c) / lambda.
   dd.mEnvironmentalNoiseCount = backgroundEnergy / mPhotonEnergy;
   dd.mEnvironmentalNoiseRate  = backgroundPower / mPhotonEnergy; // used for LADAR diagnostic output

   double thermalNoiseCountSq = mThermalNoiseCountSq;

   // Compute the thermal noise if not precomputed from circuit capacitance.
   // The thermal noise squared is the variance.
   if ((mThermalNoiseCountSq == 0.0) && (mCircuitTemperature > 0.0) && (mCircuitResistance > 0.0))
   {
      // Using the standard formula for thermal noise: 4kTB/R, assuming an effective circuit integration bandwidth of
      // 1/(2*t), where t is the slot time (aIntegrationTime).  See (1), eqns. 13-15.
      thermalNoiseCountSq = 2.0 * UtMath::cBOLTZMANN_CONSTANT * mCircuitTemperature / mCircuitResistance /
                            pow(UtMath::cELEMENTARY_CHARGE, 2) * aIntegrationTime;
   }

   if (thermalNoiseCountSq > 0.0)
   {
      dd.mThermalNoiseCount = sqrt(thermalNoiseCountSq);
   }

   // Compute the signal and noise counts (Ref. 1; see documentation)
   double signalShotNoiseVariance = (dd.mSignalCount + dd.mEnvironmentalNoiseCount) * mDetectorGain * mExcessNoiseFactor;

   dd.mSignalShotNoiseCount = sqrt(signalShotNoiseVariance);              // used in LADAR diagnostic output
   dd.mDarkNoiseRate        = mBulkDarkCountRate + mSurfaceDarkCountRate; // used in LADAR diagnostic output
   dd.mDarkNoiseCount       = dd.mDarkNoiseRate * aIntegrationTime;       // used in LADAR diagnostic output

   if (mNoiseEquivalentCountRate == 0.0)
   {
      double darkCurrentNoiseVariance =
         (mSurfaceDarkCountRate + mDetectorGain * mExcessNoiseFactor * mBulkDarkCountRate) * aIntegrationTime;
      dd.mNoiseCount = sqrt(thermalNoiseCountSq + signalShotNoiseVariance + darkCurrentNoiseVariance);
   }
   else
   {
      double noiseCountVariance = mNoiseEquivalentCountRate * aIntegrationTime;
      dd.mNoiseCount            = sqrt(signalShotNoiseVariance + noiseCountVariance);
   }

   // Account for signal amplification
   dd.mSignalCount = mDetectorGain * dd.mSignalCount;

   return dd;
}
