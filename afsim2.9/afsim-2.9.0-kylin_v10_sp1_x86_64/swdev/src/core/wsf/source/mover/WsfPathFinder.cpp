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

//* PURPOSE:
//*   This class is used to find a path from A to B in a 2D environment with
//*   weighted nodes.  It uses the UtGraph class to build a set of nodes that
//*   hold the weights for each square in a lat/lon grid.  Squares are measured
//*   in arc degrees rather than meters because the class needs to work on a
//*   spherical earth.
//*
//* AUTHOR
//*   Michael Williams
//*
//* NOTES:
//*   None
//***************************************************************************

#include "WsfPathFinder.hpp"

#include <cassert>
#include <cmath>
#include <cstdio>
#include <iomanip>
#include <memory>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtRandom.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfMover.hpp"
#include "WsfNavigationMesh.hpp"
#include "WsfObject.hpp"
#include "WsfObjectTypeList.hpp"
#include "WsfPath.hpp"
#include "WsfPlatform.hpp"
#include "WsfRoute.hpp"
#include "WsfRouteMover.hpp"
#include "WsfRouteTypes.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfTerrainPathFinder.hpp"
#include "WsfZone.hpp"
#include "WsfZoneDefinition.hpp"
#include "WsfZoneSet.hpp"
#include "WsfZoneTypes.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptManager.hpp"


WsfPathFinder::WsfPathFinder(WsfScenario* aScenarioPtr, WsfGeoPoint& aUpperLeft, WsfGeoPoint& aLowerRight, double aGridSizeDegrees)
   : mScenarioPtr(aScenarioPtr)
   , mSimulationPtr(nullptr)
   , mZones()
   , mZoneWeights()
   , mUpperLeft(aUpperLeft)
   , mLowerRight(aLowerRight)
   , mGridSizeDegrees(aGridSizeDegrees)
   , mXSize(0)
   , mYSize(0)
   , mGrid()
   , mInitialized(false)
   , mDebugDrawObjPtr(nullptr)
   , mDebugEnabled(false)
{
   mLowerRight.SetLocationLLA(0, 0, 0);
   mUpperLeft.SetLocationLLA(0, 0, 0);
}

WsfPathFinder::WsfPathFinder(WsfScenario* aScenarioPtr)
   : mScenarioPtr(aScenarioPtr)
   , mSimulationPtr(nullptr)
   , mZones()
   , mZoneWeights()
   , mUpperLeft(0, 0, 0)
   , mLowerRight(0, 0, 0)
   , mGridSizeDegrees(0)
   , mXSize(0)
   , mYSize(0)
   , mGrid()
   , mInitialized(false)
   , mDebugDrawObjPtr(nullptr)
   , mDebugEnabled(false)
{
   mLowerRight.SetLocationLLA(0, 0, 0);
   mUpperLeft.SetLocationLLA(0, 0, 0);
}

bool WsfPathFinder::Initialize(WsfSimulation* aSimulationPtr)
{
   mDebugDrawObjPtr = new WsfDraw(*aSimulationPtr);
   mInitialized     = false;
   // Make sure we have the inputs we need to build the underlying grid
   assert(mGridSizeDegrees != 0);
   mXSize = (long)ceil((mLowerRight.GetLon() - mUpperLeft.GetLon()) / mGridSizeDegrees);
   mYSize = (long)ceil((mUpperLeft.GetLat() - mLowerRight.GetLat()) / mGridSizeDegrees);

   mGrid = std::vector<node_iterator>(mXSize * mYSize);

   // Now build the graph that sits under the grid and that we'll use for path finding
   WsfPFNode*  tempNodePtr;
   WsfGeoPoint tempPoint;
   int         i;
   for (i = 0; i < mXSize; ++i)
   {
      for (int j = 0; j < mYSize; ++j)
      {
         tempPoint   = WsfGeoPoint(mUpperLeft.GetLat() - ((double)j + 0.5) * mGridSizeDegrees,
                                 mUpperLeft.GetLon() + ((double)i + 0.5) * mGridSizeDegrees,
                                 0);
         tempNodePtr = new WsfPFNode(i, j, 1, tempPoint);
         insert(*tempNodePtr);
         SetGrid(i, j, find(*tempNodePtr));
      }
   }

   // Add edges connecting adjacent nodes in the graph
   for (i = 0; i < mXSize; ++i)
   {
      for (int j = 0; j < mYSize; ++j)
      {
         if (i < mXSize - 1)
         {
            WsfPFNode  aNode      = (WsfPFNode&)*GetGrid(i, j);
            WsfPFNode* tmpNodePtr = &aNode;

            WsfPFNode  aNode2      = (WsfPFNode&)*GetGrid(i + 1, j);
            WsfPFNode* tmpNode2Ptr = &aNode2;
            WsfPFEdge  tempEdge(tmpNodePtr, tmpNode2Ptr);
            insert_edge(GetGrid(i, j), GetGrid(i + 1, j), tempEdge);
         }
         if (j < mYSize - 1)
         {
            WsfPFNode  aNode      = (WsfPFNode&)*GetGrid(i, j);
            WsfPFNode* tmpNodePtr = &aNode;

            WsfPFNode  aNode2      = (WsfPFNode&)*GetGrid(i, j + 1);
            WsfPFNode* tmpNode2Ptr = &aNode2;
            WsfPFEdge  tempEdge(tmpNodePtr, tmpNode2Ptr);
            // WsfPFEdge tempEdge((WsfPFNode*)&GetGrid(i, j), (WsfPFNode*)&GetGrid(i, j+1));
            insert_edge(GetGrid(i, j), GetGrid(i, j + 1), tempEdge);
         }
         if (i < mXSize - 1 && j < mYSize - 1)
         {
            WsfPFNode  aNode      = (WsfPFNode&)*GetGrid(i, j);
            WsfPFNode* tmpNodePtr = &aNode;

            WsfPFNode  aNode2      = (WsfPFNode&)*GetGrid(i + 1, j + 1);
            WsfPFNode* tmpNode2Ptr = &aNode2;
            WsfPFEdge  tempEdge(tmpNodePtr, tmpNode2Ptr);
            insert_edge(GetGrid(i, j), GetGrid(i + 1, j + 1), tempEdge);
         }
         if (j <= mYSize - 1 && j > 0 && i < mXSize - 1)
         {
            WsfPFNode  aNode      = (WsfPFNode&)*GetGrid(i, j);
            WsfPFNode* tmpNodePtr = &aNode;

            WsfPFNode  aNode2      = (WsfPFNode&)*GetGrid(i + 1, j - 1);
            WsfPFNode* tmpNode2Ptr = &aNode2;
            WsfPFEdge  tempEdge(tmpNodePtr, tmpNode2Ptr);
            insert_edge(GetGrid(i, j), GetGrid(i + 1, j - 1), tempEdge);
         }
      }
   }


   mInitialized = true;
   //   mEnabled = true;
   RecalculateWeights();

   return true;
}

WsfPathFinder::~WsfPathFinder()
{
   // delete the nodes and edges
   clear();
}

bool WsfPathFinder::PointIsInGridBounds(int x, int y)
{
   if (x < 0 || y < 0 || x >= mXSize || y >= mYSize)
   {
      return false;
   }
   else
   {
      return true;
   }
}


void WsfPathFinder::AddZone(WsfZone* aZonePtr, double aWeight)
{
   // prevent duplicates
   if (!ContainsZone(aZonePtr))
   {
      mZones.push_back(aZonePtr);
      mZoneWeights[aZonePtr] = aWeight;
   }
}

void WsfPathFinder::RemoveZone(WsfZone* aZonePtr)
{
   mZones.remove(aZonePtr);
   mZoneWeights.erase(aZonePtr);
}

bool WsfPathFinder::ContainsZone(WsfZone* aZonePtr)
{
   std::list<WsfZone*>::const_iterator it;
   it = std::find(mZones.begin(), mZones.end(), aZonePtr);
   if (it != mZones.end())
   {
      return true;
   }
   return false;
}

bool WsfPathFinder::ContainsZone(const std::string& aZoneName)
{
   std::list<WsfZone*>::const_iterator i;
   for (i = mZones.begin(); i != mZones.end(); ++i)
   {
      if ((*i)->GetName() == aZoneName)
      {
         return true;
      }
   }
   return false;
}

WsfZone* WsfPathFinder::GetZoneByName(const std::string& aZoneName)
{
   std::list<WsfZone*>::const_iterator i;
   for (i = mZones.begin(); i != mZones.end(); ++i)
   {
      if ((*i)->GetName() == aZoneName)
      {
         return *i;
      }
   }
   return nullptr;
}

void WsfPathFinder::RecalculateWeights()
{
   assert(mInitialized);

   node_iterator                       nodeIter;
   std::list<WsfZone*>::const_iterator i;
   double                              locWCS[3] = {0};

   // We could probably do this more efficiently by storing on each node a list of zones
   // it is contained by.  This function probably won't be called often, though,
   // so I think it's better not to waste the memory.
   for (nodeIter = begin(); nodeIter != end(); ++nodeIter)
   {
      if (nodeIter->first.mWeight == std::numeric_limits<double>::max())
      {
         continue;
      }
      nodeIter->first.mWeight = nodeIter->first.mBaseWeight;

      for (i = mZones.begin(); i != mZones.end(); ++i)
      {
         nodeIter->first.mLoc.GetLocationWCS(locWCS);
         if ((*i)->PointIsInside(GetSimulation(), locWCS, nullptr, 0))
         {
            if (nodeIter->first.mWeight != std::numeric_limits<double>::max())
            {
               nodeIter->first.mWeight += mZoneWeights[(*i)];
            }
         }
      }
   }
}

const WsfPFNode* WsfPathFinder::GetClosestNode(const WsfGeoPoint& aPointPtr)
{
   assert(mInitialized);

   double lat = aPointPtr.GetLat();
   double lon = aPointPtr.GetLon();

   // if aPointPtr is outside our region, then return null
   if (lat > mUpperLeft.GetLat() || lat < mLowerRight.GetLat() || lon > mLowerRight.GetLon() || lon < mUpperLeft.GetLon())
   {
      return nullptr;
   }

   lat   = mUpperLeft.GetLat() - lat;
   lon   = lon - mUpperLeft.GetLon();
   int x = (int)(lon / mGridSizeDegrees);
   int y = (int)(lat / mGridSizeDegrees);

   const WsfPFNode* tmpNodePtr = &(GetGrid(x, y)->first);
   return tmpNodePtr;
}

bool WsfPathFinder::FindPath(const WsfGeoPoint& aStartPtr, WsfGeoPoint& aEndPtr, WsfRoute& aRoute, double& aCost)
{
   assert(mInitialized);

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

   if (path.empty())
   {
      return false;
   }

   double lla[3];
   double ullla[3];                                         // upper left lla
   aStartPtr.GetLocationLLA(lla[0], lla[1], lla[2]);        // get the altitude passed in at the start of the path.
   mUpperLeft.GetLocationLLA(ullla[0], ullla[1], ullla[2]); // get the reference altitude of the upper left part of the
                                                            // pathfinder grid

   // The pathfinder doesn't care about Altitude, only Lat, Lon.  It tends to trash the Altitude data, thus we
   // preserve the altitude based on either the starting point passed in, or using the script defined upper-left corner's altitude...
   double aRefAlt = std::max(ullla[2], lla[2]);


   for (unsigned int i = 0; i < path.size() - 1; ++i)
   {
      const WsfGeoPoint& pathLoc = path[i].mLoc;

      aRoute.Append(WsfWaypoint(pathLoc.GetLat(), pathLoc.GetLon(), aRefAlt, WsfPath::cDOUBLE_NOT_SET));
      aRoute[i].SetNodeIdNumber(path[i].mX + mXSize * path[i].mY); // NAC Added NodeID assignment
   }
   aEndPtr.SetLocationLLA(aEndPtr.GetLat(), aEndPtr.GetLon(), aRefAlt);
   aRoute.Append(WsfWaypoint(aEndPtr.GetLat(), aEndPtr.GetLon(), aEndPtr.GetAlt(), WsfPath::cDOUBLE_NOT_SET));

   return true;
}

void WsfPathFinder::print() const
{
   assert(mInitialized);

   auto                out = ut::log::info() << "Path Finder Data Dump:";
   const_node_iterator nodeIter;
   for (nodeIter = begin(); nodeIter != end(); ++nodeIter)
   {
      const auto& nodeRef = nodeIter->first;

      auto nodeNote = out.AddNote() << "Node:";
      nodeNote.AddNote() << "X: " << nodeRef.mX;
      nodeNote.AddNote() << "Y: " << nodeRef.mY;
      nodeNote.AddNote() << "Lat: " << nodeRef.mLoc.GetLat();
      nodeNote.AddNote() << "Lon: " << nodeRef.mLoc.GetLon();
      nodeNote.AddNote() << "Weight: " << nodeRef.mWeight;
      for (const_edge_iterator edgeIter = nodeIter->second->begin(); edgeIter != nodeIter->second->end(); ++edgeIter)
      {
         auto edgeNote = nodeNote.AddNote() << "Edge:";
         edgeNote.AddNote() << "From X: " << edgeIter.from()->first.mX;
         edgeNote.AddNote() << "From Y: " << edgeIter.from()->first.mY;
         edgeNote.AddNote() << "To X: " << edgeIter.to()->first.mX;
         edgeNote.AddNote() << "To Y: " << edgeIter.to()->first.mY;
      }
   }
}

bool WsfPathFinder::shortest_path(const_node_iterator aSrcNodeIter,
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
   return PFGraph::shortest_path(aSrcNodeIter, aDstNodeIter, aPath, aCost, aCostFuncPtr);
}

WsfPathFinder::node_iterator WsfPathFinder::GetGrid(long aX, long aY)
{
   return mGrid.at(aX + mXSize * aY);
}

WsfPathFinder::node_iterator WsfPathFinder::GetGridAbsolute(unsigned long idx)
{
   return mGrid.at(idx);
}

WsfGeoPoint WsfPathFinder::GetNodeLocation(int nodeIndex)
{
   node_iterator nodeItr = GetGridAbsolute(nodeIndex);
   WsfGeoPoint   geoPt;

   geoPt.SetLocationLLA(nodeItr->first.mLoc.GetLat(), nodeItr->first.mLoc.GetLon(), nodeItr->first.mLoc.GetAlt());
   return geoPt;
}

bool WsfPathFinder::GetConsiderNode(const WsfGeoPoint& aGeoPoint)
{
   const WsfPFNode tmpNode = (WsfPFNode&)*this->GetClosestNode(aGeoPoint);
   cost_func*      aCostFuncPtr;
   pf_cost_func    costFunc;
   aCostFuncPtr = reinterpret_cast<cost_func*>(&costFunc);
   return aCostFuncPtr->consider_node(tmpNode);
}

bool WsfPathFinder::GetConsiderNode(const WsfPFNode& aNode)
{
   cost_func*   aCostFuncPtr;
   pf_cost_func costFunc;
   aCostFuncPtr = reinterpret_cast<cost_func*>(&costFunc);
   return aCostFuncPtr->consider_node(aNode);
}

void WsfPathFinder::SetGrid(unsigned long aX, unsigned long aY, node_iterator aIter)
{
   mGrid.at(aX + mXSize * aY) = aIter;
}

void WsfPathFinder::SetGridAbsolute(unsigned long aX, node_iterator aIter)
{
   mGrid.at(aX) = aIter;
}


bool WsfPathFinder::ProcessInput(UtInput& aInput)
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

      // first read in the weight as a string so it will be possible to define "unpassible" weights
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

      WsfZone* zonePtr = WsfZoneTypes::Get(WsfScenario::FromInput(aInput)).Find(zoneName);

      if (zonePtr != nullptr)
      {
         AddZone(zonePtr, weight);
      }
      else
      {
         throw UtInput::BadValue(aInput, "Zone not found: " + zoneName);
      }
   }
   else if (command == "zone_set")
   {
      std::string zoneName;
      aInput.ReadValue(zoneName);
      // read in the weight as a string so it will be possible to define "unpassable" weights
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
            std::vector<WsfZoneDefinition::Point>::iterator pointItr;
            std::vector<WsfZone::Point>                     zonePoints = zoneDefPtr->GetPoints();
            pointItr                                                   = zonePoints.begin();
            bool withinBoundry                                         = false;
            while (pointItr != zonePoints.end())
            {
               WsfZoneDefinition::Point* aPtPtr   = &*pointItr;
               double                    pointLat = aPtPtr->mX;
               double                    pointLon = aPtPtr->mY;
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

int WsfPathFinder::GetNumberOfZones()
{
   return static_cast<int>(mZones.size());
}

bool WsfPathFinder::IsDuplicateEdge(node_iterator nodeIter, const WsfPFNode& aNode2, const WsfPFEdge& tmpEdge)
{
   for (const_edge_iterator edgeIter = nodeIter->second->begin(); edgeIter != nodeIter->second->end(); ++edgeIter)
   {
      if (edgeIter.from()->first.mX == nodeIter->first.mX && edgeIter.from()->first.mY == nodeIter->first.mY)
      {
         if (edgeIter.to()->first.mX == aNode2.mX && edgeIter.to()->first.mY == aNode2.mY)
         {
            return true;
         }
      }
      else if (edgeIter.to()->first.mX == nodeIter->first.mX && edgeIter.to()->first.mY == nodeIter->first.mY)
      {
         if (edgeIter.from()->first.mX == aNode2.mX && edgeIter.from()->first.mY == aNode2.mY)
         {
            return true;
         }
      }
   }
   return false;
}

WsfGeoPoint* WsfPathFinder::FindClosestPointInQuadrant(const WsfGeoPoint& aGeoPoint, int aQuadrant)
{
   std::vector<WsfPFNode> borderNodes;
   std::vector<WsfPFNode> closestBorderNodes;

   unsigned int closestBorderSize = GetXSize();
   double       closestDistance   = std::numeric_limits<double>::max();
   WsfGeoPoint  emptyPoint(0, 0, 0);
   WsfPFNode    closestNode(0, 0, 0, emptyPoint);

   int    xSize    = GetXSize();
   int    ySize    = GetYSize();
   double distance = 0.0;

   if (aQuadrant == 0)
   {
      for (int x = 0; x <= xSize / 2; ++x)
      {
         WsfPFNode aNode = (WsfPFNode&)*GetGrid(x, 0);
         if (aNode.mWeight < std::numeric_limits<double>::max())
         {
            borderNodes.push_back(aNode);
            distance = aGeoPoint.GetDistanceFrom(aNode.mLoc);
            if (distance < closestDistance)
            {
               closestNode     = aNode;
               closestDistance = distance;
               closestBorderNodes.insert(closestBorderNodes.begin(), 1, aNode);
               if (closestBorderNodes.size() > closestBorderSize)
               {
                  closestBorderNodes.pop_back();
               }
            }
         }
      }

      for (int y = 0; y <= ySize / 2; ++y)
      {
         WsfPFNode aNode = (WsfPFNode&)*GetGrid(0, y);
         if (aNode.mWeight < std::numeric_limits<double>::max())
         {
            borderNodes.push_back(aNode);
            distance = aGeoPoint.GetDistanceFrom(aNode.mLoc);
            if (distance < closestDistance)
            {
               closestNode     = aNode;
               closestDistance = distance;
               closestBorderNodes.insert(closestBorderNodes.begin(), 1, aNode);
               if (closestBorderNodes.size() > closestBorderSize)
               {
                  closestBorderNodes.pop_back();
               }
            }
         }
      }
   }

   // get the closest node in the northwest quadrant
   else if (aQuadrant == 1)
   {
      for (int x = 0; x <= xSize / 2; ++x)
      {
         WsfPFNode aNode = (WsfPFNode&)*GetGrid(x, ySize - 1);
         if (aNode.mWeight < std::numeric_limits<double>::max())
         {
            borderNodes.push_back(aNode);
            distance = aGeoPoint.GetDistanceFrom(aNode.mLoc);
            if (distance < closestDistance)
            {
               closestNode     = aNode;
               closestDistance = distance;
               closestBorderNodes.insert(closestBorderNodes.begin(), 1, aNode);
               if (closestBorderNodes.size() > closestBorderSize)
               {
                  closestBorderNodes.pop_back();
               }
            }
         }
      }

      for (int y = ySize / 2; y < ySize; ++y)
      {
         WsfPFNode aNode = (WsfPFNode&)*GetGrid(0, y);
         if (aNode.mWeight < std::numeric_limits<double>::max())
         {
            borderNodes.push_back(aNode);
            distance = aGeoPoint.GetDistanceFrom(aNode.mLoc);
            if (distance < closestDistance)
            {
               closestNode     = aNode;
               closestDistance = distance;
               closestBorderNodes.insert(closestBorderNodes.begin(), 1, aNode);
               if (closestBorderNodes.size() > closestBorderSize)
               {
                  closestBorderNodes.pop_back();
               }
            }
         }
      }
   }

   // get the closest node in the northeast quadrant
   else if (aQuadrant == 2)
   {
      for (int x = xSize / 2; x < xSize; ++x)
      {
         WsfPFNode aNode = (WsfPFNode&)*GetGrid(x, ySize - 1);
         if (aNode.mWeight < std::numeric_limits<double>::max())
         {
            borderNodes.push_back(aNode);
            distance = aGeoPoint.GetDistanceFrom(aNode.mLoc);
            if (distance < closestDistance)
            {
               closestNode     = aNode;
               closestDistance = distance;
               closestBorderNodes.insert(closestBorderNodes.begin(), 1, aNode);
               if (closestBorderNodes.size() > closestBorderSize)
               {
                  closestBorderNodes.pop_back();
               }
            }
         }
      }

      for (int y = ySize / 2; y < ySize; ++y)
      {
         WsfPFNode aNode = (WsfPFNode&)*GetGrid(xSize - 1, y);
         if (aNode.mWeight < std::numeric_limits<double>::max())
         {
            borderNodes.push_back(aNode);
            distance = aGeoPoint.GetDistanceFrom(aNode.mLoc);
            if (distance < closestDistance)
            {
               closestNode     = aNode;
               closestDistance = distance;
               closestBorderNodes.insert(closestBorderNodes.begin(), 1, aNode);
               if (closestBorderNodes.size() > closestBorderSize)
               {
                  closestBorderNodes.pop_back();
               }
            }
         }
      }
   }

   // get the closest node in the southeast quadrant
   else if (aQuadrant == 3)
   {
      for (int x = xSize / 2; x < xSize; ++x)
      {
         WsfPFNode aNode = (WsfPFNode&)*GetGrid(x, 0);
         if (aNode.mWeight < std::numeric_limits<double>::max())
         {
            borderNodes.push_back(aNode);
            distance = aGeoPoint.GetDistanceFrom(aNode.mLoc);
            if (distance < closestDistance)
            {
               closestNode     = aNode;
               closestDistance = distance;
               closestBorderNodes.insert(closestBorderNodes.begin(), 1, aNode);
               if (closestBorderNodes.size() > closestBorderSize)
               {
                  closestBorderNodes.pop_back();
               }
            }
         }
      }

      for (int y = 0; y < ySize / 2; ++y)
      {
         WsfPFNode aNode = (WsfPFNode&)*GetGrid(xSize - 1, y);
         if (aNode.mWeight < std::numeric_limits<double>::max())
         {
            borderNodes.push_back(aNode);
            distance = aGeoPoint.GetDistanceFrom(aNode.mLoc);
            if (distance < closestDistance)
            {
               closestNode     = aNode;
               closestDistance = distance;
               closestBorderNodes.insert(closestBorderNodes.begin(), 1, aNode);
               if (closestBorderNodes.size() > closestBorderSize)
               {
                  closestBorderNodes.pop_back();
               }
            }
         }
      }
   }

   // at this point the platform should know its closest escape point, path to it
   // get the geopoints of the first and last position...

   WsfRoute    temp;
   double      cost = 0.0;
   double      endPt[3];
   WsfGeoPoint aEndPoint(closestNode.mLoc.GetLat(), closestNode.mLoc.GetLon(), closestNode.mLoc.GetAlt());
   bool        pathFound =
      FindPath(WsfGeoPoint(aGeoPoint.GetLat(), aGeoPoint.GetLon(), aGeoPoint.GetAlt()), aEndPoint, temp, cost);

   if (pathFound)
   {
      endPt[0] = temp.GetWaypointAt(temp.GetSize() - 1).GetLat();
      endPt[1] = temp.GetWaypointAt(temp.GetSize() - 1).GetLon();
      endPt[2] = temp.GetWaypointAt(temp.GetSize() - 1).GetAlt();
      return new WsfGeoPoint(endPt[0], endPt[1], endPt[2]);
   }

   // if it was not possible to path to the closest border node, find the next closest node
   for (unsigned int i = 0; i < closestBorderNodes.size(); ++i)
   {
      WsfPFNode tmpNode = closestBorderNodes[i];
      bool      checkPathFound =
         FindPath(WsfGeoPoint(aGeoPoint.GetLat(), aGeoPoint.GetLon(), aGeoPoint.GetAlt()), aEndPoint, temp, cost);
      if (checkPathFound)
      {
         endPt[0] = temp.GetWaypointAt(temp.GetSize() - 1).GetLat();
         endPt[1] = temp.GetWaypointAt(temp.GetSize() - 1).GetLon();
         endPt[2] = temp.GetWaypointAt(temp.GetSize() - 1).GetAlt();
         return new WsfGeoPoint(endPt[0], endPt[1], endPt[2]);
      }
   }
   return new WsfGeoPoint(aGeoPoint.GetLat(), aGeoPoint.GetLon(), aGeoPoint.GetAlt());
}

WsfGeoPoint* WsfPathFinder::FindClosestPointOnEdge(double aSimTime, const WsfGeoPoint& aGeoPoint)
{
   // figure out which quadrant of the pathing grid the platform is in
   int quadrant = 0;

   int       xSize             = GetXSize();
   int       ySize             = GetYSize();
   WsfPFNode northWestCentroid = (WsfPFNode&)*GetGrid((long)(0.25f * xSize), (long)(0.75f * ySize));
   WsfPFNode northEastCentroid = (WsfPFNode&)*GetGrid((long)(0.75f * xSize), (long)(0.75f * ySize));
   WsfPFNode southWestCentroid = (WsfPFNode&)*GetGrid((long)(0.25f * xSize), (long)(0.25f * ySize));
   WsfPFNode southEastCentroid = (WsfPFNode&)*GetGrid((long)(0.75f * xSize), (long)(0.25f * ySize));

   double quads[4];

   // compute the distance from the starting position to each quadrant centroid

   quads[0] = aGeoPoint.GetDistanceFrom(northWestCentroid.mLoc);
   quads[1] = aGeoPoint.GetDistanceFrom(northEastCentroid.mLoc);
   quads[2] = aGeoPoint.GetDistanceFrom(southWestCentroid.mLoc);
   quads[3] = aGeoPoint.GetDistanceFrom(southEastCentroid.mLoc);

   double distance = std::numeric_limits<double>::max();
   for (int i = 0; i < 4; ++i)
   {
      if (quads[i] < distance)
      {
         distance = quads[i];
         quadrant = i;
      }
   }
   quads[quadrant] = std::numeric_limits<double>::max();

   distance               = std::numeric_limits<double>::max();
   WsfGeoPoint* aPointPtr = FindClosestPointInQuadrant(aGeoPoint, quadrant);
   int          searches  = 1;
   while ((fabs(aPointPtr->GetLat() - aGeoPoint.GetLat()) <= std::numeric_limits<double>::epsilon()) &&
          (fabs(aPointPtr->GetLon() - aGeoPoint.GetLon()) <= std::numeric_limits<double>::epsilon()))
   {
      for (int i = 0; i < 4; ++i)
      {
         if (quads[i] < distance)
         {
            distance = quads[i];
            quadrant = i;
         }
      }
      quads[quadrant] = std::numeric_limits<double>::max();
      distance        = std::numeric_limits<double>::max();
      aPointPtr       = FindClosestPointInQuadrant(aGeoPoint, quadrant);
      searches++;
      if (searches > 4)
      {
         break;
      }
   }
   return aPointPtr;
}

//
// FindClosestValidPoint
//
//    - returns the nearest point on the grid that is not inside a zone (a pathable node)
//
WsfGeoPoint* WsfPathFinder::FindClosestValidPoint(double aSimTime, const WsfGeoPoint& aGeoPoint)
{
   WsfGeoPoint* resultPtr = new WsfGeoPoint(0, 0, 0);
   resultPtr->SetLocationLLA(aGeoPoint.GetLat(), aGeoPoint.GetLon(), aGeoPoint.GetAlt());

   double aGeoPointWCS[3] = {0};
   aGeoPoint.GetLocationWCS(aGeoPointWCS);

   WsfZoneDefinition* encroachedZonePtr = nullptr;

   // determine if the given point is inside a zone
   for (std::list<WsfZone*>::const_iterator It = mZones.begin(); It != mZones.end(); ++It)
   {
      WsfZoneDefinition* currZonePtr = (WsfZoneDefinition*)*It;
      if (currZonePtr->PointIsInside(GetSimulation(), aGeoPointWCS, aGeoPointWCS, 0))
      {
         encroachedZonePtr = currZonePtr;
         break;
      }
   }

   // if the given point is inside a zone, find the nearest vertex and find a valid point near that.
   if (encroachedZonePtr != nullptr)
   {
      // find the nearest vertex of the zone
      double nearestVertexWCS[3] = {0};
      double testVertexWCS[3]    = {0};

      std::vector<WsfZone::Point>                           zonePoints = encroachedZonePtr->GetPoints();
      std::vector<WsfZoneDefinition::Point>::const_iterator PointIt    = zonePoints.begin();
      UtEntity::ConvertLLAToWCS(PointIt->mX, PointIt->mY, 0, nearestVertexWCS);

      double distance    = aGeoPoint.GetDistanceFrom(testVertexWCS);
      double minDistance = distance;

      for (++PointIt; PointIt != zonePoints.end(); ++PointIt)
      {
         UtEntity::ConvertLLAToWCS(PointIt->mX, PointIt->mY, 0, testVertexWCS);

         distance = aGeoPoint.GetDistanceFrom(testVertexWCS);
         if (distance < minDistance)
         {
            minDistance         = distance;
            nearestVertexWCS[0] = testVertexWCS[0];
            nearestVertexWCS[1] = testVertexWCS[1];
            nearestVertexWCS[2] = testVertexWCS[2];
         }
      }

      const WsfPFNode* nearestNodePtr = GetClosestNode(WsfGeoPoint(nearestVertexWCS));
      if (nearestNodePtr != nullptr)
      {
         double nearestNodeWCS[3] = {0};
         nearestNodePtr->mLoc.GetLocationWCS(nearestNodeWCS);

         if (DebugEnabled())
         {
            // draw the nearest node
            mDebugDrawObjPtr->SetPointSize(10);
            mDebugDrawObjPtr->SetColor(0.0, 0.0, 1.0);
            mDebugDrawObjPtr->BeginPoints();
            mDebugDrawObjPtr->VertexWCS(nearestNodeWCS);
            mDebugDrawObjPtr->End();
         }

         if (encroachedZonePtr->PointIsInside(GetSimulation(), nearestNodeWCS, nearestNodeWCS, 0))
         {
            // iterate over the "edges" of this node to find one that is outside of the zone.
            double              nearestEdgeWCS[3] = {0};
            const_node_iterator nodeIt            = GetGrid(nearestNodePtr->mX, nearestNodePtr->mY);
            for (const_edge_iterator edgeIt = nodeIt->second->begin(); edgeIt != nodeIt->second->end(); ++edgeIt)
            {
               const WsfPFNode*   dstNodePtr      = &edgeIt.from()->first;
               const WsfGeoPoint* dstNodePointPtr = &dstNodePtr->mLoc;

               dstNodePointPtr->GetLocationWCS(nearestEdgeWCS);

               if (DebugEnabled())
               {
                  // draw the node we have checked
                  mDebugDrawObjPtr->SetPointSize(10);
                  mDebugDrawObjPtr->SetColor(0.0, 1.0, 1.0);
                  mDebugDrawObjPtr->BeginPoints();
                  mDebugDrawObjPtr->VertexWCS(nearestEdgeWCS);
                  mDebugDrawObjPtr->End();
               }

               if (!encroachedZonePtr->PointIsInside(GetSimulation(), nearestEdgeWCS, nearestEdgeWCS, 0))
               {
                  resultPtr->SetLocationWCS(nearestEdgeWCS);
                  return resultPtr;
               }
            }

            auto out = ut::log::error() << "FindClosestValidPoint() could not find a valid point near target.";
            out.AddNote() << "Target (WCS): " << aGeoPointWCS[0] << ":" << aGeoPointWCS[1];
         }
         else
         {
            resultPtr->SetLocationWCS(nearestNodeWCS);
         }
      }
      else
      {
         auto out = ut::log::error() << "FindClosestValidPoint() could not find a valid point near target.";
         out.AddNote() << "Target (WCS): " << aGeoPointWCS[0] << ":" << aGeoPointWCS[1];
      }
   }

   return resultPtr;
}

WsfGeoPoint WsfPathFinder::GetRandomLocation()
{
   long      randomX  = GetSimulation()->GetRandom().Uniform(0l, mXSize - 1);
   long      randomY  = GetSimulation()->GetRandom().Uniform(0l, mYSize - 1);
   WsfPFNode randNode = (WsfPFNode&)*GetGrid(randomX, randomY);
   return randNode.mLoc;
}

void WsfPathFinder::DebugDrawGrid()
{
   node_iterator nodeIter;
   mDebugDrawObjPtr->SetPointSize(5);
   mDebugDrawObjPtr->SetColor(0.0, 1.0, 0.0);
   mDebugDrawObjPtr->BeginLines();
   for (nodeIter = begin(); nodeIter != end(); ++nodeIter)
   {
      double nodeLLA[3];
      double connectedNode[3] = {0};
      nodeIter->first.mLoc.GetLocationLLA(nodeLLA[0], nodeLLA[1], nodeLLA[2]);
      for (const_edge_iterator edgeIt = nodeIter->second->begin(); edgeIt != nodeIter->second->end(); ++edgeIt)
      {
         const WsfPFNode*   dstNode      = &edgeIt.from()->first;
         const WsfGeoPoint* dstNodePoint = &dstNode->mLoc;
         dstNodePoint->GetLocationLLA(connectedNode[0], connectedNode[1], connectedNode[2]);
         mDebugDrawObjPtr->VertexLLA(nodeLLA[0], nodeLLA[1], nodeLLA[2]);
         mDebugDrawObjPtr->VertexLLA(connectedNode[0], connectedNode[1], connectedNode[2]);
      }
   }
   mDebugDrawObjPtr->End();
}

void WsfPathFinder::DebugDrawZones()
{
   std::list<WsfZone*>::const_iterator i;
   mDebugDrawObjPtr->SetColor(1.0, 1.0, 1.0);
   mDebugDrawObjPtr->SetLineSize(3);
   for (i = mZones.begin(); i != mZones.end(); ++i)
   {
      WsfZoneDefinition*                              aZonePtr   = (WsfZoneDefinition*)*i;
      std::vector<WsfZone::Point>                     zonePoints = aZonePtr->GetPoints();
      std::vector<WsfZoneDefinition::Point>::iterator pointItr;
      pointItr                                = zonePoints.begin();
      WsfZoneDefinition::Point firstPt        = *pointItr;
      double                   aFirstPoint[3] = {firstPt.mX, firstPt.mY, 0};
      mDebugDrawObjPtr->BeginPolyline();
      while (pointItr != zonePoints.end())
      {
         WsfZoneDefinition::Point aPtPtr    = *pointItr;
         double                   aPoint[3] = {aPtPtr.mX, aPtPtr.mY, 0};
         mDebugDrawObjPtr->VertexLLA(aPoint[0], aPoint[1], aPoint[2]);
         ++pointItr;
      }

      mDebugDrawObjPtr->VertexLLA(aFirstPoint[0], aFirstPoint[1], aFirstPoint[2]);
      mDebugDrawObjPtr->End();
   }
}

/************************************************************************/
/* Script Routines                                                      */
/************************************************************************/

WsfScriptPathFinderClass::WsfScriptPathFinderClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptObjectClass(aClassName, aScriptTypesPtr)
{
   mConstructible = true;
   mCloneable     = true;

   AddClassName("WsfPathFinder");

   AddMethod(ut::make_unique<FindClosestPointOnEdge>());
   AddMethod(ut::make_unique<FindClosestValidPoint>());
   AddMethod(ut::make_unique<RandomLocation>());
   AddMethod(ut::make_unique<NearEndOfPath>());
   AddMethod(ut::make_unique<SetDebugEnabled>());
   AddMethod(ut::make_unique<DebugEnabled>());
   AddMethod(ut::make_unique<DebugDrawGrid>());
   AddMethod(ut::make_unique<DebugDrawZones>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPathFinderClass, WsfPathFinder, FindClosestPointOnEdge, 1, "WsfGeoPoint", "WsfGeoPoint")
{
   WsfGeoPoint* startPointPtr = static_cast<WsfGeoPoint*>(aVarArgs[0].GetPointer()->GetAppObject());
   // WsfPlatform* aPlatformPtr = (WsfPlatform*) aVarArgs[1].GetPointer()->GetAppObject();
   // WsfRouteMover* moverPtr = dynamic_cast<WsfRouteMover*>(aPlatformPtr->GetMover());
   // WsfPathFinder* aPF = moverPtr->GetPathFinder();

   WsfGeoPoint* gpPtr = aObjectPtr->FindClosestPointOnEdge(WsfScriptContext::GetTIME_NOW(aContext),
                                                           WsfGeoPoint(startPointPtr->GetLat(),
                                                                       startPointPtr->GetLon(),
                                                                       startPointPtr->GetAlt()));

   ////WayptVecIter GetEndWaypoint()          { return mWaypoints.end();   }
   // double endPt[3];
   // endPt[0] = moverPtr->GetRoute()->GetWaypointAt(moverPtr->GetRoute()->GetSize()-1).GetLat();
   // endPt[1] = moverPtr->GetRoute()->GetWaypointAt(moverPtr->GetRoute()->GetSize()-1).GetLon();
   // endPt[2] = moverPtr->GetPlatform()->GetAltitude();
   // WsfGeoPoint* gpPtr = new WsfGeoPoint(endPt[0], endPt[1], endPt[2]);
   aReturnVal.SetPointer(new UtScriptRef(gpPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPathFinderClass, WsfPathFinder, FindClosestValidPoint, 1, "WsfGeoPoint", "WsfGeoPoint")
{
   WsfGeoPoint* startPointPtr = static_cast<WsfGeoPoint*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfGeoPoint* gpPtr         = aObjectPtr->FindClosestValidPoint(WsfScriptContext::GetTIME_NOW(aContext),
                                                          WsfGeoPoint(startPointPtr->GetLat(),
                                                                      startPointPtr->GetLon(),
                                                                      startPointPtr->GetAlt()));
   aReturnVal.SetPointer(new UtScriptRef(gpPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPathFinderClass, WsfPathFinder, NearEndOfPath, 2, "bool", "WsfPlatform, double")
{
   WsfPlatform*    aPlatformPtr = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   double          maxDistance  = aVarArgs[1].GetDouble();
   WsfRouteMover*  moverPtr     = dynamic_cast<WsfRouteMover*>(aPlatformPtr->GetMover());
   int             idx          = moverPtr->GetTargetIndex();
   const WsfRoute* curRoutePtr  = moverPtr->GetRoute();
   double          locLLA[3]    = {0, 0, 0};
   aPlatformPtr->GetLocationLLA(locLLA[0], locLLA[1], locLLA[2]);
   WsfGeoPoint        geoLoc(locLLA[0], locLLA[1], locLLA[2]);
   const WsfWaypoint& endWaypt = curRoutePtr->GetWaypointAt(curRoutePtr->GetSize() - 1);
   endWaypt.GetLocationLLA(locLLA[0], locLLA[1], locLLA[2]);
   WsfGeoPoint endGeoLoc(locLLA[0], locLLA[1], locLLA[2]);
   double      distance = geoLoc.GetDistanceFrom(endGeoLoc);
   bool        nearEnd  = ((unsigned int)idx >= curRoutePtr->GetSize() - 1);
   nearEnd |= distance < maxDistance;
   aReturnVal.SetBool(nearEnd);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPathFinderClass, WsfPathFinder, RandomLocation, 0, "WsfGeoPoint", "")
{
   WsfGeoPoint  tmpPt    = aObjectPtr->GetRandomLocation();
   WsfGeoPoint* retPtPtr = new WsfGeoPoint(tmpPt);
   aReturnVal.SetPointer(new UtScriptRef(retPtPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPathFinderClass, WsfPathFinder, SetDebugEnabled, 1, "void", "bool")
{
   aObjectPtr->SetDebugEnabled(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPathFinderClass, WsfPathFinder, DebugEnabled, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->DebugEnabled());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPathFinderClass, WsfPathFinder, DebugDrawGrid, 0, "void", "")
{
   aObjectPtr->DebugDrawGrid();

   WsfNavigationMesh* tmpPtr = dynamic_cast<WsfNavigationMesh*>(aObjectPtr);
   if (tmpPtr != nullptr && tmpPtr->GetNumCells() > 0)
   {
      tmpPtr->DebugDrawMesh();
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPathFinderClass, WsfPathFinder, DebugDrawZones, 0, "void", "")
{
   aObjectPtr->DebugDrawZones();
}

// virtual
bool WsfPathFinder::ComputeFindPath(WsfMover& aMover, WsfRoute& aRoute)
{
   int initialPoint(0);
   return ComputeSetRoute(aMover, aRoute, initialPoint);
}

// virtual
bool WsfPathFinder::ComputeSetRoute(WsfMover& aMover, WsfRoute& aRoute, int& aInitialPointIndex)
{
   bool routeChanged = false;
   if (aRoute.GetSize() > 1)
   {
      WsfRoute newRoute;
      for (unsigned int i = 1; i < aRoute.GetSize(); ++i)
      {
         // create a new route to append
         WsfRoute           newTemp;
         const WsfWaypoint& aWaypoint = aRoute.GetWaypointAt(i - 1);
         const WsfGeoPoint  aNewStart(aWaypoint.GetLat(), aWaypoint.GetLon(), aWaypoint.GetAlt());

         const WsfWaypoint& aEndWaypoint = aRoute.GetWaypointAt(i);
         WsfGeoPoint        aNewEnd(aEndWaypoint.GetLat(), aEndWaypoint.GetLon(), aEndWaypoint.GetAlt());
         double             cost    = 0.0;
         bool               success = FindPath(aNewStart, aNewEnd, newTemp, cost);
         if (success)
         {
            newRoute.Append(newTemp);
         }
      }
      if (newRoute.GetSize() > 0)
      {
         newRoute[0].SetSpeed(aRoute[0].GetSpeed());
         aRoute = newRoute;

         routeChanged = true;
      }
   }
   return routeChanged;
}

bool WsfPathFinder::ComputeUpdateRoute(WsfMover& aMover, WsfRoute& aRoute)
{
   int initialPoint(0);
   return ComputeSetRoute(aMover, aRoute, initialPoint);
}

bool WsfPathFinder::ComputeExtrapolate(WsfMover& aMover, WsfRoute& aRoute)
{
   const WsfRoute* defRoutePtr = aMover.GetDefaultRoute();

   if (!defRoutePtr)
   {
      return false;
   }
   double startLLA[3];
   double endLLA[3];
   aMover.GetPlatform()->GetLocationLLA(startLLA[0], startLLA[1], startLLA[2]);
   const WsfGeoPoint newStart(startLLA[0], startLLA[1], startLLA[2]);
   // get the first point of the default route
   const WsfWaypoint& defaultStart = defRoutePtr->GetWaypointAt(0);
   defaultStart.GetLocationLLA(endLLA[0], endLLA[1], endLLA[2]);
   WsfGeoPoint newEnd(endLLA[0], endLLA[1], endLLA[2]);

   if (aRoute.GetSize() == 1)
   {
      for (unsigned int i = 0; i < defRoutePtr->GetSize(); ++i)
      {
         aRoute.Append(defRoutePtr->GetWaypointAt(i));
      }
      aRoute.SetName("");
      return true;
   }
   aRoute.Clear();
   double cost = 0.0;
   FindPath(newStart, newEnd, aRoute, cost);

   // now add the rest of the default route to this route
   for (unsigned int i = 1; i < defRoutePtr->GetSize(); ++i)
   {
      aRoute.Append(defRoutePtr->GetWaypointAt(i));
   }

   aRoute.SetName("");
   return true;
}

bool WsfPathFinder::ReturnToRoute(WsfMover& aMover, WsfRoute& aRoute)
{
   WsfRoute           newTemp;
   WsfRouteMover*     rMoverPtr = (WsfRouteMover*)&aMover;
   int                idx       = rMoverPtr->GetCurrentIndex();
   const WsfWaypoint& aWaypoint = aRoute.GetWaypointAt(idx);
   const WsfGeoPoint  aNewStart(aWaypoint.GetLat(), aWaypoint.GetLon(), aWaypoint.GetAlt());

   auto defaultRoute = aMover.GetDefaultRoute();
   if (defaultRoute)
   {
      const WsfWaypoint& aEndWaypoint = defaultRoute->GetWaypointAt(0);
      WsfGeoPoint        aNewEnd(aEndWaypoint.GetLat(), aEndWaypoint.GetLon(), aEndWaypoint.GetAlt());
      double             cost    = 0.0;
      bool               success = FindPath(aNewStart, aNewEnd, newTemp, cost);
      aRoute                     = newTemp;
      return success;
   }
   else
   {
      return false;
   }
}

const char* WsfPathFinder::GetScriptClassName() const
{
   return "WsfPathFinder";
}


WsfPFNode::WsfPFNode(long aX, long aY, float aBaseWeight, WsfGeoPoint& aLoc)
   : mNeighbors()
   , mX(aX)
   , mY(aY)
   , mNormalAngle(0.0)
   , mBaseWeight(aBaseWeight)
   , mWeight(aBaseWeight)
   , mMarkedPt(false)
   , mLoc(aLoc)
   , mNetworkIndex(aX * 1000000 + aY)
{
   if (mBaseWeight == 0)
   {
      mBaseWeight = 0.01f;
   }
}

bool WsfPFNode::operator==(const WsfPFNode& aRhs) const
{
   return (mY == aRhs.mY && mX == aRhs.mX);
}

bool WsfPFNode::operator!=(const WsfPFNode& aRhs) const
{
   return (mY != aRhs.mY || mX != aRhs.mX);
}

bool WsfPFNode::operator<(const WsfPFNode& aRhs) const
{
   return (mX == aRhs.mX ? mY < aRhs.mY : mX < aRhs.mX);
}

void WsfPFNode::setMarked(bool b)
{
   mMarkedPt = b;
}

bool WsfPFNode::getMarked()
{
   return mMarkedPt;
}

void WsfPFNode::AddNeighbor(WsfPFNode* aNodePtr)
{
   std::vector<WsfPFNode*>::iterator neighborIter;
   neighborIter = find(mNeighbors.begin(), mNeighbors.end(), aNodePtr);
   if (neighborIter == mNeighbors.end())
   {
      mNeighbors.push_back(aNodePtr);
   }
}

WsfPathFinderTypes::WsfPathFinderTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfPathFinder>(aScenario, "path_finder")
{
}

std::unique_ptr<WsfPathFinder> WsfPathFinderTypes::ProcessPathFinderInput(UtInput& aInput)
{
   UtInputBlock inputBlock(aInput, "end_pathfinder");
   if (aInput.GetCommand() == "terrainpathfinder")
   {
      std::string pfName;
      aInput.ReadValue(pfName);
      auto pfPtr = ut::make_unique<WsfTerrainPathFinder>(GetScenario());
      pfPtr->SetName(pfName);
      ProcessBlock(inputBlock, pfPtr.get(), true);
      return std::move(pfPtr);
   }
   else if (aInput.GetCommand() == "navigationmesh")
   {
      std::string pfName;
      aInput.ReadValue(pfName);
      auto pfPtr = ut::make_unique<WsfNavigationMesh>(GetScenario());
      pfPtr->SetName(pfName);
      ProcessBlock(inputBlock, pfPtr.get(), true);
      return std::move(pfPtr);
   }
   else
   {
      std::string pfName;
      aInput.ReadValue(pfName);
      auto pfPtr = ut::make_unique<WsfPathFinder>(GetScenario());
      pfPtr->SetName(pfName);
      ProcessBlock(inputBlock, pfPtr.get(), true);
      return pfPtr;
   }
}

/** Create an instance of a 'type object' of this class

Process the current command if it would create an instance of an 'type object' of this class.

@param aInput The input stream.
@return 'true' if the command was processed or 'false' if not.
@throws UtInput::ExceptionBase (or derived class) if an error was encountered.
*/
WsfPathFinderTypes::LoadResult WsfPathFinderTypes::LoadInstance(UtInput& aInput)
{
   LoadResult result;
   if (aInput.GetCommand() == "pathfinder")
   {
      result.mIsCommandProcessed = true;

      auto pfPtr            = ProcessPathFinderInput(aInput);
      result.mObjectTypePtr = pfPtr.get();
      GetScenario()->GetPathFinderList().Add(std::move(pfPtr));
   }

   if (aInput.GetCommand() == "terrainpathfinder")
   {
      result.mIsCommandProcessed = true;

      auto pfPtr            = ProcessPathFinderInput(aInput);
      result.mObjectTypePtr = pfPtr.get();
      GetScenario()->GetPathFinderList().Add(std::move(pfPtr));
   }

   if (aInput.GetCommand() == "navigationmesh")
   {
      result.mIsCommandProcessed = true;

      auto pfPtr            = ProcessPathFinderInput(aInput);
      result.mObjectTypePtr = pfPtr.get();
      GetScenario()->GetPathFinderList().Add(std::move(pfPtr));
   }
   return result;
}

// private
// static
void WsfPathFinderTypes::ProcessBlock(UtInputBlock& aInputBlock, WsfPathFinder* aPFPtr, bool init)
{
   while (aInputBlock.ReadCommand())
   {
      if (aInputBlock.GetCommand() == "RQT")
      {
         UtInputBlock inputBlock(aInputBlock.GetInput(), "end_RQT");
         ProcessBlock(inputBlock, aPFPtr, false);
         continue;
      }
      if (!aPFPtr->ProcessInput(aInputBlock.GetInput()))
      {
         throw UtInput::UnknownCommand(aInputBlock.GetInput());
      }
   }
   //    if (init && ! aPFPtr->Initialize())
   //    {
   //       throw UtInput::BadValue(aInputBlock.GetInput(),
   //          "Unable to initialize PathFinder " + aPFPtr->GetName());
   //    }
}

bool WsfPathFinderTypes::ProcessInput(UtInput& aInput)
{
   return LoadInstance(aInput);
}

WsfPathFinderList::WsfPathFinderList()
{
   mSimulationPtr = nullptr;
}


WsfPathFinder* WsfPathFinderList::Find(const std::string& aPathFinderName) const
{
   auto i = mPathFinders.find(aPathFinderName);
   if (i != mPathFinders.end())
   {
      return i->second.get();
   }
   return nullptr;
}

void WsfPathFinderList::Add(std::unique_ptr<WsfPathFinder> aPathFinderPtr)
{
   if (mSimulationPtr != nullptr)
   {
      aPathFinderPtr->Initialize(mSimulationPtr);
   }
   mPathFinders[aPathFinderPtr->GetName()] = std::move(aPathFinderPtr);
}

bool WsfPathFinderList::Initialize(WsfSimulation* aSimulationPtr)
{
   mSimulationPtr = aSimulationPtr;
   bool ok        = true;
   for (auto i = mPathFinders.begin(); i != mPathFinders.end(); ++i)
   {
      if (!i->second->Initialize(aSimulationPtr))
      {
         ok       = false;
         auto out = ut::log::error() << "Failed to initialize path finder.";
         out.AddNote() << "Path Finder: " << i->first;
      }
   }
   return ok;
}
