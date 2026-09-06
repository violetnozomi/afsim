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

#include "ArcLodDir.hpp"

#include "ArcTileMatrix.hpp"

ArcLodDir::ArcLodDir() {}

// virtual
ArcLodDir::~ArcLodDir() {}

bool ArcLodDir::LegitimateAddr(int aLod, int aRow, int aCol)
{
   if ((aLod >= 0) && (aLod < (int)GetTileMatrixCount()))
   {
      return GetTileMatrix(aLod)->LegitimateAddress(aRow, aCol);
   }
   return false;
}

void ArcLodDir::GetRectAndResolution(int aLod, int aRow, int aCol, GeoRect& aRect, float& aResolution)
{
   if (LegitimateAddr(aLod, aRow, aCol))
   {
      GetTileMatrix(aLod)->GetRectAndResolution(aRow, aCol, aRect, aResolution);
   }
}
