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

#include "WsfShortestPath.hpp"

#include <algorithm>
#include <climits>
#include <cmath>

#include "UtCast.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"

// NOTE: The original version of this class cached the computed path in case it was
// requested again.  The performance benefit for small networks is negligible while
// the overhead of storing the computed paths became big for large networks.
//
// Performance improvements were made that basically eliminated the need to cache
// the results. That is not to say that caching wouldn't help, but in the scheme of
// things it only improved typical application performance by a percent or so. Of
// course, if you did nothing but shortest path calls then it would be more of an
// issue, but then the typical application doesn't do that (If they want to cache
// the results, they can do it themselves).

WsfShortestPath::WsfShortestPath()
   : mNodeCount(0)
   , mCostMatrix()
   , mAdjacentNodes()
{
}

//! Initialize the shortest path algorithm.
//!
//! The cost matrix defines the cost of going between two ADJACENT nodes (the algorithm
//! also uses this to determine which nodes are adjacent). The matrix must have the
//! following properties:
//!
//! - It must be square (have the same number of rows and columns)
//! - Entry [m][n] defines the cost of going from node 'm' to node 'n', where nodes 'm'
//! and 'n' are adjacent.  If nodes 'm' and 'n' are not adjacent then this value must
//! be <= 0.
//! - It must be symmetric (A[m][n] == A[n][m].  The cost of going from 'm' to 'n' must be
//!  the same as the cost of going from 'n' to 'm'.
//!
//! @param aCostMatrix The cost matrix.
//! @returns 'true' if successful or 'false' if not.
bool WsfShortestPath::Initialize(const CostMatrix& aCostMatrix)
{
   unsigned int rowIndex;
   unsigned int colIndex;

   if (aCostMatrix.empty())
   {
      ut::log::error() << "WsfShortestPath: Cost Matrix is empty.";
      return false;
   }

   // Ensure the cost matrix is square.

   mNodeCount = static_cast<unsigned int>(aCostMatrix.size());
   for (rowIndex = 0; rowIndex < mNodeCount; ++rowIndex)
   {
      if (aCostMatrix[0].size() != aCostMatrix[rowIndex].size())
      {
         ut::log::error() << "WsfShortestPath: Cost Matrix is not square.";
         return false;
      }
   }

   // Dijkstra's algorithm assumes the edges are undirected.  Therefore we must
   // ensure the matrix is symmetric.

   for (rowIndex = 0; rowIndex < mNodeCount; ++rowIndex)
   {
      for (colIndex = 0; colIndex < rowIndex; ++colIndex)
      {
         if (aCostMatrix[rowIndex][colIndex] != aCostMatrix[colIndex][rowIndex])
         {
            ut::log::error() << "WsfShortestPath: Cost Matrix is not symmetric.";
            return false;
         }
      }
   }

   mCostMatrix = aCostMatrix;

   // Create the adjacent node table.  If a cost has been defined between two nodes
   // then they are adjacent.

   mAdjacentNodes.resize(mNodeCount);
   for (rowIndex = 0; rowIndex < mNodeCount; ++rowIndex)
   {
      for (colIndex = 0; colIndex < rowIndex; ++colIndex)
      {
         if (aCostMatrix[rowIndex][colIndex] > 0)
         {
            mAdjacentNodes[rowIndex].push_back(colIndex);
            mAdjacentNodes[colIndex].push_back(rowIndex);
         }
      }
   }

   return true;
}

// Find the shortest path between two nodes.  The return value 'aCost' is
// the cost of the route and 'aPath' in a vector of node points that
// define the path.

//! Find the shortest path between two nodes.
//!
//! @param aFromNode [input] The starting node.
//! @param aToNode   [input] The ending node.
//! @param aCost     [output] The cost of the path. This will be < 0 if a path could
//! not be generated.
//! @param aPath     [output] A vector of nodes that define the path. This will be empty
//! if a path could not be generated.
//!
//! @note Initialize() must be called before invoking this method.
void WsfShortestPath::FindShortestPath(unsigned int aFromNode, unsigned int aToNode, int& aCost, std::vector<int>& aPath)
{
   // Make sure the node values are valid.
   if ((aFromNode < 0U) || (aFromNode >= mNodeCount) || (aToNode < 0U) || (aToNode >= mNodeCount))
   {
      auto out = ut::log::error() << "Node indexing error in WsfShortestPath::FindShortestPath";
      out.AddNote() << "From Node: " << aFromNode;
      out.AddNote() << "To Note: " << aToNode;
      out.AddNote() << "Max: " << mNodeCount;
      aCost = 0;
      aPath.clear();
      return;
   }

   // If to == from then we're not going anywhere.

   if (aFromNode == aToNode)
   {
      aCost = 0;
      aPath.clear();
      return;
   }

   // We actually trace the route from the end to the beginning. This eliminates
   // the need to reverse the reconstructed route.

   // Initialize the current cost for each node.
   std::vector<int> cost(mNodeCount, std::numeric_limits<int>::max());
   cost[aToNode] = 0;
   std::vector<unsigned int> predecessor(mNodeCount, 0U);

   // Initialize the unprocessed node list.
   std::vector<unsigned int> unprocessed(mNodeCount);
   for (unsigned int i = 0U; i < mNodeCount; ++i)
   {
      unprocessed[i] = i;
   }

   auto nodeCount = mNodeCount;
   while (nodeCount != 0U)
   {
      // Search the remaining unprocessed nodes for the one with the least cost.
      auto minIndex = 0U;
      auto minNode  = unprocessed[0];
      auto minCost  = cost[minNode];
      for (auto i1 = 1U; i1 < nodeCount; ++i1)
      {
         auto curNode = unprocessed[i1];
         auto curCost = cost[curNode];
         if (curCost < minCost)
         {
            minCost  = curCost;
            minNode  = curNode;
            minIndex = i1;
         }
      }

      // When the Dijkstra algorithm is used with a graph where every node is somehow reachable from
      // every other node, the following will not occur.  However, we allow disconnections in the
      // net. The following check will break out when there are no more points that can be processed.
      if (minCost == std::numeric_limits<int>::max())
      {
         break;
      }

      // Remove the node with the least cost from the list of unprocessed nodes.  We simply move
      // the last entry over on top of the entry we want to remove and then reduce the list size by one.
      // This shortens the list with the minimum of movement.
      unprocessed[minIndex] = unprocessed[nodeCount - 1U];
      --nodeCount;

      // Update the cost of adjacent nodes.
      const std::vector<int>& adjacentNodes = mAdjacentNodes[minNode];
      for (auto adjNode : adjacentNodes)
      {
         auto newCost = minCost + mCostMatrix[minNode][adjNode];
         if (newCost < cost[adjNode])
         {
            cost[adjNode]        = newCost;
            predecessor[adjNode] = minNode;
         }
      }
   }

   // See if we made it to the target node. (See an earlier comment about disconnections)

   if (cost[aToNode] == std::numeric_limits<int>::max())
   {
      aPath.clear();
      aCost = -1;
   }

   // Recover the shortest path.

   std::vector<int> path;
   auto             curNode = aFromNode;
   while (curNode != aToNode)
   {
      if (cost[curNode] == std::numeric_limits<int>::max())
      {
         aPath.clear();
         aCost = -1;
         return;
      }
      path.push_back(ut::cast_to_int(curNode));
      curNode = predecessor[curNode];
   }
   path.push_back(ut::cast_to_int(curNode));

   aPath = path;
   aCost = cost[aFromNode];
}
