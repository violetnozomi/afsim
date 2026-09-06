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

#ifndef WSFCLUSTERMANAGER_HPP
#define WSFCLUSTERMANAGER_HPP

#include "wsf_mil_export.h"

#include <limits>
#include <vector>

#include "UtCluster.hpp"
class UtScript;
class WsfAssetPerception;
class WsfDraw;
class WsfGeoPoint;
class WsfPlatform;
class WsfTrack;
#include "WsfObject.hpp"
#include "WsfTrackList.hpp"


class WSF_MIL_EXPORT WsfPlatformClusterObject : public UtClusterObject
{
public:
   WsfPlatformClusterObject(WsfPlatform* aPlatformPtr);
   ~WsfPlatformClusterObject() override
   {
      mPlatformPtr = nullptr;
   } // do not delete the pointer, it the object is managed elsewhere
   bool         VelocityValid() override;
   void         GetLocationWCS(double aLocationWCS[3]) override;
   void         GetLocationLLA(double& aLat, double& aLon, double& aAlt) override;
   void         GetVelocityWCS(double aVelocityWCS[3]) override;
   WsfPlatform* GetPlatform() { return mPlatformPtr; }
   // creates new objects in memory, must be deleted by caller
   static void GetObjects(const std::vector<WsfPlatform*>& platforms, std::vector<UtClusterObject*>& objects);
   static void GetPlatforms(std::vector<UtClusterObject*>& objects, std::vector<WsfPlatform*>& platforms);

private:
   WsfPlatform* mPlatformPtr;
};

class WSF_MIL_EXPORT WsfTrackClusterObject : public UtClusterObject
{
public:
   WsfTrackClusterObject(WsfTrack* aTrackPtr, double aSimTime);
   ~WsfTrackClusterObject() override
   {
      mTrackPtr = nullptr;
   } // do not delete the pointer, it the object is managed elsewhere
   bool      VelocityValid() override;
   void      GetLocationWCS(double aLocationWCS[3]) override;
   void      GetLocationLLA(double& aLat, double& aLon, double& aAlt) override;
   void      GetVelocityWCS(double aVelocityWCS[3]) override;
   WsfTrack* GetTrack() { return mTrackPtr; }
   // creates new objects in memory, must be deleted by caller
   static void GetObjects(double aSimTime, const std::vector<WsfTrack*>& tracks, std::vector<UtClusterObject*>& objects);
   static void GetObjects(double aSimTime, const std::vector<WsfLocalTrack*>& tracks, std::vector<UtClusterObject*>& objects);
   static void GetObjects(double aSimTime, WsfTrackList& tracks, std::vector<UtClusterObject*>& objects);
   static void GetObjects(double aSimTime, WsfLocalTrackList& tracks, std::vector<UtClusterObject*>& objects);
   static void GetTracks(std::vector<UtClusterObject*>& objects, std::vector<WsfTrack*>& tracks);
   static void GetTracks(std::vector<UtClusterObject*>& objects, std::vector<WsfLocalTrack*>& tracks);
   double      GetSimTime() { return mSimTime; }

private:
   WsfTrack* mTrackPtr;
   double    mSimTime;
};

class WSF_MIL_EXPORT WsfGeoPointClusterObject : public UtClusterObject
{
public:
   WsfGeoPointClusterObject(WsfGeoPoint* aTrackPtr);
   ~WsfGeoPointClusterObject() override
   {
      mGeoPointPtr = nullptr;
   } // do not delete the pointer, it the object is managed elsewhere
   bool         VelocityValid() override;
   void         GetLocationWCS(double aLocationWCS[3]) override;
   void         GetLocationLLA(double& aLat, double& aLon, double& aAlt) override;
   void         GetVelocityWCS(double aVelocityWCS[3]) override;
   WsfGeoPoint* GetGeoPoint() { return mGeoPointPtr; }
   // creates new objects in memory, must be deleted by caller
   static void GetObjects(const std::vector<WsfGeoPoint*>& geoPoints, std::vector<UtClusterObject*>& objects);
   static void GetGeoPoints(std::vector<UtClusterObject*>& objects, std::vector<WsfGeoPoint*>& geoPoints);

private:
   WsfGeoPoint* mGeoPointPtr;
};

class WSF_MIL_EXPORT WsfAssetClusterObject : public UtClusterObject
{
public:
   WsfAssetClusterObject(WsfAssetPerception* aAssetPtr);
   ~WsfAssetClusterObject() override
   {
      mAssetPtr = nullptr;
   } // do not delete the pointer, it the object is managed elsewhere
   bool                VelocityValid() override;
   void                GetLocationWCS(double aLocationWCS[3]) override;
   void                GetLocationLLA(double& aLat, double& aLon, double& aAlt) override;
   void                GetVelocityWCS(double aVelocityWCS[3]) override;
   WsfAssetPerception* GetAsset() { return mAssetPtr; }
   // creates new objects in memory, must be deleted by caller
   static void GetObjects(const std::vector<WsfAssetPerception*>& assets, std::vector<UtClusterObject*>& objects);
   static void GetAssets(std::vector<UtClusterObject*>& objects, std::vector<WsfAssetPerception*>& assets);

private:
   WsfAssetPerception* mAssetPtr;
};


//! A WsfClusterManager is a cluster creator & maintainer.
//!
//! WsfClusterManager provides the processing desired for dynamic clustering.
//! It is used to form clusters from an array or list of items.
//! Cluster algorithms implemented at this time:
//! <ul>
//! <li> K-Means Clustering.
//! <li> Hierarchical Tree Clustering, single-linkage.
//! <li> Hierarchical Tree Clustering, complete-linkage.
//! <li> Hierarchical Tree Clustering, average-linkage.
//! </ul>
//!
//! @see WsfCluster
class WSF_MIL_EXPORT WsfClusterManager : public UtClusterManager
{
public:
   // typedef typename UtClusterManager::UtCluster UtCluster;

   ////! The manager's clustering mode; it specifies which algorithm will be used.
   // see UtCluster : UtClusterManager::ClusteringMethod

   ////! The manager's method for calculating distance.
   // see UtCluster : UtClusterManager::DistanceFunction

   WsfClusterManager(WsfSimulation&                     aSimulation,
                     size_t                             aNumClusters = 0,
                     UtClusterManager::ClusteringMethod aMethod      = UtClusterManager::cK_MEANS,
                     UtClusterManager::DistanceFunction aDistType    = UtClusterManager::cPOSITION_ONLY);

   ~WsfClusterManager() override = default;

   //! @name 'clustering' methods.
   //@{
   std::vector<std::vector<WsfTrack*>>           GetTrackClusters(WsfTrackList& aList);
   std::vector<std::vector<WsfLocalTrack*>>      GetTrackClusters(WsfLocalTrackList& aList);
   std::vector<std::vector<WsfTrack*>>           GetTrackClusters(const std::vector<WsfTrack*>& aList);
   std::vector<std::vector<WsfLocalTrack*>>      GetTrackClusters(const std::vector<WsfLocalTrack*>& aList);
   std::vector<std::vector<WsfPlatform*>>        GetPlatformClusters(const std::vector<WsfPlatform*>& aList);
   std::vector<std::vector<WsfGeoPoint*>>        GetGeoPointClusters(const std::vector<WsfGeoPoint*>& aList);
   std::vector<std::vector<WsfAssetPerception*>> GetAssetClusters(const std::vector<WsfAssetPerception*>& aList);
   //@}

   void Delete(std::vector<UtClusterObject*>& objects);

   //! @name 'utility' methods.
   //@{
   unsigned int UniqueId(const std::vector<WsfTrack*>& aList);
   unsigned int UniqueId(const std::vector<WsfPlatform*>& aList);
   unsigned int UniqueId(const std::vector<WsfGeoPoint*>& aList);
   unsigned int UniqueId(const std::vector<WsfAssetPerception*>& aList);

   std::vector<WsfTrack*>           ConvexHull(const std::vector<WsfTrack*>& aList);
   std::vector<WsfPlatform*>        ConvexHull(const std::vector<WsfPlatform*>& aList);
   std::vector<WsfGeoPoint*>        ConvexHull(const std::vector<WsfGeoPoint*>& aList);
   std::vector<WsfAssetPerception*> ConvexHull(const std::vector<WsfAssetPerception*>& aList);

   WsfGeoPoint MeanLocation(const std::vector<WsfTrack*>& aList);
   WsfGeoPoint MeanLocation(const std::vector<WsfPlatform*>& aList);
   WsfGeoPoint MeanLocation(const std::vector<WsfGeoPoint*>& aList);
   WsfGeoPoint MeanLocation(const std::vector<WsfAssetPerception*>& aList);

   WsfTrack*           NearestMember(const std::vector<WsfTrack*>& aList, WsfGeoPoint& toPoint);
   WsfPlatform*        NearestMember(const std::vector<WsfPlatform*>& aList, WsfGeoPoint& toPoint);
   WsfGeoPoint*        NearestMember(const std::vector<WsfGeoPoint*>& aList, WsfGeoPoint& toPoint);
   WsfAssetPerception* NearestMember(const std::vector<WsfAssetPerception*>& aList, WsfGeoPoint& toPoint);

   void NearestEdgePoint(const std::vector<WsfTrack*>& aList,
                         WsfGeoPoint&                  toPoint,
                         WsfGeoPoint&                  edgePoint,
                         size_t&                       edgeIndex1,
                         size_t&                       edgeIndex2);
   void NearestEdgePoint(const std::vector<WsfPlatform*>& aList,
                         WsfGeoPoint&                     toPoint,
                         WsfGeoPoint&                     edgePoint,
                         size_t&                          edgeIndex1,
                         size_t&                          edgeIndex2);
   void NearestEdgePoint(const std::vector<WsfGeoPoint*>& aList,
                         WsfGeoPoint&                     toPoint,
                         WsfGeoPoint&                     edgePoint,
                         size_t&                          edgeIndex1,
                         size_t&                          edgeIndex2);
   void NearestEdgePoint(const std::vector<WsfAssetPerception*>& aList,
                         WsfGeoPoint&                            toPoint,
                         WsfGeoPoint&                            edgePoint,
                         size_t&                                 edgeIndex1,
                         size_t&                                 edgeIndex2);

   void CrossLength(const std::vector<WsfTrack*>& aList,
                    WsfGeoPoint&                  refPoint,
                    double&                       dist1,
                    WsfGeoPoint&                  leftPt,
                    WsfGeoPoint&                  rightPt);
   void CrossLength(const std::vector<WsfPlatform*>& aList,
                    WsfGeoPoint&                     refPoint,
                    double&                          dist1,
                    WsfGeoPoint&                     leftPt,
                    WsfGeoPoint&                     rightPt);
   void CrossLength(const std::vector<WsfGeoPoint*>& aList,
                    WsfGeoPoint&                     refPoint,
                    double&                          dist1,
                    WsfGeoPoint&                     leftPt,
                    WsfGeoPoint&                     rightPt);
   void CrossLength(const std::vector<WsfAssetPerception*>& aList,
                    WsfGeoPoint&                            refPoint,
                    double&                                 dist1,
                    WsfGeoPoint&                            leftPt,
                    WsfGeoPoint&                            rightPt);

   enum DrawType
   {
      HULL,
      STAR
   };
   void Draw(const std::vector<WsfTrack*>& aList, double duration, UtVec3d& color, DrawType type = HULL);
   void Draw(const std::vector<WsfPlatform*>& aList, double duration, UtVec3d& color, DrawType type = HULL);
   void Draw(const std::vector<WsfGeoPoint*>& aList, double duration, UtVec3d& color, DrawType type = HULL);
   void Draw(const std::vector<WsfAssetPerception*>& aList, double duration, UtVec3d& color, DrawType type = HULL);

   void DrawHull(std::vector<UtClusterObject*>& cluster, double duration, UtVec3d& color);
   void DrawStar(std::vector<UtClusterObject*>& cluster, double duration, UtVec3d& color);

   //@}


   //! @name 'configuration' methods.
   //@{
   bool SetUserDistanceFunction(UtScriptContext* aScriptContextPtr, const std::string& aScoringFunctionName);
   bool UserDistanceFunction(UtClusterObject* aObjectPtr, UtClusterObject* aObject2Ptr, double& aDistance) override;
   //@}

   WsfSimulation* GetSimulation() const { return mSimulationPtr; }

private:
   WsfSimulation* mSimulationPtr;

   //! pointer for debug drawing
   WsfDraw* mDebugDrawPtr;

   UtScriptContext* mScriptContextPtr;
   UtScript*        mScoringFunctionScriptPtr;
};

#endif
