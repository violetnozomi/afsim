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

#include "WsfDtedRect.hpp"

#include "DtedTile.hpp"
#include "DtedTileManager.hpp"
#include "UtEntity.hpp"
#include "WsfZone.hpp"

//! Latitude and Longitude in degrees.
WsfDtedRect::WsfDtedRect(double aSW_Lat, double aSW_Lon, double aNE_Lat, double aNE_Lon)
   : mSWLat(aSW_Lat)
   , mSWLon(aSW_Lon)
   , mNELat(aNE_Lat)
   , mNELon(aNE_Lon)
{
   Normalize();
   SelectTiles();
}

WsfDtedRect::~WsfDtedRect() {}

unsigned int WsfDtedRect::GetTileCount() const
{
   return static_cast<unsigned int>(mSelectedTiles.size());
}

int WsfDtedRect::GetLat(unsigned int aTileIndex)
{
   if (aTileIndex < mSelectedTiles.size())
   {
      return mSelectedTiles[aTileIndex].mLat;
   }
   else
   {
      return 91;
   }
}

int WsfDtedRect::GetLon(unsigned int aTileIndex)
{
   if (aTileIndex < mSelectedTiles.size())
   {
      return mSelectedTiles[aTileIndex].mLon;
   }
   else
   {
      return 181;
   }
}

bool WsfDtedRect::GetTile(unsigned int aIndex, double& aLat, double& aLon)
{
   if (aIndex >= mSelectedTiles.size())
   {
      return false;
   }
   aLat = static_cast<double>(mSelectedTiles[aIndex].mLat);
   aLon = static_cast<double>(mSelectedTiles[aIndex].mLon);
   return true;
}

void WsfDtedRect::SelectTiles()
{
   // Get the latitudes/longitudes of the lower left corner of the required
   // cells.  Note the special processing for negative numbers...

   mMinLat = static_cast<int>(GetSWLat());
   if (mMinLat > GetSWLat())
   {
      --mMinLat;
   }
   mMinLon = static_cast<int>(GetSWLon());
   if (mMinLon > GetSWLon())
   {
      --mMinLon;
   }
   mMaxLat = static_cast<int>(GetNELat());
   if (mMaxLat > GetNELat())
   {
      --mMaxLat;
   }
   mMaxLon = static_cast<int>(GetNELon());
   if (mMaxLon > GetNELon())
   {
      --mMaxLon;
   }

   mSelectedTiles.clear();

   SelectedTile tile;
   for (int lat = mMinLat; lat <= mMaxLat; ++lat)
   {
      for (int lon = mMinLon; lon <= mMaxLon; ++lon)
      {
         tile.mLat = lat;
         tile.mLon = lon;
         mSelectedTiles.push_back(tile);
      }
   }
}

void WsfDtedRect::Normalize()
{
   // Clamp latitudes to [-90, 90].

   if (mSWLat < -90.0)
   {
      mSWLat = -90.0;
   }
   else if (mSWLat > 90.0)
   {
      mSWLat = 90.0;
   }

   if (mNELat < -90.0)
   {
      mNELat = -90.0;
   }
   else if (mNELat > 90.0)
   {
      mNELat = 90.0;
   }

   // Normalize longitudes to the range [-180, 180].

   while (mSWLon < -180.0)
   {
      mSWLon += 360.0;
   }
   while (mSWLon > 180.0)
   {
      mSWLon -= 360.0;
   }

   while (mNELon < -180.0)
   {
      mNELon += 360.0;
   }
   while (mNELon > 180.0)
   {
      mNELon -= 360.0;
   }
}
