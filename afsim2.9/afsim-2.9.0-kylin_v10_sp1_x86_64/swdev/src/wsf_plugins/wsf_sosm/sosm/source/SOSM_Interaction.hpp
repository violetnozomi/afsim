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

#ifndef SOSM_INTERACTION_HPP
#define SOSM_INTERACTION_HPP

#include "sosm_export.h"

#include <cstddef> // to get size_t

#include "UtLog.hpp"

class SOSM_Sensor;
class SOSM_Target;
class UtAtmosphere;

//! An abstract interface for acquiring dynamic sensor/target data.
//!
//! This defines the interfaces by which SOSM communicates with the calling simulation.
//! In particular, it defines:
//!
//! - The methods used by the sensor, target and atmosphere objects to access the states of
//! sensor and target objects maintained within the calling simulation (position, orientation,
//! speed, etc.) that are needed to perform the detection calculations.
//!
//! - Data members that are passed back to the calling simulation which contain the values of
//! intermediate calculations that may be of interest to the calling simulation.
//!
//! This interface is abstract to allow the calling simulation to utilize any existing data that
//! it may be maintaining, thus potentially eliminating the need for recalculating the data that
//! is exposed by this class. Methods that are declared abstract represent the minimal set that
//! MUST be provided. All others are optional.
//!
//! The class SOSM_SimpleInteraction provides a minimal implementation for those simulations
//! who wish SOSM to compute the data.

class SOSM_EXPORT SOSM_Interaction
{
public:
   SOSM_Interaction();

   virtual ~SOSM_Interaction() {}

   //! Return the slant range between the sensor and the target.
   //! The slant range between the sensor and the target (meters)
   virtual float GetSlantRange() = 0;

   //! Return the altitude of the sensor.
   //! @returns The altitude of the sensor (meters/second).
   virtual float GetSensorAltitude() = 0;

   // Return the speed of the sensor.
   // @returns The speed of the sensor (meters/second).
   float GetSensorSpeed() { return 0.0F; }

   //! Return the altitude of the target.
   //! @return The altitude of the target (meters).
   virtual float GetTargetAltitude() = 0;

   //! Return the speed of the target.
   //! @returns The speed of the target (meters/second).
   virtual float GetTargetSpeed() = 0;

   //! Return the 'throttle setting' of the target.
   //! @returns The throttle setting of the target in the range [0..1].
   //! @note The following values should be used as indicated:
   //! - 0.0 = no power applied.
   //! - 1.0 = afterburner activated.
   virtual float GetTargetThrottle() { return 0.5F; }

   //! Return the elevation of the target with respect to the horizontal plane at the sensor location.
   //!
   //! This method should returns the elevation angle of the target location with respect to a plane
   //! that is tangent to the Earth's surface at the sensor location. It is NOT a function of the
   //! orientation of either the sensor or the target.
   //!
   //! @returns elevation angle in radians (+ up, - down)
   virtual float GetAbsoluteTargetElevation() = 0;

   //! Return the aspect of the target with respect to the sensor.
   //!
   //! This method should return the azimuth and elevation of the target platform with respect to
   //! the location AND orientation of the sensor platform (NOT the installed sensor angle).
   //!
   //! @param aAzimuth   [output] The azimuth angle (radians). Positive values are to the RIGHT with
   //!                            respect to the 'pilot' on the sensor platform.
   //! @param aElevation [output] The elevation angle (radians). Positive values are UP with respect
   //!                            to the 'pilot' on the sensor platform.
   virtual void GetSensorToTargetAspect(float& aAzimuth, float& aElevation) = 0;

   //! Return the aspect of the sensor with respect to the target.
   //!
   //! This method should return the azimuth and elevation of the sensor platform with respect to
   //! the location AND orientation of the target. This is used for target signature determination.
   //!
   //! @param aAzimuth   [output] The azimuth angle (radians). Positive values are to the RIGHT with
   //!                            respect to the 'pilot' on the target platform.
   //! @param aElevation [output] The elevation angle (radians). Positive values are UP with respect
   //!                            to the 'pilot' on the target platform.
   virtual void GetTargetToSensorAspect(float& aAzimuth, float& aElevation) = 0;

   //! Print the interaction data to standard output (for debugging).
   virtual void Print(ut::log::MessageStream& aMsgStream,
                      const UtAtmosphere&     aAtmosphere,
                      SOSM_Sensor*            aSensorPtr,
                      SOSM_Target*            aTargetPtr);

   // The target state index corresponding to the conditions.
   // This is set by SOSM_Sensor/SOSM_SensorTarget prior to calling the target routines.
   size_t mTargetStateIndex;

   //! @name computed values.
   //! The following values are computed during calls to SOSM_Sensor::ComputeTargetIrradiance() and
   //! SOSM_Sensor::ComputeProbabilityOfDetection. They are provided so calling applications can have
   //! more insight into the information that went into the computations.
   //@{

   //! The projected area of the target structure (cm^2)
   float mBodyArea;

   //! The radiant intensity of the target body (W/sr)
   float mBodyIntensity;

   //! The projected area of the target plume (cm^2)
   float mPlumeArea;

   //! The radiant intensity of the target plume (W/sr)
   float mPlumeIntensity;

   //! The transmittance of the path to the target (unit-less, in the range [0..1])
   float mTransmittance;

   //! The radiance of the background path at the observer(W/cm^2/sr)
   float mBackgroundRadiance;

   //! The radiance of the foreground path at the observer (W/cm^2/sr)
   float mForegroundRadiance;

   //! The contrast radiant intensity at the observer (W/sr)
   float mContrastIntensity;

   //! The 'Installation Adjustment' in the range [0..1].
   float mInstallationAdjustment;

   //! The noise equivalent irradiance (NEI) (W/cm^2).
   float mNoiseEquivalentIrradiance;

   //! The detection threshold.
   float mDetectionThreshold;

   //! A multiplier on the calculated transmittance
   float mTransmittanceScaleFactor;

   //! A multiplier on the calculated foreground
   float mForegroundScaleFactor;

   //! A multiplier on the calculated background
   float mBackgroundScaleFactor;
   //@}
};

#endif
