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

#include "WsfFusionStrategy.hpp"

#include <cmath>

#include "UtEllipsoidalEarth.hpp"
#include "UtInput.hpp"
#include "UtMat3.hpp"
#include "UtMath.hpp"
#include "WsfException.hpp"
#include "WsfFusionStrategyTypes.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfPlatform.hpp"
#include "WsfProcessor.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrackList.hpp"
#include "WsfTrackManager.hpp"

WsfFusionStrategy::FusionStrategyTypes* WsfFusionStrategy::mTypesPtr = nullptr;


// =================================================================================================
// static
//! A general-purpose method for updating the "non-kinematic" fields of the track
//! (frequency, side, type, iff, track quality).
//! TODO At some point we may want to break these out as individual, script-able methods,
//! but *only* for default fusion.
bool WsfFusionStrategy::UpdateNonKinematicData(double           aSimTime,
                                               WsfTrackManager& aTrackManager,
                                               WsfLocalTrack&   aLocalTrack,
                                               const WsfTrack&  aNonLocalTrack)
{
   return (aTrackManager.GetFusionStrategy().UpdateNonKinematicData(aSimTime, aLocalTrack, aNonLocalTrack));
}

// =================================================================================================
//! A general-purpose method for updating the "non-kinematic" fields of the track
//! (frequency, side, type, iff, track quality).
//! TODO At some point we may want to break these out as individual, script-able methods,
//! but *only* for default fusion.
//! @param aSimTime The current simulation time.
//! @param aLocalTrack The track to be updated.
//! @param aNonLocalTrack The track with information to be extracted to the local track.
bool WsfFusionStrategy::UpdateNonKinematicData(double aSimTime, WsfLocalTrack& aLocalTrack, const WsfTrack& aNonLocalTrack)
{
   bool updated = false;

   updated |= UpdateType(aSimTime, aLocalTrack, aNonLocalTrack);
   updated |= UpdateIFF_Status(aSimTime, aLocalTrack, aNonLocalTrack);
   updated |= UpdateSide(aSimTime, aLocalTrack, aNonLocalTrack);
   updated |= UpdateSpatialDomain(aSimTime, aLocalTrack, aNonLocalTrack);
   updated |= UpdateSignalToNoise(aSimTime, aLocalTrack, aNonLocalTrack);
   updated |= UpdateTrackQuality(aSimTime, aLocalTrack, aNonLocalTrack);
   updated |= UpdateSignalList(aSimTime, aLocalTrack, aNonLocalTrack);
   updated |= UpdateAuxData(aSimTime, aLocalTrack, aNonLocalTrack);
   return updated;
}

WsfSimulation* WsfFusionStrategy::GetSimulation() const
{
   WsfSimulation* sim(nullptr);
   if (mTrackManagerPtr != nullptr)
   {
      sim = mTrackManagerPtr->GetSimulation();
   }
   else if (mProcessorPtr != nullptr)
   {
      sim = mProcessorPtr->GetSimulation();
   }
   return sim;
}

double WsfFusionStrategy::GetSimTime()
{
   double simTime(0.0);
   if (mTrackManagerPtr != nullptr)
   {
      simTime = mTrackManagerPtr->GetSimulation()->GetSimTime();
   }
   else if (mProcessorPtr != nullptr)
   {
      simTime = mProcessorPtr->GetSimulation()->GetSimTime();
   }
   return simTime;
}

WsfPlatform* WsfFusionStrategy::GetPlatform() const
{
   WsfPlatform* pltfrm(nullptr);
   if (mTrackManagerPtr != nullptr)
   {
      pltfrm = mTrackManagerPtr->GetPlatform();
   }
   else if (mProcessorPtr != nullptr)
   {
      pltfrm = mProcessorPtr->GetPlatform();
   }
   return pltfrm;
}

WsfFusionStrategy::WsfFusionStrategy()
   : mTrackManagerPtr(nullptr)
   , mProcessorPtr(nullptr)
{
}

WsfFusionStrategy::WsfFusionStrategy(const WsfFusionStrategy& aSrc)
   : mTrackManagerPtr(nullptr)
   , // Set during Initialize.
   mProcessorPtr(nullptr)
{
}

bool WsfFusionStrategy::DebugEnabled() const
{
   bool debugEnabled(false);
   if (mTrackManagerPtr != nullptr)
   {
      debugEnabled = mTrackManagerPtr->DebugEnabled();
   }
   else if (mProcessorPtr != nullptr)
   {
      debugEnabled = mProcessorPtr->DebugEnabled();
   }
   return debugEnabled;
}

//! Fuse estimates from two tracks.  The two tracks must contain identical kinds of data in their state
//! estimates and covariance matrices (for instance, don't mix 3-d and 2-d based covariance matrices or
//! two different kinds of 2-d covariances).
//! @note The two estimates are fused based on their associated covariances; the larger the covariance,
//! the smaller the weight given to the estimate in the fusion.  It is assumed that the two estimates are
//! independent (an assumption that is only approximate due to common process noise, and should be revisited).
//! @note The weighted average fusion algorithm is based on the open-source algorithm referenced in
//! Bar Shalom and Li, Multi-target-Multisensor Fusion, equations 8.3.3-12 and 8.3.3-15, p.443.
void WsfFusionStrategy::FuseEstimates(const UtMatrixd& aEstimate1,
                                      const UtMatrixd& aEstimate2,
                                      const UtMatrixd& aCovar1,
                                      const UtMatrixd& aCovar2,
                                      UtMatrixd&       aEstimateOut,
                                      UtMatrixd&       aCovarOut)
{
   // Find the common covariance term, Inv(C1 + C2)
   unsigned  size = aEstimate1.GetSize();
   UtMatrixd commonTerm(aCovar1 + aCovar2);
   bool      matrixInverted = true;
   if (WsfFusionStrategyTypes::mCheckFuseEstimates)
   {
      // try/catch block is optional because of a potential performance hit:
      try
      {
         commonTerm.Invert();
      }
      catch (UtMatrixd::SingularMatrixError&)
      {
         matrixInverted = false;
      }
   }
   else
   {
      // Standard execution; assume we can invert the matrix:
      commonTerm.Invert();
   }
   if (matrixInverted)
   {
      UtMatrixd term1(aCovar2);
      UtMatrixd term2(aCovar1);
      term1 = term1 * commonTerm;
      UtMatrixd estimateTerm1(size, 1);
      UtMatrixd estimateTerm2(size, 1);
      estimateTerm1.Multiply(term1, aEstimate1);
      term2 = term2 * commonTerm;
      estimateTerm2.Multiply(term2, aEstimate2);
      aEstimateOut.Add(estimateTerm1, estimateTerm2);
      UtMatrixd i1(size, size);
      UtMatrixd i(aCovar2);
      i1.Multiply(commonTerm, i);
      aCovarOut.Multiply(aCovar1, i1);
   }
   else
   {
      // Pass estimate2 and covar2 out
      // (only in case where matrix invert fails and we have set
      /// the check_fuse_estimates flag in the user input).
      aEstimateOut = aEstimate2;
      aCovarOut    = aCovar2;
   }
}

//! Convert an azimuth-range estimate and covariance into a 3-d equivalent.
//! No covariance contribution from elevation is considered for this method.
void WsfFusionStrategy::NormalizeAzRangeEstimate(const UtMatrixd& aEstimate,
                                                 const UtMatrixd& aCovar,
                                                 const double&    aElevation,
                                                 UtMatrixd&       aNormEstimate,
                                                 UtMatrixd&       aNormCovar)
{
   double sinEl = sin(aElevation);
   double cosEl = cos(aElevation);
   double range = sqrt(aEstimate(0, 0) * aEstimate(0, 0) + aEstimate(1, 0) * aEstimate(1, 0));

   aNormEstimate(0, 0) = aEstimate(0, 0) * cosEl;
   aNormEstimate(1, 0) = aEstimate(1, 0) * cosEl;
   aNormEstimate(2, 0) = range * sinEl;

   // For the covariance, copy from the 2x2 to the 3x3.
   // Here the elevation is an independent channel that does not mix with the az-el.
   // Set the el variance very high, and el cross covars. to zero,
   // (anticipating fusion with a el-range or 3d covar matrix).
   aNormCovar(0, 0) = aCovar(0, 0) * cosEl * cosEl;
   aNormCovar(0, 1) = aCovar(0, 1) * cosEl * cosEl;
   aNormCovar(1, 0) = aCovar(1, 0) * cosEl * cosEl;
   aNormCovar(1, 1) = aCovar(1, 1) * cosEl * cosEl;
   aNormCovar.FillRow(2, 0.0);
   aNormCovar.FillColumn(2, 0.0);
   aNormCovar(2, 2) = 1.0e10;
}

//! Convert an el-range estimate and covariance into a 3-d equivalent.
//! No covariance contribution from azimuth is considered for this method.
void WsfFusionStrategy::NormalizeElRangeEstimate(const UtMatrixd& aEstimate,
                                                 const UtMatrixd& aCovar,
                                                 const double&    aAzimuth,
                                                 UtMatrixd&       aNormEstimate,
                                                 UtMatrixd&       aNormCovar)
{
   double sinAz = sin(aAzimuth);
   double cosAz = cos(aAzimuth);

   // Switch the elements so that the z component is in the proper place for the 3d estimate
   aNormEstimate(0, 0) = aEstimate(0, 0) * cosAz;
   aNormEstimate(1, 0) = aEstimate(0, 0) * sinAz;
   aNormEstimate(2, 0) = aEstimate(1, 0);

   // For the covariance, copy from the 2x2 to the 3x3.
   // Take into account the mixing of the x and y components with azimuth.
   aNormCovar.Fill(0.0);
   aNormCovar(0, 0) = aCovar(0, 0) * cosAz * cosAz;
   aNormCovar(1, 1) = aCovar(0, 0) * sinAz * sinAz;
   aNormCovar(2, 2) = aCovar(1, 1);
   aNormCovar(0, 2) = aCovar(0, 1) * cosAz;
   aNormCovar(2, 0) = aNormCovar(0, 2);
   aNormCovar(2, 1) = aCovar(0, 1) * sinAz;
   aNormCovar(1, 2) = aNormCovar(2, 1);
}

//! Temporary private method. The copy code may be replaced by UtMatrix utilities.
void ConvertNED_MatrixToWCS(UtMatrixd& aMatrix, const double aWCS_ToNED_Transform[3][3])
{
   int    i;
   double WCS_Matrix[3][3];
   double NED_Matrix[3][3];
   for (i = 0; i < 3; ++i)
   {
      for (int j = 0; j < 3; ++j)
      {
         NED_Matrix[i][j] = aMatrix(i, j);
      }
   }
   UtMat3d::InverseTransform(WCS_Matrix, aWCS_ToNED_Transform, NED_Matrix);
   for (i = 0; i < 3; ++i)
   {
      for (int j = 0; j < 3; ++j)
      {
         aMatrix(i, j) = WCS_Matrix[i][j];
      }
   }
}
