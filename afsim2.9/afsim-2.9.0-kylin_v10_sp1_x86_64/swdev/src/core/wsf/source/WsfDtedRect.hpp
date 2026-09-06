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

#ifndef WSFDTEDRECT_HPP
#define WSFDTEDRECT_HPP

#include "wsf_export.h"

// 'WsfDtedRect' is a utility class copied from project "geodatag" (renamed from DtedRect),
// and used to simplify the height processing of a collected North-East-aligned rectangle of DTED tiles.

#include <vector>

class DtedTile;

class WSF_EXPORT WsfDtedRect
{
public:
   WsfDtedRect(double aSW_Lat, double aSW_Lon, double aNE_Lat, double aNE_Lon);

   ~WsfDtedRect();

   unsigned int GetTileCount() const;

   bool GetTile(unsigned int aIndex, double& aLat, double& aLon);

   // Accessors
   double GetSWLat() const { return mSWLat; }
   double GetSWLon() const { return mSWLon; }
   double GetNELat() const { return mNELat; }
   double GetNELon() const { return mNELon; }

   // Mutators
   // void SetSWLat(double aSWLat) { mSWLat = aSWLat; }
   // void SetSWLon(double aSWLon) { mSWLon = aSWLon; }
   // void SetNELat(double aNELat) { mNELat = aNELat; }
   // void SetNELon(double aNELon) { mNELon = aNELon; }

   // 'Normalize' out of range values so they are in the proper range.
   // Latitudes are clamped so they are between +/-90.  Longitudes will
   // have +/- 360 added until they are in the range +/-180.

   void Normalize();

protected:
   int GetLat(unsigned int aTileIndex);
   int GetLon(unsigned int aTileIndex);

   void SelectTiles();

private:
   WsfDtedRect(const WsfDtedRect& aSrc);            // Not implemented
   WsfDtedRect& operator=(const WsfDtedRect& aRhs); // Not implemented

   struct SelectedTile
   {
      int mLat;
      int mLon;
   };

   double mSWLat;
   double mSWLon;
   double mNELat;
   double mNELon;

   int mMinLat;
   int mMaxLat;
   int mMinLon;
   int mMaxLon;

   std::vector<SelectedTile> mSelectedTiles;
};

#endif
