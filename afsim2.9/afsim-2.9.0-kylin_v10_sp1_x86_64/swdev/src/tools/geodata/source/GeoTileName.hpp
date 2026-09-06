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

#ifndef GeoTileName_hpp
#define GeoTileName_hpp

#include "geodata_export.h"

#include <cstddef>

// A 'GeoTileName' provides a unique 'name' for a tile that can be used to
// identify the tile in maps and hash tables.
//
// Everything is inline for performance...

class GeoTileMatrix;

class GEODATA_EXPORT GeoTileName
{
public:
   GeoTileName(GeoTileMatrix* aTileMatrixPtr, size_t aTileIndex)
      : mTileMatrixPtr(aTileMatrixPtr)
      , mTileIndex(aTileIndex)
   {
   }

   ~GeoTileName() {}

   bool operator==(const GeoTileName& aRhs) const
   {
      // Compare on tile index first as it will quickly decide if a match is
      // possible.  Many tiles in a map/hash table may be from the same tile
      // matrix so if the compare were reversed then the second comparison
      // would often be needed.
      return ((mTileIndex == aRhs.mTileIndex) && (mTileMatrixPtr == aRhs.mTileMatrixPtr));
   }

   bool operator<(const GeoTileName& aRhs) const
   {
      bool lessThan = false;
      if (mTileMatrixPtr < aRhs.mTileMatrixPtr)
      {
         lessThan = true;
      }
      else if (mTileMatrixPtr == aRhs.mTileMatrixPtr)
      {
         if (mTileIndex < aRhs.mTileIndex)
         {
            lessThan = true;
         }
      }
      return lessThan;
   }

   GeoTileMatrix* GetTileMatrix() const { return mTileMatrixPtr; }
   size_t         GetTileIndex() const { return mTileIndex; }

private:
   GeoTileMatrix* mTileMatrixPtr;
   size_t         mTileIndex;
};

#endif
