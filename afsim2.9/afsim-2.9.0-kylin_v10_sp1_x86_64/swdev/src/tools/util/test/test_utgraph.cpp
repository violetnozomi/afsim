// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Radiance Technologies. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <array>

#include <gtest/gtest.h>

#include "UtGraph.hpp"

// NOTE(EQUAL_COST_PATHS): For some graphs, there are multiple equally good
// paths (i.e. paths with equal cost) between two nodes. For simulation
// determinism, we require that the same path be returned every time, so the
// tests only check for this path. Other possible paths are listed in comments
// to assist in debugging.

using TestNode = size_t;
using TestEdge = void*;

struct TestNodeGenerator
{
   size_t id = 0;

   TestNode next()
   {
      TestNode node = {id};
      ++id;
      return node;
   }
};

using TestGraph = UtGraphT<TestNode, TestEdge>;

// Adds `N` nodes to a graph. The added nodes are returned as an array.
template<size_t N>
inline static std::array<TestGraph::node_iterator, N> AddNodes(TestGraph& graph)
{
   std::array<TestGraph::node_iterator, N> nodes = {};
   for (size_t i = 0; i < N; ++i)
   {
      TestNode node = TestNode{i};
      nodes[i]      = graph.insert(node);
   }
   return nodes;
}

inline static void AddBidirectionalEdge(TestGraph&                      graph,
                                        const TestGraph::node_iterator& n1,
                                        const TestGraph::node_iterator& n2)
{
   graph.insert_edge(n1, n2, nullptr);
   graph.insert_edge(n2, n1, nullptr);
}

const static double NO_PATH_COST = -1.0;

static void ExpectNoPath(const std::vector<TestNode>& path, double cost)
{
   EXPECT_TRUE(path.empty());
   EXPECT_EQ(cost, NO_PATH_COST);
}

template<size_t N>
static double defaultExpectedCost(const std::array<TestGraph::node_iterator, N>& path)
{
   // By default, the edge costs in a graph are 1.0 and the node costs are 0.0,
   // so the cost is simply equal to the number of edges in the path, which is
   // equal to the number of nodes in the path minus 1.
   return path.empty() ? NO_PATH_COST : static_cast<double>(path.size() - 1);
}

template<size_t N>
static bool ArePathsEqual(const std::array<TestGraph::node_iterator, N>& expectedPath,
                          double                                         expectedCost,
                          const std::vector<TestNode>&                   actualPath,
                          double                                         actualCost)
{
   if (actualCost != expectedCost)
   {
      return false;
   }

   if (actualPath.size() != expectedPath.size())
   {
      return false;
   }

   bool pathsEqual = true;
   for (size_t i = 0; i < expectedPath.size(); ++i)
   {
      pathsEqual &= (expectedPath[i]->first == actualPath[i]);
   }
   return pathsEqual;
}

template<size_t N>
static bool ArePathsEqual(const std::array<TestGraph::node_iterator, N>& expectedPath,
                          const std::vector<TestNode>&                   actualPath,
                          double                                         actualCost)
{
   double expectedCost = defaultExpectedCost(expectedPath);
   return ArePathsEqual(expectedPath, expectedCost, actualPath, actualCost);
}

template<size_t N>
static void ExpectPathsEqual(const std::array<TestGraph::node_iterator, N>& expectedPath,
                             double                                         expectedCost,
                             const std::vector<TestNode>&                   actualPath,
                             double                                         actualCost)
{
   EXPECT_EQ(actualCost, expectedCost);

   if (actualPath.size() != expectedPath.size())
   {
      FAIL() << "Paths differ in length";
   }
   else
   {
      bool pathsEqual = true;
      for (size_t i = 0; i < expectedPath.size(); ++i)
      {
         pathsEqual &= (expectedPath[i]->first == actualPath[i]);
      }
      if (!pathsEqual)
      {
         FAIL() << "Paths differ";
      }
   }
}

template<size_t N>
static void ExpectPathsEqual(const std::array<TestGraph::node_iterator, N>& expectedPath,
                             const std::vector<TestNode>&                   actualPath,
                             double                                         actualCost)
{
   double expectedCost = defaultExpectedCost(expectedPath);
   ExpectPathsEqual(expectedPath, expectedCost, actualPath, actualCost);
}

TEST(UtGraph, shortest_path_NodeToSelf)
{
   TestGraph                graph;
   TestGraph::node_iterator n0 = graph.insert(TestNode{0});

   std::vector<TestNode> path;
   double                cost = 0.0;
   graph.shortest_path(n0, n0, path, cost);

   std::array<TestGraph::node_iterator, 1> expectedPath = {n0};
   ExpectPathsEqual(expectedPath, path, cost);
}

TEST(UtGraph, shortest_path_TwoDisconnectedNodes)
{
   TestGraph                graph;
   TestNodeGenerator        nodeGen;
   TestGraph::node_iterator n0 = graph.insert(nodeGen.next());
   TestGraph::node_iterator n1 = graph.insert(nodeGen.next());

   std::vector<TestNode> path;
   double                cost = 0.0;
   graph.shortest_path(n0, n1, path, cost);

   ExpectNoPath(path, cost);
}

TEST(UtGraph, shortest_path_TwoConnectedNodes)
{
   TestGraph                graph;
   TestNodeGenerator        nodeGen;
   TestGraph::node_iterator n0 = graph.insert(nodeGen.next());
   TestGraph::node_iterator n1 = graph.insert(nodeGen.next());
   graph.insert_edge(n0, n1, nullptr);

   // Test path from `n0` to `n1`
   {
      std::vector<TestNode> path;
      double                cost = 0.0;
      graph.shortest_path(n0, n1, path, cost);

      std::array<TestGraph::node_iterator, 2> expectedPath = {n0, n1};
      ExpectPathsEqual(expectedPath, path, cost);
   }

   // Test path from `n1` to `n0`. Since the graph is directed, there should
   // not be a path in the other direction.
   {
      std::vector<TestNode> path;
      double                cost = 0.0;
      graph.shortest_path(n1, n0, path, cost);

      ExpectNoPath(path, cost);
   }
}

TEST(UtGraph, shortest_path_ThreeConnectedNodes)
{
   TestGraph                graph;
   TestNodeGenerator        nodeGen;
   TestGraph::node_iterator n0 = graph.insert(nodeGen.next());
   TestGraph::node_iterator n1 = graph.insert(nodeGen.next());
   TestGraph::node_iterator n2 = graph.insert(nodeGen.next());
   graph.insert_edge(n0, n1, nullptr);
   graph.insert_edge(n1, n2, nullptr);

   std::vector<TestNode> path;
   double                cost = 0.0;
   graph.shortest_path(n0, n2, path, cost);

   std::array<TestGraph::node_iterator, 3> expectedPath = {n0, n1, n2};
   ExpectPathsEqual(expectedPath, path, cost);
}

TEST(UtGraph, shortest_path_DiamondGraph)
{
   // Construct a diamond-shaped graph with two equal-length paths from `n0` to `n3`.
   TestGraph                graph;
   TestNodeGenerator        nodeGen;
   TestGraph::node_iterator n0 = graph.insert(nodeGen.next());
   TestGraph::node_iterator n1 = graph.insert(nodeGen.next());
   TestGraph::node_iterator n2 = graph.insert(nodeGen.next());
   TestGraph::node_iterator n3 = graph.insert(nodeGen.next());
   graph.insert_edge(n0, n1, nullptr);
   graph.insert_edge(n0, n2, nullptr);
   graph.insert_edge(n1, n3, nullptr);
   graph.insert_edge(n2, n3, nullptr);

   std::vector<TestNode> path;
   double                cost = 0.0;
   graph.shortest_path(n0, n3, path, cost);

   // See NOTE(EQUAL_COST_PATHS) above
   // std::array<TestGraph::node_iterator,3> expectedPath = {n0, n2, n3};
   std::array<TestGraph::node_iterator, 3> expectedPath = {n0, n1, n3};
   ExpectPathsEqual(expectedPath, path, cost);
}

// This test is similar to `shortest_path_DiamondGraph`, but adds one more node
// to one of the paths, so the shortest path is unambiguous
TEST(UtGraph, shortest_path_DiamondGraph2)
{
   TestGraph                graph;
   TestNodeGenerator        nodeGen;
   TestGraph::node_iterator n0 = graph.insert(nodeGen.next());
   TestGraph::node_iterator n1 = graph.insert(nodeGen.next());
   TestGraph::node_iterator n2 = graph.insert(nodeGen.next());
   TestGraph::node_iterator n3 = graph.insert(nodeGen.next());
   TestGraph::node_iterator n4 = graph.insert(nodeGen.next());
   graph.insert_edge(n0, n1, nullptr);
   graph.insert_edge(n1, n2, nullptr);
   graph.insert_edge(n2, n4, nullptr);
   graph.insert_edge(n0, n3, nullptr);
   graph.insert_edge(n3, n4, nullptr);

   std::vector<TestNode> path;
   double                cost = 0.0;
   graph.shortest_path(n0, n4, path, cost);

   std::array<TestGraph::node_iterator, 3> expectedPath = {n0, n3, n4};
   ExpectPathsEqual(expectedPath, path, cost);
}

TEST(UtGraph, shortest_path_ComplexGraph)
{
   // Creating a graphical representation of this graph is hugely helpful in
   // understanding this test but C++ documentation does not support embedding
   // images, so you'll have to use your imagination.

   TestGraph graph;
   auto      n = AddNodes<18>(graph);
   AddBidirectionalEdge(graph, n[1], n[2]);
   AddBidirectionalEdge(graph, n[1], n[3]);
   AddBidirectionalEdge(graph, n[2], n[4]);
   AddBidirectionalEdge(graph, n[3], n[4]);
   AddBidirectionalEdge(graph, n[3], n[6]);
   AddBidirectionalEdge(graph, n[4], n[5]);
   AddBidirectionalEdge(graph, n[4], n[9]);
   AddBidirectionalEdge(graph, n[6], n[7]);
   AddBidirectionalEdge(graph, n[6], n[12]);
   AddBidirectionalEdge(graph, n[6], n[13]);
   AddBidirectionalEdge(graph, n[7], n[8]);
   AddBidirectionalEdge(graph, n[8], n[9]);
   AddBidirectionalEdge(graph, n[9], n[10]);
   AddBidirectionalEdge(graph, n[9], n[11]);
   AddBidirectionalEdge(graph, n[9], n[15]);
   AddBidirectionalEdge(graph, n[9], n[16]);
   AddBidirectionalEdge(graph, n[12], n[14]);
   AddBidirectionalEdge(graph, n[13], n[14]);
   AddBidirectionalEdge(graph, n[13], n[15]);
   AddBidirectionalEdge(graph, n[14], n[16]);
   AddBidirectionalEdge(graph, n[15], n[16]);
   AddBidirectionalEdge(graph, n[16], n[17]);

   // Node 0 is disconnected from the rest of the graph, so there should be no
   // path from it to any other node
   {
      std::vector<TestNode> path;
      double                cost = 0.0;
      graph.shortest_path(n[0], n[1], path, cost);

      ExpectNoPath(path, cost);
   }

   // Test two nodes that are directly connected
   {
      std::vector<TestNode> path;
      double                cost = 0.0;
      graph.shortest_path(n[1], n[2], path, cost);

      std::array<TestGraph::node_iterator, 2> expectedPath = {n[1], n[2]};
      ExpectPathsEqual(expectedPath, path, cost);
   }

   // Test a long path
   {
      std::vector<TestNode> path;
      double                cost = 0.0;
      graph.shortest_path(n[1], n[17], path, cost);

      // See NOTE(EQUAL_COST_PATHS) above
      // std::array<TestGraph::node_iterator,6> expectedPath = {
      //   n[1], n[3], n[4], n[9], n[16], n[17]};
      std::array<TestGraph::node_iterator, 6> expectedPath = {n[1], n[2], n[4], n[9], n[16], n[17]};
      ExpectPathsEqual(expectedPath, path, cost);
   }

   // Test another long path
   {
      std::vector<TestNode> path;
      double                cost = 0.0;
      graph.shortest_path(n[5], n[12], path, cost);

      std::array<TestGraph::node_iterator, 5> expectedPath = {n[5], n[4], n[3], n[6], n[12]};
      ExpectPathsEqual(expectedPath, path, cost);
   }

   // Test the previous path, going in the other direction
   {
      std::vector<TestNode> path;
      double                cost = 0.0;
      graph.shortest_path(n[12], n[5], path, cost);

      std::array<TestGraph::node_iterator, 5> expectedPath = {n[12], n[6], n[3], n[4], n[5]};
      ExpectPathsEqual(expectedPath, path, cost);
   }
}
