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

#ifndef UTBITMAPIMAGE_HPP
#define UTBITMAPIMAGE_HPP

#include "ut_export.h"

#include <fstream>
#include <map>
#include <string>
#include <vector>

class UtInput;

//! A representation of an image with bitmap (.bmp) format
//! Currently only 8, 24, and 32 bit types are supported.
class UT_EXPORT UtBitmapImage
{
public:
   UtBitmapImage() = default;

   enum Type
   {
      cEIGHT_BIT,
      cTWENTY_FOUR_BIT,
      cTHIRTY_TWO_BIT
   };

   struct Header
   {
      char     mHeaderField[2];
      unsigned mSizeInBytes;
      char     mReserved1[2];
      char     mReserved2[2];
      unsigned mDataOffsetByte;
   };

   struct DIB_Header
   {
      unsigned       mSizeOfThisHeader; // 40 bytes
      int            mWidthInPixels;
      int            mHeightInPixels;
      unsigned short mColorPlanes;       // must be 1
      unsigned short mBitsPerPixel;      // typically 1, 4, 8, 16, 24, 32
      unsigned short mCompressionMethod; // currently only 0 (none) supported
      unsigned       mImageSize;
      unsigned       mHorizontalResolution; // pixels per meter
      unsigned       mVerticalResolution;   // pixels per meter
      unsigned       mColorsInPalette;
      unsigned       mNumberOfColorsUsed;
   };

   struct BGR // for 24 bit
   {
      unsigned char mRed;
      unsigned char mGreen;
      unsigned char mBlue;

      bool operator<(const BGR& aRhs) const
      {
         // Swapped red and blue
         unsigned left  = mBlue + (mGreen << 8) + (mRed << 16);
         unsigned right = aRhs.mBlue + (aRhs.mGreen << 8) + (aRhs.mRed << 16);
         return left < right;
      }

      bool operator==(const BGR& aRhs) const
      {
         return (mRed == aRhs.mRed) && (mGreen == aRhs.mGreen) && (mBlue == aRhs.mBlue);
      }
   };

   struct BGRA // for 32 bit
   {
      unsigned char mRed;
      unsigned char mGreen;
      unsigned char mBlue;
      unsigned char mAlpha;

      bool operator==(const BGRA& aRhs) const
      {
         return (mRed == aRhs.mRed) && (mGreen == aRhs.mGreen) && (mBlue == aRhs.mBlue) && (mAlpha == aRhs.mAlpha);
      }
   };

   struct ColorTableEntry
   {
      BGR           mBGR;
      unsigned char mReserved;

      ColorTableEntry()
         : mBGR()
         , mReserved(0)
      {
      }

      ColorTableEntry(BGR& aBGR)
         : mBGR(aBGR)
         , mReserved(0)
      {
      }

      ColorTableEntry(unsigned char aRed, unsigned char aGreen, unsigned char aBlue)
         : mReserved(0)
      {
         mBGR.mRed   = aRed;
         mBGR.mGreen = aGreen;
         mBGR.mBlue  = aBlue;
      }
   };

   using ColorTable = std::vector<ColorTableEntry>;

   struct DataPtr
   {
      union
      {
         unsigned char* mBytePtr{nullptr}; // eight-bit grayscale
         BGR*           mBGR_Ptr;
         BGRA*          mBGRA_Ptr;
      };
   };

   bool Load(const std::string& aFileName);
   bool Save(const std::string& aFileName);

   DataPtr GetImage();

   DataPtr GetPixel(int aRow, int aCol);

   //! Return the header data for this bitmap.
   const Header& GetHeader() const { return mHeader; }

   //! Return the DIB_Header data for this bitmap.
   const DIB_Header& GetDIB_Header() const { return mDIB_Header; }

   //! Return the type of bitmap.
   Type GetType() const { return mType; }

   void SetColorTable(const ColorTable& aColorTable);

   // The following is for specialized conversions of 24 to 8 bytes.
   // The "palette map" is passed in as an argument.
   UtBitmapImage* CreateEightBit(ColorTable& aColorTable);

private:
   using BGR_ToByteMap = std::map<BGR, unsigned char>;
   using byte          = unsigned char;

   void Initialize();

   Header     mHeader;
   DIB_Header mDIB_Header;
   ColorTable mColorTable;
   Type       mType{cTWENTY_FOUR_BIT};

   long mRowSize{0};
   long mTrueRowSize{0};
   long mTrueImageSize{0};

   std::vector<byte> mRawDataArray;
};

#endif
