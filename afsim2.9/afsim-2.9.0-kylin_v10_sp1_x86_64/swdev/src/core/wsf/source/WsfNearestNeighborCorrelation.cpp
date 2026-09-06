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

#include "WsfNearestNeighborCorrelation.hpp"

#include <memory>

#include "UtDCM.hpp"
#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtQuaternion.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfPlatform.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackList.hpp"
#include "WsfTrackManager.hpp"

WsfNearestNeighborCorrelation::WsfNearestNeighborCorrelation()
   : WsfCorrelationStrategy()
   , mTrackingSigma(1.0)
   , mTurningSigma(1.0)
   , mChangeTime(1.0)
   , mPreciseMode(true)
{
}

// virtual
bool WsfNearestNeighborCorrelation::ProcessInput(UtInput& aInput)
{
   bool        processed = true;
   std::string command   = aInput.GetCommand();
   if (command == "tracking_sigma")
   {
      aInput.ReadValue(mTrackingSigma);
   }
   else if (command == "turning_sigma")
   {
      aInput.ReadValue(mTurningSigma);
   }
   else if (command == "coast_time")
   {
      aInput.ReadValueOfType(mChangeTime, UtInput::cTIME);
   }
   else if (command == "precise_mode")
   {
      aInput.ReadValue(mPreciseMode);
   }
   else
   {
      processed = WsfCorrelationStrategy::ProcessInput(aInput);
   }
   return processed;
}

void WsfNearestNeighborCorrelation::Decorrelate(const WsfTrackId& aRawTrackId)
{
   if (mChangeMap.find(aRawTrackId) != mChangeMap.end())
   {
      mChangeMap.erase(aRawTrackId);
   }
   if (mTurnSet.find(aRawTrackId) != mTurnSet.end())
   {
      mTurnSet.erase(aRawTrackId);
   }
   WsfCorrelationStrategy::Decorrelate(aRawTrackId);
}

WsfCorrelationStrategy* WsfNearestNeighborCorrelation::Clone() const
{
   return new WsfNearestNeighborCorrelation(*this);
}

// virtual
//! Given a non-local track update (or measurement), find the track in the track list
//! that correlates with the given track or measurement.
WsfLocalTrack* WsfNearestNeighborCorrelation::CorrelateImpl(double             aSimTime,
                                                            const WsfTrack&    aNonLocalTrack,
                                                            WsfLocalTrackList& aTrackList)
{
   double         confidenceSigma;
   WsfTrackId     existingCorrelation;
   WsfLocalTrack* correlatedTrackPtr =
      CorrelateWithConfidenceInterval(aSimTime, aNonLocalTrack, aTrackList, confidenceSigma, existingCorrelation);

   if (correlatedTrackPtr != nullptr)
   {
      WsfTrackId                     correlation = correlatedTrackPtr->GetTrackId();
      std::set<WsfTrackId>::iterator turnIter    = mTurnSet.find(correlation);
      bool                           isTurning   = (turnIter != mTurnSet.end());

      if (confidenceSigma >= mTrackingSigma)
      {
         if (isTurning)
         {
            mTurnSet.erase(turnIter); // No longer turning.
         }
         mChangeMap[aNonLocalTrack.GetTrackId()] = aSimTime;
      }
      else if (confidenceSigma >= mTurningSigma)
      {
         if (!isTurning)
         {
            mTurnSet.insert(correlation);
         }
         mChangeMap[aNonLocalTrack.GetTrackId()] = aSimTime;
      }
      else
      {
         correlatedTrackPtr = nullptr; // No correlation above threshold
         if (isTurning)
         {
            mTurnSet.erase(turnIter); // Clean up.
         }
      }
      if ((correlatedTrackPtr != nullptr) && (GetTrackManager().DebugEnabled()))
      {
         auto out = ut::log::debug() << "Tracks correlated.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Correlated Track: " << correlatedTrackPtr->GetTrackId();
         out.AddNote() << "Raw Track: " << aNonLocalTrack.GetTrackId();
         out.AddNote() << "Sigma: " << confidenceSigma;
      }
   }

   if ((correlatedTrackPtr == nullptr) &&
       (!existingCorrelation.IsNull())) // not correlated this evaluation, but an existing correlation exists
   {
      std::map<WsfTrackId, double>::iterator iter = mChangeMap.find(aNonLocalTrack.GetTrackId());
      if (iter != mChangeMap.end())
      {
         if ((aSimTime - iter->second) <= mChangeTime)
         {
            // Use the existing correlation as the correlated local track.
            correlatedTrackPtr = GetTrackManager().GetTrackList().FindTrack(existingCorrelation);
            assert(correlatedTrackPtr != nullptr);

            if (GetTrackManager().DebugEnabled())
            {
               auto out = ut::log::debug() << "Tracks correlated.";
               out.AddNote() << "T = " << aSimTime;
               out.AddNote() << "Correlated Track: " << correlatedTrackPtr->GetTrackId();
               out.AddNote() << "Raw Track: " << aNonLocalTrack.GetTrackId();
               out.AddNote() << "Raw track is coasting.";
            }
         }
         else // No longer correlated.
         {
            correlatedTrackPtr = nullptr;
            mChangeMap.erase(iter);
         }
      }
   }

   return correlatedTrackPtr;
}

// private
WsfCovariance* CreateCovariance(const WsfMeasurement& aTrack)
{
   WsfCovariance* newCovarPtr = nullptr;
   if (aTrack.StateCovarianceValid())
   {
      newCovarPtr = new WsfCovariance(*aTrack.GetStateCovariance());
   }
   else if (aTrack.MeasurementCovarianceValid())
   {
      newCovarPtr = new WsfCovariance(*aTrack.GetMeasurementCovariance());
   }
   else
   {
      newCovarPtr = WsfCovariance::CreateMeasurementCovarianceWCS(aTrack);
   }

   return newCovarPtr;
}

WsfLocalTrack* WsfNearestNeighborCorrelation::CorrelateWithConfidenceInterval(double             aSimTime,
                                                                              const WsfTrack&    aNonLocalTrack,
                                                                              WsfLocalTrackList& aTrackList,
                                                                              double&            aSigmaConfidence,
                                                                              WsfTrackId&        aExistingCorrelation)
{
   // See if there is a previously correlated track:
   WsfLocalTrack* correlatedTrackPtr = nullptr;
   aSigmaConfidence                  = 0.0;

   // Take into account all possible sources of track data to obtain a WCS location (can be 2D or 3D).
   double trackUpdateLocWCS[3];
   bool   ok = aNonLocalTrack.GetExtrapolatedLocationWCS(aSimTime, trackUpdateLocWCS);

   if (ok) // only handle these cases for now.
   {
      // Find the best current match:
      double         minDistanceSquared             = 1.0e+200;
      WsfLocalTrack* nearestNeighborPtr             = nullptr;
      double         nearestNeighborTargetVecWCS[3] = {0.0};
      WsfTrackId     currentSensorTrackId;

      for (unsigned int trackNum = 0; trackNum < aTrackList.GetTrackCount(); ++trackNum)
      {
         WsfLocalTrack* localTrackPtr = aTrackList.GetTrackEntry(trackNum);
         double         locWCS[3];
         if (localTrackPtr->IsCorrelatedWith(aNonLocalTrack.GetTrackId()))
         {
            aExistingCorrelation = localTrackPtr->GetTrackId();
         }
         ok = localTrackPtr->GetExtrapolatedLocationWCS(aSimTime, locWCS);
         if (ok)
         {
            // Find squared distance and compare vs. covariance
            double targetVecWCS[3];
            UtVec3d::Subtract(targetVecWCS, trackUpdateLocWCS, locWCS);
            double distanceSquared = UtVec3d::MagnitudeSquared(targetVecWCS);

            if (distanceSquared < minDistanceSquared)
            {
               UtVec3d::Set(nearestNeighborTargetVecWCS, targetVecWCS);
               minDistanceSquared = distanceSquared;
               nearestNeighborPtr = localTrackPtr;
            }
         }
      } // Done with distance checks.

      if (nearestNeighborPtr != nullptr)
      {
         // Check covariance matrices.
         WsfCovariance* nearestNeighborCovarPtr = CreateCovariance(*nearestNeighborPtr);
         WsfCovariance* trackUpdateCovarPtr     = CreateCovariance(aNonLocalTrack);

         if ((nearestNeighborCovarPtr != nullptr) && (trackUpdateCovarPtr != nullptr))
         {
            nearestNeighborPtr->KinematicUpdate(aSimTime);
            WsfCovariance& nearestNeighborCovar = *nearestNeighborCovarPtr;
            WsfCovariance& trackUpdateCovar     = *trackUpdateCovarPtr;

            // Bring the nearest neigbhor up-to-date in terms of its process noise model.
            nearestNeighborCovar.ComputePredictedState(aSimTime, trackUpdateCovar, aNonLocalTrack);

            // Check whether we are within the errors of the tracks.
            // By default we perform a quick check using diagonal variance values, which correspond to variances along
            // the ECEF x, y, z axes.  This is not a comprehensive test, however, and can give some misleading results
            // for covariances that are highly elongated.  If we specified "precise_mode" in the input, compute the
            // exact errors along the vector between covariance centriods.
            double              trackUpdateToTrackDistance = UtVec3d::Magnitude(nearestNeighborTargetVecWCS);
            static const double x[3]                       = {1.0, 0.0, 0.0};

            // Find the angle of rotation.  The idea is to align a rotated coordinate system along the target vector,
            // so that the x axes of that coordinate system align with that vector,
            // then compare the length of the vector with with sum of the lengths of the covariance matrices' x-x component.
            double angle = acos(
               std::max(-1.0,
                        std::min(UtVec3d::DotProduct(nearestNeighborTargetVecWCS, x) / trackUpdateToTrackDistance, 1.0)));
            double axis[3];
            UtVec3d::CrossProduct(axis, nearestNeighborTargetVecWCS, x);
            UtVec3d::Normalize(axis);
            UtDCM  rotMatrixDCM(angle, axis);
            double rotMatrix[3][3];
            rotMatrixDCM.Get(rotMatrix);
            UtMatrixd      invRotMatrix(3, 3);
            WsfCovariance& rotNearestNeighborCovar = nearestNeighborCovar;
            WsfCovariance& rotTrackUpdateCovar     = trackUpdateCovar;
            UtCovariance::ConvertFromFrameA_ToFrameB(rotNearestNeighborCovar, rotMatrix);
            UtCovariance::ConvertFromFrameA_ToFrameB(rotTrackUpdateCovar, rotMatrix);
            aSigmaConfidence =
               (sqrt(rotNearestNeighborCovar(0, 0)) + sqrt(rotTrackUpdateCovar(0, 0))) / trackUpdateToTrackDistance;
            if (GetTrackManager().DebugEnabled())
            {
               double distance = sqrt(rotNearestNeighborCovar(0, 0)) + sqrt(rotTrackUpdateCovar(0, 0));
               auto   out      = ut::log::debug() << "NN correlation result:";
               out.AddNote() << "T = " << aSimTime;
               out.AddNote() << "Ellipsoid Distance: " << distance;
               out.AddNote() << "Track-to-Track Distance: " << trackUpdateToTrackDistance;
               out.AddNote() << "Sigma: " << aSigmaConfidence;
            }
         }
         else // we have zero or one covariance; revert to using the expected location error.
         {
            // Note that we are only using tracks now; extending this for measurements is possible
            // (could use the IsTrack() method to be as rigorous as possible in extracting all possible data.
            aSigmaConfidence = nearestNeighborPtr->GetExpectedLocationError() + aNonLocalTrack.GetExpectedLocationError();
         }

         correlatedTrackPtr = nearestNeighborPtr;
         delete nearestNeighborCovarPtr;
         delete trackUpdateCovarPtr;
      }
   }
   else // There is no location available (bearing, bearing-elevation tracks)
   {
      // If this is a mirror of a local track, return the mirror.
      // This will prevent blow-up in the track list.
      for (unsigned int trackNum = 0; trackNum < aTrackList.GetTrackCount(); ++trackNum)
      {
         WsfLocalTrack* localTrackPtr = aTrackList.GetTrackEntry(trackNum);
         if (localTrackPtr->IsMirrorTrack(aNonLocalTrack))
         {
            correlatedTrackPtr = localTrackPtr;
            break;
         }
      }
   }

   return correlatedTrackPtr;
}
