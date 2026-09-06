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

#include "SOSM_SimpleInteraction.hpp"

#include <algorithm>
#include <cmath>

#include "UtEarth.hpp"
#include "UtEntity.hpp"
#include "UtMath.hpp"
#include "UtVec3.hpp"

// =================================================================================================
SOSM_SimpleInteraction::SOSM_SimpleInteraction()
   : mSlantRange(0.0F)
   , mSensorAltitude(0.0F)
   , mSensorSpeed(0.0F)
   , mTargetAltitude(0.0F)
   , mTargetSpeed(0.0F)
   , mTargetThrottle(0.5F)
   , mAbsoluteTargetElevation(0.0F)
   , mSensorToTargetAzimuth(0.0F)
   , mSensorToTargetElevation(0.0F)
   , mTargetToSensorAzimuth(0.0F)
   , mTargetToSensorElevation(0.0F)
{
}

// =================================================================================================
//! Compute geometry using the supplied slant range, altitudes, and target orientation.
//!
//! @param aSlantRange     The slant range to the target (meters).
//! @param aSensorAltitude The sensor altitude (meters).
//! @param aTargetAltitude The target altitude (meters).
//! @param aTargetHeading  The heading of the target (radians from north).
//! @param aTargetPitch    The pitch angle of the target (radians, + nose up).
//! @param aTargetRoll     The roll angle of the target (radians, + right wing down).
void SOSM_SimpleInteraction::ComputeGeometry(float aSlantRange,
                                             float aSensorAltitude,
                                             float aTargetAltitude,
                                             float aTargetHeading,
                                             float aTargetPitch,
                                             float aTargetRoll)
{
   mSlantRange     = aSlantRange;
   mSensorAltitude = aSensorAltitude;
   mTargetAltitude = aTargetAltitude;

   // Use the law of cosines on the spherical Earth to get the elevation angle above the horizontal plane.
   // See NOTE below as to use why we use the semi-major axis of the ellipse rather than the spherical earth

   double re                = UtEarth::cA; // Not UtSphericalEarth::cEARTH_RADIUS (See NOTE below).
   double rs                = re + mSensorAltitude;
   double rt                = re + mTargetAltitude;
   double rst               = mSlantRange;
   double cosTheta          = ((rs * rs) - (rt * rt) + (rst * rst)) / (2.0 * rs * rst);
   double theta             = acos(std::min(std::max(cosTheta, -1.0), 1.0));
   mAbsoluteTargetElevation = static_cast<float>(theta - UtMath::cPI_OVER_2);

   // Use the law of cosines again to get the separation angle between the position vectors, which is
   // just the longitude east of the prime meridian (sensor is at 0 lat, 0 lon).
   //
   // NOTE:
   //
   // East was chosen rather than north because there is no eccentricity in the equatorial plane.
   // The heading will be adjusted so the user *THINKS* we went north (so a default heading of zero
   // works as expected).

   cosTheta = ((rs * rs) + (rt * rt) - (rst * rst)) / (2.0 * rs * rt);
   theta    = acos(std::min(std::max(cosTheta, -1.0), 1.0));

   double sensorLat     = 0.0;
   double sensorLon     = 0.0;
   double targetLat     = 0.0;
   double targetLon     = theta * UtMath::cDEG_PER_RAD;
   double targetHeading = UtMath::NormalizeAngleMinusPi_Pi(aTargetHeading + UtMath::cPI_OVER_2);

   UtEntity sensor;
   UtEntity target;
   sensor.SetLocationLLA(sensorLat, sensorLon, mSensorAltitude);
   target.SetLocationLLA(targetLat, targetLon, mTargetAltitude);
   target.SetOrientationNED(targetHeading, aTargetPitch, aTargetRoll);

   // Get the ECS position of the sensor in the target frame and use it to compute the aspect angles.

   double tgtToSnrLocECS[3];
   target.GetRelativeLocationECS(&sensor, tgtToSnrLocECS);
   double tgtToSnrAz;
   double tgtToSnrEl;
   target.ComputeAzimuthAndElevation(tgtToSnrLocECS, tgtToSnrAz, tgtToSnrEl);
   mTargetToSensorAzimuth   = static_cast<float>(tgtToSnrAz);
   mTargetToSensorElevation = static_cast<float>(tgtToSnrEl);
}

// =================================================================================================
//! Compute geometry using the supplied positions and orientations.
//!
//! @param aSensorLatitude  The latitude of the sensor (degrees)
//! @param aSensorLongitude The longitude of the sensor (degrees)
//! @param aSensorAltitude  The altitude of the sensor (meters).
//! @param aSensorHeading   The heading of the sensor (radians from north).
//! @param aSensorPitch     The pitch angle of the sensor (radians, + nose up).
//! @param aSensorRoll      The roll angle of the sensor (radians, + right wing down).
//! @param aTargetLatitude  The latitude of the target (degrees)
//! @param aTargetLongitude The longitude of the target (degrees)
//! @param aTargetAltitude  The altitude of the target (meters).
//! @param aTargetHeading   The heading of the target (radians from north).
//! @param aTargetPitch     The pitch angle of the target (radians, + nose up).
//! @param aTargetRoll      The roll angle of the target (radians, + right wing down).
//! @note An ellipsoidal Earth is assumed.
void SOSM_SimpleInteraction::ComputeGeometry(double aSensorLatitude,
                                             double aSensorLongitude,
                                             float  aSensorAltitude,
                                             float  aSensorHeading,
                                             float  aSensorPitch,
                                             float  aSensorRoll,
                                             double aTargetLatitude,
                                             double aTargetLongitude,
                                             float  aTargetAltitude,
                                             float  aTargetHeading,
                                             float  aTargetPitch,
                                             float  aTargetRoll)
{
   mSensorAltitude = aSensorAltitude;
   mTargetAltitude = aTargetAltitude;

   UtEntity sensor;
   UtEntity target;

   sensor.SetLocationLLA(aSensorLatitude, aSensorLongitude, aSensorAltitude);
   sensor.SetOrientationNED(aSensorHeading, aSensorPitch, aSensorRoll);
   target.SetLocationLLA(aTargetLatitude, aTargetLongitude, aTargetAltitude);
   target.SetOrientationNED(aTargetHeading, aTargetPitch, aTargetRoll);

   // Get the NED position of the target in the sensor horizontal plane and use it to
   // compute the absolute target elevation angle and the slant range to the target.

   double snrToTgtLocNED[3];
   sensor.GetRelativeLocationNED(&target, snrToTgtLocNED);
   mSlantRange = static_cast<float>(UtVec3d::Magnitude(snrToTgtLocNED));
   double absTgtAz;
   double absTgtEl;
   UtEntity::ComputeAzimuthAndElevation(snrToTgtLocNED, absTgtAz, absTgtEl);
   mAbsoluteTargetElevation = static_cast<float>(absTgtEl);

   // Compute the aspect of the target platform with respect to the sensing platform

   double snrToTgtLocECS[3];
   sensor.GetRelativeLocationECS(&target, snrToTgtLocECS);
   double snrToTgtAz;
   double snrToTgtEl;
   sensor.ComputeAzimuthAndElevation(snrToTgtLocECS, snrToTgtAz, snrToTgtEl);
   mSensorToTargetAzimuth   = static_cast<float>(snrToTgtAz);
   mSensorToTargetElevation = static_cast<float>(snrToTgtEl);

   // Compute the aspect of the sensing platform with respect to the target platform.

   double tgtToSnrLocECS[3];
   target.GetRelativeLocationECS(&sensor, tgtToSnrLocECS);
   double tgtToSnrAz;
   double tgtToSnrEl;
   target.ComputeAzimuthAndElevation(tgtToSnrLocECS, tgtToSnrAz, tgtToSnrEl);
   mTargetToSensorAzimuth   = static_cast<float>(tgtToSnrAz);
   mTargetToSensorElevation = static_cast<float>(tgtToSnrEl);
}
