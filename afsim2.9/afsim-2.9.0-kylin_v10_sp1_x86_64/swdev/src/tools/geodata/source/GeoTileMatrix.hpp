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

#ifndef GeoTileMatrix_hpp
#define GeoTileMatrix_hpp

#include "geodata_export.h"

// 'GeoTileMatrix' defines a matrix of 'GeoTiles'.

#include "GeoRect.hpp"

class GEODATA_EXPORT GeoTileMatrix : public GeoRect
{
public:
   GeoTileMatrix();

   ~GeoTileMatrix() override;

   // Accessors

   // Return the number of rows and columns in the tile matrix.
   unsigned int GetNumRows() const { return mNumRows; }
   unsigned int GetNumCols() const { return mNumCols; }

   // Return the dimension of a tile (in decimal degrees).  This can be
   // used to compute the row/column of a tile given a latitude/longitude.
   double GetLatTileDimDeg() const { return mLatTileDimDeg; }
   double GetLonTileDimDeg() const { return mLonTileDimDeg; }

   // Mutators

   void SetNumRows(unsigned int aNumRows) { mNumRows = aNumRows; }
   void SetNumCols(unsigned int aNumCols) { mNumCols = aNumCols; }

   void SetLatTileDimDeg(double aLatTileDimDeg) { mLatTileDimDeg = aLatTileDimDeg; }
   void SetLonTileDimDeg(double aLonTileDimDeg) { mLonTileDimDeg = aLonTileDimDeg; }

   // Other methods

   // Get the column number of the specified longitude.  Column 0 is the
   // left-most column.  If outside the bounds then it will be set to
   // the appropriate bound (0 or GetNumCols() - 1)

   int GetCol(double aLon) const;

   // Get the row number of the specified latitude.  Row 0 is the
   // top-most row.  If outside the bounds then it will be set to the
   // appropriate bound (0 or GetNumRows() - 1);

   int GetRow(double aLat) const;

private:
   // Number of rows/columns in the tile matrix
   unsigned int mNumRows;
   unsigned int mNumCols;

   // The dimension of a tile in decimal degrees
   double mLatTileDimDeg;
   double mLonTileDimDeg;
};

#endif
