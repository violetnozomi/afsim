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

#ifndef GEOTIFFTILE_HPP
#define GEOTIFFTILE_HPP

#include "geodata_export.h"

#include <list>
#include <map>
#include <memory>

#include "GeoElevationTile.hpp"
#include "GeoElevationTileManager.hpp"

namespace Geodata
{
class GEODATA_EXPORT GeotiffTile : public GeoElevationTile
{
public:
   GeotiffTile(size_t aIndex);
   ~GeotiffTile() override = default;

   struct TileData
   {
      TileData(const std::string& aFilename,
               const size_t       aWidth,
               const size_t       aHeight,
               const double       aLatMin,
               const double       aLatMax,
               const double       aLonMin,
               const double       aLonMax)
         : mFilename(aFilename)
         , mWidth(aWidth)
         , mHeight(aHeight)
         , mLatRange(aLatMin, aLatMax)
         , mLonRange(aLonMin, aLonMax)
      {
      }
      std::string               mFilename;
      size_t                    mWidth;
      size_t                    mHeight;
      std::pair<double, double> mLatRange;
      std::pair<double, double> mLonRange;
   };

   // Load the cell from the specified file.
   //
   // aStatus = 0 if success.
   //           1 if the file cannot be opened.
   //           2 if the contents appear bad.
   int LoadTile(const TileData& aData);

   int LoadTile(const char* aFilename) override { return 1; }

   size_t GetGeotiffIndex() const { return mGeotiffIndex; }

private:
   size_t mGeotiffIndex;
};

class GEODATA_EXPORT GeotiffTileCache
{
public:
   GeotiffTileCache()                        = default;
   GeotiffTileCache(GeotiffTileCache const&) = delete;
   GeotiffTileCache(GeotiffTileCache&&)      = delete;
   GeotiffTileCache& operator=(GeotiffTileCache const&) = delete;
   GeotiffTileCache& operator=(GeotiffTileCache&&) = delete;

   GeotiffTile* UseTile(size_t aIndex);
   void         ReleaseTile(GeotiffTile* aTilePtr);
   void         PurgeInactive(double aMaxDataSize); // is this a double just to deconflict the signatures, here?
                                                    // this affects process input, so the code is stuck with it for now
   void PurgeInactive(unsigned int aTileCount);
   void AddTile(std::unique_ptr<GeotiffTile> aTilePtr);

   void Reset();

private:
   std::map<size_t, std::unique_ptr<GeotiffTile>> mCache;
   std::list<size_t>                              mInactive;
   double                                         mSizeInBytes{0};
};
} // namespace Geodata

#endif
