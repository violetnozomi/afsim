// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfCommGraph.hpp"
#include "gtest/gtest.h"

namespace wsf
{
namespace comm
{
namespace graph
{

//! Note: The FindPath method requires usage of a RoutingAlgorithm object.
//! This method will be tested on a graph within WsfCommRoutingAlgorithm.cpp.

TEST(CommGraph, NodeAndCopyTest)
{
   Address address1("111.222.111.222");
   Address address2("111.222.111.223");
   Address address3("111.222.111.224");
   Address address4("111.222.111.225");

   Graph graph;

   graph.CreateNode(address1);
   graph.CreateNode(address2);
   graph.CreateNode(address3);
   graph.CreateNode(address4);

   auto nodePtr1 = graph.FindNode(address1);
   auto nodePtr2 = graph.FindNode(address2);
   auto nodePtr3 = graph.FindNode(address3);
   auto nodePtr4 = graph.FindNode(address4);

   EXPECT_TRUE(nodePtr1);
   EXPECT_TRUE(nodePtr2);
   EXPECT_TRUE(nodePtr3);
   EXPECT_TRUE(nodePtr4);

   Graph copyGraph;
   nodePtr1 = copyGraph.FindNode(address1);
   nodePtr2 = copyGraph.FindNode(address2);
   nodePtr3 = copyGraph.FindNode(address3);
   nodePtr4 = copyGraph.FindNode(address4);

   EXPECT_FALSE(nodePtr1);
   EXPECT_FALSE(nodePtr2);
   EXPECT_FALSE(nodePtr3);
   EXPECT_FALSE(nodePtr4);

   copyGraph = graph;

   nodePtr1 = copyGraph.FindNode(address1);
   nodePtr2 = copyGraph.FindNode(address2);
   nodePtr3 = copyGraph.FindNode(address3);
   nodePtr4 = copyGraph.FindNode(address4);

   EXPECT_TRUE(nodePtr1);
   EXPECT_TRUE(nodePtr2);
   EXPECT_TRUE(nodePtr3);
   EXPECT_TRUE(nodePtr4);

   Graph anotherCopyGraph(copyGraph);

   nodePtr1 = anotherCopyGraph.FindNode(address1);
   nodePtr2 = anotherCopyGraph.FindNode(address2);
   nodePtr3 = anotherCopyGraph.FindNode(address3);
   nodePtr4 = anotherCopyGraph.FindNode(address4);

   EXPECT_TRUE(nodePtr1);
   EXPECT_TRUE(nodePtr2);
   EXPECT_TRUE(nodePtr3);
   EXPECT_TRUE(nodePtr4);

   Graph toClearGraph(copyGraph);

   anotherCopyGraph.RemoveNode(nodePtr1);
   anotherCopyGraph.RemoveNode(nodePtr2);
   anotherCopyGraph.RemoveNode(nodePtr3);
   anotherCopyGraph.RemoveNode(nodePtr4);

   nodePtr1 = anotherCopyGraph.FindNode(address1);
   nodePtr2 = anotherCopyGraph.FindNode(address2);
   nodePtr3 = anotherCopyGraph.FindNode(address3);
   nodePtr4 = anotherCopyGraph.FindNode(address4);

   EXPECT_FALSE(nodePtr1);
   EXPECT_FALSE(nodePtr2);
   EXPECT_FALSE(nodePtr3);
   EXPECT_FALSE(nodePtr4);

   nodePtr1 = toClearGraph.FindNode(address1);
   nodePtr2 = toClearGraph.FindNode(address2);
   nodePtr3 = toClearGraph.FindNode(address3);
   nodePtr4 = toClearGraph.FindNode(address4);

   EXPECT_TRUE(nodePtr1);
   EXPECT_TRUE(nodePtr2);
   EXPECT_TRUE(nodePtr3);
   EXPECT_TRUE(nodePtr4);

   toClearGraph.Clear();

   nodePtr1 = toClearGraph.FindNode(address1);
   nodePtr2 = toClearGraph.FindNode(address2);
   nodePtr3 = toClearGraph.FindNode(address3);
   nodePtr4 = toClearGraph.FindNode(address4);

   EXPECT_FALSE(nodePtr1);
   EXPECT_FALSE(nodePtr2);
   EXPECT_FALSE(nodePtr3);
   EXPECT_FALSE(nodePtr4);
}

TEST(CommGraph, EdgeTest)
{
   Address address1("111.222.111.222");
   Address address2("111.222.111.223");
   Address address3("111.222.111.224");
   Address address4("111.222.111.225");

   std::vector<Address> addressVec = {address1, address2, address3, address4};

   Graph graph;

   graph.CreateNode(address1);
   graph.CreateNode(address2);
   graph.CreateNode(address3);
   graph.CreateNode(address4);

   for (size_t i = 0; i < 4; ++i)
   {
      for (size_t j = 0; j < 4; ++j)
      {
         if (i != j)
         {
            auto edgePtr = graph.InsertEdge(addressVec[i], addressVec[j], true);
            EXPECT_TRUE(edgePtr);
         }
      }
   }

   for (size_t i = 0; i < 4; ++i)
   {
      for (size_t j = 0; j < 4; ++j)
      {
         if (i != j)
         {
            auto edgePtr = graph.FindEdge(addressVec[i], addressVec[j]);
            EXPECT_TRUE(edgePtr);
         }
      }
   }

   for (size_t i = 0; i < 4; ++i)
   {
      auto edgeList = graph.GetOutgoingNodeEdges(addressVec[i]);
      EXPECT_TRUE(edgeList.size() == 3);
   }

   for (size_t i = 0; i < 4; ++i)
   {
      for (size_t j = 0; j < 4; ++j)
      {
         if (i != j)
         {
            EXPECT_TRUE(graph.EraseEdge(addressVec[i], addressVec[j]));
         }
      }
   }

   Graph graphCopy(graph);

   for (size_t i = 0; i < 4; ++i)
   {
      auto edgeList = graph.GetOutgoingNodeEdges(addressVec[i]);
      EXPECT_TRUE(edgeList.size() == 0);
   }
}
} // namespace graph
} // namespace comm
} // namespace wsf