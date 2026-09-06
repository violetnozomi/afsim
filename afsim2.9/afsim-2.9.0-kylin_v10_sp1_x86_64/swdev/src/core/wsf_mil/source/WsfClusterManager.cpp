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

#include "WsfClusterManager.hpp"

#include <cstdint>
#include <limits>
#include <map>
#include <sstream>
#include <vector>

#include "UtEllipsoidalEarth.hpp"
#include "UtEntity.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtScript.hpp"
#include "UtScriptContext.hpp"
#include "UtScriptData.hpp"
#include "UtScriptEnvironment.hpp"
#include "WsfAssetPerception.hpp"
#include "WsfDraw.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackList.hpp"
#include "WsfUtil.hpp"


WsfPlatformClusterObject::WsfPlatformClusterObject(WsfPlatform* aPlatformPtr)
   : UtClusterObject("WsfPlatform")
   , // match script class type
   mPlatformPtr(aPlatformPtr)
{
   if (mPlatformPtr != nullptr)
   {
      std::stringstream ss;
      ss << mPlatformPtr->GetIndex();
      mId = ss.str();
   }
}

// virtual
bool WsfPlatformClusterObject::VelocityValid()
{
   return (mPlatformPtr != nullptr); // every platform has a velocity, even if it is zero
}

// virtual
void WsfPlatformClusterObject::GetLocationWCS(double aLocationWCS[3])
{
   if (mPlatformPtr != nullptr)
   {
      return mPlatformPtr->GetLocationWCS(aLocationWCS);
   }
   aLocationWCS[0] = aLocationWCS[1] = aLocationWCS[2] = 0.0;
}

// virtual
void WsfPlatformClusterObject::GetLocationLLA(double& aLat, double& aLon, double& aAlt)
{
   if (mPlatformPtr != nullptr)
   {
      return mPlatformPtr->GetLocationLLA(aLat, aLon, aAlt);
   }
   aLat = aLon = aAlt = 0.0;
}

// virtual
void WsfPlatformClusterObject::GetVelocityWCS(double aVelocityWCS[3])
{
   if (mPlatformPtr != nullptr)
   {
      return mPlatformPtr->GetVelocityWCS(aVelocityWCS);
   }
   aVelocityWCS[0] = aVelocityWCS[1] = aVelocityWCS[2] = 0.0;
}

// static
void WsfPlatformClusterObject::GetObjects(const std::vector<WsfPlatform*>& platforms, std::vector<UtClusterObject*>& objects)
{
   // assumes 'objects' is empty or does not need deleted or cleared
   // convert from platforms to generic cluster objects
   objects.reserve(platforms.size());
   for (auto& platform : platforms)
   {
      objects.push_back(new WsfPlatformClusterObject(platform));
   }
}

// static
void WsfPlatformClusterObject::GetPlatforms(std::vector<UtClusterObject*>& objects, std::vector<WsfPlatform*>& platforms)
{
   // assumes 'platforms' is empty or does not need deleted or cleared
   // convert from generic cluster objects to platforms
   platforms.reserve(objects.size());
   for (auto& object : objects)
   {
      WsfPlatform* platformPtr = dynamic_cast<WsfPlatformClusterObject*>(object)->GetPlatform();
      platforms.push_back(platformPtr);
   }
}


WsfTrackClusterObject::WsfTrackClusterObject(WsfTrack* aTrackPtr, double aSimTime)
   : UtClusterObject("WsfTrack")
   , mTrackPtr(aTrackPtr)
   , mSimTime(aSimTime)
{
   if (mTrackPtr != nullptr)
   {
      std::stringstream ss;
      ss << mTrackPtr->GetTrackId();
      mId = ss.str();
   }
}

// virtual
bool WsfTrackClusterObject::VelocityValid()
{
   return (mTrackPtr != nullptr && mTrackPtr->VelocityValid());
}

// virtual
void WsfTrackClusterObject::GetLocationWCS(double aLocationWCS[3])
{
   if ((mTrackPtr) != nullptr && mTrackPtr->GetExtrapolatedLocationWCS(GetSimTime(), aLocationWCS))
   {
   }
   else
   {
      aLocationWCS[0] = aLocationWCS[1] = aLocationWCS[2] = 0.0;
   }
}

// virtual
void WsfTrackClusterObject::GetLocationLLA(double& aLat, double& aLon, double& aAlt)
{
   double locationWCS[3];
   if ((mTrackPtr) != nullptr && mTrackPtr->GetExtrapolatedLocationWCS(GetSimTime(), locationWCS))
   {
      UtEllipsoidalEarth::ConvertECEFToLLA(locationWCS, aLat, aLon, aAlt);
   }
   else
   {
      aLat = aLon = aAlt = 0.0;
   }
}

// virtual
void WsfTrackClusterObject::GetVelocityWCS(double aVelocityWCS[3])
{
   if (mTrackPtr != nullptr && mTrackPtr->VelocityValid())
   {
      return mTrackPtr->GetVelocityWCS(aVelocityWCS);
   }
   aVelocityWCS[0] = aVelocityWCS[1] = aVelocityWCS[2] = 0.0;
}

// static
void WsfTrackClusterObject::GetObjects(double                         aSimTime,
                                       const std::vector<WsfTrack*>&  tracks,
                                       std::vector<UtClusterObject*>& objects)
{
   // assumes 'objects' is empty or does not need deleted or cleared
   // convert from tracks to generic cluster objects
   objects.reserve(tracks.size());
   for (auto& track : tracks)
   {
      objects.push_back(new WsfTrackClusterObject(track, aSimTime));
   }
}

// static
void WsfTrackClusterObject::GetObjects(double                             aSimTime,
                                       const std::vector<WsfLocalTrack*>& tracks,
                                       std::vector<UtClusterObject*>&     objects)
{
   // assumes 'objects' is empty or does not need deleted or cleared
   // convert from tracks to generic cluster objects
   objects.reserve(tracks.size());
   for (auto& track : tracks)
   {
      objects.push_back(new WsfTrackClusterObject(track, aSimTime));
   }
}

// static
void WsfTrackClusterObject::GetObjects(double aSimTime, WsfTrackList& tracks, std::vector<UtClusterObject*>& objects)
{
   // assumes 'objects' is empty or does not need deleted or cleared
   // convert from tracks to generic cluster objects
   objects.reserve(tracks.GetTrackCount());
   for (unsigned int i = 0; i < tracks.GetTrackCount(); ++i)
   {
      WsfTrack* tPtr = tracks.GetTrackEntry(i);
      objects.push_back(new WsfTrackClusterObject(tPtr, aSimTime));
   }
}

// static
void WsfTrackClusterObject::GetObjects(double aSimTime, WsfLocalTrackList& tracks, std::vector<UtClusterObject*>& objects)
{
   // assumes 'objects' is empty or does not need deleted or cleared
   // convert from tracks to generic cluster objects
   objects.reserve(tracks.GetTrackCount());
   for (unsigned int i = 0; i < tracks.GetTrackCount(); ++i)
   {
      // WsfTrack* tp = tracks.GetTrackEntry(i);
      WsfLocalTrack* ltPtr = tracks.GetTrackEntry(i);
      objects.push_back(new WsfTrackClusterObject(dynamic_cast<WsfTrack*>(ltPtr), aSimTime));
   }
}

// static
void WsfTrackClusterObject::GetTracks(std::vector<UtClusterObject*>& objects, std::vector<WsfTrack*>& tracks)
{
   // assumes 'tracks' is empty or does not need deleted or cleared
   // convert from generic cluster objects to tracks
   tracks.reserve(objects.size());
   for (auto& object : objects)
   {
      WsfTrack* trackPtr = dynamic_cast<WsfTrackClusterObject*>(object)->GetTrack();
      tracks.push_back(trackPtr);
   }
}

// static
void WsfTrackClusterObject::GetTracks(std::vector<UtClusterObject*>& objects, std::vector<WsfLocalTrack*>& tracks)
{
   // assumes 'tracks' is empty or does not need deleted or cleared
   // convert from generic cluster objects to tracks
   tracks.reserve(objects.size());
   for (auto& object : objects)
   {
      WsfLocalTrack* trackPtr = dynamic_cast<WsfLocalTrack*>(dynamic_cast<WsfTrackClusterObject*>(object)->GetTrack());
      tracks.push_back(trackPtr);
   }
}


WsfGeoPointClusterObject::WsfGeoPointClusterObject(WsfGeoPoint* aGeoPtr)
   : UtClusterObject("WsfGeoPoint")
   , mGeoPointPtr(aGeoPtr)
{
   if (mGeoPointPtr != nullptr)
   {
      std::stringstream ss;
      ss << mGeoPointPtr->GetLat() << mGeoPointPtr->GetLon() << mGeoPointPtr->GetAlt();
      mId = ss.str();
   }
}

// virtual
bool WsfGeoPointClusterObject::VelocityValid()
{
   return false;
}

// virtual
void WsfGeoPointClusterObject::GetLocationWCS(double aLocationWCS[3])
{
   if (mGeoPointPtr != nullptr)
   {
      return mGeoPointPtr->GetLocationWCS(aLocationWCS);
   }
   aLocationWCS[0] = aLocationWCS[1] = aLocationWCS[2] = 0.0;
}

// virtual
void WsfGeoPointClusterObject::GetLocationLLA(double& aLat, double& aLon, double& aAlt)
{
   if (mGeoPointPtr != nullptr)
   {
      return mGeoPointPtr->GetLocationLLA(aLat, aLon, aAlt);
   }
   aLat = aLon = aAlt = 0.0;
}

// virtual
void WsfGeoPointClusterObject::GetVelocityWCS(double aVelocityWCS[3])
{
   aVelocityWCS[0] = aVelocityWCS[1] = aVelocityWCS[2] = 0.0;
}

// static
void WsfGeoPointClusterObject::GetObjects(const std::vector<WsfGeoPoint*>& geoPoints, std::vector<UtClusterObject*>& objects)
{
   // assumes 'objects' is empty or does not need deleted or cleared
   // convert from geo-points to generic cluster objects
   objects.reserve(geoPoints.size());
   for (auto& geoPoint : geoPoints)
   {
      objects.push_back(new WsfGeoPointClusterObject(geoPoint));
   }
}

// static
void WsfGeoPointClusterObject::GetGeoPoints(std::vector<UtClusterObject*>& objects, std::vector<WsfGeoPoint*>& geoPoints)
{
   // assumes 'geoPoints' is empty or does not need deleted or cleared
   // convert from generic cluster objects to geo-points
   geoPoints.reserve(objects.size());
   for (auto& object : objects)
   {
      WsfGeoPoint* geoPtr = dynamic_cast<WsfGeoPointClusterObject*>(object)->GetGeoPoint();
      geoPoints.push_back(geoPtr);
   }
}


WsfAssetClusterObject::WsfAssetClusterObject(WsfAssetPerception* aAssetPtr)
   : UtClusterObject("WsfAssetPerception")
   , mAssetPtr(aAssetPtr)
{
   if (mAssetPtr != nullptr)
   {
      std::stringstream ss;
      ss << mAssetPtr->mIndex;
      mId = ss.str();
   }
}

// virtual
bool WsfAssetClusterObject::VelocityValid()
{
   return (mAssetPtr != nullptr);
}

// virtual
void WsfAssetClusterObject::GetLocationWCS(double aLocationWCS[3])
{
   if (mAssetPtr != nullptr)
   {
      aLocationWCS[0] = mAssetPtr->mLocationWCS[0];
      aLocationWCS[1] = mAssetPtr->mLocationWCS[1];
      aLocationWCS[2] = mAssetPtr->mLocationWCS[2];
   }
}

// virtual
void WsfAssetClusterObject::GetLocationLLA(double& aLat, double& aLon, double& aAlt)
{
   if (mAssetPtr != nullptr)
   {
      UtEllipsoidalEarth::ConvertECEFToLLA(mAssetPtr->mLocationWCS, aLat, aLon, aAlt);
   }
}

// virtual
void WsfAssetClusterObject::GetVelocityWCS(double aVelocityWCS[3])
{
   if (mAssetPtr != nullptr)
   {
      aVelocityWCS[0] = mAssetPtr->mVelocityWCS[0];
      aVelocityWCS[1] = mAssetPtr->mVelocityWCS[1];
      aVelocityWCS[2] = mAssetPtr->mVelocityWCS[2];
   }
}


// static
void WsfAssetClusterObject::GetObjects(const std::vector<WsfAssetPerception*>& assets, std::vector<UtClusterObject*>& objects)
{
   // assumes 'objects' is empty or does not need deleted or cleared
   // convert from assets to generic cluster objects
   objects.reserve(assets.size());
   for (auto& asset : assets)
   {
      objects.push_back(new WsfAssetClusterObject(asset));
   }
}

// static
void WsfAssetClusterObject::GetAssets(std::vector<UtClusterObject*>& objects, std::vector<WsfAssetPerception*>& assets)
{
   // assumes 'assets' is empty or does not need deleted or cleared
   // convert from generic cluster objects to assets
   assets.reserve(objects.size());
   for (auto& object : objects)
   {
      WsfAssetPerception* assetPtr = dynamic_cast<WsfAssetClusterObject*>(object)->GetAsset();
      assets.push_back(assetPtr);
   }
}


WsfClusterManager::WsfClusterManager(WsfSimulation&                     aSimulation,
                                     size_t                             aNumClusters,
                                     UtClusterManager::ClusteringMethod aMethod,
                                     UtClusterManager::DistanceFunction aDistType)
   : UtClusterManager(aMethod, aDistType)
   , mSimulationPtr(&aSimulation)
   , mScriptContextPtr(nullptr)
   , mScoringFunctionScriptPtr(nullptr)
{
   UtClusterManager::SetNumClustersToCreate(aNumClusters);
   mDebugDrawPtr = nullptr;
}

void WsfClusterManager::Delete(std::vector<UtClusterObject*>& objects)
{
   for (auto& object : objects)
   {
      delete object;
   }
}


std::vector<std::vector<WsfTrack*>> WsfClusterManager::GetTrackClusters(WsfTrackList& aList)
{
   // convert to generic cluster objects for algorithm
   std::vector<UtClusterObject*> objects;
   WsfTrackClusterObject::GetObjects(GetSimulation()->GetSimTime(), aList, objects);

   // generate clusters
   std::vector<std::vector<UtClusterObject*>> clusters;
   UtClusterManager::GetClusters(objects, clusters);

   // convert back to clusters of track
   std::vector<std::vector<WsfTrack*>> trackClusters(clusters.size()); // same as resize, constructs outer std::vector
   for (size_t i = 0; i < clusters.size(); ++i)
   {
      WsfTrackClusterObject::GetTracks(clusters[i], trackClusters[i]);
   }
   Delete(objects); // this also cleans up memory in 'clusters', they point to same objects

   return trackClusters;
}


std::vector<std::vector<WsfLocalTrack*>> WsfClusterManager::GetTrackClusters(WsfLocalTrackList& aList)
{
   // convert to generic cluster objects for algorithm
   std::vector<UtClusterObject*> objects;
   WsfTrackClusterObject::GetObjects(GetSimulation()->GetSimTime(), aList, objects);

   // generate clusters
   std::vector<std::vector<UtClusterObject*>> clusters;
   UtClusterManager::GetClusters(objects, clusters);

   // convert back to clusters of track
   std::vector<std::vector<WsfLocalTrack*>> trackClusters(clusters.size()); // same as resize, constructs outer std::vector
   for (size_t i = 0; i < clusters.size(); ++i)
   {
      WsfTrackClusterObject::GetTracks(clusters[i], trackClusters[i]);
   }
   Delete(objects); // this also cleans up memory in 'clusters', they point to same objects
   return trackClusters;
}


std::vector<std::vector<WsfTrack*>> WsfClusterManager::GetTrackClusters(const std::vector<WsfTrack*>& aList)
{
   // convert to generic cluster objects for algorithm
   std::vector<UtClusterObject*> objects;
   WsfTrackClusterObject::GetObjects(GetSimulation()->GetSimTime(), aList, objects);

   // generate clusters
   std::vector<std::vector<UtClusterObject*>> clusters;
   UtClusterManager::GetClusters(objects, clusters);

   // convert back to clusters of track
   std::vector<std::vector<WsfTrack*>> trackClusters(clusters.size()); // same as resize, constructs outer std::vector
   for (size_t i = 0; i < clusters.size(); ++i)
   {
      WsfTrackClusterObject::GetTracks(clusters[i], trackClusters[i]);
   }

   Delete(objects); // this also cleans up memory in 'clusters', they point to same objects

   return trackClusters;
}


std::vector<std::vector<WsfLocalTrack*>> WsfClusterManager::GetTrackClusters(const std::vector<WsfLocalTrack*>& aList)
{
   // convert to generic cluster objects for algorithm
   std::vector<UtClusterObject*> objects;
   WsfTrackClusterObject::GetObjects(GetSimulation()->GetSimTime(), aList, objects);

   // generate clusters
   std::vector<std::vector<UtClusterObject*>> clusters;
   UtClusterManager::GetClusters(objects, clusters);

   // convert back to clusters of track
   std::vector<std::vector<WsfLocalTrack*>> trackClusters(clusters.size()); // same as resize, constructs outer std::vector
   for (size_t i = 0; i < clusters.size(); ++i)
   {
      WsfTrackClusterObject::GetTracks(clusters[i], trackClusters[i]);
   }

   Delete(objects); // this also cleans up memory in 'clusters', they point to same objects

   return trackClusters;
}


std::vector<std::vector<WsfPlatform*>> WsfClusterManager::GetPlatformClusters(const std::vector<WsfPlatform*>& aList)
{
   // convert to generic cluster objects for algorithm
   std::vector<UtClusterObject*> objects;
   WsfPlatformClusterObject::GetObjects(aList, objects);

   // generate clusters
   std::vector<std::vector<UtClusterObject*>> clusters;
   UtClusterManager::GetClusters(objects, clusters);

   // convert back to clusters of platforms
   std::vector<std::vector<WsfPlatform*>> platformClusters(clusters.size()); // same as resize, constructs outer std::vector
   for (size_t i = 0; i < clusters.size(); ++i)
   {
      WsfPlatformClusterObject::GetPlatforms(clusters[i], platformClusters[i]);
   }
   Delete(objects); // this also cleans up memory in 'clusters', they point to same objects

   return platformClusters;
}


std::vector<std::vector<WsfGeoPoint*>> WsfClusterManager::GetGeoPointClusters(const std::vector<WsfGeoPoint*>& aList)
{
   // convert to generic cluster objects for algorithm
   std::vector<UtClusterObject*> objects;
   WsfGeoPointClusterObject::GetObjects(aList, objects);

   // generate clusters
   std::vector<std::vector<UtClusterObject*>> clusters;
   UtClusterManager::GetClusters(objects, clusters);

   // convert back to clusters of geo-points
   std::vector<std::vector<WsfGeoPoint*>> geoPointClusters(clusters.size()); // same as resize, constructs outer std::vector
   for (size_t i = 0; i < clusters.size(); ++i)
   {
      WsfGeoPointClusterObject::GetGeoPoints(clusters[i], geoPointClusters[i]);
   }
   Delete(objects); // this also cleans up memory in 'clusters', they point to same objects

   return geoPointClusters;
}


std::vector<std::vector<WsfAssetPerception*>> WsfClusterManager::GetAssetClusters(const std::vector<WsfAssetPerception*>& aList)
{
   // convert to generic cluster objects for algorithm
   std::vector<UtClusterObject*> objects;
   WsfAssetClusterObject::GetObjects(aList, objects);

   // generate clusters
   std::vector<std::vector<UtClusterObject*>> clusters;
   UtClusterManager::GetClusters(objects, clusters);

   // convert back to clusters of assets
   std::vector<std::vector<WsfAssetPerception*>> assetClusters(
      clusters.size()); // same as resize, constructs outer std::vector
   for (size_t i = 0; i < clusters.size(); ++i)
   {
      WsfAssetClusterObject::GetAssets(clusters[i], assetClusters[i]);
   }
   Delete(objects); // this also cleans up memory in 'clusters', they point to same objects
   return assetClusters;
}


unsigned int WsfClusterManager::UniqueId(const std::vector<WsfTrack*>& aList)
{
   std::vector<UtClusterObject*> objects;
   WsfTrackClusterObject::GetObjects(GetSimulation()->GetSimTime(), aList, objects);
   unsigned int id = UtClusterManager::UniqueId(objects);
   Delete(objects);
   return id;
}


unsigned int WsfClusterManager::UniqueId(const std::vector<WsfPlatform*>& aList)
{
   std::vector<UtClusterObject*> objects;
   WsfPlatformClusterObject::GetObjects(aList, objects);
   unsigned int id = UtClusterManager::UniqueId(objects);
   Delete(objects);
   return id;
}


unsigned int WsfClusterManager::UniqueId(const std::vector<WsfGeoPoint*>& aList)
{
   std::vector<UtClusterObject*> objects;
   WsfGeoPointClusterObject::GetObjects(aList, objects);
   unsigned int id = UtClusterManager::UniqueId(objects);
   Delete(objects);
   return id;
}


unsigned int WsfClusterManager::UniqueId(const std::vector<WsfAssetPerception*>& aList)
{
   std::vector<UtClusterObject*> objects;
   WsfAssetClusterObject::GetObjects(aList, objects);
   unsigned int id = UtClusterManager::UniqueId(objects);
   Delete(objects);
   return id;
}


std::vector<WsfTrack*> WsfClusterManager::ConvexHull(const std::vector<WsfTrack*>& aList)
{
   // convert to generic cluster objects for algorithm
   std::vector<UtClusterObject*> objects;
   WsfTrackClusterObject::GetObjects(GetSimulation()->GetSimTime(), aList, objects);

   // generate hull
   std::vector<UtClusterObject*> hullObjects = UtClusterManager::ConvexHull(objects);

   // convert back to a hull of tracks
   std::vector<WsfTrack*> hull;
   WsfTrackClusterObject::GetTracks(hullObjects, hull);

   Delete(objects); // this cleans up 'hullObjects' too, pointers to same objects
   return hull;
}


std::vector<WsfPlatform*> WsfClusterManager::ConvexHull(const std::vector<WsfPlatform*>& aList)
{
   // convert to generic cluster objects for algorithm
   std::vector<UtClusterObject*> objects;
   WsfPlatformClusterObject::GetObjects(aList, objects);

   // generate hull
   std::vector<UtClusterObject*> hullObjects = UtClusterManager::ConvexHull(objects);

   // convert back to a hull of platforms
   std::vector<WsfPlatform*> hull;
   WsfPlatformClusterObject::GetPlatforms(hullObjects, hull);

   Delete(objects); // this cleans up 'hullObjects' too, pointers to same objects
   return hull;
}


std::vector<WsfGeoPoint*> WsfClusterManager::ConvexHull(const std::vector<WsfGeoPoint*>& aList)
{
   // convert to generic cluster objects for algorithm
   std::vector<UtClusterObject*> objects;
   WsfGeoPointClusterObject::GetObjects(aList, objects);

   // generate hull
   std::vector<UtClusterObject*> hullObjects = UtClusterManager::ConvexHull(objects);

   // convert back to a hull of geo-points
   std::vector<WsfGeoPoint*> hull;
   WsfGeoPointClusterObject::GetGeoPoints(hullObjects, hull);

   Delete(objects); // this cleans up 'hullObjects' too, pointers to same objects
   return hull;
}


std::vector<WsfAssetPerception*> WsfClusterManager::ConvexHull(const std::vector<WsfAssetPerception*>& aList)
{
   // convert to generic cluster objects for algorithm
   std::vector<UtClusterObject*> objects;
   WsfAssetClusterObject::GetObjects(aList, objects);

   // generate hull
   std::vector<UtClusterObject*> hullObjects = UtClusterManager::ConvexHull(objects);

   // convert back to a hull of assets
   std::vector<WsfAssetPerception*> hull;
   WsfAssetClusterObject::GetAssets(hullObjects, hull);
   Delete(objects); // this cleans up 'hullObjects' too, pointers to same objects
   return hull;
}


WsfGeoPoint WsfClusterManager::MeanLocation(const std::vector<WsfTrack*>& aList)
{
   std::vector<UtClusterObject*> objects;
   WsfTrackClusterObject::GetObjects(GetSimulation()->GetSimTime(), aList, objects);
   double locWCS[3];
   UtClusterManager::MeanLocWCS(objects, locWCS);
   Delete(objects);
   return WsfGeoPoint(locWCS);
}


WsfGeoPoint WsfClusterManager::MeanLocation(const std::vector<WsfPlatform*>& aList)
{
   std::vector<UtClusterObject*> objects;
   WsfPlatformClusterObject::GetObjects(aList, objects);
   double locWCS[3];
   UtClusterManager::MeanLocWCS(objects, locWCS);
   Delete(objects);
   return WsfGeoPoint(locWCS);
}


WsfGeoPoint WsfClusterManager::MeanLocation(const std::vector<WsfGeoPoint*>& aList)
{
   std::vector<UtClusterObject*> objects;
   WsfGeoPointClusterObject::GetObjects(aList, objects);
   double locWCS[3];
   UtClusterManager::MeanLocWCS(objects, locWCS);
   Delete(objects);
   return WsfGeoPoint(locWCS);
}


WsfGeoPoint WsfClusterManager::MeanLocation(const std::vector<WsfAssetPerception*>& aList)
{
   std::vector<UtClusterObject*> objects;
   WsfAssetClusterObject::GetObjects(aList, objects);
   double locWCS[3];
   UtClusterManager::MeanLocWCS(objects, locWCS);
   Delete(objects);
   return WsfGeoPoint(locWCS);
}


WsfTrack* WsfClusterManager::NearestMember(const std::vector<WsfTrack*>& aList, WsfGeoPoint& toPoint)
{
   std::vector<UtClusterObject*> objects;
   WsfTrackClusterObject::GetObjects(GetSimulation()->GetSimTime(), aList, objects);
   WsfGeoPointClusterObject geoObj(&toPoint);
   WsfTrack*                nearestPtr =
      dynamic_cast<WsfTrackClusterObject*>(UtClusterManager::NearestMember(geoObj, objects))->GetTrack();
   Delete(objects);
   return nearestPtr;
}


WsfPlatform* WsfClusterManager::NearestMember(const std::vector<WsfPlatform*>& aList, WsfGeoPoint& toPoint)
{
   std::vector<UtClusterObject*> objects;
   WsfPlatformClusterObject::GetObjects(aList, objects);
   WsfGeoPointClusterObject geoObj(&toPoint);
   WsfPlatform*             nearestPtr =
      dynamic_cast<WsfPlatformClusterObject*>(UtClusterManager::NearestMember(geoObj, objects))->GetPlatform();
   Delete(objects);
   return nearestPtr;
}


WsfGeoPoint* WsfClusterManager::NearestMember(const std::vector<WsfGeoPoint*>& aList, WsfGeoPoint& toPoint)
{
   std::vector<UtClusterObject*> objects;
   WsfGeoPointClusterObject::GetObjects(aList, objects);
   WsfGeoPointClusterObject geoObj(&toPoint);
   WsfGeoPoint*             nearestPtr =
      dynamic_cast<WsfGeoPointClusterObject*>(UtClusterManager::NearestMember(geoObj, objects))->GetGeoPoint();
   Delete(objects);
   return nearestPtr;
}


WsfAssetPerception* WsfClusterManager::NearestMember(const std::vector<WsfAssetPerception*>& aList, WsfGeoPoint& toPoint)
{
   std::vector<UtClusterObject*> objects;
   WsfAssetClusterObject::GetObjects(aList, objects);
   WsfGeoPointClusterObject geoObj(&toPoint);
   WsfAssetPerception*      nearestPtr =
      dynamic_cast<WsfAssetClusterObject*>(UtClusterManager::NearestMember(geoObj, objects))->GetAsset();
   Delete(objects);
   return nearestPtr;
}


void WsfClusterManager::NearestEdgePoint(const std::vector<WsfTrack*>& aList,
                                         WsfGeoPoint&                  toPoint,
                                         WsfGeoPoint&                  edgePoint,
                                         size_t&                       edgeIndex1,
                                         size_t&                       edgeIndex2)
{
   WsfGeoPointClusterObject      object(&toPoint);
   std::vector<UtClusterObject*> objects;
   WsfTrackClusterObject::GetObjects(GetSimulation()->GetSimTime(), aList, objects);
   double locWCS[3];
   UtClusterManager::NearestEdgePoint(&object, objects, locWCS, edgeIndex1, edgeIndex2);
   edgePoint.SetLocationWCS(locWCS);
   Delete(objects);
}


void WsfClusterManager::NearestEdgePoint(const std::vector<WsfPlatform*>& aList,
                                         WsfGeoPoint&                     toPoint,
                                         WsfGeoPoint&                     edgePoint,
                                         size_t&                          edgeIndex1,
                                         size_t&                          edgeIndex2)
{
   WsfGeoPointClusterObject      object(&toPoint);
   std::vector<UtClusterObject*> objects;
   WsfPlatformClusterObject::GetObjects(aList, objects);
   double locWCS[3];
   UtClusterManager::NearestEdgePoint(&object, objects, locWCS, edgeIndex1, edgeIndex2);
   edgePoint.SetLocationWCS(locWCS);
   Delete(objects);
}


void WsfClusterManager::NearestEdgePoint(const std::vector<WsfGeoPoint*>& aList,
                                         WsfGeoPoint&                     toPoint,
                                         WsfGeoPoint&                     edgePoint,
                                         size_t&                          edgeIndex1,
                                         size_t&                          edgeIndex2)
{
   WsfGeoPointClusterObject      object(&toPoint);
   std::vector<UtClusterObject*> objects;
   WsfGeoPointClusterObject::GetObjects(aList, objects);
   double locWCS[3];
   UtClusterManager::NearestEdgePoint(&object, objects, locWCS, edgeIndex1, edgeIndex2);
   edgePoint.SetLocationWCS(locWCS);
   Delete(objects);
}


void WsfClusterManager::NearestEdgePoint(const std::vector<WsfAssetPerception*>& aList,
                                         WsfGeoPoint&                            toPoint,
                                         WsfGeoPoint&                            edgePoint,
                                         size_t&                                 edgeIndex1,
                                         size_t&                                 edgeIndex2)
{
   WsfGeoPointClusterObject      object(&toPoint);
   std::vector<UtClusterObject*> objects;
   WsfAssetClusterObject::GetObjects(aList, objects);
   double locWCS[3];
   UtClusterManager::NearestEdgePoint(&object, objects, locWCS, edgeIndex1, edgeIndex2);
   edgePoint.SetLocationWCS(locWCS);
   Delete(objects);
}


void WsfClusterManager::CrossLength(const std::vector<WsfTrack*>& aList,
                                    WsfGeoPoint&                  refPoint,
                                    double&                       dist1,
                                    WsfGeoPoint&                  leftPt,
                                    WsfGeoPoint&                  rightPt)
{
   std::vector<UtClusterObject*> objects;
   WsfTrackClusterObject::GetObjects(GetSimulation()->GetSimTime(), aList, objects);
   double refWCS[3];
   refPoint.GetLocationWCS(refWCS);
   double dist;
   double leftWCS[3];
   double rightWCS[3];
   UtClusterManager::CrossLength(objects, refWCS, dist, leftWCS, rightWCS);
   leftPt.SetLocationWCS(leftWCS);
   rightPt.SetLocationWCS(rightWCS);
   Delete(objects);
}


void WsfClusterManager::CrossLength(const std::vector<WsfPlatform*>& aList,
                                    WsfGeoPoint&                     refPoint,
                                    double&                          dist1,
                                    WsfGeoPoint&                     leftPt,
                                    WsfGeoPoint&                     rightPt)
{
   std::vector<UtClusterObject*> objects;
   WsfPlatformClusterObject::GetObjects(aList, objects);
   double refWCS[3];
   refPoint.GetLocationWCS(refWCS);
   double dist;
   double leftWCS[3];
   double rightWCS[3];
   UtClusterManager::CrossLength(objects, refWCS, dist, leftWCS, rightWCS);
   leftPt.SetLocationWCS(leftWCS);
   rightPt.SetLocationWCS(rightWCS);
   Delete(objects);
}


void WsfClusterManager::CrossLength(const std::vector<WsfGeoPoint*>& aList,
                                    WsfGeoPoint&                     refPoint,
                                    double&                          dist1,
                                    WsfGeoPoint&                     leftPt,
                                    WsfGeoPoint&                     rightPt)
{
   std::vector<UtClusterObject*> objects;
   WsfGeoPointClusterObject::GetObjects(aList, objects);
   double refWCS[3];
   refPoint.GetLocationWCS(refWCS);
   double dist;
   double leftWCS[3];
   double rightWCS[3];
   UtClusterManager::CrossLength(objects, refWCS, dist, leftWCS, rightWCS);
   leftPt.SetLocationWCS(leftWCS);
   rightPt.SetLocationWCS(rightWCS);
   Delete(objects);
}


void WsfClusterManager::CrossLength(const std::vector<WsfAssetPerception*>& aList,
                                    WsfGeoPoint&                            refPoint,
                                    double&                                 dist1,
                                    WsfGeoPoint&                            leftPt,
                                    WsfGeoPoint&                            rightPt)
{
   std::vector<UtClusterObject*> objects;
   WsfAssetClusterObject::GetObjects(aList, objects);
   double refWCS[3];
   refPoint.GetLocationWCS(refWCS);
   double dist;
   double leftWCS[3];
   double rightWCS[3];
   UtClusterManager::CrossLength(objects, refWCS, dist, leftWCS, rightWCS);
   leftPt.SetLocationWCS(leftWCS);
   rightPt.SetLocationWCS(rightWCS);
   Delete(objects);
}


void WsfClusterManager::Draw(const std::vector<WsfTrack*>& aList, double duration, UtVec3d& color, DrawType type)
{
   std::vector<UtClusterObject*> objects;
   WsfTrackClusterObject::GetObjects(GetSimulation()->GetSimTime(), aList, objects);
   if (type == STAR)
   {
      DrawStar(objects, duration, color);
   }
   else
   {
      DrawHull(objects, duration, color);
   }
   Delete(objects);
}


void WsfClusterManager::Draw(const std::vector<WsfPlatform*>& aList, double duration, UtVec3d& color, DrawType type)
{
   std::vector<UtClusterObject*> objects;
   WsfPlatformClusterObject::GetObjects(aList, objects);
   if (type == STAR)
   {
      DrawStar(objects, duration, color);
   }
   else
   {
      DrawHull(objects, duration, color);
   }
   Delete(objects);
}


void WsfClusterManager::Draw(const std::vector<WsfGeoPoint*>& aList, double duration, UtVec3d& color, DrawType type)
{
   std::vector<UtClusterObject*> objects;
   WsfGeoPointClusterObject::GetObjects(aList, objects);
   if (type == STAR)
   {
      DrawStar(objects, duration, color);
   }
   else
   {
      DrawHull(objects, duration, color);
   }
   Delete(objects);
}


void WsfClusterManager::Draw(const std::vector<WsfAssetPerception*>& aList, double duration, UtVec3d& color, DrawType type)
{
   std::vector<UtClusterObject*> objects;
   WsfAssetClusterObject::GetObjects(aList, objects);
   if (type == STAR)
   {
      DrawStar(objects, duration, color);
   }
   else
   {
      DrawHull(objects, duration, color);
   }
   Delete(objects);
}


void WsfClusterManager::DrawHull(std::vector<UtClusterObject*>& cluster, double duration, UtVec3d& color)
{
   if (cluster.size() <= 1)
   {
      return;
   }
   if (mDebugDrawPtr == nullptr)
   {
      mDebugDrawPtr = new WsfDraw(*GetSimulation());
      mDebugDrawPtr->SetLayer("WsfClusterManager");
   }
   mDebugDrawPtr->SetDuration(duration);
   mDebugDrawPtr->SetLineSize(2);
   mDebugDrawPtr->SetColor(color[0], color[1], color[2]);
   std::vector<UtClusterObject*> hull = UtClusterManager::ConvexHull(cluster);
   double                        locWCS[3];
   mDebugDrawPtr->BeginPolyline();
   for (auto& clusterObj : hull)
   {
      clusterObj->GetLocationWCS(locWCS);
      mDebugDrawPtr->VertexWCS(locWCS);
   }
   hull[0]->GetLocationWCS(locWCS);  // connect last hull to first hull point
   mDebugDrawPtr->VertexWCS(locWCS); // to complete the hull loop
   mDebugDrawPtr->End();
}


void WsfClusterManager::DrawStar(std::vector<UtClusterObject*>& cluster, double duration, UtVec3d& color)
{
   if (cluster.size() <= 1)
   {
      return;
   }

   if (mDebugDrawPtr == nullptr)
   {
      mDebugDrawPtr = new WsfDraw(*GetSimulation());
      mDebugDrawPtr->SetLayer("WsfClusterManager");
   }

   mDebugDrawPtr->SetDuration(duration);
   mDebugDrawPtr->SetLineSize(2);
   mDebugDrawPtr->SetColor(color[0], color[1], color[2]);
   double meanWCS[3];
   UtClusterManager::MeanLocWCS(cluster, meanWCS);
   double locWCS[3];
   mDebugDrawPtr->BeginLines();
   for (auto& clusterObj : cluster)
   {
      mDebugDrawPtr->VertexWCS(meanWCS);
      clusterObj->GetLocationWCS(locWCS);
      mDebugDrawPtr->VertexWCS(locWCS);
   }
   mDebugDrawPtr->End();
}

//! finds and validates the script the user passed in as a scoring function
bool WsfClusterManager::SetUserDistanceFunction(UtScriptContext* aScriptContextPtr, const std::string& aScoringFunctionName)
{
   mScoringFunctionScriptPtr = nullptr;
   mScriptContextPtr         = aScriptContextPtr;

   mScoringFunctionScriptPtr = aScriptContextPtr->FindScript(aScoringFunctionName);
   while (mScoringFunctionScriptPtr == nullptr && aScriptContextPtr->GetParent() != nullptr)
   {
      aScriptContextPtr         = aScriptContextPtr->GetParent();
      mScoringFunctionScriptPtr = aScriptContextPtr->FindScript(aScoringFunctionName);
   }

   if (mScoringFunctionScriptPtr == nullptr)
   {
      auto out = ut::log::error() << "Unable to find script for cluster manager distance function.";
      out.AddNote() << "Script: " << aScoringFunctionName;
      return false;
   }

   UtScriptEnvironment*    environmentPtr   = aScriptContextPtr->GetEnvironment();
   const UtScriptFunction* trackProtoPtr    = environmentPtr->GetPrototype("double", "WsfTrack, WsfTrack");
   const UtScriptFunction* platformProtoPtr = environmentPtr->GetPrototype("double", "WsfPlatform, WsfPlatform");
   const UtScriptFunction* geoPointProtoPtr = environmentPtr->GetPrototype("double", "WsfGeoPoint, WsfGeoPoint");
   const UtScriptFunction* assetProtoPtr =
      environmentPtr->GetPrototype("double", "WsfAssetPerception, WsfAssetPerception");
   // const UtScriptFunction* objectProtoPtr   = aScriptContextPtr->GetTypes()->GetPrototype("double", "WsfObject, WsfObject");

   if (trackProtoPtr == nullptr || platformProtoPtr == nullptr || geoPointProtoPtr == nullptr || assetProtoPtr == nullptr)
   {
      ut::log::error() << "WsfClusterManager: Invalid expected signature options.";
      assert(false);
   }

   if (trackProtoPtr != mScoringFunctionScriptPtr->mPrototype && platformProtoPtr != mScoringFunctionScriptPtr->mPrototype &&
       geoPointProtoPtr != mScoringFunctionScriptPtr->mPrototype && assetProtoPtr != mScoringFunctionScriptPtr->mPrototype)
   {
      auto out = ut::log::error() << "Invalid signature for cluster manager distance function.";
      out.AddNote() << "Actual: " << environmentPtr->GetPrototypeDescription(mScoringFunctionScriptPtr->mPrototype);

      auto note = out.AddNote() << "Expected One Of:";
      note.AddNote() << "double(WsfTrack, WsfTrack)";
      note.AddNote() << "double(WsfPlatform, WsfPlatform)";
      note.AddNote() << "double(WsfGeoPoint, WsfGeoPoint)";

      mScoringFunctionScriptPtr = nullptr;
      return false;
   }
   return true;
}

// virtual
bool WsfClusterManager::UserDistanceFunction(UtClusterObject* aObjectPtr, UtClusterObject* aObject2Ptr, double& aDistance)
{
   if (mScriptContextPtr == nullptr || mScoringFunctionScriptPtr == nullptr)
   {
      return false;
   }
   // reaching this point means we have a scoring function that takes two identical arguments (platform or track or
   // whatever) pack the script args with the right types, then call the script down below
   UtScriptDataList args;
   UtScriptData     retVal;
   if (aObjectPtr->Type() == "WsfTrack" && mScoringFunctionScriptPtr->mPrototype->mArgs[0] == "WsfTrack")
   {
      WsfTrack*      t1_Ptr    = dynamic_cast<WsfTrackClusterObject*>(aObjectPtr)->GetTrack();
      WsfTrack*      t2_Ptr    = dynamic_cast<WsfTrackClusterObject*>(aObject2Ptr)->GetTrack();
      UtScriptClass* tClassPtr = mScriptContextPtr->GetTypes()->GetClass("WsfTrack");
      args.push_back(UtScriptData(new UtScriptRef(t1_Ptr, tClassPtr)));
      args.push_back(UtScriptData(new UtScriptRef(t2_Ptr, tClassPtr)));
   }
   else if (aObjectPtr->Type() == "WsfPlatform" && mScoringFunctionScriptPtr->mPrototype->mArgs[0] == "WsfPlatform")
   {
      WsfPlatform*   p1_Ptr    = dynamic_cast<WsfPlatformClusterObject*>(aObjectPtr)->GetPlatform();
      WsfPlatform*   p2_Ptr    = dynamic_cast<WsfPlatformClusterObject*>(aObject2Ptr)->GetPlatform();
      UtScriptClass* tClassPtr = mScriptContextPtr->GetTypes()->GetClass("WsfPlatform");
      args.push_back(UtScriptData(new UtScriptRef(p1_Ptr, tClassPtr)));
      args.push_back(UtScriptData(new UtScriptRef(p2_Ptr, tClassPtr)));
   }
   else if (aObjectPtr->Type() == "WsfGeoPoint" && mScoringFunctionScriptPtr->mPrototype->mArgs[0] == "WsfGeoPoint")
   {
      WsfGeoPoint*   p1_Ptr    = dynamic_cast<WsfGeoPointClusterObject*>(aObjectPtr)->GetGeoPoint();
      WsfGeoPoint*   p2_Ptr    = dynamic_cast<WsfGeoPointClusterObject*>(aObject2Ptr)->GetGeoPoint();
      UtScriptClass* tClassPtr = mScriptContextPtr->GetTypes()->GetClass("WsfGeoPoint");
      args.push_back(UtScriptData(new UtScriptRef(p1_Ptr, tClassPtr)));
      args.push_back(UtScriptData(new UtScriptRef(p2_Ptr, tClassPtr)));
   }
   else if (aObjectPtr->Type() == "WsfAssetPerception" &&
            mScoringFunctionScriptPtr->mPrototype->mArgs[0] == "WsfAssetPerception")
   {
      WsfAssetPerception* p1_Ptr    = dynamic_cast<WsfAssetClusterObject*>(aObjectPtr)->GetAsset();
      WsfAssetPerception* p2_Ptr    = dynamic_cast<WsfAssetClusterObject*>(aObject2Ptr)->GetAsset();
      UtScriptClass*      tClassPtr = mScriptContextPtr->GetTypes()->GetClass("WsfAssetPerception");
      args.push_back(UtScriptData(new UtScriptRef(p1_Ptr, tClassPtr)));
      args.push_back(UtScriptData(new UtScriptRef(p2_Ptr, tClassPtr)));
   }
   else
   {
      ut::log::error() << "Cluster manager distance function arguments do not match objects being clustered.";
      return false;
   }
   UtScriptExecutor* execPtr = &GetSimulation()->GetScriptExecutor();
   mScriptContextPtr->Execute(execPtr, mScoringFunctionScriptPtr, retVal, args);
   aDistance = retVal.GetDouble();
   return true;
}
