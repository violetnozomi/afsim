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

#include "UtBitmapImage.hpp"

#include <cstdlib>
#include <iostream>

#include "UtLog.hpp"
#include "UtTypeInfo.hpp"


//! Set the color table in the image.
//!@param aColorTable The color table that the image will use.
//!@note This method is primarily used in 24 to 8 bit conversions.
void UtBitmapImage::SetColorTable(const UtBitmapImage::ColorTable& aColorTable)
{
   mColorTable                  = aColorTable;
   mDIB_Header.mColorsInPalette = static_cast<unsigned int>(aColorTable.size());
   mHeader.mDataOffsetByte      = 54 + 4 * mDIB_Header.mColorsInPalette;
   mHeader.mSizeInBytes         = mHeader.mDataOffsetByte + mDIB_Header.mImageSize;
}

// private
void UtBitmapImage::Initialize()
{
   mRowSize               = (mDIB_Header.mBitsPerPixel * mDIB_Header.mWidthInPixels + 31) / 32 * 4;
   mDIB_Header.mImageSize = mRowSize * std::abs(mDIB_Header.mHeightInPixels);

   if (mDIB_Header.mBitsPerPixel == 8)
   {
      mType        = cEIGHT_BIT;
      mTrueRowSize = mDIB_Header.mWidthInPixels;
   }
   else if (mDIB_Header.mBitsPerPixel == 24)
   {
      mType        = cTWENTY_FOUR_BIT;
      mTrueRowSize = mDIB_Header.mWidthInPixels * 3;
   }
   else if (mDIB_Header.mBitsPerPixel == 32)
   {
      mType        = cTHIRTY_TWO_BIT;
      mTrueRowSize = mDIB_Header.mWidthInPixels * 4;
   }

   mRawDataArray.clear();
   mTrueImageSize = mTrueRowSize * std::abs(mDIB_Header.mHeightInPixels);
   mRawDataArray.resize(mTrueImageSize);
}

UtBitmapImage::DataPtr UtBitmapImage::GetImage()
{
   DataPtr image;
   image.mBytePtr = mRawDataArray.data();
   return image;
}

//! Load a bitmap from a file
//! @param aFileName The name of the bitmap file to be loaded.
bool UtBitmapImage::Load(const std::string& aFileName)
{
   std::ifstream in;
   in.exceptions(std::ifstream::failbit | std::ifstream::badbit);
   try
   {
      in.open(aFileName.c_str(), std::ios_base::binary);
      in >> mHeader.mHeaderField[0];
      in >> mHeader.mHeaderField[1];
      in.read(reinterpret_cast<char*>(&mHeader.mSizeInBytes), 4);
      in >> mHeader.mReserved1[0];
      in >> mHeader.mReserved1[1];
      in >> mHeader.mReserved2[0];
      in >> mHeader.mReserved2[1];
      in.read(reinterpret_cast<char*>(&mHeader.mDataOffsetByte), 4);

      in.read(reinterpret_cast<char*>(&mDIB_Header.mSizeOfThisHeader), 4); // 40 bytes
      in.read(reinterpret_cast<char*>(&mDIB_Header.mWidthInPixels), 4);
      in.read(reinterpret_cast<char*>(&mDIB_Header.mHeightInPixels), 4);
      in.read(reinterpret_cast<char*>(&mDIB_Header.mColorPlanes), 2);       // must be 1
      in.read(reinterpret_cast<char*>(&mDIB_Header.mBitsPerPixel), 2);      // typically 1, 4, 8, 16, 24, 32
      in.read(reinterpret_cast<char*>(&mDIB_Header.mCompressionMethod), 4); // currently only 0 (none) supported
      in.read(reinterpret_cast<char*>(&mDIB_Header.mImageSize), 4);
      in.read(reinterpret_cast<char*>(&mDIB_Header.mHorizontalResolution), 4); // pixels per meter
      in.read(reinterpret_cast<char*>(&mDIB_Header.mVerticalResolution), 4);   // pixels per meter
      in.read(reinterpret_cast<char*>(&mDIB_Header.mColorsInPalette), 4);
      in.read(reinterpret_cast<char*>(&mDIB_Header.mNumberOfColorsUsed), 4);
   }
   catch (std::ifstream::failure& e)
   {
      auto out = ut::log::error() << "UtBitmapImage::Load Failed to load bitmap header from file";
      out.AddNote() << "Type: " << ut::TypeNameOf(e);
      out.AddNote() << "What: " << e.what();
      return false;
   }


   mColorTable.clear();
   unsigned colorTableSize = (mHeader.mDataOffsetByte - 54) / 4;
   for (unsigned entry = 0; entry < colorTableSize; ++entry)
   {
      ColorTableEntry cte;
      in.read(reinterpret_cast<char*>(&cte), 4);
      mColorTable.push_back(cte);
   }

   Initialize();

   int      height      = std::abs(mDIB_Header.mHeightInPixels);
   unsigned paddingSize = mRowSize - mTrueRowSize;
   char     padding[4];

   if (height == mDIB_Header.mHeightInPixels) // standard raster scan order, lower left to upper right
   {
      unsigned long offset = mTrueRowSize * (height - 1);
      for (int row = (height - 1); row >= 0; --row)
      {
         in.read(reinterpret_cast<char*>(mRawDataArray.data()) + offset, mTrueRowSize);
         in.read(padding, paddingSize);
         offset -= mTrueRowSize;
      }
   }
   else // negative value means upper left to lower right order.
   {
      unsigned long offset = 0;
      for (int row = 0; row < height; ++row)
      {
         in.read(reinterpret_cast<char*>(mRawDataArray.data()) + offset, mTrueRowSize);
         in.read(padding, paddingSize);
         offset += mTrueRowSize;
      }
   }
   return true;
}

//! Save a bitmap file to the specified location.
//! @param aFileName The name of the bitmap file to save.
bool UtBitmapImage::Save(const std::string& aFileName)
{
   if (mRawDataArray.empty())
   {
      ut::log::error() << "UtBitmapImage::Save File contains no data\n";
      return false;
   }

   std::ofstream out;
   out.exceptions(std::ostream::failbit | std::ostream::badbit);

   try
   {
      out.open(aFileName.c_str(), std::ios_base::binary);
      out << mHeader.mHeaderField[0];
      out << mHeader.mHeaderField[1];
      out.write(reinterpret_cast<char*>(&mHeader.mSizeInBytes), 4);
      out << mHeader.mReserved1[0];
      out << mHeader.mReserved1[1];
      out << mHeader.mReserved2[0];
      out << mHeader.mReserved2[1];
      out.write(reinterpret_cast<char*>(&mHeader.mDataOffsetByte), 4);

      out.write(reinterpret_cast<char*>(&mDIB_Header.mSizeOfThisHeader), 4); // 40 bytes
      out.write(reinterpret_cast<char*>(&mDIB_Header.mWidthInPixels), 4);
      out.write(reinterpret_cast<char*>(&mDIB_Header.mHeightInPixels), 4);
      out.write(reinterpret_cast<char*>(&mDIB_Header.mColorPlanes), 2);       // must be 1
      out.write(reinterpret_cast<char*>(&mDIB_Header.mBitsPerPixel), 2);      // typically 1, 4, 8, 16, 24, 32
      out.write(reinterpret_cast<char*>(&mDIB_Header.mCompressionMethod), 4); // currently only 0 (none) supported
      out.write(reinterpret_cast<char*>(&mDIB_Header.mImageSize), 4);
      out.write(reinterpret_cast<char*>(&mDIB_Header.mHorizontalResolution), 4); // pixels per meter
      out.write(reinterpret_cast<char*>(&mDIB_Header.mVerticalResolution), 4);   // pixels per meter
      out.write(reinterpret_cast<char*>(&mDIB_Header.mColorsInPalette), 4);
      out.write(reinterpret_cast<char*>(&mDIB_Header.mNumberOfColorsUsed), 4);
   }
   catch (std::ofstream::failure& e)
   {
      auto out = ut::log::error() << "UtBitmapImage::Save Failed to save bitmap header to file\n";
      out.AddNote() << "Type: " << ut::TypeNameOf(e);
      out.AddNote() << "What: " << e.what();
      return false;
   }

   for (auto& entry : mColorTable)
   {
      out.write((char*)(&entry), 4);
   }

   int      height      = std::abs(mDIB_Header.mHeightInPixels);
   unsigned paddingSize = mRowSize - mTrueRowSize;
   char     padding[4]  = {0, 0, 0, 0};

   if (height == mDIB_Header.mHeightInPixels) // standard raster scan order, lower left to upper right
   {
      unsigned long offset = mTrueRowSize * (height - 1);
      for (int row = (height - 1); row >= 0; --row)
      {
         out.write(reinterpret_cast<char*>(mRawDataArray.data()) + offset, mTrueRowSize);
         out.write(padding, paddingSize);
         offset -= mTrueRowSize;
      }
   }
   else // negative value means upper left to lower right order.
   {
      unsigned long offset = 0;
      for (int row = 0; row < height; ++row)
      {
         out.write(reinterpret_cast<char*>(mRawDataArray.data()) + offset, mTrueRowSize);
         out.write(padding, paddingSize);
         offset += mTrueRowSize;
      }
   }
   return true;
}

//! Get the pixel specified for the given row and column.
//! @param aRow The row of the pixel to retrieve.
//! @param aCol the column of the pixel to retrieve.
//! @return The DataPtr object of corresponding with the pixel value.
UtBitmapImage::DataPtr UtBitmapImage::GetPixel(int aRow, int aCol)
{
   DataPtr dataPtr;
   if ((aRow < mDIB_Header.mWidthInPixels) && (aCol < std::abs(mDIB_Header.mHeightInPixels)))
   {
      dataPtr.mBytePtr = (mRawDataArray.data()) + aCol * mTrueRowSize;
      if (mType == cEIGHT_BIT)
      {
         dataPtr.mBytePtr = (mRawDataArray.data());
         dataPtr.mBytePtr += aRow;
      }
      else if (mType == cTWENTY_FOUR_BIT)
      {
         dataPtr.mBGR_Ptr += aRow;
      }
      else if (mType == cTHIRTY_TWO_BIT)
      {
         dataPtr.mBGRA_Ptr += aRow;
      }
   }
   return dataPtr;
}


//! Convert a 24-bit bitmap to an eight bit bitmap with color table.
//! @param aColorTable The color table to use with the new 8-bit bitmap.
//! @return The new bitmap, or zero, if the conversion was unsuccessful.
//! @note If the color table is incomplete, this code will fill in the values as it finds them.
UtBitmapImage* UtBitmapImage::CreateEightBit(ColorTable& aColorTable)
{
   if (mDIB_Header.mBitsPerPixel != 24)
   {
      ut::log::error() << "UtBitmapImage::CreateEightBit Source bitmap file not 24 bit\n";
      return nullptr;
   }

   UtBitmapImage* newBitmap             = new UtBitmapImage(*this);
   newBitmap->mDIB_Header.mBitsPerPixel = 8U;
   newBitmap->mRawDataArray.clear();
   newBitmap->Initialize();

   int height  = std::abs(mDIB_Header.mHeightInPixels);
   int width   = mDIB_Header.mWidthInPixels;
   mColorTable = aColorTable;

   BGR_ToByteMap byte;
   unsigned char value = 0;
   for (const auto& entry : aColorTable)
   {
      byte[entry.mBGR] = value++;
   }

   unsigned char defaultValue = static_cast<unsigned char>(aColorTable.size());
   ColorTable    colorTable   = aColorTable;

   for (int row = 0; row < height; ++row)
   {
      int     BGR_offset  = 0;
      int     byte_offset = 0;
      DataPtr bgrRow;
      DataPtr byteRow;
      bgrRow.mBytePtr  = (mRawDataArray.data()) + row * mTrueRowSize;
      byteRow.mBytePtr = newBitmap->mRawDataArray.data() + row * newBitmap->mTrueRowSize;

      for (int col = 0; col < width; ++col)
      {
         BGR  bgr      = bgrRow.mBGR_Ptr[BGR_offset];
         auto BGR_Iter = byte.find(bgr);
         if (BGR_Iter != byte.end())
         {
            byteRow.mBytePtr[byte_offset] = BGR_Iter->second;
         }
         else
         {
            auto out = ut::log::warning() << "RGB value not mapped.";
            out.AddNote() << "R: " << static_cast<int>(bgr.mRed);
            out.AddNote() << "G: " << static_cast<int>(bgr.mGreen);
            out.AddNote() << "B: " << static_cast<int>(bgr.mBlue);
            out.AddNote() << "Mapping value to " << static_cast<int>(defaultValue) << "(default).";
            byte[bgr] = defaultValue;
            ColorTableEntry cte(bgr);
            colorTable.push_back(cte);
            byteRow.mBytePtr[byte_offset] = defaultValue++;
         }
         ++byte_offset;
         ++BGR_offset;
      }
   }

   newBitmap->SetColorTable(colorTable);
   return newBitmap;
}
