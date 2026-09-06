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

#include "WsfUtil.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>
#include <memory>

#include "UtAngle.hpp"
#include "UtEarth.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtEntity.hpp"
#include "UtInputBlock.hpp"
#include "UtLatPos.hpp"
#include "UtLog.hpp"
#include "UtLonPos.hpp"
#include "UtMat3.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfTerrain.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackId.hpp"

//! Count the number of bits on in a bit mask
//! @param aBitMask The bit mask to be tested.
//! @returns The number of bits on in the bit mask.
int WsfUtil::CountBitsOn(unsigned int aBitMask)
{
   int numberOfBitsOn = 0;
   while (aBitMask != 0)
   {
      numberOfBitsOn += (aBitMask & 1);
      aBitMask = aBitMask >> 1;
   }
   return numberOfBitsOn;
}

//! Compute the horizontal point of closest approach of a moving object to a given point.
//! Given the location and velocity of a moving object and determine the horizontal point of closest approach
//! to a given target location. This algorithm only solves the problem in the horizontal plane.
//!
//! @param aMoverLocWCS   [input]  The WCS location of the moving object.
//! @param aMoverVelWCS   [input]  The WCS velocity of the moving object.
//! @param aTargetLocWCS  [input]  The WCS location of the target.
//! @param aClosestLocWCS [output] The WCS location of the moving object at its point of closest approach.
//!
//! @note If the velocity of the mover is zero or if the points are coincident then location of the mover
//! will be returned.
void WsfUtil::ClosestApproachPoint2D(const double aMoverLocWCS[3],
                                     const double aMoverVelWCS[3],
                                     const double aTargetLocWCS[3],
                                     double       aClosestLocWCS[3])
{
   // Create the moving entity and clamp it to the ground.

   UtEntity mover;
   mover.SetLocationWCS(aMoverLocWCS);
   mover.SetVelocityWCS(aMoverVelWCS);
   double moverLat;
   double moverLon;
   double moverAlt;
   mover.GetLocationLLA(moverLat, moverLon, moverAlt);
   mover.SetLocationLLA(moverLat, moverLon, 0.0);
   double moverVelNED[3];
   mover.GetVelocityNED(moverVelNED);
   moverVelNED[2] = 0.0;

   // Get the NED vector from the mover to the target.

   double targetLat;
   double targetLon;
   double targetAlt;
   UtEntity::ConvertWCSToLLA(aTargetLocWCS, targetLat, targetLon, targetAlt);
   double targetLocNED[3];
   mover.ConvertLLAToNED(targetLat, targetLon, 0.0, targetLocNED);

   double range = UtVec3d::Normalize(targetLocNED);
   double speed = UtVec3d::Normalize(moverVelNED);

   // Assume that the target is either not moving (and therefore won't get any closer) or it is
   // coincident with the observer. If either is true then we'll just return the current target
   // location as the closest approach point.

   double pcaLat = moverLat;
   double pcaLon = moverLon;
   double pcaAlt = moverAlt;
   if ((range > 1.0E-5) && (speed > 1.0E-5))
   {
      double pcaLocNED[3];

      // Use the dot product to get the angle between the vector from the observer-to-target and the
      // targets horizontal velocity vector. Note that the two vectors have already been normalized.
      double cosTheta = UtVec3d::DotProduct(targetLocNED, moverVelNED);

      // Project the range vector on to the target velocity vector to get the range vector
      // to the point of closest approach.
      UtVec3d::Multiply(pcaLocNED, moverVelNED, cosTheta * range);

      // Get the equivalent spherical coordinates.
      mover.ConvertNEDToLLA(pcaLocNED, pcaLat, pcaLon, pcaAlt);

      // The initial pass assumed the target velocity was tangent to the surface. If the range is large
      // (100km or greater) then the resulting endpoint has a significant altitude (1000's m).  Adjusting
      // the velocity vector so it points to the ground at the proposed intercept point and then recomputing
      // the intercept point gives a much better answer. At 200km it only took one additional iteration
      // and this gives a much better approximation of a great circle path.

      int iter = 1;
      while ((pcaAlt > 100.0) && (iter < 5))
      {
         ++iter;
         mover.ConvertLLAToNED(pcaLat, pcaLon, 0.0, pcaLocNED);
         UtVec3d::Normalize(moverVelNED, pcaLocNED);
         cosTheta = UtVec3d::DotProduct(targetLocNED, moverVelNED);
         UtVec3d::Multiply(pcaLocNED, moverVelNED, cosTheta * range);
         mover.ConvertNEDToLLA(pcaLocNED, pcaLat, pcaLon, pcaAlt);
      }
   }
   UtEntity::ConvertLLAToWCS(pcaLat, pcaLon, pcaAlt, aClosestLocWCS);
}

//! Compute the point of closest approach of a moving object at a given velocity.
//!
//! @param aMoverLocWCS   [input]  The WCS location of the moving object.
//! @param aMoverVelWCS   [input]  The WCS velocity of the moving object.
//! @param aTargetLocWCS  [input]  The WCS location of the static target.
//! @param aClosestLocWCS [output] The WCS location of the moving object at its point of closest approach.
//! @returns Time in seconds when the closest point is reached.  Returns -1 if the velocity is zero.
//! @note If the velocity of the mover is zero then the location of the mover will be returned.
double WsfUtil::ClosestApproachPoint3D(const double aMoverLocWCS[3],
                                       const double aMoverVelWCS[3],
                                       const double aTargetLocWCS[3],
                                       double       aClosestLocWCS[3])
{
   // Represent the path of the mover with aMoverLocWCS + t * aMoverVelWCS
   // vTargetToMover = aMoverLocWCS + t * aMoverVelWCS - aTargetLocWCS
   // The closest point of approach happens when
   //      DotProduct(vTargetToMover, aMoverVelWCS) = 0

   double t;

   double c = aMoverVelWCS[0] * aMoverVelWCS[0] + aMoverVelWCS[1] * aMoverVelWCS[1] + aMoverVelWCS[2] * aMoverVelWCS[2];

   if (c >= 1.0E-5)
   {
      // Compute t
      double a = aMoverVelWCS[0] * aMoverLocWCS[0] + aMoverVelWCS[1] * aMoverLocWCS[1] + aMoverVelWCS[2] * aMoverLocWCS[2];

      double b =
         -aTargetLocWCS[0] * aMoverVelWCS[0] + -aTargetLocWCS[1] * aMoverVelWCS[1] + -aTargetLocWCS[2] * aMoverVelWCS[2];

      t = (-a - b) / c;

      if (t < 0.0)
      {
         t = 0.0;
      }
      // Extrapolate to time t
      aClosestLocWCS[0] = aMoverLocWCS[0] + aMoverVelWCS[0] * t;
      aClosestLocWCS[1] = aMoverLocWCS[1] + aMoverVelWCS[1] * t;
      aClosestLocWCS[2] = aMoverLocWCS[2] + aMoverVelWCS[2] * t;
   }
   else
   {
      UtVec3d::Set(aClosestLocWCS, aMoverLocWCS);
      t = -1.0;
   }
   return t;
}

//! Perform a very conservative check to see if two objects are within a defined interaction range.
//!
//! This method is called by various 'device' objects (sensors, interferers, etc.) to determine if two platforms are
//! POTENTIALLY within the interaction range of the device. It is basically a fast culling check that is used to
//! prevent more expensive calculations that may be unnecessary.
//!
//! Judicious use of this method can reduce computation time.
//!
//! @param aSimTime      The current simulation time.
//! @param aObject1Ptr   Pointer to the first platform.
//! @param aObject2Ptr   Pointer to the second platform.
//! @param aMaximumRange The declared maximum interaction range.
//! @returns true if there is even a remote possibility that the objects can interact.

bool WsfUtil::PotentiallyWithinRange(double aSimTime, WsfPlatform* aObject1Ptr, WsfPlatform* aObject2Ptr, double aMaximumRange)
{
   double deltaTime;

   // If the first platform position is too old then force an update.

   double object1_Movement = 0.0;
   deltaTime               = aSimTime - aObject1Ptr->GetLastUpdateTime();
   if (deltaTime > 5.0)
   {
      aObject1Ptr->Update(aSimTime);
   }
   else
   {
      double speed     = aObject1Ptr->GetSpeed();
      double accel     = aObject1Ptr->GetAccelerationMagnitude();
      object1_Movement = (speed * deltaTime) + (0.5 * accel * deltaTime * deltaTime);
   }

   // If the second platform position is too old then force an update.

   double object2_Movement = 0.0;
   deltaTime               = aSimTime - aObject2Ptr->GetLastUpdateTime();
   if (deltaTime > 5.0)
   {
      aObject2Ptr->Update(aSimTime);
   }
   else
   {
      double speed     = aObject2Ptr->GetSpeed();
      double accel     = aObject2Ptr->GetAccelerationMagnitude();
      object2_Movement = (speed * deltaTime) + (0.5 * accel * deltaTime * deltaTime);
   }

   // Compute the distance between the platforms as of their last update times.

   double relLocWCS[3];
   aObject1Ptr->GetRelativeLocationWCS(aObject2Ptr, relLocWCS);
   double estimatedRange = UtVec3d::Magnitude(relLocWCS);

   // Assume the worst-case scenario that the objects are moving directly towards each other.
   // Also reduce the range just for an added measure of safety.

   estimatedRange -= (object1_Movement + object2_Movement);
   estimatedRange *= 0.8;
   return (estimatedRange < aMaximumRange);
}

//! Given a WSF track id, compute a unique single integer.
//! @note This algorithm returns unique numbers only for track numbers < 65535.
//!   This method should be used as a convenience for interfacing with software that
//!   expects integer track numbers.
int WsfUtil::TrackIdToInt(const WsfTrackId& aTrackId)
{
   return ((WsfStringId::UnsafeGetNumber(aTrackId.GetOwningPlatformId()) << 16) + aTrackId.GetLocalTrackNumber());
}

//! Given an integer track id, compute a WSF track id.
//!   It is assumed that the argument was initially computed by the WsfUtil::ToInt method;
//!   otherwise the returned track id will not have meaning to an WSF simulation.
//! @note This algorithm returns unique numbers only for track numbers < 65535.
//!   This method should be used as a convenience for interfacing with software that
//!   expects integer track numbers.
WsfTrackId WsfUtil::IntToTrackId(int aIntTrackId)
{
   return WsfTrackId(WsfStringId::UnsafeFindStringId(aIntTrackId >> 16), aIntTrackId & 65535);
}

// static
//! Compute a triangulated position based on two valid azimuth/elevation measurements.
//! @param aOriginWCS_1 The WCS origin of the first measurement.
//! @param aBearing1 The bearing measurement of the target from the first sensor.
//! @param aElevation1 The elevation measurement of the target from the first sensor.
//! @param aOriginWCS_2 The WCS origin of the first measurement.
//! @param aBearing2 The bearing measurement of the target from the first sensor.
//! @param aElevation2 The elevation measurement of the target from the first sensor.
//! @param aLocationWCS The computed triangulated position.
//! TODO The algorithm currently only utilized the second elevation measurement.
//! review the paper to ensure the triangulation is as accurate as possible.
bool WsfUtil::TriangulateLocation(const double  aOriginWCS_1[3],
                                  const double& aBearing1,
                                  const double& aElevation1,
                                  const double  aOriginWCS_2[3],
                                  const double& aBearing2,
                                  const double& aElevation2,
                                  double        aLocationWCS[3])
{
   double sensorVecWCS[3];
   UtVec3d::Subtract(sensorVecWCS, aOriginWCS_2, aOriginWCS_1);
   double sensorSeparationSquared = UtVec3d::MagnitudeSquared(sensorVecWCS);

   if (sensorSeparationSquared < 100000.0)
   {
      return false;
   }

   // Now find the NED location of the 2nd track's origin
   double sensorVecNED[3];
   double lat, lon, alt;
   UtEntity::ConvertWCSToLLA(aOriginWCS_1, lat, lon, alt);
   double WCSTo_NEDTransform[3][3];
   double refLocWCS[3];
   UtEllipsoidalEarth::ComputeNEDTransform(lat, lon, alt, WCSTo_NEDTransform, refLocWCS);
   UtEllipsoidalEarth::ConvertECEFToLocal(aOriginWCS_1, WCSTo_NEDTransform, aOriginWCS_2, sensorVecNED);

   // Compute the NED location of the target (Sanders-Reed, 2002)
   double tanBearing1   = tan(aBearing1);
   double tanBearing2   = tan(aBearing2);
   double tanElevation2 = tan(aElevation2);
   double divisor       = tanBearing1 - tanBearing2;

   if (divisor == 0.0)
   {
      // The measurements probably identical or from the same platform.
      return false;
   }

   double targetVecENU[3] = {0.0, 0.0, 0.0};
   double sensorVecENU[3] = {sensorVecNED[1], sensorVecNED[0], -sensorVecNED[2]};
   targetVecENU[0]        = (sensorVecENU[0] * tanBearing1 - sensorVecENU[1] * tanBearing1 * tanBearing2) / divisor;
   targetVecENU[1]        = (sensorVecENU[0] - sensorVecENU[1] * tanBearing2) / divisor;
   double planeRadius = sqrt(pow((sensorVecENU[0] - targetVecENU[0]), 2) + pow((sensorVecENU[1] - targetVecENU[1]), 2));
   targetVecENU[2]    = planeRadius * tanElevation2 + targetVecENU[2];
   double targetVecNED[3] = {targetVecENU[1], targetVecENU[0], -targetVecENU[2]};

   // Convert back to WCS, using the return value
   UtEllipsoidalEarth::ConvertLocalToECEF(aOriginWCS_1, WCSTo_NEDTransform, targetVecNED, aLocationWCS);
   return true;
}

//! Given a 2x2 covariance matrix (in the form of a std::vector<double>),
//!   compute the semi-major axis, semi-minor axis, and bearing, in radians,
//!   of the associated error ellipse measured clockwise from north (y-axis).
//! @param aMatrix The covariance matrix in std::vector<double> form.
//! @param aSemiMajorAxis The semi-major axis of the error ellipse.
//! @param aSemiMinorAxis The semi-minor axis of the error ellipse.
//! @param aBearingRad The bearing, in radians, of the semi-major axis
//!    measured clockwise from north (y-axis).
//! @note e.g., Ref. http://www.math.harvard.edu/archive/21b_fall_04/exhibits/2dmatrices/index.html
// static
void WsfUtil::CovarianceToEllipse_2x2(std::vector<double>& aMatrix,
                                      double&              aSemiMajorAxis,
                                      double&              aSemiMinorAxis,
                                      double&              aBearingRad)
{
   // matrix is | a  b |
   //           | c  d |
   double a = aMatrix[0];
   double b = aMatrix[1];
   double c = aMatrix[2];
   double d = aMatrix[3];

   double T = a + d;         // trace
   double D = a * d - b * c; // determinant

   double c1     = sqrt(T * T / 4.0 - D);
   double L1     = T / 2.0 + c1; // eigenvalue 1
   double L2     = T / 2.0 - c1; // eigenvalue 2
   double lam[2] = {L1, L2};
   double ev[2][2]; // eigenvectors

   if (fabs(c) > 0.0)
   {
      ev[0][0] = L1 - d;
      ev[0][1] = c;
      ev[1][0] = L2 - d;
      ev[1][1] = c;
   }
   else if (fabs(b) > 0.0)
   {
      ev[0][0] = b;
      ev[0][1] = L1 - a;
      ev[1][0] = b;
      ev[1][1] = L2 - a;
   }
   else
   {
      ev[0][0] = 1.0;
      ev[0][1] = 0.0;
      ev[1][0] = 0.0;
      ev[1][1] = 1.0;
   }

   aSemiMajorAxis                   = sqrt(lam[0]);
   aSemiMinorAxis                   = sqrt(lam[1]);
   double              rotaAng_Rad  = 0.0;
   static const double numTolerance = 1.0e-9;
   if (fabs(ev[1][0]) > numTolerance) // avoid numerical roundoff issues near 0 and 180 deg.
   {
      rotaAng_Rad = atan2(ev[1][1], ev[1][0]) - UtMath::cPI_OVER_2;
   }
   else
   {
      rotaAng_Rad = atan2(ev[0][1], ev[0][0]);
   }
   aBearingRad = UtMath::NormalizeAngleMinusPi_Pi(rotaAng_Rad);
}

//! Given an origin WCS location, range, and bearing, return the range and bearing from the
//! given entity.
void WsfUtil::ConvertRangeBearing(const double aFromLocationWCS[3], UtEntity* aToEntityPtr, double& aRange, double& aBearing)
{
   // Convert the range/bearing from the non-local frame to the local frame.
   double locationNED[3];
   aToEntityPtr->ConvertWCSToNED(aFromLocationWCS, locationNED);
   double rangeBearingNED[3] = {aRange * cos(aBearing), aRange * sin(aBearing), 0.0};
   UtVec3d::Add(locationNED, locationNED, rangeBearingNED);
   aRange   = UtVec3d::Magnitude(locationNED);
   aBearing = atan2(locationNED[1], locationNED[0]);
}

//! Given a range, bearing, elevation, and associated errors, compute the
//! associated NED error.
void WsfUtil::ErrorNED(double aRange,
                       double aBearing,
                       double aElevation,
                       double aRangeError,
                       double aBearingError,
                       double aElevationError,
                       double aErrorNED[3])
{
   double sinB = sin(aBearing);
   double cosB = cos(aBearing);
   double sinE = sin(aElevation);
   double cosE = cos(aElevation);
   double RBE_ToNED[3][3];
   RBE_ToNED[0][0]    = cosB * cosE;
   RBE_ToNED[0][1]    = -aRange * sinB * cosE;
   RBE_ToNED[0][2]    = -aRange * cosB * sinE;
   RBE_ToNED[1][0]    = sinB * cosE;
   RBE_ToNED[1][1]    = aRange * cosB * cosE;
   RBE_ToNED[1][2]    = -aRange * sinB * sinE;
   RBE_ToNED[2][0]    = -sinE;
   RBE_ToNED[2][1]    = 0.0;
   RBE_ToNED[2][2]    = -aRange * cosE;
   double deltaRBE[3] = {aRangeError, aBearingError, aElevationError};
   UtMat3d::Transform(aErrorNED, RBE_ToNED, deltaRBE);
}

//! Given a range and bearing, and associated errors, compute the
//! associated NED error.
//! This version of the method will return a D error of the input range.
void WsfUtil::ErrorNED(double aRange, double aBearing, double aRangeError, double aBearingError, double aErrorNED[3])
{
   double sinB  = sin(aBearing);
   double cosB  = cos(aBearing);
   aErrorNED[0] = -aRange * sinB * aBearingError + cosB * aRangeError;
   aErrorNED[1] = aRange * cosB * aBearingError + sinB * aRangeError;
   aErrorNED[2] = aRange;
}

//! Computes a simple ballistic projection from a starting location and velocity
//! until falling below an altitude.  Does NOT consider drag.
//! @param aLocationWCS Starting location in WCS
//! @param aVelocityWCS Starting velocity in WCS
//! @param aStopAltitude Altitude below which propagation is halted
//! @param aPropagatedLocationWCS Resulting location in WCS
//! @param aPropagatedVelocityWCS Resulting velocity in WCS
//! @return Seconds until reaching the desired altitude
double WsfUtil::BallisticPropagate(const UtVec3d& aLocationWCS,
                                   const UtVec3d& aVelocityWCS,
                                   double         aStopAltitude,
                                   UtVec3d&       aPropagatedLocationWCS,
                                   UtVec3d&       aPropagatedVelocityWCS)
{
   double lat, lon, alt;
   UtEntity::ConvertWCSToLLA(aLocationWCS.GetData(), lat, lon, alt);

   double       groundLevel = aLocationWCS.Magnitude() - alt;
   const double cTIME_STEP  = 1.0;
   double       timeDelta   = 0;

   UtVec3d position(aLocationWCS);
   UtVec3d velocity(aVelocityWCS);

   UtVec3d oldPos(aLocationWCS);
   UtVec3d oldVel(aVelocityWCS);
   double  oldAlt = alt;

   while (alt > aStopAltitude)
   {
      oldPos = position;
      oldVel = velocity;
      position += velocity * cTIME_STEP;
      UtVec3d gravity = (position + oldPos);
      gravity *= -UtEarth::cACCEL_OF_GRAVITY / gravity.Magnitude();
      velocity += gravity * cTIME_STEP;
      oldAlt = alt;
      alt    = position.Magnitude() - groundLevel;
      timeDelta += cTIME_STEP;
   }

   // Interpolate last step
   if (oldAlt > aStopAltitude)
   {
      double t = (oldAlt - aStopAltitude) / (oldAlt - alt);
      position = (t * position + (1 - t) * oldPos);
      velocity = (t * velocity + (1 - t) * oldVel);
      timeDelta -= (1 - t) * cTIME_STEP;
   }

   aPropagatedLocationWCS = position;
   aPropagatedVelocityWCS = velocity;
   return timeDelta;
}

//! Fill a track with truth information on aTarget with aSource as the track source.
void WsfUtil::MakeTrack(double aSimTime, WsfTrack& aTrack, WsfPlatform& aTarget, WsfPlatform& aSource)
{
   aTrack.SetPlatformOriginator(&aSource);
   aTrack.Initialize(aSimTime, aSource.GetNextTrackId(), *aSource.GetSimulation());

   double vecWCS[3];
   aSource.GetLocationWCS(vecWCS);
   aTrack.SetOriginatorLocationWCS(vecWCS);
   aTarget.GetLocationWCS(vecWCS);
   aTrack.SetLocationWCS(vecWCS);
   aTrack.SetWCS_LocationValid(true);
   aTrack.Set3D(true);
   aTarget.GetVelocityWCS(vecWCS);
   if (UtVec3d::MagnitudeSquared(vecWCS) == 0.0)
   {
      // Create a velocity vector that is small that will allow the user to recover any heading.
      // Pitch and roll are assumed to be zero.
      double heading;
      double pitch;
      double roll;
      aTarget.GetOrientationNED(heading, pitch, roll);
      const double speed = 0.001;
      double       velNED[3];
      velNED[0] = speed * cos(heading);
      velNED[1] = speed * sin(heading);
      velNED[2] = 0.0;
      aTarget.ConvertNEDVectorToWCS(vecWCS, velNED);
   }
   aTrack.SetVelocityWCS(vecWCS);
   aTrack.SetVelocityValid(true);
   aTrack.SetSpatialDomain(aTarget.GetSpatialDomain());
   aTrack.SetTypeIdValid(true);
   aTrack.SetTypeId(aTarget.GetTypeId());
   aTrack.SetSideIdValid(true);
   aTrack.SetSideId(aTarget.GetSideId());
   aTrack.SetTargetIndex(aTarget.GetIndex());
   aTrack.SetTargetName(aTarget.GetNameId());
   aTrack.SetTargetType(aTarget.GetTypeId());
}

//! Given a track with valid location and covariance, provide
//! estimates of error in location (lat, lon, alt),
//! as well as speed, and heading of the target.  If the covariance is 3x3, only the location
//! errors are will be valid.  A validity bit-field is passed as a parameter to determine
//! which of the outputs are valid.  The masks for this field are defied in the WsfUtil header:
//!   WsfUtil::cLLA_ERROR_VALID     = 0x01
//!   WsfUtil::cSPEED_ERROR_VALID   = 0x02
//!   WsfUtil::cHEADING_ERROR_VALID = 0x04
//! @param aTrack The track that is the source of the data being evaluated (position,
//! velocity, and covariance).
//! @param aStDevLat The estimated latitude error of the track location, in degreees.
//! @param aValidity The bit-field that indicates the validitity of the return values.
//! @param aStDevLon The estimated longitude error of the track location, in degrees.
//! @param aStDevAlt The estimated altitude error of the track location, in meters.
//! @param aStDevSpeed The estimated error in the track's speed
//! (magnitude of velocity estimate), in meters per second.
//! @param aStDevHeading The estimated error of the track's heading (arctan of east velocity
//! component divided by north velocity component), in degrees.
//! @return true for a 6x6 covariance matrix, false for a 3x3 or unrecognized size matrix.
void WsfUtil::GetErrorEstimateLLA_SpeedHeading(const WsfTrack& aTrack,
                                               unsigned short& aValidity,
                                               double&         aStDevLat,
                                               double&         aStDevLon,
                                               double&         aStDevAlt,
                                               double&         aStDevSpeed,
                                               double&         aStDevHeading)
{
   bool valid = (aTrack.StateCovarianceValid() && aTrack.LocationValid());
   aValidity  = 0;
   if (valid)
   {
      double       locationWCS[3];
      UtCovariance nedCovar;
      aTrack.GetLocationWCS(locationWCS);
      nedCovar.ComputeNED_FromWCS(*aTrack.GetStateCovariance(), locationWCS);
      aStDevLat = sqrt(nedCovar(0, 0));
      aStDevLon = sqrt(nedCovar(1, 1));
      aStDevAlt = sqrt(nedCovar(2, 2));

      // Have to convert lat, lon to degrees
      double lat, lon, alt;
      aTrack.GetLocationLLA(lat, lon, alt);

      double cosLat = cos(lat * UtMath::cRAD_PER_DEG);
      aStDevLat *= UtMath::cDEG_PER_RAD / UtSphericalEarth::cEARTH_RADIUS;
      aStDevLon *= UtMath::cDEG_PER_RAD * cosLat / UtSphericalEarth::cEARTH_RADIUS;
      aValidity = cLLA_ERROR_VALID;

      valid = aTrack.VelocityValid() &&
              (aTrack.GetStateCovariance()->GetSize() == 36); // Contains data for both location and speed.

      if (valid)
      {
         aStDevSpeed = sqrt(nedCovar(3, 3) + nedCovar(4, 4) + nedCovar(5, 5));
         aValidity |= cSPEED_ERROR_VALID;

         // Now for the heading error, this is a chain-rule based derivation.
         double velNED[3];
         double WCS_ToNED_Transform[3][3];
         UtEllipsoidalEarth::ComputeNEDTransform(lat, lon, alt, WCS_ToNED_Transform, locationWCS);
         double velWCS[3];
         aTrack.GetVelocityWCS(velWCS);

         // Rotate the vector in the WCS to the NED frame
         UtMat3d::Transform(velNED, WCS_ToNED_Transform, velWCS);

         // Todo document the algorithm to compute the heading error.
         if ((velNED[0] != 0.0) || (velNED[1] != 0.0))
         {
            double commonTerm = 1.0 / (velNED[1] * velNED[1] + velNED[0] * velNED[0]);
            double term1      = pow(commonTerm * velNED[1], 2) * nedCovar(4, 4);
            double term2      = pow(commonTerm * velNED[0], 2) * nedCovar(5, 5);
            aStDevHeading     = sqrt(term1 + term2);
            aValidity |= cHEADING_ERROR_VALID;
         }
      }
   }
}

//! Calls PrintLLA() for the LLA representation of aLocationWCS
void WsfUtil::PrintLLA(const std::string& aLabel, double aLocationWCS[3])
{
   double lat, lon, alt;
   UtEntity::ConvertWCSToLLA(aLocationWCS, lat, lon, alt);
   PrintLLA(aLabel, lat, lon, alt);
}

//! Prints out lat,lon,alt in both DMS and D formats as an info message.
//! Used for debug output.
void WsfUtil::PrintLLA(const std::string& aLabel, double aLat, double aLon, double aAlt)
{
   auto out = ut::log::info() << aLabel;
   out.AddNote() << "Lat Deg: " << UtLatPos(aLat, UtAngle::FmtD + 4);
   out.AddNote() << "Lon Deg: " << UtLonPos(aLon, UtAngle::FmtD + 4);
   out.AddNote() << "Lat: " << UtLatPos(aLat, UtAngle::FmtDMS + 2);
   out.AddNote() << "Lon: " << UtLonPos(aLon, UtAngle::FmtDMS + 2);
   out.AddNote() << "Alt: " << aAlt << " m (" << aAlt * UtMath::cFT_PER_M << " ft)";
}

//! Prints out a length value in meters and feet on a single line prepended by a label.
//! used for debug output.
void WsfUtil::PrintLength(const std::string& aLabel, double aLength)
{
   ut::log::info() << aLabel << aLength << " m (" << aLength * UtMath::cFT_PER_M << " ft)";
}
