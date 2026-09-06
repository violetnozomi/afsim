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

#include "GeoImageTile.hpp"

GeoImageTile::GeoImageTile()
   : GeoTile()
   , mImagePtr(nullptr)
   , mWidth(0)
   , mHeight(0)
   , mRow0IsBottomRow(false)
   , mUMax(1.0f)
   , mVMax(1.0f)
{
}

// virtual
GeoImageTile::~GeoImageTile()
{
   if (mImagePtr != nullptr)
   {
      delete[] mImagePtr;
   }
}

void GeoImageTile::SetImage(int aWidth, int aHeight, unsigned char* aImagePtr, float aUMax, float aVMax)
{
   if (mImagePtr != nullptr)
   {
      delete[] mImagePtr;
   }
   mWidth    = aWidth;
   mHeight   = aHeight;
   mImagePtr = aImagePtr;
   mUMax     = aUMax;
   mVMax     = aVMax;
}

// This is a convenience method for allocating tiles.  This
// is typically passed as an argument to the tile manager.
// It simply creates a new instance of a MapImageTile.

// static
GeoImageTile* GeoImageTile::TileAllocator()
{
   return new GeoImageTile();
}
