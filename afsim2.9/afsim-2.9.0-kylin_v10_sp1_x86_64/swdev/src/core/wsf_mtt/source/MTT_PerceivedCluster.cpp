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

#include "MTT_PerceivedCluster.hpp"

#include "MTT.hpp"
#include "MTT_Measurement.hpp"
#include "MTT_NonlocalTrack.hpp"
#include "MTT_RadarMeasurement.hpp"
#include "SupBlock.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtEntity.hpp"
#include "UtLog.hpp"
#include "UtMat3.hpp"
#include "UtMath.hpp"
#include "UtMatrix.hpp"
#include "UtMeasurementUtil.hpp"
#include "WsfAttributeContainer.hpp"
#include "WsfCovariance.hpp"
#include "WsfMTT_Interface.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrack.hpp"
#include "WsfUtil.hpp"

// static
// map<int, MTT_CombinedTrack*> MTT_PerceivedCluster::mTrackIdToCombinedTrackMap;

MTT_PerceivedCluster::MTT_PerceivedCluster()
   : mCombinedTrackPtr(nullptr)
   , mMeasurementPtr(nullptr)
   , mTrackNumber(0)
   , mLastOriginatorId(nullptr)
   , mLastSensorId(nullptr)
   , mConsecutiveSingleSourceHits(0)
{
}

// virtual
MTT_PerceivedCluster::~MTT_PerceivedCluster()
{
   delete mCombinedTrackPtr;
   delete mMeasurementPtr;
}

void MTT_PerceivedCluster::CopyFrom(double aSimTime, WsfMTT_Interface* aMTT_InterfacePtr, const WsfTrack& aTrack)
{
   // Note: A MTT_PerceivedCluster can represent either
   // - A sensor track / measurement, in which case the aggregated MTT_RadarMeasurement block is filled, OR
   // - A processed track (probably from another platform), in which case the aggregated MTT_CombinedTrack block is
   // filled. First case: sensor measurements. We set the data for direct sensor reports by
   if ((aTrack.GetTrackType() == WsfTrack::cUNFILTERED_SENSOR) ||
       (aTrack.GetTrackType() == WsfTrack::cFILTERED_SENSOR)) // strictly, should not be double filtering,
   // but the alternative is an error.
   {
      // - setting the header data
      if (mMeasurementPtr == nullptr)
      {
         mMeasurementPtr = new MTT_RadarMeasurement(); // Superset for all possible data
      }

      // Set convenience data
      if ((mLastOriginatorId == aTrack.GetOriginatorNameId()) && (mLastSensorId == aTrack.GetSensorNameId()))
      {
         ++mConsecutiveSingleSourceHits;
      }
      else
      {
         mConsecutiveSingleSourceHits = 0;
      }

      mLastOriginatorId = aTrack.GetOriginatorNameId();
      mLastSensorId     = aTrack.GetSensorNameId();

      MTT_RadarMeasurement::Data& data = *(static_cast<MTT_RadarMeasurement::Data*>(mMeasurementPtr->mDataPtr));
      mMeasurementPtr->mDataAvailable  = 0;
      double originatorLocWCS[3];
      aTrack.GetOriginatorLocationWCS(originatorLocWCS);

      if (aTrack.LocationValid()) // Could be a Kalman-Filtered location or sensor reporting location
      {
         double targetLocWCS[3];
         aTrack.GetLocationWCS(targetLocWCS);
         double DB_locNED[3]; // Referenced to database center.
         aMTT_InterfacePtr->GetReferencePoint().ConvertWCSToNED(targetLocWCS, DB_locNED);
         double originatorLocDB_NED[3];
         aMTT_InterfacePtr->GetReferencePoint().ConvertWCSToNED(originatorLocWCS, originatorLocDB_NED);
         double targetVecNED[3];
         UtVec3d::Subtract(targetVecNED, DB_locNED, originatorLocDB_NED);
         double range, bearing, elevation;
         UtMeasurementUtil::RangeBearingElevation(targetVecNED, range, bearing, elevation);
         data.mRange = range;
         if (data.mRange < 1.0)
         {
            data.mRange = 1.0;
         }
         data.mAzimuth = UtMath::cPI_OVER_2 - bearing;
         if (data.mAzimuth > UtMath::cPI)
         {
            data.mAzimuth -= UtMath::cTWO_PI;
         }
         if (aTrack.Is3D())
         {
            mMeasurementPtr->mDataAvailable |= 7;
         }
         else
         {
            mMeasurementPtr->mDataAvailable |= 3;
         }
         data.mElevation = elevation;
      }
      else if (aTrack.RangeValid() && aTrack.BearingValid())
      {
         // Re-compute range, bearing, elevation in flat enu...
         // todo optimize
         mMeasurementPtr->mDataAvailable |= 3;
         data.mRange = aTrack.GetRange();
         if (data.mRange < 1.0)
         {
            data.mRange = 1.0;
         }
         data.mAzimuth = UtMath::cPI_OVER_2 - aTrack.GetBearing();
         if (data.mAzimuth > UtMath::cPI)
         {
            data.mAzimuth -= UtMath::cTWO_PI;
         }
         if (aTrack.ElevationValid())
         {
            mMeasurementPtr->mDataAvailable |= 4;
            data.mElevation = aTrack.GetElevation();
         }
         else
         {
            data.mElevation = UtMath::cPI_OVER_2;
            if (aTrack.GetDefaultAltitude() > data.mRange)
            {
               data.mElevation = asin(aTrack.GetDefaultAltitude() / data.mRange);
            }
         }
      }
      else // probably don't have enough info, but we can try...
      {
         if (aTrack.RangeValid())
         {
            mMeasurementPtr->mDataAvailable |= 1;
            data.mRange = aTrack.GetRange();
         }
         else
         {
            // We must have a range for mtt; best to throw an exception here
            // Todo verify this assumption.
            std::string msg = "MTT: All measurements must have valid range; no range reported from sensor: " +
                              aTrack.GetOriginatorTypeId();
            auto logger = ut::log::error() << "MTT: All measurements must have a valid range.";
            logger.AddNote() << "No range reported from sensor: " + aTrack.GetOriginatorTypeId();
            throw UtException(msg);
         }
         if (aTrack.BearingValid())
         {
            mMeasurementPtr->mDataAvailable |= 2;
            data.mAzimuth = UtMath::cPI_OVER_2 - aTrack.GetBearing();
            if (data.mAzimuth > UtMath::cPI)
            {
               data.mAzimuth -= UtMath::cTWO_PI;
            }
         }
         if (aTrack.ElevationValid())
         {
            mMeasurementPtr->mDataAvailable |= 4;
            data.mElevation = aTrack.GetElevation();
         }
      }

      // Handle angular errors; if none specified, we have to make them up!
      // MTT expects expected error values for all measurements, even if
      // the particular sensor does not report those values.
      // Note that error of zero means that no errors were computed;
      // Otherwise, the error is valid.
      double scaleFactor = 1.0 / std::max(aTrack.GetTrackQuality(), 0.1);
      data.mRangeStDev   = aTrack.GetRangeError();
      if (data.mRangeStDev == 0.0)
      {
         data.mRangeStDev = 0.001 * scaleFactor * data.mRange; // (arbitrary value)
      }
      data.mAzimuthStDev = aTrack.GetBearingError();
      if (data.mAzimuthStDev == 0.0)
      {
         data.mAzimuthStDev = scaleFactor * 0.001; // (arbitrary value)
      }
      data.mElevationStDev = aTrack.GetElevationError();
      if (data.mElevationStDev == 0.0)
      {
         data.mElevationStDev = scaleFactor * 0.001; // (arbitrary value)
      }

      // Might need to use the target location to calculate the "measured" range rate.

      WsfPlatform* targetPtr     = aMTT_InterfacePtr->GetSimulation().GetPlatformByIndex(aTrack.GetTargetIndex());
      WsfPlatform* originatorPtr = aMTT_InterfacePtr->GetSimulation().GetPlatformByName(aTrack.GetOriginatorNameId());
      double       targetLocWCS[3];
      if (targetPtr != nullptr)
      {
         targetPtr->Update(aSimTime);
         targetPtr->GetLocationWCS(targetLocWCS);
      }

      if (aTrack.RangeRateValid())
      {
         mMeasurementPtr->mDataAvailable |= 8;
         data.mRangeRate      = aTrack.GetRangeRate();
         data.mRangeRateStDev = aTrack.GetRangeRateError();
      }
      else if ((aTrack.VelocityValid() && targetPtr != nullptr))
      {
         // Use velocity to calculate the range rate.
         double velWCS[3];
         aTrack.GetVelocityWCS(velWCS);
         double targetVecWCS[3];
         UtVec3d::Subtract(targetVecWCS, targetLocWCS, originatorLocWCS);
         double range                  = std::max(UtVec3d::Magnitude(targetVecWCS), 1.0);
         data.mRangeRate               = UtVec3d::DotProduct(velWCS, targetVecWCS) / range;
         const WsfCovariance* covarPtr = aTrack.GetStateCovariance();
         if ((covarPtr != nullptr) && (covarPtr->GetSize() == 36)) // has vel variances
         {
            const WsfCovariance& c = *covarPtr;
            // Take a "vector" of variances and dot these with the target vector.
            double varVecWCS[3]  = {c(3, 3), c(4, 4), c(5, 5)};
            data.mRangeRateStDev = sqrt(fabs(UtVec3d::DotProduct(varVecWCS, targetVecWCS) / range));
         }
         else
         {
            // use 10% of the range rate:
            data.mRangeRateStDev = 0.1 * fabs(data.mRangeRate);
         }
         mMeasurementPtr->mDataAvailable |= 8;
      }

      // Fill in truth data.
      double originatorVelNED[3] = {0.0, 0.0, 0.0};
      double originatorVelWCS[3] = {0.0, 0.0, 0.0};

      if (originatorPtr != nullptr)
      {
         originatorPtr->GetVelocityWCS(originatorVelWCS);
         aMTT_InterfacePtr->GetReferencePoint().ConvertWCSVectorToNED(originatorVelNED, originatorVelWCS);
      }

      double originatorLocNED[3];
      aMTT_InterfacePtr->GetReferencePoint().ConvertWCSToNED(originatorLocWCS, originatorLocNED);
      data.mSensorLocation[0] = originatorLocNED[1];
      data.mSensorLocation[1] = originatorLocNED[0];
      data.mSensorLocation[2] = -originatorLocNED[2];
      data.mSensorVelocity[0] = originatorVelNED[1];
      data.mSensorVelocity[1] = originatorVelNED[0];
      data.mSensorVelocity[2] = -originatorVelNED[2];
      data.mSignalToNoise     = 1.0; // not used.

      if (targetPtr != nullptr)
      {
         targetPtr->Update(aSimTime);
         targetPtr->GetLocationWCS(targetLocWCS);
         double lat, lon, alt;
         targetPtr->GetLocationLLA(lat, lon, alt);
         UtEllipsoidalEarth::ConvertECEFToLLA(originatorLocWCS, lat, lon, alt);
         double transECEF[3][3];
         UtEllipsoidalEarth::ComputeNEDTransform(lat, lon, alt, transECEF, originatorLocWCS);
         double targetLocNED[3];
         UtEllipsoidalEarth::ConvertECEFToLocal(originatorLocWCS, transECEF, targetLocWCS, targetLocNED);
         data.mTrueRange     = UtVec3d::Magnitude(targetLocNED);
         data.mTrueAzimuth   = atan2(targetLocNED[0], targetLocNED[1]); // cc east-north
         data.mTrueElevation = -asin(targetLocNED[2] / data.mTrueRange);
         double targetVelWCS[3];
         targetPtr->GetVelocityWCS(targetVelWCS);
         double relativeVelNED[3];
         double relativeVelWCS[3];
         UtVec3d::Subtract(relativeVelWCS, targetVelWCS, originatorVelWCS);
         aMTT_InterfacePtr->GetReferencePoint().ConvertWCSVectorToNED(relativeVelNED, relativeVelWCS);
         data.mTrueRangeRate = UtVec3d::DotProduct(relativeVelNED, targetLocNED) / data.mTrueRange;
      }

      mTrackNumber = 0;
   }
   // Otherwise: This is a processed track; save in the combined data block.
   else
   {
      mConsecutiveSingleSourceHits = 0;

      if (aTrack.LocationValid() || (aTrack.RangeValid() && aTrack.BearingValid()) ||
          (aTrack.RangeValid() && aTrack.ElevationValid()))
      {
         double locWCS[3] = {0.0, 0.0, 0.0};

         double originatorLocWCS[3];
         aTrack.GetOriginatorLocationWCS(originatorLocWCS);
         unsigned dimension = 0; // Dimension of measurement (1,2,3)

         if (aTrack.LocationValid())
         {
            aTrack.GetLocationWCS(locWCS);
            dimension = 3;
         }
         else
         {
            double elevation = 0.0;
            if (aTrack.ElevationValid() && aTrack.RangeValid())
            {
               elevation = aTrack.GetElevation();
               ++dimension;
            }
            else
            {
               elevation = UtMeasurementUtil::Elevation(originatorLocWCS, aTrack.GetRange(), aTrack.GetDefaultAltitude());
            }
            if (aTrack.BearingValid() && aTrack.RangeValid())
            {
               UtMeasurementUtil::LocationWCS(originatorLocWCS, aTrack.GetRange(), aTrack.GetBearing(), elevation, locWCS);
               dimension += 2;
            }
         }

         if (dimension == 0)
         {
            auto logger = ut::log::error()
                          << "MTT Tracker: Need valid location, (range, az), or (range, el) track for fusion.";
            logger.AddNote() << "Track ID: " + aTrack.GetOriginatorTypeId();
            return;
         }

         WsfCovariance* covarPtr = nullptr;

         // Get the track number and combined track data
         mTrackNumber            = WsfUtil::TrackIdToInt(aTrack.GetTrackId());
         bool isNewCombinedTrack = (mCombinedTrackPtr == nullptr);
         if (isNewCombinedTrack)
         {
            mCombinedTrackPtr = new MTT_CombinedTrack();
         }

         MTT_CombinedTrack::Filter& filter = mCombinedTrackPtr->mFilter;
         UtMatrixd                  filteredState(6);

         if (dimension >= 2) // Copy 2d-3d horizontal data
         {
            double velWCS[3] = {0.0, 0.0, 0.0};
            if (aTrack.VelocityValid())
            {
               aTrack.GetVelocityWCS(velWCS);
            }
            else if (aTrack.RangeRateValid())
            {
               // todo
            }

            // Convert the location and velocity into an NED reference.
            double locNED[3];
            double velNED[3];
            aMTT_InterfacePtr->GetReferencePoint().ConvertWCSToNED(locWCS, locNED);
            aMTT_InterfacePtr->GetReferencePoint().ConvertWCSVectorToNED(velNED, velWCS);

            // If one already exists, copy it.
            if (aTrack.StateCovarianceMatrixValid())
            {
               covarPtr = new WsfCovariance(*aTrack.GetStateCovariance());
            }
            else
            {
               // We *must* have a covariance for the tracker, so here we try our best to provide the data.
               // This is a little ad-hoc, but the alternative is a broken tracker.
               // todo We might want to add a one-time warning message about this; we would have to add as static map.
               // The best thing is to use kalman filters at all nodes, so we always have a well-formed covar.
               double scaleFactor = 1.0 / std::max(aTrack.GetTrackQuality(), 0.1);
               double rangeError  = aTrack.GetRangeError();
               double range, bearing, elevation;
               double lat, lon, alt;

               UtEllipsoidalEarth::ConvertECEFToLLA(originatorLocWCS, lat, lon, alt);
               double WCSTo_NED[3][3];
               UtEllipsoidalEarth::ComputeNEDTransform(lat, lon, alt, WCSTo_NED, originatorLocWCS);
               double targetVecWCS[3];
               UtVec3d::Subtract(targetVecWCS, locWCS, originatorLocWCS);
               double targetVecNED[3];
               UtMat3d::Transform(targetVecNED, WCSTo_NED, targetVecWCS);

               UtMeasurementUtil::RangeBearingElevation(targetVecNED, range, bearing, elevation);
               if (rangeError == 0.0)
               {
                  rangeError = 0.01 * scaleFactor * range; // (arbitrary value)
               }
               double bearingError = aTrack.GetBearingError();
               if (bearingError == 0.0)
               {
                  bearingError = scaleFactor * 0.01; // (arbitrary value)
               }
               double elevationError = aTrack.GetElevationError();
               if (elevationError == 0.0)
               {
                  elevationError = scaleFactor * 0.01; // (arbitrary value)
               }
               covarPtr = WsfCovariance::CreateMeasurementCovarianceWCS(range,
                                                                        rangeError,
                                                                        bearing,
                                                                        bearingError,
                                                                        elevation,
                                                                        elevationError,
                                                                        originatorLocWCS);
            }

            // - do not set the header data.
            if (mMeasurementPtr != nullptr)
            {
               delete mMeasurementPtr;
               mMeasurementPtr = nullptr;
            }

            // transform ned to enu.
            filteredState(0) = locNED[1];  // east
            filteredState(1) = locNED[0];  // north
            filteredState(4) = -locNED[2]; // up
            filteredState(2) = velNED[1];  // east
            filteredState(3) = velNED[0];  // north
            filteredState(5) = -velNED[2]; // up

            assert(covarPtr != nullptr);

            WsfCovariance covar(6, 6);
            if (aTrack.GetAuxDataConst().AttributeExists("MTT_Covariance"))
            {
               UtMatrixd supCovar;
               aTrack.GetAuxDataConst().Get("MTT_Covariance", supCovar);
               covar = supCovar;
               assert(aTrack.GetAuxDataConst().AttributeExists("eastAcceleration"));
               filter.mX_Acceleration = aTrack.GetAuxDataConst().GetDouble("eastAcceleration");
               assert(aTrack.GetAuxDataConst().AttributeExists("northAcceleration"));
               filter.mY_Acceleration = aTrack.GetAuxDataConst().GetDouble("northAcceleration");
            }
            else
            {
               std::string str = "You must use the MTT tracker for all Command Chains that report fused tracks "
                                 "(Reporting Platform:) " +
                                 aTrack.GetOriginatorNameId();
               auto logger = ut::log::error()
                             << "You must use the MTT tracker for all Command Chains that report fused tracks.";
               logger.AddNote() << "Reporting Platform: " << aTrack.GetOriginatorNameId();
               throw UtException(str);
            }

            // If update flag = 2 we have horizontal location data; if 3, we have vert. and horizontal location data (3d loc.)
            mCombinedTrackPtr->mUpdateFlag = 2;

            if (isNewCombinedTrack)
            {
               // Make sure the previous update time is the same as the update time;
               // This way the previous estimates, which don't yet exist, won't be used.
               filter.mPreviousFilteredState     = filteredState;
               filter.mPreviousInformationMatrix = covar;
            }
            else
            {
               // Set the previous values to be the current values:
               filter.mPreviousFilteredState     = filter.mFilteredState;
               filter.mPreviousInformationMatrix = filter.mInformationMatrix;
            }

            filter.mInformationMatrix = covar;
         } // end 2d-3d horizontal data copy

         mCombinedTrackPtr->mHasVerticalData = (aTrack.Is3D() || (aTrack.ElevationValid() && aTrack.RangeValid()));

         // Before completing conversion of info matrix; need to save off vertical data:
         if (mCombinedTrackPtr->mHasVerticalData)
         {
            if (aTrack.GetAuxDataConst().AttributeExists("MTT_VerticalCovariance"))
            {
               UtMatrixd vCovar(2, 2); // may not be used.
               aTrack.GetAuxDataConst().Get("MTT_VerticalCovariance", vCovar);
               mCombinedTrackPtr->mUpdateFlag += 1; // for 3D loc.
               vCovar.Invert();                     // Convert to information matrix (inverse of covar matrix).
               filter.mVerticalInformationMatrix = vCovar;
            }
            else
            {
               std::string str = "You must use the MTT tracker for all Command Chains that report fused tracks "
                                 "(Reporting Platform:) " +
                                 aTrack.GetOriginatorNameId();
               auto logger = ut::log::error()
                             << "You must use the MTT tracker for all Command Chains that report fused tracks.";
               logger.AddNote() << "Reporting Platform: " << aTrack.GetOriginatorNameId();
               throw UtException(str);
            }
         }

         if (isNewCombinedTrack)
         {
            filter.mPreviousUpdateTime     = aTrack.GetUpdateTime();
            filter.mLastVerticalUpdateTime = aTrack.GetUpdateTime();
         }
         else
         {
            filter.mPreviousUpdateTime = filter.mUpdateTime;
            filter.mPreviousUpdateTime = filter.mUpdateTime;
         }

         if (isNewCombinedTrack)
         {
            filter.mPreviousFilteredState = filteredState;
         }
         else
         {
            // Set the previous values to be the current values:
            filter.mPreviousFilteredState = filter.mFilteredState;
         }

         filter.mFilteredState = filteredState;
         filter.mUpdateTime    = aTrack.GetUpdateTime();
         delete covarPtr;
      }
   }
}

void MTT_PerceivedCluster::SetCombinedTrack(const MTT_CombinedTrack& aCombinedTrack)
{
   if (mCombinedTrackPtr != nullptr)
   {
      delete mCombinedTrackPtr;
   }
   mCombinedTrackPtr = new MTT_CombinedTrack(aCombinedTrack);
}

void MTT_PerceivedCluster::SetMeasurement(const MTT_Measurement& aMeasurement)
{
   if (mMeasurementPtr != nullptr)
   {
      delete mMeasurementPtr;
   }
   mMeasurementPtr = aMeasurement.Clone();
}

void MTT_PerceivedCluster::ConvertFrom(const SupBlock& aBlock)
{
   mTrackNumber = aBlock.mIntBlock[41];
}
