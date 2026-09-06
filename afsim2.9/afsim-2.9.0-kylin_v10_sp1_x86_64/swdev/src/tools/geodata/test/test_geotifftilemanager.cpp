// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include <gtest/gtest.h>

#include "GeotiffTileManager.hpp"
#include "UtLocator.hpp"
#include "UtTestDataService.hpp"

// This checks the spatial partitioning code in GeotiffTileManager
TEST(GeotiffTileManager, SpatialPartitioning)
{
   std::list<Geodata::SP_Edge> splist1;
   splist1.emplace_back(true, true, -2.0);         // x > -2
   splist1.emplace_back(true, false, 2.0);         // x < 2
   splist1.emplace_back(false, true, -2.0);        // y > -2
   splist1.emplace_back(false, false, 2.0);        // y < 2
   Geodata::SimpleSP_TreeNode tree(1, 0, splist1); // create a base tile
   EXPECT_EQ(tree.LookUpIndex(-3, -3), 0);
   EXPECT_EQ(tree.LookUpIndex(-3, 0), 0);
   EXPECT_EQ(tree.LookUpIndex(-3, 3), 0);
   EXPECT_EQ(tree.LookUpIndex(0, -3), 0);
   EXPECT_EQ(tree.LookUpIndex(0, 0), 1);
   EXPECT_EQ(tree.LookUpIndex(0, 3), 0);
   EXPECT_EQ(tree.LookUpIndex(3, -3), 0);
   EXPECT_EQ(tree.LookUpIndex(3, 0), 0);
   EXPECT_EQ(tree.LookUpIndex(3, 3), 0);
   std::list<Geodata::SP_Edge> splist2;     // add a tile to the left
   splist2.emplace_back(true, true, -4.0);  // x > -4
   splist2.emplace_back(true, false, -2.0); // x < -2
   splist2.emplace_back(false, true, -2.0); // y > -2
   splist2.emplace_back(false, false, 2.0); // y < 2
   tree.InsertZone(2, splist2);
   EXPECT_EQ(tree.LookUpIndex(-5, -3), 0);
   EXPECT_EQ(tree.LookUpIndex(-5, 0), 0);
   EXPECT_EQ(tree.LookUpIndex(-5, 3), 0);
   EXPECT_EQ(tree.LookUpIndex(-3, -3), 0);
   EXPECT_EQ(tree.LookUpIndex(-3, 0), 2);
   EXPECT_EQ(tree.LookUpIndex(-3, 3), 0);
   EXPECT_EQ(tree.LookUpIndex(0, -3), 0);
   EXPECT_EQ(tree.LookUpIndex(0, 0), 1);
   EXPECT_EQ(tree.LookUpIndex(0, 3), 0);
   EXPECT_EQ(tree.LookUpIndex(3, -3), 0);
   EXPECT_EQ(tree.LookUpIndex(3, 0), 0);
   EXPECT_EQ(tree.LookUpIndex(3, 3), 0);
   std::list<Geodata::SP_Edge> splist3;     // add a tile above
   splist3.emplace_back(true, true, -2.0);  // x > -2
   splist3.emplace_back(true, false, 2.0);  // x < 2
   splist3.emplace_back(false, true, 2.0);  // y > 2
   splist3.emplace_back(false, false, 4.0); // y < 4
   tree.InsertZone(3, splist3);
   EXPECT_EQ(tree.LookUpIndex(-5, -3), 0);
   EXPECT_EQ(tree.LookUpIndex(-5, 0), 0);
   EXPECT_EQ(tree.LookUpIndex(-5, 3), 0);
   EXPECT_EQ(tree.LookUpIndex(-5, 5), 0);
   EXPECT_EQ(tree.LookUpIndex(-3, -3), 0);
   EXPECT_EQ(tree.LookUpIndex(-3, 0), 2);
   EXPECT_EQ(tree.LookUpIndex(-3, 3), 0);
   EXPECT_EQ(tree.LookUpIndex(-3, 5), 0);
   EXPECT_EQ(tree.LookUpIndex(0, -3), 0);
   EXPECT_EQ(tree.LookUpIndex(0, 0), 1);
   EXPECT_EQ(tree.LookUpIndex(0, 3), 3);
   EXPECT_EQ(tree.LookUpIndex(0, 5), 0);
   EXPECT_EQ(tree.LookUpIndex(3, -3), 0);
   EXPECT_EQ(tree.LookUpIndex(3, 0), 0);
   EXPECT_EQ(tree.LookUpIndex(3, 3), 0);
   EXPECT_EQ(tree.LookUpIndex(3, 5), 0);
   std::list<Geodata::SP_Edge> splist4;     // add a tile with overlap
   splist4.emplace_back(true, true, -3.0);  // x > -3
   splist4.emplace_back(true, false, -1.0); // x < -1
   splist4.emplace_back(false, true, 1.0);  // y > 1
   splist4.emplace_back(false, false, 3.0); // y < 3
   tree.InsertZone(4, splist4);
   EXPECT_EQ(tree.LookUpIndex(-3.5, 3), 0);
   EXPECT_EQ(tree.LookUpIndex(0, 0), 1);
   EXPECT_EQ(tree.LookUpIndex(-3.5, 0), 2);
   EXPECT_EQ(tree.LookUpIndex(0, 3.5), 3);
   EXPECT_EQ(tree.LookUpIndex(-2, 2), 4);
}

// this tests the whole GeotiffTileManager from directory to value lookup
// specifically:
//    GeotiffTileManager::AddDirectory
//    GeotiffTileManager::LoadElevationTile
//    and the dummy tile system which provides an empty tile where none is available
TEST(GeotiffTileManager, ManagerQuery)
{
   Geodata::GeotiffTileManager gtm;
   const auto&                 dataService = ut::Locator<ut::TestDataService>::get();
   gtm.AddDirectory(dataService.getResource("").GetNormalizedPath());

   double            lat   = 37.3335;
   double            lon   = -120;
   GeoElevationTile* tile1 = gtm.LoadElevationTile(lat, lon);
   float             intp;
   tile1->GetElevInterp(lat, lon, intp);
   // make sure we get a valid tile when querying in the data
   EXPECT_EQ(tile1->IsDummyTile(), false);
   EXPECT_EQ(intp, 389.0F);
   // make sure we get a dummy tile when querying off the data
   GeoElevationTile* tile2 = gtm.LoadElevationTile(39, -100);
   EXPECT_EQ(tile2->IsDummyTile(), true);
}
