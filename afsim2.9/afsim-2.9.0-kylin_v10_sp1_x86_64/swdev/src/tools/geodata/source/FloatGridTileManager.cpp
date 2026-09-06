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

#include "FloatGridTileManager.hpp"

#include <cassert>
#include <cstdlib>
#include <sstream>

#include "FloatGridDir.hpp"
#include "GeoTileCache.hpp"
#include "UtException.hpp"
#include "UtLog.hpp"

FloatGridTileManager::FloatGridTileManager(FloatGridTile* (*aTileAllocator)())
   : mTileAllocator(aTileAllocator)
   , mDirPtr(nullptr)
   , mCachePtr(new GeoTileCache())
   , mCurrentFilePtr(nullptr)
   , mCacheLock()
{
   // Use the default tile allocator if none was provided...
   if (mTileAllocator == nullptr)
   {
      mTileAllocator = DefaultTileAllocator;
   }

   // Allow the tile cache size to be overriden by an environment variable
   char* envPtr = ::getenv("FLOAT_GRID_TILE_CACHE_SIZE");
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
FloatGridTileManager::~FloatGridTileManager()
{
   // Tiles are deleted when cache is deleted.
   delete mCachePtr;
   delete mDirPtr;
}

// Add a 'raw' directory to be considered when looking for Dem.  The
// directory should contain the longitudinal directories as its direct
// descendents.
//
// The return value is 0 if successful or non-zero on failure.
int FloatGridTileManager::AddDirectory(const char* aDirName, double aSWLat, double aSWLon, double aNELat, double aNELon)
{
   int error = 0;
   if (mDirPtr == nullptr)
   {
      mDirPtr = new FloatGridDir(aDirName, aSWLat, aSWLon, aNELat, aNELon, error);
   }
   else
   {
      ut::log::error() << "FloatGridTileManager: More than one float-grid directory is not supported.";
      error = 5;
   }
   return error;
}

// The default tile allocator
FloatGridTile* FloatGridTileManager::DefaultTileAllocator()
{
   return new FloatGridTile();
}

//! For adding specific files, we use a generic directory.
//! Use the tiles themselves, and load the header first to later determine whether to load the rest.
int FloatGridTileManager::AddFile(const char* aFileName)
{
   int error = 0;
   if ((mDirPtr != nullptr) && (mDirPtr->GetDirName() != ""))
   {
      ut::log::error() << "FloatGridTileManager: Mixed use of both individual files and directories is not supported.";
      error = 5;
   }
   else
   {
      if (mDirPtr == nullptr)
      {
         mDirPtr = new FloatGridDir();
      }
      FloatGridTile* mTilePtr = new FloatGridTile();
      std::string    fileName = std::string(aFileName); // + ".hdr";
      error                   = mTilePtr->LoadHeader(fileName);
      if (error != 0)
      {
         auto out = ut::log::error() << "FloatGridTileManager: File not found.";
         out.AddNote() << "File: " << fileName;

         delete mTilePtr;
      }
      else
      {
         mFileNameToTilePtrMap.insert(std::make_pair(aFileName, mTilePtr));
      }
   }
   return error;
}


FloatGridTile* FloatGridTileManager::GetCachedTile(double aLat, double aLon) const
{
   // load the cell from cache

   std::lock_guard<std::recursive_mutex> lock(mCacheLock);

   // currently only one dir setting
   if (mDirPtr != nullptr)
   {
      if ((aLat >= mDirPtr->GetSWLat()) && (aLat < mDirPtr->GetNELat()) && (aLon >= mDirPtr->GetSWLon()) &&
          (aLon < mDirPtr->GetNELon()))
      {
         int row = mDirPtr->GetRow(aLat);
         int col = mDirPtr->GetCol(aLon);

         GeoTile* cacheTilePtr = mCachePtr->UseTile(mDirPtr, row, col);

         if (cacheTilePtr != nullptr)
         {
            // Tile was in the cache... no need to reload it...
            FloatGridTile* tilePtr = reinterpret_cast<FloatGridTile*>(cacheTilePtr);
            assert(tilePtr->Contains(aLat, aLon) == 1);
            return tilePtr;
         }
      }
      else
      {
         std::ostringstream str;
         str << "Float Grid- out of bounds for lat: " << aLat << ", lon: " << aLon;
         throw(UtException(str.str()));
      }
   }
   return nullptr;
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
FloatGridTile* FloatGridTileManager::LoadTile(double aLat, double aLon)
{
   FloatGridTile* tilePtr = nullptr;

   // Either load the cell from cache or from the directory

   std::lock_guard<std::recursive_mutex> lock(mCacheLock);

   // currently only one dir setting
   if (mDirPtr != nullptr)
   {
      int      row          = mDirPtr->GetRow(aLat);
      int      col          = mDirPtr->GetCol(aLon);
      GeoTile* cacheTilePtr = nullptr;

      if ((aLat >= mDirPtr->GetSWLat()) && (aLat < mDirPtr->GetNELat()) && (aLon >= mDirPtr->GetSWLon()) &&
          (aLon < mDirPtr->GetNELon()))
      {
         cacheTilePtr = mCachePtr->UseTile(mDirPtr, row, col);
      }
      else
      {
         std::ostringstream str;
         str << "Float Grid- out of bounds for lat: " << aLat << ", lon: " << aLon;
         throw(UtException(str.str()));
      }

      if (cacheTilePtr != nullptr)
      {
         // Tile was in the cache... no need to reload it...
         tilePtr = reinterpret_cast<FloatGridTile*>(cacheTilePtr);
         assert(tilePtr->Contains(aLat, aLon) == 1);
      }
      else
      {
         // Tile is not in cache; it must be loaded
         if (mDirPtr->GetDirName() != "") // Using float-grid directory structure
         {
            tilePtr = new FloatGridTile();
            mDirPtr->LoadTile(tilePtr, aLat, aLon);
         }
         else
         {
            // Using individual files; check each individual file for specified region
            FileNameToTilePtrMap::iterator iter = mFileNameToTilePtrMap.begin();
            assert(tilePtr == nullptr);
            double swLat = (int)(aLat * 10.0) / 10.0;
            double swLon = (int)(aLon * 10.0) / 10.0;
            if (swLat < 0.0)
            {
               swLat -= 0.1;
            }
            if (swLon < 0.0)
            {
               swLon -= 0.1;
            }
            GeoRect tileBoundary(swLat, swLon, swLat + 0.1, swLon + 0.1);

            // We need to make sure we look at all files when creating a new subtile,
            // As more than one irregular tile may contribute to the data.
            while (iter != mFileNameToTilePtrMap.end())
            {
               FloatGridTile* irregularTilePtr = iter->second;
               if (irregularTilePtr->Intersects(tileBoundary))
               {
                  // This is the correct tile.
                  if (!irregularTilePtr->IsLoaded())
                  {
                     irregularTilePtr->LoadData();
                  }
                  irregularTilePtr->FillSubtile(tileBoundary, tilePtr);
               }
               ++iter;
            }

            // Finally if still no tile, create a dummy tile:
            if (tilePtr == nullptr)
            {
               tilePtr = new FloatGridTile();
               tilePtr->MakeDummyTile(aLat, aLon);
            }

            // In either case, register this tile with the directory.
            mDirPtr->RegisterTile(*tilePtr);
         }
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

// Indicate the specified tile is no longer needed.  The reference
// count is decremented.  If the reference count is zero the tile
// *MAY* be unloaded from memory (depending on internal caching).

void FloatGridTileManager::UnloadTile(FloatGridTile* aTilePtr)
{
   if (aTilePtr != nullptr)
   {
      std::lock_guard<std::recursive_mutex> lock(mCacheLock);
      if (mDirPtr != nullptr)
      {
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
}

// virtual
GeoElevationTile* FloatGridTileManager::LoadElevationTile(double aLat, double aLon)
{
   return static_cast<GeoElevationTile*>(LoadTile(aLat, aLon));
}

// virtual
void FloatGridTileManager::UnloadElevationTile(GeoElevationTile* aTilePtr)
{
   UnloadTile(dynamic_cast<FloatGridTile*>(aTilePtr));
}
