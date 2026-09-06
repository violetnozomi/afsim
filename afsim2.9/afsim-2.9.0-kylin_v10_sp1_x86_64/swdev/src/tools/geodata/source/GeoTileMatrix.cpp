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

#include "GeoTileMatrix.hpp"

GeoTileMatrix::GeoTileMatrix()
   : mNumRows(0)
   , mNumCols(0)
   , mLatTileDimDeg(0.0)
   , mLonTileDimDeg(0.0)
{
}

GeoTileMatrix::~GeoTileMatrix() {}

// Get the column number of the specified longitude.  Column 0 is the
// left-most column.  If outside the bounds then it will be set to
// the appropriate bound (0 or GetNumCols() - 1)
int GeoTileMatrix::GetCol(double aLon) const
{
   double lonOffset = aLon - GetSWLon();
   // if (GetSWLon() < GetNELon())
   //{
   //    // Matrix does not span the dateline...
   // }
   // else
   //{
   //    // Matrix spans the dateline
   // }
   int col = static_cast<int>(lonOffset / mLonTileDimDeg);
   if (col < 0)
   {
      col = 0;
   }
   else if (col >= static_cast<int>(mNumCols))
   {
      col = mNumCols;
   }
   return col;
}

// Get the row number of the specified latitude.  Row 0 is the
// top-most row.  If outside the bounds then it will be set to the
// appropriate bound (0 or GetNumRows() - 1);

int GeoTileMatrix::GetRow(double aLat) const
{
   double latOffset = GetNELat() - aLat;
   int    row       = static_cast<int>(latOffset / mLatTileDimDeg);
   if (row < 0)
   {
      row = 0;
   }
   else if (row >= static_cast<int>(mNumRows))
   {
      row = mNumRows - 1;
   }
   return row;
}
