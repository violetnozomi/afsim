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

#include "BathymetryXYZ_TileManager.hpp"

#include <algorithm>

// Utilities
#include "UtException.hpp"
#include "UtMemory.hpp"

// =================================================================================================
Geodata::BathymetryXYZ_TileManager::BathymetryXYZ_TileManager()
   : GeoElevationTileManager()
   , mCacheLock()
   , mDefaultTile(std::numeric_limits<size_t>::max())
{
   mDefaultTile.MakeDummyTile(-90, -180, 180, 360);
}

Geodata::BathymetryXYZ_TileManager::~BathymetryXYZ_TileManager()
{
   // Tiles are deleted when cache is deleted if they have been loaded
}

// =================================================================================================
GeoElevationTile* Geodata::BathymetryXYZ_TileManager::LoadElevationTile(double aLat, double aLon)
{
   return static_cast<GeoElevationTile*>(LoadTile(aLat, aLon));
}

// =================================================================================================
void Geodata::BathymetryXYZ_TileManager::UnloadElevationTile(GeoElevationTile* aTilePtr)
{
   UnloadTile(dynamic_cast<BathymetryXYZ_Tile*>(aTilePtr));
}

// =================================================================================================
GeoElevationTile* Geodata::BathymetryXYZ_TileManager::GetCachedTile(double aLat, double aLon) const
{
   if (mSP_Tree)
   {
      size_t index = mSP_Tree->LookUpIndex(aLon, aLat);
      if (index <= mTileMap.size())
      {
         BathymetryXYZ_Tile* cacheTilePtr = mCache.UseTile(index);
         if (cacheTilePtr != nullptr)
         {
            return cacheTilePtr;
         }
      }
   }
   return nullptr;
}

// =================================================================================================
bool Geodata::BathymetryXYZ_TileManager::AddFile(const std::string& aFileName)
{
   // File is already checked to make sure it exists so add to mapping
   // The file is not loaded yet but a quick check of the contents is performed
   BathymetryXYZ_Tile::TileData tileData(aFileName);
   if (!BathymetryXYZ_Tile::CheckTile(tileData))
   {
      return false;
   }

   size_t assignedIndex = mTileMap.size();
   mTileMap.emplace_back(tileData); // create an entry in the map
                                    // pull the constraints and make an edge list
   std::list<SP_Edge> edgelist;
   edgelist.emplace_back(true, true, tileData.mLonRange.first);
   edgelist.emplace_back(true, false, tileData.mLonRange.second);
   edgelist.emplace_back(false, true, tileData.mLatRange.first);
   edgelist.emplace_back(false, false, tileData.mLatRange.second);

   // add it to the spatial partition tree
   if (mSP_Tree)
   {
      mSP_Tree->InsertZone(assignedIndex, edgelist);
   }
   else
   {
      mSP_Tree = ut::make_unique<SimpleSP_TreeNode>(assignedIndex, std::numeric_limits<size_t>::max(), edgelist);
   }
   return true;
}

// =================================================================================================
Geodata::BathymetryXYZ_Tile* Geodata::BathymetryXYZ_TileManager::LoadTile(double aLat, double aLon)
{
   if (!mSP_Tree) // no data
   {
      return &mDefaultTile;
   }
   else
   {
      size_t index = mSP_Tree->LookUpIndex(aLon, aLat);
      if (index > mTileMap.size()) // no data at the requested location
      {
         return &mDefaultTile;
      }
      else
      {
         // use a cached tile, or load
         BathymetryXYZ_Tile* cacheTilePtr = mCache.UseTile(index);
         if (cacheTilePtr != nullptr)
         {
            return cacheTilePtr;
         }
         else
         {
            // Tile is not in cache... it must be loaded
            std::unique_ptr<BathymetryXYZ_Tile> tilePtr = ut::make_unique<BathymetryXYZ_Tile>(index);
            switch (tilePtr->LoadTile(mTileMap.at(index)))
            {
            case (0):
            {
               mCache.AddTile(std::move(tilePtr));
               if (mTileCacheSize >= 0) // I don't think this has a lot of value when dealing with variable sized tiles
               {
                  mCache.PurgeInactive(static_cast<unsigned int>(mTileCacheSize));
               }
               else if (mMaxDataSize > 0.0)
               {
                  mCache.PurgeInactive(mMaxDataSize);
               }
               cacheTilePtr = mCache.UseTile(index);
               if (cacheTilePtr != nullptr)
               {
                  return cacheTilePtr;
               }
            }
            break;
            case (1): // cannot open
               throw UtException("Geodata::BathymetryXYZ_Tile, could not open bathymetry tile: " +
                                 mTileMap.at(index).mFilename);
               break;
            case (2): // bad data
               throw UtException("Geodata::BathymetryXYZ_Tile, bad data in bathymetry tile: " +
                                 mTileMap.at(index).mFilename);
               break;
            }
         }
      }
   }
   return &mDefaultTile;
}

// =================================================================================================
void Geodata::BathymetryXYZ_TileManager::UnloadTile(BathymetryXYZ_Tile* aTilePtr)
{
   if (aTilePtr != nullptr)
   {
      std::lock_guard<std::recursive_mutex> lock(mCacheLock);
      mCache.ReleaseTile(aTilePtr);
      if (mTileCacheSize >= 0)
      {
         mCache.PurgeInactive(static_cast<unsigned int>(mTileCacheSize));
      }
      else if (mMaxDataSize > 0.0)
      {
         mCache.PurgeInactive(mMaxDataSize);
      }
   }
}
