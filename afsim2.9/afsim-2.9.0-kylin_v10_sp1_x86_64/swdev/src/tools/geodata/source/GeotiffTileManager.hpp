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

#ifndef GEOTIFFTILEMANAGER_HPP
#define GEOTIFFTILEMANAGER_HPP

#include "geodata_export.h"

#include <memory>
#include <mutex>
#include <vector>

#include "GeoElevationTileManager.hpp"
#include "GeoSpatialPartitioning.hpp"
#include "GeoTileCache.hpp"
#include "GeotiffTile.hpp"

namespace Geodata
{
class GEODATA_EXPORT GeotiffTileManager : public GeoElevationTileManager
{
public:
   GeotiffTileManager();

   bool AddDirectory(const std::string& aPath);
   void RemoveDirectories();
   bool AddFile(const std::string& aPath);

   GeoElevationTile* LoadElevationTile(double aLat, double aLon) override;

   void UnloadElevationTile(GeoElevationTile* aTilePtr) override;

   // Find the tile (if loaded) containing the specified location.  The tile may only
   // be loaded either from internal cache or may be a dummy tile
   // with zero elevation if no data exists for the specified location.
   // This is handy for threaded applications.
   //
   // The return value is a pointer to the tile.  The caller *MUST NOT*
   // 'delete' the tile as it is a reference counted object that could
   // be used by something else.  When the caller no longer requires
   // access to the tile it should call 'UnloadTile(tilePtr)' to decrement
   // the reference count and release memory if necessary.
   GeotiffTile* GetCachedTile(double aLat, double aLon) const override;

   bool IsBestTile(GeotiffTile* aTile, double aLat, double aLon) const;

private:
   // Load the tile containing the specified location.  The tile may be
   // loaded either from disk or internal cache or may be a dummy tile
   // with zero elevation if no data exists for the specified location.
   //
   // The return value is a pointer to the tile.  The caller *MUST NOT*
   // 'delete' the tile as it is a reference counted object that could
   // be used by something else.  When the caller no longer requires
   // access to the tile it should call 'UnloadTile(tilePtr)' to decrement
   // the reference count and release memory if necessary.
   GeotiffTile* LoadTile(double aLat, double aLon);

   // Indicate the specified tile is no longer needed.  The reference
   // count is decremented.  If the reference count is zero the tile
   // *MAY* be unloaded from memory (depending on internal caching).
   void UnloadTile(GeotiffTile* aTilePtr);

   mutable GeotiffTileCache mCache; // GeoTileCache makes some assumptions that don't work with the newer data

   std::vector<GeotiffTile::TileData> mTileMap;
   std::unique_ptr<SimpleSP_TreeNode> mSP_Tree;

   std::recursive_mutex mCacheLock;

   GeotiffTile mDefaultTile;
};
} // namespace Geodata

#endif
