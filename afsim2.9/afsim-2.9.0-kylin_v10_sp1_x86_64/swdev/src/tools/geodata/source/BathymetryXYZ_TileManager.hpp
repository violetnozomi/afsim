// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef BATHYMETRYXYZTILEMANAGER_HPP
#define BATHYMETRYXYZTILEMANAGER_HPP

#include "geodata_export.h"

#include <memory>
#include <mutex>
#include <string>
#include <vector>

// Geodata
#include "BathymetryXYZ_Tile.hpp"
#include "GeoSpatialPartitioning.hpp"

// Base class
#include "GeoElevationTileManager.hpp"

class GeoElevationTile;

namespace Geodata
{
//! Tile manager for directories containing bathymetry data.
class GEODATA_EXPORT BathymetryXYZ_TileManager : public GeoElevationTileManager
{
public:
   BathymetryXYZ_TileManager();
   ~BathymetryXYZ_TileManager() override;

   //! Required GeoElevationTileManager virtual methods
   //@{
   GeoElevationTile* LoadElevationTile(double aLat, double aLon) override;
   void              UnloadElevationTile(GeoElevationTile* aTilePtr) override;
   GeoElevationTile* GetCachedTile(double aLat, double aLon) const override;
   //@}

   bool AddFile(const std::string& aFileName);

private:
   // Load the tile containing the specified location.  The tile may be
   // loaded either from disk or internal cache or may be a dummy tile
   // with zero elevation if no data exists for the specified location.
   // The return value is a pointer to the tile.
   BathymetryXYZ_Tile* LoadTile(double aLat, double aLon);

   // Indicate the specified tile is no longer needed.  The reference
   // count is decremented.  If the reference count is zero the tile
   // *MAY* be unloaded from memory (depending on internal caching).
   void UnloadTile(BathymetryXYZ_Tile* aTilePtr);

   mutable BathymetryXYZ_TileCache mCache; // GeoTileCache makes some assumptions that don't work with the newer data

   std::vector<BathymetryXYZ_Tile::TileData> mTileMap;
   std::unique_ptr<SimpleSP_TreeNode>        mSP_Tree;

   mutable std::recursive_mutex mCacheLock;

   BathymetryXYZ_Tile mDefaultTile;
};
} // namespace Geodata

#endif
