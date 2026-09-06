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

#ifndef LANDUSETILEMANAGER_HPP
#define LANDUSETILEMANAGER_HPP

#include "geodata_export.h"

// This is the tile manager for managing directories containing NLCD
// (National Land Coverage Data).

#include <mutex>
#include <string>
#include <vector>

#include "GeoRect.hpp"
#include "GeoTileManager.hpp"
class GeoTileCache;
class LandUseDir;
#include "LandUseTile.hpp"

//! A tile manager for NLCD formatted data.  Currently we assume
//! the data are stored in the standard format of
class GEODATA_EXPORT LandUseTileManager : public GeoTileManager
{
public:
   // The optional single argument to the constructor is the address of
   // a function that will be called to instantiate an object of type
   // 'LandUseTile' or a class derived from it.  This allows the caller
   // to maintain extra information with the tile (texture name, etc)
   // that is application specific.

   LandUseTileManager(LandUseTile* (*aTileAllocator)() = nullptr);

   ~LandUseTileManager() override;

   //! Set the tile format that we expect to load (8-bit bitmap or ARC-ASCII).
   static void SetTileFormat(LandUseTile::Format aFormat) { sFormat = aFormat; }

   // Required virtual methods:
   virtual GeoLatLonGridTile* LoadLatLonTile(double aLat, double aLon);

   virtual void UnloadTile(GeoLatLonGridTile* aTilePtr);

   // Add a directory to be considered when looking for NLCD.  The
   // directory *MUST* contain the files/subdirectories as they would
   // on a NIMA CD (i.e.: dmed, Dem, gazette, text).
   //
   // The return value is 0 if successful or non-zero on failure.


   // TODO: tbd
   //      int AddDirectory(const char* aDirName);

   // Add a 'raw' directory to be considered when looking for NLCD.  The
   // directory should contain the longitudinal directories as its direct
   // descendants.
   //
   // The return value is 0 if successful or non-zero on failure.
   int AddDirectory(const char* aDirName, double aSWLat, double aSWLon, double aNELat, double aNELon);

   // Add a file to be considered when looking for NLCD.
   //
   // The return value is 0 if successful or non-zero on failure.
   int AddFile(const char* aFileName);

   // Load the tile containing the specified location.  The tile may be
   // loaded either from disk or internal cache or may be a dummy tile
   // with zero if no data exists for the specified location.
   //
   // The return value is a pointer to the tile.  The caller *MUST NOT*
   // 'delete' the tile as it is a reference counted object that could
   // be used by something else.  When the caller no longer requires
   // access to the tile it should call 'UnloadTile(tilePtr)' to decrement
   // the reference count and release memory if necessary.

   LandUseTile* LoadTile(double aLat, double aLon);

   // Set the size of the tile cache.  0 indicates no tiles will be cached.
   // A negative value indicates tile will never be purge (except when
   // the tile manager is deleted).  The default value is 10 tiles.

   void SetTileCacheSize(int aTileCacheSize) { mTileCacheSize = aTileCacheSize; }

   // Indicate the specified tile is no longer needed.  The reference
   // count is decremented.  If the reference count is zero the tile
   // *MAY* be unloaded from memory (depending on internal caching).

   void UnloadTile(LandUseTile* aTilePtr);

private:
   // The default tile allocator
   static LandUseTile*        DefaultTileAllocator();
   static LandUseTile::Format sFormat;

   // Address of the routine to allocate tiles...
   LandUseTile* (*mTileAllocator)();

   // Points to directory that contain NLCD
   LandUseDir*   mDirPtr;
   LandUseTile*  mTilePtr;
   GeoTileCache* mCachePtr;

   int mTileCacheSize;

   std::recursive_mutex mCacheLock;
};

#endif
