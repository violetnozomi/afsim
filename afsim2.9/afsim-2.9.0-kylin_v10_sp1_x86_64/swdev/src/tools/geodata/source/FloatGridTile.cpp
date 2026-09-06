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

#include "FloatGridTile.hpp"

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <sstream>

#include "UtLog.hpp"

FloatGridTile::FloatGridTile()
   : GeoElevationTile()
   , mDataFileName("")
   , mHeaderLoaded(false)
   , mIsLoaded(false)
{
   SetDataType(cFLOAT);
   SetDataOrderType(cROW_MAJOR);
}

void FloatGridTile::InitializeFromHeader()
{
   mLatPoints = mHeader.mNRows;
   mLonPoints = mHeader.mNCols;

   mLatInterval      = mHeader.mCellSize;
   mHalfLatInterval  = 0.5 * mLatInterval;
   mRecipLatInterval = 1.0 / mLatInterval;

   mLonInterval      = mHeader.mCellSize;
   mHalfLonInterval  = 0.5 * mLonInterval;
   mRecipLonInterval = 1.0 / mLonInterval;

   // Allocate the space for the actual elevation data
   unsigned totalPoints = mLonPoints * mLatPoints;
   CreateData(totalPoints);
}

// Note: this is a low-level interface routine; do not use this method
// in conjunction with a FloatGridTileManager!
int FloatGridTile::UnloadData()
{
   if (mIsLoaded)
   {
      DeleteData();
      mIsLoaded = false;
   }
   return 0;
}

// Load the information for the cell that contains the specified location.
// aStatus = 0 if success.
//           1 if the file cannot be opened.
//           2 if the contents appear bad.

// Load a cell into the object.  The return value is:
//   0 = success;
//   1 = could not open file;
//   2 = contents do not appear to be a Dem file.
//
int FloatGridTile::LoadData()
{
   std::ifstream demFile;
   demFile.open(mDataFileName, std::ifstream::in | std::ifstream::binary);
   if (!demFile.is_open())
   {
      return 1;
   }

   InitializeFromHeader();

   // Read the data.  Each record represents a line of constant longitude.
   // The values are stored from south to north.

   // Direct read of rows in reverse order if byte ordering is compatible.
   if (mHeader.mByteOrder == "LSBFIRST") // true for all supported machines
   {
      for (unsigned row = 1; row <= mHeader.mNRows; ++row)
      {
         float* rowPtr = mData.mAsFloat + (mHeader.mNRows - row) * mHeader.mNCols;
         char*  buffer = reinterpret_cast<char*>(rowPtr);
         demFile.read(buffer, (sizeof(float) * mHeader.mNCols));
         /*
         //  validation
         for (unsigned col=0; col<mHeader.mNCols; ++col)
         {
            if (rowPtr[col] == 0.0)
            {
               auto out = ut::log::error() << "Zero data in FloatGridTile.";
               out.AddNote() << "Row: " << row;
               out.AddNote() << "Col: " << col;
            }
         }
         */
      }
   }
   else
   {
      for (unsigned row = 1; row <= mHeader.mNRows; ++row)
      {
         float* rowPtr = mData.mAsFloat + (mHeader.mNRows - row) * mHeader.mNCols;
         char*  buffer = reinterpret_cast<char*>(rowPtr);
         demFile.read(buffer, (sizeof(float) * mHeader.mNCols));
         for (int i = 0; i + 3 < demFile.gcount(); i += 4)
         {
            // swap the top two bytes:
            std::swap(buffer[i], buffer[i + 3]);
            std::swap(buffer[i + 1], buffer[i + 2]);
         }
      }
   }

   mIsLoaded = true;
   demFile.close(); // Close file and release buffers
#ifdef CHECK_DATA
   auto out = ut::log::debug() << "Checking cell for bad data. (diagnostic build: CHECK_DATA)";
   for (unsigned row = 0; row < mHeader.mNRows; ++row)
   {
      for (unsigned col = 0; col < mHeader.mNCols; ++col)
      {
         const double badLow  = mHeader.mNoDataValue - 0.01;
         const double badHigh = mHeader.mNoDataValue + 0.01;
         float        data    = mData.mAsFloat[row * mHeader.mNCols + col];
         if ((data >= badLow) && (data <= badHigh))
         {
            auto note = out.AddNote("error") << "Bad data found.";
            note.AddNote() << "Row: " << row;
            note.AddNote() << "Col: " << col;
            note.AddNote() << "Data: " << data;
         }
      }
   }
#endif
   return 0;
}

int FloatGridTile::SaveTile(const char* aTileName)
{
   std::string tileName(aTileName);
   std::string headerName = tileName + ".hdr";

   // write header data
   std::fstream header;
   header.open(headerName.c_str(), std::ios_base::out);

   // clang-format off
   header   << std::setprecision(14);
   header   << "ncols        " << mHeader.mNCols        << "\n"
            << "nrows        " << mHeader.mNRows        << "\n"
            << "xllcorner    " << mHeader.mXLLCorner    << "\n"
            << "yllcorner    " << mHeader.mYLLCorner    << "\n"
            << "NODATA_value " << mHeader.mNoDataValue  << "\n"
            << "cellsize     " << mHeader.mCellSize     << "\n"
            << "byteorder    " << mHeader.mByteOrder    << "\n";
   // clang-format on
   header.close();

   std::string dataFileName = tileName + ".flt";
   mDataFileName            = dataFileName;
   FILE* data;
   data = fopen(dataFileName.c_str(), "wb");

   for (unsigned row = 1; row <= mHeader.mNRows; ++row)
   {
      float* rowPtr = mData.mAsFloat + (mHeader.mNRows - row) * mHeader.mNCols;
      ::fwrite(rowPtr, 4, mHeader.mNCols, data);
   }

   fclose(data);
   return 0;
}

int FloatGridTile::LoadTile(const char* aFileName)
{
   int status = 0;
   if (!mDummyTile)
   {
      if (!mHeaderLoaded)
      {
         std::string headerFileName = std::string(aFileName);
         status = LoadHeader(headerFileName); // saves data file name for use with LoadData call, below.
      }
      if (status == 0)
      {
         status = LoadData();
      }
   }
   else
   {
      mIsLoaded = true;
   }
   return status;
}

//! Make a dummy tile for a standard float grid structure.
//! This places the tile on the standard 0.1 degree grid set up
//! in standard directories.  For irregular grids or file use,
//! the base class' implementation should be used instead.
void FloatGridTile::MakeDummyTile(double aLat, double aLon, double aLatInterval, double aLonInterval)
{
   double lat = static_cast<int>(aLat * 10.0) / 10.0;
   double lon = static_cast<int>(aLon * 10.0) / 10.0;
   if (lat < 0.0)
   {
      lat -= 0.1;
   }
   if (lon < 0.0)
   {
      lon -= 0.1;
   }

   // Use base class' implementation (we could be using default parameters for 0.1 degrees interval).
   GeoLatLonGridTile::MakeDummyTile(lat, lon, aLatInterval, aLonInterval);

   mHeader.mCellSize    = mLatInterval;
   mHeader.mNCols       = mLonPoints;
   mHeader.mNRows       = mLatPoints;
   mHeader.mNoDataValue = -9999;
   mHeader.mXLLCorner   = GetSWLon();
   mHeader.mYLLCorner   = GetSWLat();

   mIsLoaded = true;
}

int FloatGridTile::LoadHeader(const std::string& aFileName)
{
   mDataFileName              = aFileName + ".flt";
   std::string headerFileName = aFileName + ".hdr";
   int         status         = LoadHeader(headerFileName, mHeader);
   mHeaderLoaded              = (status == 0);
   if (status == 0)
   {
      mHeaderLoaded = true;
      SetSWLon(mHeader.GetSWLon());
      SetNELon(mHeader.GetNELon());
      SetSWLat(mHeader.GetSWLat());
      SetNELat(mHeader.GetNELat());
   }
   return status;
}

// static
int FloatGridTile::LoadHeader(std::ifstream& aStream, Header& aHeader,
                              unsigned aLines) // = 1000
{
   int status = 0;
   if (aStream)
   {
      std::string line;
      unsigned    lineNum = 1;
      while ((lineNum <= aLines) && getline(aStream, line))
      {
         ++lineNum;
         static const std::string space(" \t");
         std::string::size_type   begPos    = 0;
         std::string::size_type   endPos    = line.find_first_of(space);
         std::string              firstWord = line.substr(begPos, endPos - begPos);
         begPos                             = line.find_first_not_of(space, endPos);
         std::string secondWord             = line.substr(begPos);
         if (firstWord == "byteorder")
         {
            aHeader.mByteOrder = secondWord;
         }
         else
         {
            std::istringstream iss(secondWord);
            if (firstWord == "ncols")
            {
               iss >> aHeader.mNCols;
            }
            else if (firstWord == "nrows")
            {
               iss >> aHeader.mNRows;
            }
            else if (firstWord == "xllcorner")
            {
               iss >> aHeader.mXLLCorner;
            }
            else if (firstWord == "yllcorner")
            {
               iss >> aHeader.mYLLCorner;
            }
            else if (firstWord == "NODATA_value")
            {
               iss >> aHeader.mNoDataValue;
            }
            else if (firstWord == "cellsize")
            {
               iss >> aHeader.mCellSize;
            }
            else
            {
               // bad data suspected.
               status = 2;
               break;
            }
         }
      }
   }
   else
   {
      status = 1;
   }
   return status;
}

int FloatGridTile::LoadHeader(const std::string& aHeaderFileName, Header& aHeader)
{
   // Each line in the file consists of pairs of space-separated identifiers
   std::ifstream ifs(aHeaderFileName.c_str());
   return LoadHeader(ifs, aHeader);
}

// This is a convenience method for allocating tiles.  This
// is typically passed as an argument to the tile manager.
// It simply creates a new instance of a FloatGridTile.

// static
FloatGridTile* FloatGridTile::TileAllocator()
{
   return new FloatGridTile();
}

bool FloatGridTile::FillSubtile(const GeoRect&  aBoundingBox,
                                FloatGridTile*& aSubTilePtr,
                                unsigned        aDownSampleFactor) // =1
{
   int hasIntersection = Intersects(aBoundingBox);
   if (hasIntersection == 1)
   {
      // Union is the area to be filled; expand it to have one row / col of overlap.
      bool isNewTile = (aSubTilePtr == nullptr);
      if (isNewTile)
      {
         aSubTilePtr = new FloatGridTile();
         ut::log::info() << "Creating new subtile.";
         aSubTilePtr->mHeader = mHeader;
      }

      int swLatIndex = (int)((aBoundingBox.GetSWLat() - GetSWLat()) / mHeader.mCellSize) - 1; // could be less than zero.
      int swLonIndex = (int)((aBoundingBox.GetSWLon() - GetSWLon()) / mHeader.mCellSize) - 1; // could be less than zero.

      if (isNewTile)
      {
         if (Contains(aBoundingBox) && aBoundingBox.Contains(*this))
         {
            // Special case; this is the same bounding box
            aSubTilePtr->SetSWLat(GetSWLat());
            aSubTilePtr->SetSWLon(GetSWLon());
            aSubTilePtr->SetNELat(GetNELat());
            aSubTilePtr->SetNELon(GetNELon());

            if (aDownSampleFactor > 1)
            {
               aSubTilePtr->mHeader.mCellSize *= aDownSampleFactor;
               aSubTilePtr->mHeader.mNRows /= aDownSampleFactor;
               aSubTilePtr->mHeader.mNCols /= aDownSampleFactor;
            }
         }
         else
         {
            // The following indices represent the bounding box offset wrt "this" tile.
            // Note that we overlap by two posts.  In float-grid we are not guaranteed to have posts directly on the
            // edge of the 0.1 degree boundary.  Overlapping in this way assures that we do not create any holes in
            // the "stitching" of the tiles.
            aSubTilePtr->SetSWLat(GetSWLat() + swLatIndex * mHeader.mCellSize); // exact boundary based on parent tile.

            aSubTilePtr->SetSWLon(GetSWLon() + swLonIndex * mHeader.mCellSize); // exact boundary based on parent tile.

            int neLatIndex = (int)((aBoundingBox.GetNELat() - GetSWLat()) / mHeader.mCellSize) + 2;
            aSubTilePtr->SetNELat(GetSWLat() + neLatIndex * mHeader.mCellSize);

            int neLonIndex = (int)((aBoundingBox.GetNELon() - GetSWLon()) / mHeader.mCellSize) + 2;
            aSubTilePtr->SetNELon(GetSWLon() + neLonIndex * mHeader.mCellSize);

            aSubTilePtr->mHeader.mYLLCorner = aSubTilePtr->GetSWLat();
            aSubTilePtr->mHeader.mXLLCorner = aSubTilePtr->GetSWLon();
            if (aDownSampleFactor > 1)
            {
               aSubTilePtr->mHeader.mCellSize *= aDownSampleFactor;
            }
            aSubTilePtr->mHeader.mNRows = static_cast<unsigned>((aSubTilePtr->GetNELat() - aSubTilePtr->GetSWLat()) /
                                                                aSubTilePtr->mHeader.mCellSize) +
                                          2;
            aSubTilePtr->mHeader.mNCols = static_cast<unsigned>((aSubTilePtr->GetNELon() - aSubTilePtr->GetSWLon()) /
                                                                aSubTilePtr->mHeader.mCellSize) +
                                          2;
         }

         // test code
         aSubTilePtr->GetHeader().PrintData();
         // end test code
         aSubTilePtr->InitializeFromHeader();
      }

      // Now copy data
      unsigned numRows        = aSubTilePtr->mHeader.mNRows;
      unsigned numCols        = aSubTilePtr->mHeader.mNCols;
      bool     allDataInvalid = true;

      for (unsigned latIndex = 0; latIndex < numRows; ++latIndex)
      {
         for (unsigned lonIndex = 0; lonIndex < numCols; ++lonIndex)
         {
            int sourceLatIndex = latIndex * aDownSampleFactor + swLatIndex;
            int sourceLonIndex = lonIndex * aDownSampleFactor + swLonIndex;
            int destIndex      = (latIndex * aSubTilePtr->mLonPoints) + lonIndex;

            if ((sourceLatIndex >= 0) && (sourceLatIndex < (int)mLatPoints) && (sourceLonIndex >= 0) &&
                (sourceLonIndex < (int)mLonPoints))
            {
               int  sourceIndex  = (sourceLatIndex * mLonPoints) + sourceLonIndex;
               bool datumInvalid = ((int)mData.mAsFloat[sourceIndex] == mHeader.mNoDataValue);
               allDataInvalid &= datumInvalid;
               if (!datumInvalid)
               {
                  aSubTilePtr->mData.mAsFloat[destIndex] = mData.mAsFloat[sourceIndex];
               }
            }
            else if (isNewTile)
            {
               // Store zeroes instead of invalid
               aSubTilePtr->mData.mAsFloat[destIndex] = (float)mHeader.mNoDataValue;
            }
         }
      }
      if (isNewTile && allDataInvalid)
      {
         // No need to create
         ut::log::info() << "Subtile: All Data Invalid.";
         delete aSubTilePtr;
         aSubTilePtr = nullptr;
      }
      else
      {
         aSubTilePtr->mIsLoaded = true;
      }
   }
   return (hasIntersection == 1);
}

void FloatGridTile::Header::PrintData() const
{
   double tileSizeLat = mCellSize * mNRows;
   double tileSizeLon = mCellSize * mNCols;
   auto   out         = ut::log::info() << "Header:";
   { // RAII block
      auto note = out.AddNote() << "Tile Extents:";
      note.AddNote() << "Lower Left: " << mYLLCorner << ", " << mXLLCorner;
      note.AddNote() << "Upper Right: " << mYLLCorner + tileSizeLat << ", " << mXLLCorner + tileSizeLon;
   }

   { // RAII block
      auto note = out.AddNote() << "Tile Size:";
      note.AddNote() << "Lat Size: " << tileSizeLat;
      note.AddNote() << "Lon Size: " << tileSizeLon;
   }
}
