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

#ifndef LANDUSETILE_HPP
#define LANDUSETILE_HPP

#include "geodata_export.h"

#include <cstdio>
#include <iosfwd>
#include <string>

class UtBitmapImage;
#include "FloatGridTile.hpp" // for common header info
#include "GeoLatLonGridTile.hpp"

class GEODATA_EXPORT LandUseTile : public GeoLatLonGridTile
{
public:
   enum Format
   {
      cARC_ASCII,
      cBITMAP
   };

   LandUseTile(Format aFormat = cBITMAP);

   ~LandUseTile() override;

   inline float GetLandUse(unsigned int aLatIndex, unsigned int aLonIndex) const
   {
      return GetValue(aLatIndex, aLonIndex);
   }

   int GetLandUse(double aLat, double aLon, int& aLandUse) const;

   //! Load the cell from the specified file.
   //!
   //! @return   0 if success.
   //!           1 if the file cannot be opened.
   //!           2 if the contents appear bad.
   int LoadTile(const char* aFileName) override;

   void MakeDummyTile(double aSWLat, double aSWLon, double aLatInterval = 1.0, double aLonInterval = 1.0) override;

protected:
   int  LoadERS_Header(const std::string& aFileName);
   void InitializeFromHeader();

   Format                mFormat;
   FloatGridTile::Header mHeader;
   UtBitmapImage*        mBitmapPtr; // nonzero if we use the bmp format.
};

//! Get the approximate LandUse at the specified
//! WGS-84 latitude and longitude.  This is approximate in that
//! it returns the LandUse of the closest sample.
//!
//! Return value is zero for success and non-zero if the position
//! is outside of the cell.
inline int LandUseTile::GetLandUse(double aLat, double aLon, int& aLandUse) const
{
   aLandUse      = 0;
   int    status = 1;
   double latOffset; // defined below
   if (mFormat == cARC_ASCII)
   {
      latOffset = aLat - GetSWLat();
   }
   else // bmp
   {
      latOffset = GetNELat() - aLat; // bmp rows are stored in reverse order
   }
   double lonOffset = aLon - GetSWLon();
   int    latIndex  = static_cast<int>(latOffset * mRecipLatInterval);
   int    lonIndex  = static_cast<int>(lonOffset * mRecipLonInterval);
   if ((latIndex >= 0) && (latIndex < static_cast<int>(mLatPoints)) && (lonIndex >= 0) &&
       (lonIndex < static_cast<int>(mLonPoints)))
   {
      GetValue(latIndex, lonIndex, aLandUse);
      status = 0;
   }
   return status;
}
#endif
