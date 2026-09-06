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
#include "WsfCommRoutingAlgorithmLibrary.hpp"
#include "gtest/gtest.h"

/*!
 * GRAPH VISUALIZATION - Paths 1 and 3 with 4 hops from A to B, path 2 with 3 hops from A to B
 *                       Path 4 is not connected, with no path from A to B. (Could be a separate network)
 * @verbatim
 *               1  2  3  4
 *
 *                  B
 *                / | \
 *               X  X  X
 *               |  |  |
 *               X  |  X  X
 *               |  |  |  |
 *               X  X  X  X
 *                \ | /
 *                  A
 * @endverbatim
 */

namespace wsf
{
namespace comm
{
namespace router
{
TEST(CommRoutingAlgorithm, LeastHopsTest)
{
   LeastHops    pathAlgorithm;
   graph::Graph graph;

   Address address_A("192.168.1.1");
   Address address_path1_1("192.168.1.2");
   Address address_path1_2("192.168.1.3");
   Address address_path1_3("192.168.1.4");
   Address address_path2_1("192.168.1.5");
   Address address_path2_2("192.168.1.6");
   Address address_path3_1("192.168.1.7");
   Address address_path3_2("192.168.1.8");
   Address address_path3_3("192.168.1.9");
   Address address_path4_1("192.168.2.1");
   Address address_path4_2("192.168.2.2");
   Address address_B("192.168.1.10");

   ASSERT_TRUE(graph.CreateNode(address_A));
   ASSERT_TRUE(graph.CreateNode(address_path1_1));
   ASSERT_TRUE(graph.CreateNode(address_path1_2));
   ASSERT_TRUE(graph.CreateNode(address_path1_3));
   ASSERT_TRUE(graph.CreateNode(address_path2_1));
   ASSERT_TRUE(graph.CreateNode(address_path2_2));
   ASSERT_TRUE(graph.CreateNode(address_path3_1));
   ASSERT_TRUE(graph.CreateNode(address_path3_2));
   ASSERT_TRUE(graph.CreateNode(address_path3_3));
   ASSERT_TRUE(graph.CreateNode(address_path4_1));
   ASSERT_TRUE(graph.CreateNode(address_path4_2));
   ASSERT_TRUE(graph.CreateNode(address_B));

   ASSERT_TRUE(graph.InsertEdge(address_A, address_path1_1, true));
   ASSERT_TRUE(graph.InsertEdge(address_path1_1, address_path1_2, true));
   ASSERT_TRUE(graph.InsertEdge(address_path1_2, address_path1_3, true));
   ASSERT_TRUE(graph.InsertEdge(address_path1_3, address_B, true));
   ASSERT_TRUE(graph.InsertEdge(address_A, address_path2_1, true));
   ASSERT_TRUE(graph.InsertEdge(address_path2_1, address_path2_2, true));
   ASSERT_TRUE(graph.InsertEdge(address_path2_2, address_B, true));
   ASSERT_TRUE(graph.InsertEdge(address_A, address_path3_1, true));
   ASSERT_TRUE(graph.InsertEdge(address_path3_1, address_path3_2, true));
   ASSERT_TRUE(graph.InsertEdge(address_path3_2, address_path3_3, true));
   ASSERT_TRUE(graph.InsertEdge(address_path3_3, address_B, true));
   ASSERT_TRUE(graph.InsertEdge(address_path4_1, address_path4_2, true));

   //! Least hops from A to B should be path 2
   graph::AddressList path;
   double             cost = 0.0;
   EXPECT_TRUE(graph.FindPath(address_A, address_B, path, cost, &pathAlgorithm));

   EXPECT_TRUE(path.size() == 4);
   EXPECT_TRUE(path[0] == address_A);
   EXPECT_TRUE(path[1] == address_path2_1);
   EXPECT_TRUE(path[2] == address_path2_2);
   EXPECT_TRUE(path[3] == address_B);
   EXPECT_TRUE(cost == 3.0);

   //! Should be no path from A to the disconnected address on path 4
   EXPECT_FALSE(graph.FindPath(address_A, address_path4_1, path, cost, &pathAlgorithm));

   //! Disabling edges on path 2 and 3 should result in a path 1 usage.
   graph.FindEdge(address_path2_2, address_B)->SetDisabled();
   graph.FindEdge(address_path3_3, address_B)->SetDisabled();
   EXPECT_TRUE(graph.FindPath(address_A, address_B, path, cost, &pathAlgorithm));

   EXPECT_TRUE(path.size() == 5);
   EXPECT_TRUE(path[0] == address_A);
   EXPECT_TRUE(path[1] == address_path1_1);
   EXPECT_TRUE(path[2] == address_path1_2);
   EXPECT_TRUE(path[3] == address_path1_3);
   EXPECT_TRUE(path[4] == address_B);
   EXPECT_TRUE(cost == 4.0);

   //! Enable the previously disabled edge on path 3. Now both path 1 and 3 are
   //! of equal cost and length. Either is a correct result.
   //! NOTE: This is undesirable. Even if two paths are correct, we should result
   //! in the same path selection every time for simulation repeatability. This is a
   //! bug that should be examined in the future. - KWW
   graph.FindEdge(address_path3_3, address_B)->SetEnabled();
   EXPECT_TRUE(graph.FindPath(address_A, address_B, path, cost, &pathAlgorithm));

   EXPECT_TRUE(path.size() == 5);
   EXPECT_TRUE(path[0] == address_A);
   EXPECT_TRUE(path[1] == address_path3_1 || path[1] == address_path1_1);
   EXPECT_TRUE(path[2] == address_path3_2 || path[2] == address_path1_2);
   EXPECT_TRUE(path[3] == address_path3_3 || path[3] == address_path1_3);
   EXPECT_TRUE(path[4] == address_B);
   EXPECT_TRUE(cost == 4.0);
}

//! TODO_ENHANCED_COMM - Implement these tests when these objects are completed.
// TEST(CommRoutingAlgorithm, ShortestTimeTest)
//{
// }

// TEST(CommRoutingAlgorithm, ScriptAlgorithmTest)
//{
// }

} // namespace router
} // namespace comm
} // namespace wsf
