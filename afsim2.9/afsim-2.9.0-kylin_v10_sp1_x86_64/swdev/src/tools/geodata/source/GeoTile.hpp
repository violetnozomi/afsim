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

#ifndef GeoTile_hpp
#define GeoTile_hpp

#include "geodata_export.h"

// A 'GeoTile' is an abstract base class that represents a rectangular tile of
// information for a geographic area.  This information can be something like
// a pixmap image, elevation data, etc.

#include "GeoRect.hpp"

class GeoTileMatrix;

class GEODATA_EXPORT GeoTile : public GeoRect
{
public:
   GeoTile();
   ~GeoTile() override;

   // Use count maintenance functions.  The return value is the updated
   // value of the lock count.

   unsigned int DecrementUseCount();
   unsigned int IncrementUseCount();

   // Accessors

   unsigned int   GetTileIndex() const;
   GeoTileMatrix* GetTileMatrix() const;

   // Mutators

   void SetTileIndex(unsigned int aTileIndex);
   void SetTileMatrix(GeoTileMatrix* aTileMatrixPtr);

   // Get the size of the data contained in the tile.
   // Typically this will be the size in bytes (as with elevation tiles).
   virtual double GetDataSize() const
   {
      return 0.0; // Base class has no data
   }

private:
   // A pointer to the tile matrix of which this tile is a member.
   GeoTileMatrix* mTileMatrixPtr;

   // The row-major index (i.e. (row * num_rows) + column) of the tile.
   unsigned int mTileIndex;

   // Number of outstanding users
   unsigned int mUseCount;

   // Don't allow copy or assignment for now...
   GeoTile(const GeoTile& aSrc);
   GeoTile& operator=(const GeoTile& aRhs);
};

// Lock count maintenance

inline unsigned int GeoTile::DecrementUseCount()
{
   if (mUseCount != 0)
   {
      --mUseCount;
   }
   return mUseCount;
}
inline unsigned int GeoTile::IncrementUseCount()
{
   return ++mUseCount;
}

// Accessors

inline unsigned int GeoTile::GetTileIndex() const
{
   return mTileIndex;
}
inline GeoTileMatrix* GeoTile::GetTileMatrix() const
{
   return mTileMatrixPtr;
}

// Mutators

inline void GeoTile::SetTileIndex(unsigned int aTileIndex)
{
   mTileIndex = aTileIndex;
}
inline void GeoTile::SetTileMatrix(GeoTileMatrix* aTileMatrixPtr)
{
   mTileMatrixPtr = aTileMatrixPtr;
}

#endif
