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

#include "GeotiffTileManager.hpp"

#include "UtException.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "UtPath.hpp"
#include "UtPathParser.hpp"
#include "UtTiff.hpp"

Geodata::GeotiffTileManager::GeotiffTileManager()
   : mDefaultTile(std::numeric_limits<size_t>::max())
{
   mDefaultTile.MakeDummyTile(-90, -180, 180, 360);
}

bool Geodata::GeotiffTileManager::AddDirectory(const std::string& aPath)
{
   bool                     retval = false;
   UtPath                   p(aPath);
   std::vector<std::string> filelist;
   std::vector<std::string> dirlist;
   p.ListDir(filelist, dirlist);
   // will not seek recursively as implemented we might have a different interface for TMS

   for (auto& file : filelist)
   {
      std::string extension = UtPathParser::GetExtension(file);
      if ((extension == ".tif") || (extension == ".tiff"))
      {
         if (AddFile(aPath + '/' + file))
         {
            retval = true;
         }
         else
         {
            return false;
         }
      }
   }
   if (!retval)
   {
      auto out = ut::log::error() << " No usable geotiffs were found in the provided directory.";
      out.AddNote() << "Directory: " << aPath;
   }
   return retval;
}

void Geodata::GeotiffTileManager::RemoveDirectories()
{
   std::lock_guard<std::recursive_mutex> lock(mCacheLock);
   mCache.Reset();
   mTileMap.clear();
   mSP_Tree.reset();
}

bool Geodata::GeotiffTileManager::AddFile(const std::string& aPath)
{
   if (UtTiff::IsTiffFile(aPath)) // is this a tiff file
   {
      UtTiff::PreviewList pl = UtTiff::PreviewHeaderInformation(aPath);
      if (!pl.empty())
      {
         if (pl[0].mIsGeotiff) // is this a geotiff file
         {
            if (pl[0].mIsEquirectangular) // this is a lat/lon projected geotiff file
            {
               if (pl[0].mCompression == UtTiff::cNone)
               {
                  if (pl[0].mIsAFSIM_Usable) // the projection is supported by AFSIM
                  {
                     size_t assignedIndex = mTileMap.size();
                     mTileMap.emplace_back(aPath,
                                           pl[0].mImageWidth,
                                           pl[0].mImageHeight,
                                           pl[0].mLatMin,
                                           pl[0].mLatMax,
                                           pl[0].mLonMin,
                                           pl[0].mLonMax); // create an entry in the map
                     // pull the constraints and make an edge list
                     std::list<SP_Edge> edgelist;
                     edgelist.emplace_back(true, true, pl[0].mLonMin);
                     edgelist.emplace_back(true, false, pl[0].mLonMax);
                     edgelist.emplace_back(false, true, pl[0].mLatMin);
                     edgelist.emplace_back(false, false, pl[0].mLatMax);

                     // add it to the spatial partition tree
                     if (mSP_Tree)
                     {
                        mSP_Tree->InsertZone(assignedIndex, edgelist);
                     }
                     else
                     {
                        mSP_Tree =
                           ut::make_unique<SimpleSP_TreeNode>(assignedIndex, std::numeric_limits<size_t>::max(), edgelist);
                     }
                     return true;
                  }
                  else
                  {
                     auto out = ut::log::error() << "The projection of the geotiff file is not supported by AFSIM.";
                     out.AddNote() << "File: " << aPath;
                     return false;
                  }
               }
               else
               {
                  auto out = ut::log::error() << "The compression of the geotiff file is not supported by AFSIM.";
                  out.AddNote() << "File: " << aPath;
                  return false;
               }
            }
            else
            {
               auto out = ut::log::error() << "The projection of the geotiff file is not supported by AFSIM.";
               out.AddNote() << "GTModelTypeGeoKey does not indicate a latitude-longitude System" << aPath;
               out.AddNote() << "File: " << aPath;
               return false;
            }
         }
         else
         {
            auto out = ut::log::error() << "The provided file is not recognizable as a geotiff file.";
            out.AddNote() << "File: " << aPath;
            return false;
         }
      }
      else
      {
         auto out = ut::log::error() << "The provided file has no image layers.";
         out.AddNote() << "File: " << aPath;
         return false;
      }
   }
   else
   {
      auto out = ut::log::error() << "The provided file is not recognizable as a tiff file.";
      out.AddNote() << "File: " << aPath;
      return false;
   }
   // provided file is not usable as a geotiff
   return false;
}

GeoElevationTile* Geodata::GeotiffTileManager::LoadElevationTile(double aLat, double aLon)
{
   return static_cast<GeoElevationTile*>(LoadTile(aLat, aLon));
}

void Geodata::GeotiffTileManager::UnloadElevationTile(GeoElevationTile* aTilePtr)
{
   UnloadTile(dynamic_cast<GeotiffTile*>(aTilePtr));
}

Geodata::GeotiffTile* Geodata::GeotiffTileManager::GetCachedTile(double aLat, double aLon) const
{
   if (mSP_Tree)
   {
      size_t index = mSP_Tree->LookUpIndex(aLon, aLat);
      if (index <= mTileMap.size())
      {
         GeotiffTile* cacheTilePtr = mCache.UseTile(index);
         if (cacheTilePtr != nullptr)
         {
            return cacheTilePtr;
         }
      }
   }
   return nullptr;
}

Geodata::GeotiffTile* Geodata::GeotiffTileManager::LoadTile(double aLat, double aLon)
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
         GeotiffTile* cacheTilePtr = mCache.UseTile(index);
         if (cacheTilePtr != nullptr)
         {
            return cacheTilePtr;
         }
         else
         {
            // Tile is not in cache... it must be loaded
            std::unique_ptr<GeotiffTile> tilePtr = ut::make_unique<GeotiffTile>(index);
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
               GeotiffTile* cacheTilePtr = mCache.UseTile(index);
               if (cacheTilePtr != nullptr)
               {
                  return cacheTilePtr;
               }
            }
            break;
            case (1): // cannot open
               throw UtException("Geodata::GeotiffTile, could not open geotiff tile: " + mTileMap.at(index).mFilename);
               break;
            case (2): // bad data
               throw UtException("Geodata::GeotiffTile, bad data in geotiff tile: " + mTileMap.at(index).mFilename);
               break;
            }
         }
      }
   }
   return &mDefaultTile;
}

void Geodata::GeotiffTileManager::UnloadTile(GeotiffTile* aTilePtr)
{
   if (aTilePtr != nullptr)
   {
      if (aTilePtr->GetTileIndex() == mDefaultTile.GetTileIndex())
      {
         return;
      }
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

bool Geodata::GeotiffTileManager::IsBestTile(GeotiffTile* aTile, double aLat, double aLon) const
{
   if (aTile)
   {
      if (!mSP_Tree) // no data
      {
         return (aTile->GetGeotiffIndex() == mDefaultTile.GetGeotiffIndex());
      }
      else
      {
         return (mSP_Tree->LookUpIndex(aLon, aLat) == aTile->GetGeotiffIndex());
      }
   }
   return false;
}
