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

#ifndef DTEDTILEMANAGER_HPP
#define DTEDTILEMANAGER_HPP

#include "geodata_export.h"

// This is the tile manager for managing directories containing NIMA Digital
// Terrain Elevation Data (DTED).

#include <mutex>
#include <vector>

#include "DtedTile.hpp"
class DtedDir;
class GeoRect;
class GeoTileCache;
#include "GeoElevationTileManager.hpp"

class GEODATA_EXPORT DtedTileManager : public GeoElevationTileManager
{
public:
   // The optional single argument to the constructor is the address of
   // a function that will be called to instantiate an object of type
   // 'DtedTile' or a class derived from it.  This allows the caller
   // to maintain extra information with the tile (texture name, etc)
   // that is application specific.

   DtedTileManager(DtedTile* (*aTileAllocator)() = nullptr);

   ~DtedTileManager() override;

   //! Raw directory access (used by External Services and the directory checker)
   //@{
   virtual size_t GetDtedDirectoryCount() { return mDirVector.size(); }

   //! Return the directory with the specified index.
   //! @param aIndex The index of the directory required.
   //!               The value must be in the range [0 .. GetDtedDirectoryCount()-1].
   //! @returns      The pointer to the directory.
   //! @note Directory index 0 is a dummy directory and doesn't contain any usable data.
   virtual DtedDir* GetDtedDirectory(size_t aIndex) { return mDirVector[aIndex]; }
   //@}

   // Required virtual methods:
   GeoElevationTile* LoadElevationTile(double aLat, double aLon) override;

   void UnloadElevationTile(GeoElevationTile* aTilePtr) override;

   // Add a directory to be considered when looking for DTED.  The
   // directory *MUST* contain the files/subdirectories as they would
   // on a NIMA CD (i.e.: dmed, dted, gazette, text).
   //
   // The return value is 0 if successful or non-zero on failure.

   int AddDirectory(const char* aDirName, int aLevel);

   // Add a 'raw' directory to be considered when looking for DTED.  The
   // directory should contain the longitudinal directories as its direct
   // descendants.
   //
   // The return value is 0 if successful or non-zero on failure.

   int AddDirectory(const char* aDirName, int aLevel, int aSWLat, int aSWLon, int aNELat, int aNELon);

   //! Remove DTED directories except for the first one added by the constructor.
   void RemoveDirectories();

   // Load the tile containing the specified location.  The tile may be
   // loaded either from disk or internal cache or may be a dummy tile
   // with zero elevation if no data exists for the specified location.
   //
   // The return value is a pointer to the tile.  The caller *MUST NOT*
   // 'delete' the tile as it is a reference counted object that could
   // be used by something else.  When the caller no longer requires
   // access to the tile it should call 'UnloadTile(tilePtr)' to decrement
   // the reference count and release memory if necessary.

   DtedTile* LoadTile(double aLat, double aLon);

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
   DtedTile* GetCachedTile(double aLat, double aLon) const override;

   // Indicate the specified tile is no longer needed.  The reference
   // count is decremented.  If the reference count is zero the tile
   // *MAY* be unloaded from memory (depending on internal caching).

   void UnloadTile(DtedTile* aTilePtr);

private:
   // The default tile allocator
   static DtedTile* DefaultTileAllocator();

   // Points to directories that contain DTED data.
   std::vector<DtedDir*> mDirVector;

   GeoTileCache* mCachePtr;

   // Address of the routine to allocate tiles...
   DtedTile* (*mTileAllocator)();

   mutable std::recursive_mutex mCacheLock;
};

#endif
