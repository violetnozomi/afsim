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

#include "LandUseTileManager.hpp"

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <sstream>

#include "GeoTileCache.hpp"
#include "LandUseDir.hpp"
#include "LandUseTile.hpp"
#include "UtException.hpp"

// static
LandUseTile::Format LandUseTileManager::sFormat = LandUseTile::cBITMAP;

LandUseTileManager::LandUseTileManager(LandUseTile* (*aTileAllocator)())
   : mTileAllocator(aTileAllocator)
   , mDirPtr(nullptr)
   , mTilePtr(nullptr)
   , mCachePtr(nullptr)
   , mTileCacheSize(10)
   , mCacheLock()
{
   // Use the default tile allocator if none was provided...
   if (mTileAllocator == nullptr)
   {
      mTileAllocator = DefaultTileAllocator;
   }

   // Allocate a tile cache
   mCachePtr = new GeoTileCache();

   // Allow the tile cache size to be overridden by an environment variable
   char* envPtr = ::getenv("LAND_USE_TILE_CACHE_SIZE");
   if (envPtr != nullptr)
   {
      std::istringstream iss(envPtr);
      int                tileCacheSize;
      if (iss >> tileCacheSize)
      {
         mTileCacheSize = tileCacheSize;
      }
   }
}

// virtual
LandUseTileManager::~LandUseTileManager()
{
   delete mCachePtr;
   delete mDirPtr;
}

// Add a 'raw' directory to be considered when looking for Land Use.  The
// directory should contain the longitudinal directories as its direct
// descendants.
//
// The return value is 0 if successful or non-zero on failure.
int LandUseTileManager::AddDirectory(const char* aDirName, double aSWLat, double aSWLon, double aNELat, double aNELon)
{
   int error = 2;
   mDirPtr   = new LandUseDir(aDirName, aSWLat, aSWLon, aNELat, aNELon, error);
   return error;
}

// Add a file to be considered when looking for Land Use.
//
// The return value is 0 if successful or non-zero on failure.
int LandUseTileManager::AddFile(const char* aFileName)
{
   int error = 2;
   mTilePtr  = new LandUseTile(sFormat);
   error     = mTilePtr->LoadTile(aFileName);
   return error;
}

// The default tile allocator
LandUseTile* LandUseTileManager::DefaultTileAllocator()
{
   return new LandUseTile(sFormat);
}

// Load the tile containing the specified location.  The tile may be
// loaded either from disk or internal cache or may be a dummy tile
// with zero if no data exists for the specified location.
//
// The return value is a pointer to the tile.  The caller *MUST NOT*
// 'delete' the tile as it is a reference counted object that could
// be used by something else.  When the caller no longer requires
// access to the tile it should call 'UnloadTile(tilePtr)' to decrement
// the reference count and release memory if necessary.
LandUseTile* LandUseTileManager::LoadTile(double aLat, double aLon)
{
   LandUseTile* tilePtr = nullptr;

   // Either load the cell from cache or from the directory

   std::lock_guard<std::recursive_mutex> lock(mCacheLock);
   GeoTile*                              cacheTilePtr = nullptr;
   if (mDirPtr != nullptr)
   {
      int row = mDirPtr->GetRow(aLat);
      int col = mDirPtr->GetCol(aLon);

      // currently only one dir setting
      if ((aLat >= mDirPtr->GetSWLat()) && (aLat <= mDirPtr->GetNELat()) && (aLon >= mDirPtr->GetSWLon()) &&
          (aLon <= mDirPtr->GetNELon()))
      {
         cacheTilePtr = mCachePtr->UseTile(mDirPtr, row, col);
      }
      else
      {
         std::ostringstream str;
         str << "Land Use - out of bounds for lat: " << aLat << ", lon: " << aLon;
         throw(UtException(str.str()));
      }
   }
   if (cacheTilePtr != nullptr)
   {
      // Tile is in cache... no need to reload it...
      tilePtr = reinterpret_cast<LandUseTile*>(cacheTilePtr);
   }
   else
   {
      // Tile is not in cache... it must be loaded
      tilePtr          = (*mTileAllocator)();
      bool dirSuccess  = false;
      bool tileSuccess = false;
      if (mDirPtr != nullptr)
      {
         dirSuccess = (mDirPtr->LoadTile(tilePtr, aLat, aLon) == 0);
         if (dirSuccess)
         {
            mCachePtr->AddTile(tilePtr);
            if (mTileCacheSize >= 0)
            {
               mCachePtr->PurgeInactive(static_cast<unsigned int>(mTileCacheSize));
            }
         }
      }
      if (mTilePtr != nullptr)
      {
         int value   = 0;
         tileSuccess = (mTilePtr->GetLandUse(aLat, aLon, value) == 0);
      }
      if (!dirSuccess && tileSuccess)
      {
         tilePtr = mTilePtr;
      }
   }
   return tilePtr;
}

// Indicate the specified tile is no longer needed.  The reference
// count is decremented.  If the reference count is zero the tile
// *MAY* be unloaded from memory (depending on internal caching).

void LandUseTileManager::UnloadTile(LandUseTile* aTilePtr)
{
   if (aTilePtr != nullptr)
   {
      std::lock_guard<std::recursive_mutex> lock(mCacheLock);
      mCachePtr->ReleaseTile(aTilePtr);
      if (mTileCacheSize >= 0)
      {
         mCachePtr->PurgeInactive(static_cast<unsigned int>(mTileCacheSize));
      }
   }
}

// virtual
GeoLatLonGridTile* LandUseTileManager::LoadLatLonTile(double aLat, double aLon)
{
   return static_cast<GeoLatLonGridTile*>(LoadTile(aLat, aLon));
}

// virtual
void LandUseTileManager::UnloadTile(GeoLatLonGridTile* aTilePtr)
{
   UnloadTile(dynamic_cast<LandUseTile*>(aTilePtr));
}
