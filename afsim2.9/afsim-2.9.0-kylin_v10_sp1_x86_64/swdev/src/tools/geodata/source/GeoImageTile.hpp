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

#ifndef GeoImageTile_hpp
#define GeoImageTile_hpp

#include "geodata_export.h"

// 'GeoImageTile' is a specialization of a 'GeoTile' that represents a bitmap
// image of a rectangular geographic region.

#include "GeoTile.hpp"

class GEODATA_EXPORT GeoImageTile : public GeoTile
{
public:
   GeoImageTile();

   ~GeoImageTile() override;

   // Dimension of the image in bits.
   int   GetHeight() const { return mHeight; }
   int   GetWidth() const { return mWidth; }
   float GetTexUMax() const { return mUMax; }
   float GetTexVMax() const { return mVMax; }

   // Retrieve a pointer to the image.
   const unsigned char* GetImage() const { return mImagePtr; }

   // Attach the image data to this object.  Any existing data attached
   // to the object is deleted.
   void SetImage(int aWidth, int aHeight, unsigned char* aImagePtr, float aUMax = 1.0f, float aVMax = 1.0f);

   // In a 'normal' image, row 0 is considered the TOP row and row N is
   // considered the BOTTOM row.  Some image loaders load them in the
   // reverse order, however.  This method must be called by such image
   // loaders to indicate that row 0 is the BOTTOM row.
   //
   // This should be called right after calling SetImage();

   void SetRow0IsBottomRow() { mRow0IsBottomRow = true; }
   bool Row0IsBottomRow() const { return mRow0IsBottomRow; }

   // This is a convenience method for allocating tiles.  This
   // is typically passed as an argument to the tile manager.
   // It simply creates a new instance of a MapImageTile.

   static GeoImageTile* TileAllocator();

private:
   unsigned char* mImagePtr;
   int            mWidth;
   int            mHeight;
   bool           mRow0IsBottomRow;
   float          mUMax;
   float          mVMax;
};

#endif
