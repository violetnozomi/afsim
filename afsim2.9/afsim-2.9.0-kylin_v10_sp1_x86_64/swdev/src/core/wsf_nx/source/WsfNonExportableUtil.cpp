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

#include "WsfNonExportableUtil.hpp"

#include "UtMath.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrack.hpp"

namespace
{
// This table is used in the GetLink16_TrackQuality function.
const double sErrorDistanceTable[14] = {177697.0, // feet
                                        88662.0,
                                        70927.0,
                                        53202.0,
                                        35504.0,
                                        17590.0,
                                        7101.0,
                                        3550.0,
                                        567.0,
                                        283.0,
                                        144.0,
                                        68.0,
                                        34.0,
                                        19.0};
} // namespace

//! Compute the Link-16 track quality for a given track.  This method does not currently support
//! the sensor platform's location error.  The given track should have a valid state or measurement
//! covariance for accurate computation.
//! @param aSimTime The simulation time.
//! @param aTrack The track used to compute the Link-16 track quality.
//! @return the WSF-normalized Link-16 track quality, in the range 0.0-1.0 (not 0-15).
double WsfNonExportableUtil::GetLink16_TrackQuality(double aSimTime, const WsfTrack& aTrack)
{
   double quality = aTrack.GetTrackQuality();

   const UtCovariance* covarPtr = nullptr;
   if (aTrack.StateCovarianceMatrixValid())
   {
      covarPtr = aTrack.GetStateCovariance();
   }
   else if (aTrack.MeasurementCovarianceValid())
   {
      covarPtr = aTrack.GetMeasurementCovariance();
   }

   if (covarPtr != nullptr)
   {
      // We do not require valid velocity, but this is preferred.
      double speed = 0.0;
      if (aTrack.VelocityValid())
      {
         speed = aTrack.GetSpeed();
      }

      // Calculate the "cap" (circular area of probability)
      // We compute the "varTp" as the mean of the x-y-z diagonal components:
      const UtCovariance& covar = *covarPtr;
      double              varTp = (covar(0, 0) + covar(1, 1) + covar(2, 2)) / 3.0;
      quality                   = GetLink16_TrackQuality(aSimTime, aTrack.GetUpdateTime(), varTp, speed);
   }

   return quality;
}

//! (Deprecated) Compute the Link-16 track quality for a given track.  This method does not currently support
//! the sensor platform's location error.  The given track must have a valid covariance matrix.
//! @param aSimTime The current simulation time.
//! @param aTrackUpdateTime Simulation time at which the track was previously updated.
//! @param aCovar The covariance that is used to create the track quality.
//! @param aSpeed The current estimated speed of the tracked object (optional).
//! @return the WSF-normalized Link-16 track quality, in the range 0.0-1.0 (not 0-15).  If
//! the track quality cannot be computed, zero is returned.
double WsfNonExportableUtil::GetLink16_TrackQuality(double              aSimTime,
                                                    double              aTrackUpdateTime,
                                                    const UtCovariance& aCovar,
                                                    double              aSpeed)
{
   // Calculate the "cap" (circular area of probability)
   // We compute the "varTp" as the mean of the x-y-z diagonal components:
   double varTp = (aCovar(0, 0) + aCovar(1, 1) + aCovar(2, 2)) / 3;
   return GetLink16_TrackQuality(aSimTime, aTrackUpdateTime, varTp, aSpeed);
}

//! Compute the Link-16 track quality for a given track.  This method does not currently support
//! the sensor platform's location error.
//! @param aSimTime The current simulation time.
//! @param aTrackUpdateTime Simulation time at which the track was previously updated.
//! @param aVarTp The average uncertainty in position squared (average variance).
//! @param aSpeed The current estimated speed of the tracked object (optional).
//! @return the WSF-normalized Link-16 track quality, in the range 0.0-1.0 (not 0-15).  If
//! the track quality cannot be computed, zero is returned.
double WsfNonExportableUtil::GetLink16_TrackQuality(double aSimTime, double aTrackUpdateTime, double aVarTp, double aSpeed)
{
   double quality = 0.0;

   // Calculate the "cap" (circular area of probability)
   double delT = aSimTime - aTrackUpdateTime; // time since last update

   // double cap = 9.0 * UtMath::cPI * varTp + UtMath::cPI * speed * speed * delT * delT;
   // The square root of this (divided by pi- notional radius) is the error distance, which is all we need for this computation.
   double errorDistance = 3.0 * sqrt(aVarTp) + aSpeed * delT;

   // convert to feet.
   double errorDistanceFt = errorDistance * UtMath::cFT_PER_M;

   unsigned tq       = 0;
   double   fraction = 0.0;
   while (tq < 14)
   {
      if (errorDistanceFt > sErrorDistanceTable[tq])
      {
         if (tq > 0)
         {
            fraction =
               (errorDistanceFt - sErrorDistanceTable[tq]) / (sErrorDistanceTable[tq - 1] - sErrorDistanceTable[tq]);
         }
         break;
      }
      ++tq;
   }

   // the table is zero based; add one.
   ++tq;

   // Normalize track quality to the WSF standard, which is 0.0-1.0
   quality = (static_cast<double>(tq) - fraction) / 15.0;

   return quality;
}

//! Given a Link-16 track quality (see mil-std 6016C, pp 303-305), stuff the equivalent variance into
//! the diagonals of a 3x3 matrix, producing an equivalent spherical covariance matrix.
//! @param aLink16_TrackQuality the track quality computed according to the 6016C standard and normalized into
//! the range 0-1.
//! @return a 3x3 matrix with the diagonal elements set to the variance associated with the track quality.
//! This is a "pseudo" covariance matrix that has a spherical representation.
//! @note It is the responsibility of the caller to delete the returned matrix.
UtCovariance* WsfNonExportableUtil::ConvertLink16_TrackQualityToSphericalCovariance(double aLink16_TrackQuality)
{
   UtCovariance* covariancePtr = new UtCovariance(3, 3); // elements are initialized to zero.
   unsigned      tq            = static_cast<unsigned>(aLink16_TrackQuality * 15.0);

   // TQ *should* not be less than one, but check anyway.
   if ((tq == 0) || (tq == 1))
   {
      tq = 2; // clamp worse or nonexistent track quality to the largest defined value.
   }

   unsigned      tqIndex       = tq - 2; // brings it into the range 0-13 supported by the table
   double        errorDistance = sErrorDistanceTable[tqIndex];
   double        varTp         = pow(errorDistance / 3.0, 2);
   UtCovariance& covar         = *covariancePtr;
   covar(0, 0)                 = varTp;
   covar(1, 1)                 = varTp;
   covar(2, 2)                 = varTp;

   return covariancePtr;
}
