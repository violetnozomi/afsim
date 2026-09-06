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

#include "WsfNavigationMesh.hpp"

#include <ctime>
#include <fstream>
#include <memory>

#include "UtEllipsoidalEarth.hpp"
#include "UtInput.hpp"
#include "UtInputFile.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "UtRandom.hpp"
#include "WsfRoute.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfTerrain.hpp"
#include "WsfZoneSet.hpp"
#include "script/WsfScriptManager.hpp"

#define MAX_TESSILLATION_LEVEL 5

WsfNavigationMesh::WsfNavigationMesh(WsfScenario* aScenarioPtr)
   : WsfPathFinder(aScenarioPtr)
   , mMeshCells()
   , mMeshCellsIterator()
   , mDebugMeshDrawPtr(nullptr)
   , mDebugRouteDrawPtr(nullptr)
   , mMeshNodes()
   , mAdjustedPath()
   , mGetCellForPointCache()
   , mMaxLat(-std::numeric_limits<double>::max())
   , mMinLat(std::numeric_limits<double>::max())
   , mMaxLon(-std::numeric_limits<double>::max())
   , mMinLon(std::numeric_limits<double>::max())
   , mRefineUpdateTime(0.0)
   , mNavMeshThinkTime(0.0)
   , mInputFile()
   , mOutputFile()
   , mLoadBinaryFile(false)
   , mCellMap()
{
   for (size_t i = 0; i < 3; ++i)
   {
      mCenterLLA[i] = 0.0;
   }
}

WsfNavigationMesh::~WsfNavigationMesh()
{
   delete mDebugMeshDrawPtr;
   delete mDebugRouteDrawPtr;
   mMeshCells.clear();
}

bool WsfNavigationMesh::Initialize(WsfSimulation* aSimulationPtr)
{
   mDebugMeshDrawPtr  = new WsfDraw(*aSimulationPtr);
   mDebugRouteDrawPtr = new WsfDraw(*aSimulationPtr);
   mSimulationPtr     = aSimulationPtr;
   mInitialized       = false;
   mRefineUpdateTime  = 0.0;
   // Make sure we have the inputs we need to build the underlying grid
   assert(mGridSizeDegrees != 0);
   mXSize = (long)ceil((mLowerRight.GetLon() - mUpperLeft.GetLon()) / mGridSizeDegrees);
   mYSize = (long)ceil((mUpperLeft.GetLat() - mLowerRight.GetLat()) / mGridSizeDegrees);

   if (mInputFile.empty())
   {
      CreateNavigationMesh();
      if (!mOutputFile.empty())
      {
         GenerateExportedNavMesh();
      }
   }
   else
   {
      ImportNavMesh();
      if (!mOutputFile.empty())
      {
         GenerateExportedNavMesh();
      }
   }
   mInitialized = true;
   return true;
}

void WsfNavigationMesh::CreateNavigationMesh()
{
   unsigned int x = 0U;
   WsfGeoPoint  tempPoint;
   unsigned int cellNumber = 0;

   // Static cast is okay here, mXSize and mYSize are logically enforced to be 0 or greater.
   // These member variables should be updated in future maintenance efforts.
   for (x = 0U; x < static_cast<unsigned int>(mXSize); ++x)
   {
      unsigned int y = 0U;
      for (unsigned int j = 0U; j < static_cast<unsigned int>(mYSize); ++j)
      {
         double             pointA[3]  = {mUpperLeft.GetLat() - (static_cast<double>(j) + 0.5) * mGridSizeDegrees,
                             mUpperLeft.GetLon() + (static_cast<double>(x) + 0.5) * mGridSizeDegrees,
                             0.0};
         double             pointB[3]  = {mUpperLeft.GetLat() - (static_cast<double>(j) + 1.5) * mGridSizeDegrees,
                             mUpperLeft.GetLon() + (static_cast<double>(x) + 0.5) * mGridSizeDegrees,
                             0.0};
         double             pointC[3]  = {mUpperLeft.GetLat() - (static_cast<double>(j) + 0.5) * mGridSizeDegrees,
                             mUpperLeft.GetLon() + (static_cast<double>(x) + 1.5) * mGridSizeDegrees,
                             0.0};
         WsfNavigationCell* navCellPtr = new WsfNavigationCell();
         navCellPtr->AddVertex(WsfGeoPoint(pointA[0], pointA[1], pointA[2]));
         navCellPtr->AddVertex(WsfGeoPoint(pointB[0], pointB[1], pointB[2]));
         navCellPtr->AddVertex(WsfGeoPoint(pointC[0], pointC[1], pointC[2]));
         navCellPtr->mCellID           = cellNumber;
         navCellPtr->mX                = x;
         navCellPtr->mY                = y;
         navCellPtr->mTessilationLevel = 0;

         SetMeshBoundries(pointA);
         SetMeshBoundries(pointB);
         SetMeshBoundries(pointC);

         std::list<WsfZone*>::iterator zoneItr;
         zoneItr                        = mZones.begin();
         ZONE_INTERACTION   insideCount = NO_INTERACTION;
         WsfZoneDefinition* zonePtr     = (WsfZoneDefinition*)*zoneItr;
         while (zoneItr != mZones.end())
         {
            zonePtr     = (WsfZoneDefinition*)*zoneItr;
            insideCount = GetCellZoneInteraction(navCellPtr, zonePtr);
            if (insideCount > NO_INTERACTION)
            {
               break;
            }
            ++zoneItr;
         }
         if (insideCount == NO_INTERACTION)
         {
            mMeshCells.push_back(navCellPtr);
            ++cellNumber;
            ++y;
         }
         else
         {
            if (insideCount != THREE_VERTS_INSIDE)
            {
               TessellateCell(navCellPtr, zonePtr, 0);
            }
         }

         // create the mirror of this one
         double             mirrorPointA[3] = {mUpperLeft.GetLat() - (static_cast<double>(j) + 1.5) * mGridSizeDegrees,
                                   mUpperLeft.GetLon() + (static_cast<double>(x) + 1.5) * mGridSizeDegrees,
                                   0.0};
         double             mirrorPointB[3] = {mUpperLeft.GetLat() - (static_cast<double>(j) + 0.5) * mGridSizeDegrees,
                                   mUpperLeft.GetLon() + (static_cast<double>(x) + 1.5) * mGridSizeDegrees,
                                   0.0};
         double             mirrorPointC[3] = {mUpperLeft.GetLat() - (static_cast<double>(j) + 1.5) * mGridSizeDegrees,
                                   mUpperLeft.GetLon() + (static_cast<double>(x) + 0.5) * mGridSizeDegrees,
                                   0.0};
         WsfNavigationCell* mirrorPtr       = new WsfNavigationCell();
         mirrorPtr->AddVertex(WsfGeoPoint(mirrorPointA[0], mirrorPointA[1], mirrorPointA[2]));
         mirrorPtr->AddVertex(WsfGeoPoint(mirrorPointB[0], mirrorPointB[1], mirrorPointB[2]));
         mirrorPtr->AddVertex(WsfGeoPoint(mirrorPointC[0], mirrorPointC[1], mirrorPointC[2]));
         mirrorPtr->mCellID           = cellNumber;
         mirrorPtr->mX                = x;
         mirrorPtr->mY                = y;
         mirrorPtr->mTessilationLevel = 0;

         SetMeshBoundries(mirrorPointA);
         SetMeshBoundries(mirrorPointB);
         SetMeshBoundries(mirrorPointC);

         zoneItr     = mZones.begin();
         insideCount = NO_INTERACTION;
         while (zoneItr != mZones.end())
         {
            zonePtr     = (WsfZoneDefinition*)*zoneItr;
            insideCount = GetCellZoneInteraction(mirrorPtr, zonePtr);
            if (insideCount > NO_INTERACTION)
            {
               break;
            }
            ++zoneItr;
         }
         if (insideCount == NO_INTERACTION)
         {
            mMeshCells.push_back(mirrorPtr);
            ++cellNumber;
            ++y;
         }
         else
         {
            if (insideCount != THREE_VERTS_INSIDE)
            {
               TessellateCell(mirrorPtr, zonePtr, 0);
            }
         }
      }
   }

   // at this point the boundaries have been calculated, set the center
   SetMeshCenter();
   // set a unique identifier for each cell...
   ReNumberMeshCells();

   // it's possible that some cells need to be further tessellated...
   mMeshCellsIterator = mMeshCells.begin();
   int idx            = 0;
   while (mMeshCellsIterator != mMeshCells.end())
   {
      WsfNavigationCell* navCellPtr = *mMeshCellsIterator;
      if (navCellPtr->mNoZoneInteraction)
      {
         ++mMeshCellsIterator;
         ++idx;
         continue;
      }
      std::list<WsfZone*>::iterator zoneItr;
      zoneItr                      = mZones.begin();
      ZONE_INTERACTION insideCount = NO_INTERACTION;

      WsfZoneDefinition* zonePtr = (WsfZoneDefinition*)*zoneItr;
      while (zoneItr != mZones.end())
      {
         zonePtr     = (WsfZoneDefinition*)*zoneItr;
         insideCount = GetCellZoneInteraction(navCellPtr, zonePtr);
         if (insideCount > NO_INTERACTION)
         {
            break;
         }
         ++zoneItr;
      }
      if (insideCount > NO_INTERACTION)
      {
         if (insideCount != THREE_VERTS_INSIDE)
         {
            WsfNavigationCell* navCopyPtr = new WsfNavigationCell();
            navCopyPtr->AddVertex(navCellPtr->mVerts[0]);
            navCopyPtr->AddVertex(navCellPtr->mVerts[1]);
            navCopyPtr->AddVertex(navCellPtr->mVerts[2]);
            navCopyPtr->mCellID           = navCellPtr->mCellNumber;
            navCopyPtr->mX                = navCellPtr->mX;
            navCopyPtr->mY                = navCellPtr->mY;
            navCopyPtr->mTessilationLevel = navCellPtr->mTessilationLevel;
            mMeshCells.erase(mMeshCellsIterator);
            TessellateCell(navCopyPtr, zonePtr, navCopyPtr->mTessilationLevel);
            ReNumberMeshCells();
            // start over?
            mMeshCellsIterator = mMeshCells.begin();
            idx                = 0;
         }
         else
         {
            mMeshCells.erase(mMeshCellsIterator);
            ReNumberMeshCells();
            // start over?
            mMeshCellsIterator = mMeshCells.begin();
            idx                = 0;
         }
      }
      else
      {
         // this cell has no interaction with zones, flag it
         navCellPtr->mNoZoneInteraction = true;
         ++mMeshCellsIterator;
         ++idx;
      }
   }

   // set a unique identifier for each cell...
   ReNumberMeshCells();

   // create the grid using the centroid of the polys now
   mGrid = std::vector<node_iterator>(mMeshCells.size());
   // resolve the neighbors, assign IDs and insert into the grid
   for (WsfNavigationCell* tmpPtr : mMeshCells)
   {
      GetCellNeighbors(tmpPtr);
      InsertCellIntoGrid(tmpPtr);
   }

   // once all the centroids are populated into the grid, link their neighbors
   for (WsfNavigationCell* tmpPtr : mMeshCells)
   {
      // get the cell from the cell list
      LinkGridNeighbors(tmpPtr);
   }
}

void WsfNavigationMesh::InsertCellIntoGrid(WsfNavigationCell* aNavCellPtr)
{
   double      centroidLoc[3];
   WsfGeoPoint tempPoint;
   aNavCellPtr->mCellCentroid.Get(centroidLoc);
   tempPoint = WsfGeoPoint(centroidLoc[0], centroidLoc[1], centroidLoc[2]);
   WsfPFNode* tempNodePtr;
   tempNodePtr = new WsfPFNode(aNavCellPtr->mCellNumber, 0, 1, tempPoint);
   insert(*tempNodePtr);
   SetGridAbsolute(aNavCellPtr->mCellNumber, find(*tempNodePtr));

   node_iterator nodeIter = find(*tempNodePtr);
   nodeIter->first.mWeight += aNavCellPtr->mWeight;
}

void WsfNavigationMesh::LinkGridNeighbors(WsfNavigationCell* aNavCellPtr)
{
   WsfPFNode  node       = (WsfPFNode&)*mGrid.at(aNavCellPtr->mCellID);
   WsfPFNode* tmpNodePtr = &node;
   // for each of temps neighbors, create an edge in the grid
   aNavCellPtr->mNeighborIterator = aNavCellPtr->mNeighbors.begin();
   while (aNavCellPtr->mNeighborIterator != aNavCellPtr->mNeighbors.end())
   {
      WsfNavigationCell* neighborPtr       = *aNavCellPtr->mNeighborIterator;
      WsfPFNode          anotherNode       = (WsfPFNode&)*GetGrid(neighborPtr->mCellNumber, 0);
      WsfPFNode*         anotherTmpNodePtr = &anotherNode;
      if (anotherTmpNodePtr->mX >= 0 && anotherTmpNodePtr->mY >= 0 && tmpNodePtr->mX >= 0 && tmpNodePtr->mY >= 0)
      {
         WsfPFEdge tempEdge(tmpNodePtr, anotherTmpNodePtr);
         insert_edge(GetGrid((long)aNavCellPtr->mCellNumber, 0), GetGrid((long)neighborPtr->mCellNumber, 0), tempEdge);
      }
      ++(aNavCellPtr->mNeighborIterator);
   }
}
void WsfNavigationMesh::ReNumberMeshCells()
{
   for (unsigned int q = 0; q < mMeshCells.size(); ++q)
   {
      WsfNavigationCell* tmpPtr = mMeshCells[q];
      tmpPtr->mCellNumber       = q;
      tmpPtr->mCellID           = q;
   }
}

void WsfNavigationMesh::SetMeshBoundries(double aLLA[3])
{
   if (aLLA[0] > mMaxLat)
   {
      mMaxLat = aLLA[0];
   }
   if (aLLA[0] < mMinLat)
   {
      mMinLat = aLLA[0];
   }

   if (aLLA[1] > mMaxLon)
   {
      mMaxLon = aLLA[1];
   }
   if (aLLA[1] < mMinLon)
   {
      mMinLon = aLLA[1];
   }
}

void WsfNavigationMesh::SetMeshCenter()
{
   mCenterLLA[0] = (mMaxLat + mMinLat) / 2.0;
   mCenterLLA[1] = (mMaxLon + mMinLon) / 2.0;
   mCenterLLA[2] = 0.0;
}

void WsfNavigationMesh::GetCellNeighbors(WsfNavigationCell* aNavCellPtr)
{
   mMeshCellsIterator = mMeshCells.begin();

   if (!aNavCellPtr->mNeighborIDs.empty())
   {
      // this means there was a navmesh imported, resolve these id's to pointers.
      for (unsigned int i = 0; i < aNavCellPtr->mNeighborIDs.size(); ++i)
      {
         int                id     = aNavCellPtr->mNeighborIDs[i];
         WsfNavigationCell* tmpPtr = mCellMap[id];
         aNavCellPtr->mNeighbors.push_back(tmpPtr);
      }
   }
   else
   {
      while (mMeshCellsIterator != mMeshCells.end())
      {
         WsfNavigationCell* tmpPtr = *mMeshCellsIterator;

         if (tmpPtr != aNavCellPtr)
         {
            // first verify that tmp isn't already a neighbor of navCell
            aNavCellPtr->mNeighborIterator = aNavCellPtr->mNeighbors.begin();
            bool found                     = false;
            while (aNavCellPtr->mNeighborIterator != aNavCellPtr->mNeighbors.end())
            {
               if (*aNavCellPtr->mNeighborIterator == tmpPtr)
               {
                  found = true;
                  break;
               }
               ++aNavCellPtr->mNeighborIterator;
            }
            if (found)
            {
               ++mMeshCellsIterator;
               continue;
            }

            // now loop through the sides...
            aNavCellPtr->mSideIterator = aNavCellPtr->mSides.begin();
            while (aNavCellPtr->mSideIterator != aNavCellPtr->mSides.end())
            {
               tmpPtr->mSideIterator = tmpPtr->mSides.begin();
               while (tmpPtr->mSideIterator != tmpPtr->mSides.end())
               {
                  UtLineSegment navCellLine = *aNavCellPtr->mSideIterator;
                  UtLineSegment tmpCellLine = *tmpPtr->mSideIterator;
                  if (navCellLine == tmpCellLine)
                  {
                     aNavCellPtr->mNeighbors.push_back(tmpPtr);
                  }
                  else if (navCellLine.IsCollinear(tmpCellLine))
                  {
                     aNavCellPtr->mNeighbors.push_back(tmpPtr);
                     tmpPtr->mNeighbors.push_back(aNavCellPtr);
                  }
                  ++tmpPtr->mSideIterator;
               }
               ++aNavCellPtr->mSideIterator;
            }
         }
         ++mMeshCellsIterator;
      }
   }
}

const WsfPFNode* WsfNavigationMesh::GetClosestNode(const WsfGeoPoint& aPointPtr)
{
   assert(mInitialized);
   WsfNavigationCell* tmpCellPtr = GetCellForPoint(aPointPtr);
   if (tmpCellPtr != nullptr)
   {
      const WsfPFNode* tmpNodePtr = &(GetGrid(tmpCellPtr->mCellNumber, 0)->first);
      return tmpNodePtr;
   }
   return nullptr;
}

WsfPathFinder::node_iterator WsfNavigationMesh::GetGrid(long aX, long aY)
{
   return std::find_if(begin(),
                       end(),
                       [=](const NodeMapValueType& iter) { return (iter.first.mX == aX && iter.first.mY == aY); });
}

bool WsfNavigationMesh::shortest_path(const_node_iterator aSrcNodeIter,
                                      const_node_iterator aDstNodeIter,
                                      NodeList&           aPath,
                                      double&             aCost,
                                      const cost_func*    aCostFuncPtr) const
{
   assert(mInitialized);

   if (!mInitialized)
   {
      return false;
   }

   if (aCostFuncPtr == nullptr)
   {
      static pf_cost_func costFunc;
      // The reinterprest_cast is required by the Visual Studio 6.0 compiler.
      // It should be necessary because comm_cost_func derives from cost_func.
      aCostFuncPtr = reinterpret_cast<cost_func*>(&costFunc);
   }

   bool result = PFGraph::shortest_path(aSrcNodeIter, aDstNodeIter, aPath, aCost, aCostFuncPtr);
   return result;
}

// virtual
bool WsfNavigationMesh::ComputeFindPath(WsfMover& aMover, WsfRoute& aRoute)
{
   int initialPoint(0);
   mAdjustedPath.clear();
   return ComputeSetRoute(aMover, aRoute, initialPoint);
}

// virtual
bool WsfNavigationMesh::ComputeSetRoute(WsfMover& aMover, WsfRoute& aRoute, int& aInitialPointIndex)
{
   bool routeChanged = false;
   // create a new route to append
   const WsfRoute* defRoutePtr = aMover.GetDefaultRoute();
   if (defRoutePtr && (aRoute.GetNameId() == defRoutePtr->GetNameId()))
   {
      WsfRoute tmpRoute;
      if (defRoutePtr->GetSize() > 0)
      {
         // if there is only one point in the default route, add it and move on.
         if (defRoutePtr->GetSize() == 1)
         {
            tmpRoute.SetSpeed(aRoute[0].GetSpeed());
            const WsfWaypoint& waypoint = aRoute.GetWaypointAt(0);
            tmpRoute.Append(waypoint);
            routeChanged = true;
         }
         else
         {
            for (unsigned int i = 0; i < aRoute.GetSize(); ++i)
            {
               const WsfWaypoint& waypoint = aRoute.GetWaypointAt(i);
               const WsfGeoPoint  newStart(waypoint.GetLat(), waypoint.GetLon(), 0.0);
               WsfWaypoint        endWaypoint;
               if (i == aRoute.GetSize() - 1)
               {
                  endWaypoint = aRoute.GetWaypointAt(0);
               }
               else
               {
                  endWaypoint = aRoute.GetWaypointAt(i + 1);
               }
               WsfGeoPoint newEnd(endWaypoint.GetLat(), endWaypoint.GetLon(), 0.0);
               // mDefaultRoute.Append(waypoint);
               double   cost = 0.0;
               WsfRoute newRoute;
               FindPath(newStart, newEnd, newRoute, cost);

               if (aRoute.GetSize() > 0)
               {
                  newRoute.SetSpeed(aRoute[0].GetSpeed());
                  WsfRoute refinedRoute;
                  RefineFromCurrentPosition(0, waypoint.GetLat(), waypoint.GetLon(), 0.0, refinedRoute, &newRoute);
                  refinedRoute.SetSpeed(aRoute[0].GetSpeed());
                  tmpRoute.Append(refinedRoute);
                  routeChanged = true;
               }
            }
            aRoute = tmpRoute;
            tmpRoute.Clear();
         }
      }
   }
   else
   {
      WsfRoute           newRoute;
      WsfRoute           newTemp;
      const WsfWaypoint& waypoint = aRoute.GetWaypointAt(0);
      const WsfGeoPoint  newStart(waypoint.GetLat(), waypoint.GetLon(), 0.0);
      const WsfWaypoint& endWaypoint = aRoute.GetWaypointAt(aRoute.GetSize() - 1);
      WsfGeoPoint        newEnd(endWaypoint.GetLat(), endWaypoint.GetLon(), 0.0);
      double             cost = 0.0;
      FindPath(newStart, newEnd, newRoute, cost);

      if (newRoute.GetSize() > 0)
      {
         newRoute.SetSpeed(aRoute[0].GetSpeed());
         aRoute = newRoute;
         aRoute.SetName("meshRoute");
         routeChanged = true;
      }
   }
   return routeChanged;
}

void WsfNavigationMesh::GetRouteIntersectionPoints()
{
   // for each waypoint in the route, get the cell it belongs in, get the next waypoint in the route
   // create a line connecting the two points and then get the intersection point on the shared edge of the cells

   std::vector<UtVec3d> intersectionPoints;
   for (unsigned int i = 0; i < mAdjustedPath.size() - 1; ++i)
   {
      WsfGeoPoint        fromPt(mAdjustedPath[i]);
      WsfGeoPoint        toPt(mAdjustedPath[i + 1]);
      WsfNavigationCell* fromCellPtr = GetCellForPoint(fromPt);
      UtVec3d            pointOfIntersection;
      UtLineSegment      motionPath;

      double beginPt[3];
      double endPt[3];

      fromPt.GetLocationLLA(beginPt[0], beginPt[1], beginPt[2]);
      toPt.GetLocationLLA(endPt[0], endPt[1], endPt[2]);
      motionPath.SetBeginPoint(beginPt);
      motionPath.SetEndPoint(endPt);

      for (UtLineSegment mySide : fromCellPtr->mSides)
      {
         UtLineSegment::LINE_CLASSIFICATION IntersectResult = motionPath.Intersects(&mySide, &pointOfIntersection);
         if (IntersectResult == UtLineSegment::SEGMENTS_INTERSECT || IntersectResult == UtLineSegment::A_BISECTS_B)
         {
            intersectionPoints.push_back(pointOfIntersection);
         }
      }
   }

   // now we have a list of the intersection points, insert them into the adjusted path
   std::vector<UtVec3d>::iterator pointItr;
   pointItr = intersectionPoints.begin();
   std::vector<WsfGeoPoint>::iterator adjPathItr;
   adjPathItr = mAdjustedPath.begin();
   int offset = 1;
   while (pointItr != intersectionPoints.end())
   {
      adjPathItr = mAdjustedPath.begin();
      // move the adjustedPath iterator to the offset position
      adjPathItr     = adjPathItr + offset;
      UtVec3d tmpVec = *pointItr;
      double  pointLLA[3];
      tmpVec.Get(pointLLA);
      WsfGeoPoint newPoint(pointLLA[0], pointLLA[1], pointLLA[2]);
      mAdjustedPath.insert(adjPathItr, newPoint);
      offset += 2;
      ++pointItr;
   }
}

bool WsfNavigationMesh::FindPath(const WsfGeoPoint& aStartPtr, WsfGeoPoint& aEndPtr, WsfRoute& aRoute, double& aCost)
{
   assert(mInitialized);

   if (!mAdjustedPath.empty())
   {
      // get the end point of the current adjusted path
      WsfGeoPoint curEnd      = mAdjustedPath[mAdjustedPath.size() - 1];
      double      endDistance = curEnd.GetDistanceFrom(aEndPtr);
      if (endDistance == 0)
      {
         for (unsigned int i = 0; i < mAdjustedPath.size(); ++i)
         {
            aRoute.Append(WsfWaypoint(mAdjustedPath[i], WsfPath::cDOUBLE_NOT_SET));
            aRoute[i].SetNodeIdNumber(i);
         }
         return true;
      }
   }
   aRoute.Clear();

   if (!mInitialized)
   {
      return false;
   }

   const WsfPFNode* firstPtr = GetClosestNode(aStartPtr);
   const WsfPFNode* lastPtr  = GetClosestNode(aEndPtr);

   if (firstPtr == nullptr || lastPtr == nullptr)
   {
      return false;
   }

   const_node_iterator firstIter = find(*firstPtr);
   const_node_iterator lastIter  = find(*lastPtr);
   NodeList            path;

   shortest_path(firstIter, lastIter, path, aCost);

   double lla[3];
   double ullla[3]; // upper left lla
   double llaEnd[3];
   aStartPtr.GetLocationLLA(lla[0], lla[1], lla[2]);        // get the altitude passed in at the start of the path.
   aEndPtr.GetLocationLLA(llaEnd[0], llaEnd[1], llaEnd[2]); // get the altitude passed in at the start of the path.
   mUpperLeft.GetLocationLLA(ullla[0], ullla[1], ullla[2]); // get the reference altitude of the upper left part of the
                                                            // pathfinder grid

   if (((lla[0] - llaEnd[0]) < FLOAT_EPSILON) && ((lla[1] - llaEnd[1]) < FLOAT_EPSILON) && path.size() <= 2)
   {
      // this is a 1 point path.  don't do anything with the adjusted path
      mAdjustedPath.clear();
      return false;
   }
   // refine the path generated to smooth the route
   mAdjustedPath.clear();
   mAdjustedPath.push_back(aStartPtr);
   for (unsigned int i = 1; i < path.size(); ++i)
   {
      if (i < path.size() - 1)
      {
         mAdjustedPath.push_back(path[i].mLoc);
      }
   }
   // add the final point
   mAdjustedPath.push_back(aEndPtr);

   // now get a little bit more information about the path to work with
   GetRouteIntersectionPoints();
   CleanUpDuplicatePoints();

   for (unsigned int i = 0; i < mAdjustedPath.size(); ++i)
   {
      aRoute.Append(WsfWaypoint(mAdjustedPath[i], WsfPath::cDOUBLE_NOT_SET));
      aRoute[i].SetNodeIdNumber(i);
   }
   return true;
}

void WsfNavigationMesh::RefineFromCurrentIndex(int aIndex)
{
   // get the furthest point we can see from this index
   WsfRoute tmpRoute;
   int      farthestPt = GetFurthestVisiblePoint(aIndex, tmpRoute);

   // copy from the front everything from the original path
   std::vector<WsfGeoPoint> modifiedPathFront;
   for (int i = 0; i < aIndex; ++i)
   {
      modifiedPathFront.push_back(mAdjustedPath[i]);
   }

   // copy from the back everything up to the farthestPt
   std::vector<WsfGeoPoint> modifiedPathBack;
   for (auto i = farthestPt; i < ut::cast_to_int(mAdjustedPath.size()); ++i)
   {
      modifiedPathBack.push_back(mAdjustedPath[i]);
   }

   // now create a section of the path based on the route generated from the farthest point check
   std::vector<WsfGeoPoint> modifiedPathMiddle;
   for (unsigned int i = 0; i < tmpRoute.GetSize(); ++i)
   {
      WsfWaypoint tmpPt = tmpRoute.GetWaypointAt(i);
      double      ptLLA[3];
      tmpPt.GetLocationLLA(ptLLA[0], ptLLA[1], ptLLA[2]);
      WsfGeoPoint ptGeo(ptLLA[0], ptLLA[1], ptLLA[2]);
      modifiedPathMiddle.push_back(ptGeo);
   }

   // now glue all the pieces together to get the final path
   modifiedPathFront.insert(modifiedPathFront.end(), modifiedPathMiddle.begin(), modifiedPathMiddle.end());
   modifiedPathFront.insert(modifiedPathFront.end(), modifiedPathBack.begin(), modifiedPathBack.end());

   // finally set the adjusted path to this new one
   mAdjustedPath = modifiedPathFront;
}

bool WsfNavigationMesh::RefineFromCurrentPosition(int             aIndex,
                                                  double          aLat,
                                                  double          aLon,
                                                  double          aAlt,
                                                  WsfRoute&       aRoute,
                                                  const WsfRoute* aOriginalRoute)
{
   mAdjustedPath.clear();
   // copy the original route into the adjusted path
   for (unsigned int i = 0; i < aOriginalRoute->GetSize(); ++i)
   {
      const WsfWaypoint& tmpWayPt = aOriginalRoute->GetWaypointAt(i);
      double             wayPtLLA[3];
      tmpWayPt.GetLocationLLA(wayPtLLA[0], wayPtLLA[1], wayPtLLA[2]);
      WsfGeoPoint geoPt(wayPtLLA[0], wayPtLLA[1], 0.0);
      mAdjustedPath.push_back(geoPt);
   }

   if (mAdjustedPath.empty())
   {
      return false;
   }

   // find out where we are along the path, add a new point into the adjust path, assign that new point to be aIndex,
   // calculate the new path, delete the point?
   WsfGeoPoint                        newPoint(aLat, aLon, aAlt);
   std::vector<WsfGeoPoint>::iterator adjPathItr;
   adjPathItr = mAdjustedPath.begin();
   adjPathItr = adjPathItr + (aIndex);
   adjPathItr = mAdjustedPath.insert(adjPathItr, newPoint);
   ++adjPathItr;
   mAdjustedPath.erase(adjPathItr);
   // get the furthest point we can see from this index
   WsfRoute tmpRoute;
   tmpRoute.Clear();
   int farthestPt = GetFurthestVisiblePoint(aIndex, tmpRoute);

   // copy from the front everything from the original path
   std::vector<WsfGeoPoint> modifiedPathFront;
   for (int i = 0; i < aIndex; ++i)
   {
      modifiedPathFront.push_back(mAdjustedPath[i]);
   }

   // copy from the back everything up to the farthestPt
   std::vector<WsfGeoPoint> modifiedPathBack;
   for (int i = farthestPt; i < ut::cast_to_int(mAdjustedPath.size()); ++i)
   {
      modifiedPathBack.push_back(mAdjustedPath[i]);
   }

   // now create a section of the path based on the route generated from the farthest point check
   std::vector<WsfGeoPoint> modifiedPathMiddle;
   for (unsigned int i = 0; i < tmpRoute.GetSize(); ++i)
   {
      WsfWaypoint tmpPt = tmpRoute.GetWaypointAt(i);
      double      ptLLA[3];
      tmpPt.GetLocationLLA(ptLLA[0], ptLLA[1], ptLLA[2]);
      WsfGeoPoint ptGeo(ptLLA[0], ptLLA[1], 0.0);
      modifiedPathMiddle.push_back(ptGeo);
   }

   // now glue all the pieces together to get the final path
   modifiedPathFront.insert(modifiedPathFront.end(), modifiedPathMiddle.begin(), modifiedPathMiddle.end());
   modifiedPathFront.insert(modifiedPathFront.end(), modifiedPathBack.begin(), modifiedPathBack.end());

   // finally set the adjusted path to this new one
   mAdjustedPath = modifiedPathFront;
   CleanUpDuplicatePoints();

   int nodeID = 0;
   for (unsigned int i = 0; i < ut::safe_cast<unsigned int, size_t>(mAdjustedPath.size()); ++i)
   {
      aRoute.Append(WsfWaypoint(mAdjustedPath[i], WsfPath::cDOUBLE_NOT_SET));
      aRoute[nodeID].SetNodeIdNumber(i);
   }
   aRoute.SetSpeed(aOriginalRoute->GetSpeed(0));
   aRoute.SetName("meshRoute");
   return true;
}

int WsfNavigationMesh::GetFurthestVisiblePoint(int aIndex, WsfRoute& aRoute)
{
   // see if we are already talking about the last point
   if (aIndex >= (int)mAdjustedPath.size() - 1)
   {
      return static_cast<int>(mAdjustedPath.size()) - 1;
   }
   std::vector<WsfGeoPoint>::iterator pathItr;
   pathItr = mAdjustedPath.begin();
   pathItr += aIndex;

   std::vector<WsfGeoPoint>::iterator pathEndItr;
   int                                lastVisibleIndex = aIndex;
   pathEndItr                                          = mAdjustedPath.end();
   --pathEndItr;
   if (pathItr == pathEndItr)
   {
      return static_cast<int>(mAdjustedPath.size()) - 1;
   }
   ++pathItr;
   ++lastVisibleIndex;
   WsfNavigationCell* fromCellPtr = GetCellForPoint(mAdjustedPath[aIndex]);
   WsfNavigationCell* nextCellPtr = GetCellForPoint(mAdjustedPath[aIndex + 1]);

   if (nextCellPtr && !fromCellPtr)
   {
      return aIndex + 1;
   }
   if (!nextCellPtr || !fromCellPtr)
   {
      return aIndex;
   }

   WsfGeoPoint tmpPoint = mAdjustedPath[aIndex];
   while (!fromCellPtr)
   {
      MovePointAlongPath(tmpPoint, aIndex, aIndex + 1);
      fromCellPtr = GetCellForPoint(tmpPoint);
   }

   WsfRoute curValidRoute;
   while (pathItr != mAdjustedPath.end()) // pathEndItr)
   {
      // if we cant get line-of-sight on this node, then the previous one is our last visible node
      if (!LineOfSightTest(fromCellPtr, aIndex, lastVisibleIndex, aRoute))
      {
         --lastVisibleIndex;
         aRoute = curValidRoute;
         return lastVisibleIndex;
      }
      curValidRoute = aRoute;
      aRoute.Clear();
      ++lastVisibleIndex;
      ++pathItr;
   }

   aRoute = curValidRoute;
   return lastVisibleIndex;
}

void WsfNavigationMesh::CleanUpDuplicatePoints()
{
   WsfGeoPoint                        prevPoint;
   std::vector<WsfGeoPoint>::iterator adjItr;
   adjItr               = mAdjustedPath.begin();
   WsfGeoPoint curPoint = *adjItr;
   prevPoint            = curPoint;
   ++adjItr;
   while (adjItr != mAdjustedPath.end())
   {
      curPoint = *adjItr;
      if (curPoint.GetLat() == 0.0 && curPoint.GetLon() == 0.0)
      {
         adjItr = mAdjustedPath.erase(adjItr);
         continue;
      }
      double curPointLLA[3];
      double prevPointLLA[3];
      curPoint.GetLocationLLA(curPointLLA[0], curPointLLA[1], curPointLLA[2]);
      prevPoint.GetLocationLLA(prevPointLLA[0], prevPointLLA[1], prevPointLLA[2]);
      if (fabs(curPointLLA[0] - prevPointLLA[0]) < 1.0E-5F)
      {
         if (fabs(curPointLLA[1] - prevPointLLA[1]) < 1.0E-5F)
         {
            if (fabs(curPointLLA[2] - prevPointLLA[2]) < 1.0E-5F)
            {
               adjItr = mAdjustedPath.erase(adjItr);
               continue;
            }
         }
      }
      prevPoint = curPoint;
      ++adjItr;
   }
}
bool WsfNavigationMesh::LineOfSightTest(WsfNavigationCell* aFromCellPtr, int aIndexFrom, int aIndexTo, WsfRoute& aRoute)
{
   WsfNavigationCell::PATH_RESULT theResult = WsfNavigationCell::NO_RELATIONSHIP;
   WsfNavigationCell::CELL_SIDE   wallNumber;
   WsfNavigationCell*             nextCellPtr = aFromCellPtr;
   UtVec3d                        PointOfIntersection;
   UtLineSegment                  motionPath;

   double beginPt[3];
   double endPt[3];

   mAdjustedPath[aIndexFrom].GetLocationLLA(beginPt[0], beginPt[1], beginPt[2]);
   mAdjustedPath[aIndexTo].GetLocationLLA(endPt[0], endPt[1], endPt[2]);
   motionPath.SetBeginPoint(beginPt);
   motionPath.SetEndPoint(endPt);

   aRoute.Append(WsfWaypoint(mAdjustedPath[aIndexFrom], WsfPath::cDOUBLE_NOT_SET));

   // as it stands, the points in the adjusted path are all centroids of cells within the navigation mesh
   WsfNavigationCell* prevCellPtr = aFromCellPtr;
   WsfNavigationCell* curCellPtr  = aFromCellPtr;

   if (curCellPtr == nullptr)
   {
      return false;
   }
   while (curCellPtr &&
          (theResult =
              curCellPtr->ClassifyPathToCell(motionPath, &nextCellPtr, wallNumber, &PointOfIntersection, prevCellPtr)) !=
             WsfNavigationCell::ENDING_CELL)
   {
      if (theResult == WsfNavigationCell::EXITING_CELL)
      {
         if (nextCellPtr == nullptr)
         {
            return (false);
         }
         double intersectPt[3];
         PointOfIntersection.Get(intersectPt);
         WsfGeoPoint geoIntersectPt(intersectPt[0], intersectPt[1], intersectPt[2]);
         aRoute.Append(WsfWaypoint(geoIntersectPt, WsfPath::cDOUBLE_NOT_SET));
         prevCellPtr = curCellPtr;
         curCellPtr  = nextCellPtr;
      }
      else if (theResult == WsfNavigationCell::NO_RELATIONSHIP)
      {
         double pointLLA[3];
         motionPath.GetBeginPoint(pointLLA);
         WsfGeoPoint tmpPoint(pointLLA[0], pointLLA[1], pointLLA[2]);
         MovePointAlongPath(tmpPoint, aIndexFrom, aIndexTo);
         tmpPoint.GetLocationLLA(pointLLA[0], pointLLA[1], pointLLA[2]);
         motionPath.SetBeginPoint(pointLLA);
         prevCellPtr = curCellPtr;
         curCellPtr  = GetCellForPoint(tmpPoint);
      }
   }

   if (theResult == WsfNavigationCell::ENDING_CELL)
   {
      double intersectPt[3];
      motionPath.GetEndPoint(intersectPt);
      if (intersectPt[2] < 0.0 || intersectPt[2] > 0.0)
      {
         intersectPt[2] = 0.0;
      }
      WsfGeoPoint geoIntersectPt(intersectPt[0], intersectPt[1], intersectPt[2]);
      aRoute.Append(WsfWaypoint(geoIntersectPt, WsfPath::cDOUBLE_NOT_SET));
   }

   return (theResult == WsfNavigationCell::ENDING_CELL);
}

void WsfNavigationMesh::MovePointAlongPath(WsfGeoPoint& aPoint, int aIndexFrom, int aIndexTo)
{
   // given the starting index, create a vector between the index pt and the next point on the path.  move the given
   // point along that vector a little bit...
   double startPoint[3];
   double endPoint[3];

   mAdjustedPath[aIndexFrom].GetLocationLLA(startPoint[0], startPoint[1], startPoint[2]);
   mAdjustedPath[aIndexTo].GetLocationLLA(endPoint[0], endPoint[1], endPoint[2]);

   double pathVec[3];

   UtVec3d::Subtract(pathVec, endPoint, startPoint);
   UtVec3d::Normalize(pathVec);
   double adjustedPoint[3];
   aPoint.GetLocationLLA(adjustedPoint[0], adjustedPoint[1], adjustedPoint[2]);

   adjustedPoint[0] = adjustedPoint[0] + (0.00001 * pathVec[0]);
   adjustedPoint[1] = adjustedPoint[1] + (0.00001 * pathVec[1]);
   adjustedPoint[2] = adjustedPoint[2] + (0.00001 * pathVec[2]);

   aPoint.SetLocationLLA(adjustedPoint[0], adjustedPoint[1], adjustedPoint[2]);
}

void WsfNavigationMesh::MovePointTowardsCentroid(WsfGeoPoint& aPoint, int aIndexFrom, WsfNavigationCell* aCellPtr)
{
   double startPoint[3];
   double endPoint[3];

   mAdjustedPath[aIndexFrom].GetLocationLLA(startPoint[0], startPoint[1], startPoint[2]);
   aCellPtr->mCellCentroid.Get(endPoint[0], endPoint[1], endPoint[2]);

   double pathVec[3];

   UtVec3d::Subtract(pathVec, endPoint, startPoint);
   UtVec3d::Normalize(pathVec);
   double adjustedPoint[3];
   aPoint.GetLocationLLA(adjustedPoint[0], adjustedPoint[1], adjustedPoint[2]);

   adjustedPoint[0] = adjustedPoint[0] + (0.00001 * pathVec[0]);
   adjustedPoint[1] = adjustedPoint[1] + (0.00001 * pathVec[1]);
   adjustedPoint[2] = adjustedPoint[2] + (0.00001 * pathVec[2]);

   aPoint.SetLocationLLA(adjustedPoint[0], adjustedPoint[1], adjustedPoint[2]);
}

WsfNavigationCell* WsfNavigationMesh::GetCellForPoint(const WsfGeoPoint& aPoint)
{
   // first check the cache to see if this cell has been accessed recently
   std::vector<WsfNavigationCell*>::iterator cacheItr;
   cacheItr = mGetCellForPointCache.begin();
   while (cacheItr != mGetCellForPointCache.end())
   {
      WsfNavigationCell* cacheCellPtr = *cacheItr;
      if (cacheCellPtr->PointInCell(aPoint.GetLat(), aPoint.GetLon(), aPoint.GetAlt()))
      {
         return cacheCellPtr;
      }
      ++cacheItr;
   }

   mMeshCellsIterator = mMeshCells.begin();
   while (mMeshCellsIterator != mMeshCells.end())
   {
      WsfNavigationCell* tmpCellPtr = *mMeshCellsIterator;

      if (tmpCellPtr->PointInCell(aPoint.GetLat(), aPoint.GetLon(), aPoint.GetAlt()))
      {
         // a cell has been found, add it to the cache
         cacheItr = mGetCellForPointCache.begin();
         cacheItr = mGetCellForPointCache.insert(cacheItr, tmpCellPtr);
         // limit the size of the cache
         if (mGetCellForPointCache.size() > 25)
         {
            mGetCellForPointCache.resize(25);
         }

         return tmpCellPtr;
      }
      ++mMeshCellsIterator;
   }
   return nullptr;
}

std::vector<WsfNavigationCell*> WsfNavigationMesh::GetCellsForPoint(const WsfGeoPoint& aPoint)
{
   std::vector<WsfNavigationCell*> cells;
   mMeshCellsIterator = mMeshCells.begin();
   while (mMeshCellsIterator != mMeshCells.end())
   {
      WsfNavigationCell* tmpCellPtr = *mMeshCellsIterator;

      if (tmpCellPtr->PointInCell(aPoint.GetLat(), aPoint.GetLon(), aPoint.GetAlt()))
      {
         cells.push_back(tmpCellPtr);
      }
      ++mMeshCellsIterator;
   }
   return cells;
}


WsfNavigationMesh::ZONE_INTERACTION WsfNavigationMesh::GetCellZoneInteraction(WsfNavigationCell* aCellPtr,
                                                                              WsfZoneDefinition* aZonePtr)
{
   // test to see if a cell lies within a zone
   // there are four cases to consider here
   //  1) a zone is fully contained within a cell  (tessellation?)
   //  2) a cell is fully contained within a zone (just mark all points not pathable)
   //  3) a cell is partially contained within a zone (tessellation?)
   //  4) a point of the zone lies within the cell
   double locWCS[3]   = {0};
   int    insideCount = 0;

   if (aCellPtr->mVerts.empty())
   {
      return NO_INTERACTION;
   }
   for (aCellPtr->mVertexIterator = aCellPtr->mVerts.begin(); aCellPtr->mVertexIterator != aCellPtr->mVerts.end();
        ++(aCellPtr->mVertexIterator))
   {
      WsfGeoPoint geoPt = *aCellPtr->mVertexIterator;
      geoPt.GetLocationWCS(locWCS);
      if (aZonePtr->PointIsInside(GetSimulation(), locWCS, nullptr, 0))
      {
         ++insideCount;
      }
   }

   // as a final check, loop through all verts of the zone and see if any of them lie within the cell
   std::vector<WsfZone::Point>           zonePoints = aZonePtr->GetPoints();
   std::vector<WsfZone::Point>::iterator zonePointItr;
   zonePointItr = zonePoints.begin();
   while (zonePointItr != zonePoints.end())
   {
      WsfZoneDefinition::Point* ptPtr        = &*zonePointItr;
      double                    zonePointLat = ptPtr->mX;
      double                    zonePointLon = ptPtr->mY;
      double                    zonePointAlt = 0.0;

      if (aCellPtr->PointInCell(zonePointLat, zonePointLon, zonePointAlt))
      {
         return ZONE_VERT_INSIDE;
      }
      ++zonePointItr;
   }

   return (ZONE_INTERACTION)insideCount;
}
void WsfNavigationMesh::TessellateCell(WsfNavigationCell* aCellPtr, WsfZoneDefinition* aZonePtr, int aCurrentTessLevel)
{
   // The following should always be true about the cell passed into this function
   // point 1 is the where the right angle of the cell is
   // points 2, 3 make up the hypotenuse of the triangle representing the cell

   /*
        B
        |\
        | \
        |  \ a
      c |   \
        |    \
        |     \
      A -------- C
            b
   */
   // find which of the points lies at the right angle...
   // angle is perpendicular when the dot product of the two vectors is 0
   double AtoB[3];
   double BtoC[3];
   double CtoA[3];
   double pointA[3];
   double pointB[3];
   double pointC[3];

   aCellPtr->mVerts[0].GetLocationLLA(pointA[0], pointA[1], pointA[2]);
   aCellPtr->mVerts[1].GetLocationLLA(pointB[0], pointB[1], pointB[2]);
   aCellPtr->mVerts[2].GetLocationLLA(pointC[0], pointC[1], pointC[2]);

   UtVec3d::Subtract(AtoB, pointB, pointA);
   UtVec3d::Subtract(BtoC, pointC, pointB);
   UtVec3d::Subtract(CtoA, pointA, pointC);

   int rightAngleIndex = 0;
   int angleOne        = 0;
   int angleTwo        = 0;
   if (UtVec3d::DotProduct(AtoB, CtoA) == 0)
   {
      rightAngleIndex = 0;
      angleOne        = 2;
      angleTwo        = 1;
   }
   else if (UtVec3d::DotProduct(AtoB, BtoC) == 0)
   {
      rightAngleIndex = 1;
      angleOne        = 0;
      angleTwo        = 2;
   }
   else if (UtVec3d::DotProduct(BtoC, CtoA) == 0)
   {
      rightAngleIndex = 2;
      angleOne        = 1;
      angleTwo        = 0;
   }

   double midPointC[3];
   double midPointB[3];
   double midPointA[3];

   double aX = aCellPtr->mVerts[rightAngleIndex].GetLat();
   double aY = aCellPtr->mVerts[rightAngleIndex].GetLon();
   double bX = aCellPtr->mVerts[angleOne].GetLat();
   double bY = aCellPtr->mVerts[angleOne].GetLon();
   double cX = aCellPtr->mVerts[angleTwo].GetLat();
   double cY = aCellPtr->mVerts[angleTwo].GetLon();

   midPointC[0] = (aX + bX) / 2.0;
   midPointC[1] = (aY + bY) / 2.0;
   midPointC[2] = 0;

   midPointB[0] = (aX + cX) / 2.0;
   midPointB[1] = (aY + cY) / 2.0;
   midPointB[2] = 0;

   midPointA[0] = (bX + cX) / 2.0;
   midPointA[1] = (bY + cY) / 2.0;
   midPointA[2] = 0;

   // turn this initial cell into 4 cells
   WsfNavigationCell* navCellAPtr = new WsfNavigationCell();
   navCellAPtr->AddVertex(WsfGeoPoint(aX, aY, 0));
   navCellAPtr->AddVertex(WsfGeoPoint(midPointB[0], midPointB[1], midPointB[2]));
   navCellAPtr->AddVertex(WsfGeoPoint(midPointC[0], midPointC[1], midPointC[2]));
   navCellAPtr->mCellID           = 1;
   navCellAPtr->mX                = 0;
   navCellAPtr->mY                = 0;
   navCellAPtr->mTessilationLevel = aCurrentTessLevel;

   ZONE_INTERACTION insideCount = GetCellZoneInteraction(navCellAPtr, aZonePtr);

   if (insideCount == NO_INTERACTION)
   {
      navCellAPtr->mTessillated = true;
      mMeshCells.push_back(navCellAPtr);
   }
   else
   {
      // if this is the case then all points of the cell are inside of the zone, no need to make this cell
      if (insideCount != THREE_VERTS_INSIDE && aCurrentTessLevel < MAX_TESSILLATION_LEVEL)
      {
         TessellateCell(navCellAPtr, aZonePtr, aCurrentTessLevel + 1);
      }
   }

   WsfNavigationCell* navCellBPtr = new WsfNavigationCell();
   navCellBPtr->AddVertex(WsfGeoPoint(midPointA[0], midPointA[1], midPointA[2]));
   navCellBPtr->AddVertex(WsfGeoPoint(midPointC[0], midPointC[1], midPointC[2]));
   navCellBPtr->AddVertex(WsfGeoPoint(midPointB[0], midPointB[1], midPointB[2]));
   navCellBPtr->mCellID           = 1;
   navCellBPtr->mX                = 0;
   navCellBPtr->mY                = 0;
   navCellBPtr->mTessilationLevel = aCurrentTessLevel;

   insideCount = GetCellZoneInteraction(navCellBPtr, aZonePtr);

   if (insideCount == NO_INTERACTION)
   {
      navCellBPtr->mTessillated = true;
      mMeshCells.push_back(navCellBPtr);
   }
   else
   {
      // if this is the case then all points of the cell are inside of the zone, no need to make this cell
      if (insideCount != THREE_VERTS_INSIDE && aCurrentTessLevel < MAX_TESSILLATION_LEVEL)
      {
         TessellateCell(navCellBPtr, aZonePtr, aCurrentTessLevel + 1);
      }
   }

   WsfNavigationCell* navCellCPtr = new WsfNavigationCell();
   navCellCPtr->AddVertex(WsfGeoPoint(midPointC[0], midPointC[1], midPointC[2]));
   navCellCPtr->AddVertex(WsfGeoPoint(midPointA[0], midPointA[1], midPointA[2]));
   navCellCPtr->AddVertex(WsfGeoPoint(bX, bY, 0));
   navCellCPtr->mCellID           = 1;
   navCellCPtr->mX                = 0;
   navCellCPtr->mY                = 0;
   navCellCPtr->mTessilationLevel = aCurrentTessLevel;
   insideCount                    = GetCellZoneInteraction(navCellCPtr, aZonePtr);

   if (insideCount == NO_INTERACTION)
   {
      mMeshCells.push_back(navCellCPtr);
   }
   else
   {
      // if this is the case then all points of the cell are inside of the zone, no need to make this cell
      if (insideCount != THREE_VERTS_INSIDE && aCurrentTessLevel < MAX_TESSILLATION_LEVEL)
      {
         navCellCPtr->mTessillated = true;
         TessellateCell(navCellCPtr, aZonePtr, aCurrentTessLevel + 1);
      }
   }

   WsfNavigationCell* navCellDPtr = new WsfNavigationCell();
   navCellDPtr->AddVertex(WsfGeoPoint(midPointB[0], midPointB[1], midPointB[2]));
   navCellDPtr->AddVertex(WsfGeoPoint(cX, cY, 0));
   navCellDPtr->AddVertex(WsfGeoPoint(midPointA[0], midPointA[1], midPointA[2]));
   navCellDPtr->mCellID           = 1;
   navCellDPtr->mX                = 0;
   navCellDPtr->mY                = 0;
   navCellDPtr->mTessilationLevel = aCurrentTessLevel;

   insideCount = GetCellZoneInteraction(navCellDPtr, aZonePtr);

   if (insideCount == NO_INTERACTION)
   {
      navCellDPtr->mTessillated = true;
      mMeshCells.push_back(navCellDPtr);
   }
   else
   {
      // if this is the case then all points of the cell are inside of the zone, no need to make this cell
      if (insideCount != THREE_VERTS_INSIDE && aCurrentTessLevel < MAX_TESSILLATION_LEVEL)
      {
         TessellateCell(navCellDPtr, aZonePtr, aCurrentTessLevel + 1);
      }
   }
}

void WsfNavigationMesh::GenerateExportedNavMesh()
{
   std::ofstream outNavMeshBin(mOutputFile.c_str(), std::ios::binary | std::ios::out);
   if (outNavMeshBin.is_open())
   {
      int numCells = static_cast<int>(mMeshCells.size());
      outNavMeshBin.write(reinterpret_cast<char*>(&numCells), sizeof(int));
      std::vector<WsfNavigationCell*>::iterator cellItr;
      cellItr = mMeshCells.begin();
      while (cellItr != mMeshCells.end())
      {
         WsfNavigationCell* tmpCellPtr = *cellItr;

         /***********************/
         /**********Verts********/
         /**********************/
         // write out the number of verts
         int numVerts = static_cast<int>(tmpCellPtr->mVerts.size());
         outNavMeshBin.write(reinterpret_cast<char*>(&numVerts), sizeof(int));
         // now loop through all the verts and write out all components of them
         tmpCellPtr->mVertexIterator = tmpCellPtr->mVerts.begin();
         while (tmpCellPtr->mVertexIterator != tmpCellPtr->mVerts.end())
         {
            WsfGeoPoint tmpPoint = *tmpCellPtr->mVertexIterator;
            double      tmpLat   = tmpPoint.GetLat();
            double      tmpLon   = tmpPoint.GetLon();
            double      tmpAlt   = tmpPoint.GetAlt();
            outNavMeshBin.write(reinterpret_cast<char*>(&tmpLat), sizeof(double));
            outNavMeshBin.write(reinterpret_cast<char*>(&tmpLon), sizeof(double));
            outNavMeshBin.write(reinterpret_cast<char*>(&tmpAlt), sizeof(double));
            ++tmpCellPtr->mVertexIterator;
         }

         /***********************/
         /**********Neighbors****/
         /**********************/
         int numNeighbors = static_cast<int>(tmpCellPtr->mNeighbors.size());
         outNavMeshBin.write(reinterpret_cast<char*>(&numNeighbors), sizeof(int));
         // now loop through all the verts and write out all components of them
         tmpCellPtr->mNeighborIterator = tmpCellPtr->mNeighbors.begin();
         while (tmpCellPtr->mNeighborIterator != tmpCellPtr->mNeighbors.end())
         {
            WsfNavigationCell* tmpNeighborPtr = *tmpCellPtr->mNeighborIterator;
            outNavMeshBin.write(reinterpret_cast<char*>(&tmpNeighborPtr->mCellID), sizeof(int));
            ++tmpCellPtr->mNeighborIterator;
         }
         // Add the id / Cell number
         outNavMeshBin.write(reinterpret_cast<char*>(&tmpCellPtr->mCellID), sizeof(int));
         // Add the TessilationLevel
         outNavMeshBin.write(reinterpret_cast<char*>(&tmpCellPtr->mTessilationLevel), sizeof(int));

         ++cellItr;
      }
   }
   outNavMeshBin.close();
}

void WsfNavigationMesh::ImportNavMesh()
{
   // if this is a .txt file, it is not binary, parse it like a normal script file
   size_t found;
   found = mInputFile.find(".txt");

   // it is a .txt file
   if (found != std::string::npos)
   {
      UtInput input;
      input.PushInput(ut::make_unique<UtInputFile>(mInputFile));
      std::string line;
      std::string curZone;
      bool        endOfFile = false;
      while (!endOfFile)
      {
         try
         {
            input.ReadValue(line); // read in zone
            input.ReadValue(line); // read in zone name
            curZone = line;
            input.ReadValue(line); // read in polygonal
            input.ReadValue(line); // read in latlon


            bool   hasWeight = false;
            double weight    = 0.0;
            input.ReadValue(line); // read in weight
            if (line == "weight")
            {
               input.ReadValue(weight);
               hasWeight = true;
            }

            if (hasWeight)
            {
               input.ReadValue(line); // read in point or modifier
            }

            WsfNavigationCell* navCellPtr = new WsfNavigationCell();

            // read in the modifiers
            while (line != "point")
            {
               if (line == "modifier")
               {
                  std::string modifierKey;
                  input.ReadValue(modifierKey);
                  std::string modifierValue;
                  input.ReadValue(modifierValue);
                  WsfStringId keyId                = modifierKey;
                  WsfStringId valueId              = modifierValue;
                  navCellPtr->mModifierList[keyId] = valueId;
               }
               input.ReadValue(line); // read in point or modifier
            }
            // read in the points
            while (line != "end_zone")
            {
               double xOrLat;
               double yOrLon;

               input.ReadValueOfType(xOrLat, UtInput::cLATITUDE);
               input.ReadValueOfType(yOrLon, UtInput::cLONGITUDE);
               // check to make sure no duplicate points are added...
               bool dupeFound = false;
               for (const WsfGeoPoint& curPt : navCellPtr->mVerts)
               {
                  if (curPt.GetLat() == xOrLat && curPt.GetLon() == yOrLon)
                  {
                     dupeFound = true;
                     break;
                  }
               }
               if (!dupeFound)
               {
                  navCellPtr->AddVertex(WsfGeoPoint(xOrLat, yOrLon, 0));
               }
               input.ReadValue(line); // read in point
            }
            weight = (weight == -1.0) ? std::numeric_limits<double>::max() : weight;
            navCellPtr->SetWeight(weight);
            navCellPtr->mCellID           = ut::safe_cast<unsigned int, size_t>(mMeshCells.size());
            navCellPtr->mCellNumber       = navCellPtr->mCellID;
            navCellPtr->mTessilationLevel = 0;
            navCellPtr->mTessillated      = false;
            mMeshCells.push_back(navCellPtr);

         } // end try
         catch (std::exception&)
         {
            endOfFile = true;
         }
      }
   }
   // it is not a file with .txt
   else
   {
      std::ifstream inNavMeshBin(mInputFile.c_str(), std::ios::binary);
      if (inNavMeshBin.is_open())
      {
         int numCells;
         inNavMeshBin.read(reinterpret_cast<char*>(&numCells), sizeof(int));

         // start reading in the cells
         for (int i = 0; i < numCells; ++i)
         {
            WsfNavigationCell* navCellPtr = new WsfNavigationCell();

            // read in the verts
            int numVerts = 0;
            inNavMeshBin.read(reinterpret_cast<char*>(&numVerts), sizeof(int));
            for (int j = 0; j < numVerts; ++j)
            {
               double cellLat = 0.0;
               double cellLon = 0.0;
               double cellAlt = 0.0;
               inNavMeshBin.read(reinterpret_cast<char*>(&cellLat), sizeof(double));
               inNavMeshBin.read(reinterpret_cast<char*>(&cellLon), sizeof(double));
               inNavMeshBin.read(reinterpret_cast<char*>(&cellAlt), sizeof(double));
               navCellPtr->AddVertex(WsfGeoPoint(cellLat, cellLon, cellAlt));
            }

            // read in the neighbors, these will get resolved to pointers later
            int numNeighbors;
            inNavMeshBin.read(reinterpret_cast<char*>(&numNeighbors), sizeof(int));
            for (int k = 0; k < numNeighbors; ++k)
            {
               int neighborNum;
               inNavMeshBin.read(reinterpret_cast<char*>(&neighborNum), sizeof(int));
               navCellPtr->mNeighborIDs.push_back(ut::safe_cast<unsigned int, int>(neighborNum));
            }

            // read in the CellID/Number;
            int cellNumID;
            inNavMeshBin.read(reinterpret_cast<char*>(&cellNumID), sizeof(int));
            navCellPtr->mCellID     = ut::safe_cast<unsigned int, int>(cellNumID);
            navCellPtr->mCellNumber = ut::safe_cast<unsigned int, int>(cellNumID);

            // read in the tessilationlevel
            int tessLevel = 0;
            inNavMeshBin.read(reinterpret_cast<char*>(&tessLevel), sizeof(int));
            navCellPtr->mTessilationLevel = tessLevel;
            navCellPtr->mTessillated      = tessLevel > 0 ? true : false;

            // finally add the cell to the mesh
            mMeshCells.push_back(navCellPtr);
            mCellMap[cellNumID] = navCellPtr;
         }
      }
      inNavMeshBin.close();
   }
   // create the grid
   mGrid = std::vector<node_iterator>(mMeshCells.size());

   // now that all the cells are in the mesh, we need to resolve the neighbors and add the cells to the grid.
   ut::log::info() << "Processing Navigation Mesh...";

   for (WsfNavigationCell* tmpPtr : mMeshCells)
   {
      GetCellNeighbors(tmpPtr);
      // tmp->mCellID = tmp->mCellNumber;
      InsertCellIntoGrid(tmpPtr);
   }

   ut::log::info() << "Loading NavMesh Cell Neighbors...";
   // now that the grid is populated, link the neighbors
   for (WsfNavigationCell* tmpPtr : mMeshCells)
   {
      LinkGridNeighbors(tmpPtr);
   }

   ut::log::info() << "Finished processing Navigation Mesh.";
}

bool WsfNavigationMesh::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "upper_left")
   {
      WsfGeoPoint geoPoint;
      geoPoint.ProcessInput(aInput);
      mUpperLeft = geoPoint;
   }
   else if (command == "lower_right")
   {
      WsfGeoPoint geoPoint;
      geoPoint.ProcessInput(aInput);
      mLowerRight = geoPoint;
   }
   else if (command == "grid_size")
   {
      aInput.ReadValueOfType(mGridSizeDegrees, UtInput::cANGLE);
      mGridSizeDegrees *= UtMath::cDEG_PER_RAD;
   }
   else if (command == "zone")
   {
      std::string zoneName;
      aInput.ReadValue(zoneName);

      // first read in the weight as a string so it will be possible to define "impassible" weights
      std::string sWeight;
      aInput.ReadValue(sWeight);

      double weight;

      if (sWeight == "NO_FLY_ZONE" || sWeight == "no_fly_zone")
      {
         weight = std::numeric_limits<double>::max();
      }
      else
      {
         weight = (double)atof(sWeight.c_str());
      }

      WsfZone* zonePtr = dynamic_cast<WsfZone*>(WsfScenario::FromInput(aInput).FindType("zone", zoneName));

      if (zonePtr != nullptr)
      {
         AddZone(zonePtr, weight);
      }
      else
      {
         throw UtInput::BadValue(aInput, "Zone not found: " + zoneName);
      }
   }
   else if (command == "output_file")
   {
      std::string sOutputFile;
      aInput.ReadValue(mOutputFile); // read in desired filename
      mLoadBinaryFile = false;
   }
   else if (command == "input_file")
   {
      std::string sOutputFile;
      aInput.ReadValue(mInputFile);
      mLoadBinaryFile = true;
   }
   else if (command == "think_time")
   {
      std::string sTime;
      aInput.ReadValue(sTime);

      double thinkTime;
      thinkTime = (double)atof(sTime.c_str());
      SetNavMeshThinkTime(thinkTime);
   }

   else if (command == "zone_set")
   {
      std::string zoneName;
      aInput.ReadValue(zoneName);
      // read in the weight as a string so it will be possible to define "impassable" weights
      std::string sWeight;
      aInput.ReadValue(sWeight);

      double weight;

      if (sWeight == "NO_FLY_ZONE" || sWeight == "no_fly_zone")
      {
         weight = std::numeric_limits<double>::max();
      }
      else
      {
         weight = (double)atof(sWeight.c_str());
      }

      WsfZoneSet* zonePtr = dynamic_cast<WsfZoneSet*>(WsfScenario::FromInput(aInput).FindType("zone", zoneName));

      std::unique_ptr<WsfZoneDefinition> boundingZonePtr(new WsfZoneDefinition());
      boundingZonePtr->SetShapeType(WsfZoneDefinition::cPOLYGONAL);
      std::vector<WsfGeoPoint> boundingPoints;
      WsfGeoPoint              upperRight(mLowerRight.GetLat(), mUpperLeft.GetLon(), 0);
      WsfGeoPoint              lowerRight(mLowerRight.GetLat(), mLowerRight.GetLon(), 0);
      WsfGeoPoint              lowerLeft(mUpperLeft.GetLat(), mLowerRight.GetLon(), 0);
      WsfGeoPoint              upperLeft(mUpperLeft.GetLat(), mUpperLeft.GetLon(), 0);
      boundingPoints.push_back(upperRight);
      boundingPoints.push_back(lowerRight);
      boundingPoints.push_back(lowerLeft);
      boundingPoints.push_back(upperLeft);
      boundingZonePtr->SetPolyPoints(boundingPoints);
      if (zonePtr != nullptr)
      {
         for (int i = 0; i < zonePtr->GetNumZones(); ++i)
         {
            WsfZoneDefinition*                              zoneDefPtr = (WsfZoneDefinition*)zonePtr->GetZoneAtIndex(i);
            std::vector<WsfZone::Point>                     zonePoints = zoneDefPtr->GetPoints();
            std::vector<WsfZoneDefinition::Point>::iterator pointItr;
            pointItr           = zonePoints.begin();
            bool withinBoundry = false;
            while (pointItr != zonePoints.end())
            {
               WsfZoneDefinition::Point* ptPtr    = &*pointItr;
               double                    pointLat = ptPtr->mX;
               double                    pointLon = ptPtr->mY;
               WsfGeoPoint               aGeoPt(pointLat, pointLon, 0);
               double                    locWCS[3] = {0};
               aGeoPt.GetLocationWCS(locWCS);
               if (boundingZonePtr->PointIsInside(GetSimulation(), locWCS, nullptr, 0))
               {
                  withinBoundry = true;
                  break;
               }
               ++pointItr;
            }
            if (withinBoundry)
            {
               mZones.push_back(zoneDefPtr);
               mZoneWeights[zoneDefPtr] = weight;
            }
         }
      }
      else
      {
         throw UtInput::BadValue(aInput, "Zone not found: " + zoneName);
      }
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

WsfGeoPoint WsfNavigationMesh::GetRandomLocation()
{
   // grab a random cell
   size_t             randomDraw  = GetSimulation()->GetRandom().Uniform<size_t>(0, mMeshCells.size() - 1);
   WsfNavigationCell* randCellPtr = mMeshCells[randomDraw];
   // pick a random vert on the cell
   randomDraw = GetSimulation()->GetRandom().Uniform<size_t>(0, randCellPtr->mVerts.size());
   double posLLA[3];
   if (randomDraw == randCellPtr->mVerts.size())
   {
      randCellPtr->mCellCentroid.Get(posLLA);
   }
   else
   {
      WsfGeoPoint vertPoint = randCellPtr->mVerts[randomDraw];
      vertPoint.GetLocationLLA(posLLA[0], posLLA[1], posLLA[2]);
   }

   return WsfGeoPoint(posLLA[0], posLLA[1], posLLA[2]);
}

WsfGeoPoint* WsfNavigationMesh::FindClosestValidPoint(double aSimTime, const WsfGeoPoint& aGeoPoint)
{
   if (GetCellForPoint(aGeoPoint))
   {
      WsfGeoPoint* retPointPtr = new WsfGeoPoint(aGeoPoint.GetLat(), aGeoPoint.GetLon(), aGeoPoint.GetAlt());
      return retPointPtr;
   }
   else
   {
      double closestDist                = std::numeric_limits<double>::max();
      mMeshCellsIterator                = mMeshCells.begin();
      WsfNavigationCell* closestCellPtr = nullptr;
      while (mMeshCellsIterator != mMeshCells.end())
      {
         WsfNavigationCell* tmpCellPtr = *mMeshCellsIterator;
         double             distVec[3];
         double             startPt[3] = {aGeoPoint.GetLat(), aGeoPoint.GetLon(), aGeoPoint.GetAlt()};
         double             endPt[3];
         tmpCellPtr->mCellCentroid.Get(endPt);
         UtVec3d::Subtract(distVec, endPt, startPt);
         double dist = UtVec3d::MagnitudeSquared(distVec);
         if (dist < closestDist)
         {
            closestDist    = dist;
            closestCellPtr = tmpCellPtr;
         }
         ++mMeshCellsIterator;
      }

      if (closestCellPtr != nullptr)
      {
         double closestLoc[3];
         closestCellPtr->mCellCentroid.Get(closestLoc);
         WsfGeoPoint* retPointPtr = new WsfGeoPoint(closestLoc[0], closestLoc[1], closestLoc[2]);
         return retPointPtr;
      }

      else
      {
         WsfGeoPoint* retPointPtr = new WsfGeoPoint(aGeoPoint.GetLat(), aGeoPoint.GetLon(), aGeoPoint.GetAlt());
         return retPointPtr;
      }
   }
}


void WsfNavigationMesh::DebugDrawMesh()
{
   wsf::Terrain* mTerrainPtr = new wsf::Terrain(GetSimulation()->GetTerrainInterface());
   GetSimulation()->GetTerrainInterface()->EnableTerrain();

   // draw each cell
   mDebugMeshDrawPtr->SetColor(1, 0, 0);
   mDebugMeshDrawPtr->SetLineSize(3);
   mDebugMeshDrawPtr->BeginLines();
   for (WsfNavigationCell* tmpPtr : mMeshCells)
   {
      tmpPtr->mSideIterator = tmpPtr->mSides.begin();
      while (tmpPtr->mSideIterator != tmpPtr->mSides.end())
      {
         UtLineSegment aSide = *tmpPtr->mSideIterator;
         double        aBeginPt[3];
         double        aEndPt[3];
         aSide.GetBeginPoint(aBeginPt);
         aSide.GetEndPoint(aEndPt);
         float tmpElevationBegin = 0.f;
         float tmpElevationEnd   = 0.f;
         mTerrainPtr->GetElevApprox(aBeginPt[0], aBeginPt[1], tmpElevationBegin);
         mTerrainPtr->GetElevApprox(aEndPt[0], aEndPt[1], tmpElevationEnd);
         mDebugMeshDrawPtr->VertexLLA(aBeginPt[0], aBeginPt[1], tmpElevationBegin + 100); // aBeginPt[2]);
         mDebugMeshDrawPtr->VertexLLA(aEndPt[0], aEndPt[1], tmpElevationEnd + 100);       // aEndPt[2]);
         ++tmpPtr->mSideIterator;
      }
   }
   mDebugMeshDrawPtr->End();

   // draw the neighbors
   mDebugMeshDrawPtr->SetColor(0, 1, 1);
   mDebugMeshDrawPtr->SetLineSize(3);
   mDebugMeshDrawPtr->BeginLines();
   for (WsfNavigationCell* tmpPtr : mMeshCells)
   {
      tmpPtr->mNeighborIterator = tmpPtr->mNeighbors.begin();
      while (tmpPtr->mNeighborIterator != tmpPtr->mNeighbors.end())
      {
         WsfNavigationCell* neighborPtr = *tmpPtr->mNeighborIterator;
         double             centroid[3];
         double             neighborCentroid[3];

         centroid[0] = tmpPtr->mCellCentroid.Get(0);
         centroid[1] = tmpPtr->mCellCentroid.Get(1);
         centroid[2] = tmpPtr->mCellCentroid.Get(2);

         neighborCentroid[0] = neighborPtr->mCellCentroid.Get(0);
         neighborCentroid[1] = neighborPtr->mCellCentroid.Get(1);
         neighborCentroid[2] = neighborPtr->mCellCentroid.Get(2);

         float tmpElevationBegin = 0.f;
         float tmpElevationEnd   = 0.f;
         mTerrainPtr->GetElevApprox(centroid[0], centroid[1], tmpElevationBegin);
         mTerrainPtr->GetElevApprox(neighborCentroid[0], neighborCentroid[1], tmpElevationEnd);
         mDebugMeshDrawPtr->VertexLLA(centroid[0], centroid[1], tmpElevationBegin + 100);               // centroid[2]);
         mDebugMeshDrawPtr->VertexLLA(neighborCentroid[0], neighborCentroid[1], tmpElevationEnd + 100); // neighborCentroid[2]);
         ++tmpPtr->mNeighborIterator;
      }
   }
   mDebugMeshDrawPtr->End();

   delete mTerrainPtr;
}


void WsfNavigationMesh::DebugDrawZones()
{
   DebugDrawMesh();
   std::list<WsfZone*>::const_iterator i;
   mDebugDrawObjPtr->SetColor(1.0, 1.0, 1.0);
   mDebugDrawObjPtr->SetLineSize(3);
   for (i = mZones.begin(); i != mZones.end(); ++i)
   {
      WsfZoneDefinition*                              zonePtr    = (WsfZoneDefinition*)*i;
      std::vector<WsfZone::Point>                     zonePoints = zonePtr->GetPoints();
      std::vector<WsfZoneDefinition::Point>::iterator pointItr;
      pointItr                               = zonePoints.begin();
      WsfZoneDefinition::Point firstPt       = *pointItr;
      double                   firstPoint[3] = {firstPt.mX, firstPt.mY, 0};
      mDebugDrawObjPtr->BeginPolyline();
      while (pointItr != zonePoints.end())
      {
         WsfZoneDefinition::Point pt       = *pointItr;
         double                   point[3] = {pt.mX, pt.mY, 0};
         mDebugDrawObjPtr->VertexLLA(point[0], point[1], point[2]);
         ++pointItr;
      }

      mDebugDrawObjPtr->VertexLLA(firstPoint[0], firstPoint[1], firstPoint[2]);
      mDebugDrawObjPtr->End();
   }
}

const char* WsfNavigationMesh::GetScriptClassName() const
{
   return "WsfNavigationMesh";
}

/************************************************************************/
/* Script Routines                                                      */
/************************************************************************/

WsfScriptNavigationMeshClass::WsfScriptNavigationMeshClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptObjectClass(aClassName, aScriptTypesPtr)
{
   AddClassName("WsfNavigationMesh");

   AddMethod(ut::make_unique<DebugDrawMesh>()); // NO_DOC | FOR_TEST_ONLY
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptNavigationMeshClass, WsfNavigationMesh, DebugDrawMesh, 0, "void", "") // NO_DOC | FOR_TEST_ONLY
{
   aObjectPtr->DebugDrawMesh();
}
