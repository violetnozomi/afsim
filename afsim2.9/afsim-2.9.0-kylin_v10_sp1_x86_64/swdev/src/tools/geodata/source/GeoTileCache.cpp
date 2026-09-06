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

// GeoTileCache maintains a 'cache' of reference-counted 'GeoTile' objects.
//
// When an object is first loaded into memory, a cache entry is created and
// the use count for the object is incremented (from 0 to 1).  When the caller
// no longer requires access to the object it 'releases' the object which
// decrements the use count by one.  When the use count goes to zero, it is
// moved to the inactive list.  When a the 'PurgeInactive' method is called
// a specified number of entries are retained in the inactive list and the
// remainder are deleted.
//
// Note that when an entry is on the inactive list it is still in the cache.
// The inactive list is simply a mechanism that makes it easy to decide which
// entries are to be deleted.  When an entry is moved to the inactive list it
// is put at the FRONT of the list.  Entries are deleted from the END of the
// list, so entries that have been on the list the longest are deleted.

#include "GeoTileCache.hpp"

#include <cassert>
#include <utility>

#include "GeoTile.hpp"
#include "GeoTileMatrix.hpp"
#include "GeoTileName.hpp"
#include "UtLog.hpp"

//#define DEBUG_ON

GeoTileCache::GeoTileCache()
   : mCache()
   , mInactive()
   , mSizeInBytes(0.0)
   , mNumInactive(0)
{
}

GeoTileCache::~GeoTileCache()
{
   for (std::map<GeoTileName, GeoTile*>::iterator cacheIter = mCache.begin(); cacheIter != mCache.end(); ++cacheIter)
   {
      GeoTile* tilePtr = (*cacheIter).second;
#ifdef DEBUG_ON
      { // RAII block
         auto out = ut::log::debug() << "Delete tile:";
         out.AddNote() << "From: " << tilePtr->GetSWLat() << ", " << tilePtr->GetSWLon();
         out.AddNote() << "To: " << tilePtr->GetNELat() << ", " << tilePtr->GetNELon();
      }
#endif
      delete tilePtr;
   }
}

// A new tile has been loaded into memory.  Allocate a cache entry and
// initialize the use count for the tile to 1.
//
// NOTE: The tile becomes the property of the GeoTileCache.  It will
//       delete the object when necessary.

void GeoTileCache::AddTile(GeoTile* aTilePtr)
{
   unsigned int useCount = aTilePtr->IncrementUseCount();
   if (useCount == 1)
   {
      GeoTileName tileName(aTilePtr->GetTileMatrix(), aTilePtr->GetTileIndex());
      mCache.insert(std::pair<const GeoTileName, GeoTile*>(tileName, aTilePtr));
      mSizeInBytes += aTilePtr->GetDataSize();
#ifdef DEBUG_ON
      auto out = ut::log::debug() << "Add tile:";
      out.AddNote() << "From: " << aTilePtr->GetSWLat() << ", " << aTilePtr->GetSWLon();
      out.AddNote() << "To: " << aTilePtr->GetNELat() << ", " << aTilePtr->GetNELon();
      out.AddNote() << "Use Count: " << useCount;
#endif
   }
}

// Purge tiles from the inactive list.  Tiles whose use count are zero
// may be released from memory if conditions warrant.

void GeoTileCache::PurgeInactive(double aMaxDataSize)
{
   if (mSizeInBytes > aMaxDataSize)
   {
      std::list<GeoTile*>::iterator li = mInactive.end();
      while ((mSizeInBytes > aMaxDataSize) && (mNumInactive > 0))
      {
         --li;
         --mNumInactive;
         GeoTile* tilePtr = *li;
#ifdef DEBUG_ON
         { // RAII block
            auto out = ut::log::debug() << "Purge tile:";
            out.AddNote() << "From: " << tilePtr->GetSWLat() << ", " << tilePtr->GetSWLon();
            out.AddNote() << "To: " << tilePtr->GetNELat() << ", " << tilePtr->GetNELon();
         }
#endif
         GeoTileName                               tileName(tilePtr->GetTileMatrix(), tilePtr->GetTileIndex());
         std::map<GeoTileName, GeoTile*>::iterator cacheIter = mCache.find(tileName);
         if (cacheIter != mCache.end())
         {
            mCache.erase(cacheIter);
         }
         mSizeInBytes -= tilePtr->GetDataSize();
         delete tilePtr;
      }
      mInactive.erase(li, mInactive.end());

      assert(mNumInactive == mInactive.size());
   }
}

// Purge tiles from the inactive list.  Tiles whose use count are zero
// may be released from memory if conditions warrant.

void GeoTileCache::PurgeInactive(unsigned int aKeepCount)
{
   assert(mNumInactive == mInactive.size());

   if (mNumInactive > aKeepCount)
   {
      std::list<GeoTile*>::iterator li = mInactive.end();
      while (mNumInactive > aKeepCount)
      {
         --li;
         --mNumInactive;
         GeoTile* tilePtr = *li;
#ifdef DEBUG_ON
         { // RAII block
            auto out = ut::log::debug() << "Purge tile:";
            out.AddNote() << "From: " << tilePtr->GetSWLat() << ", " << tilePtr->GetSWLon();
            out.AddNote() << "To: " << tilePtr->GetNELat() << ", " << tilePtr->GetNELon();
         }
#endif
         GeoTileName                               tileName(tilePtr->GetTileMatrix(), tilePtr->GetTileIndex());
         std::map<GeoTileName, GeoTile*>::iterator cacheIter = mCache.find(tileName);
         if (cacheIter != mCache.end())
         {
            mCache.erase(cacheIter);
         }
         mSizeInBytes -= tilePtr->GetDataSize();
         delete tilePtr;
      }
      mInactive.erase(li, mInactive.end());

      assert(mNumInactive == mInactive.size());
   }
}

// A reference to a tile is no longer required.  Decrement the use count
// for the tile.

void GeoTileCache::ReleaseTile(GeoTile* aTilePtr)
{
   unsigned int useCount = aTilePtr->DecrementUseCount();
#ifdef DEBUG_ON
   { // RAII block
      auto out = ut::log::debug() << "Release tile:";
      out.AddNote() << "From: " << aTilePtr->GetSWLat() << ", " << aTilePtr->GetSWLon();
      out.AddNote() << "To: " << aTilePtr->GetNELat() << ", " << aTilePtr->GetNELon();
      out.AddNote() << "Use Count: " << useCount;
   }
#endif
   if (useCount == 0)
   {
      mInactive.push_front(aTilePtr); // Add to inactive list...
      ++mNumInactive;
   }
}

// Attempt to find a cache entry for the specified tile.  If found, the
// use count for the tile will be incremented and a pointer to the tile
// will be returned.  If the tile is not in the cache then zero will be
// returned.

GeoTile* GeoTileCache::UseTile(GeoTileMatrix* aTileMatrixPtr, unsigned int aRow, unsigned int aCol)
{
   unsigned int                              tileIndex = (aRow * aTileMatrixPtr->GetNumCols()) + aCol;
   GeoTileName                               tileName(aTileMatrixPtr, tileIndex);
   GeoTile*                                  tilePtr   = nullptr;
   std::map<GeoTileName, GeoTile*>::iterator cacheIter = mCache.find(tileName);
   if (cacheIter != mCache.end())
   {
      tilePtr               = (*cacheIter).second;
      unsigned int useCount = tilePtr->IncrementUseCount();
#ifdef DEBUG_ON
      { // RAII block
         auto out = ut::log::debug() << "Release tile:";
         out.AddNote() << "From: " << tilePtr->GetSWLat() << ", " << tilePtr->GetSWLon();
         out.AddNote() << "To: " << tilePtr->GetNELat() << ", " << tilePtr->GetNELon();
         out.AddNote() << "Use Count: " << useCount;
      }
#endif
      if (useCount == 1)
      {
         // When the use count goes from 0 to 1 it means the tile is going
         // from inactive to active.  Therefore we must remove it from the
         // inactive list...
         mInactive.remove(tilePtr);
         assert(mNumInactive != 0);
         --mNumInactive;
      }
   }
   return tilePtr;
}
