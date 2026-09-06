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

// 'AdrgTileMatrix' is a specialization of the 'GeoTileMatrix'.
// It represents a single NIMA ADRG Zone Distribution Rectangle.

#include "AdrgTileMatrix.hpp"

#include <cstring>
#include <fstream>

#include "AdrgFile.hpp"
#include "GeoImageTile.hpp"
#include "NimaUtil.hpp"
#include "RIFData.hpp"
#include "UtLog.hpp"

//#define DEBUG_ON

// DATA_SET-ID_FIELD (tag DSI)
struct DSIData
{
public:
   char mProdType[4]; // "ADRG"
   char mImgName[8];  // ZDR image name "ssccddzz"
   char mFieldDelim;  // 0x1E
};

// GENERAL_INFORMATION_FIELD (tag GEN)
struct GENData
{
public:
   char mTypeCode; // 3=Raster RGB
   char mFill2[2 * 6];
   char mDensityUOM[3]; // Data density Unit-Of-Measure
   // The following 8 fields are the lat/lon of the corners of the
   // unpadded image.
   char mLLLon[11]; // Lower left longitude
   char mLLLat[10]; // Lower left latitude
   char mULLon[11]; // Upper left longitude
   char mULLat[10]; // Upper left latitude
   char mURLon[11];
   char mURLat[10];
   char mLRLon[11];
   char mLRLat[10];
   char mScale[9];
   char mZone[2];    // Zone 01..18
   char mSpacing[5]; // 100.0
   char mRectified;  // N
   char mAsz[8];
   char mBs[8];
   char mULLonPadded[11];
   char mULLatPadded[10];
   char mText[64];
   char mFieldDelim; // 0x1E
};

// DATA_SET_PARAMETERS_FIELD (tag SPR)
struct SPRData
{
public:
   char mURPixelRow[6];
   char mURPixelCol[6];
   char mLLPixelRow[6];
   char mLLPixelCol[6];
   char mNumTilesHigh[3];
   char mNumTilesWide[3];
   char mPixelsPerTileRow[6]; // 000128
   char mPixelsPerTileCol[6]; // 000128
   char mColumnDirection;     // 0 = left-to-right
   char mRowDirection;        // 1 = top-to-bottom
   char mPixelOrder;          // 0 = column in row in band in tile
   char mBitsPerPixel;        // 0 (not applicable)
   char mBitsPerPixelValue;   // 8
   char mFileName[12];        // ssccddzz.IMG
   char mTiled;
   char mFieldDelim; // 0x1E
};

// BAND_ID_FIELD (BDF)
struct BDFData
{
public:
   char mFill1[3 * 15];
   char mFieldDelim; // 0x1E
};

// TILE_INDEX_MAP_FIELD (TIM)
struct TIMData
{
public:
   char mFill[5];
   char mFieldDelim; // 0x1E
};

AdrgTileMatrix::AdrgTileMatrix(AdrgFile& aGenFile, const char* aDrDirName, int& aStatus)
   : ArcTileMatrix()
   , mFilePtr(nullptr)
{
   aStatus = 1; // Assume failure...

   // Skip the RECORD_ID_FIELD
   RIFData rif;
   aGenFile.Get(&rif, sizeof(rif));

   // Read the DATA_SET-ID_FIELD
   DSIData dsi;
   aGenFile.Get(&dsi, sizeof(dsi));

   // Read the GENERAL_INFORMATION_FIELD
   GENData gen;
   aGenFile.Get(&gen, sizeof(gen));
   mZone           = NimaUtil::StrToInt(gen.mZone, sizeof(gen.mZone));
   mLonDegPerPixel = 360.0 / NimaUtil::StrToInt(gen.mAsz, sizeof(gen.mAsz));
   mLatDegPerPixel = 360.0 / NimaUtil::StrToInt(gen.mBs, sizeof(gen.mBs));
   double lonBase  = aGenFile.StrToDeg(gen.mULLonPadded, sizeof(gen.mULLonPadded));
   double latBase  = aGenFile.StrToDeg(gen.mULLatPadded, sizeof(gen.mULLatPadded));

   // Read the DATA_SET_PARAMETERS_FIELD
   SPRData spr;
   aGenFile.Get(&spr, sizeof(spr));
   mURPixelRow          = NimaUtil::StrToInt(spr.mURPixelRow, sizeof(spr.mURPixelRow));
   mURPixelCol          = NimaUtil::StrToInt(spr.mURPixelCol, sizeof(spr.mURPixelCol));
   mLLPixelRow          = NimaUtil::StrToInt(spr.mLLPixelRow, sizeof(spr.mLLPixelRow));
   mLLPixelCol          = NimaUtil::StrToInt(spr.mLLPixelCol, sizeof(spr.mLLPixelCol));
   unsigned int numCols = NimaUtil::StrToInt(spr.mNumTilesWide, sizeof(spr.mNumTilesWide));
   unsigned int numRows = NimaUtil::StrToInt(spr.mNumTilesHigh, sizeof(spr.mNumTilesHigh));
   // TODO - need to deal with upper/lower case issue...
   char imageFileName[13];
   ::memcpy(imageFileName, spr.mFileName, 12);
   imageFileName[12] = '\0';
   NimaUtil::LowerCase(imageFileName);
   mFileName = aDrDirName;
   mFileName += imageFileName;

   // Skip the BAND_ID_FIELD
   BDFData bdf;
   aGenFile.Get(&bdf, sizeof(bdf));

   // Read the TILE_INDEX_MAP_FIELD if it is present
   if (spr.mTiled == 'Y')
   {
      char tempBuf[5];
      int  numTiles = numRows * numCols;
      mTileMap.reserve(numTiles);
      for (int i = 0; i < numTiles; ++i)
      {
         aGenFile.Get(tempBuf, 5);
         int mapEntry = NimaUtil::StrToInt(tempBuf, 5);
         mTileMap.push_back(mapEntry);
      }
      aGenFile.Get(tempBuf, 1); // Read the field delimiter...
   }

   // Populate the base class items

   SetNumRows(numRows);
   SetNumCols(numCols);
   SetLatTileDimDeg(mLatDegPerPixel * 128.0);
   SetLonTileDimDeg(mLonDegPerPixel * 128.0);

   double swLat = latBase - (numRows * 128 - 1) * mLatDegPerPixel;
   double swLon = lonBase;
   double neLat = latBase;
   double neLon = lonBase + (numCols * 128 - 1) * mLonDegPerPixel;
   SetSWLat(swLat);
   SetSWLon(swLon);
   SetNELat(neLat);
   SetNELon(neLon);

   SetDataSeries(NimaUtil::GetDataSeriesId(spr.mFileName));

#ifdef DEBUG_ON
   {
      double llLon = aGenFile.StrToDeg(gen.mLLLon, sizeof(gen.mLLLon));
      double llLat = aGenFile.StrToDeg(gen.mLLLat, sizeof(gen.mLLLat));
      double ulLon = aGenFile.StrToDeg(gen.mULLon, sizeof(gen.mULLon));
      double ulLat = aGenFile.StrToDeg(gen.mULLat, sizeof(gen.mULLat));
      double urLon = aGenFile.StrToDeg(gen.mURLon, sizeof(gen.mURLon));
      double urLat = aGenFile.StrToDeg(gen.mURLat, sizeof(gen.mURLat));
      double lrLon = aGenFile.StrToDeg(gen.mLRLon, sizeof(gen.mLRLon));
      double lrLat = aGenFile.StrToDeg(gen.mLRLat, sizeof(gen.mLRLat));

      auto out = ut::log::debug() << "ZDR:";
      out.AddNote() << "File: " << mFileName;

      out.AddNote() << "Scale: " << NimaUtil::StrToInt(gen.mScale, sizeof(gen.mScale));
      out.AddNote() << "Zone: " << NimaUtil::StrToInt(gen.mZone, sizeof(gen.mZone));
      out.AddNote() << "Asz: " << NimaUtil::StrToInt(gen.mAsz, sizeof(gen.mAsz));
      out.AddNote() << "Bs: " << NimaUtil::StrToInt(gen.mBs, sizeof(gen.mBs));
      out.AddNote() << "Lon Degrees Per Pixel: " << mLonDegPerPixel;
      out.AddNote() << "Lat Degrees Per Pixel: " << mLatDegPerPixel;
      out.AddNote() << "Upper Right Pixel Row: " << mURPixelRow;
      out.AddNote() << "Upper Right Pixel Col: " << mURPixelCol;
      out.AddNote() << "Lower Left  Pixel Row: " << mLLPixelRow;
      out.AddNote() << "Lower Left  Pixel Col: " << mLLPixelCol;
      out.AddNote() << "Number of Rows: " << numRows;
      out.AddNote() << "Number of Columns: " << numCols;
      out.AddNote() << "Base (upper left): " << latBase << ", " << lonBase;
      out.AddNote() << "Unpadded lower left: " << llLat << ", " << llLon;
      out.AddNote() << "Unpadded upper left: " << ulLat << ", " << ulLon;
      out.AddNote() << "Unpadded upper right: " << urLat << ", " << urLon;
      out.AddNote() << "Unpadded lower right: " << lrLat << ", " << lrLon;

      llLon = lonBase + (mLLPixelCol * mLonDegPerPixel);
      llLat = latBase - (mLLPixelRow * mLatDegPerPixel);
      out.AddNote() << "Computed unpadded lower left: " << llLat << ", " << llLon;

      ulLat = latBase - (mURPixelRow * mLatDegPerPixel);
      ulLon = llLon;
      out.AddNote() << "Computed unpadded upper left: " << ulLat << ", " << ulLon;

      urLat = ulLat;
      urLon = lonBase + (mURPixelCol * mLonDegPerPixel);
      out.AddNote() << "Computed unpadded upper right: " << urLat << ", " << urLon;

      lrLat = llLat;
      lrLon = urLon;
      out.AddNote() << "Computed unpadded lower right: " << lrLat << ", " << lrLon;

      out.AddNote() << "Rect SW corner: " << swLat << ", " << swLon;
      out.AddNote() << "Rect NE corner: " << neLat << ", " << neLon;
   }
#endif
   aStatus = 0;
}

AdrgTileMatrix::~AdrgTileMatrix()
{
   if (mFilePtr != nullptr)
   {
      ::fclose(mFilePtr);
      mFilePtr = nullptr;
   }
}

// Load the specified tile from the matrix.  The return value is zero
// for success and non-zero for failure.

// virtual
int AdrgTileMatrix::LoadTile(GeoImageTile* aImagePtr, int aRow, int aCol)
{
   int tileIndex = -1;
   int status    = 1;

   if ((aRow < 0) || (aRow >= static_cast<int>(GetNumRows())) || (aCol < 0) || (aCol >= static_cast<int>(GetNumCols())))
   {
      // Invalid address
   }
   else if (mTileMap.empty())
   {
      // No tile map present
      tileIndex = (aRow * GetNumCols()) + aCol;
   }
   else
   {
      // Tile map is present.  The entry is 0 if the tile is not present.
      tileIndex = mTileMap[(aRow * GetNumCols()) + aCol] - 1;
   }
   if (tileIndex >= 0)
   {
      if (mFilePtr == nullptr)
      {
         mFilePtr = ::fopen(mFileName.c_str(), "rb");
      }
      if (mFilePtr == nullptr)
      {
         auto out = ut::log::error() << "Unable to open ADRG file.";
         out.AddNote() << "File: " << mFileName;
      }
      if (mFilePtr != nullptr)
      {
         AdrgFile imgFile(mFilePtr);

         // Position to the proper tile.
         int tileOffset = tileIndex * 128 * 128 * 3;
         imgFile.Seek(2048 + tileOffset, SEEK_SET);

         // Convert from band-sequential-RGB to byte-RGB
         unsigned char* tilePtr = new unsigned char[128 * 128 * 3];
         unsigned char* tempBuf = new unsigned char[128 * 128 * 3];
         imgFile.Get(tempBuf, 128 * 128 * 3);
         const int gOff = 128 * 128;
         const int bOff = 128 * 128 * 2;
         for (int i = 0; i < (128 * 128); ++i)
         {
            int j          = i * 3;
            tilePtr[j]     = tempBuf[i];
            tilePtr[j + 1] = tempBuf[i + gOff];
            tilePtr[j + 2] = tempBuf[i + bOff];
         }
         delete[] tempBuf;

         double ulLatMat = GetNELat();
         double ulLonMat = GetSWLon();

         aImagePtr->SetImage(128, 128, tilePtr);
         double swLon = ulLonMat + (aCol * 128.0 * mLonDegPerPixel);
         double neLon = swLon + (128.0 * mLonDegPerPixel);
         double neLat = ulLatMat - (aRow * 128.0 * mLatDegPerPixel);
         double swLat = neLat - (128.0 * mLatDegPerPixel);
         aImagePtr->SetSWLat(swLat);
         aImagePtr->SetSWLon(swLon);
         aImagePtr->SetNELat(neLat);
         aImagePtr->SetNELon(neLon);
         aImagePtr->SetTileMatrix(this);
         aImagePtr->SetTileIndex((aRow * GetNumCols()) + aCol);

         status = 0;
      }
   }
   return status;
}
