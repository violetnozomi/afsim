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

#ifndef GEOLATLONGRIDTILE_HPP
#define GEOLATLONGRIDTILE_HPP

#include "geodata_export.h"

#include "GeoTile.hpp"

//! A class that supports a rectangular grid of arbitrary points.
//! These could be elevation points, land use data, etc.
//! @note Note that this is not an abstract class.  Tiles are
//! explicitly implemented using a single-dimensional array.
//! This is a choice of speed over flexibility.  The elevation data
//! from these tiles will be accessed very, very often.  It is important
//! to maintain this speed bias.  For example, do not pull the GetElev...
//! methods into parent class virtual methods
//! (~15% performance hit with Win32/VS8).
class GEODATA_EXPORT GeoLatLonGridTile : public GeoTile
{
public:
   GeoLatLonGridTile();

   ~GeoLatLonGridTile() override;

   const void* GetData() const;

   float GetValue(unsigned int aLatIndex, unsigned int aLonIndex) const;

   void GetValue(unsigned int aLatIndex, unsigned int aLonIndex, int& aVaue) const;

   int GetApproxValue(double aLat, double aLon, float& aValue) const;

   int GetInterpValue(double aLat, double aLon, float& aValue) const;

   unsigned int GetLatIndex(double aLat) const;
   unsigned int GetLonIndex(double aLon) const;

   // Get the latitude (longitude) index that is at or south west of the
   // specified latitude (longitude).
   //
   // NOTE: The input values are not checked for validity.  They *MUST* be
   //       inside the cell!

   unsigned int GetSWLatIndex(double aLat) const;
   unsigned int GetSWLonIndex(double aLon) const;

   // Get the distance (in degrees) between the sample points.

   double GetLatInterval() const;
   double GetLonInterval() const;

   // Get the latitude (longitude) that corresponds to an index.
   //
   // NOTE: The input values are not checked for validity.  They *MUST* be
   //       inside the cell!

   double GetLatOfIndex(unsigned int aLatIndex) const;
   double GetLonOfIndex(unsigned int aLonIndex) const;

   // Get the number of sample points in each direction.

   unsigned int GetLatPoints() const;
   unsigned int GetLonPoints() const;

   //! Load the cell from the specified file.
   //!
   //! @return   0 if success.
   //!           1 if the file cannot be opened.
   //!           2 if the contents appear bad.
   virtual int LoadTile(const char* aFileName) = 0;

   virtual void MakeDummyTile(double aSWLat, double aSWLon, double aLatExtent, double aLonExtent);

   //! Let the application find out if the result is valid.
   bool IsDummyTile() { return mDummyTile; }

   enum DataType
   {
      cUNSIGNED_CHAR,
      cSHORT_INT,
      cFLOAT,
      cDOUBLE
   };

   enum DataOrderType
   {
      cROW_MAJOR    = 0,
      cCOLUMN_MAJOR = 1
   };

   //! Return the data type being used to store the data.
   //! This can be used in conjunction with GetLatLonGridTile::GetData to
   //! access the data directly.
   int GetDataType() const { return mDataType; }

   //! Return the size, in bytes, that the tile data occupy in memory.
   //! @note The size does not reflect the memory used by any other object data.
   double GetDataSize() const override { return mSizeInBytes; }

protected:
   void SetDataType(DataType aDataType) { mDataType = aDataType; }

   void SetDataOrderType(DataOrderType aDataOrderType) { mDataOrderType = aDataOrderType; }

   void CreateData(unsigned aSize)
   {
      if (mData.mAsVoid != nullptr)
      {
         DeleteData();
      }
      if (mDataType == cSHORT_INT)
      {
         mData.mAsShortInt = new short int[aSize];
         mSizeInBytes      = static_cast<double>(aSize * 2);
      }
      else if (mDataType == cUNSIGNED_CHAR)
      {
         mData.mAsUnsignedChar = new unsigned char[aSize];
         mSizeInBytes          = static_cast<double>(aSize);
      }
      else if (mDataType == cFLOAT)
      {
         mData.mAsFloat = new float[aSize];
         mSizeInBytes   = static_cast<double>(aSize * 4);
      }
      else if (mDataType == cDOUBLE)
      {
         mData.mAsDouble = new double[aSize];
         mSizeInBytes    = static_cast<double>(aSize * 8);
      }
   }

   void DeleteData()
   {
      if (mDataType == cSHORT_INT)
      {
         delete[] mData.mAsShortInt;
         mData.mAsShortInt = nullptr;
      }
      else if (mDataType == cUNSIGNED_CHAR)
      {
         delete[] mData.mAsUnsignedChar;
         mData.mAsUnsignedChar = nullptr;
      }
      else if (mDataType == cFLOAT)
      {
         delete[] mData.mAsFloat;
         mData.mAsFloat = nullptr;
      }
      else if (mDataType == cDOUBLE)
      {
         delete[] mData.mAsDouble;
         mData.mAsDouble = nullptr;
      }
      mSizeInBytes = 0;
   }

   double mLatInterval;
   double mLonInterval;
   double mHalfLatInterval;  // 0.5 * mLatInterval
   double mHalfLonInterval;  // 0.5 * mLonInterval
   double mRecipLatInterval; // 1.0 / mLatInterval
   double mRecipLonInterval; // 1.0 / mLonInterval

   unsigned int mLatPoints;
   unsigned int mLonPoints;
   double       mSizeInBytes;
   bool         mDummyTile;

   DataType      mDataType;
   DataOrderType mDataOrderType;

   union
   {
      unsigned char* mAsUnsignedChar;
      short int*     mAsShortInt;
      float*         mAsFloat;
      double*        mAsDouble;
      void*          mAsVoid;
   } mData;
};

// Inline method definitions

//! Get the latitude index that is closest to the specified
//! latitude.
//!
//! @note The input values are not checked for validity.  They *MUST* be
//  inside the cell!
inline unsigned int GeoLatLonGridTile::GetLatIndex(double aLat) const
{
   // rounding is done by adding 1/2 then truncating to an integer
   double latOffset = aLat - GetSWLat() + mHalfLatInterval;
   return static_cast<unsigned int>(latOffset * mRecipLatInterval);
}

//! Get the longitude index that is closest to the specifed
//! longitude.
//!
//! @note The input values are not checked for validity.  They *MUST* be
//  inside the cell!
inline unsigned int GeoLatLonGridTile::GetLonIndex(double aLon) const
{
   // rounding is done by adding 1/2 then truncating to an integer
   double lonOffset = aLon - GetSWLon() + mHalfLatInterval;
   return static_cast<unsigned int>(lonOffset * mRecipLonInterval);
}
inline unsigned int GeoLatLonGridTile::GetSWLatIndex(double aLat) const
{
   // just truncate, no rounding
   double latOffset = aLat - GetSWLat();
   return static_cast<unsigned int>(latOffset * mRecipLatInterval);
}
inline unsigned int GeoLatLonGridTile::GetSWLonIndex(double aLon) const
{
   // just truncate, no rounding
   double lonOffset = aLon - GetSWLon();
   return static_cast<unsigned int>(lonOffset * mRecipLonInterval);
}
inline double GeoLatLonGridTile::GetLatInterval() const
{
   return mLatInterval;
}
inline double GeoLatLonGridTile::GetLonInterval() const
{
   return mLonInterval;
}
inline double GeoLatLonGridTile::GetLatOfIndex(unsigned int aLatIndex) const
{
   return GetSWLat() + (aLatIndex * mLatInterval);
}
inline double GeoLatLonGridTile::GetLonOfIndex(unsigned int aLonIndex) const
{
   return GetSWLon() + (aLonIndex * mLonInterval);
}
inline unsigned int GeoLatLonGridTile::GetLatPoints() const
{
   return mLatPoints;
}
inline unsigned int GeoLatLonGridTile::GetLonPoints() const
{
   return mLonPoints;
}

//! Get the value using a row/column index.
//! The indicies are NOT checked for validity.
//! @note This is primarily used for terrain elevation checks,
//! but all four native data types are optionally supported.
inline float GeoLatLonGridTile::GetValue(unsigned int aLatIndex, unsigned int aLonIndex) const
{
   unsigned int index; // assigned below.
   if (mDataOrderType == cCOLUMN_MAJOR)
   {
      index = (aLonIndex * mLatPoints) + aLatIndex;
   }
   else // row major
   {
      index = (aLatIndex * mLonPoints) + aLonIndex;
   }

   if (mDataType == cSHORT_INT)
   {
      return mData.mAsShortInt[index];
   }
   if (mDataType == cFLOAT)
   {
      return mData.mAsFloat[index];
   }
   else if (mDataType == cUNSIGNED_CHAR) // cUNSIGNED_CHAR
   {
      return mData.mAsUnsignedChar[index];
   }
   else // cDOUBLE
   {
      return static_cast<float>(mData.mAsDouble[index]);
   }
}

//! Return an int value at the specified indices.
//!@note This version only supports int and unsigned char data types.
inline void GeoLatLonGridTile::GetValue(unsigned int aLatIndex, unsigned int aLonIndex, int& aValue) const
{
   unsigned int index; // assigned below.
   if (mDataOrderType == cCOLUMN_MAJOR)
   {
      index = (aLonIndex * mLatPoints) + aLatIndex;
   }
   else // row major
   {
      index = (aLatIndex * mLonPoints) + aLonIndex;
   }

   if (mDataType == cSHORT_INT)
   {
      aValue = mData.mAsShortInt[index];
   }
   else // cUNSIGNED_CHAR
   {
      aValue = mData.mAsUnsignedChar[index];
   }
}

//! Return a pointer to raw data...
inline const void* GeoLatLonGridTile::GetData() const
{
   return mData.mAsVoid;
}

//#include <iostream>
//! Get the approximate value at the specified
//! WGS-84 latitude and longitude.  This is approximate in that
//! it returns the altitude of the closest sample.
//!
//! Return value is zero for success and non-zero if the position
//! is outside of the cell.
inline int GeoLatLonGridTile::GetApproxValue(double aLat, double aLon, float& aValue) const
{
   aValue     = 0.0F;
   int status = 1;

   // Perform an initial check to see whether we are at a negative location with
   // respect to this tile.  If so, stop and return that we are outside the tile.
   double latOffset = aLat - GetSWLat();
   double lonOffset = aLon - GetSWLon();

   if ((latOffset >= 0.0) && (lonOffset >= 0.0))
   {
      // Next, find the correct latitude and longitude index.
      // Add the half interval to get better roundoff approximation.
      int latIndex = static_cast<int>((latOffset + mHalfLatInterval) * mRecipLatInterval);
      int lonIndex = static_cast<int>((lonOffset + mHalfLonInterval) * mRecipLonInterval);

      int idx;
      if (mDataOrderType == cCOLUMN_MAJOR)
      {
         idx = lonIndex * mLatPoints + latIndex;
      }
      else
      {
         idx = latIndex * mLonPoints + lonIndex;
      }

      // Only continue of the indices are within bounds.
      if ((latIndex < static_cast<int>(mLatPoints)) && (lonIndex < static_cast<int>(mLonPoints)))
      {
         if (mDataType == cSHORT_INT)
         {
            aValue = mData.mAsShortInt[idx];
         }
         else if (mDataType == cFLOAT)
         {
            aValue = mData.mAsFloat[idx];
         }
         else // double
         {
            aValue = static_cast<float>(mData.mAsDouble[idx]);
         }
         status = 0;
      }
   }

   return status;
}

#endif
