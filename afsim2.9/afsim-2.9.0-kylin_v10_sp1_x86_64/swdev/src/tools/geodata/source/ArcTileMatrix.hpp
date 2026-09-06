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

#ifndef ArcTileMatrix_hpp
#define ArcTileMatrix_hpp

#include "geodata_export.h"

// ArcTileMatrix is a common base class that is used by tile matrices that
// are managed by ArcTileManager.

#include "GeoTileMatrix.hpp"

class GeoImageTile;

class GEODATA_EXPORT ArcTileMatrix : public GeoTileMatrix
{
public:
   ArcTileMatrix();

   ~ArcTileMatrix() override;

   // Get/Set the 'type' (or product) the matrix contains.
   int  GetDataSeries() const { return mDataSeries; }
   void SetDataSeries(int aDataSeries) { mDataSeries = aDataSeries; }

   // Load the specified tile from the matrix.  The return value is zero
   // for success and non-zero for failure.

   virtual int  LoadTile(GeoImageTile* aImagePtr, int aRow, int aCol) = 0;
   virtual bool LegitimateAddress(int aRow, int aCol) { return true; }
   virtual void GetRectAndResolution(int aRow, int aCol, GeoRect& aRect, float& aResolution) {}

private:
   int mDataSeries;
};

#endif
