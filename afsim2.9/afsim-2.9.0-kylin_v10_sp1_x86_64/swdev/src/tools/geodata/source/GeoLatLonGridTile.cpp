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

#include "GeoLatLonGridTile.hpp"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>

#include "UtException.hpp"

GeoLatLonGridTile::GeoLatLonGridTile()
   : GeoTile()
   , mLatInterval(1.0)
   , mLonInterval(1.0)
   , mHalfLatInterval(0.5)
   , mHalfLonInterval(0.5)
   , mRecipLatInterval(1.0)
   , mRecipLonInterval(1.0)
   , mLatPoints(0)
   , mLonPoints(0)
   , mSizeInBytes(0.0)
   , mDummyTile(false)
   , mDataType(cSHORT_INT)
   , mDataOrderType(cCOLUMN_MAJOR)
   , mData()
{
   mData.mAsVoid = nullptr;
}

GeoLatLonGridTile::~GeoLatLonGridTile()
{
   if (mData.mAsVoid != nullptr)
   {
      DeleteData();
   }
}

//! Get the interpolated elevation (in meters) at the specified
//! WGS-84 latitude and longitude.  The returned value is gotten
//! by performing a bilinear interpolation using the elevations
//! from the four surrounding samples.
//!
//! Return value is zero for success and non-zero if the position
//! is outside of the cell.
int GeoLatLonGridTile::GetInterpValue(double aLat, double aLon, float& aValue) const
{
   // Determine the indicies of the southwest corner.
   aValue = 0.0F;

   // If the lat/lon provided to the tile manager returned a tile and the
   // same lat/lon is provided to this route, we must not reject the tile.
   // Therefore, we must reject based on the GetXXLat()/GetXXLon() methods
   // and not using indices. The computation of indices may have truncation
   // from the conversion to integer.

   // NOTE: This routine is called A LOT during terrain masking checks.
   // Comparing the integer indices first and then performing the floating
   // comparison only when needed is faster than just performing the floating
   // comparison.  One benchmark gave about a 5% total improvement.
   double realLatIndex = (aLat - GetSWLat()) * mRecipLatInterval;
   int    latIndex     = static_cast<int>(realLatIndex);
   double realLonIndex = (aLon - GetSWLon()) * mRecipLonInterval;
   int    lonIndex     = static_cast<int>(realLonIndex);

   if (latIndex <= 0)
   {
      if (aLat < GetSWLat()) // Compare like the tile manager
      {
         return 1;
      }
      latIndex = 0;
   }
   else if (latIndex >= (static_cast<int>(mLatPoints) - 1))
   {
      if (aLat > GetNELat()) // Compare like the tile manager
      {
         return 1;
      }
      latIndex = static_cast<int>(mLatPoints) - 2;
   }

   if (lonIndex <= 0)
   {
      if (aLon < GetSWLon()) // Compare like the tile manager
      {
         return 1;
      }
      lonIndex = 0;
   }
   else if (lonIndex >= (static_cast<int>(mLonPoints) - 1))
   {
      if (aLon > GetNELon()) // Compare like the tile manager
      {
         return 1;
      }
      lonIndex = static_cast<int>(mLonPoints) - 2;
   }

   // Get function values at corners (fUV = f at u = U, v = V)
   // Let 'u' be the parametric direction for longitude in [0, 1]
   // Let 'v' be the parametric direction for latitude  in [0, 1]

   double f00 = 0.0;
   double f10 = 0.0;
   double f01 = 0.0;
   double f11 = 0.0;

   int i00;
   int i10;
   int i01;
   int i11;
   if (mDataOrderType == cCOLUMN_MAJOR)
   {
      i00 = lonIndex * mLatPoints + latIndex;
      i10 = i00 + mLatPoints;
      i01 = i00 + 1;
      i11 = i00 + mLatPoints + 1;
   }
   else
   {
      i00 = latIndex * mLonPoints + lonIndex;
      i10 = i00 + 1;
      i01 = i00 + mLonPoints;
      i11 = i00 + mLonPoints + 1;
   }
   if (mDataType == cSHORT_INT)
   {
      f00 = mData.mAsShortInt[i00]; // lon, lat
      f10 = mData.mAsShortInt[i10]; // lon + 1, lat
      f01 = mData.mAsShortInt[i01]; // lon, lat + 1
      f11 = mData.mAsShortInt[i11]; // lon + 1, lat + 1
   }
   else if (mDataType == cFLOAT)
   {
      f00 = mData.mAsFloat[i00]; // lon, lat
      f10 = mData.mAsFloat[i10]; // lon + 1, lat
      f01 = mData.mAsFloat[i01]; // lon, lat + 1
      f11 = mData.mAsFloat[i11]; // lon + 1, lat + 1
   }
   else if (mDataType == cDOUBLE)
   {
      f00 = mData.mAsDouble[i00]; // lon, lat
      f10 = mData.mAsDouble[i10]; // lon + 1, lat
      f01 = mData.mAsDouble[i01]; // lon, lat + 1
      f11 = mData.mAsDouble[i11]; // lon + 1, lat + 1
   }
   else
   {
      // It may not be appropriate to interpolate for other data types.
      throw UtException("GeoLatLonGridTile::GetInterpValue: Cannot interpolate between points of given type.");
   }

   // Determine the coefficients for the bilinear interpolation equation:
   //   f = a + (b * u) + (c * v) + (d * u * v)

   // double a = f00;
   // double b = f10 - f00;
   // double c = f01 - f00;
   // double d = f11 - f01 - f10 + f00;

   // Interpolate

   double v = realLatIndex - latIndex;
   double u = realLonIndex - lonIndex;
   // aValue  = a + (b * u) + (c * v) + (d * u * v);
   double elev = f00 + ((f10 - f00) * u) + ((f01 - f00) + (f11 - f10 - (f01 - f00)) * u) * v;
   aValue      = static_cast<float>(elev);
   return 0;
}

//! Construct a dummy Dem cell for the specified location. The location
//! is specified in the WGS-84 reference frame using decimal degrees
//! with negative values for west and south.
void GeoLatLonGridTile::MakeDummyTile(double aLat, double aLon, double aLatExtent, double aLonExtent)
{
   // Determine the integral location of the southwest corner that
   // contains the specified location.

   mDummyTile = true;

   SetSWLat(aLat);
   SetSWLon(aLon);
   SetNELat(aLat + aLatExtent);
   SetNELon(aLon + aLonExtent);
   mLatInterval      = aLatExtent / 2.0; // two points lat.
   mHalfLatInterval  = 0.5 * mLatInterval;
   mRecipLatInterval = 1.0 / mLatInterval;
   mLonInterval      = aLonExtent / 2.0; // two points lon.
   mHalfLonInterval  = 0.5 * mLonInterval;
   mRecipLonInterval = 1.0 / mLonInterval;
   mLatPoints        = 2;
   mLonPoints        = 2;

   // Create dummy elevation data.

   CreateData(4);
   for (unsigned int i = 0; i < 4; ++i)
   {
      if (mDataType == cSHORT_INT)
      {
         mData.mAsShortInt[i] = 0;
      }
      else if (mDataType == cFLOAT)
      {
         mData.mAsFloat[i] = 0.0;
      }
      else if (mDataType == cDOUBLE)
      {
         mData.mAsDouble[i] = 0.0;
      }
   }
}
