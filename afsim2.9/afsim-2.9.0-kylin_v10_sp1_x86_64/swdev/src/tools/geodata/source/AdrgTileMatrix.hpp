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

#ifndef ADRGTILEMATRIX_HPP
#define ADRGTILEMATRIX_HPP

#include "geodata_export.h"

// 'AdrgTileMatrix' is a specialization of the 'ArcTileMatrix'.
// It represents a single NIMA ADRG Zone Distribution Rectangle.

#include <string>
#include <vector>

#include "ArcTileMatrix.hpp"
class AdrgFile;
class GeoImageTile;

class GEODATA_EXPORT AdrgTileMatrix : public ArcTileMatrix
{
public:
   AdrgTileMatrix(AdrgFile& aGenFile, const char* aDrDirName, int& aStatus);

   ~AdrgTileMatrix() override;

   // Load the specified tile from the matrix.  The return value is zero
   // for success and non-zero for failure.

   int LoadTile(GeoImageTile* aTile, int aRow, int aCol) override;

private:
   std::string mFileName;
   FILE*       mFilePtr;

   double mLonDegPerPixel; // Asz / 360
   double mLatDegPerPixel; // Bs / 360
   int    mZone;           // 1-9 in north, 10-18 in south

   // The following four fields specify the padding.
   int mURPixelRow;
   int mURPixelCol;
   int mLLPixelRow;
   int mLLPixelCol;

   std::vector<unsigned int> mTileMap;
};

#endif
