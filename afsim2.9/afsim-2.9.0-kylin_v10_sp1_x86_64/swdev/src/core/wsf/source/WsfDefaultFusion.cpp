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

#include "WsfDefaultFusion.hpp"

#include <algorithm>
#include <cassert>

#include "UtEllipsoidalEarth.hpp"
#include "UtLog.hpp"
#include "UtMat3.hpp"
#include "UtMath.hpp"
#include "UtMatrix.hpp"
#include "UtMeasurementUtil.hpp"
#include "UtMemory.hpp"
#include "UtScriptData.hpp"
#include "UtSphericalEarth.hpp"
#include "WsfCovariance.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfKalmanFilter.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfMeasurement.hpp"
#include "WsfPlatform.hpp"
#include "WsfProcessor.hpp"
#include "WsfScenario.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "WsfTrackList.hpp"
#include "WsfTrackManager.hpp"
#include "WsfTypes.hpp"
#include "WsfUtil.hpp"

// =================================================================================================
WsfDefaultFusion::WsfDefaultFusion(const WsfScenario& aScenario, bool aIsWeightedAverage /*=false*/)
   : mIsWeightedAverage(aIsWeightedAverage)
   , mContextPtr(ut::make_unique<WsfScriptContext>(*aScenario.GetScriptContext()))
   , mUpdateTypePtr(nullptr)
   , mUpdateSidePtr(nullptr)
   , mUpdateIFF_StatusPtr(nullptr)
   , mLocalTrackVarPtr(nullptr)
   , mNonLocalTrackVarPtr(nullptr)
{
   mContextPtr->DeclareVariable("WsfLocalTrack", "LOCAL_TRACK");
   mContextPtr->DeclareVariable("WsfTrack", "NON_LOCAL_TRACK");
}

bool WsfDefaultFusion::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "on_type_update")
   {
      mUpdateTypePtr = mContextPtr->CompileImplicitScript(aInput, "on_type_update", "void");
   }
   else
   {
      myCommand = mContextPtr->ProcessInput(aInput);
   }

   return myCommand;
}

bool WsfDefaultFusion::Initialize(WsfTrackManager* aTrackManagerPtr)
{
   bool ok = WsfFusionStrategy::Initialize(aTrackManagerPtr);
   ok &= mContextPtr->Initialize(*GetSimulation(), &GetTrackManager());
   if (ok)
   {
      mUpdateTypePtr       = mContextPtr->FindScript("on_type_update");
      mLocalTrackVarPtr    = &mContextPtr->GetContext().Var("LOCAL_TRACK");
      mNonLocalTrackVarPtr = &mContextPtr->GetContext().Var("NON_LOCAL_TRACK");
   }
   return ok;
}

// =================================================================================================
// virtual
void WsfDefaultFusion::ReviewTrack(WsfLocalTrack& aLocalTrack)
{
   ReviewIFF(GetTrackManager(), aLocalTrack);
   ReviewTrackQuality(GetTrackManager(), aLocalTrack);
   aLocalTrack.UpdateSignalList();
}

// =================================================================================================
// static
void WsfDefaultFusion::ReviewTrack(WsfTrackManager& aTrackManager, WsfLocalTrack& aLocalTrack)
{
   ReviewIFF(aTrackManager, aLocalTrack);
   ReviewTrackQuality(aTrackManager, aLocalTrack);
   aLocalTrack.UpdateSignalList();
}

// =================================================================================================
// virtual
bool WsfDefaultFusion::UpdateLocalTrackFromNonLocalTrack(double          aSimTime,
                                                         WsfLocalTrack&  aLocalTrack,
                                                         const WsfTrack& aNonLocalTrack)
{
   bool processed = true;
   if (aLocalTrack.IsMirrorTrack(aNonLocalTrack))
   {
      PerformMirrorTrackUpdate(aSimTime, aLocalTrack, aNonLocalTrack);
   }
   else
   {
      processed = PerformStandardTrackUpdate(aSimTime, aLocalTrack, aNonLocalTrack);
   }
   return processed;
}

bool WsfDefaultFusion::UpdateLocalResultFromNonLocalResult(double                 aSimTime,
                                                           WsfLocalSensorResult&  aLocalResult,
                                                           const WsfSensorResult& aNonLocalResult)
{
   // First check to see if the local result is just a mirror for the non-local track, and if so
   // perform simple replacement. This ensures that 'mirror' tracks reflects EXACTLY (except for
   // a identifying fields) the underlying non-local track.
   //
   // This special processing will be employed if and only if ALL of the contributing updates for
   // the track came from the same non-local track (unchanging track ID). If the local track EVER
   // receives an update from another source this special processing will be disabled.
   //
   // Note that this *could* probably be relaxed for some cases. If a local track was a mirror for
   // a sensor track and the sensor dropped the track, the track would continue to exist before
   // being purged. If a subsequent update came in from the same sensor then it could be safely
   // assimilated if perfect correlation and replacement fusion were being used.
   // if (aLocalTrack.IsMirrorTrack(aNonLocalTrack))
   if (aLocalResult.GetReceiver() == aNonLocalResult.GetReceiver() &&
       aLocalResult.GetTarget() == aNonLocalResult.GetTarget())
   {
      // Perform direct replacement.
      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "Platform replacing measurement from external source.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Ext. Source: " << aNonLocalResult.GetReceiver()->GetPlatform()->GetNameId();
      }
      aLocalResult.mMeasurement = aNonLocalResult.mMeasurement;
      return true;
   }

   bool localResultUpdated = false;

   // updateLocation refers to whether the locationWCS has been set
   // and should be copied/fused into the local track later.
   bool updateLocation = false;

   double         locationWCS[3];
   WsfCovariance* covariancePtr = nullptr;

   if (aNonLocalResult.mMeasurement.StateCovarianceValid())
   {
      assert(aNonLocalResult.mMeasurement.GetStateCovariance() != nullptr);
      // The covariancePtr has been set and should be copied/fused into the local result.
      covariancePtr = new WsfCovariance(*aNonLocalResult.mMeasurement.GetStateCovariance());
   }
   else if (aNonLocalResult.mMeasurement.MeasurementCovarianceValid())
   {
      assert(aNonLocalResult.mMeasurement.GetMeasurementCovariance() != nullptr);
      covariancePtr = new WsfCovariance(*aNonLocalResult.mMeasurement.GetMeasurementCovariance());
   }

   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Platform updating measurement from external source.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Ext. Source: " << aNonLocalResult.GetReceiver()->GetPlatform()->GetNameId();
   }

   bool nonLocalResultIs3D = aNonLocalResult.mMeasurement.LocationValid() ||
                             (aNonLocalResult.mMeasurement.RangeValid() && aNonLocalResult.mMeasurement.BearingValid() &&
                              aNonLocalResult.mMeasurement.ElevationValid());

   if (nonLocalResultIs3D /*&& // Either polar (RBE) or Cartesian 3-D location is valid.
       UpdateHasSufficientTrackQuality(aNonLocalTrack, aLocalTrack)*/)
   {
      if (aNonLocalResult.mMeasurement.LocationValid())
      {
         // The WCS location is valid.
         // It may be fused, depending on track quality.
         aNonLocalResult.mMeasurement.GetLocationWCS(locationWCS);
      }
      else // valid 3-D polar location (RBE)
      {
         assert(aNonLocalResult.mMeasurement.ElevationValid() && aNonLocalResult.mMeasurement.BearingValid() &&
                aNonLocalResult.mMeasurement.RangeValid());

         // retrieve the world coordinates of this RBE location.
         double origLocWCS[3];
         aNonLocalResult.mMeasurement.GetOriginatorLocationWCS(origLocWCS);
         UtMeasurementUtil::LocationWCS(origLocWCS,
                                        aNonLocalResult.mMeasurement.GetRange(),
                                        aNonLocalResult.mMeasurement.GetBearing(),
                                        aNonLocalResult.mMeasurement.GetElevation(),
                                        locationWCS);
      }
      // aLocalTrack.Set3D(true);
      updateLocation = true;
   }

   bool localResultIs3D = aLocalResult.mMeasurement.LocationValid() ||
                          (aLocalResult.mMeasurement.RangeValid() && aLocalResult.mMeasurement.BearingValid() &&
                           aLocalResult.mMeasurement.ElevationValid());

   if (updateLocation)
   {
      // Defer updating the location until later.
   }
   else if (localResultIs3D)
   {
      // Don't update a 3d track with non-3d information unless the quality is at least as good.
      // TODO updateLocation = UpdateHasSufficientTrackQuality(aNonLocalTrack, aLocalTrack);

      if (updateLocation)
      {
         // There is accurate sensor-specific information (range, bearing, elevation)
         // that should be fused with the local track's location to provide a better estimate.
         // Handle range, bearing, and elevation components from the source track.
         if (aNonLocalResult.mMeasurement.RangeValid() || aNonLocalResult.mMeasurement.BearingValid() ||
             aNonLocalResult.mMeasurement.ElevationValid())
         {
            // If no covariance matrix is provided from the non-local track,
            // and there is no covariance from a filter, attempt to create one.
            bool doCreateCovariance = ((covariancePtr == nullptr) && aNonLocalResult.mMeasurement.RangeValid() &&
                                       aNonLocalResult.mMeasurement.BearingValid());
            FuseLocationWithRangeBearingElevation(aLocalResult.mMeasurement,
                                                  aNonLocalResult.mMeasurement,
                                                  doCreateCovariance,
                                                  locationWCS,
                                                  covariancePtr);
         }
         else if (aNonLocalResult.mMeasurement.LocationValid()) // Also we know that it's 2D if we are here and location is valid.
         {
            // We have a 2d location track with no r,b,e.  This situation can happen if we pass r,b tracks around without fusion.
            FuseLocationWith2D_Location(aLocalResult.mMeasurement, aNonLocalResult.mMeasurement, locationWCS);
            updateLocation = true;
         }
         else
         {
            updateLocation = false;
         }
      }
   }
   else if (aLocalResult.mMeasurement.RangeValid() && aLocalResult.mMeasurement.BearingValid() &&
            aNonLocalResult.mMeasurement.ElevationValid() && // and not a 3-D track
            (!aNonLocalResult.mMeasurement.BearingValid()))
   {
      bool doCreateCovariance = false; // Don't create a covariance matrix on an elevation update.
      FuseRangeBearingWithElevation(aLocalResult.mMeasurement,
                                    aNonLocalResult.mMeasurement,
                                    doCreateCovariance,
                                    locationWCS,
                                    covariancePtr);
      // aLocalResult.Set3D(true);
      updateLocation = true;
      aLocalResult.mMeasurement.SetUpdateTime(aNonLocalResult.mMeasurement.GetUpdateTime());
   }
   else if (aNonLocalResult.mMeasurement.RangeValid() && aNonLocalResult.mMeasurement.BearingValid() &&
            aLocalResult.mMeasurement.ElevationValid()) // the symmetric case of the one above
   {
      bool doCreateCovariance = (covariancePtr == nullptr); // and valid range-bearing in the track update.
      FuseRangeBearingWithElevation(aNonLocalResult.mMeasurement,
                                    aLocalResult.mMeasurement,
                                    doCreateCovariance,
                                    locationWCS,
                                    covariancePtr);
      updateLocation = true;
      // aLocalResult.Set3D(true);
      aLocalResult.mMeasurement.SetUpdateTime(aNonLocalResult.mMeasurement.GetUpdateTime());
   }
   else if (aNonLocalResult.mMeasurement.RangeValid() && aNonLocalResult.mMeasurement.BearingValid())
   {
      //! The result will be marked as non-3d to prevent weighted-average fusion
      //! (the covariance matrix cannot be computed from sensor errors).
      FuseRangeBearing(aLocalResult.mMeasurement, aNonLocalResult.mMeasurement, locationWCS);
      // aLocalResult.Set3D(false); // This is not a true 3D solution
      localResultUpdated = true;
      updateLocation     = true; // Set the calculated location later.
   }
   else if ((!aLocalResult.mMeasurement.BearingValid()) &&
            aNonLocalResult.mMeasurement.ElevationValid()) // and location not valid
   {
      // Save the elevation and/or range for later processing.
      aLocalResult.mMeasurement.SetElevation(aNonLocalResult.mMeasurement.GetElevation());
      aLocalResult.mMeasurement.SetElevationError(aNonLocalResult.mMeasurement.GetElevationError());
      if (aNonLocalResult.mMeasurement.RangeValid())
      {
         aLocalResult.mMeasurement.SetRange(aNonLocalResult.mMeasurement.GetRange());
         aLocalResult.mMeasurement.SetRangeError(aNonLocalResult.mMeasurement.GetRangeError());
      }
      double originatorLocWCS[3];
      aNonLocalResult.mMeasurement.GetOriginatorLocationWCS(originatorLocWCS);
      aLocalResult.mMeasurement.SetOriginatorLocationWCS(originatorLocWCS);
      localResultUpdated = true;
   }
   if ((!updateLocation) && (!aNonLocalResult.mMeasurement.RangeValid()) && aNonLocalResult.mMeasurement.BearingValid())
   {
      // Save valid bearing and elevation data for later processing.
      aLocalResult.mMeasurement.SetBearingValid(true);
      aLocalResult.mMeasurement.SetBearing(aNonLocalResult.mMeasurement.GetBearing());
      aLocalResult.mMeasurement.SetBearingError(aNonLocalResult.mMeasurement.GetBearingError());
      double originatorLocWCS[3];
      aNonLocalResult.mMeasurement.GetOriginatorLocationWCS(originatorLocWCS);
      aLocalResult.mMeasurement.SetOriginatorLocationWCS(originatorLocWCS);
      if (aNonLocalResult.mMeasurement.ElevationValid())
      {
         aLocalResult.mMeasurement.SetElevationValid(true);
         aLocalResult.mMeasurement.SetElevation(aNonLocalResult.mMeasurement.GetElevation());
         aLocalResult.mMeasurement.SetElevationError(aNonLocalResult.mMeasurement.GetElevationError());
      }
      localResultUpdated = true;
   }

   if (updateLocation)
   {
      // It is possible that the local track must first be updated to the time of the nonlocal track,
      // (call to KinematicUpdate in UpdateLocation), then updated again to the current simulation time
      // (call to KinematicUpdate, below).  This situation results in the update count incremented twice.
      // Because the intent is to show only one increment in the update count, we control it manually.
      // int updateCount = aLocalResult.mMeasurement.GetUpdateCount();
      UpdateLocation(aLocalResult.mMeasurement,
                     aNonLocalResult.mMeasurement,
                     locationWCS,
                     covariancePtr,
                     aLocalResult.mMeasurementCount);
      localResultUpdated = true;

      if (localResultIs3D)
      {
         // Set the originating location to be the current location of the host platform.
         // We are about to update a platform based on a perceived time.
         // This time should never be greater than the current simulation time.
         GetPlatform()->Update(aSimTime);
         double reportingLocationWCS[3];
         GetPlatform()->GetLocationWCS(reportingLocationWCS);
         aLocalResult.mMeasurement.SetOriginatorLocationWCS(reportingLocationWCS);
         double WCS_ToNED_Transform[3][3];
         GetPlatform()->GetWCSToNEDTransform(WCS_ToNED_Transform);
         aLocalResult.mMeasurement.SetOriginatorTransformWCS(WCS_ToNED_Transform);

         // Invalidate range, bearing, elevation.
         aLocalResult.mMeasurement.SetRangeValid(false);
         aLocalResult.mMeasurement.SetBearingValid(false);
         aLocalResult.mMeasurement.SetElevationValid(false);
      }

      // Finally, perform a kinematic update of the local track to the current simulation time.
      aLocalResult.mMeasurement.KinematicUpdate(aSimTime);

      // TODO ++updateCount;
      // TODO aLocalResult.mMeasurement.SetUpdateCount(updateCount); // See above comment concerning the update count.
   }
   else if (localResultUpdated)
   {
      aLocalResult.mMeasurement.Update(aSimTime);
   }

   // Note: We assume the track quality is associated with kinematic data,
   // so call this method with every track update.
   // TODO SKIP localResultUpdated |= UpdateNonKinematicData(aSimTime, GetTrackManager(), aLocalResult, aNonLocalResult);

   if (covariancePtr != nullptr)
   {
      delete covariancePtr;
   }

   // Copy the residual covariance if one exists in the nonlocal track.
   // TODO if (aNonLocalResult.mMeasurement.GetResidualCovariance() != 0)
   // TODO {
   // TODO    aLocalTrack.SetResidualCovariance(*aNonLocalResult.mMeasurement.GetResidualCovariance());
   // TODO }

   // If the local track was updated then it was because of an update from some track other than the mirror source,
   // indicate the track will no longer be acting as a mirror.
   if (localResultUpdated)
   {
      // TODO aLocalResult.ClearMirrorResult();
   }
   return localResultUpdated;
}

// =================================================================================================
//! private
//! Given a location estimate, update a local track with that estimate.
//! If weighted average fusion is specified, the location will be updated based on
//! the weighted average of the local track's covariance matrix and the
//! given covariance matrix, associated with the location estimate.
void WsfDefaultFusion::UpdateLocation(WsfMeasurement&       aLocalMeasurement,
                                      const WsfMeasurement& aNonLocalMeasurement,
                                      const double          aLocationWCS[3],
                                      WsfCovariance*&       aStateCovarianceMatrixPtr,
                                      unsigned int          aFusedCount)
{
   if (DebugEnabled())
   {
      double lat, lon, alt;
      WsfPlatform::ConvertWCSToLLA(aLocationWCS, lat, lon, alt);
      auto out = ut::log::debug() << "WsfDefaultFusion: UpdateLocation called.";
      out.AddNote() << "UpdateTime: " << aNonLocalMeasurement.GetUpdateTime();
      out.AddNote() << "LLA: " << lat << ":" << lon << ":" << alt;
   }

   // Velocity replacement, if applicable.
   if (aNonLocalMeasurement.VelocityValid()) // and "updateLocation" is true
   {
      double velocityWCS[3];
      aNonLocalMeasurement.GetVelocityWCS(velocityWCS);
      aLocalMeasurement.SetVelocityWCS(velocityWCS);
      aLocalMeasurement.SetVelocityValid(true);
   }

   bool locationUpdated = false;

   if (mIsWeightedAverage)
   {
      bool updateCovarianceMatrix = (aStateCovarianceMatrixPtr != nullptr);

      if ((!updateCovarianceMatrix) && aNonLocalMeasurement.MeasurementCovarianceValid())
      {
         aStateCovarianceMatrixPtr = new WsfCovariance(*aNonLocalMeasurement.GetMeasurementCovariance());
         updateCovarianceMatrix    = true;
      }

      // Does the local track have a covariance matrix?
      if (updateCovarianceMatrix && (aLocalMeasurement.GetStateCovariance() == nullptr))
      {
         // Use the source track covariance
         aLocalMeasurement.SetStateCovariance(GetSimulation()->GetSimTime(), *aStateCovarianceMatrixPtr);
         aLocalMeasurement.SetStateCovarianceValid(true);

         // Skip weighted fusion this pass.
         updateCovarianceMatrix = false;
      }

      // For the weighted-average case, 3D location needs to be valid,
      // and there should be more than one raw measurement being fused
      // (otherwise, this becomes a simple filter).
      updateCovarianceMatrix &= (aFusedCount > 1);
      if (updateCovarianceMatrix && aLocalMeasurement.LocationValid()) // Location will be invalid on 1st pass
      {
         assert(aStateCovarianceMatrixPtr != nullptr);
         assert(aLocalMeasurement.GetStateCovariance() != nullptr);

         // First we need to synchronize the local track to the update time of the source track.
         aLocalMeasurement.KinematicUpdate(aNonLocalMeasurement.GetUpdateTime());
         assert(aLocalMeasurement.GetStateCovariance() != nullptr);

         UtMatrixd measurement(3);
         measurement.CopyFrom(aLocationWCS);

         // We have an opportunity to do weighted average fusion on both position and velocity, but only if we have
         // velocity estimate and full 6x6 covariance matrices from both tracks.
         if (aNonLocalMeasurement.VelocityValid() && (aStateCovarianceMatrixPtr->GetSize() == 36) &&
             (aLocalMeasurement.GetStateCovariance()->GetSize() == 36))
         {
            // Include velocity in the weighted average fusion
            measurement.SetSize(6, 1);
            double velWCS[3];
            aNonLocalMeasurement.GetVelocityWCS(velWCS);
            measurement.CopyFrom(velWCS, 3, 3);
         }
         else
         {
            // Otherwise, only worry about position.
            aStateCovarianceMatrixPtr->ResizeToOrder(3);
         }

         // Now update the track state estimate with a weighted-average based on the covariances.
         UpdateTrackStateEstimate(measurement, *aStateCovarianceMatrixPtr, aLocalMeasurement);
         locationUpdated = true;
      }
   }

   if (!locationUpdated)
   {
      // Simple location replacement.
      aLocalMeasurement.SetLocationWCS(aLocationWCS);
      aLocalMeasurement.SetWCS_LocationValid(true);
      aLocalMeasurement.Update(aNonLocalMeasurement.GetUpdateTime());
      if (aStateCovarianceMatrixPtr != nullptr)
      {
         // Simple replacement means we use the new values, regardless of quality...
         aLocalMeasurement.SetStateCovariance(GetSimulation()->GetSimTime(), *aStateCovarianceMatrixPtr);
         aLocalMeasurement.SetStateCovarianceValid(true);
      }
   }

   aLocalMeasurement.SetWCS_LocationValid(true);
}

// =================================================================================================
//! Merge range and bearing data for a local measurement that does not have a 3d solution.
//! The range and bearing will be kept, in case they will be merged with elevation (height-finder) data.
//! Otherwise a pseudo 3D location will be computed and used for subsequent fusion.
void WsfDefaultFusion::FuseRangeBearing(WsfMeasurement&       aFusedLocationMeasurement,
                                        const WsfMeasurement& aRangeBearingMeasurement,
                                        double                aLocationWCS[3])
{
   aFusedLocationMeasurement.SetRange(aRangeBearingMeasurement.GetRange());
   aFusedLocationMeasurement.SetRangeError(aRangeBearingMeasurement.GetRangeError());
   aFusedLocationMeasurement.SetRangeValid(true);
   aFusedLocationMeasurement.SetBearing(aRangeBearingMeasurement.GetBearing());
   aFusedLocationMeasurement.SetBearingError(aRangeBearingMeasurement.GetBearingError());
   aFusedLocationMeasurement.SetBearingValid(true);
   assert(aFusedLocationMeasurement.ElevationValid() == false);
   double originatorLocWCS[3];
   aRangeBearingMeasurement.GetOriginatorLocationWCS(originatorLocWCS);
   aFusedLocationMeasurement.SetOriginatorLocationWCS(originatorLocWCS);

   // Estimate the elevation from the range-bearing track,
   // Then use it to estimate the WCS location.
   aRangeBearingMeasurement.GetExtrapolatedLocationWCS(aRangeBearingMeasurement.GetUpdateTime(), aLocationWCS);
}

// =================================================================================================
//! For a given measurement with 3D location, fuse arbitrary range, bearing, and/or elevation
//! information from another track.
void WsfDefaultFusion::FuseLocationWithRangeBearingElevation(WsfMeasurement&       aFusedLocationMeasurement,
                                                             const WsfMeasurement& aRangeBearingElevationMeasurement,
                                                             bool                  aCreateMeasurementCovariance,
                                                             double                aLocationWCS[3],
                                                             WsfCovariance*&       aStateCovarianceMatrixPtr)
{
   double rbeOriginLocationWCS[3];
   aRangeBearingElevationMeasurement.GetOriginatorLocationWCS(rbeOriginLocationWCS);

   // Find the extrapolated location of the local track at the measurement time of the non-local measurement.
   aFusedLocationMeasurement.GetExtrapolatedLocationWCS(aRangeBearingElevationMeasurement.GetUpdateTime(), aLocationWCS);
   double range, bearing, elevation;

   // Find the range, bearing, and elevation using existing data; then
   // use them to merge new data.
   UtMeasurementUtil::RangeBearingElevation(rbeOriginLocationWCS, aLocationWCS, range, bearing, elevation);

   if (aRangeBearingElevationMeasurement.RangeValid())
   {
      range = aRangeBearingElevationMeasurement.GetRange();
      // Set the error in the track, in case a covariance needs to be created later.
      aFusedLocationMeasurement.SetRangeError(aRangeBearingElevationMeasurement.GetRangeError());
   }
   if (aRangeBearingElevationMeasurement.BearingValid())
   {
      bearing = aRangeBearingElevationMeasurement.GetBearing();
      // Set the error in the measurement, in case a covariance needs to be created later.
      aFusedLocationMeasurement.SetBearingError(aRangeBearingElevationMeasurement.GetBearingError());
   }
   if (aRangeBearingElevationMeasurement.ElevationValid())
   {
      elevation = aRangeBearingElevationMeasurement.GetElevation();
      // Set the error in the measurement, in case another covariance needs to be created later.
      aFusedLocationMeasurement.SetElevationError(aRangeBearingElevationMeasurement.GetElevationError());
   }
   UtMeasurementUtil::LocationWCS(rbeOriginLocationWCS, range, bearing, elevation, aLocationWCS);

   // If no covariance matrix is provided from the non-local measurement,
   // and there is no covariance from a filter, attempt to create one.
   if (aCreateMeasurementCovariance)
   {
      // Approximately, these sensors should be co-located;
      // Otherwise, it's inappropriate to do this kind of fusion.
      // Impose the limit that measured range > 10x baseline distance between sensors.
      double sensorLocationVectorWCS[3];
      double locationOriginLocationWCS[3]; // he origin location of the track with valid 3-d location.
      aFusedLocationMeasurement.GetOriginatorLocationWCS(locationOriginLocationWCS);
      UtVec3d::Subtract(sensorLocationVectorWCS, locationOriginLocationWCS, rbeOriginLocationWCS);
      if (UtVec3d::MagnitudeSquared(sensorLocationVectorWCS) < (100.0 * range * range)) // (10.0 * range)^2
      {
         double rangeError = aRangeBearingElevationMeasurement.GetRangeError();

         double bearingError = aFusedLocationMeasurement.GetBearingError();
         if (aRangeBearingElevationMeasurement.BearingValid())
         {
            bearingError = aRangeBearingElevationMeasurement.GetBearingError();
         }

         double elevationError = aFusedLocationMeasurement.GetElevationError();
         if (aRangeBearingElevationMeasurement.ElevationValid())
         {
            elevationError = aRangeBearingElevationMeasurement.GetElevationError();
         }

         aStateCovarianceMatrixPtr = WsfCovariance::CreateMeasurementCovarianceWCS(range,
                                                                                   rangeError,
                                                                                   bearing,
                                                                                   bearingError,
                                                                                   elevation,
                                                                                   elevationError,
                                                                                   rbeOriginLocationWCS);
      }
   }
}

// =================================================================================================
//! For a given track with 3D location, fuse a measurement with 2D information contained
//! in a valid location (range, bearing, and elevation are invalid).
//! This fusion simply combines the range and bearing extracted from the 2D track
//! with elevation from the 3D measurement.
//! Note that this becomes more problematic the further apart the originator locations
//! are from each other, due to the curvature of the earth.
void WsfDefaultFusion::FuseLocationWith2D_Location(WsfMeasurement&       aFusedLocationMeasurement,
                                                   const WsfMeasurement& a2D_LocationMeasurement,
                                                   double                aLocationWCS[3])
{
   double originWCS[3];
   aFusedLocationMeasurement.GetOriginatorLocationWCS(originWCS);

   double range, bearing, elevation;
   aFusedLocationMeasurement.GetExtrapolatedLocationWCS(a2D_LocationMeasurement.GetUpdateTime(), aLocationWCS);
   UtMeasurementUtil::RangeBearingElevation(originWCS, aLocationWCS, range, bearing, elevation);

   double r2d, b2d, e2d;
   a2D_LocationMeasurement.GetLocationWCS(aLocationWCS);
   UtMeasurementUtil::RangeBearingElevation(originWCS, aLocationWCS, r2d, b2d, e2d);

   UtMeasurementUtil::LocationWCS(originWCS, r2d, b2d, elevation, aLocationWCS);
}

// =================================================================================================
//! Merge new elevation data with existing range & bearing in the local measurement.
//! This type of fusion is performed typically when a height-finding radar
//! provides elevation data to an existing range-bearing measurement from another radar.
void WsfDefaultFusion::FuseRangeBearingWithElevation(const WsfMeasurement& aRangeBearingMeasurement,
                                                     const WsfMeasurement& aElevationMeasurement,
                                                     bool                  aCreateMeasurementCovariance,
                                                     double                aLocationWCS[3],
                                                     WsfCovariance*&       aStateCovarianceMatrixPtr)
{
   double sourceLocationWCS[3];

   // Merge existing range and bearing with elevation.
   aElevationMeasurement.GetOriginatorLocationWCS(sourceLocationWCS);

   double trackLocationWCS[3];

   // Call the GetLocationWCS of the measurement; it uses an estimate of the target altitude
   // to deter
   aRangeBearingMeasurement.GetExtrapolatedLocationWCS(aElevationMeasurement.GetUpdateTime(), trackLocationWCS);

   // Determine the NED location of the local position in the non-local frame.
   double lat, lon, alt;
   UtEntity::ConvertWCSToLLA(sourceLocationWCS, lat, lon, alt);

   double WCS_ToNED_Transform[3][3];
   UtEllipsoidalEarth::ComputeNEDTransform(lat, lon, alt, WCS_ToNED_Transform, sourceLocationWCS);

   double trackLocationNED[3];
   UtEllipsoidalEarth::ConvertECEFToLocal(sourceLocationWCS, WCS_ToNED_Transform, trackLocationWCS, trackLocationNED);

   // Determine the N/E planar range and then adjust D to account for the reported elevation.
   double range2d      = sqrt(trackLocationNED[0] * trackLocationNED[0] + trackLocationNED[1] * trackLocationNED[1]);
   range2d             = std::max(range2d, 1.0);
   trackLocationNED[2] = -range2d * tan(aElevationMeasurement.GetElevation());

   // Convert the NED location (relative to the non-local measurement) back to WCS and store it
   // in the local measurement.
   UtEllipsoidalEarth::ConvertLocalToECEF(sourceLocationWCS, WCS_ToNED_Transform, trackLocationNED, aLocationWCS);

   // Approximately, these sensors should be co-located.
   // Otherwise, it's inappropriate to do this kind of fusion.
   if (aCreateMeasurementCovariance)
   {
      aStateCovarianceMatrixPtr =
         WsfCovariance::CreateMeasurementCovarianceWCS(aRangeBearingMeasurement.GetRange(),
                                                       aRangeBearingMeasurement.GetRangeError(),
                                                       aRangeBearingMeasurement.GetBearing(),
                                                       aRangeBearingMeasurement.GetBearingError(),
                                                       aElevationMeasurement.GetElevation(),
                                                       aElevationMeasurement.GetElevationError(),
                                                       sourceLocationWCS);
   }
}

// =================================================================================================
//! A general-purpose method for updating the "non-kinematic" fields of the track
//! (frequency, side, type, iff, track quality).
//! TODO At some point we may want to break these out as individual, script-able methods,
//! but *only* for default fusion.
bool WsfDefaultFusion::UpdateNonKinematicData(double aSimTime, WsfLocalTrack& aLocalTrack, const WsfTrack& aNonLocalTrack)
{
   // Now merge 'scalar' data.  Update side, type, IFF status, signal-to-noise
   // signals, spatial domain, track quality and aux data.
   return WsfFusionStrategy::UpdateNonKinematicData(aSimTime, aLocalTrack, aNonLocalTrack);
}

WsfDefaultFusion::WsfDefaultFusion(const WsfDefaultFusion& aSrc)
   : WsfFusionStrategy(aSrc)
   , mIsWeightedAverage(aSrc.mIsWeightedAverage)
   , mReferencePoint(aSrc.mReferencePoint)
   , mContextPtr(ut::make_unique<WsfScriptContext>(*aSrc.mContextPtr))
   , mUpdateTypePtr(nullptr)
   , mUpdateSidePtr(nullptr)
   , mUpdateIFF_StatusPtr(nullptr)
   , mLocalTrackVarPtr(nullptr)
   , mNonLocalTrackVarPtr(nullptr)
{
}

// =================================================================================================
// static
//! Determine whether IFF values are either all "friend" or "foe;"
//! If so, set the value as the current IFF value.
void WsfDefaultFusion::ReviewIFF(WsfTrackManager& aTrackManager, WsfLocalTrack& aLocalTrack)
{
   // Don't review if we think we know what the IFF status is
   WsfTrack::IFF_Status currentIFF_Status = aLocalTrack.GetIFF_Status();
   if ((currentIFF_Status == WsfTrack::cIFF_AMBIGUOUS) || (currentIFF_Status == WsfTrack::cIFF_UNKNOWN))
   {
      unsigned int numTracks = aLocalTrack.GetRawTrackIds().GetCount();

      if (numTracks > 0)
      {
         WsfTrack*    rawTrackPtr = aTrackManager.GetRawTrack(&aLocalTrack, 0);
         int          iff         = rawTrackPtr->GetIFF_Status(); // this must be the same as all other raw values.
         unsigned int trackNum    = 1;
         bool         isAmbiguous = false;

         while ((!isAmbiguous) && (trackNum < numTracks))
         {
            rawTrackPtr = aTrackManager.GetRawTrack(&aLocalTrack, trackNum);
            int testIFF = rawTrackPtr->GetIFF_Status();
            isAmbiguous = ((iff != testIFF) || ((testIFF != WsfTrack::cIFF_FRIEND) && (testIFF != WsfTrack::cIFF_FOE)));
            ++trackNum;
         }

         if (!isAmbiguous) // Ambiguity was resolved.
         {
            aLocalTrack.SetIFF_Status(static_cast<WsfTrack::IFF_Status>(iff));
         }
      }
   }
}

// =================================================================================================
// static
//! Determine the best track quality among a local track's raw tracks,
//! and replace it as the local track's quality indicator.
void WsfDefaultFusion::ReviewTrackQuality(WsfTrackManager& aTrackManager, WsfLocalTrack& aLocalTrack)
{
   unsigned int numTracks = aLocalTrack.GetRawTrackIds().GetCount();
   double       quality   = 0.0;

   if (numTracks > 0)
   {
      unsigned int trackNum = 0;
      WsfTrackId   trackId;
      while (trackNum < numTracks)
      {
         WsfTrack* rawTrackPtr = aTrackManager.GetRawTrack(&aLocalTrack, trackNum);
         if (rawTrackPtr->GetTrackQuality() > quality)
         {
            quality = rawTrackPtr->GetTrackQuality();
            trackId = rawTrackPtr->GetTrackId();
         }
         ++trackNum;
      }
      aLocalTrack.SetHighestTrackQualityId(trackId);
   }
   else // No raw tracks left.
   {
      WsfTrackId null;
      aLocalTrack.SetHighestTrackQualityId(null);
   }

   aLocalTrack.SetTrackQuality(quality); // Set to zero if no other correlated tracks.
}

// =================================================================================================
//! Update the track's state estimate.  If a covariance matrix is available for the local and
//! non-local tracks, these will be used.  Otherwise a measurement covariance will be calculated based on
//! available sensor errors (unless sensor errors are not available, in which case a simple replacement
//! scheme will be used).  The covariances for the local and non-local track will be fused using a
//! weighted-average method.
//! @param aSimTime The time to which the track is to be updated.
//! @param aLocationWCS An updated WCS location to be merged with the existing track.
//! @param aCovarianceMatrix The covariance matrix, if any, of the measurement to be merged.
//! @param aTrack The local track whose state estimate is to be updated with the state and covariance data.
void WsfDefaultFusion::UpdateTrackStateEstimate(const UtMatrixd&     aStateEstimate,
                                                const WsfCovariance& aStateCovarianceMatrix,
                                                WsfMeasurement&      aMeasurement)
{
   // Fuse the existing covariance with the one we just created.
   unsigned int         size       = aStateEstimate.GetSize();
   const WsfCovariance& trackCovar = *(aMeasurement.GetStateCovariance());
   UtMatrixd            currentEstimate(size);
   double               currentLocationWCS[3];

   aMeasurement.GetLocationWCS(currentLocationWCS); // No extrapolation necessary
   currentEstimate.CopyFrom(currentLocationWCS, 3, 0);
   WsfCovariance currentCovar(size, size);
   currentCovar = trackCovar;

   if (size == 6) // measurement includes a velocity estimate
   {
      assert(aMeasurement.VelocityValid());
      // Copy track's velocity into the current measurement
      double currentVelWCS[3];
      aMeasurement.GetVelocityWCS(currentVelWCS);
      currentEstimate.CopyFrom(currentVelWCS, 3, 3);
   }
   else
   {
      // Need to make sure the current covariance is properly sized.
      assert(size == 3);
      currentCovar.ResizeToOrder(3);
   }

   UtMatrixd     fusedEstimate(size);
   WsfCovariance fusedCovar(size, size);
   FuseEstimates(currentEstimate, aStateEstimate, currentCovar, aStateCovarianceMatrix, fusedEstimate, fusedCovar);

   aMeasurement.SetStateCovariance(GetSimulation()->GetSimTime(), fusedCovar);
   SetEstimateInTrack(fusedEstimate, aMeasurement);
}

// =================================================================================================
// static
void WsfDefaultFusion::GetEstimateFromTrack(double aSimTime, const WsfMeasurement& aMeasurement, UtMatrixd& aEstimate)
{
   double currentLocationWCS[3];
   aMeasurement.GetExtrapolatedLocationWCS(aSimTime, currentLocationWCS);
   aEstimate.CopyFrom(currentLocationWCS, 3, 0);

   if (aMeasurement.VelocityValid())
   {
      double currentVelocityWCS[3] = {0.0, 0.0, 0.0};
      aMeasurement.GetVelocityWCS(currentVelocityWCS);
      aEstimate.CopyFrom(currentVelocityWCS, 3, 3);
   }
}

// =================================================================================================
// static
void WsfDefaultFusion::SetEstimateInTrack(UtMatrixd& aEstimate, WsfMeasurement& aMeasurement)
{
   double locationWCS[3];
   aEstimate.CopyTo(locationWCS, 3, 0);
   aMeasurement.SetLocationWCS(locationWCS);
   aMeasurement.SetWCS_LocationValid(true);
   if (aEstimate.GetSize() == 6)
   {
      double velocityWCS[3];
      aEstimate.CopyTo(velocityWCS, 3, 3);
      aMeasurement.SetVelocityWCS(velocityWCS);
      aMeasurement.SetVelocityValid(true);
   }
}

// =================================================================================================
// private
void WsfDefaultFusion::WeightedAverageFusion1D(double  aMeasurement1,
                                               double  aMeasurement2,
                                               double  aError1,
                                               double  aError2,
                                               double& aWeightedMeasurement,
                                               double& aWeightedError)
{
   double divisor       = aError1 + aError2;
   aWeightedMeasurement = aMeasurement1 * aError2 / divisor + aMeasurement2 * aError1 / divisor;
   aWeightedError       = aError1 * aError2 / divisor;
}

bool WsfDefaultFusion::UpdateHasSufficientTrackQuality(const WsfTrack& aTrackUpdate, WsfLocalTrack& aLocalTrack)
{
   if (aLocalTrack.GetHighestTrackQualityId().IsNull())
   {
      // This may execute in certain situations
      ReviewTrackQuality(*static_cast<WsfTrackManager*>(aLocalTrack.GetTrackManager()), aLocalTrack);
   }
   return ((aTrackUpdate.GetTrackQuality() >= aLocalTrack.GetTrackQuality()) ||
           ((!aLocalTrack.GetHighestTrackQualityId().IsNull()) &&
            (aLocalTrack.GetHighestTrackQualityId() == aTrackUpdate.GetTrackId())));
}

// virtual
bool WsfDefaultFusion::UpdateIFF_Status(double aSimTime, WsfLocalTrack& aLocalTrack, const WsfTrack& aNonLocalTrack)
{
   bool updated = false;
   if (aNonLocalTrack.GetIFF_Status() != WsfTrack::cIFF_UNKNOWN)
   {
      updated = true;

      // TODO this should be script-able based on Rules of Engagement (ROE).
      // Current implementation is to report "ambiguous" if all reports do not agree.
      WsfTrack::IFF_Status nonLocalIFF = aNonLocalTrack.GetIFF_Status();
      WsfTrack::IFF_Status localIFF    = aLocalTrack.GetIFF_Status();

      bool nonLocalFOF = ((nonLocalIFF == WsfTrack::cIFF_FRIEND) || (nonLocalIFF == WsfTrack::cIFF_FOE));
      bool localFOF    = ((localIFF == WsfTrack::cIFF_FRIEND) || (localIFF == WsfTrack::cIFF_FOE));

      if (localIFF == WsfTrack::cIFF_UNKNOWN)
      {
         aLocalTrack.SetIFF_Status(nonLocalIFF);
      }
      else if (nonLocalFOF)
      {
         if ((localFOF) && (nonLocalIFF != localIFF))
         {
            aLocalTrack.SetIFF_Status(WsfTrack::cIFF_AMBIGUOUS);
         }
         // Local track iff should be "ambiguous" in order to be here in the logic.
         else if ((localIFF == WsfTrack::cIFF_AMBIGUOUS) && (nonLocalIFF != WsfTrack::cIFF_AMBIGUOUS))
         {
            // Does this update resolve the ambiguity?
            // Must examine our raw track inputs to find out.
            ReviewIFF(GetTrackManager(), aLocalTrack);
         }
      }
      else if (nonLocalIFF == WsfTrack::cIFF_AMBIGUOUS)
      {
         aLocalTrack.SetIFF_Status(WsfTrack::cIFF_AMBIGUOUS);
      }
   }
   return updated;
}

// virtual
bool WsfDefaultFusion::UpdateSide(double aSimTime, WsfLocalTrack& aLocalTrack, const WsfTrack& aNonLocalTrack)
{
   bool updated = false;
   if (aNonLocalTrack.SideIdValid() &&
       ((!aLocalTrack.SideIdValid()) || UpdateHasSufficientTrackQuality(aNonLocalTrack, aLocalTrack)))
   {
      aLocalTrack.SetSideId(aNonLocalTrack.GetSideId());
      aLocalTrack.SetSideIdValid(true);
      updated = true;
   }
   return updated;
}

// virtual
bool WsfDefaultFusion::UpdateType(double aSimTime, WsfLocalTrack& aLocalTrack, const WsfTrack& aNonLocalTrack)
{
   bool updated = false;
   // If we defined a script to implement a custom behavior, run it.
   if (mUpdateTypePtr != nullptr)
   {
      mNonLocalTrackVarPtr->SetPointer(new UtScriptRef(&aLocalTrack, mContextPtr->GetClass("WsfLocalTrack")));
      mLocalTrackVarPtr->SetPointer(
         new UtScriptRef(const_cast<WsfTrack*>(&aNonLocalTrack), mContextPtr->GetClass("WsfTrack")));
      mContextPtr->ExecuteScript(aSimTime, mUpdateTypePtr);
   }
   else // Replace the type id(s) in the local track.
   {
      // Todo Determine whether the track quality check below should be removed.
      if (aNonLocalTrack.TypeIdsValid() &&
          ((!aLocalTrack.TypeIdsValid()) || UpdateHasSufficientTrackQuality(aNonLocalTrack, aLocalTrack)))
      {
         aLocalTrack.SetTypeIds(aNonLocalTrack.GetTypeIds());
         aLocalTrack.SetTypeIdsValid(true);
         updated = true;
      }
   }
   return updated;
}

// virtual
bool WsfDefaultFusion::UpdateSpatialDomain(double aSimTime, WsfLocalTrack& aLocalTrack, const WsfTrack& aNonLocalTrack)
{
   bool updated = false;
   if (aNonLocalTrack.GetSpatialDomain() != WsfSpatialDomain::WSF_SPATIAL_DOMAIN_UNKNOWN)
   {
      aLocalTrack.SetSpatialDomain(aNonLocalTrack.GetSpatialDomain());
      updated = true;
   }
   return updated;
}

bool WsfDefaultFusion::UpdateTrackQuality(double aSimTime, WsfLocalTrack& aLocalTrack, const WsfTrack& aNonLocalTrack)
{
   bool updateHasSufficientQuality = UpdateHasSufficientTrackQuality(aNonLocalTrack, aLocalTrack);
   if (updateHasSufficientQuality)
   {
      aLocalTrack.SetTrackQuality(aNonLocalTrack.GetTrackQuality());
      aLocalTrack.SetHighestTrackQualityId(aNonLocalTrack.GetTrackId());
   }
   return updateHasSufficientQuality;
}

bool WsfDefaultFusion::UpdateSignalToNoise(double aSimTime, WsfLocalTrack& aLocalTrack, const WsfTrack& aNonLocalTrack)
{
   bool updateHasSufficientQuality = UpdateHasSufficientTrackQuality(aNonLocalTrack, aLocalTrack);
   if (aNonLocalTrack.SignalToNoiseValid() && ((!aLocalTrack.SignalToNoiseValid()) || updateHasSufficientQuality))
   {
      aLocalTrack.SetSignalToNoise(aNonLocalTrack.GetSignalToNoise());
   }
   return updateHasSufficientQuality;
}

bool WsfDefaultFusion::UpdateSignalList(double aSimTime, WsfLocalTrack& aLocalTrack, const WsfTrack& aNonLocalTrack)
{
   bool updated = false;
   // Is there a signal list in the non-local track?
   // If so, merge with the signal list in the local track.
   if (aNonLocalTrack.GetSignalList() != nullptr)
   {
      aLocalTrack.MergeSignalLists(aNonLocalTrack);
      updated = true;
   }

   aLocalTrack.UpdateSignalList();

   return updated;
}

bool WsfDefaultFusion::UpdateAuxData(double aSimTime, WsfLocalTrack& aLocalTrack, const WsfTrack& aNonLocalTrack)
{
   // Update the aux_data in the local track with the aux_data from the non-local track.
   // Default is to use the track manager's aux data fusion rules.
   return GetTrackManager().UpdateAuxData(aSimTime, aLocalTrack, aNonLocalTrack);
}

void WsfDefaultFusion::PerformMirrorTrackUpdate(double aSimTime, WsfLocalTrack& aLocalTrack, const WsfTrack& aNonLocalTrack)
{
   // First check to see if the local track is just a mirror for the non-local track, and if so
   // perform simple replacement. This ensures that 'mirror' tracks reflects EXACTLY (except for
   // a identifying fields) the underlying non-local track.
   //
   // This special processing will be employed if and only if ALL of the contributing updates for
   // the track came from the same non-local track (unchanging track ID). If the local track EVER
   // receives an update from another source this special processing will be disabled.
   //
   // Note that this *could* probably be relaxed for some cases. If a local track was a mirror for
   // a sensor track and the sensor dropped the track, the track would continue to exist before
   // being purged. If a subsequent update came in from the same sensor then it could be safely
   // assimilated if perfect correlation and replacement fusion were being used.
   // Perform direct replacement.
   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Platform replacing track from external originator.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Ext. Originator: " << aNonLocalTrack.GetOriginatorNameId();
   }

   aLocalTrack.ReplacementUpdate(aNonLocalTrack);

   // Aux data is not copied by the direct replacement because we still want to allow the user
   // to have local aux data. This causes aux data to go through our 'fusion' process.
   GetTrackManager().UpdateAuxData(aSimTime, aLocalTrack, aNonLocalTrack);
}

bool WsfDefaultFusion::PerformStandardTrackUpdate(double aSimTime, WsfLocalTrack& aLocalTrack, const WsfTrack& aNonLocalTrack)
{
   bool localTrackUpdated = false;

   // updateLocation refers to whether the locationWCS has been set
   // and should be copied/fused into the local track later.
   bool updateLocation = false;

   double         locationWCS[3];
   WsfCovariance* covariancePtr = nullptr;

   if (aNonLocalTrack.StateCovarianceValid())
   {
      assert(aNonLocalTrack.GetStateCovariance() != nullptr);
      // The covariancePtr has been set and should be copied/fused into the local track.
      covariancePtr = new WsfCovariance(*aNonLocalTrack.GetStateCovariance());
   }
   else if (aNonLocalTrack.MeasurementCovarianceValid())
   {
      assert(aNonLocalTrack.GetMeasurementCovariance() != nullptr);
      covariancePtr = new WsfCovariance(*aNonLocalTrack.GetMeasurementCovariance());
   }

   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Platform updating track from external originator.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Ext. Originator: " << aNonLocalTrack.GetOriginatorNameId();
   }

   if (aNonLocalTrack.Is3D() && // Either polar (RBE) or Cartesian 3-D location is valid.
       UpdateHasSufficientTrackQuality(aNonLocalTrack, aLocalTrack))
   {
      if (aNonLocalTrack.LocationValid())
      {
         // The WCS location is valid.
         // It may be fused, depending on track quality.
         aNonLocalTrack.GetLocationWCS(locationWCS);
      }
      else // valid 3-D polar location (RBE)
      {
         assert(aNonLocalTrack.ElevationValid() && aNonLocalTrack.BearingValid() && aNonLocalTrack.RangeValid());

         // retrieve the world coordinates of this RBE location.
         double origLocWCS[3];
         aNonLocalTrack.GetOriginatorLocationWCS(origLocWCS);
         UtMeasurementUtil::LocationWCS(origLocWCS,
                                        aNonLocalTrack.GetRange(),
                                        aNonLocalTrack.GetBearing(),
                                        aNonLocalTrack.GetElevation(),
                                        locationWCS);
      }
      aLocalTrack.Set3D(true);
      updateLocation = true;
   }

   if (updateLocation)
   {
      // Defer updating the location until later.
   }
   else if (aLocalTrack.Is3D())
   {
      // Don't update a 3d track with non-3d information unless the quality is at least as good.
      updateLocation = UpdateHasSufficientTrackQuality(aNonLocalTrack, aLocalTrack);

      if (updateLocation)
      {
         // There is accurate sensor-specific information (range, bearing, elevation)
         // that should be fused with the local track's location to provide a better estimate.
         // Handle range, bearing, and elevation components from the source track.
         if (aNonLocalTrack.RangeValid() || aNonLocalTrack.BearingValid() || aNonLocalTrack.ElevationValid())
         {
            // If no covariance matrix is provided from the non-local track,
            // and there is no covariance from a filter, attempt to create one.
            bool doCreateCovariance =
               ((covariancePtr == nullptr) && aNonLocalTrack.RangeValid() && aNonLocalTrack.BearingValid());
            FuseLocationWithRangeBearingElevation(aLocalTrack, aNonLocalTrack, doCreateCovariance, locationWCS, covariancePtr);
         }
         else if (aNonLocalTrack.LocationValid()) // Also we know that it's 2D if we are here and location is valid.
         {
            // We have a 2d location track with no r,b,e.  This situation can happen if we pass r,b tracks around without fusion.
            FuseLocationWith2D_Location(aLocalTrack, aNonLocalTrack, locationWCS);
            updateLocation = true;
         }
         else
         {
            updateLocation = false;
         }
      }
   }
   else if (aLocalTrack.RangeValid() && aLocalTrack.BearingValid() &&
            aNonLocalTrack.ElevationValid() && // and not a 3-D track
            (!aNonLocalTrack.BearingValid()))
   {
      bool doCreateCovariance = false; // Don't create a covariance matrix on an elevation update.
      FuseRangeBearingWithElevation(aLocalTrack, aNonLocalTrack, doCreateCovariance, locationWCS, covariancePtr);
      aLocalTrack.Set3D(true);
      updateLocation = true;
      aLocalTrack.SetUpdateTime(aNonLocalTrack.GetUpdateTime());
   }
   else if (aNonLocalTrack.RangeValid() && aNonLocalTrack.BearingValid() &&
            aLocalTrack.ElevationValid()) // the symmetric case of the one above
   {
      bool doCreateCovariance = (covariancePtr == nullptr); // and valid range-bearing in the track update.
      FuseRangeBearingWithElevation(aNonLocalTrack, aLocalTrack, doCreateCovariance, locationWCS, covariancePtr);
      updateLocation = true;
      aLocalTrack.Set3D(true);
      aLocalTrack.SetUpdateTime(aNonLocalTrack.GetUpdateTime());
   }
   else if (aNonLocalTrack.RangeValid() && aNonLocalTrack.BearingValid())
   {
      //! The track will be marked as non-3d to prevent weighted-average fusion
      //! (the covariance matrix cannot be computed from sensor errors).
      FuseRangeBearing(aLocalTrack, aNonLocalTrack, locationWCS);
      aLocalTrack.Set3D(false); // This is not a true 3D solution
      localTrackUpdated = true;
      updateLocation    = true; // Set the calculated location later.
   }
   else if ((!aLocalTrack.BearingValid()) && aNonLocalTrack.ElevationValid()) // and location not valid
   {
      // Save the elevation and/or range for later processing.
      aLocalTrack.SetElevation(aNonLocalTrack.GetElevation());
      aLocalTrack.SetElevationError(aNonLocalTrack.GetElevationError());
      if (aNonLocalTrack.RangeValid())
      {
         aLocalTrack.SetRange(aNonLocalTrack.GetRange());
         aLocalTrack.SetRangeError(aNonLocalTrack.GetRangeError());
      }
      double originatorLocWCS[3];
      aNonLocalTrack.GetOriginatorLocationWCS(originatorLocWCS);
      aLocalTrack.SetOriginatorLocationWCS(originatorLocWCS);
      localTrackUpdated = true;
   }
   if ((!updateLocation) && (!aNonLocalTrack.RangeValid()) && aNonLocalTrack.BearingValid())
   {
      // Save valid bearing and elevation data for later processing.
      aLocalTrack.SetBearingValid(true);
      aLocalTrack.SetBearing(aNonLocalTrack.GetBearing());
      aLocalTrack.SetBearingError(aNonLocalTrack.GetBearingError());
      double originatorLocWCS[3];
      aNonLocalTrack.GetOriginatorLocationWCS(originatorLocWCS);
      aLocalTrack.SetOriginatorLocationWCS(originatorLocWCS);
      if (aNonLocalTrack.ElevationValid())
      {
         aLocalTrack.SetElevationValid(true);
         aLocalTrack.SetElevation(aNonLocalTrack.GetElevation());
         aLocalTrack.SetElevationError(aNonLocalTrack.GetElevationError());
      }
      localTrackUpdated = true;
   }

   if (updateLocation)
   {
      // It is possible that the local track must first be updated to the time of the nonlocal track,
      // (call to KinematicUpdate in UpdateLocation), then updated again to the current simulation time
      // (call to KinematicUpdate, below).  This situation results in the update count incremented twice.
      // Because the intent is to show only one increment in the update count, we control it manually.
      int updateCount = aLocalTrack.GetUpdateCount();
      UpdateLocation(aLocalTrack, aNonLocalTrack, locationWCS, covariancePtr, aLocalTrack.GetRawTrackIds().GetCount());
      localTrackUpdated = true;

      if (aLocalTrack.Is3D())
      {
         // Set the originating location to be the current location of the host platform.
         // We are about to update a platform based on a perceived time.
         // This time should never be greater than the current simulation time.
         GetPlatform()->Update(aSimTime);
         double reportingLocationWCS[3];
         GetPlatform()->GetLocationWCS(reportingLocationWCS);
         aLocalTrack.SetOriginatorLocationWCS(reportingLocationWCS);
         double WCS_ToNED_Transform[3][3];
         GetPlatform()->GetWCSToNEDTransform(WCS_ToNED_Transform);
         aLocalTrack.SetOriginatorTransformWCS(WCS_ToNED_Transform);

         // Invalidate range, bearing, elevation.
         aLocalTrack.SetRangeValid(false);
         aLocalTrack.SetBearingValid(false);
         aLocalTrack.SetElevationValid(false);
      }

      // Finally, perform a kinematic update of the local track to the current simulation time.
      aLocalTrack.KinematicUpdate(aSimTime);

      ++updateCount;
      aLocalTrack.SetUpdateCount(updateCount); // See above comment concerning the update count.
   }
   else if (localTrackUpdated)
   {
      aLocalTrack.Update(aSimTime);
   }

   // Note: We assume the track quality is associated with kinematic data,
   // so call this method with every track update.
   localTrackUpdated |= UpdateNonKinematicData(aSimTime, aLocalTrack, aNonLocalTrack);

   if (covariancePtr != nullptr)
   {
      delete covariancePtr;
   }

   // Copy the residual covariance if one exists in the nonlocal track.
   if (aNonLocalTrack.GetResidualCovariance() != nullptr)
   {
      aLocalTrack.SetResidualCovariance(*aNonLocalTrack.GetResidualCovariance());
   }

   // If the local track was updated then it was because of an update from some track other than the mirror source,
   // indicate the track will no longer be acting as a mirror.
   if (localTrackUpdated)
   {
      aLocalTrack.ClearMirrorTrack();
   }
   return localTrackUpdated;
}
