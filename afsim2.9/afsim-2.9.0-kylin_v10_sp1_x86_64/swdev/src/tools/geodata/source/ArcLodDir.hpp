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

#ifndef ARCLODDIR_HPP
#define ARCLODDIR_HPP

#include "geodata_export.h"

// This is an abstract base class that represents a directory containing
// NIMA Equal Arc-Second (ARC) data.  This includes ADRG, CADRG and CIB
// products.

#include <vector>

class ArcTileMatrix;
class GeoRect;

class GEODATA_EXPORT ArcLodDir
{
public:
   ArcLodDir();

   virtual ~ArcLodDir();

   // Return the number of tile matrices.

   unsigned int GetTileMatrixCount() const;

   // Return a pointer to the n'th tile matrix where
   //  0 <= 'aIndex' < GetTileMatrixCount();

   ArcTileMatrix* GetTileMatrix(unsigned int aIndex) const;

   bool LegitimateAddr(int aLod, int aRow, int aCol);

   void GetRectAndResolution(int aLod, int aRow, int aCol, GeoRect& aRect, float& aResolution);

protected:
   // A collection of tile matrices that are contained in this directory
   std::vector<ArcTileMatrix*> mTileMatrix;

private:
};

inline unsigned int ArcLodDir::GetTileMatrixCount() const
{
   return static_cast<unsigned int>(mTileMatrix.size());
}
inline ArcTileMatrix* ArcLodDir::GetTileMatrix(unsigned int aIndex) const
{
   return mTileMatrix[aIndex];
}

#endif
