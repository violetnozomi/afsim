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

#include "WsfClusterCorrelation.hpp"

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtRandom.hpp"
#include "WsfClusterManager.hpp"
#include "WsfFilter.hpp"
#include "WsfFusionStrategy.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackId.hpp"
#include "WsfTrackList.hpp"
#include "WsfTrackManager.hpp"

//! WsfClusterCorrelation uses WsfClusterManager to perform dynamic clustering on raw tracks.
//! Clusters of raw tracks are then treated as local tracks [system tracks].
//! Newly computed clusters are best matched to previous local tracks, or create new local tracks.
//! The H-Tree Complete-Linkage clustering algorithm is used.
//! WsfClusterCorrelation uses sensor error limits to control the WsfClusterManager's clustering
WsfClusterCorrelation::WsfClusterCorrelation(WsfScenario& aScenario)
   : WsfCorrelationStrategy()
   , mClusterManagerPtr(nullptr)
   , mUpdateInterval(0.0)
   , mLastReclusterTime(0.0)
{
}

WsfClusterCorrelation::~WsfClusterCorrelation()
{
   delete mClusterManagerPtr;
}

// virtual
bool WsfClusterCorrelation::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "update_interval")
   {
      aInput.ReadValueOfType(mUpdateInterval, UtInput::cTIME);
      aInput.ValueGreater(mUpdateInterval, 0.0);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

//! Gets a local track that is correlated with a raw track in 'aClusterPtr'
//! Returns the first one found, usually from the first raw track in the cluster's member list
WsfLocalTrack* WsfClusterCorrelation::GetCorrelatedTrack(std::vector<WsfTrack*>& aCluster)
{
   std::map<WsfTrackId, WsfTrackId>::iterator tit;

   // search for an existing local track that matches with any of the cluster's raw tracks
   for (WsfTrack* rawPtr : aCluster)
   {
      // this is the raw track
      // if we've ever received this raw track before, it should exist here
      tit = mCorrelationMap.find(rawPtr->GetTrackId());
      if (tit != mCorrelationMap.end())
      {
         WsfTrackId     tid           = tit->second; // use this one?
         WsfLocalTrack* localTrackPtr = GetTrackManager().FindTrack(tid);
         if (localTrackPtr != nullptr)
         {
            return localTrackPtr;
         }
      }
   }
   return nullptr;
}

//! Gets the local track correlated with the raw track having id 'aNonLocalTrackId'
WsfLocalTrack* WsfClusterCorrelation::GetCorrelatedTrack(const WsfTrackId& aNonLocalTrackId)
{
   WsfLocalTrack* localTrackPtr = nullptr;
   // if we've ever received this raw track before, it should exist here
   auto tit = mCorrelationMap.find(aNonLocalTrackId);
   if (tit != mCorrelationMap.end())
   {
      localTrackPtr = GetTrackManager().FindTrack(tit->second);
   }
   return localTrackPtr;
}

//! Correlates all raw tracks in 'aClusterPtr' with 'aLocalTrack'
void WsfClusterCorrelation::MakeCorrelated(std::vector<WsfTrack*>& aCluster, WsfLocalTrack& aLocalTrack)
{
   for (auto& trackPtr : aCluster)
   {
      MakeCorrelated(trackPtr->GetTrackId(), aLocalTrack);
   }
}

//! Correlates the raw track having id 'aNonLocalTrackId' with 'aLocalTrack'
void WsfClusterCorrelation::MakeCorrelated(const WsfTrackId& aNonLocalTrackId, WsfLocalTrack& aLocalTrack)
{
   aLocalTrack.Correlate(aNonLocalTrackId);
   mCorrelationMap[aNonLocalTrackId] = aLocalTrack.GetTrackId();
}

//! Decorrelates the raw track having id 'aNonLocalTrackId' from 'aLocalTrack'
void WsfClusterCorrelation::MakeDecorrelated(const WsfTrackId& aNonLocalTrackId, WsfLocalTrack& aLocalTrack)
{
   // do this decorrelation first, the next line could delete the raw track
   Decorrelate(aNonLocalTrackId);
   aLocalTrack.Decorrelate(aNonLocalTrackId);
}

//! Decorrelates all raw tracks fused with 'aLocalTrack' from it
void WsfClusterCorrelation::MakeDecorrelated(WsfLocalTrack& aLocalTrack)
{
   int last = static_cast<int>(aLocalTrack.GetRawTrackIds().GetCount()) - 1;
   for (int i = last; i >= 0; i--)
   {
      const WsfTrackId* rawIdPtr = aLocalTrack.GetRawTrackIds().GetEntry(i);
      if (rawIdPtr != nullptr) // only remove valid raw track id's
      {
         MakeDecorrelated(*rawIdPtr, aLocalTrack);
      }
   }
}

//! Decorrelates all raw tracks in 'aClusterPtr' from the local tracks they are fused to
void WsfClusterCorrelation::MakeDecorrelated(std::vector<WsfTrack*>& aCluster)
{
   for (auto& trackPtr : aCluster)
   {
      const WsfTrackId& rawId         = trackPtr->GetTrackId();
      WsfLocalTrack*    localTrackPtr = GetCorrelatedTrack(rawId);
      if (localTrackPtr != nullptr)
      {
         MakeDecorrelated(rawId, *localTrackPtr);
      }
   }
}

double WsfClusterCorrelation::GetMaxMeasurementError(double aSimTime, const WsfTrack& aTrack)
{
   double error = -1.0;

   // update max error used in clustering, if possible & necessary
   WsfStringId sensorNameId = aTrack.GetSensorNameId(); // must be positive if its a sensor measurement
   if (sensorNameId > nullptr && aTrack.RangeValid())   // no need if sensor isn't reporting range
   {
      // only update the max error if the track is further away than
      // any other track that the sensor has previously reported on
      auto it = mSensorIdToRangeMap.find(sensorNameId);
      if (it != mSensorIdToRangeMap.end() && aTrack.GetRange() < it->second)
      {
         // error has already been logged, do nothing
      }
      else
      {
         // save this range in the map, its larger & will produce a larger error
         mSensorIdToRangeMap[sensorNameId] = aTrack.GetRange();

         // check if covariance matrix already exists, use it for variance values & calculate max sensor error
         if (aTrack.StateCovarianceMatrixValid() && aTrack.GetStateCovariance() != nullptr)
         {
            UtMatrixd mat = *(aTrack.GetStateCovariance());
            // the variances are on the diagonal of the covariance matrix
            error = sqrt(mat(0, 0) + mat(1, 1) + mat(2, 2)); // calculate sensor's error
         }
         else if (aTrack.RangeValid() && aTrack.BearingValid() && aTrack.ElevationValid())
         {
            // calculate variance if possible, find max error from that
            bool ok = ((aTrack.GetRangeError() != 0.0) && (aTrack.GetBearingError() != 0.0) &&
                       (aTrack.GetElevationError() != 0.0));
            if (ok)
            {
               // In the following equations range squared == r2, rs==range sigma, bs==bearing sigma, es==elevation sigma
               double bearing     = aTrack.GetBearing();
               double elevation   = aTrack.GetElevation();
               double sinb        = sin(bearing);
               double cosb        = cos(bearing);
               double sine        = sin(elevation);
               double cose        = cos(elevation);
               double r2          = aTrack.GetRange() * aTrack.GetRange();
               double rs2         = aTrack.GetRangeError() * aTrack.GetRangeError();
               double bs2         = aTrack.GetBearingError() * aTrack.GetBearingError();
               double es2         = aTrack.GetElevationError() * aTrack.GetElevationError();
               double cosb2       = cosb * cosb;
               double sinb2       = sinb * sinb;
               double cose2       = cose * cose;
               double sine2       = sine * sine;
               double sumVariance = rs2 * cose2 * cosb2 + r2 * cose2 * sinb2 * bs2 + r2 * sine2 * cosb2 * es2 +
                                    rs2 * cose2 * sinb2 + r2 * cose2 * cosb2 * bs2 + r2 * sine2 * sinb2 * es2 +
                                    rs2 * sine2 + r2 * cose2 * es2;
               error = sqrt(sumVariance); // calculate sensor's error
            }
         }
         else
         {
            ut::log::error() << "WsfClusterCorrelation: No way to calculate errors.";
         }
      }
   }

   return error;
}

void WsfClusterCorrelation::ClusterRawTracksIntoLocalTracks(double aSimTime)
{
   std::vector<std::vector<WsfTrack*>> clusters;
   if (GetTrackManager().HasFilter())
   {
      // force a proper kinematic update on filtered tracks that do not have a stable filter yet
      // if the filter is not stable yet, then bypass the WsfLocalTrack methods:
      // KinematicUpdate and GetKinematicStateEstimate both rely on the filter
      const WsfLocalTrackList& ftlPtr = GetTrackManager().GetFilteredRawTrackList();
      WsfLocalTrackList*       tlPtr  = ftlPtr.Clone();
      std::vector<WsfTrack*>   tracks;
      for (unsigned int i = 0; i < tlPtr->GetTrackCount(); ++i)
      {
         WsfLocalTrack* tPtr = tlPtr->GetTrackEntry(i);
         if (tPtr->GetUpdateTime() < aSimTime)
         {
            // propagate the track forward to the current aSimTime
            tPtr->KinematicUpdate(aSimTime);
         }
         tracks.push_back(dynamic_cast<WsfTrack*>(tPtr));
      }
      clusters = mClusterManagerPtr->GetTrackClusters(tracks);
   }
   else
   {
      clusters = mClusterManagerPtr->GetTrackClusters(GetTrackManager().GetRawTrackList());
   }

   //// this can be used for [engineering] debugging
   // if (GetTrackManager().DebugEnabled())
   //{
   //    mClusterManagerPtr->DrawAllClusters(3.0);  //three second duration?
   // }

   // PROXIMITY MATCHING, (not using any truth info)
   // create the proximity matrix between clusters and previous local tracks
   // to find which clusters probably represent previous local tracks
   // pair them up, only create new local track if no good match exists for a cluster
   // start with M clusters and N local tracks, populate proximity matrix
   unsigned int    M = static_cast<unsigned int>(clusters.size());
   unsigned int    N = (size_t)GetTrackManager().GetTrackCount();
   ProximityMatrix mat(M, N);
   for (unsigned int i = 0; i < M; ++i)
   {
      double cLoc[3];
      double tLoc[3];
      double rLoc[3];
      for (unsigned int j = 0; j < N; ++j)
      {
         // calculate our own distance, based on slant range (assumes cluster manager distance function is cPOSITION_ONLY)
         WsfLocalTrack* trackPtr = GetTrackManager().GetTrackEntry(j);
         trackPtr->GetExtrapolatedLocationWCS(aSimTime, tLoc);
         WsfGeoPoint pt = mClusterManagerPtr->MeanLocation(clusters[i]);
         pt.GetLocationWCS(cLoc);
         UtVec3d::Subtract(rLoc, tLoc, cLoc);
         double dist = UtVec3d::Magnitude(rLoc);
         mat.Set(i, j, dist);
      }
   }
   ProximityMatrix matCopy(mat);

   // using 3x the max distance limit for cluster formation is generous, but
   // it appears to work well and keeps extra finicky tracks from being created
   // basically... try matching to a previous track if plausible
   double maxClusterLocalTrackSeparation = mClusterManagerPtr->GetDistanceLimit() * 3.0;
   // this gives us the two indices of the nearest matrix entries
   // row = index for clusters, col = index for localTracks
   size_t row    = 0;
   size_t col    = 0;
   double lowest = mat.GetMinIndices(row, col);
   // only associate clusters with previous local tracks if they...
   // are reasonably near each other & still left in the matrix
   while (lowest < maxClusterLocalTrackSeparation)
   {
      // get items of interest
      std::vector<WsfTrack*>& cluster  = clusters[row];
      WsfLocalTrack*          trackPtr = GetTrackManager().GetTrackEntry(static_cast<unsigned int>(col));

      // associate cluster indexed at 'r' with previous local track indexed at 'c'
      // first decorrelate both the cluster and the local track
      MakeDecorrelated(cluster); // all raw tracks in cluster are now decorrelated from previous local tracks
      MakeDecorrelated(
         *trackPtr); // all raw tracks in local track are now decorrelated (in case they don't exist in the cluster)

      // then correlate the cluster into the local track
      MakeCorrelated(cluster, *trackPtr); // now the local track consists of only raw tracks found in the cluster

      // whatever cluster doesn't get correlated in this loop...
      // will be initiated as a new local track in the next loop below
      clusters.erase(clusters.begin() + row); // remove the cluster from THIS particular vector, don't delete it

      // update proximity matrix and grab next closest: cluster <--> local_track
      mat.DeleteRow(row);
      mat.SetColumn(col, std::numeric_limits<double>::max()); // causes GetMinIndices() to ignore this column
      lowest = mat.GetMinIndices(row, col);
   }

   // for any clusters not matched with existing local tracks...
   for (std::vector<WsfTrack*>& cluster : clusters)
   {
      // create new local track for this cluster
      // WsfLocalTrack* trackPtr = GetTrackManager().InitiateAndCorrelateTrack(aSimTime, *clusterPtr->Entry(0));
      WsfTrack*      rawTrackPtr = GetTrackManager().FindRawTrack(cluster.front()->GetTrackId());
      WsfLocalTrack* trackPtr    = GetTrackManager().InitiateAndCorrelateTrack(aSimTime, *rawTrackPtr);

      // decorrelate cluster with any other local tracks
      MakeDecorrelated(cluster); // all raw tracks in cluster are now decorrelated from previous local tracks

      // then correlate the cluster into the local track
      MakeCorrelated(cluster, *trackPtr);

      // Notify interested parties of the new local track creation.
      GetTrackManager().NotifyOfLocalTrackInitiated(aSimTime, trackPtr, rawTrackPtr);
   }

   mLastReclusterTime = aSimTime;
}

// virtual
//! Given a non-local track update (or measurement), find the track in the track list
//! that correlates with the given track or measurement.
WsfLocalTrack* WsfClusterCorrelation::CorrelateImpl(double             aSimTime,
                                                    const WsfTrack&    aNonLocalTrack,
                                                    WsfLocalTrackList& aTrackList)
{
   // if this raw tracks has never been received before OR
   // if the correlator is not scheduled to update periodically
   // THEN re-cluster the raw tracks into local tracks
   if (GetCorrelatedTrack(aNonLocalTrack.GetTrackId()) == nullptr || GetUpdateInterval() <= 0.0)
   {
      // update the cluster manager if a larger possible sensor error exists than before
      double error = GetMaxMeasurementError(aSimTime, aNonLocalTrack);
      // use 3 times the std deviation of error (3 sigma) to include 99% of all random samples
      if ((3.0 * error) > mClusterManagerPtr->GetDistanceLimit())
      {
         mClusterManagerPtr->SetDistanceLimit(3.0 * error);
      }

      // perform clustering
      ClusterRawTracksIntoLocalTracks(aSimTime);
   }

   // return the proper output local track for the raw track input
   // if reclustering did not just occur, then this is basically giving
   // back the same answer that the last re-clustering gave
   return GetCorrelatedTrack(aNonLocalTrack.GetTrackId());
}

bool WsfClusterCorrelation::Initialize(WsfTrackManager* aTrackManagerPtr)
{
   WsfCorrelationStrategy::Initialize(aTrackManagerPtr);
   assert(mClusterManagerPtr == nullptr);
   mClusterManagerPtr = new WsfClusterManager(*aTrackManagerPtr->GetSimulation());
   mClusterManagerPtr->SetClusterMethod(WsfClusterManager::cHIERARCHICAL_TREE_COMPLETE);
   mClusterManagerPtr->SetDistanceFunction(WsfClusterManager::cPOSITION_ONLY);
   mClusterManagerPtr->SetDistanceLimit(100.0); // 100 meters to start, let sensor error boost it up

   if (GetUpdateInterval() > 0.0)
   {
      // start periodic events to re-correlate the raw tracks with global clustering
      WsfSimulation& simulation = *aTrackManagerPtr->GetPlatform()->GetSimulation();
      double         startTime  = simulation.GetSimTime() + simulation.GetRandom().Uniform(0.0, GetUpdateInterval());
      simulation.AddEvent(ut::make_unique<ClusterCorrelationEvent>(startTime, this));
   }
   return true;
}

// virtual
WsfEvent::EventDisposition WsfClusterCorrelation::ClusterCorrelationEvent::Execute()
{
   if (mCorrelatorPtr != nullptr)
   {
      if (mCorrelatorPtr->GetTrackManager().GetRawTrackCount() > 0)
      {
         mCorrelatorPtr->ClusterRawTracksIntoLocalTracks(GetTime());
      }
      // reschedule the event here
      // use the event's scheduled time + the update interval, otherwise time creep could occur
      SetTime(GetTime() + mCorrelatorPtr->GetUpdateInterval());
      return WsfEvent::cRESCHEDULE;
   }
   else
   {
      return WsfEvent::cDELETE;
   }
}
