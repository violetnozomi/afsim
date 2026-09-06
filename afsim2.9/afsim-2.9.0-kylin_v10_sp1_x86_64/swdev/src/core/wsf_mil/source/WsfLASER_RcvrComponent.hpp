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

#ifndef WSFLASER_RCVRCOMPONENT_HPP
#define WSFLASER_RCVRCOMPONENT_HPP

#include "WsfEM_Rcvr.hpp"
#include "WsfMilComponentRoles.hpp"

//! A LASER receiver component.
//! This component provides LASER or narrowband optical receiver attributes
//! to be used with a WsfEM_Rcvr.  It also computes detection data
//! useful in S/N calculations for LADAR and LASER comm.
class WsfLASER_RcvrComponent : public WsfEM_RcvrComponent
{
public:
   WsfLASER_RcvrComponent();
   WsfLASER_RcvrComponent(const WsfLASER_RcvrComponent& aSrc) = default;
   WsfLASER_RcvrComponent& operator=(const WsfLASER_RcvrComponent&) = delete;

   //! @name WsfComponent required methods.
   //@{
   WsfStringId   GetComponentName() const override;
   WsfComponent* CloneComponent() const override;
   const int*    GetComponentRoles() const override;
   void*         QueryInterface(int aRole) override;
   //@}

   //! @name Framework methods.
   //@{
   bool Initialize(double aSimTime) override;
   bool ProcessInput(UtInput& aInput) override;
   //!@}

   double GetAntennaGain(double aXmtr_Wavelength) const;

   //! Return the instantaneous field-of-view of this receiver.
   double GetInstantaneousFieldOfView() const { return mIFOV; }

   //! Return the diameter of the receiver (detector) aperture.
   double GetApertureDiameter() const { return mApertureDiameter; }

   //! Return the solid angle subtended by the aperture at the detector.
   double GetApertureSolidAngle() const { return mApertureSolidAngle; }

   //! Return the aperture area
   double GetApertureArea() const { return mApertureArea; }

   //! Return the fraction of the transmitted laser light that passes through the receiver optics.
   double GetOpticsTransmissionFactor() const { return mOpticsTransmissionFactor; }

   //! Return the responsivity of the detector (amps of current produced per watt of incident radiation [wavelength dependent]).
   double GetResponsivity() const { return mResponsivity; }

   //! A structure used to return data from the ComputeDetectionData method.
   //! These data are used to compute signal-to-noise, and to output various diagnostic data.
   class DetectionData
   {
   public:
      DetectionData() = default;

      double mPower{0.0};       //!< Signal power at detector, optical loss included.
      double mEnergy{0.0};      //!< Integrated signal power at detector, optical loss included.
      double mSignalCount{0.0}; //!< Number of signal photons converted to electrons (and possibly amplified).
      double mEnvironmentalNoiseRate{0.0}; //!< Rate of background photons converted to electrons (and possibly amplified).
      double mEnvironmentalNoiseCount{0.0}; //!< Number of background photons converted to electrons (and possibly amplified).
      double mDarkNoiseRate{0.0};           //!< Rate of background electrons due to dark noise.
      double mDarkNoiseCount{0.0};          //!< Number of background electrons due to dark noise.
      double mSignalShotNoiseCount{0.0}; //!< Noise count due to variations in 'signal' (source + environmental background).
      double mThermalNoiseCount{0.0}; //!< Number of thermal electrons.
      double mDetectorNoiseCount{0.0}; //!< Total noise count due to detector noise alone (i.e., not from environmental noise).
      double mNoiseCount{0.0};         //!< Total noise from all sources.
   };

   //! Given an incident signal and noise radiance and irradiance values, compute receiver detection data
   //! consistent with conversion of incident photons to electrons (current) by a standard (PIN), or avalanche, photodiode.
   //! @param aSignalRadiance The incident signal radiance (laser energy).
   //! @param aSignalIrradiance The incident signal irradiance (laser energy).
   //! @param aBackgroundRadiance The incident background radiance.
   //! @param aBackgroundIrradiance The incident background irradiance.
   //! @param aIntegrationTime The integration time over which the signal and background are accumulated in the detector.
   virtual DetectionData ComputeDetectionData(double aSignalRadiance,       // W/m^2/sr
                                              double aSignalIrradiance,     // W/m^2
                                              double aBackgroundRadiance,   // W/m^2/m/sr
                                              double aBackgroundIrradiance, // W/m^2/m
                                              double aIntegrationTime);

protected:
   double mApertureDiameter;         //!< Defines the diameter of the receiver (detector) aperture.
   double mApertureSolidAngle;       //!< The solid angle subtended by the aperture at the detector.
   double mApertureArea;             //!< The area of the aperture (assumed circular).
   double mOpticsTransmissionFactor; //!< Defines the fraction of the received laser laser light that passes through the
                                     //!< receiver optics.
   double mFocalLength;              //!< Optionally used to compute the aperture solid angle.
   double mDetectorSize;             //!< The length or width of the physical detector
   double mPhotonEnergy;             //!< The photon energy at the center wavelength of the detector.
   double mIFOV;              //!< The instantaneous field of view (FOV of one detector element not always applicable).
   double mQuantumEfficiency; //!< The fraction of electrons that are produced for each incident photon.
   double mResponsivity; //!< Computed from quantum efficiency, this is the number of amps produced per incident watt of power.
   double mDetectorGain;       //!< Gain of the photodetector.
   double mCircuitTemperature; //!< The temperature of the integrating circuit; used to compute thermal noise.
   double mCircuitCapacitance; //!< Circuit capacitance of the detector integrating circuit, sometimes provided in APD spec sheets.
   double mCircuitResistance;   //!< Circuit resistance of the detector integrating circuit.
   double mThermalNoiseCountSq; //!< If computed from circuit capacitance, this is the square of the thermal noise count.
   double mBulkDarkCountRate;   //!< The "bulk" or amplified dark count rate.
   double mBandPass; //!< Assuming an optical filter, this defines the range of wavelengths about the center that are detected.
   double mSurfaceDarkCountRate; //!< The un-amplified dark count rate.
   double mExcessNoiseFactor; //!< Used with APD, this is the additional gain observed when measuring noise in an APD.
   double mNoiseEquivalentPower;       //!< The incident power that would be needed to produce a S/N of one.
   double mNoiseEquivalentCountRate;   //!< The equivalent count rate with the noise equivalent power.
   double mPointingTransmissionFactor; //!< Applicable only when spot size is nearly diffraction-limited.
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfLASER_RcvrComponent, cWSF_COMPONENT_LASER_RCVR)

#endif
