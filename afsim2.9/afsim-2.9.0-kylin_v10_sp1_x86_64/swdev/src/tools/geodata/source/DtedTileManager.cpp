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

#include "DtedTileManager.hpp"

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <sstream>

#include "DtedDir.hpp"
#include "DtedTile.hpp"
#include "GeoTileCache.hpp"

DtedTileManager::DtedTileManager(DtedTile* (*aTileAllocator)())
   : mTileAllocator(aTileAllocator)
   , mCacheLock()
{
   // Use the default tile allocator if none was provided...
   if (mTileAllocator == nullptr)
   {
      mTileAllocator = DefaultTileAllocator;
   }

   // Allocate a tile cache
   mCachePtr = new GeoTileCache();

   // Allocate the catch-all entry used for areas where DTED does not exist.
   mDirVector.push_back(new DtedDir());

   // Allow the tile cache size to be overriden by an environment variable

   char* envPtr = ::getenv("DTED_TILE_CACHE_SIZE");
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
DtedTileManager::~DtedTileManager()
{
   delete mCachePtr;
   mCachePtr = nullptr;

   for (unsigned int i = 0; i < mDirVector.size(); ++i)
   {
      delete mDirVector[i];
   }
   mDirVector.clear();
}

// Add a directory to be considered when looking for DTED.  The
// directory *MUST* contain the files/subdirectories as they would
// on a NIMA CD (i.e.: dmed, dted, gazette, text).
//
// The return value is 0 if successful or non-zero on failure.

int DtedTileManager::AddDirectory(const char* aDirName, int aLevel)
{
   int status = 0;

   DtedDir* dirPtr = new DtedDir(aDirName, aLevel, status);
   if (status == 0)
   {
      mDirVector.push_back(dirPtr);
   }
   else
   {
      delete dirPtr;
   }
   return status;
}

// Add a 'raw' directory to be considered when looking for DTED.  The
// directory should contain the longitudinal directories as its direct
// descendents.
//
// The return value is 0 if successful or non-zero on failure.

int DtedTileManager::AddDirectory(const char* aDirName, int aLevel, int aSWLat, int aSWLon, int aNELat, int aNELon)
{
   int status = 0;

   DtedDir* dirPtr = new DtedDir(aDirName, aLevel, aSWLat, aSWLon, aNELat, aNELon, status);
   if (status == 0)
   {
      mDirVector.push_back(dirPtr);
   }
   else
   {
      delete dirPtr;
   }
   return status;
}

void DtedTileManager::RemoveDirectories()
{
   while (mDirVector.size() > 1)
   {
      delete mDirVector.back();
      mDirVector.pop_back();
   }
}

// The default tile allocator
DtedTile* DtedTileManager::DefaultTileAllocator()
{
   return new DtedTile();
}

// Load the tile containing the specified location.  The tile may be
// loaded either from disk or internal cache or may be a dummy tile
// with zero elevation if no data exists for the specified location.
//
// The return value is a pointer to the tile.  The caller *MUST NOT*
// 'delete' the tile as it is a reference counted object that could
// be used by something else.  When the caller no longer requires
// access to the tile it should call 'UnloadTile(tilePtr)' to decrement
// the reference count and release memory if necessary.

DtedTile* DtedTileManager::LoadTile(double aLat, double aLon)
{
   DtedDir*  dirPtr  = nullptr;
   DtedTile* tilePtr = nullptr;

   // Search the list of directories in reverse order.  This way later entries
   // can override newer entries.
   //
   // NOTE: Remember that the first entry is a 'catch-all' entry and spans the
   //       entire earth.  If no other entry gets picked then this one will be
   //       selected and will cause a dummy tile to be returned.

   int dirIndex = static_cast<int>(mDirVector.size()) - 1;
   while (dirIndex >= 0)
   {
      dirPtr = mDirVector[dirIndex];
      if ((aLat >= dirPtr->GetSWLat()) && (aLat <= dirPtr->GetNELat()) && (aLon >= dirPtr->GetSWLon()) &&
          (aLon <= dirPtr->GetNELon()))
      {
         break; // found a directory
      }
      --dirIndex;
   }
   assert(dirIndex >= 0);

   // Either load the cell from cache or from the directory

   std::lock_guard<std::recursive_mutex> lock(mCacheLock);
   if (dirPtr)
   {
      int      row          = dirPtr->GetRow(aLat);
      int      col          = dirPtr->GetCol(aLon);
      GeoTile* cacheTilePtr = mCachePtr->UseTile(dirPtr, row, col);
      if (cacheTilePtr != nullptr)
      {
         // Tile was in the cache... no need to reload it...
         tilePtr = reinterpret_cast<DtedTile*>(cacheTilePtr);
      }
      else
      {
         // Tile is not in cache... it must be loaded
         tilePtr = (*mTileAllocator)();
         dirPtr->LoadTile(tilePtr, row, col);
         mCachePtr->AddTile(tilePtr);
         if (mTileCacheSize >= 0)
         {
            mCachePtr->PurgeInactive(static_cast<unsigned int>(mTileCacheSize));
         }
         else if (mMaxDataSize > 0.0)
         {
            mCachePtr->PurgeInactive(mMaxDataSize);
         }
      }
   }


   return tilePtr;
}

DtedTile* DtedTileManager::GetCachedTile(double aLat, double aLon) const
{
   DtedDir*  dirPtr  = nullptr;
   DtedTile* tilePtr = nullptr;

   // Search the list of directories in reverse order.  This way later entries
   // can override newer entries.
   //
   // NOTE: Remember that the first entry is a 'catch-all' entry and spans the
   //       entire earth.  If no other entry gets picked then this one will be
   //       selected and will cause a dummy tile to be returned.

   int dirIndex = static_cast<int>(mDirVector.size()) - 1;
   while (dirIndex >= 0)
   {
      dirPtr = mDirVector[dirIndex];
      if ((aLat >= dirPtr->GetSWLat()) && (aLat <= dirPtr->GetNELat()) && (aLon >= dirPtr->GetSWLon()) &&
          (aLon <= dirPtr->GetNELon()))
      {
         break; // found a directory
      }
      --dirIndex;
   }
   assert(dirIndex >= 0);

   std::lock_guard<std::recursive_mutex> lock(mCacheLock);
   if (dirPtr)
   {
      int      row          = dirPtr->GetRow(aLat);
      int      col          = dirPtr->GetCol(aLon);
      GeoTile* cacheTilePtr = mCachePtr->UseTile(dirPtr, row, col);
      if (cacheTilePtr != nullptr)
      {
         // Tile was in the cache... no need to reload it...
         tilePtr = reinterpret_cast<DtedTile*>(cacheTilePtr);
      }
   }


   return tilePtr;
}

// Indicate the specified tile is no longer needed.  The reference
// count is decremented.  If the reference count is zero the tile
// *MAY* be unloaded from memory (depending on internal caching).

void DtedTileManager::UnloadTile(DtedTile* aTilePtr)
{
   if (aTilePtr != nullptr)
   {
      std::lock_guard<std::recursive_mutex> lock(mCacheLock);
      mCachePtr->ReleaseTile(aTilePtr);
      if (mTileCacheSize >= 0)
      {
         mCachePtr->PurgeInactive(static_cast<unsigned int>(mTileCacheSize));
      }
      else if (mMaxDataSize > 0.0)
      {
         mCachePtr->PurgeInactive(mMaxDataSize);
      }
   }
}

// virtual
GeoElevationTile* DtedTileManager::LoadElevationTile(double aLat, double aLon)
{
   return static_cast<GeoElevationTile*>(LoadTile(aLat, aLon));
}

// virtual
void DtedTileManager::UnloadElevationTile(GeoElevationTile* aTilePtr)
{
   UnloadTile(dynamic_cast<DtedTile*>(aTilePtr));
}
