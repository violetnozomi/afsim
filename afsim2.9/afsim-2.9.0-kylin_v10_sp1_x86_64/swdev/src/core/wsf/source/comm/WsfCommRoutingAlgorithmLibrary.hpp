// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCOMMROUTINGALGORITHMLIBRARY_HPP
#define WSFCOMMROUTINGALGORITHMLIBRARY_HPP

#include "wsf_export.h"

#include "WsfCommGraph.hpp"

namespace wsf
{
namespace comm
{
//! This header provides some pre-defined routing algorithm classes for use
//! with protocol implementations for correct pathfinding of a WsfCommGraph
//! object. Users should extend this namespace for use of custom defined
//! algorithm classes.

//! The least hops class finds the shortest path based on the least amount
//! of edges traversed to the destination, regardless of edge cost.
class WSF_EXPORT LeastHops : public graph::GraphImpl::cost_func
{
public:
   LeastHops()           = default;
   ~LeastHops() override = default;

   double operator()(const graph::Edge& aEdge, const graph::Node& aSourceNode, const graph::Node& aDestinationNode) const override
   {
      if (!aEdge.IsEnabled())
      {
         return std::numeric_limits<double>::max();
      }
      return 1.0;
   }

   double operator()(const graph::Node& aSourceNode, const graph::Node& aDestinationNode) const override { return 0.0; }

   bool consider_node(const graph::Node& aNode) const override { return aNode.IsEnabled(); }
};

//! This algorithm uses the user defined edge costs to determine
//! the shortest path. It is assumed these values are set in the graph
//! prior to usage, otherwise they are defaulted to a value of 1.
class WSF_EXPORT EdgeWeight : public graph::GraphImpl::cost_func
{
public:
   EdgeWeight()           = default;
   ~EdgeWeight() override = default;

   double operator()(const graph::Edge& aEdge, const graph::Node& aSourceNode, const graph::Node& aDestinationNode) const override
   {
      if (!aEdge.IsEnabled())
      {
         return std::numeric_limits<double>::max();
      }

      return aEdge.GetWeight();
   }

   double operator()(const graph::Node& aSourceNode, const graph::Node& aDestinationNode) const override { return 0.0; }

   bool consider_node(const graph::Node& aNode) const override { return aNode.IsEnabled(); }
};

//! Exactly the same as the EdgeWeight algorithm, with the exception that
//! weights are subtracted from a max value to invert the edge weights.
//! This is useful for some edge weights where a higher assigned value
//! actually indicates a "better" weight. Users using this algorithm
//! must be mindful that the reported costs return by the FindPath algorithm
//! will reflect the added value for inversion.
class WSF_EXPORT InverseEdgeWeight : public graph::GraphImpl::cost_func
{
public:
   InverseEdgeWeight()           = default;
   ~InverseEdgeWeight() override = default;

   double operator()(const graph::Edge& aEdge, const graph::Node& aSourceNode, const graph::Node& aDestinationNode) const override
   {
      if (!aEdge.IsEnabled())
      {
         return std::numeric_limits<double>::max();
      }

      return (aEdge.GetWeight() * -1.0);
   }

   double operator()(const graph::Node& aSourceNode, const graph::Node& aDestinationNode) const override { return 0.0; }

   bool consider_node(const graph::Node& aNode) const override { return aNode.IsEnabled(); }
};

} // namespace comm
} // namespace wsf

#endif
