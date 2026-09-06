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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef WSFROUTENETWORK_HPP
#define WSFROUTENETWORK_HPP

#include "wsf_export.h"

#include <memory>
#include <string>
#include <vector>

#include "UtGraph.hpp"
class UtInput;
#include "UtLineSegment.hpp"
class UtScriptClass;
class UtScriptTypes;
#include "UtSpatialTree.hpp"
#include "UtSphericalEarth.hpp"
#include "UtVec3.hpp"
#include "WsfObject.hpp"
#include "WsfRoute.hpp"
class WsfRouteNetworkTypes;
#include "WsfWaypoint.hpp"


//!   A collection of WsfRoute object that represents a connected network (such as a road network).
class WSF_EXPORT WsfRouteNetwork : public WsfObject
{
public:
   WsfStringId cSTART_NODE_ID;
   WsfStringId cSTOP_NODE_ID;

   //! A waypoint address.  Minimal information required to look up a waypoint.
   using WaypointAddr     = std::pair<int, int>;
   using WaypointAddrList = std::vector<WaypointAddr>;

   //! Specifies a location along a route
   class RoutePoint
   {
   public:
      RoutePoint() = default;
      RoutePoint(const WaypointAddr& aAddr, double aDistanceAlongSegment = 0)
         : mWaypointAddr(aAddr)
         , mDistanceAlongSegment(aDistanceAlongSegment)
      {
      }

      int RouteIndex() const { return mWaypointAddr.first; }
      int WaypointIndex() const { return mWaypointAddr.second; }

      //! Waypoint and route index
      WaypointAddr mWaypointAddr;

      //! Distance from the waypoint addressed toward the next waypoint
      double mDistanceAlongSegment = 0.0;
   };

   WsfRouteNetwork();
   WsfRouteNetwork& operator=(const WsfRouteNetwork&) = delete;
   ~WsfRouteNetwork() override;

   static std::unique_ptr<UtScriptClass> CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   WsfRouteNetwork* Clone() const override;

   bool ProcessInput(UtInput& aInput) override;

   bool Initialize();

   WSF_DEPRECATED bool Add(WsfRoute* aRoute);
   bool                Add(std::unique_ptr<WsfRoute> aRoutePtr);

   bool AppendShortestPathOnNetwork(RoutePoint aPoint1, RoutePoint aPoint2, WsfRoute& aPath);

   bool GenerateShortestPathBetweenWaypoints(const WsfWaypoint& aFromWaypoint,
                                             const WsfWaypoint& aToWaypoint,
                                             WsfRoute&          aPath);

   bool GeneratePathOffRouteToRoute(double aStartLat, double aStartLon, double aEndLat, double aEndLon, WsfRoute& aPath);

   bool GeneratePathBetweenClosestWaypoints(double aStartLat, double aStartLon, double aEndLat, double aEndLon, WsfRoute& aPath);

   bool GeneratePathBetweenClosestSegments(double    aStartLat,
                                           double    aStartLon,
                                           double    aEndLat,
                                           double    aEndLon,
                                           WsfRoute& aPath,
                                           bool      aLeaveRouteNetwork = true);

   bool FindClosestWaypoint(double aLat, double aLon, WsfWaypoint& aWpt, double& aDistance);

   bool FindClosestSegment(double aLat, double aLon, RoutePoint& aProjectedPoint, double& aDistanceToSegment);

   unsigned int GetRouteCount() const { return static_cast<unsigned int>(mRoutes.size()); }

   //! Return a reference to the route by index in the network.
   WsfRoute& operator[](unsigned int aIndex);

   //! Returns the list of routes in the route network.
   std::vector<const WsfRoute*> GetRoutes() const;

   void Append(const RoutePoint& aPoint, WsfRoute& aRoute);
   void AppendSubroute(const RoutePoint& aPoint1, const RoutePoint& aPoint2, WsfRoute& aRoute);
   int  GetAdjacentNodeIndices(const RoutePoint& aPoint, int* aIndices);

   // The cost functor needs access to the 'GetRouteIndex' and 'GetNodeIndex' methods.
   friend struct RouteCostFunc;

   void BuildQuadtree();

   static bool CompareWaypointAddrRoute(const WaypointAddr& aLhs, const WaypointAddr& aRhs)
   {
      return aLhs.first < aRhs.first;
   }

   //! Represents a node key for use with the RouteGraph class.
   struct RouteNode
   {
      RouteNode()
         : mSegmentNode(false)
         , mId()
         , mWaypoints(nullptr)
      {
      }

      explicit RouteNode(WsfStringId aId, WaypointAddrList* aWptListPtr = nullptr)
         : mSegmentNode(false)
         , mId(aId)
         , mWaypoints(aWptListPtr)
      {
      }

      bool operator==(const RouteNode& aRhs) const { return (mId == aRhs.mId); }

      bool operator!=(const RouteNode& aRhs) const { return (mId != aRhs.mId); }

      bool              operator<(const RouteNode& aRhs) const { return (mId < aRhs.mId); }
      bool              mSegmentNode; // Does this node represent a segment (and not just a point)
      WsfStringId       mId;          // Nodes unique id.
      WaypointAddrList* mWaypoints;   // List of waypoints addresses which share this node
   };

   //! Represents an edge key for use with the RouteGraph class.
   struct RouteEdge
   {
      RouteEdge()
         : mSrcNode()
         , mDstNode()
         , mDistance(0.0)
      {
      }

      RouteEdge(const RouteNode& aSrcNode, const RouteNode& aDstNode)
         : mSrcNode(aSrcNode)
         , mDstNode(aDstNode)
         , mDistance(0.0)
      {
      }

      bool operator==(const RouteEdge& aRhs) const
      {
         return ((mSrcNode == aRhs.mSrcNode) && (mDstNode == aRhs.mDstNode));
      }

      RouteNode mSrcNode;
      RouteNode mDstNode;
      double    mDistance;
   };

   using RouteGraph = UtGraphT<RouteNode, RouteEdge, false, double>;

   const RouteGraph& GetGraph() const { return mGraph; }

   //! A cost function used by the shortest_path algorithm provided by the RouteGraph class.
   class RouteCostFunc : public RouteGraph::cost_func
   {
   public:
      RouteCostFunc()
         : mNetworkPtr(nullptr)
      {
      }
      RouteCostFunc(WsfRouteNetwork* aNetworkPtr)
         : mNetworkPtr(aNetworkPtr)
      {
      }

      //! Returns the precomputed cost along the edge
      double operator()(const RouteEdge& aEdge, const RouteNode& /*aNode1*/, const RouteNode& /*aNode2*/) const override
      {
         return aEdge.mDistance;
      }

      //! Computes a lower-bound cost to move from aNode1 to aNode2 (a straight (curved) line)
      double operator()(const RouteNode& aNode1, const RouteNode& aNode2) const override
      {
         double                        cost = 0.0;
         WsfRouteNetwork::WaypointAddr wa1  = aNode1.mWaypoints->front();
         WsfRouteNetwork::WaypointAddr wa2  = aNode2.mWaypoints->front();
         if (wa1 != wa2)
         {
            // Get the WsfWaypoint objects that correspond to both nodes.

            const WsfWaypoint& n1 = (*mNetworkPtr)[wa1.first][wa1.second];
            const WsfWaypoint& n2 = (*mNetworkPtr)[wa2.first][wa2.second];

            // Calculate the great circle distance between the two nodes.

            double heading;
            double distance;
            UtSphericalEarth::GreatCircleHeadingAndDistance(n1.GetLat(), n1.GetLon(), n2.GetLat(), n2.GetLon(), heading, distance);
            cost = distance;
         }
         return cost;
      }

      WsfRouteNetwork* mNetworkPtr;
   };

protected:
   // Copy constructor (for Clone());
   WsfRouteNetwork(const WsfRouteNetwork& aSrc);

private:
   // this class sits in the quad-tree and provides query tests
   class SegmentDatum : public UtQuadDatum
   {
   public:
      SegmentDatum(const UtLineSegment& aSegment);
      ~SegmentDatum() override;

      bool IsPoint() const override { return false; }

      bool IsWhollyContainedIn(const UtBoundingBox& aBox) const override;

      bool IsPartiallyContainedIn(const UtBoundingBox& aBox) const override;

      const UtBoundingBoxI<2> GetBoundingBox() const { return mBoundingBox; }

      unsigned int      mRouteIndex;
      int               mWaypointIndex;
      UtLineSegment     mSegment;
      UtBoundingBoxI<2> mBoundingBox;
   };

   using Path = std::vector<RouteNode>;

   bool FindShortestPath(WsfStringId aSrcNodeId, WsfStringId aDstNodeId, Path& aPath, double& aCost) const;


   bool BuildRoute(const Path& aShortestPath, int aStartIndex, int aStopIndex, WsfRoute& aPath);

   void AppendSubroute(const WsfRouteNetwork::RouteNode& aStartingWaypoint,
                       const WsfRouteNetwork::RouteNode& aEndingWaypoint,
                       WsfRoute&                         aPath);
   void AppendSubroute(const WsfRoute& aSourceRoute, int aStartWptIndex, int aStopWptIndex, WsfRoute& aAppendRoute);

   void InitializeShortestPath();

   void TestNetwork();

   std::vector<ut::CloneablePtr<WsfRoute>> mRoutes;    // The routes in the network
   std::vector<WsfWaypoint>                mNwaypoint; // Waypoint location of node
   // Graph representation needed for shortest path computations.

   RouteGraph    mGraph;
   RouteCostFunc mCostFunc;

   // The following data is needed for shortest path computations.
   bool mShortestPathInitialized;
   bool mVerbose;
   bool mShowRoutes;

   UtQuadTree* mTreePtr;
   double      mMaxLat;
   double      mMinLat;
   double      mMaxLon;
   double      mMinLon;
   double      mCenterLLA[3];
   int         mTreeDepth;

   double mTransECEF[3][3];
   double mRefECEF[3];

   bool mInitialized;
   int  mNodeId;
};

#endif
