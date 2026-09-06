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

#include "Utility.hpp"

#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "UtVec3.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfPlatform.hpp"
#include "WsfSensor.hpp"

// =================================================================================================
void Utility::ConvertRBA_ToWCS(WsfPlatform* aPlatformPtr,
                               double       aGroundRange,
                               double       aBearing,
                               double       aAltitude,
                               double       aEarthRadiusMultiplier,
                               double       aLocationWCS[3])
{
   double sensorLat;
   double sensorLon;
   double sensorAlt;
   aPlatformPtr->GetLocationLLA(sensorLat, sensorLon, sensorAlt);

   // Process the trivial case.
   if (aGroundRange < 1.0)
   {
      WsfPlatform::ConvertLLAToWCS(sensorLat, sensorLon, aAltitude, aLocationWCS);
      return;
   }

   // The following method is more difficult that simply assuming a spherical Earth, but gives
   // more exact results when comparing against codes such as SALRAM and SUPPRESSOR. The overall
   // errors of the simpler methods aren't significant, but this produces a better match.

   // For the purposes of matching detections in various models, what we REALLY care about is the
   // slant range and elevation angle.

   // Given the ground range and altitude, calculate the slant range on the scaled earth.

   double earthRadius = UtSphericalEarth::cEARTH_RADIUS;
   double re          = earthRadius * aEarthRadiusMultiplier;
   double sideA       = re + sensorAlt;
   double sideB       = re + aAltitude;
   double angleC      = aGroundRange / re;
   double sideC       = sqrt((sideA * sideA) + (sideB * sideB) - (2.0 * sideA * sideB * cos(angleC)));
   double slantRange  = sideC;

   // Given the slant range and altitude, compute the elevation angle on the unscaled earth.

   re               = earthRadius;
   sideA            = re + sensorAlt;
   sideB            = re + aAltitude;
   double cosAngleB = ((sideA * sideA) + (sideC * sideC) - (sideB * sideB)) / (2.0 * sideA * sideC);
   double angleB    = acos(std::min(std::max(cosAngleB, -1.0), 1.0));
   double elevation = angleB - UtMath::cPI_OVER_2;

   double locationNED[3];
   double sinEl   = sin(elevation);
   double cosEl   = cos(elevation);
   locationNED[0] = slantRange * cosEl * cos(aBearing);
   locationNED[1] = slantRange * cosEl * sin(aBearing);
   locationNED[2] = -slantRange * sinEl;
   aPlatformPtr->ConvertNEDToWCS(locationNED, aLocationWCS);
}

// =================================================================================================
void Utility::CueSensorToTarget(WsfSensor* aSensorPtr, WsfPlatform* aTargetPtr)
{
   // Define a cue to the apparent target location so the detection will work with a tracking sensor.
   // NOTE: This code is basically a copy of the code in WsfEM_Interaction::WithinFieldOfView

   double cueLocWCS[3];
   aTargetPtr->GetLocationWCS(cueLocWCS);
   double earthRadiusScale = 1.0;
   if (aSensorPtr->GetEM_XmtrCount() > 0)
   {
      earthRadiusScale = aSensorPtr->GetEM_Xmtr(0).GetEarthRadiusMultiplier();
   }

   if (earthRadiusScale != 1.0)
   {
      double srcLocWCS[3];
      double tgtLocWCS[3];
      double lat;
      double lon;
      double srcAlt;
      double tgtAlt;
      aSensorPtr->GetLocationWCS(srcLocWCS);
      aSensorPtr->GetLocationLLA(lat, lon, srcAlt);
      aTargetPtr->GetLocationWCS(tgtLocWCS);
      aTargetPtr->GetLocationLLA(lat, lon, tgtAlt);
      double srcToTgtLocWCS[3];
      UtVec3d::Subtract(srcToTgtLocWCS, tgtLocWCS, srcLocWCS);
      double srcToTgtRange = UtVec3d::Magnitude(srcToTgtLocWCS);
      if (srcToTgtRange >= 1.0) // Objects are co-located if (srcToTgtRange < 1.0)
      {
         // Use the law of cosines to compute the elevation angle in the scaled Earth frame of the target with
         // respect to the source (Note that we are really computing the elevation angle + 90 degrees). This
         // angle is the elevation angle of the 'apparent' target in the non-scaled frame.
         //
         // sideA  is the side from the Earth center to the source location.
         // sideB  is the side from the Earth center to the target location.
         // sideC  is the side from the source location to the target location.
         // angleB is the included angle between sides A and C.

         double earthRadius = UtSphericalEarth::cEARTH_RADIUS * earthRadiusScale;
         double sideA       = earthRadius + srcAlt;
         double sideB       = earthRadius + tgtAlt;
         double sideC       = srcToTgtRange;
         double cosAngleB   = ((sideA * sideA) + (sideC * sideC) - (sideB * sideB)) / (2.0 * sideA * sideC);
         cosAngleB          = std::min(std::max(cosAngleB, -1.0), 1.0);

         // Using local versions of the WCS position vectors, modify the target WCS position vector so the
         // resulting elevation matches the apparent elevation.
         //
         // NOTE: This computation does NOT preserve the length of the slant range, but that is not important.
         // What we're after is unit pointing vector. The slant range is carried along separately as a scalar.

         double apparentSrcLocWCS[3];
         double apparentTgtLocWCS[3];
         double srcLocMag = UtVec3d::Normalize(apparentSrcLocWCS, srcLocWCS);
         double tgtLocMag = UtVec3d::Normalize(apparentTgtLocWCS, tgtLocWCS);

         double cosAngleC = UtVec3d::DotProduct(apparentSrcLocWCS, apparentTgtLocWCS);
         cosAngleC        = std::min(std::max(cosAngleC, -1.0), 1.0);
         double angleB    = acos(cosAngleB);
         double angleC    = acos(cosAngleC);
         double angleA    = UtMath::cPI - angleB - angleC;
         sideA            = srcLocMag;
         if (angleA != 0.0)
         {
            sideB = sideA * sin(angleB) / sin(angleA);
         }
         else
         {
            // sensor is directly below the target
            sideB = tgtLocMag;
         }
         double heightAdjustment = sideB - tgtLocMag;

         // Adjust the position of the target position vector to reflect its apparent position
         // and compute the aspect of the target WRT the source.

         UtVec3d::Multiply(apparentTgtLocWCS, tgtLocMag + heightAdjustment);
         UtVec3d::Subtract(srcToTgtLocWCS, apparentTgtLocWCS, srcLocWCS);
         UtVec3d::Normalize(srcToTgtLocWCS);
         UtVec3d::Multiply(srcToTgtLocWCS, srcToTgtRange);
         UtVec3d::Add(cueLocWCS, srcLocWCS, srcToTgtLocWCS);
      }
   }
   aSensorPtr->SetCuedLocationWCS(cueLocWCS);
}

// =================================================================================================
double Utility::GetDetectionReference(WsfSensor* aSensorPtr, double aDefaultThreshold)
{
   // TODO - for now this just returns the default threshold.
   // TODO - The sensor interface must be modified to get the nominal detection threshold.
   // TODO - (Some sensors use a hard value, others use something statistical like Marcum-Swerling).

   double threshold = aDefaultThreshold;
   //    if (aSensorPtr->GetEM_RcvrCount() > 0)
   //    {
   //       WsfEM_Rcvr& rcvr = aSensorPtr->GetEM_Rcvr(0);
   //       if (rcvr.GetDetectionReference() > 0.0)
   //       {
   //          threshold = rcvr.GetDetectionReference();
   //       }
   //    }
   //    cout << "Threshold=" << threshold << " abs; " << UtMath::LinearToDB(threshold) << " dB" << endl;
   return threshold;
}
