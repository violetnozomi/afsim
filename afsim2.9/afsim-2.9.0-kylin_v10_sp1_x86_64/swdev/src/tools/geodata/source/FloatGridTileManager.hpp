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

#ifndef FLOATGRIDTILEMANAGER_HPP
#define FLOATGRIDTILEMANAGER_HPP

#include "geodata_export.h"

// This is the tile manager for managing directories containing NIMA Digital
// Terrain Elevation Data (Dem).

#include <map>
#include <mutex>
#include <string>
#include <vector>

class FloatGridDir;
#include "FloatGridTile.hpp"
#include "GeoElevationTileManager.hpp"
#include "GeoRect.hpp"
class GeoTileCache;

//! A tile manager for "grid-float" formatted data.  Currently we assume
//! the data are stored in the standard format of
class GEODATA_EXPORT FloatGridTileManager : public GeoElevationTileManager
{
public:
   // The optional single argument to the constructor is the address of
   // a function that will be called to instantiate an object of type
   // 'FloatGridTile' or a class derived from it.  This allows the caller
   // to maintain extra information with the tile (texture name, etc)
   // that is application specific.

   FloatGridTileManager(FloatGridTile* (*aTileAllocator)() = nullptr);

   ~FloatGridTileManager() override;

   // Required virtual methods:
   GeoElevationTile* LoadElevationTile(double aLat, double aLon) override;

   void UnloadElevationTile(GeoElevationTile* aTilePtr) override;

   // Add a 'raw' directory to be considered when looking for Dem.  The
   // directory should contain the longitudinal directories as its direct
   // descendants.
   //
   // The return value is 0 if successful or non-zero on failure.
   int AddDirectory(const char* aDirName, double aSWLat, double aSWLon, double aNELat, double aNELon);

   // Add a single float grid file.
   int AddFile(const char* aFileName);

   // Load the tile containing the specified location.  The tile may be
   // loaded either from disk or internal cache or may be a dummy tile
   // with zero elevation if no data exists for the specified location.
   //
   // The return value is a pointer to the tile.  The caller *MUST NOT*
   // 'delete' the tile as it is a reference counted object that could
   // be used by something else.  When the caller no longer requires
   // access to the tile it should call 'UnloadTile(tilePtr)' to decrement
   // the reference count and release memory if necessary.

   FloatGridTile* LoadTile(double aLat, double aLon);


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
   FloatGridTile* GetCachedTile(double aLat, double aLon) const override;

   // Indicate the specified tile is no longer needed.  The reference
   // count is decremented.  If the reference count is zero the tile
   // *MAY* be unloaded from memory (depending on internal caching).

   void UnloadTile(FloatGridTile* aTilePtr);

private:
   // The default tile allocator
   static FloatGridTile* DefaultTileAllocator();

   // Address of the routine to allocate tiles...
   FloatGridTile* (*mTileAllocator)();

   // Points to directory that contain Grid-Float
   FloatGridDir*  mDirPtr;
   GeoTileCache*  mCachePtr;
   FloatGridTile* mCurrentFilePtr;

   typedef std::map<std::string, FloatGridTile*> FileNameToTilePtrMap;
   FileNameToTilePtrMap                          mFileNameToTilePtrMap;

   mutable std::recursive_mutex mCacheLock;
};

#endif
