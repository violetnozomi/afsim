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

#ifndef WSFSHORTESTPATH_HPP
#define WSFSHORTESTPATH_HPP

#include "wsf_export.h"

#include <vector>

/**
   Determine the least cost path between nodes in a graph.

   This class uses the Dijkstra algorithm to determine the least cost path
   in a graph.  The graph is defined by a cost matrix with the following
   attributes:

   - A matrix element [m][n] with a value greater that zero implies that node
     'm' is ADJACENT to node 'n'. This defines an 'edge' in the graph with
     endpoints 'm' and 'n'. The value of the element is the cost of going from
     node 'm' to node 'n'.
   - A matrix element [m][n] with a value less than zero implies that node 'm'
     is NOT ADJACENT to node 'n'.
   - The matrix must be square.
   - The matrix must be symmetric, i.e., element [m][n] must equal to [n][m].
     (This means the that the cost of the path from 'a' to 'b' will be equal to
     the cost from 'b' to 'a').
*/

class WSF_EXPORT WsfShortestPath
{
public:
   typedef std::vector<std::vector<int>> CostMatrix;

   WsfShortestPath();

   bool Initialize(const CostMatrix& aCostMatrix);

   void FindShortestPath(unsigned int aFromNode, unsigned int aToNode, int& aCost, std::vector<int>& aPath);

private:
   typedef std::vector<std::vector<int>> AdjacentNodes;

   void UpdatePathMatrix(const std::vector<int>& aPath);

   //! The number of nodes in the network.
   unsigned int mNodeCount;

   //! The cost matrix entry mCostMatrix[from][to] is the 'cost' associated with going
   //! from node 'from' to node 'to'.
   CostMatrix mCostMatrix;

   //! The adjacency matrix indicates which nodes are adjacent to a given node.
   //! mAdjacentNodes[n] is the set of nodes that are adjacent to the node 'n'.
   AdjacentNodes mAdjacentNodes;
};

#endif
