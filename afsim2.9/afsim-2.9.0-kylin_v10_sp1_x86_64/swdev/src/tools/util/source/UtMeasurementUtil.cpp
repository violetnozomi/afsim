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

#include "UtMeasurementUtil.hpp"

#include <algorithm>

#include "UtEllipsoidalEarth.hpp"
#include "UtEntity.hpp"
#include "UtMath.hpp"
#include "UtMeasurementData.hpp"
#include "UtSphericalEarth.hpp"
#include "UtVec3.hpp"

namespace
{
const double cLOCATION_SIGMA_CONSTANT = 1000.0; // meters
}

//! Return the range, bearing, and elevation of the measurement's location, given an NED location
//! @param aLocationNED A NED vector from the coordinate origin to the target's location.
//! @param aRange The range to a measurement's location.
//! @param aBearing The bearing to a measurement's location.
//! @param aElevation The elevation to a measurement's location.
void UtMeasurementUtil::RangeBearingElevation(const double aLocationNED[3], double& aRange, double& aBearing, double& aElevation)
{
   // Bearing
   aBearing = atan2(aLocationNED[1], aLocationNED[0]);

   // Elevation
   double xy  = sqrt(aLocationNED[0] * aLocationNED[0] + aLocationNED[1] * aLocationNED[1]);
   aElevation = atan2(-aLocationNED[2], xy);

   // Range
   aRange = UtVec3d::Magnitude(aLocationNED);
}

//! Given two WCS locations, return the range, bearing, and elevation,
//! corrected for a spherical earth.
//! @ Note this method should be faster than creating a new UtEntity object.
void UtMeasurementUtil::RangeBearingElevation(const double aOriginLocationWCS[3],
                                              const double aDestinationLocationWCS[3],
                                              double&      aRange,
                                              double&      aBearing,
                                              double&      aElevation)
{
   double locNED[3];
   LocationNED(aOriginLocationWCS, aDestinationLocationWCS, locNED);
   aRange     = UtVec3d::Magnitude(locNED);
   aBearing   = 0.0; // not defined for range zero.
   aElevation = 0.0; // not defined for range zero.
   if (aRange > 0.0)
   {
      aBearing   = atan2(locNED[1], locNED[0]);
      aElevation = -asin(locNED[2] / aRange);
   }
}

//! Given two WCS locations, return the relative NED location,
//! corrected for an ellipsoidal earth.
//! @ Note this method should be faster than creating a new UtEntity object.
void UtMeasurementUtil::LocationNED(const double aOriginLocationWCS[3],
                                    const double aDestinationLocationWCS[3],
                                    double       aLocationNED[3])
{
   double lat;
   double lon;
   double alt;
   UtEllipsoidalEarth::ConvertECEFToLLA(aOriginLocationWCS, lat, lon, alt);
   double WCSTo_NEDTransform[3][3];
   double refLocWCS[3];
   UtEllipsoidalEarth::ComputeNEDTransform(lat, lon, alt, WCSTo_NEDTransform, refLocWCS);
   UtEllipsoidalEarth::ConvertECEFToLocal(aOriginLocationWCS, WCSTo_NEDTransform, aDestinationLocationWCS, aLocationNED);
}

//! Provide an estimate of the elevation, corrected for the curvature of the earth,
//! given an origin location, range, bearing, and target altitude.
//! Given a valid range and bearing, one can compute an estimated 3-D location
//! as in the following example:
//! /code
//!     // Have a valid measurement with originatorLocWCS, range, and bearing, and target altitude estimate.
//!     double elevation = WsfUtil::GetElevation(originatorLocWCS, range, altitude);
//!     UtMeasurementUtil::LocationWCS(originatorLocWCS, range, bearing, elevation, locationWCS);
//! /endcode
//! @return The estimated elevation, or 0 if there are insufficient data.
double UtMeasurementUtil::Elevation(const double aOriginLocationWCS[3], const double& aRange, const double& aAltitude)
{
   // Solve for the angle between the WCS vectors of sensor and target.
   double sensorRadius = UtVec3d::Magnitude(aOriginLocationWCS);

   // Now, need to estimate the earth radius at the target lat/lon
   // Assume the target is close (range << earth radius).
   // Then it should be good enough to use the sensor's lat/lon.
   // Note: It is not sufficient to assume a spherical earth for this calculation.
   double sensorLat;
   double sensorLon;
   double sensorAlt;
   UtEllipsoidalEarth::ConvertECEFToLLA(aOriginLocationWCS, sensorLat, sensorLon, sensorAlt);

   // Convert back to WCS for zero altitude;
   // This gives the earth radius at the lat/lon of the sensor.
   double sensorSurfaceLocationWCS[3];
   UtEllipsoidalEarth::ConvertLLAToECEF(sensorLat, sensorLon, 0.0, sensorSurfaceLocationWCS);

   // Now we have the estimated target radius from the zero altitude sensor location,
   // plus the altitude of the target.
   double targetRadius = UtVec3d::Magnitude(sensorSurfaceLocationWCS) + aAltitude;

   // Use the law of cosines on a round-earth to approximate the elevation angle.
   double cosA = ((sensorRadius * sensorRadius) - (targetRadius * targetRadius) + (aRange * aRange)) /
                 (2.0 * sensorRadius * aRange);
   cosA = std::min(std::max(cosA, -1.0), 1.0);
   // double elevation = acos(cosA) - UtMath::cPI_OVER_2;
   return acos(cosA) - UtMath::cPI_OVER_2;
}


//! Return the WCS location, given a origin location, range, bearing, and elevation.
//! @param aOriginLocationWCS A WCS location from which the range, elevation, and bearing are measured.
//! @param aRange The range to a measurement's location.
//! @param aBearing The bearing to a measurement's location.
//! @param aElevation The elevation to a measurement's location.
//! @param aLocationWCS The WCS location of a target.
void UtMeasurementUtil::LocationWCS(const double  aOriginLocationWCS[3],
                                    const double& aRange,
                                    const double& aBearing,
                                    const double& aElevation,
                                    double        aLocationWCS[3])
{
   double sinBearing = sin(aBearing);
   double cosBearing = cos(aBearing);
   double lat;
   double lon;
   double alt;
   UtEllipsoidalEarth::ConvertECEFToLLA(aOriginLocationWCS, lat, lon, alt);
   double transECEF[3][3];
   double refLocWCS[3];
   UtEllipsoidalEarth::ComputeNEDTransform(lat, lon, alt, transECEF, refLocWCS);
   if (aElevation != 0.0)
   {
      double sinElevation = sin(aElevation);
      double cosElevation = cos(aElevation);
      double myLocNED[3] = {aRange * cosBearing * cosElevation, aRange * sinBearing * cosElevation, -aRange * sinElevation};
      UtEllipsoidalEarth::ConvertLocalToECEF(aOriginLocationWCS, transECEF, myLocNED, aLocationWCS);
   }
   else
   {
      double myLocNED[3] = {aRange * cosBearing, aRange * sinBearing, 0.0};
      UtEllipsoidalEarth::ConvertLocalToECEF(aOriginLocationWCS, transECEF, myLocNED, aLocationWCS);
   }
}

//! Compute the quantitative measurement quality for a given measurement.  This method does not currently support
//! the sensor platform's location error.  The given measurement must have a valid covariance matrix.
//! @param aMeasurement The measurement used to compute the quantitative measurement quality.
//! @return the normalized quantitative measurement quality, in the range 0.0-1.0 (not 0-15).  If
//! the measurement quality cannot be computed, zero is returned.
double UtMeasurementUtil::GetQuantitativeMeasurementQuality(double aSimTime, const UtMeasurementData& aMeasurement)
{
   double quality = 0.0;

   // must have a covariance
   // Note: Use the zero check instead of if (aMeasurement.StateCovarianceValid()) because
   // we may be in the middle of setting covariance and validity
   // todo-mjm Move this into UtCovariance as a factory method.
   if (aMeasurement.GetStateCovariance() != nullptr)
   {
      // Calculate the "cap" (circular area of probability)
      // We compute the "varTp" as the mean of the x-y-z diagonal components:
      const UtMatrixd& covar = *aMeasurement.GetStateCovariance();
      double           speed = 0.0;

      // We do not require valid velocity, but this is preferred.
      if (aMeasurement.VelocityValid())
      {
         double velWCS[3];
         aMeasurement.GetVelocityWCS(velWCS);
         speed = UtVec3d::Magnitude(velWCS);
      }

      quality = GetQuantitativeMeasurementQuality(aSimTime, aMeasurement.GetUpdateTime(), covar, speed);
   }
   return quality;
}

//! Compute the quantitative measurement quality for a given measurement.  This method does not currently support
//! the sensor platform's location error.  The given measurement must have a valid covariance matrix.
//! @param aSimTime The time at which the covariance matrix is valid (not necessarily the current simulation time).
//! @param aCovar The covariance that is used to create the measurement quality.
//! @param aSpeed The current estimated speed of the measured object (optional).
//! @return the normalized quantitative measurement quality, in the range 0.0-1.0.
double UtMeasurementUtil::GetQuantitativeMeasurementQuality(double                  aSimTime,
                                                            double                  aMeasurementUpdateTime,
                                                            const UtMatrix<double>& aCovar,
                                                            double                  aSpeed /*= 0.0*/,
                                                            bool                    aNormalizeIt /*= true*/)
{
   double rmsLocationSigma = sqrt(aCovar(0, 0) + aCovar(1, 1) + aCovar(2, 2));
   if (aCovar.GetSize() == 36) // includes velocity dispersion
   {
      double dt = aSimTime - aMeasurementUpdateTime;
      if (dt > 0.0)
      {
         double rmsVelocitySigma = sqrt(aCovar(3, 3) + aCovar(4, 4) + aCovar(5, 5));
         rmsLocationSigma += rmsVelocitySigma * dt;
      }
   }

   double quality = rmsLocationSigma;
   if (aNormalizeIt)
   {
      // The TQ algorithm used is a decaying exponential:
      // TQ = 1 when rmsLocationSigma == 0;
      // TQ = 1/e (~.37) when rmsLocationSigma == cLOCATION_SIGMA_CONSTANT
      quality = exp(-rmsLocationSigma / cLOCATION_SIGMA_CONSTANT);
   }

   return quality;
}

//! Given a measurement quality, compute the average expected location error.
//! This method is the inverse of GetQuantitativeMeasurementQuality in that it provides the
//! RMS location sigma, given a measurement quality.
//! @param aMeasurementQuality A measurement quality indicator in the closed range of 0-1
//! @return the RMS location sigma representative of the provided measurement quality.
double UtMeasurementUtil::GetExpectedLocationError(double aMeasurementQuality)
{
   double rmsLocationSigma = 1.0E6;
   if (aMeasurementQuality > 0.0)
   {
      rmsLocationSigma = -cLOCATION_SIGMA_CONSTANT * log(aMeasurementQuality);
   }
   return rmsLocationSigma;
}

//! Compute the approximate ground range between two WCS locations.
double UtMeasurementUtil::GroundRange(const double aPoint1LocWCS[3], const double aPoint2LocWCS[3])
{
   // convert points to spherical
   double sphericalPoint1_LocWCS[3];
   double sphericalPoint2_LocWCS[3];
   UtEllipsoidalEarth::ConvertEllipsoidalToSpherical(aPoint1LocWCS, sphericalPoint1_LocWCS);
   UtEllipsoidalEarth::ConvertEllipsoidalToSpherical(aPoint2LocWCS, sphericalPoint2_LocWCS);
   // Use the definition of the dot product to get the angle between the WCS locations.
   double cosTheta = UtVec3d::DotProduct(sphericalPoint1_LocWCS, sphericalPoint2_LocWCS) /
                     (UtVec3d::Magnitude(sphericalPoint1_LocWCS) * UtVec3d::Magnitude(sphericalPoint2_LocWCS));
   cosTheta     = std::max(-1.0, std::min(cosTheta, 1.0));
   double theta = acos(cosTheta);
   // Arc length (ground range) = earth radius * theta
   return UtSphericalEarth::cEARTH_RADIUS * theta;
}

double UtMeasurementUtil::SlantRange(const double aPoint1LocWCS[3], const double aPoint2LocWCS[3])
{
   double sphericalPoint1_LocWCS[3];
   double sphericalPoint2_LocWCS[3];
   double sphericalRel_WCS[3];

   // convert points to spherical
   UtEllipsoidalEarth::ConvertEllipsoidalToSpherical(aPoint1LocWCS, sphericalPoint1_LocWCS);
   UtEllipsoidalEarth::ConvertEllipsoidalToSpherical(aPoint2LocWCS, sphericalPoint2_LocWCS);

   // get relative vector
   UtVec3d::Subtract(sphericalRel_WCS, sphericalPoint1_LocWCS, sphericalPoint2_LocWCS);

   return UtVec3d::Magnitude(sphericalRel_WCS);
}
