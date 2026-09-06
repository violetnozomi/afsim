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

#ifndef WSFEOIR_IRST_SENSORMODE_HPP
#define WSFEOIR_IRST_SENSORMODE_HPP

#include "wsf_mil_export.h"

#include <vector>

#include "UtAtmosphere.hpp"
#include "WsfEM_Antenna.hpp"
class WsfEM_Attenuation;
#include "WsfEM_Rcvr.hpp"
#include "WsfOpticalEnvironment.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSensorResult.hpp"

//! A base class for the common processing of WsfEOIR_Sensor and WsfIRST_Sensor.
//!
//! WsfEOIR_Sensor and WsfIRST_Sensor employ the same physical detection methodologies.
//! Formerly each duplicated the associated code, but now it has be merged into a common base.

class WSF_MIL_EXPORT WsfEOIR_IRST_SensorMode : public WsfSensorMode
{
public:
   // Redefine a few things to to simplify coding.
   using Result   = WsfSensorResult;
   using Settings = WsfSensor::Settings;
   using BandList = WsfOpticalEnvironment::BandList;

   WsfEOIR_IRST_SensorMode(WsfScenario& aScenario);
   WsfEOIR_IRST_SensorMode(const WsfEOIR_IRST_SensorMode& aSrc);
   ~WsfEOIR_IRST_SensorMode() override = default;
   WsfEOIR_IRST_SensorMode& operator=(const WsfEOIR_IRST_SensorMode&) = delete;

   bool Initialize(double aSimTime) override;

   bool ProcessInput(UtInput& aInput) override;

   void AttemptToDetectInfrared(double aSimTime, WsfPlatform* aTargetPtr, Settings& aSettings, Result& aResult);

   void AttemptToDetectVisual(double aSimTime, WsfPlatform* aTargetPtr, Settings& aSettings, Result& aResult);

   bool BeginDetectionAttempt(double aSimTime, WsfPlatform* aTargetPtr, Settings& aSettings, Result& aResult);

   bool EndDetectionAttempt(double aSimTime, WsfPlatform* aTargetPtr, Settings& aSettings, Result& aResult);

   WsfEM_Antenna& GetAntenna() { return mAntenna; }
   WsfEM_Rcvr&    GetReceiver() { return mRcvr; }

protected:
   double ComputeInstallationEffects(Result& aResult);

   double ComputeProbabilityOfDetection(double aSignal, double aNoise, double aThreshold);

   //! Declare if 'band visual' is the default, or if an entry is required.
   //! WsfEOIR_Sensor defaults to using visual mode while WsfIRST_Sensor requires and band.
   void SetVisualBandDefault(bool aVisualBand)
   {
      mInfraredBand            = WsfEM_Types::cIRBAND_COUNT;
      mOpticalBand             = aVisualBand ? WsfEM_Types::cOPT_VISUAL : WsfEM_Types::cOPT_IR_COUNT;
      mUseSimpleVisualDetector = true;
   }

   WsfEM_Antenna         mAntenna;
   WsfEM_Rcvr            mRcvr;
   WsfOpticalEnvironment mEnvironment; //!< Common optical environment model
   //! The list of all bands for multi-band detection. If multiple bands are specified, mOpticalBand and
   //! mInfraredBand are both still valid, but only for the first band specified (for compatibility)
   BandList                  mOpticalBands;
   WsfEM_Types::OpticalBand  mOpticalBand;             //!< Optical spectrum band (visual, or one of the IR bands)
   WsfEM_Types::InfraredBand mInfraredBand;            //!< Band for infrared spectrum (only valid if IR band selected)
   bool                      mDetectNegativeContrast;  //!< true if negative contrast is the same as positive contrast
   bool                      mUseSimpleVisualDetector; //!< If true, use the simple legacy visual detector
   double                    mDetectorGain;
   double                    mIntegrationGain;
   double                    mNoiseEquivalentIrradiance; //! Noise Equivalent Irradiance (NEI) (W/m^2)
   double                    mDetectionThreshold;        //! Signal-To-Noise required for detection

   //! @name Deprecated input for the old simple attenuation model
   //@{
   double       mAtmosphericAttenuation; //! Attenuation (1/m)
   UtAtmosphere mAtmosphere;             //! Atmosphere model
   //@}

   //! 'true' if the old (deprecated) method of computing installation effects should be used.
   //! This will be set to 'true' if the receiver has an antenna_pattern. The preferred method
   //! is to use a 'masking_pattern'.
   bool mComputeOldInstallationEffects;
};

#endif
