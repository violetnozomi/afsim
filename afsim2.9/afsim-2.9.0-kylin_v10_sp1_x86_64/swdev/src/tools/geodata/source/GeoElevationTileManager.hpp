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

#ifndef GeoElevationTileManager_hpp
#define GeoElevationTileManager_hpp

#include "geodata_export.h"

#include <string>

class GeoElevationTile;
#include "GeoTileManager.hpp"

//! 'GeoElevationTileManager' is base class that is used to load and unload 'tiles' of
//! elevation as required by the application.
class GEODATA_EXPORT GeoElevationTileManager : public GeoTileManager
{
public:
   GeoElevationTileManager()
      : GeoTileManager()
      , mTileCacheSize(10)
      ,                 // 10 tiles
      mMaxDataSize(0.0) // not used
   {
   }

   ~GeoElevationTileManager() override {}

   //! Set the size of the tile cache.  0 indicates no tiles will be cached.
   //! A negative value indicates tile will never be purge (except when
   //! the tile manager is deleted).  The default value is 10 tiles.
   //! @note If this value is set, it overrides the max data size.
   void SetTileCacheSize(int aTileCacheSize)
   {
      mTileCacheSize = aTileCacheSize;
      mMaxDataSize   = 0.0;
   }

   //! Set the maximum data size for data in the tiles.
   //! If this is set, it overrides the tile cache size setting.
   void SetMaxDataSize(double aDataSize)
   {
      mMaxDataSize   = aDataSize;
      mTileCacheSize = -1;
   }

   virtual GeoElevationTile* LoadElevationTile(double aLat, double aLon) = 0;

   virtual void UnloadElevationTile(GeoElevationTile* aTilePtr) = 0;

   //! Get a tile if already loaded
   virtual GeoElevationTile* GetCachedTile(double aLat, double aLon) const = 0;

protected:
   int    mTileCacheSize;
   double mMaxDataSize;

private:
};

#endif
