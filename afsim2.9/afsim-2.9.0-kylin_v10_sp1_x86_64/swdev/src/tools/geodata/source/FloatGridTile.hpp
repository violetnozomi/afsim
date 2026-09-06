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

#ifndef FLOATGRIDTILE_HPP
#define FLOATGRIDTILE_HPP

#include "geodata_export.h"

#include <iosfwd>
#include <string>

#include "GeoElevationTile.hpp"

//! A 'FloatGridTile' represents processed data from a single DEM cell.
class GEODATA_EXPORT FloatGridTile : public GeoElevationTile
{
public:
   FloatGridTile();
   FloatGridTile(const FloatGridTile& aSrc) = delete;
   FloatGridTile& operator=(const FloatGridTile& aRhs) = delete;
   ~FloatGridTile() override                           = default;

   int LoadTile(const char* aFileName) override;

   int SaveTile(const char* aTileName);

   int LoadHeader(const std::string& aHeaderFileName);

   int LoadData();

   int UnloadData();

   // This is a convenience method for allocating tiles.  This
   // is typically passed as an argument to the tile manager.
   // It simply creates a new instance of a FloatGridTile.

   static FloatGridTile* TileAllocator();

   //! The Header structure for both float-grid and landuse.
   //! This is the standard format for ARC GIS files,
   //! so float-grid can be considered an open version of ARC GIS binary,
   //! and the non-bitmap form of landuse is the ARC ASCII format.
   //! This could eventually be moved into a utility class.
   struct Header
   {
      unsigned    mNCols;       //< Number of columns in the image.
      unsigned    mNRows;       //< Number of rows in the image.
      double      mXLLCorner;   //< X coordinate at the lower left corner (longitude).
      double      mYLLCorner;   //< Y coordinate at the lower left corner (latitude).
      double      mCellSize;    //< Cellsize of each cell (assumed DeltaX=DeltaY).
      std::string mByteOrder;   //< either "LSBFIRST" (Intel) or "MSBFIRST" (Motorola)
      int         mNoDataValue; //< Numeric value or cells indicating NO DATA.

      // Constructor defined to set default values for byteorder and nodatavalue.
      // Assume all other values set elsewhere.
      Header()
         : mByteOrder("LSBFIRST")
         , mNoDataValue(-9999)
      {
      }

      // The following can be used to set the GeoRect parent of FloatGridTile:
      double GetSWLat() const
      {
         double swLat = mYLLCorner;
         return swLat;
      }

      double GetSWLon() const
      {
         double swLon = mXLLCorner;
         return swLon;
      }

      double GetNELat() const
      {
         double neLat = mYLLCorner + mCellSize * (double)mNRows;
         return neLat;
      }

      double GetNELon() const
      {
         double neLon = mXLLCorner + mCellSize * (double)mNCols;
         return neLon;
      }

      void PrintData() const;
   };

   const Header& GetHeader() const { return mHeader; }

   const std::string& GetDataFileName() const { return mDataFileName; }

   bool IsLoaded() const { return mIsLoaded; }

   bool HeaderValid() const { return mHeaderLoaded; }

   static int LoadHeader(const std::string& aHeaderFileName, Header& aHeader);

   static int LoadHeader(std::ifstream& aStream,
                         Header&        aHeader,
                         unsigned       aLines = 1000); // Limit the # lines to read if necessary.

   bool FillSubtile(const GeoRect& aBoundingBox, FloatGridTile*& aSubTilePtr, unsigned aDownSampleFactor = 1);

   void MakeDummyTile(double aSWLat, double aSWLon, double aLatInterval = 0.1, double aLonInterval = 0.1) override;

   /* tbd
         struct Projection
         {
            std::string mProjection;  //< "Geographic"
            std::string mUnits;       //< "DD?"
            std::string mZUnits;      //< "Meters"
            std::string mDatum;       //< "NAD83"
            double      mXShift;      //< 0.0
            double      mYShift;      //< 0.0
         };
   */

private:
   void InitializeFromHeader();

   // tbd int LoadProjection(const char* aProjectionFileName);

   Header      mHeader;
   std::string mDataFileName;
   bool        mHeaderLoaded;
   bool        mIsLoaded;
};

#endif
