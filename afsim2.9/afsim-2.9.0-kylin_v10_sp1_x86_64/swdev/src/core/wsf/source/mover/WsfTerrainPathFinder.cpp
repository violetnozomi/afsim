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
//*   This class is used to find a path from A to B in a 3D environment with
//*   weighted nodes.  It uses the UtGraph class to build a set of nodes that
//*   hold the weights for each square in a lat/lon grid.  Squares are measured
//*   in arc degrees rather than meters because the class needs to work on a
//*   spherical earth.  Extending from WsfPathfinder, this class makes use of
//*   available terrain data and analyzes the height data as well.  The class
//*   makes use of the Restricted Quadtree Triangulation algorithm
//*   to reduce the amount of nodes used to represent the pathing grid.
//*
//* AUTHOR
//*   Nick Chinnici
//*
//* NOTES:
//*   For more information about the Restricted Quadtree Triangulation algorithm
//*    visit: http://portal.acm.org/citation.cfm?id=288216.288219
//***************************************************************************

#include "WsfTerrainPathFinder.hpp"

#include <cassert>
#include <fstream>
#include <sstream>

#include "UtInput.hpp"
#include "UtLineSegment.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "UtVec3.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfTerrain.hpp"
#include "WsfZone.hpp"

WsfTerrainPathFinder::WsfTerrainPathFinder(WsfScenario* aScenarioPtr)
   : WsfPathFinder(aScenarioPtr)
   , mRQTMetric(0.0)
   , mChunkSize(0)
   , mRQTFile()
   , mUseRQTFile(false)
   , mNavMeshZonesFile()
   , mMaxNormalAngle(0.f)
{
}

bool WsfTerrainPathFinder::Initialize(WsfSimulation* aSimulationPtr)
{
   mSimulationPtr = aSimulationPtr;
   mInitialized   = false;
   // first check to see if a load file has been specified
   if (mUseRQTFile)
   {
      LoadGridInfo();
      mInitialized = true;
      return true;
   }

   // Make sure we have the inputs we need to build the underlying grid
   assert(mGridSizeDegrees != 0);
   mXSize = (long)ceil((mLowerRight.GetLon() - mUpperLeft.GetLon()) / mGridSizeDegrees);
   mYSize = (long)ceil((mUpperLeft.GetLat() - mLowerRight.GetLat()) / mGridSizeDegrees);

   mGrid = std::vector<node_iterator>(mXSize * mYSize);

   // Now build the graph that sits under the grid and that we'll use for path finding
   WsfPFNode*    tempNodePtr;
   WsfGeoPoint   tempPoint;
   wsf::Terrain* mTerrainPtr = new wsf::Terrain(aSimulationPtr->GetTerrainInterface());
   aSimulationPtr->GetTerrainInterface()->EnableTerrain();
   int i;
   for (i = 0; i < mXSize; ++i)
   {
      for (int j = 0; j < mYSize; ++j)
      {
         tempPoint          = WsfGeoPoint(mUpperLeft.GetLat() - ((double)j + 0.5) * mGridSizeDegrees,
                                 mUpperLeft.GetLon() + ((double)i + 0.5) * mGridSizeDegrees,
                                 0);
         float tmpElevation = 0.f;
         mTerrainPtr->GetElevApprox(tempPoint.GetLat(), tempPoint.GetLon(), tmpElevation);
         tempPoint.SetLocationLLA(tempPoint.GetLat(), tempPoint.GetLon(), tmpElevation);
         tempNodePtr = new WsfPFNode(i, j, 1, tempPoint); // get height from the terrain
         insert(*tempNodePtr);
         SetGrid(i, j, find(*tempNodePtr));
      }
   }

   delete mTerrainPtr;

   // the initial grid has been created.  Now run RQT reduction algorithm if requested...
   ComputeNodeNormals();
   RQTRefineMesh();

   mInitialized = true;
   RecalculateWeights();

   GeneratePathFinderLight(); // output the generated graph into a text file for to load in later

   return true;
}

void WsfTerrainPathFinder::RQTRefineMesh(float aMetric)
{
   mRQTMetric = aMetric;
   RQTRefineMesh();
}

void WsfTerrainPathFinder::RQTRefineMesh()
{
   erase_all_edges();
   // clear all refined data and set weights of bad normals...
   for (int i = 0; i < mXSize; ++i)
   {
      for (int j = 0; j < mYSize; ++j)
      {
         WsfPFNode& n = (WsfPFNode&)*GetGrid(i, j);
         n.setMarked(false);

         if (n.mNormalAngle > mMaxNormalAngle)
         {
            n.mWeight = std::numeric_limits<double>::max();
         }
         else if (n.mLoc.GetAlt() <= SHRT_MIN + 1)
         {
            n.mWeight = std::numeric_limits<double>::max();
         }
         else
         {
            n.mWeight = 1.0f;
         }
      }
   }

   // At this point all corners of the defined chunk size are set as marked to be included into the mesh
   //(i.e. a chunk size defined as 3 will create 3x3 'chunks' in the graph).
   for (int x = 0; x < mXSize; x += mChunkSize - 1)
   {
      for (int y = 0; y < mYSize; y += mChunkSize - 1)
      {
         bool markNorthEast = true;

         WsfPFNode& n = (WsfPFNode&)*GetGrid(x, y);
         n.setMarked(true);

         // mark the corner "north" of the current node
         if (y + (mChunkSize - 1) < mYSize)
         {
            WsfPFNode& north = (WsfPFNode&)*GetGrid(x, y + (mChunkSize - 1));
            north.setMarked(true);
         }
         else
         {
            markNorthEast = false;
         }
         // mark the corner "east" of the current node
         if (x + (mChunkSize - 1) < mXSize)
         {
            WsfPFNode& east = (WsfPFNode&)*GetGrid(x + (mChunkSize - 1), y);
            east.setMarked(true);
         }
         else
         {
            markNorthEast = false;
         }
         // if "north" and "east" nodes were valid, then mark the "northeast" node as well
         if (markNorthEast)
         {
            WsfPFNode& northEast = (WsfPFNode&)*GetGrid(x + (mChunkSize - 1), y + (mChunkSize - 1));
            northEast.setMarked(true);
         }
         // always mark the east boundry
         if (x == mXSize - 2)
         {
            WsfPFNode& east = (WsfPFNode&)*GetGrid(mXSize - 1, y);
            east.setMarked(true);
         }
      }
      // always mark the north boundry
      WsfPFNode& north = (WsfPFNode&)*GetGrid(x, mYSize - 1);
      north.setMarked(true);
   }

   // at this point all chunk corner nodes are marked.
   // now it is time to determine all other nodes that should be marked given the metric
   for (int k = 2; k < mXSize; k *= 2)
   {
      int stride = k;
      // visit non-'center point' verts first
      for (int x = 0; x < mXSize - stride; x += stride)
      {
         for (int y = 0; y < mXSize - stride; y += stride)
         {
            RQTEvaluateLine(x, y, mRQTMetric, stride);
         }
      }
   }

   for (int i = 0; i < mXSize; ++i)
   {
      for (int j = 0; j < mYSize; ++j)
      {
         WsfPFNode& n = (WsfPFNode&)*GetGrid(i, j);

         // if it is not a RQT marked node, leave it out of the nav mesh
         if (!n.getMarked())
         {
            n.mWeight = std::numeric_limits<double>::max();
            continue;
         }

         // find the closest 'east' marked node
         for (int k = i + 1; k < mXSize; ++k)
         {
            WsfPFNode& tmp = (WsfPFNode&)*GetGrid(k, j);
            if (tmp.getMarked())
            {
               if (std::abs(i - k) > mChunkSize)
               {
                  break;
               }
               else
               {
                  WsfPFNode  aNode      = (WsfPFNode&)*GetGrid(i, j);
                  WsfPFNode* tmpNodePtr = &aNode;

                  WsfPFNode  aNode2      = (WsfPFNode&)*GetGrid(k, j);
                  WsfPFNode* tmpNode2Ptr = &aNode2;

                  WsfPFEdge tempEdge(tmpNodePtr, tmpNode2Ptr);
                  if (!IsDuplicateEdge(GetGrid(i, j), aNode2, tempEdge))
                  {
                     insert_edge(GetGrid(i, j), GetGrid(k, j), tempEdge);
                     WsfPFNode& node1 = (WsfPFNode&)*GetGrid(i, j);
                     WsfPFNode& node2 = (WsfPFNode&)*GetGrid(k, j);
                     node1.AddNeighbor(&node2);
                     node2.AddNeighbor(&node1);
                  }
                  break;
               }
            }
         }

         // find the closest 'north' marked node
         for (int k = j + 1; k < mYSize; ++k)
         {
            WsfPFNode& tmp = (WsfPFNode&)*GetGrid(i, k);
            if (tmp.getMarked())
            {
               if (std::abs(j - k) > mChunkSize)
               {
                  break;
               }
               else
               {
                  WsfPFNode  aNode      = (WsfPFNode&)*GetGrid(i, j);
                  WsfPFNode* tmpNodePtr = &aNode;

                  WsfPFNode  aNode2      = (WsfPFNode&)*GetGrid(i, k);
                  WsfPFNode* tmpNode2Ptr = &aNode2;

                  WsfPFEdge tempEdge(tmpNodePtr, tmpNode2Ptr);
                  // WsfPFEdge tempEdge((WsfPFNode*)&GetGrid(i, j), (WsfPFNode*)&GetGrid(i, k));
                  if (!IsDuplicateEdge(GetGrid(i, j), aNode2, tempEdge))
                  {
                     insert_edge(GetGrid(i, j), GetGrid(i, k), tempEdge);
                     WsfPFNode& node1 = (WsfPFNode&)*GetGrid(i, j);
                     WsfPFNode& node2 = (WsfPFNode&)*GetGrid(i, k);
                     node1.AddNeighbor(&node2);
                     node2.AddNeighbor(&node1);
                  }
                  break;
               }
            }
         }

         // find the closest 'west' marked node
         for (int k = i - 1; k >= 0; k--)
         {
            WsfPFNode& tmp = (WsfPFNode&)*GetGrid(k, j);
            if (tmp.getMarked())
            {
               if (std::abs(i - k) > mChunkSize)
               {
                  break;
               }
               else
               {
                  WsfPFNode  aNode      = (WsfPFNode&)*GetGrid(i, j);
                  WsfPFNode* tmpNodePtr = &aNode;

                  WsfPFNode  aNode2      = (WsfPFNode&)*GetGrid(k, j);
                  WsfPFNode* tmpNode2Ptr = &aNode2;

                  WsfPFEdge tempEdge(tmpNodePtr, tmpNode2Ptr);
                  // WsfPFEdge tempEdge((WsfPFNode*)&GetGrid(i, j), (WsfPFNode*)&GetGrid(k, j));
                  if (!IsDuplicateEdge(GetGrid(i, j), aNode2, tempEdge))
                  {
                     insert_edge(GetGrid(i, j), GetGrid(k, j), tempEdge);
                     WsfPFNode& node1 = (WsfPFNode&)*GetGrid(i, j);
                     WsfPFNode& node2 = (WsfPFNode&)*GetGrid(k, j);
                     node1.AddNeighbor(&node2);
                     node2.AddNeighbor(&node1);
                  }
                  break;
               }
            }
         }

         // find the closest 'south' marked node
         for (int k = j - 1; k >= 0; k--)
         {
            WsfPFNode& tmp = (WsfPFNode&)*GetGrid(i, k);
            if (tmp.getMarked())
            {
               if (std::abs(j - k) > mChunkSize)
               {
                  break;
               }
               else
               {
                  WsfPFNode  aNode      = (WsfPFNode&)*GetGrid(i, j);
                  WsfPFNode* tmpNodePtr = &aNode;

                  WsfPFNode  aNode2      = (WsfPFNode&)*GetGrid(i, k);
                  WsfPFNode* tmpNode2Ptr = &aNode2;

                  WsfPFEdge tempEdge(tmpNodePtr, tmpNode2Ptr);
                  // WsfPFEdge tempEdge((WsfPFNode*)&GetGrid(i, j), (WsfPFNode*)&GetGrid(i, k));
                  if (!IsDuplicateEdge(GetGrid(i, j), aNode2, tempEdge))
                  {
                     insert_edge(GetGrid(i, j), GetGrid(i, k), tempEdge);
                     WsfPFNode& node1 = (WsfPFNode&)*GetGrid(i, j);
                     WsfPFNode& node2 = (WsfPFNode&)*GetGrid(i, k);
                     node1.AddNeighbor(&node2);
                     node2.AddNeighbor(&node1);
                  }
                  break;
               }
            }
         }
         // find the closest 'west' marked node
         for (int k = i - 1; k >= 0; k--)
         {
            WsfPFNode& tmp = (WsfPFNode&)*GetGrid(k, j);
            if (tmp.getMarked())
            {
               if (std::abs(i - k) > mChunkSize)
               {
                  break;
               }
               else
               {
                  WsfPFNode  aNode      = (WsfPFNode&)*GetGrid(i, j);
                  WsfPFNode* tmpNodePtr = &aNode;

                  WsfPFNode  aNode2      = (WsfPFNode&)*GetGrid(k, j);
                  WsfPFNode* tmpNode2Ptr = &aNode2;

                  WsfPFEdge tempEdge(tmpNodePtr, tmpNode2Ptr);
                  // WsfPFEdge tempEdge((WsfPFNode*)&GetGrid(i, j), (WsfPFNode*)&GetGrid(k, j));
                  if (!IsDuplicateEdge(GetGrid(i, j), aNode2, tempEdge))
                  {
                     insert_edge(GetGrid(i, j), GetGrid(k, j), tempEdge);
                     WsfPFNode& node1 = (WsfPFNode&)*GetGrid(i, j);
                     WsfPFNode& node2 = (WsfPFNode&)*GetGrid(k, j);
                     node1.AddNeighbor(&node2);
                     node2.AddNeighbor(&node1);
                  }
                  break;
               }
            }
         }

         // find the 'northeast' marked node
         int x = i + 1;
         int y = j + 1;
         while (x < mXSize && y < mYSize)
         {
            WsfPFNode& tmp = (WsfPFNode&)*GetGrid(x, y);
            if (tmp.getMarked())
            {
               if (std::abs(i - x) > mChunkSize || std::abs(j - y) > mChunkSize)
               {
                  break;
               }
               else
               {
                  WsfPFNode  aNode      = (WsfPFNode&)*GetGrid(i, j);
                  WsfPFNode* tmpNodePtr = &aNode;

                  WsfPFNode  aNode2      = (WsfPFNode&)*GetGrid(x, y);
                  WsfPFNode* tmpNode2Ptr = &aNode2;

                  WsfPFEdge tempEdge(tmpNodePtr, tmpNode2Ptr);

                  if (!IsDuplicateEdge(GetGrid(i, j), aNode2, tempEdge))
                  {
                     insert_edge(GetGrid(i, j), GetGrid(x, y), tempEdge);
                     WsfPFNode& node1 = (WsfPFNode&)*GetGrid(i, j);
                     WsfPFNode& node2 = (WsfPFNode&)*GetGrid(x, y);
                     node1.AddNeighbor(&node2);
                     node2.AddNeighbor(&node1);
                  }
                  break;
               }
            }
            x++;
            y++;
         }

         // find the 'northwest' marked node
         x = i - 1;
         y = j + 1;
         while (x >= 0 && y < mYSize)
         {
            WsfPFNode& tmp = (WsfPFNode&)*GetGrid(x, y);
            if (tmp.getMarked())
            {
               if (std::abs(i - x) > mChunkSize || std::abs(j - y) > mChunkSize)
               {
                  break;
               }
               else
               {
                  WsfPFNode  aNode      = (WsfPFNode&)*GetGrid(i, j);
                  WsfPFNode* tmpNodePtr = &aNode;

                  WsfPFNode  aNode2      = (WsfPFNode&)*GetGrid(x, y);
                  WsfPFNode* tmpNode2Ptr = &aNode2;

                  WsfPFEdge tempEdge(tmpNodePtr, tmpNode2Ptr);

                  if (!IsDuplicateEdge(GetGrid(i, j), aNode2, tempEdge))
                  {
                     insert_edge(GetGrid(i, j), GetGrid(x, y), tempEdge);
                     WsfPFNode& node1 = (WsfPFNode&)*GetGrid(i, j);
                     WsfPFNode& node2 = (WsfPFNode&)*GetGrid(x, y);
                     node1.AddNeighbor(&node2);
                     node2.AddNeighbor(&node1);
                  }
                  break;
               }
            }
            x--;
            y++;
         }

         // find the 'southwest' marked node
         x = i - 1;
         y = j - 1;
         while (x >= 0 && y >= 0)
         {
            WsfPFNode& tmp = (WsfPFNode&)*GetGrid(x, y);
            if (tmp.getMarked())
            {
               if (std::abs(i - x) > mChunkSize || std::abs(j - y) > mChunkSize)
               {
                  break;
               }
               else
               {
                  WsfPFNode  aNode      = (WsfPFNode&)*GetGrid(i, j);
                  WsfPFNode* tmpNodePtr = &aNode;

                  WsfPFNode  aNode2      = (WsfPFNode&)*GetGrid(x, y);
                  WsfPFNode* tmpNode2Ptr = &aNode2;

                  WsfPFEdge tempEdge(tmpNodePtr, tmpNode2Ptr);

                  if (!IsDuplicateEdge(GetGrid(i, j), aNode2, tempEdge))
                  {
                     insert_edge(GetGrid(i, j), GetGrid(x, y), tempEdge);
                     WsfPFNode& node1 = (WsfPFNode&)*GetGrid(i, j);
                     WsfPFNode& node2 = (WsfPFNode&)*GetGrid(x, y);
                     node1.AddNeighbor(&node2);
                     node2.AddNeighbor(&node1);
                  }
                  break;
               }
            }
            x--;
            y--;
         }

         // find the 'southeast' marked node
         x = i + 1;
         y = j - 1;
         while (x < mXSize && y >= 0)
         {
            WsfPFNode& tmp = (WsfPFNode&)*GetGrid(x, y);
            if (tmp.getMarked())
            {
               if (std::abs(i - x) > mChunkSize || std::abs(j - y) > mChunkSize)
               {
                  break;
               }
               else
               {
                  WsfPFNode  aNode      = (WsfPFNode&)*GetGrid(i, j);
                  WsfPFNode* tmpNodePtr = &aNode;

                  WsfPFNode  aNode2      = (WsfPFNode&)*GetGrid(x, y);
                  WsfPFNode* tmpNode2Ptr = &aNode2;

                  WsfPFEdge tempEdge(tmpNodePtr, tmpNode2Ptr);

                  if (!IsDuplicateEdge(GetGrid(i, j), aNode2, tempEdge))
                  {
                     insert_edge(GetGrid(i, j), GetGrid(x, y), tempEdge);
                     WsfPFNode& node1 = (WsfPFNode&)*GetGrid(i, j);
                     WsfPFNode& node2 = (WsfPFNode&)*GetGrid(x, y);
                     node1.AddNeighbor(&node2);
                     node2.AddNeighbor(&node1);
                  }
                  break;
               }
            }
            x++;
            y--;
         }
      }
   }
}

void WsfTerrainPathFinder::RQTEvaluateLine(int x, int y, float metric, int stride)
{
   int max = stride;
   int mid = stride / 2;

   ////TOP
   WsfPFNode& neighborTop = (WsfPFNode&)*GetGrid(x + mid, y + max);
   if (neighborTop.getMarked())
   {
      RQTMarkPointDependants(neighborTop.mX, neighborTop.mY, stride);
   }
   else
   {
      WsfPFNode& neighborTopLeft  = (WsfPFNode&)*GetGrid(x, y + max);
      WsfPFNode& neighborTopRight = (WsfPFNode&)*GetGrid(x + max, y + max);

      if (RQTPointLineDistance(&neighborTopLeft, &neighborTopRight, &neighborTop) > metric)
      {
         RQTMarkPointDependants(neighborTop.mX, neighborTop.mY, stride);
      }
   }

   ////BOTTOM
   WsfPFNode& neighborBottom = (WsfPFNode&)*GetGrid(x + mid, y);
   if (neighborBottom.getMarked())
   {
      RQTMarkPointDependants(neighborBottom.mX, neighborBottom.mY, stride);
   }
   else
   {
      WsfPFNode& neighborBottomLeft  = (WsfPFNode&)*GetGrid(x, y);
      WsfPFNode& neighborBottomRight = (WsfPFNode&)*GetGrid(x + max, y);

      if (RQTPointLineDistance(&neighborBottomLeft, &neighborBottomRight, &neighborBottom) > metric)
      {
         RQTMarkPointDependants(neighborBottom.mX, neighborBottom.mY, stride);
      }
   }

   ////LEFT
   WsfPFNode& neighborLeft = (WsfPFNode&)*GetGrid(x, y + mid);
   if (neighborLeft.getMarked())
   {
      RQTMarkPointDependants(neighborLeft.mX, neighborLeft.mY, stride);
   }
   else
   {
      WsfPFNode& neighborLeftBottom = (WsfPFNode&)*GetGrid(x, y);
      WsfPFNode& neighborLeftTop    = (WsfPFNode&)*GetGrid(x, y + max);

      if (RQTPointLineDistance(&neighborLeftBottom, &neighborLeftTop, &neighborLeft) > metric)
      {
         RQTMarkPointDependants(neighborLeft.mX, neighborLeft.mY, stride);
      }
   }

   ////RIGHT
   WsfPFNode& neighborRight = (WsfPFNode&)*GetGrid(x + max, y + mid);
   if (neighborRight.getMarked())
   {
      RQTMarkPointDependants(neighborRight.mX, neighborRight.mY, stride);
   }
   else
   {
      WsfPFNode& neighborRightBottom = (WsfPFNode&)*GetGrid(x + max, y);
      WsfPFNode& neighborRightTop    = (WsfPFNode&)*GetGrid(x + max, y + max);

      if (RQTPointLineDistance(&neighborRightBottom, &neighborRightTop, &neighborRight) > metric)
      {
         RQTMarkPointDependants(neighborRight.mX, neighborRight.mY, stride);
      }
   }

   ////MIDDLE
   WsfPFNode& neighborMiddle = (WsfPFNode&)*GetGrid(x + mid, y + mid);
   if (neighborMiddle.getMarked() || neighborTop.getMarked() || neighborBottom.getMarked() ||
       neighborLeft.getMarked() || neighborRight.getMarked())
   {
      RQTMarkPointDependants(neighborMiddle.mX, neighborMiddle.mY, stride);
   }
   else if (RQTPointLineDistance(&neighborBottom, &neighborTop, &neighborMiddle) > metric)
   {
      RQTMarkPointDependants(neighborMiddle.mX, neighborMiddle.mY, stride);
   }
   else if (RQTPointLineDistance(&neighborLeft, &neighborRight, &neighborMiddle) > metric)
   {
      RQTMarkPointDependants(neighborMiddle.mX, neighborMiddle.mY, stride);
   }
}

void WsfTerrainPathFinder::RQTMarkPointDependants(int x, int y, int stride)
{
   WsfPFNode& p = (WsfPFNode&)*GetGrid(x, y);
   p.setMarked(true);

   int by = stride / 2;

   // if it is a center point, mark it's neighbors at the corners...
   if (p.mX % stride != 0 && (p.mY % stride != 0))
   {
      if (PointIsInGridBounds(p.mX - by, p.mY - by))
      {
         WsfPFNode& sw = (WsfPFNode&)*GetGrid(p.mX - by, p.mY - by);
         sw.setMarked(true);
      }

      if (PointIsInGridBounds(p.mX + by, p.mY - by))
      {
         WsfPFNode& se = (WsfPFNode&)*GetGrid(p.mX + by, p.mY - by);
         se.setMarked(true);
      }

      if (PointIsInGridBounds(p.mX + by, p.mY + by))
      {
         WsfPFNode& ne = (WsfPFNode&)*GetGrid(p.mX + by, p.mY + by);
         ne.setMarked(true);
      }

      if (PointIsInGridBounds(p.mX - by, p.mY + by))
      {
         WsfPFNode& nw = (WsfPFNode&)*GetGrid(p.mX - by, p.mY + by);
         nw.setMarked(true);
      }
   }
   // else if it is a horizontal edge, mark the left and right neighbors
   else if (p.mX % stride == 0)
   {
      if (PointIsInGridBounds(p.mX + by, p.mY))
      {
         WsfPFNode& e = (WsfPFNode&)*GetGrid(p.mX + by, p.mY);
         e.setMarked(true);
      }

      if (PointIsInGridBounds(p.mX - by, p.mY))
      {
         WsfPFNode& w = (WsfPFNode&)*GetGrid(p.mX - by, p.mY);
         w.setMarked(true);
      }
   }
   // else if it is a vertical edge, mark the top and bottom neighbors
   else if (p.mY % stride == 0)
   {
      if (PointIsInGridBounds(p.mX, p.mY + by))
      {
         WsfPFNode& n = (WsfPFNode&)*GetGrid(p.mX, p.mY + by);
         n.setMarked(true);
      }

      if (PointIsInGridBounds(p.mX, p.mY - by))
      {
         WsfPFNode& s = (WsfPFNode&)*GetGrid(p.mX, p.mY - by);
         s.setMarked(true);
      }
   }
}

double WsfTerrainPathFinder::RQTPointLineDistance(WsfPFNode* APtr, WsfPFNode* BPtr, WsfPFNode* PPtr)
{
   double AP[3], AB[3], ABxAP[3];
   AP[0] = PPtr->mX - APtr->mX;
   AP[1] = PPtr->mY - APtr->mY;
   AP[2] = PPtr->mLoc.GetAlt() - APtr->mLoc.GetAlt();

   AB[0] = BPtr->mX - APtr->mX;
   AB[1] = BPtr->mY - APtr->mY;
   AB[2] = BPtr->mLoc.GetAlt() - APtr->mLoc.GetAlt();

   UtVec3d::CrossProduct(ABxAP, AB, AP);

   double a = UtVec3d::Magnitude(ABxAP);
   double d = UtVec3d::Magnitude(AB);

   return a / d;
}

void WsfTerrainPathFinder::ComputeNodeNormals()
{
   // loop through all the edges and get the normals
   wsf::Terrain* mTerrainPtr = new wsf::Terrain(GetSimulation()->GetTerrainInterface());
   for (int x = 0; x < mXSize; ++x)
   {
      for (int y = 0; y < mYSize; ++y)
      {
         // the node for which we want to compute the normal
         WsfPFNode& p = (WsfPFNode&)*GetGrid(x, y);

         double normalNED[3] = {0.0, 0.0, -1.0};
         if (mTerrainPtr->IsEnabled())
         {
            mTerrainPtr->GetNormalNED(p.mLoc.GetLat(), p.mLoc.GetLon(), normalNED);
         }
         double upAxis[3] = {0.0f, 0.0f, -1.0f};

         double cosTheta =
            UtVec3d::DotProduct(normalNED, upAxis) / (UtVec3d::Magnitude(normalNED) * UtVec3d::Magnitude(upAxis));

         p.mNormalAngle = (float)(acos(cosTheta) * 180 / UtMath::cPI); // convert from radians to degrees
      }
   }
   delete mTerrainPtr;
}

const WsfPFNode* WsfTerrainPathFinder::GetClosestNode(const WsfGeoPoint& aPointPtr)
{
   assert(mInitialized);
   return GetClosestNodePos((float)count_nodes() / 2.f, aPointPtr);
}


const WsfPFNode* WsfTerrainPathFinder::GetClosestNodePos(float x, const WsfGeoPoint& aPointPtr)
{
   int    closestPos  = 0;
   double closestDist = 10000;
   double heading;
   double distance;


   for (int i = 0; i < count_nodes(); ++i)
   {
      WsfPFNode tmpNode = (const WsfPFNode&)*GetGridAbsolute(i);

      if (!tmpNode.getMarked())
      {
         continue;
      }

      UtSphericalEarth::GreatCircleHeadingAndDistance(aPointPtr.GetLat(),
                                                      aPointPtr.GetLon(),
                                                      tmpNode.mLoc.GetLat(),
                                                      tmpNode.mLoc.GetLon(),
                                                      heading,
                                                      distance);

      if (distance < closestDist)
      {
         closestPos  = i;
         closestDist = distance;
      }
   }
   const WsfPFNode* retNodePtr = &(GetGridAbsolute(closestPos)->first);
   return retNodePtr;
}

bool WsfTerrainPathFinder::ProcessInput(UtInput& aInput)
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
   else if (command == "metric")
   {
      std::string sMetric;
      aInput.ReadValue(sMetric);
      mRQTMetric = (float)atof(sMetric.c_str());
   }
   else if (command == "min_LOD")
   {
      std::string sChunkSize;
      aInput.ReadValue(sChunkSize);
      mChunkSize = (int)atoi(sChunkSize.c_str());
   }
   else if (command == "output_file")
   {
      aInput.ReadValue(mRQTFile); // read in desired filename
      mUseRQTFile = false;
   }
   else if (command == "load_file")
   {
      aInput.ReadValue(mRQTFile);
      mUseRQTFile = true;
   }
   else if (command == "navigation_mesh_output_file")
   {
      aInput.ReadValue(mNavMeshZonesFile); // read in desired filename
   }
   else if (command == "max_slope")
   {
      std::string sMetric;
      aInput.ReadValue(sMetric);
      mMaxNormalAngle = (float)atof(sMetric.c_str());
      aInput.ReadValue(sMetric); // read in the word "degrees"
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}


void WsfTerrainPathFinder::GeneratePathFinderLight()
{
   std::map<std::string, int> gridMap;

   std::ofstream outPathFinderBin(mRQTFile.c_str(), std::ios::binary | std::ios::out);
   if (outPathFinderBin.is_open())
   {
      int numNodes = 0;
      // loop through the grid and get a count of marked nodes...
      for (int x = 0; x < mXSize; ++x)
      {
         for (int y = 0; y < mYSize; ++y)
         {
            WsfPFNode& p = (WsfPFNode&)*GetGrid(x, y);
            if (p.getMarked() && p.mWeight != std::numeric_limits<double>::max())
            {
               ++numNodes;
            }
         }
      }

      outPathFinderBin.write(reinterpret_cast<char*>(&numNodes), sizeof(int));

      // loop through the grid and write out all nodes that are marked
      int idx = 0;
      for (int x = 0; x < mXSize; ++x)
      {
         for (int y = 0; y < mYSize; ++y)
         {
            WsfPFNode& p = (WsfPFNode&)*GetGrid(x, y);
            if (p.getMarked() && p.mWeight != std::numeric_limits<double>::max())
            {
               double locLLA[3];
               p.mLoc.GetLocationLLA(locLLA[0], locLLA[1], locLLA[2]);

               // Write out the Binary file
               outPathFinderBin.write(reinterpret_cast<char*>(&p.mX), sizeof(int));
               outPathFinderBin.write(reinterpret_cast<char*>(&p.mY), sizeof(int));
               outPathFinderBin.write(reinterpret_cast<char*>(&p.mNormalAngle), sizeof(float));
               outPathFinderBin.write(reinterpret_cast<char*>(&p.mBaseWeight), sizeof(float));
               outPathFinderBin.write(reinterpret_cast<char*>(&p.mWeight), sizeof(double));
               outPathFinderBin.write(reinterpret_cast<char*>(&locLLA[0]), sizeof(double));
               outPathFinderBin.write(reinterpret_cast<char*>(&locLLA[1]), sizeof(double));
               outPathFinderBin.write(reinterpret_cast<char*>(&locLLA[2]), sizeof(double));

               char buff[100];
               sprintf(buff, "%li,%li", p.mX, p.mY);
               std::string mapKey(buff);
               gridMap[mapKey] = idx++;
            }
         }
      }

      // next write out all the edges...
      node_iterator nodeIter;
      for (nodeIter = begin(); nodeIter != end(); ++nodeIter)
      {
         if (nodeIter->first.mMarkedPt && nodeIter->first.mWeight != std::numeric_limits<double>::max())
         {
            for (const_edge_iterator edgeIter = nodeIter->second->begin(); edgeIter != nodeIter->second->end(); ++edgeIter)
            {
               // if the edge is connected to an invalid node, do not add it...
               if (!edgeIter.from()->first.mMarkedPt ||
                   edgeIter.from()->first.mWeight == std::numeric_limits<double>::max() ||
                   !edgeIter.to()->first.mMarkedPt || edgeIter.to()->first.mWeight == std::numeric_limits<double>::max())
               {
                  continue;
               }
               else
               {
                  char buff[100];
                  sprintf(buff, "%li,%li", edgeIter.from()->first.mX, edgeIter.from()->first.mY);
                  outPathFinderBin.write(reinterpret_cast<char*>(&gridMap[buff]), sizeof(int));
                  sprintf(buff, "%li,%li", edgeIter.to()->first.mX, edgeIter.to()->first.mY);
                  outPathFinderBin.write(reinterpret_cast<char*>(&gridMap[buff]), sizeof(int));
               }
            }
         }
      }
   }
   outPathFinderBin.close();

   if (!mNavMeshZonesFile.empty())
   {
      WriteOutAsZones();
   }
}

void WsfTerrainPathFinder::WriteOutAsZones()
{
   std::vector<sTerrainTriCell> TriVec;
   std::ofstream                outZoneFile(mNavMeshZonesFile.c_str(), std::ios::out);
   node_iterator                nodeIter;
   int                          nodeNumber = 0;
   for (nodeIter = begin(); nodeIter != end(); ++nodeIter)
   {
      if (nodeIter->first.mMarkedPt && nodeIter->first.mWeight != std::numeric_limits<double>::max())
      {
         ut::log::info() << "Processing node " << nodeNumber;
         nodeNumber++;

         double    locLLA[3];
         WsfPFNode curNode = nodeIter->first;

         curNode.mLoc.GetLocationLLA(locLLA[0], locLLA[1], locLLA[2]);

         for (const_edge_iterator edgeIter = nodeIter->second->begin(); edgeIter != nodeIter->second->end(); ++edgeIter)
         {
            // if the edge is connected to an invalid node, do not add it...
            if (!edgeIter.from()->first.mMarkedPt || edgeIter.from()->first.mWeight == std::numeric_limits<double>::max() ||
                !edgeIter.to()->first.mMarkedPt || edgeIter.to()->first.mWeight == std::numeric_limits<double>::max())
            {
               continue;
            }
            else
            {
               if (curNode == edgeIter.from()->first)
               {
                  WsfPFNode toNode = edgeIter.to()->first;
                  toNode.mLoc.GetLocationLLA(locLLA[0], locLLA[1], locLLA[2]);

                  // check to see if any of the nodes "toNode" is connected to are also connected to "curNode"
                  // if that is the case create a "Tri" out of the three points

                  std::vector<WsfPFNode*>::iterator neighborIter;
                  neighborIter = curNode.mNeighbors.begin();
                  while (neighborIter != curNode.mNeighbors.end())
                  {
                     WsfPFNode*                        curNodeNeighborPtr = *neighborIter;
                     std::vector<WsfPFNode*>::iterator toNodeNeighborIter;
                     toNodeNeighborIter = toNode.mNeighbors.begin();
                     while (toNodeNeighborIter != toNode.mNeighbors.end())
                     {
                        WsfPFNode* toNodeNeighborPtr = *toNodeNeighborIter;
                        if (toNodeNeighborPtr != &toNode && toNodeNeighborPtr != &curNode &&
                            toNodeNeighborPtr == curNodeNeighborPtr)
                        {
                           sTerrainTriCell newTri;
                           sTerrainLatLon  Pt1;
                           Pt1.Lat = curNode.mLoc.GetLat();
                           Pt1.Lon = curNode.mLoc.GetLon();

                           sTerrainLatLon Pt2;
                           Pt2.Lat = toNode.mLoc.GetLat();
                           Pt2.Lon = toNode.mLoc.GetLon();

                           sTerrainLatLon Pt3;
                           Pt3.Lat = toNodeNeighborPtr->mLoc.GetLat();
                           Pt3.Lon = toNodeNeighborPtr->mLoc.GetLon();

                           newTri.Pt1 = Pt1;
                           newTri.Pt2 = Pt2;
                           newTri.Pt3 = Pt3;

                           // now iterate through the tri list and make sure this one does not exist.
                           bool found      = false;
                           bool intersects = false;
                           for (sTerrainTriCell curTri : TriVec)
                           {
                              bool found1 = false;
                              bool found2 = false;
                              bool found3 = false;
                              if (ComparePoints(newTri.Pt1, curTri.Pt1) || ComparePoints(newTri.Pt1, curTri.Pt2) ||
                                  ComparePoints(newTri.Pt1, curTri.Pt3))
                              {
                                 found1 = true;
                              }
                              if (ComparePoints(newTri.Pt2, curTri.Pt1) || ComparePoints(newTri.Pt2, curTri.Pt2) ||
                                  ComparePoints(newTri.Pt2, curTri.Pt3))
                              {
                                 found2 = true;
                              }
                              if (ComparePoints(newTri.Pt3, curTri.Pt1) || ComparePoints(newTri.Pt3, curTri.Pt2) ||
                                  ComparePoints(newTri.Pt3, curTri.Pt3))
                              {
                                 found3 = true;
                              }
                              found = found1 & found2 & found3;
                              if (found)
                              {
                                 break;
                              }

                              // now create line segments out of the points on each triangle, check for line segment intersections
                              double pointA[3] = {newTri.Pt1.Lat, newTri.Pt1.Lon, 0.0};
                              double pointB[3] = {newTri.Pt2.Lat, newTri.Pt2.Lon, 0.0};
                              double pointC[3] = {newTri.Pt3.Lat, newTri.Pt3.Lon, 0.0};
                              double pointD[3] = {curTri.Pt1.Lat, curTri.Pt1.Lon, 0.0};
                              double pointE[3] = {curTri.Pt2.Lat, curTri.Pt2.Lon, 0.0};
                              double pointF[3] = {curTri.Pt3.Lat, curTri.Pt3.Lon, 0.0};

                              UtLineSegment newTripointAtoB;
                              UtLineSegment newTripointAtoC;
                              UtLineSegment newTripointBtoC;

                              UtLineSegment curTripointDtoE;
                              UtLineSegment curTripointDtoF;
                              UtLineSegment curTripointEtoF;

                              newTripointAtoB.SetBeginPoint(pointA);
                              newTripointAtoB.SetEndPoint(pointB);

                              newTripointAtoC.SetBeginPoint(pointA);
                              newTripointAtoC.SetEndPoint(pointC);

                              newTripointBtoC.SetBeginPoint(pointB);
                              newTripointBtoC.SetEndPoint(pointC);

                              curTripointDtoE.SetBeginPoint(pointD);
                              curTripointDtoE.SetEndPoint(pointE);

                              curTripointDtoF.SetBeginPoint(pointD);
                              curTripointDtoF.SetEndPoint(pointF);

                              curTripointEtoF.SetBeginPoint(pointE);
                              curTripointEtoF.SetEndPoint(pointF);


                              UtVec3d                            intersectionPt;
                              UtLineSegment::LINE_CLASSIFICATION intersectType;

                              // AB -> DE
                              intersectType = newTripointAtoB.Intersects(&curTripointDtoE, &intersectionPt);
                              if (intersectType == UtLineSegment::SEGMENTS_INTERSECT)
                              {
                                 double data[3];
                                 intersectionPt.Get(data);
                                 sTerrainLatLon realIntersectPt;
                                 realIntersectPt.Lat = data[0];
                                 realIntersectPt.Lon = data[1];

                                 if (!ComparePoints(newTri.Pt1, realIntersectPt) &&
                                     !ComparePoints(newTri.Pt2, realIntersectPt) &&
                                     !ComparePoints(curTri.Pt1, realIntersectPt) &&
                                     !ComparePoints(curTri.Pt2, realIntersectPt))
                                 {
                                    intersects = true;
                                    break;
                                 }
                              }

                              // AB -> DF
                              intersectType = newTripointAtoB.Intersects(&curTripointDtoF, &intersectionPt);
                              if (intersectType == UtLineSegment::SEGMENTS_INTERSECT)
                              {
                                 double data[3];
                                 intersectionPt.Get(data);
                                 sTerrainLatLon realIntersectPt;
                                 realIntersectPt.Lat = data[0];
                                 realIntersectPt.Lon = data[1];

                                 if (!ComparePoints(newTri.Pt1, realIntersectPt) &&
                                     !ComparePoints(newTri.Pt2, realIntersectPt) &&
                                     !ComparePoints(curTri.Pt1, realIntersectPt) &&
                                     !ComparePoints(curTri.Pt3, realIntersectPt))
                                 {
                                    intersects = true;
                                    break;
                                 }
                              }

                              // AB -> EF
                              intersectType = newTripointAtoB.Intersects(&curTripointEtoF, &intersectionPt);
                              if (intersectType == UtLineSegment::SEGMENTS_INTERSECT)
                              {
                                 double data[3];
                                 intersectionPt.Get(data);
                                 sTerrainLatLon realIntersectPt;
                                 realIntersectPt.Lat = data[0];
                                 realIntersectPt.Lon = data[1];

                                 if (!ComparePoints(newTri.Pt1, realIntersectPt) &&
                                     !ComparePoints(newTri.Pt2, realIntersectPt) &&
                                     !ComparePoints(curTri.Pt2, realIntersectPt) &&
                                     !ComparePoints(curTri.Pt3, realIntersectPt))
                                 {
                                    intersects = true;
                                    break;
                                 }
                              }

                              // AC -> DE
                              intersectType = newTripointAtoC.Intersects(&curTripointDtoE, &intersectionPt);
                              if (intersectType == UtLineSegment::SEGMENTS_INTERSECT)
                              {
                                 double data[3];
                                 intersectionPt.Get(data);
                                 sTerrainLatLon realIntersectPt;
                                 realIntersectPt.Lat = data[0];
                                 realIntersectPt.Lon = data[1];

                                 if (!ComparePoints(newTri.Pt1, realIntersectPt) &&
                                     !ComparePoints(newTri.Pt3, realIntersectPt) &&
                                     !ComparePoints(curTri.Pt1, realIntersectPt) &&
                                     !ComparePoints(curTri.Pt2, realIntersectPt))
                                 {
                                    intersects = true;
                                    break;
                                 }
                              }

                              // AC -> DF
                              intersectType = newTripointAtoC.Intersects(&curTripointDtoF, &intersectionPt);
                              if (intersectType == UtLineSegment::SEGMENTS_INTERSECT)
                              {
                                 double data[3];
                                 intersectionPt.Get(data);
                                 sTerrainLatLon realIntersectPt;
                                 realIntersectPt.Lat = data[0];
                                 realIntersectPt.Lon = data[1];

                                 if (!ComparePoints(newTri.Pt1, realIntersectPt) &&
                                     !ComparePoints(newTri.Pt3, realIntersectPt) &&
                                     !ComparePoints(curTri.Pt1, realIntersectPt) &&
                                     !ComparePoints(curTri.Pt3, realIntersectPt))
                                 {
                                    intersects = true;
                                    break;
                                 };
                              }

                              // AC -> EF
                              intersectType = newTripointAtoC.Intersects(&curTripointEtoF, &intersectionPt);
                              if (intersectType == UtLineSegment::SEGMENTS_INTERSECT)
                              {
                                 double data[3];
                                 intersectionPt.Get(data);
                                 sTerrainLatLon realIntersectPt;
                                 realIntersectPt.Lat = data[0];
                                 realIntersectPt.Lon = data[1];

                                 if (!ComparePoints(newTri.Pt1, realIntersectPt) &&
                                     !ComparePoints(newTri.Pt3, realIntersectPt) &&
                                     !ComparePoints(curTri.Pt2, realIntersectPt) &&
                                     !ComparePoints(curTri.Pt3, realIntersectPt))
                                 {
                                    intersects = true;
                                    break;
                                 }
                              }

                              // BC -> DE
                              intersectType = newTripointBtoC.Intersects(&curTripointDtoE, &intersectionPt);
                              if (intersectType == UtLineSegment::SEGMENTS_INTERSECT)
                              {
                                 double data[3];
                                 intersectionPt.Get(data);
                                 sTerrainLatLon realIntersectPt;
                                 realIntersectPt.Lat = data[0];
                                 realIntersectPt.Lon = data[1];

                                 if (!ComparePoints(newTri.Pt2, realIntersectPt) &&
                                     !ComparePoints(newTri.Pt3, realIntersectPt) &&
                                     !ComparePoints(curTri.Pt1, realIntersectPt) &&
                                     !ComparePoints(curTri.Pt2, realIntersectPt))
                                 {
                                    intersects = true;
                                    break;
                                 }
                              }

                              // BC -> DF
                              intersectType = newTripointBtoC.Intersects(&curTripointDtoF, &intersectionPt);
                              if (intersectType == UtLineSegment::SEGMENTS_INTERSECT)
                              {
                                 double data[3];
                                 intersectionPt.Get(data);
                                 sTerrainLatLon realIntersectPt;
                                 realIntersectPt.Lat = data[0];
                                 realIntersectPt.Lon = data[1];

                                 if (!ComparePoints(newTri.Pt2, realIntersectPt) &&
                                     !ComparePoints(newTri.Pt3, realIntersectPt) &&
                                     !ComparePoints(curTri.Pt1, realIntersectPt) &&
                                     !ComparePoints(curTri.Pt3, realIntersectPt))
                                 {
                                    intersects = true;
                                    break;
                                 }
                              }

                              // BC -> EF
                              intersectType = newTripointBtoC.Intersects(&curTripointEtoF, &intersectionPt);
                              if (intersectType == UtLineSegment::SEGMENTS_INTERSECT)
                              {
                                 double data[3];
                                 intersectionPt.Get(data);
                                 sTerrainLatLon realIntersectPt;
                                 realIntersectPt.Lat = data[0];
                                 realIntersectPt.Lon = data[1];

                                 if (!ComparePoints(newTri.Pt2, realIntersectPt) &&
                                     !ComparePoints(newTri.Pt3, realIntersectPt) &&
                                     !ComparePoints(curTri.Pt2, realIntersectPt) &&
                                     !ComparePoints(curTri.Pt3, realIntersectPt))
                                 {
                                    intersects = true;
                                    break;
                                 }
                              }
                           }

                           // if it is a new tri, add it to the list
                           if (!found && !intersects)
                           {
                              TriVec.push_back(newTri);
                           }
                        }
                        ++toNodeNeighborIter;
                     }
                     ++neighborIter;
                  }
               }
               else
               {
                  WsfPFNode toNode = edgeIter.from()->first;
                  toNode.mLoc.GetLocationLLA(locLLA[0], locLLA[1], locLLA[2]);

                  // check to see if any of the nodes "toNode" is connected to are also connected to "curNode"
                  // if that is the case create a "Tri" out of the three points

                  std::vector<WsfPFNode*>::iterator neighborIter;
                  neighborIter = curNode.mNeighbors.begin();
                  while (neighborIter != curNode.mNeighbors.end())
                  {
                     WsfPFNode*                        curNodeNeighborPtr = *neighborIter;
                     std::vector<WsfPFNode*>::iterator toNodeNeighborIter;
                     toNodeNeighborIter = toNode.mNeighbors.begin();
                     while (toNodeNeighborIter != toNode.mNeighbors.end())
                     {
                        WsfPFNode* toNodeNeighborPtr = *toNodeNeighborIter;
                        if (toNodeNeighborPtr != &toNode && toNodeNeighborPtr != &curNode &&
                            toNodeNeighborPtr == curNodeNeighborPtr)
                        {
                           sTerrainTriCell newTri;
                           sTerrainLatLon  Pt1;
                           Pt1.Lat = curNode.mLoc.GetLat();
                           Pt1.Lon = curNode.mLoc.GetLon();

                           sTerrainLatLon Pt2;
                           Pt2.Lat = toNode.mLoc.GetLat();
                           Pt2.Lon = toNode.mLoc.GetLon();

                           sTerrainLatLon Pt3;
                           Pt3.Lat = toNodeNeighborPtr->mLoc.GetLat();
                           Pt3.Lon = toNodeNeighborPtr->mLoc.GetLon();

                           newTri.Pt1 = Pt1;
                           newTri.Pt2 = Pt2;
                           newTri.Pt3 = Pt3;

                           // now iterate through the tri list and make sure this one does not exist.
                           bool found      = false;
                           bool intersects = false;
                           for (sTerrainTriCell curTri : TriVec)
                           {
                              bool found1 = false;
                              bool found2 = false;
                              bool found3 = false;
                              if (ComparePoints(newTri.Pt1, curTri.Pt1) || ComparePoints(newTri.Pt1, curTri.Pt2) ||
                                  ComparePoints(newTri.Pt1, curTri.Pt3))
                              {
                                 found1 = true;
                              }

                              if (ComparePoints(newTri.Pt2, curTri.Pt1) || ComparePoints(newTri.Pt2, curTri.Pt2) ||
                                  ComparePoints(newTri.Pt2, curTri.Pt3))
                              {
                                 found2 = true;
                              }

                              if (ComparePoints(newTri.Pt3, curTri.Pt1) || ComparePoints(newTri.Pt3, curTri.Pt2) ||
                                  ComparePoints(newTri.Pt3, curTri.Pt3))
                              {
                                 found3 = true;
                              }
                              found = found1 & found2 & found3;
                              if (found)
                              {
                                 break;
                              }

                              // now create line segments out of the points on each triangle, check for line segment intersections
                              double pointA[3] = {newTri.Pt1.Lat, newTri.Pt1.Lon, 0.0};
                              double pointB[3] = {newTri.Pt2.Lat, newTri.Pt2.Lon, 0.0};
                              double pointC[3] = {newTri.Pt3.Lat, newTri.Pt3.Lon, 0.0};

                              double pointD[3] = {curTri.Pt1.Lat, curTri.Pt1.Lon, 0.0};
                              double pointE[3] = {curTri.Pt2.Lat, curTri.Pt2.Lon, 0.0};
                              double pointF[3] = {curTri.Pt3.Lat, curTri.Pt3.Lon, 0.0};

                              UtLineSegment newTripointAtoB;
                              UtLineSegment newTripointAtoC;
                              UtLineSegment newTripointBtoC;

                              UtLineSegment curTripointDtoE;
                              UtLineSegment curTripointDtoF;
                              UtLineSegment curTripointEtoF;

                              newTripointAtoB.SetBeginPoint(pointA);
                              newTripointAtoB.SetEndPoint(pointB);

                              newTripointAtoC.SetBeginPoint(pointA);
                              newTripointAtoC.SetEndPoint(pointC);

                              newTripointBtoC.SetBeginPoint(pointB);
                              newTripointBtoC.SetEndPoint(pointC);

                              curTripointDtoE.SetBeginPoint(pointD);
                              curTripointDtoE.SetEndPoint(pointE);

                              curTripointDtoF.SetBeginPoint(pointD);
                              curTripointDtoF.SetEndPoint(pointF);

                              curTripointEtoF.SetBeginPoint(pointE);
                              curTripointEtoF.SetEndPoint(pointF);

                              UtVec3d                            intersectionPt;
                              UtLineSegment::LINE_CLASSIFICATION intersectType;
                              // AB -> DF
                              intersectType = newTripointAtoB.Intersects(&curTripointDtoF, &intersectionPt);
                              if (intersectType == UtLineSegment::SEGMENTS_INTERSECT)
                              {
                                 double data[3];
                                 intersectionPt.Get(data);
                                 sTerrainLatLon realIntersectPt;
                                 realIntersectPt.Lat = data[0];
                                 realIntersectPt.Lon = data[1];

                                 if (!ComparePoints(newTri.Pt1, realIntersectPt) &&
                                     !ComparePoints(newTri.Pt2, realIntersectPt) &&
                                     !ComparePoints(curTri.Pt1, realIntersectPt) &&
                                     !ComparePoints(curTri.Pt3, realIntersectPt))
                                 {
                                    intersects = true;
                                    break;
                                 }
                              }

                              // AB -> EF
                              intersectType = newTripointAtoB.Intersects(&curTripointEtoF, &intersectionPt);
                              if (intersectType == UtLineSegment::SEGMENTS_INTERSECT)
                              {
                                 double data[3];
                                 intersectionPt.Get(data);
                                 sTerrainLatLon realIntersectPt;
                                 realIntersectPt.Lat = data[0];
                                 realIntersectPt.Lon = data[1];

                                 if (!ComparePoints(newTri.Pt1, realIntersectPt) &&
                                     !ComparePoints(newTri.Pt2, realIntersectPt) &&
                                     !ComparePoints(curTri.Pt2, realIntersectPt) &&
                                     !ComparePoints(curTri.Pt3, realIntersectPt))
                                 {
                                    intersects = true;
                                    break;
                                 }
                              }

                              // AC -> DE
                              intersectType = newTripointAtoC.Intersects(&curTripointDtoE, &intersectionPt);
                              if (intersectType == UtLineSegment::SEGMENTS_INTERSECT)
                              {
                                 double data[3];
                                 intersectionPt.Get(data);
                                 sTerrainLatLon realIntersectPt;
                                 realIntersectPt.Lat = data[0];
                                 realIntersectPt.Lon = data[1];

                                 if (!ComparePoints(newTri.Pt1, realIntersectPt) &&
                                     !ComparePoints(newTri.Pt3, realIntersectPt) &&
                                     !ComparePoints(curTri.Pt1, realIntersectPt) &&
                                     !ComparePoints(curTri.Pt2, realIntersectPt))
                                 {
                                    intersects = true;
                                    break;
                                 }
                              }


                              // AC -> DF
                              intersectType = newTripointAtoC.Intersects(&curTripointDtoF, &intersectionPt);
                              if (intersectType == UtLineSegment::SEGMENTS_INTERSECT)
                              {
                                 double data[3];
                                 intersectionPt.Get(data);
                                 sTerrainLatLon realIntersectPt;
                                 realIntersectPt.Lat = data[0];
                                 realIntersectPt.Lon = data[1];

                                 if (!ComparePoints(newTri.Pt1, realIntersectPt) &&
                                     !ComparePoints(newTri.Pt3, realIntersectPt) &&
                                     !ComparePoints(curTri.Pt1, realIntersectPt) &&
                                     !ComparePoints(curTri.Pt3, realIntersectPt))
                                 {
                                    intersects = true;
                                    break;
                                 };
                              }

                              // AC -> EF
                              intersectType = newTripointAtoC.Intersects(&curTripointEtoF, &intersectionPt);
                              if (intersectType == UtLineSegment::SEGMENTS_INTERSECT)
                              {
                                 double data[3];
                                 intersectionPt.Get(data);
                                 sTerrainLatLon realIntersectPt;
                                 realIntersectPt.Lat = data[0];
                                 realIntersectPt.Lon = data[1];

                                 if (!ComparePoints(newTri.Pt1, realIntersectPt) &&
                                     !ComparePoints(newTri.Pt3, realIntersectPt) &&
                                     !ComparePoints(curTri.Pt2, realIntersectPt) &&
                                     !ComparePoints(curTri.Pt3, realIntersectPt))
                                 {
                                    intersects = true;
                                    break;
                                 }
                              }

                              // BC -> DE
                              intersectType = newTripointBtoC.Intersects(&curTripointDtoE, &intersectionPt);
                              if (intersectType == UtLineSegment::SEGMENTS_INTERSECT)
                              {
                                 double data[3];
                                 intersectionPt.Get(data);
                                 sTerrainLatLon realIntersectPt;
                                 realIntersectPt.Lat = data[0];
                                 realIntersectPt.Lon = data[1];

                                 if (!ComparePoints(newTri.Pt2, realIntersectPt) &&
                                     !ComparePoints(newTri.Pt3, realIntersectPt) &&
                                     !ComparePoints(curTri.Pt1, realIntersectPt) &&
                                     !ComparePoints(curTri.Pt2, realIntersectPt))
                                 {
                                    intersects = true;
                                    break;
                                 }
                              }


                              // BC -> DF
                              intersectType = newTripointBtoC.Intersects(&curTripointDtoF, &intersectionPt);
                              if (intersectType == UtLineSegment::SEGMENTS_INTERSECT)
                              {
                                 double data[3];
                                 intersectionPt.Get(data);
                                 sTerrainLatLon realIntersectPt;
                                 realIntersectPt.Lat = data[0];
                                 realIntersectPt.Lon = data[1];

                                 if (!ComparePoints(newTri.Pt2, realIntersectPt) &&
                                     !ComparePoints(newTri.Pt3, realIntersectPt) &&
                                     !ComparePoints(curTri.Pt1, realIntersectPt) &&
                                     !ComparePoints(curTri.Pt3, realIntersectPt))
                                 {
                                    intersects = true;
                                    break;
                                 }
                              }

                              // BC -> EF
                              intersectType = newTripointBtoC.Intersects(&curTripointEtoF, &intersectionPt);
                              if (intersectType == UtLineSegment::SEGMENTS_INTERSECT)
                              {
                                 double data[3];
                                 intersectionPt.Get(data);
                                 sTerrainLatLon realIntersectPt;
                                 realIntersectPt.Lat = data[0];
                                 realIntersectPt.Lon = data[1];

                                 if (!ComparePoints(newTri.Pt2, realIntersectPt) &&
                                     !ComparePoints(newTri.Pt3, realIntersectPt) &&
                                     !ComparePoints(curTri.Pt2, realIntersectPt) &&
                                     !ComparePoints(curTri.Pt3, realIntersectPt))
                                 {
                                    intersects = true;
                                    break;
                                 }
                              }
                           }

                           // if it is a new tri, add it to the list
                           if (!found && !intersects)
                           {
                              TriVec.push_back(newTri);
                           }
                        }
                        ++toNodeNeighborIter;
                     }
                     ++neighborIter;
                  }
               }
            }
         }
      }
   }

   // print out all the points
   outZoneFile << std::endl << std::endl << std::endl;
   int zoneNumber = 0;
   for (sTerrainTriCell curTri : TriVec)
   {
      std::string       zoneName = "zone";
      std::stringstream ss;
      ss << zoneNumber;
      zoneName.append(ss.str());
      ++zoneNumber;

      outZoneFile << "zone " << zoneName << std::endl;
      outZoneFile << "   polygonal" << std::endl;
      outZoneFile << "   lat_lon" << std::endl;
      std::string Lat = ConvertToDegMinSec(curTri.Pt1.Lat);
      if (curTri.Pt1.Lat > 0)
      {
         Lat.append("n");
      }
      else
      {
         Lat.append("s");
      }

      std::string Lon = ConvertToDegMinSec(curTri.Pt1.Lon);
      if (curTri.Pt1.Lon > 0)
      {
         Lon.append("e");
      }
      else
      {
         Lon.append("w");
      }
      outZoneFile << "   point " << Lat << " " << Lon << " " << std::endl;


      Lat = ConvertToDegMinSec(curTri.Pt2.Lat);
      if (curTri.Pt2.Lat > 0)
      {
         Lat.append("n");
      }
      else
      {
         Lat.append("s");
      }

      Lon = ConvertToDegMinSec(curTri.Pt2.Lon);
      if (curTri.Pt2.Lon > 0)
      {
         Lon.append("e");
      }
      else
      {
         Lon.append("w");
      }
      outZoneFile << "   point " << Lat << " " << Lon << " " << std::endl;

      Lat = ConvertToDegMinSec(curTri.Pt3.Lat);
      if (curTri.Pt3.Lat > 0)
      {
         Lat.append("n");
      }
      else
      {
         Lat.append("s");
      }

      Lon = ConvertToDegMinSec(curTri.Pt3.Lon);
      if (curTri.Pt3.Lon > 0)
      {
         Lon.append("e");
      }
      else
      {
         Lon.append("w");
      }
      outZoneFile << "   point " << Lat << " " << Lon << " " << std::endl;
      outZoneFile << "   #fill_zone" << std::endl;
      outZoneFile << "end_zone " << std::endl;
      outZoneFile << std::endl;
   }
   outZoneFile.close();
}

bool WsfTerrainPathFinder::ComparePoints(sTerrainLatLon aPt1, sTerrainLatLon aPt2)
{
   if (aPt1.Lat == aPt2.Lat && aPt1.Lon == aPt2.Lon)
   {
      return true;
   }
   return false;
}

std::string WsfTerrainPathFinder::ConvertToDegMinSec(double aLoc)
{
   std::string       retString = "";
   std::stringstream ss;
   ss << aLoc;

   size_t decimalLoc;
   decimalLoc = ss.str().find(".");

   std::string degreesStringFirst = ss.str().substr(0, decimalLoc);

   int degrees = atoi(degreesStringFirst.c_str());
   if (degrees < 0.0)
   {
      aLoc *= -1;
      degrees *= -1;
   }
   std::stringstream ss0;
   ss0 << degrees;
   std::string degreesString = ss0.str().substr(0, decimalLoc);

   aLoc -= degrees;

   // now get the minutes
   aLoc *= 60;

   std::stringstream ss2;
   ss2 << aLoc;
   decimalLoc = ss2.str().find(".");
   std::string minutesString;
   if (aLoc < 10)
   {
      minutesString = "0";
   }
   minutesString.append(ss2.str().substr(0, decimalLoc));
   degrees = atoi(minutesString.c_str());
   aLoc -= degrees;


   // and finally the seconds
   aLoc *= 60;
   std::stringstream ss3;
   if (aLoc < 0)
   {
      aLoc = 0;
   }
   ss3 << aLoc;
   decimalLoc = ss3.str().find(".");

   std::string secondsString;
   if (aLoc < 10)
   {
      secondsString = "0";
   }

   secondsString.append(ss3.str().substr(0, decimalLoc));

   retString.append(degreesString);
   retString.append(":");
   retString.append(minutesString);
   retString.append(":");
   retString.append(secondsString);

   return retString;
}

void WsfTerrainPathFinder::LoadGridInfo()
{
   assert(mRQTFile != "");

   std::ifstream inPathFinderBin(mRQTFile.c_str(), std::ios::binary);

   if (inPathFinderBin.is_open())
   {
      int numNodes;
      inPathFinderBin.read(reinterpret_cast<char*>(&numNodes), sizeof(int));

      // get the nodes first and generate the grid
      mGrid = std::vector<node_iterator>(numNodes);

      for (int i = 0; i < numNodes; ++i)
      {
         // first value is x position
         int x;
         inPathFinderBin.read(reinterpret_cast<char*>(&x), sizeof(int));

         // second value is the y position
         int y;
         inPathFinderBin.read(reinterpret_cast<char*>(&y), sizeof(int));

         // third value is the normal Angle
         float normalAngle;
         inPathFinderBin.read(reinterpret_cast<char*>(&normalAngle), sizeof(float));


         // fourth value is the baseweight
         float baseWeight;
         inPathFinderBin.read(reinterpret_cast<char*>(&baseWeight), sizeof(float));

         // fifth value is the weight
         double weight;
         inPathFinderBin.read(reinterpret_cast<char*>(&weight), sizeof(double));

         // sixth - eighth value is the Lat/Lon/Alt position
         double LLA[3];
         inPathFinderBin.read(reinterpret_cast<char*>(&LLA[0]), sizeof(double));
         inPathFinderBin.read(reinterpret_cast<char*>(&LLA[1]), sizeof(double));
         inPathFinderBin.read(reinterpret_cast<char*>(&LLA[2]), sizeof(double));

         WsfGeoPoint tempPoint{LLA[0], LLA[1], LLA[2]};
         WsfPFNode*  tempNodePtr = new WsfPFNode(x, y, baseWeight, tempPoint); // get height from the terrain
         tempNodePtr->setMarked(true);
         tempNodePtr->mWeight      = weight;
         tempNodePtr->mNormalAngle = normalAngle;

         insert(*tempNodePtr);
         SetGridAbsolute(i, find(*tempNodePtr));
      }

      // now it's time to get the edges...

      WsfGeoPoint tmpGeo1(0, 0, 0);
      WsfGeoPoint tmpGeo2(0, 0, 0);
      WsfPFNode   aNode(0, 0, 0, tmpGeo1);
      WsfPFNode   aNode2(0, 0, 0, tmpGeo2);
      while (!inPathFinderBin.eof())
      {
         // first value is the absolute index of the first node
         int a;
         inPathFinderBin.read(reinterpret_cast<char*>(&a), sizeof(int));

         // second value is the absolute index of the second node
         int b;
         inPathFinderBin.read(reinterpret_cast<char*>(&b), sizeof(int));

         aNode                 = (WsfPFNode&)*GetGridAbsolute(a);
         WsfPFNode* tmpNodePtr = &aNode;

         aNode2                 = (WsfPFNode&)*GetGridAbsolute(b);
         WsfPFNode* tmpNode2Ptr = &aNode2;
         WsfPFEdge  tempEdge(tmpNodePtr, tmpNode2Ptr);

         insert_edge(GetGridAbsolute(a), GetGridAbsolute(b), tempEdge);
         WsfPFNode& node1 = (WsfPFNode&)*GetGridAbsolute(a);
         WsfPFNode& node2 = (WsfPFNode&)*GetGridAbsolute(b);
         node1.AddNeighbor(&node2);
         node2.AddNeighbor(&node1);
      }
   }
   inPathFinderBin.close();
}


bool WsfTerrainPathFinder::shortest_path(const_node_iterator aSrcNodeIter,
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
