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

#ifndef WSFUTIL_HPP
#define WSFUTIL_HPP

#include "wsf_export.h"

#include <vector>

class UtEntity;
class UtInput;
#include "UtMatrix.hpp"
#include "UtSphericalEarth.hpp"
#include "UtVec3.hpp"
class WsfPlatform;
class WsfScenario;
class WsfTrack;
#include "WsfTrackId.hpp"

//! A set of utilities that perform generic calculations among WSF objects.
namespace WsfUtil
{
template<class T1, class T2>
double TimeToDestination(T1& aSource, T2& aDest, const double aVel[3]);

template<class T1, class T2>
double DistanceToDestination(T1& aSource, T2& aDest);

int WSF_EXPORT CountBitsOn(unsigned int aBitMask);

//! Form a bit mask with specified number of least-significant bits on.
//! @param aNumBits The number of bits to be on.
//! @returns The resulting mask.
inline unsigned int GenerateBitMask(int aNumBits)
{
   assert((aNumBits >= 1) && (aNumBits <= 32));
   if (aNumBits <= 0)
   {
      return 0;
   }
   else if (aNumBits >= 32)
   {
      return 0xFFFFFFFF;
   }
   return 0xFFFFFFFF >> (32 - aNumBits);
}

void WSF_EXPORT ClosestApproachPoint2D(const double aMoverLocWCS[3],
                                       const double aMoverVelWCS[3],
                                       const double aTargetLocWCS[3],
                                       double       aClosestLocWCS[3]);

double WSF_EXPORT ClosestApproachPoint3D(const double aMoverLocWCS[3],
                                         const double aMoverVelWCS[3],
                                         const double aTargetLocWCS[3],
                                         double       aClosestLocWCS[3]);

bool WSF_EXPORT PotentiallyWithinRange(double aSimTime, WsfPlatform* aObject1Ptr, WsfPlatform* aObject2Ptr, double aMaximumRange);


bool WSF_EXPORT TriangulateLocation(const double  aOriginWCS_1[3],
                                    const double& aBearing1,
                                    const double& aElevation1,
                                    const double  aOriginWCS_2[3],
                                    const double& aBearing2,
                                    const double& aElevation2,
                                    double        aLocationWCS[3]);


void WSF_EXPORT ConvertRangeBearing(const double aFromLocationWCS[3], UtEntity* aToEntity, double& aRange, double& aBearing);

void WSF_EXPORT ErrorNED(double aRange,
                         double aBearing,
                         double aElevation,
                         double aRangeError,
                         double aBearingError,
                         double aElevationError,
                         double aErrorNED[3]);

void WSF_EXPORT ErrorNED(double aRange, double aBearing, double aRangeError, double aBearingError, double aErrorNED[3]);

int WSF_EXPORT TrackIdToInt(const WsfTrackId& aTrackId);

WsfTrackId WSF_EXPORT IntToTrackId(int aIntTrackId);

void WSF_EXPORT CovarianceToEllipse_2x2(std::vector<double>& aMatrix,
                                        double&              aSemiMajorAxis,
                                        double&              aSemiMinorAxis,
                                        double&              aBearingRad);

double WSF_EXPORT BallisticPropagate(const UtVec3d& aLocationWCS,
                                     const UtVec3d& aVelocityWCS,
                                     double         aStopAltitude,
                                     UtVec3d&       aPropagatedLocationWCS,
                                     UtVec3d&       aPropagatedVelocityWCS);

void WSF_EXPORT MakeTrack(double aSimTime, WsfTrack& aTrack, WsfPlatform& aTarget, WsfPlatform& aSource);


//! Validity Masks associated with the "aValidity" parameter in the following method:
static const unsigned short cLLA_ERROR_VALID     = 0x01;
static const unsigned short cSPEED_ERROR_VALID   = 0x02;
static const unsigned short cHEADING_ERROR_VALID = 0x04;

void WSF_EXPORT GetErrorEstimateLLA_SpeedHeading(const WsfTrack& aTrack,
                                                 unsigned short& aValidity,
                                                 double&         aStDevLat,
                                                 double&         aStDevLon,
                                                 double&         aStDevAlt,
                                                 double&         aStDevSpeed,
                                                 double&         aStDevHeading);

void WSF_EXPORT PrintLLA(const std::string& aLabel, double aLocationWCS[3]);
void WSF_EXPORT PrintLLA(const std::string& aLabel, double aLat, double aLon, double aAlt);
void WSF_EXPORT PrintLength(const std::string& aLabel, double aLength);

/** Calculate the time it would take an object (e.g., a WsfPlatform)
to traverse a great circle distance from a source to a destination.

@param   aSource The source object.
@param   aDest   The destination object.
@param   aVel    velocity vector for the source object.
@note  'aSource' and 'aDest' must be of types that implement the method GetLocationLLA(
double& lat, double& lon, double& alt) (e.g., WsfPlatform::GetLocationLLA).
@returns The time required to reach the given
destination.
*/
template<class T1, class T2>
double TimeToDestination(T1& aSource, T2& aDest, const double aVel[3])
{
   return DistanceToDestination(aSource, aDest) / UtVec3d::Magnitude(aVel);
}

/** Calculate the great circle distance from a source to a destination.

@param   aSource The source object (non-const; typically a WsfPlatform or UtEntity).
@param   aDest   The destination object.
@note  'aSource' and 'aDest' must be of types that implement the method GetLocationLLA(
double& lat, double& lon, double& alt) (e.g., WsfPlatform::GetLocationLLA).
@returns The great circle distance between 'source' and 'destination.'
*/
template<class T1, class T2>
double DistanceToDestination(T1& aSource, T2& aDest)
{
   double sourceLat;
   double sourceLon;
   double sourceAlt;
   double destLat;
   double destLon;
   double destAlt;
   aSource.GetLocationLLA(sourceLat, sourceLon, sourceAlt);
   aDest.GetLocationLLA(destLat, destLon, destAlt);
   double heading;
   double distance;
   UtSphericalEarth::GreatCircleHeadingAndDistance(sourceLat, sourceLon, destLat, destLon, heading, distance);
   return distance;
}

/** Calculate the slant range distance from a source to a destination (second parameter is const).
@param   aSource The source object (non-const; typically a WsfPlatform or UtEntity).
@param   aDest   The destination object.
@note  'aSource' and 'aDest' must be of types that implement the method GetLocationWCS(
double[3]) (e.g., WsfPlatform::GetLocationWCS).
@returns The slant range distance between 'source' and 'destination.'
*/
template<class T1, class T2>
double SlantRange(T1& aSource, T2& aDest)
{
   double sourceLocWCS[3];
   double destLocWCS[3];
   aSource.GetLocationWCS(sourceLocWCS);
   aDest.GetLocationWCS(destLocWCS);
   double targetVec[3];
   UtVec3d::Subtract(targetVec, destLocWCS, sourceLocWCS);
   return UtVec3d::Magnitude(targetVec);
}

} // end namespace WsfUtil

#endif
