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

#include "WsfRouteNetwork.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <functional>
#include <limits>
#include <queue>
#include <sstream>

#include "UtCast.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLatPos.hpp"
#include "UtLog.hpp"
#include "UtLonPos.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtSphericalEarth.hpp"
#include "UtVec2.hpp"
#include "UtWallClock.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfRoute.hpp"
#include "WsfRouteNetworkTypes.hpp"
#include "WsfRouteTypes.hpp"
#include "WsfScenario.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptObjectClass.hpp"

// =================================================================================================
WsfRouteNetwork::WsfRouteNetwork()
   : WsfObject()
   , cSTART_NODE_ID("__node_start")
   , cSTOP_NODE_ID("__node_stop")
   , mShortestPathInitialized(false)
   , mVerbose(false)
   , mShowRoutes(false)
   , mTreePtr(nullptr)
   , mMaxLat(0.0)
   , mMinLat(0.0)
   , mMaxLon(0.0)
   , mMinLon(0.0)
   , mTreeDepth(8)
   , mInitialized(false)
   , mNodeId(1)
{
   for (size_t i = 0; i < 3; ++i)
   {
      mCenterLLA[i] = 0.0;
      mRefECEF[i]   = 0.0;
      for (size_t j = 0; j < 3; ++j)
      {
         mTransECEF[i][j] = 0.0;
      }
   }

   mCostFunc.mNetworkPtr = this;
}

// =================================================================================================
//! Copy constructor (for Clone())
// NOTE: Only the basic input data should be cloned here. Other data that
// is created by Initialize() must be set to the construction values because
// the quad tree object does not support copy construction. In theory we could
// use shared data for a given route network and simply copy pointers, but that
// will come at a latter time.
WsfRouteNetwork::WsfRouteNetwork(const WsfRouteNetwork& aSrc)
   : WsfObject(aSrc)
   , cSTART_NODE_ID(aSrc.cSTART_NODE_ID)
   , cSTOP_NODE_ID(aSrc.cSTOP_NODE_ID)
   , mRoutes(aSrc.mRoutes)
   , mNwaypoint(aSrc.mNwaypoint)
   , mGraph(aSrc.mGraph)
   , mCostFunc(aSrc.mCostFunc)
   , mShortestPathInitialized(false)
   , mVerbose(aSrc.mVerbose)
   , mShowRoutes(aSrc.mShowRoutes)
   , mTreePtr(nullptr)
   , mMaxLat(0.0)
   , mMinLat(0.0)
   , mMaxLon(0.0)
   , mMinLon(0.0)
   , mTreeDepth(aSrc.mTreeDepth)
   , mInitialized(false)
   , // Force reinitialization on first use
   mNodeId(1)
{
   mCostFunc.mNetworkPtr = this;

   for (size_t i = 0; i < 3; ++i)
   {
      mCenterLLA[i] = 0.0; // reset in Initialize
      mRefECEF[i]   = 0.0; // reset in Initialize
      for (size_t j = 0; j < 3; ++j)
      {
         mTransECEF[i][j] = 0.0; // reset in Initialize
      }
   }
}

// =================================================================================================
// virtual
WsfRouteNetwork::~WsfRouteNetwork()
{
   for (auto& graph : mGraph)
   {
      delete graph.first.mWaypoints;
   }

   delete mTreePtr;
}

// =================================================================================================
// virtual
WsfRouteNetwork* WsfRouteNetwork::Clone() const
{
   return new WsfRouteNetwork(*this);
}

// =================================================================================================
// virtual
bool WsfRouteNetwork::Initialize()
{
   if (!mInitialized)
   {
      mCenterLLA[0] = (mMinLat + mMaxLat) / 2.0;
      mCenterLLA[1] = (mMinLon + mMaxLon) / 2.0;
      mCenterLLA[2] = 0.0;
      BuildQuadtree();
      mInitialized = true;
   }
   return true;
}

// =================================================================================================
// virtual
bool WsfRouteNetwork::ProcessInput(UtInput& aInput)
{
   bool        myCommand = false;
   std::string command(aInput.GetCommand());

   std::unique_ptr<WsfRoute> newRoutePtr = nullptr;
   if (WsfRouteTypes::Get(WsfScenario::FromInput(aInput)).LoadInstance(aInput, newRoutePtr))
   {
      if (!Add(std::move(newRoutePtr)))
      {
         auto out = ut::log::warning() << "Route not added to route_network " << GetName();
         out.AddNote() << "Does route have at least two waypoints?";
         out.AddNote() << "Location: " << aInput.GetLocation();
      }
      myCommand = true;
   }
   else if (command == "test")
   {
      myCommand = true;
      TestNetwork();
   }
   else if (command == "test_nodes")
   {
      myCommand = true;
      std::string fromNodeId;
      std::string toNodeId;
      aInput.ReadValue(fromNodeId);
      aInput.ReadValue(toNodeId);
      InitializeShortestPath();
      double cost = 0.0;
      Path   path;
      FindShortestPath(fromNodeId, toNodeId, path, cost);
      auto out = ut::log::info() << "Route network 'test_nodes':";
      out.AddNote() << "From Node: " << fromNodeId;
      out.AddNote() << "To Node: " << toNodeId;
      out.AddNote() << "Cost: " << cost;
      auto pathNote = out.AddNote() << "Path:";
      for (auto& p : path)
      {
         pathNote.AddNote() << p.mId;
      }
   }
   else if (command == "verbose")
   {
      mVerbose  = true;
      myCommand = true;
   }
   return myCommand;
}

// =================================================================================================
// DEPRECATED as of 2.9
bool WsfRouteNetwork::Add(WsfRoute* aRoutePtr)
{
   return Add(std::unique_ptr<WsfRoute>(aRoutePtr));
}

// =================================================================================================
bool WsfRouteNetwork::Add(std::unique_ptr<WsfRoute> aRoutePtr)
{
   if (mShortestPathInitialized)
   {
      return false;
   }
   if (aRoutePtr->GetSize() < 2)
   {
      return false;
   }
   auto routeId = static_cast<unsigned int>(mRoutes.size());
   aRoutePtr->SetNetworkRouteId(routeId);
   if (aRoutePtr->GetNameId() == nullptr)
   {
      // Assign a default name based on its' position in the network.
      std::ostringstream oss;
      oss << "segment_" << routeId + 1;
      aRoutePtr->SetName(oss.str());
   }

   // Update the Lat/Lon bounding box
   const std::vector<WsfWaypoint>& waypoints = aRoutePtr->GetWaypoints();
   if (mMinLat == mMaxLat && mMinLat == 0)
   {
      mMinLat = mMaxLat = waypoints[0].GetLat();
      mMinLon = mMaxLon = waypoints[0].GetLon();
   }

   for (size_t i = 1; i < waypoints.size(); ++i)
   {
      const WsfWaypoint& wpt = waypoints[i];
      mMinLat                = std::min(mMinLat, wpt.GetLat());
      mMinLon                = std::min(mMinLon, wpt.GetLon());
      mMaxLat                = std::max(mMaxLat, wpt.GetLat());
      mMaxLon                = std::max(mMaxLon, wpt.GetLon());
   }

   auto startWpt = aRoutePtr->Begin();
   auto endWpt   = aRoutePtr->End();
   auto testWpt  = startWpt;
   auto lastWpt  = startWpt;
   // move the lastWpt iterator to the last waypoint
   while (testWpt != endWpt)
   {
      lastWpt = testWpt++;
   }

   // add a nodeId to each end of a route.  If end waypoint is equal to end waypoint in another route use same nodeId
   std::vector<WsfWaypoint>::iterator nWayit;
   if ((nWayit = find(mNwaypoint.begin(), mNwaypoint.end(), *startWpt)) == mNwaypoint.end())
   {
      // make sure nodeId is not already set
      if (startWpt->GetNodeId() == "")
      {
         std::ostringstream oss;
         oss << cSTART_NODE_ID << "_" << mNodeId++;
         startWpt->SetNodeId(WsfStringId(oss.str()));
      }
      mNwaypoint.push_back((*startWpt));
   }
   else
   {
      startWpt->SetNodeId(nWayit->GetNodeId());
   }
   if ((nWayit = find(mNwaypoint.begin(), mNwaypoint.end(), *lastWpt)) == mNwaypoint.end())
   {
      if (lastWpt->GetNodeId() == "")
      {
         std::ostringstream oss;
         oss << cSTOP_NODE_ID << "_" << mNodeId++;
         lastWpt->SetNodeId(WsfStringId(oss.str()));
      }
      mNwaypoint.push_back((*lastWpt));
   }
   else
   {
      lastWpt->SetNodeId(nWayit->GetNodeId());
   }
   mRoutes.emplace_back(std::move(aRoutePtr));
   return true;
}

// =================================================================================================
bool WsfRouteNetwork::AppendShortestPathOnNetwork(RoutePoint aPoint1, RoutePoint aPoint2, WsfRoute& aPath)
{
   bool ok = false;
   InitializeShortestPath();

   // If points are on the same segment, just return the segment piece
   if (aPoint1.mWaypointAddr == aPoint2.mWaypointAddr)
   {
      Append(aPoint1, aPath);
      Append(aPoint2, aPath);
      return true;
   }

   // Find all nearest intersections, a maximum of two for each endpoint
   WsfRoute&          startRoute    = *mRoutes[aPoint1.RouteIndex()];
   int                startWptIndex = aPoint1.WaypointIndex();
   const WsfWaypoint& fromWpt       = startRoute[startWptIndex];
   int                startNodeIndices[2];
   int                startNodeCount = GetAdjacentNodeIndices(aPoint1, startNodeIndices);

   WsfRoute&          endRoute    = *mRoutes[aPoint2.RouteIndex()];
   int                endWptIndex = aPoint2.WaypointIndex();
   const WsfWaypoint& endWpt      = endRoute[endWptIndex];
   int                endNodeIndices[2];
   int                endNodeCount = GetAdjacentNodeIndices(aPoint2, endNodeIndices);

   if (startNodeCount == 0 || endNodeCount == 0)
   {
      // we have an isolated segment, can only path if waypoints share the same route
      if (aPoint1.RouteIndex() == aPoint2.RouteIndex())
      {
         AppendSubroute(aPoint1, aPoint2, aPath);
         ok = true;
      }
   }
   else
   {
      // Update the graph to contain our begin and end nodes, adding edges to nearest route nodes.
      RouteNode startNode(cSTART_NODE_ID + "_" + WsfStringInt(mNodeId++), new WaypointAddrList);
      startNode.mWaypoints->push_back(WaypointAddr(fromWpt.GetRouteId(), fromWpt.GetPositionInRoute()));
      startNode.mSegmentNode = (aPoint1.mDistanceAlongSegment > 0);

      RouteNode stopNode(cSTOP_NODE_ID + "_" + WsfStringInt(mNodeId++), new WaypointAddrList);
      stopNode.mWaypoints->push_back(WaypointAddr(endWpt.GetRouteId(), endWpt.GetPositionInRoute()));
      stopNode.mSegmentNode = (aPoint2.mDistanceAlongSegment > 0);

      auto startIter = mGraph.insert(startNode);
      for (int i = 0; i < startNodeCount; ++i)
      {
         int          wptIndex = startNodeIndices[i];
         WsfWaypoint& nodeWpt  = startRoute[wptIndex];
         double       distance =
            fabs(fromWpt.GetDistanceAlongRoute() + aPoint1.mDistanceAlongSegment - nodeWpt.GetDistanceAlongRoute());
         auto      nodeIter = mGraph.find(RouteNode(nodeWpt.GetNodeId()));
         RouteEdge edge(startNode, nodeIter->first);
         edge.mDistance = distance;
         mGraph.insert_edge(startIter, nodeIter, edge);
      }

      auto endIter = mGraph.insert(stopNode);
      for (int i = 0; i < endNodeCount; ++i)
      {
         int          wptIndex = endNodeIndices[i];
         WsfWaypoint& nodeWpt  = endRoute[wptIndex];
         double       distance =
            fabs(endWpt.GetDistanceAlongRoute() + aPoint2.mDistanceAlongSegment - nodeWpt.GetDistanceAlongRoute());
         auto      nodeIter = mGraph.find(RouteNode(nodeWpt.GetNodeId()));
         RouteEdge edge(stopNode, nodeIter->first);
         edge.mDistance = distance;
         mGraph.insert_edge(endIter, nodeIter, edge);
      }

      double cost = 0.0;
      Path   bestPath;
      ok = mGraph.shortest_path(startIter, endIter, bestPath, cost, &mCostFunc);
      if (ok)
      {
         // If source and destination is on the same route,
         // The quickest route may be to not use any intersections.
         // Check this case here:
         bool switchingRoutes = true;
         if (fromWpt.GetRouteId() == endWpt.GetRouteId())
         {
            double noIntersectionCost = fabs((fromWpt.GetDistanceAlongRoute() + aPoint1.mDistanceAlongSegment) -
                                             (endWpt.GetDistanceAlongRoute() + aPoint2.mDistanceAlongSegment));
            if (noIntersectionCost < cost)
            {
               switchingRoutes = false;
               AppendSubroute(aPoint1, aPoint2, aPath);
            }
         }
         if (switchingRoutes)
         {
            if (bestPath.size() >= 2)
            {
               Append(aPoint1, aPath);
               BuildRoute(bestPath, 0, static_cast<int>(bestPath.size()), aPath);
               Append(aPoint2, aPath);
            }
            else
            {
               Append(aPoint1, aPath);
               Append(aPoint2, aPath);
            }
         }
      }

      // Clean up temporary nodes/edges

      delete startIter->first.mWaypoints;
      delete endIter->first.mWaypoints;
      mGraph.erase_all_edges(startIter);
      mGraph.erase_all_edges(endIter);
      mGraph.erase(startIter);
      mGraph.erase(endIter);
   }
   return ok;
}

// =================================================================================================
bool WsfRouteNetwork::GenerateShortestPathBetweenWaypoints(const WsfWaypoint& aFromWaypoint,
                                                           const WsfWaypoint& aToWaypoint,
                                                           WsfRoute&          aPath)
{
   aPath.Clear();
   WsfRoute& startRoute    = *mRoutes[aFromWaypoint.GetRouteId()];
   int       startWptIndex = startRoute.FindWaypointIndex(aFromWaypoint);
   WsfRoute& endRoute      = *mRoutes[aToWaypoint.GetRouteId()];
   int       endWptIndex   = endRoute.FindWaypointIndex(aToWaypoint);
   return AppendShortestPathOnNetwork(RoutePoint(WaypointAddr(aFromWaypoint.GetRouteId(), startWptIndex), 0.0),
                                      RoutePoint(WaypointAddr(aToWaypoint.GetRouteId(), endWptIndex), 0.0),
                                      aPath);
}

// =================================================================================================
//! Creates the following path:
//! (aStartLat, aStartLon) -> ShortestPath(Wpt1, Wpt2) -> (aEndLat, aEndLon)
//! where,
//! Wpt1 - the closest waypoint to (aStartLat, aStartLon)
//! Wpt2 - the closest waypoint to (aEndLat, aEndLon)
//! ShortestPath(Wpt1, Wpt2) - the shortest path between Wpt1 to Wpt2
//! including both Wpt1 and Wpt2.
bool WsfRouteNetwork::GeneratePathOffRouteToRoute(double    aStartLat,
                                                  double    aStartLon,
                                                  double    aEndLat,
                                                  double    aEndLon,
                                                  WsfRoute& aPath)
{
   bool     haveRoute = false;
   WsfRoute tempRoute;

   InitializeShortestPath();

   if (GeneratePathBetweenClosestWaypoints(aStartLat, aStartLon, aEndLat, aEndLon, tempRoute))
   {
      aPath.Clear();

      double distance = 0.1;
      double heading;

      if (!tempRoute.Empty())
      {
         UtSphericalEarth::GreatCircleHeadingAndDistance(aStartLat,
                                                         aStartLon,
                                                         tempRoute[0].GetLat(),
                                                         tempRoute[0].GetLon(),
                                                         heading,
                                                         distance);
      }
      // If the start waypoint is really close to the first waypoint in the path, don't append it.
      if (distance >= 0.1)
      {
         WsfWaypoint startWaypoint;
         startWaypoint.SetLat(aStartLat);
         startWaypoint.SetLon(aStartLon);
         aPath.Append(startWaypoint);
      }

      // Append the path.
      aPath.Append(tempRoute);

      distance       = 0.1;
      const int last = aPath.GetSize() - 1;
      if (!tempRoute.Empty())
      {
         UtSphericalEarth::GreatCircleHeadingAndDistance(aEndLat,
                                                         aEndLon,
                                                         aPath[last].GetLat(),
                                                         aPath[last].GetLon(),
                                                         heading,
                                                         distance);
      }
      // If the last waypoint is really close to the last waypoint in the path, don't append it.
      if (distance >= 0.1)
      {
         WsfWaypoint endWaypoint;
         endWaypoint.SetLat(aEndLat);
         endWaypoint.SetLon(aEndLon);
         aPath.Append(endWaypoint);
      }
      haveRoute = true;
   }
   return haveRoute;
}

// =================================================================================================
//! Creates the following path:
//! ShortestPath(Wpt1, Wpt2)
//! where,
//! Wpt1 - the closest waypoint to (aStartLat, aStartLon)
//! Wpt2 - the closest waypoint to (aEndLat, aEndLon)
//! ShortestPath(Wpt1, Wpt2) - the shortest path between Wpt1 to Wpt2
//! including both Wpt1 and Wpt2.
bool WsfRouteNetwork::GeneratePathBetweenClosestWaypoints(double    aStartLat,
                                                          double    aStartLon,
                                                          double    aEndLat,
                                                          double    aEndLon,
                                                          WsfRoute& aPath)
{
   InitializeShortestPath();

   bool        haveRoute = false;
   WsfWaypoint startingWaypoint;
   WsfWaypoint endingWaypoint;
   double      distanceToWpt;

   if (FindClosestWaypoint(aStartLat, aStartLon, startingWaypoint, distanceToWpt) &&
       FindClosestWaypoint(aEndLat, aEndLon, endingWaypoint, distanceToWpt))
   {
      aPath.Clear();
      haveRoute = GenerateShortestPathBetweenWaypoints(startingWaypoint, endingWaypoint, aPath);
   }
   return haveRoute;
}

// =================================================================================================
//! Creates the following path:
//! ShortestPath(Wpt1, Wpt2)
//! where,
//! Wpt1 - the closest waypoint or segment normal projection to (aStartLat, aStartLon)
//! Wpt2 - the closest waypoint or segment normal projection to (aEndLat, aEndLon)
//! ShortestPath(Wpt1, Wpt2) - the shortest path between Wpt1 to Wpt2
//! including both Wpt1 and Wpt2.
//! If aLeaveRouteNetwork is true, the mover will go to the specified aEndLat/aEndLon after reaching
//! the closest point on the route network.
bool WsfRouteNetwork::GeneratePathBetweenClosestSegments(double    aStartLat,
                                                         double    aStartLon,
                                                         double    aEndLat,
                                                         double    aEndLon,
                                                         WsfRoute& aPath,
                                                         bool      aLeaveRouteNetwork /*=true*/)
{
   bool haveRoute = false;
   if (!mInitialized)
   {
      Initialize();
   }

   InitializeShortestPath();
   RoutePoint startPoint;
   RoutePoint endPoint;
   double     startDistanceToSegment;
   double     endDistanceToSegment;
   bool       startProjected = FindClosestSegment(aStartLat, aStartLon, startPoint, startDistanceToSegment);
   bool       endProjected   = FindClosestSegment(aEndLat, aEndLon, endPoint, endDistanceToSegment);
   if (startProjected && endProjected)
   {
      aPath.Clear();
      if (startDistanceToSegment > 1.0 && aLeaveRouteNetwork)
      {
         WsfWaypoint beginPt;
         beginPt.SetLat(aStartLat);
         beginPt.SetLon(aStartLon);
         aPath.Append(beginPt);
      }
      haveRoute = AppendShortestPathOnNetwork(startPoint, endPoint, aPath);
      if (endDistanceToSegment > 1.0 && aLeaveRouteNetwork)
      {
         WsfWaypoint endPt;
         endPt.SetLat(aEndLat);
         endPt.SetLon(aEndLon);
         aPath.Append(endPt);
      }
   }
   return haveRoute;
}

// =================================================================================================
bool WsfRouteNetwork::FindClosestSegment(double aLat, double aLon, RoutePoint& aProjectedPoint, double& aDistance)
{
   if (!mInitialized)
   {
      Initialize();
   }
   double xyz[3];
   UtEllipsoidalEarth::ConvertLLAToLocal(mRefECEF, mTransECEF, aLat, aLon, 0, xyz);
   const double        cLARGE_DISTANCE   = 1.0E30;
   double              dist2             = cLARGE_DISTANCE;
   const SegmentDatum* nearestSegmentPtr = nullptr;
   using RangeNode                       = std::pair<double, const UtQuadNode*>;
   std::priority_queue<RangeNode, std::vector<RangeNode>, std::greater<RangeNode>> searchQueue;
   searchQueue.push(RangeNode(0, mTreePtr->GetRoot()));
   while (!searchQueue.empty())
   {
      RangeNode         rangeNode = searchQueue.top();
      const UtQuadNode* nodePtr   = rangeNode.second;
      searchQueue.pop();
      if (dist2 < rangeNode.first)
      {
         break;
      }
      if (!nodePtr->IsLeaf())
      {
         for (int i = 0; i < 4; ++i)
         {
            const UtQuadNode*        childPtr  = nodePtr->GetChild(i);
            const UtBoundingBoxI<2>& bound     = childPtr->GetBoundingBox();
            double                   distance2 = bound.DistanceSquareFrom(xyz);
            if (distance2 < dist2)
            {
               searchQueue.push(RangeNode(distance2, childPtr));
            }
         }
      }
      else
      {
         const SpatialData& data = nodePtr->GetData();
         for (auto sd : data)
         {
            const SegmentDatum* datumPtr = (const SegmentDatum*)sd;
            double              range    = datumPtr->mSegment.DistanceBetween(xyz, true);
            if (range * range < dist2)
            {
               dist2                                = range * range;
               aProjectedPoint.mWaypointAddr.first  = datumPtr->mRouteIndex;
               aProjectedPoint.mWaypointAddr.second = datumPtr->mWaypointIndex;
               nearestSegmentPtr                    = datumPtr;
               aDistance                            = range;
            }
         }
      }
   }
   if (nearestSegmentPtr != nullptr)
   {
      double CPA[3];
      double mod;
      nearestSegmentPtr->mSegment.PointOfClosestApproach(xyz, CPA, mod, true);
      WsfRoute*    routePtr                 = mRoutes[aProjectedPoint.mWaypointAddr.first];
      WsfWaypoint& wpt1                     = routePtr->GetWaypointAt(aProjectedPoint.mWaypointAddr.second);
      WsfWaypoint& wpt2                     = routePtr->GetWaypointAt(aProjectedPoint.mWaypointAddr.second + 1);
      aProjectedPoint.mDistanceAlongSegment = mod * (wpt2.GetDistanceAlongRoute() - wpt1.GetDistanceAlongRoute());
   }
   return dist2 != cLARGE_DISTANCE;
}

// =================================================================================================
//! Returns the closest waypoint relative to the provided latitude and longitude.
bool WsfRouteNetwork::FindClosestWaypoint(double aLat, double aLon, WsfWaypoint& aWpt, double& aDistance)
{
   if (!mInitialized)
   {
      Initialize();
   }
   double xyz[3];
   UtEllipsoidalEarth::ConvertLLAToLocal(mRefECEF, mTransECEF, aLat, aLon, 0, xyz);
   const double cLARGE_DISTANCE = 1.0E30;
   double       dist2           = cLARGE_DISTANCE;
   aDistance                    = dist2;
   using RangeNode              = std::pair<double, const UtQuadNode*>;
   std::priority_queue<RangeNode, std::vector<RangeNode>, std::greater<RangeNode>> searchQueue;
   searchQueue.push(RangeNode(0, mTreePtr->GetRoot()));
   while (!searchQueue.empty())
   {
      RangeNode         rangeNode = searchQueue.top();
      const UtQuadNode* nodePtr   = rangeNode.second;
      searchQueue.pop();
      if (dist2 < rangeNode.first)
      {
         break;
      }
      if (!nodePtr->IsLeaf())
      {
         for (int i = 0; i < 4; ++i)
         {
            const UtQuadNode*        childPtr  = nodePtr->GetChild(i);
            const UtBoundingBoxI<2>& bound     = childPtr->GetBoundingBox();
            double                   distance2 = bound.DistanceSquareFrom(xyz);
            // distance2 = 0;
            if (distance2 < dist2)
            {
               searchQueue.push(RangeNode(distance2, childPtr));
            }
         }
      }
      else
      {
         const SpatialData& data = nodePtr->GetData();
         for (auto sd : data)
         {
            const SegmentDatum* datumPtr = (const SegmentDatum*)sd;
            double              diff1[2], diff2[2];
            UtVec2d::Subtract(diff1, xyz, datumPtr->mSegment.GetBeginPtr());
            UtVec2d::Subtract(diff2, xyz, datumPtr->mSegment.GetEndPtr());
            double R1 = UtVec2d::MagnitudeSquared(diff1);
            double R2 = UtVec2d::MagnitudeSquared(diff2);
            if (R1 <= R2 && R1 < dist2)
            {
               dist2 = R1;
               aWpt  = (*mRoutes[datumPtr->mRouteIndex])[datumPtr->mWaypointIndex];
            }
            else if (R2 < dist2)
            {
               dist2 = R2;
               aWpt  = (*mRoutes[datumPtr->mRouteIndex])[datumPtr->mWaypointIndex + 1];
            }
         }
      }
   }
   aDistance = sqrt(dist2);
   return dist2 != cLARGE_DISTANCE;
}

// =================================================================================================
WsfRoute& WsfRouteNetwork::operator[](unsigned int aIndex)
{
   assert(aIndex < mRoutes.size());
   return *mRoutes[aIndex];
}

// =================================================================================================
std::vector<const WsfRoute*> WsfRouteNetwork::GetRoutes() const
{
   std::vector<const WsfRoute*> routes;
   std::transform(mRoutes.begin(),
                  mRoutes.end(),
                  std::back_inserter(routes),
                  [](const decltype(mRoutes)::value_type& route) { return route.get(); });
   return routes;
}

// =================================================================================================
void WsfRouteNetwork::Append(const RoutePoint& aPoint, WsfRoute& aRoute)
{
   WsfRoute* routePtr = mRoutes[aPoint.mWaypointAddr.first];
   if (aPoint.mDistanceAlongSegment == 0)
   {
      // Protect against adding coincident waypoints
      WsfWaypoint& wpt = routePtr->GetWaypointAt(aPoint.mWaypointAddr.second);
      if (aRoute.Empty() || wpt != aRoute.GetWaypoints().back())
      {
         aRoute.Append(wpt);
      }
   }
   else
   {
      WsfWaypoint& wpt1   = routePtr->GetWaypointAt(aPoint.mWaypointAddr.second);
      WsfWaypoint& wpt2   = routePtr->GetWaypointAt(aPoint.mWaypointAddr.second + 1);
      double       d1     = wpt1.GetDistanceAlongRoute();
      double       d2     = wpt2.GetDistanceAlongRoute();
      double       D      = std::min((d2 - d1), aPoint.mDistanceAlongSegment);
      double       ratio  = D / (d2 - d1);
      WsfWaypoint  newWpt = wpt1;
      newWpt.SetLat(wpt1.GetLat() * (1.0 - ratio) + wpt2.GetLat() * ratio);
      newWpt.SetLon(wpt1.GetLon() * (1.0 - ratio) + wpt2.GetLon() * ratio);
      if (aRoute.Empty())
      {
         aRoute.Append(newWpt);
      }
      else
      {
         // Don't insert a coincident waypoint
         WsfWaypoint prevWpt = aRoute.GetWaypoints().back();
         double      heading, distance;
         UtSphericalEarth::GreatCircleHeadingAndDistance(prevWpt.GetLat(),
                                                         prevWpt.GetLon(),
                                                         newWpt.GetLat(),
                                                         newWpt.GetLon(),
                                                         heading,
                                                         distance);
         if (distance > 0.1)
         {
            aRoute.Append(newWpt);
         }
      }
   }
}

// =================================================================================================
//! Append a sub-route between aPoint1 and aPoint2, which share the same route.
void WsfRouteNetwork::AppendSubroute(const RoutePoint& aPoint1, const RoutePoint& aPoint2, WsfRoute& aRoute)
{
   assert(aPoint1.RouteIndex() == aPoint2.RouteIndex());
   bool   startIsSegment = aPoint1.mDistanceAlongSegment > 0;
   bool   endIsSegment   = aPoint2.mDistanceAlongSegment > 0;
   int    i              = aPoint1.WaypointIndex();
   int    e              = aPoint2.WaypointIndex();
   double delta          = (e + (endIsSegment ? 0.5 : 0)) - (i + (startIsSegment ? 0.5 : 0));

   Append(aPoint1, aRoute);
   // Extra care here to ensure we do not backtrack when traveling to/from the middle of segments.
   if (delta >= 0)
   {
      ++i;
      if (!endIsSegment)
      {
         --e;
      }
      for (; i <= e; ++i)
      {
         aRoute.Append(mRoutes[aPoint1.RouteIndex()]->GetWaypointAt(i));
      }
   }
   else
   {
      if (!startIsSegment)
      {
         --i;
      }
      ++e;
      for (; i >= e; --i)
      {
         aRoute.Append(mRoutes[aPoint1.RouteIndex()]->GetWaypointAt(i));
      }
   }
   Append(aPoint2, aRoute);
}

// =================================================================================================
// Same as WsfRoute::AppendSubroute(), but will not append a coincident waypoint.
void WsfRouteNetwork::AppendSubroute(const WsfRoute& aSourceRoute, int aStartIndex, int aEndIndex, WsfRoute& aRoute)
{
   const std::vector<WsfWaypoint>& waypoints = aSourceRoute.GetWaypoints();
   if (aStartIndex < aEndIndex)
   {
      aStartIndex = std::max(aStartIndex, 0);
      aEndIndex   = std::min(aEndIndex, int(waypoints.size()) - 1);
      for (int ptIndex = aStartIndex; ptIndex <= aEndIndex; ++ptIndex)
      {
         if (aRoute.Empty() || (aRoute.GetWaypoints().back() != waypoints[ptIndex]))
         {
            aRoute.Append(waypoints[ptIndex]);
         }
      }
   }
   else
   {
      aEndIndex   = std::max(aEndIndex, 0);
      aStartIndex = std::min(aStartIndex, int(waypoints.size()) - 1);
      for (int ptIndex = aStartIndex; ptIndex >= aEndIndex; --ptIndex)
      {
         if (aRoute.Empty() || (aRoute.GetWaypoints().back() != waypoints[ptIndex]))
         {
            aRoute.Append(waypoints[ptIndex]);
         }
      }
   }
}

// =================================================================================================
// Fills in the path between waypoints
// private
void WsfRouteNetwork::AppendSubroute(const WsfRouteNetwork::RouteNode& aStartingWaypoint,
                                     const WsfRouteNetwork::RouteNode& aEndingWaypoint,
                                     WsfRoute&                         aPath)
{
   if (aStartingWaypoint.mWaypoints && aEndingWaypoint.mWaypoints)
   {
      // search for waypoints which share the same route:
      // There may be more than one match
      const double cLARGE_DISTANCE = 1.0E30;
      double       minimumDistance = cLARGE_DISTANCE;
      WsfRoute*    routePtr        = nullptr;
      int          i1              = 0;
      int          i2              = 0;
      auto         a               = aStartingWaypoint.mWaypoints->begin();
      auto         b               = aEndingWaypoint.mWaypoints->begin();
      while ((a != aStartingWaypoint.mWaypoints->end()) && (b != aEndingWaypoint.mWaypoints->end()))
      {
         if (a->first < b->first)
         {
            ++a;
         }
         else if (b->first < a->first)
         {
            ++b;
         }
         else
         {
            // Route IDs match
            routePtr                        = mRoutes[a->first];
            double distanceBetweenWaypoints = routePtr->GetDistance(a->second, b->second);
            if (distanceBetweenWaypoints < minimumDistance)
            {
               minimumDistance = distanceBetweenWaypoints;
               i1              = a->second;
               i2              = b->second;
            }
            ++a;
            ++b;
         }
      }
      if (routePtr != nullptr)
      {
         if (aPath.Empty())
         {
            AppendSubroute(*routePtr, i1, i2, aPath);
         }
         else
         {
            double delta =
               (i2 + (aEndingWaypoint.mSegmentNode ? 0.5 : 0)) - (i1 + (aStartingWaypoint.mSegmentNode ? 0.5 : 0));
            if (delta > 0.0)
            {
               i1 += 1;
               if (i1 <= i2)
               {
                  AppendSubroute(*routePtr, i1, i2, aPath);
               }
            }
            else if (delta < 0.0)
            {
               if (!aStartingWaypoint.mSegmentNode)
               {
                  --i1;
               }
               if (aEndingWaypoint.mSegmentNode)
               {
                  ++i2;
               }
               if (i1 >= i2)
               {
                  AppendSubroute(*routePtr, i1, i2, aPath);
               }
            }
         }
      }
      else
      {
         auto out = ut::log::error() << "Could not find the associated route for source and destination nodes.";
         out.AddNote() << "Road Network: " << GetName();
         out.AddNote() << "Source Node: " << aStartingWaypoint.mId;
         out.AddNote() << "Destination Node: " << aEndingWaypoint.mId;
      }
   }
}

// =================================================================================================
int WsfRouteNetwork::GetAdjacentNodeIndices(const RoutePoint& aPoint, int* aIndices)
{
   int       indexCount = 0;
   WsfRoute* routePtr   = mRoutes[aPoint.RouteIndex()];
   int       nextIndex  = aPoint.WaypointIndex() + 1;
   if (routePtr->GetWaypointAt(ut::cast_to_size_t(aPoint.WaypointIndex())).GetNodeId() != 0)
   {
      // Segment starts at node.
      aIndices[indexCount++] = aPoint.WaypointIndex();
      if (aPoint.mDistanceAlongSegment == 0.0)
      {
         // point is directly on the node, don't search anymore
         nextIndex = ut::cast_to_int(routePtr->GetSize());
      }
   }
   else
   {
      // Search left of waypoint for node.
      for (int i = aPoint.WaypointIndex() - 1; i >= 0; --i)
      {
         if (!routePtr->GetWaypointAt(ut::cast_to_size_t(i)).GetNodeId().IsNull())
         {
            aIndices[indexCount++] = i;
            break;
         }
      }
   }
   // Search right of waypoint for node.
   for (int i = nextIndex; i < ut::cast_to_int(routePtr->GetSize()); ++i)
   {
      if (!routePtr->GetWaypointAt(static_cast<size_t>(i)).GetNodeId().IsNull())
      {
         aIndices[indexCount++] = i;
         break;
      }
   }
   return indexCount;
}

// =================================================================================================
void WsfRouteNetwork::BuildQuadtree()
{
   double minXYZ[3] = {0, 0, 0};
   double maxXYZ[3] = {0, 0, 0};

   // build a Cartesian coordinate system for calculations since we are using a NED cs and a quad-tree based on the
   // north and east coordinate, this class will be limited in area it is able to represent.  hopefully we won't be
   // modeling urban environments spanning over 90 degrees of the earth!

   UtEllipsoidalEarth::ComputeNEDTransform(mCenterLLA[0], mCenterLLA[1], 0, mTransECEF, mRefECEF);

   UtEllipsoidalEarth::ConvertLLAToLocal(mRefECEF, mTransECEF, mMinLat, mMinLon, 0, minXYZ);
   UtEllipsoidalEarth::ConvertLLAToLocal(mRefECEF, mTransECEF, mMaxLat, mMaxLon, 0, maxXYZ);
   if (minXYZ[0] == maxXYZ[0])
   {
      minXYZ[0] -= 100.0;
      maxXYZ[0] += 100.0;
   }
   if (minXYZ[1] == maxXYZ[1])
   {
      minXYZ[1] -= 100.0;
      maxXYZ[1] += 100.0;
   }
   // Important: We are doing an ellipsoidal conversion, and the min/max bounds in XYZ cannot be determined
   // by the box corners.  Take samples along the edges of the box keeping track of the bounds in local coordinates.
   for (int x = 0; x < 100; ++x)
   {
      double t1 = x / 99.0;
      double t2 = 1.0 - t1;
      for (int i = 0; i < 2; ++i)
      {
         for (int j = 0; j < 2; ++j)
         {
            double lat = mMinLat * t1 + mMaxLat * t2;
            double lon = mMinLon * t1 + mMaxLon * t2;
            if (i == 0 && j == 0)
            {
               lon = mMinLon;
            }
            if (i == 0 && j == 1)
            {
               lon = mMaxLon;
            }
            if (i == 1 && j == 0)
            {
               lat = mMinLat;
            }
            if (i == 1 && j == 1)
            {
               lat = mMaxLat;
            }
            double pointXYZ[3];
            UtEllipsoidalEarth::ConvertLLAToLocal(mRefECEF, mTransECEF, lat, lon, 0, pointXYZ);
            minXYZ[0] = std::min(minXYZ[0], pointXYZ[0]);
            minXYZ[1] = std::min(minXYZ[1], pointXYZ[1]);
            maxXYZ[0] = std::max(maxXYZ[0], pointXYZ[0]);
            maxXYZ[1] = std::max(maxXYZ[1], pointXYZ[1]);
         }
      }
   }
   // Add another 4% to make sure the box contains all points
   double rangeX = maxXYZ[0] - minXYZ[0];
   double rangeY = maxXYZ[1] - minXYZ[1];
   minXYZ[0] -= .02 * rangeX;
   minXYZ[1] -= .02 * rangeY;
   maxXYZ[0] += .02 * rangeX;
   maxXYZ[1] += .02 * rangeY;

   UtBoundingBoxI<2> boundingBox(minXYZ, maxXYZ);
   delete mTreePtr; // Shouldn't happen, but just in case we get called more than once
   mTreePtr = new UtQuadTree(boundingBox, mTreeDepth);

   for (const auto& route : mRoutes)
   {
      // loop through each of the routes and insert each segment into the segment datum vector
      for (int wptIndex = 0; wptIndex < (static_cast<int>(route->GetSize()) - 1); ++wptIndex)
      {
         UtLineSegment lineSegment;
         double        wptXYZ[3];
         double        wayPtLLA[3];
         (*route)[wptIndex].GetLocationLLA(wayPtLLA[0], wayPtLLA[1], wayPtLLA[2]);
         UtEllipsoidalEarth::ConvertLLAToLocal(mRefECEF, mTransECEF, wayPtLLA[0], wayPtLLA[1], 0, wptXYZ);
         lineSegment.SetBeginPoint(wptXYZ);
         (*route)[wptIndex + 1].GetLocationLLA(wayPtLLA[0], wayPtLLA[1], wayPtLLA[2]);
         UtEllipsoidalEarth::ConvertLLAToLocal(mRefECEF, mTransECEF, wayPtLLA[0], wayPtLLA[1], 0, wptXYZ);
         lineSegment.SetEndPoint(wptXYZ);

         SegmentDatum* datumPtr   = new SegmentDatum(lineSegment);
         datumPtr->mRouteIndex    = route->GetNetworkRouteId();
         datumPtr->mWaypointIndex = wptIndex;
         if (!mTreePtr->InsertArea(datumPtr))
         {
            // This should not fail, but if it does it will cause many problems.
            ut::log::error() << "Failure in WsfRouteNetwork: InsertArea().";
         }
      }
   }
}

// =================================================================================================
// private
bool WsfRouteNetwork::FindShortestPath(WsfStringId aSrcNodeId, WsfStringId aDstNodeId, Path& aPath, double& aCost) const
{
   bool success = false;

   if (aSrcNodeId != aDstNodeId)
   {
      auto srcNodeIter = mGraph.find(RouteNode(aSrcNodeId));
      auto dstNodeIter = mGraph.find(RouteNode(aDstNodeId));
      if ((srcNodeIter != mGraph.end()) && (dstNodeIter != mGraph.end()))
      {
         success = mGraph.shortest_path(srcNodeIter, dstNodeIter, aPath, aCost, &mCostFunc);
      }
   }
   return success;
}

// =================================================================================================
// private
bool WsfRouteNetwork::BuildRoute(const Path& aShortestPath, int aStartIndex, int aStopIndex, WsfRoute& aPath)
{
   for (int i = aStartIndex; i < (aStopIndex - 1); ++i)
   {
      const RouteNode& s = aShortestPath[i];
      const RouteNode& d = aShortestPath[i + 1];
      AppendSubroute(s, d, aPath);
   }
   return true;
}

// =================================================================================================
// Initializes the shortest path data structures if not already done.
// private
void WsfRouteNetwork::InitializeShortestPath()
{
   if (mShortestPathInitialized)
   {
      return;
   }
   mShortestPathInitialized = true;

   unsigned int routeCount = static_cast<unsigned int>(mRoutes.size());
   for (unsigned int routeIndex = 0; routeIndex < routeCount; ++routeIndex)
   {
      int       prevIndex    = 0;
      auto      prevNodeIter = mGraph.end();
      WsfRoute& route        = *mRoutes[routeIndex];
      for (unsigned int ptIndex = 0; ptIndex < route.GetSize(); ++ptIndex)
      {
         WsfStringId nodeId = route[ptIndex].GetNodeId();
         if (nodeId > nullptr)
         {
            // Create a node in the graph for each node in the route.

            auto nodeIter = mGraph.find(RouteNode(nodeId));
            if (nodeIter == mGraph.end())
            {
               nodeIter = mGraph.insert(RouteNode(nodeId, new WaypointAddrList));
            }
            WaypointAddr wptAddr(routeIndex, ptIndex);
            nodeIter->first.mWaypoints->insert(std::lower_bound(nodeIter->first.mWaypoints->begin(),
                                                                nodeIter->first.mWaypoints->end(),
                                                                wptAddr),
                                               wptAddr);

            // Create an edge that runs between each successive node in a route.

            if (prevNodeIter != mGraph.end())
            {
               RouteEdge    edge(prevNodeIter->first, nodeIter->first);
               WsfWaypoint& wpt     = route[ptIndex];
               WsfWaypoint& prevWpt = route[prevIndex];
               edge.mDistance       = int(wpt.GetDistanceAlongRoute() - prevWpt.GetDistanceAlongRoute());
               prevNodeIter->second->insert_edge(nodeIter, edge);
            }
            prevNodeIter = nodeIter;
            prevIndex    = ptIndex;
         }
      }
   }
}

// =================================================================================================
// private
void WsfRouteNetwork::TestNetwork()
{
   InitializeShortestPath();

   { // RAII block
      auto out = ut::log::info() << "Begin testing route network.";
      out.AddNote() << "Network: " << GetName();
      out.AddNote() << "Nodes: " << mGraph.count_nodes();
   }

   std::vector<RouteNode> bestPath;
   double                 bestCost;
   UtWallClock            totalTimer;
   for (RouteGraph::const_node_iterator iter1 = mGraph.begin(); iter1 != mGraph.end(); ++iter1)
   {
      UtWallClock timer;
      for (RouteGraph::const_node_iterator iter2 = mGraph.begin(); iter2 != mGraph.end(); ++iter2)
      {
         if (iter1 == iter2)
         {
            continue;
         }

         bestPath.clear();
         if (!mGraph.shortest_path(iter1, iter2, bestPath, bestCost, &mCostFunc))
         {
            auto out = ut::log::warning() << "Could not solve shortest path.";
            out.AddNote() << "From: " << iter1->first.mId;
            out.AddNote() << "To: " << iter2->first.mId;
         }

         if (mVerbose)
         {
            auto out = ut::log::info() << "";
            out.AddNote() << "From: " << iter1->first.mId;
            out.AddNote() << "To: " << iter2->first.mId;
            out.AddNote() << "Cost: " << bestCost;
            auto pathNote = out.AddNote() << "Path:";
            if (bestPath.empty())
            {
               pathNote.AddNote() << "No path could be found.";
            }
            for (auto& bp : bestPath)
            {
               pathNote.AddNote() << bp.mId;
            }
         }
      }
   }

   auto out = ut::log::info() << "Done testing route network.";
   out.AddNote() << "Total time: " << totalTimer.GetClock();
}

// =================================================================================================
// Nested class WsfRouteNetwork::SegmentDatum
// =================================================================================================
WsfRouteNetwork::SegmentDatum::SegmentDatum(const UtLineSegment& aSegment)
   : mRouteIndex(0)
   , mWaypointIndex(0)
   , mSegment(aSegment)
   , mBoundingBox()
{
   double A[3], B[3];
   aSegment.GetBeginPoint(A);
   aSegment.GetEndPoint(B);
   double least[3]    = {std::min(A[0], B[0]), std::min(A[1], B[1]), 0};
   double greatest[3] = {std::max(A[0], B[0]), std::max(A[1], B[1]), 0};

   mBoundingBox.SetLeast(least);
   mBoundingBox.SetGreatest(greatest);
}

// =================================================================================================
//! destructor
// public
WsfRouteNetwork::SegmentDatum::~SegmentDatum() {}

// =================================================================================================
//! Indicates whether the datum is entirely in aBox
//! @param aBox The bounding box checked against.
//! @return true is this is in aBox completely, false otherwise
// public virtual
bool WsfRouteNetwork::SegmentDatum::IsWhollyContainedIn(const UtBoundingBox& aBox) const
{
   return aBox.Contains(mBoundingBox);
}

// =================================================================================================
//! Indicates whether the datum overlaps aBox
//! @param aBox The bounding box to check against.
//! @return true indicates an intersection between the areas, false otherwise.
// public virtual
bool WsfRouteNetwork::SegmentDatum::IsPartiallyContainedIn(const UtBoundingBox& aBox) const
{
   return mBoundingBox.Intersects(aBox);
}

// =================================================================================================
// The script interface 'class'
// =================================================================================================
class WSF_EXPORT WsfScriptRouteNetworkClass : public WsfScriptObjectClass
{
public:
   WsfScriptRouteNetworkClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(Routes);
   UT_DECLARE_SCRIPT_METHOD(FindRouteNetwork);
   UT_DECLARE_SCRIPT_METHOD(GeneratePathOffRouteToRoute_1);
   UT_DECLARE_SCRIPT_METHOD(GeneratePathOffRouteToRoute_2);
   UT_DECLARE_SCRIPT_METHOD(GeneratePathOnRoute);
   UT_DECLARE_SCRIPT_METHOD(SnapToNearestSegment);
   UT_DECLARE_SCRIPT_METHOD(SnapToNearestWaypoint);
};

// =================================================================================================
//! Create the 'class' object for the script system.
//! This is invoked once by WsfScriptManager to create the 'class' object that defines
//! the interface to instances of this class from the script system.
// static
std::unique_ptr<UtScriptClass> WsfRouteNetwork::CreateScriptClass(const std::string& aClassName,
                                                                  UtScriptTypes*     aScriptTypesPtr)
{
   return ut::make_unique<WsfScriptRouteNetworkClass>(aClassName, aScriptTypesPtr);
}

// =================================================================================================
WsfScriptRouteNetworkClass::WsfScriptRouteNetworkClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : WsfScriptObjectClass(aClassName, aTypesPtr)
{
   SetClassName("WsfRouteNetwork");

   // Add each of the method objects to the class.

   AddStaticMethod(ut::make_unique<FindRouteNetwork>());
   AddMethod(ut::make_unique<Routes>());
   AddMethod(ut::make_unique<GeneratePathOffRouteToRoute_1>("GeneratePathOffRouteToRoute"));
   AddMethod(ut::make_unique<GeneratePathOffRouteToRoute_2>("GeneratePathOffRouteToRoute"));
   AddMethod(ut::make_unique<GeneratePathOnRoute>());
   AddMethod(ut::make_unique<SnapToNearestSegment>());
   AddMethod(ut::make_unique<SnapToNearestWaypoint>());
}

// =================================================================================================
//! WsfRouteNetwork network = FindRouteNetwork(string aNetworkName);
UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteNetworkClass, WsfRouteNetwork, FindRouteNetwork, 1, "WsfRouteNetwork", "string")
{
   // Get a pointer to our current road network.
   std::string      networkName = aVarArgs[0].GetString();
   WsfRouteNetwork* routeNetworkPtr =
      WsfRouteNetworkTypes::Get(*WsfScriptContext::GetSCENARIO(aContext)).Find(networkName);
   aReturnVal.SetPointer(new UtScriptRef(routeNetworkPtr, aReturnClassPtr));
}

// =================================================================================================
//! Array<WsfRoute> routes = network.Routes();
UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteNetworkClass, WsfRouteNetwork, Routes, 0, "Array<WsfRoute>", "")
{
   auto              routes       = aObjectPtr->GetRoutes();
   UtScriptDataList* routeListPtr = new UtScriptDataList;
   for (auto& route : routes)
   {
      routeListPtr->push_back(UtScriptData(route->ConstScriptRef()));
   }
   aReturnVal.SetPointer(new UtScriptRef(routeListPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteNetworkClass,
                        WsfRouteNetwork,
                        GeneratePathOffRouteToRoute_1,
                        4,
                        "WsfRoute",
                        "double, double, double, double")
{
   double begLat = aVarArgs[0].GetDouble();
   double begLon = aVarArgs[1].GetDouble();
   double endLat = aVarArgs[2].GetDouble();
   double endLon = aVarArgs[3].GetDouble();

   WsfRoute* pathPtr = new WsfRoute;
   if (!aObjectPtr->GeneratePathBetweenClosestSegments(begLat, begLon, endLat, endLon, *pathPtr, true))
   {
      delete pathPtr;
      pathPtr = nullptr;
   }
   aReturnVal.SetPointer(new UtScriptRef(pathPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteNetworkClass, WsfRouteNetwork, GeneratePathOffRouteToRoute_2, 2, "WsfRoute", "WsfGeoPoint, WsfGeoPoint")
{
   WsfGeoPoint* begPointPtr = static_cast<WsfGeoPoint*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfGeoPoint* endPointPtr = static_cast<WsfGeoPoint*>(aVarArgs[1].GetPointer()->GetAppObject());

   WsfRoute* pathPtr = new WsfRoute;
   if (!aObjectPtr->GeneratePathBetweenClosestSegments(begPointPtr->GetLat(),
                                                       begPointPtr->GetLon(),
                                                       endPointPtr->GetLat(),
                                                       endPointPtr->GetLon(),
                                                       *pathPtr,
                                                       true))
   {
      delete pathPtr;
      pathPtr = nullptr;
   }
   aReturnVal.SetPointer(new UtScriptRef(pathPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteNetworkClass, WsfRouteNetwork, GeneratePathOnRoute, 2, "WsfRoute", "WsfGeoPoint, WsfGeoPoint")
{
   WsfGeoPoint* begPointPtr = static_cast<WsfGeoPoint*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfGeoPoint* endPointPtr = static_cast<WsfGeoPoint*>(aVarArgs[1].GetPointer()->GetAppObject());

   WsfRoute* pathPtr = new WsfRoute;
   if (!aObjectPtr->GeneratePathBetweenClosestSegments(begPointPtr->GetLat(),
                                                       begPointPtr->GetLon(),
                                                       endPointPtr->GetLat(),
                                                       endPointPtr->GetLon(),
                                                       *pathPtr,
                                                       false))
   {
      delete pathPtr;
      pathPtr = nullptr;
   }
   aReturnVal.SetPointer(new UtScriptRef(pathPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteNetworkClass, WsfRouteNetwork, SnapToNearestSegment, 1, "WsfGeoPoint", "WsfGeoPoint")
{
   WsfGeoPoint*                inputPointPtr = static_cast<WsfGeoPoint*>(aVarArgs[0].GetPointer()->GetAppObject());
   double                      distanceToSegment;
   WsfRouteNetwork::RoutePoint point;
   if (aObjectPtr->FindClosestSegment(inputPointPtr->GetLat(), inputPointPtr->GetLon(), point, distanceToSegment))
   {
      WsfRoute&     route       = (*aObjectPtr)[point.RouteIndex()];
      WsfWaypoint&  a           = route[point.WaypointIndex()];
      WsfWaypoint&  b           = route[point.WaypointIndex() + 1];
      double        start[3]    = {a.GetLat(), a.GetLon(), 0};
      double        end[3]      = {b.GetLat(), b.GetLon(), 0};
      double        pointLLA[3] = {inputPointPtr->GetLat(), inputPointPtr->GetLon(), 0};
      UtLineSegment segment(start, end);
      double        closestPoint[3];
      double        mod;
      segment.PointOfClosestApproach(pointLLA, closestPoint, mod, true);
      WsfGeoPoint* outputPointPtr = new WsfGeoPoint(closestPoint[0], closestPoint[1], 0.0);
      aReturnVal.SetPointer(new UtScriptRef(outputPointPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(new UtScriptRef(nullptr, aReturnClassPtr));
   }
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteNetworkClass, WsfRouteNetwork, SnapToNearestWaypoint, 1, "WsfGeoPoint", "WsfGeoPoint")
{
   WsfGeoPoint* inputPointPtr = static_cast<WsfGeoPoint*>(aVarArgs[0].GetPointer()->GetAppObject());
   double       distanceToSegment;
   WsfWaypoint  wpt;
   if (aObjectPtr->FindClosestWaypoint(inputPointPtr->GetLat(), inputPointPtr->GetLon(), wpt, distanceToSegment))
   {
      WsfGeoPoint* outputPointPtr = new WsfGeoPoint(wpt.GetLat(), wpt.GetLon(), 0.0);
      aReturnVal.SetPointer(new UtScriptRef(outputPointPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(new UtScriptRef(nullptr, aReturnClassPtr));
   }
}
