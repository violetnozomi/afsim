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

#ifndef GEOTILECACHE_HPP
#define GEOTILECACHE_HPP

#include "geodata_export.h"

// A GeoTileCache is used to maintain a cache of GeoTile objects.
#include <list>
#include <map>

class GeoTile;
class GeoTileMatrix;
#include "GeoTileName.hpp"

class GEODATA_EXPORT GeoTileCache
{
public:
   GeoTileCache();

   ~GeoTileCache();

   // A new tile has been loaded into memory.  Allocate a cache entry and
   // initialize the use count for the tile to 1.
   //
   // NOTE: The tile becomes the property of the GeoTileCache.  It will
   //       delete the object when necessary.

   void AddTile(GeoTile* aTilePtr);

   // Purge tiles from the inactive list.  Tiles whose use count are zero
   // may be released from memory if conditions warrant.

   void PurgeInactive(unsigned int aKeepCount);

   // Purge tiles from the inactive list.  Tiles will be purged if the memory
   // limit is set and has been exceeded.

   void PurgeInactive(double aMaxDataSize);

   // A reference to a tile is no longer required.  Decrement the use count
   // for the tile.

   void ReleaseTile(GeoTile* aTilePtr);

   // Attempt to find a cache entry for the specified tile.  If found, the
   // use count for the tile will be incremented and a pointer to the tile
   // will be returned.  If the tile is not in the cache then zero will be
   // returned.

   GeoTile* UseTile(GeoTileMatrix* aTileMatrixPtr, unsigned int aRow, unsigned int aCol);

   double GetDataSize() const { return mSizeInBytes; }

private:
   std::map<GeoTileName, GeoTile*> mCache;
   std::list<GeoTile*>             mInactive;
   double                          mSizeInBytes;
   unsigned int                    mNumInactive;
};

#endif
