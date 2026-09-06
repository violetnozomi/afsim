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

#ifndef BATHYMETRYXYZTILE_HPP
#define BATHYMETRYXYZTILE_HPP

#include "geodata_export.h"

#include <list>
#include <map>
#include <memory>
#include <string>
#include <utility>

// Base class
#include "GeoElevationTile.hpp"

namespace Geodata
{
class BathymetryXYZ_TileManager;

//! A class representing a file containing bathymetry in the ASCII XYZ format.
//! XYZ format is a three column table with longitude (x), latitude (y), and elevation (z).
//! Each point is average center of a grid cell. The spacing between each cell must be constant.
//! Units are decimal degrees, decimal degrees, meters.
//! By default a east-north-up coordinate system.
//! References: noaa.gov
//!             gdal.org (ASCII Gridded XYZ) (https://gdal.org/drivers/raster/xyz.html)
class GEODATA_EXPORT BathymetryXYZ_Tile : public GeoElevationTile
{
public:
   //! Constructor and destructor
   explicit BathymetryXYZ_Tile(size_t aIndex);
   ~BathymetryXYZ_Tile() override = default;

   struct TileData
   {
      TileData(const std::string& aFilename)
         : mFilename(aFilename)
      {
      }
      std::string               mFilename;
      std::pair<double, double> mLatRange;
      std::pair<double, double> mLonRange;
      bool                      mAscendingLatitudes{false};
   };

   static bool CheckTile(TileData& aData);

   // Load the cell from the specified file.
   //
   // aStatus = 0 if success.
   //           1 if the file cannot be opened.
   //           2 if the contents appear bad.
   int LoadTile(const TileData& aData);
   int LoadTile(const char* aFilename) override { return 1; }

   size_t GetBathymetryIndex() const { return mBathymetryIndex; }

private:
   // 0 string is not a header
   // 1 if string is a header and is valid (x,y,z)
   // 2 if string is a header but contains unexpected info
   static int CheckHeader(const std::string& aHeader);

   static void ProcessLine(std::string& aLine, float& aLatitude, float& aLongitude, float& aDepth);

   size_t mBathymetryIndex;
};

class GEODATA_EXPORT BathymetryXYZ_TileCache
{
public:
   BathymetryXYZ_TileCache()                               = default;
   BathymetryXYZ_TileCache(BathymetryXYZ_TileCache const&) = delete;
   BathymetryXYZ_TileCache(BathymetryXYZ_TileCache&&)      = delete;
   BathymetryXYZ_TileCache& operator=(BathymetryXYZ_TileCache const&) = delete;
   BathymetryXYZ_TileCache& operator=(BathymetryXYZ_TileCache&&) = delete;

   BathymetryXYZ_Tile* UseTile(size_t aIndex);
   void                ReleaseTile(BathymetryXYZ_Tile* aTilePtr);
   void                PurgeInactive(double aMaxDataSize); // is this a double just to deconflict the signatures, here?
                                            // this affects process input, so the code is stuck with it for now
   void PurgeInactive(unsigned int aTileCount);
   void AddTile(std::unique_ptr<BathymetryXYZ_Tile> aTilePtr);

   void Reset();

private:
   std::map<size_t, std::unique_ptr<BathymetryXYZ_Tile>> mCache;
   std::list<size_t>                                     mInactive;
   double                                                mSizeInBytes{0.0};
};
} // namespace Geodata

#endif
