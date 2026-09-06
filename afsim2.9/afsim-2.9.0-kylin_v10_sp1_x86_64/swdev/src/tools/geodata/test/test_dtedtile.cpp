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

#include "DtedTile.hpp"
#include "UtLocator.hpp"
#include "UtTestDataService.hpp"

// This test scans a line on the tile from 37.3335 N 120 W to 37.6667 N 119 W and samples 18 points.  It checks:
//    DtedTile::LoadTile
//    DtedTile::GetLatIndex
//    DtedTile::GetLonIndex
//    DtedTile::GetElev
//    DtedTile::GetElevApprox
//    DtedTile::GetElevInterp
TEST(DtedTile, ReadCell)
{
   int latValues[] = {400, 424, 447, 471, 494, 518, 541, 565, 588, 612, 635, 659, 682, 706, 729, 753, 777, 800};
   int lonValues[] = {0, 71, 141, 212, 282, 353, 423, 494, 564, 635, 706, 776, 847, 917, 988, 1058, 1129, 1200};
   float getValues[] = {385, 566, 503, 679, 960, 1380, 1517, 1754, 2530, 2519, 2698, 2654, 2485, 2086, 3052, 2863, 2814, 2837};
   float apxValues[] = {385, 566, 503, 679, 960, 1380, 1517, 1754, 2530, 2519, 2698, 2654, 2485, 2086, 3052, 2863, 2814, 2837};
   float intpValues[] = {389,
                         565.603F,
                         507.930F,
                         670.610F,
                         971.803F,
                         1376.680F,
                         1527.064F,
                         1754.582F,
                         2527.864F,
                         2518.925F,
                         2680.64F,
                         2654.029F,
                         2491.362F,
                         2078.539F,
                         3064.480F,
                         2863.8F,
                         2820.270F,
                         2830.091F};

   DtedTile    tile;
   const auto& dataService = ut::Locator<ut::TestDataService>::get();
   std::string filepath    = dataService.getResource("n37_w120_3arc_v2.dt1").GetNormalizedPath();
   ASSERT_EQ(tile.LoadTile(filepath.c_str()), 0);

   double       lat = 37.3335;
   double       lon = -120;
   unsigned int i   = 0;
   for (; (lat < 37.6667) && (lon < -119); lat += 0.0196)
   {
      unsigned int latIdx = tile.GetLatIndex(lat);
      unsigned int lonIdx = tile.GetLonIndex(lon);
      EXPECT_EQ(latIdx, latValues[i]) << "DtedTile::GetLatIndex failed at test index(i) = " << i;
      EXPECT_EQ(lonIdx, lonValues[i]) << "DtedTile::GetLonIndex failed at test index(i) = " << i;

      EXPECT_EQ(tile.GetElev(latIdx, lonIdx), getValues[i]) << "DtedTile::GetElev failed at test index(i) = " << i;
      float apx;
      tile.GetElevApprox(lat, lon, apx);
      EXPECT_EQ(apx, apxValues[i]) << "DtedTile::GetElevApprox failed at test index(i) = " << i;
      float intp;
      tile.GetElevInterp(lat, lon, intp);
      EXPECT_NEAR(intp, intpValues[i], 0.001) << "DtedTile::GetElevInterp failed at test index(i) = " << i;
      lon += 0.0588;
      ++i;
   }
}
