// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "UtTiff.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>

#include "UtCast.hpp"

namespace
{
template<typename T>
T* NewArray(size_t aAllocations)
{
   if (aAllocations <= std::numeric_limits<size_t>::max() / sizeof(T))
   {
      return new T[aAllocations];
   }
   return nullptr;
}
} // namespace

void UtTiff::WriteToFile(const std::string& aFilename) const
{
   CompatibleFileStream file(aFilename, std::fstream::out);
   if (file.IsNativeLittleEndian())
   {
      file.Write("II", 2, CompatibleFileStream::cNATIVE);
   }
   else
   {
      file.Write("MM", 2, CompatibleFileStream::cNATIVE);
   }
   uint16_t magicnumber = 42;
   file.Write(reinterpret_cast<const char*>(&magicnumber), sizeof(magicnumber));
   uint32_t offset = 8;
   size_t   count  = mImageList.size();
   file.Write(reinterpret_cast<const char*>(&offset), sizeof(offset));
   for (auto&& it : mImageList)
   {
      count--;
      it.WriteToFile(file, offset, count == 0);
      // write the image, update the offset
   }
   file.Close();
}

bool UtTiff::IsTiffFile(const std::string& aFilename)
{
   CompatibleFileStream file(aFilename, std::fstream::in);
   uint16_t             endian;
   file.Read(reinterpret_cast<char*>(&endian), sizeof(endian), CompatibleFileStream::cNATIVE);
   CompatibleFileStream::Mode readMode =
      (endian == 18761) ? CompatibleFileStream::cLITTLE_ENDIAN : CompatibleFileStream::cBIG_ENDIAN;
   uint16_t magicnumber;
   file.Read(reinterpret_cast<char*>(&magicnumber), sizeof(magicnumber), readMode);
   return (magicnumber == 42);
}

UtTiff::PreviewList UtTiff::PreviewHeaderInformation(const std::string& aFilename)
{
   bool                 scaleValid    = false;
   bool                 tiepointValid = false;
   double               scaleX        = 1.0;
   double               scaleY        = 1.0;
   double               tiePointX     = 0.0;
   double               tiePointY     = 0.0;
   PreviewList          retval;
   CompatibleFileStream file(aFilename, std::fstream::in);
   uint16_t             endian;
   file.Read(reinterpret_cast<char*>(&endian), sizeof(endian), CompatibleFileStream::cNATIVE);
   CompatibleFileStream::Mode readMode =
      (endian == 18761) ? CompatibleFileStream::cLITTLE_ENDIAN : CompatibleFileStream::cBIG_ENDIAN;
   uint16_t magicnumber;
   file.Read(reinterpret_cast<char*>(&magicnumber), sizeof(magicnumber), readMode);
   uint32_t offset;
   file.Read(reinterpret_cast<char*>(&offset), sizeof(offset), readMode);
   while (offset != 0)
   {
      ImagePreview ip;
      uint16_t     numtags;
      file.SeekG(offset);
      file.Read(reinterpret_cast<char*>(&numtags), sizeof(numtags), readMode);
      for (uint16_t i = 0; i < numtags; ++i)
      {
         TagId tagId;
         file.Read(reinterpret_cast<char*>(&tagId), sizeof(tagId), readMode);
         DataType dataType;
         file.Read(reinterpret_cast<char*>(&dataType), sizeof(dataType), readMode);
         uint32_t dataNum;
         file.Read(reinterpret_cast<char*>(&dataNum), sizeof(dataNum), readMode);
         uint32_t toffset;
         file.Read(reinterpret_cast<char*>(&toffset), sizeof(toffset), readMode);
         switch (tagId)
         {
         case cSoftware:
            if (dataType == cASCII)
            {
               std::string tagValue = ReadTagString(file, toffset, dataNum);
               if (tagValue == "AFSIM SensorPlot")
               {
                  ip.mIsSensorPlot = true;
               }
            }
            break;
         case cImageDescription:
            if (dataType == cASCII)
            {
               ip.mDescription = ReadTagString(file, toffset, dataNum);
            }
            break;
         case cSMinSampleValue:
            if ((dataType == cSLong) && (dataNum == 1))
            {
               file.SeekG(-4, std::ios_base::cur);
               file.Read(reinterpret_cast<char*>(&ip.mMinValue), sizeof(ip.mMinValue), readMode);
            }
            break;
         case cSMaxSampleValue:
            if ((dataType == cSLong) && (dataNum == 1))
            {
               file.SeekG(-4, std::ios_base::cur);
               file.Read(reinterpret_cast<char*>(&ip.mMaxValue), sizeof(ip.mMaxValue), readMode);
            }
            break;
         case cGeokeyDirectoryTag:
         {
            ip.mIsGeotiff = true;
            auto g        = file.TellG();
            file.SeekG(toffset);
            for (uint32_t i = 0; i < dataNum / 4; ++i)
            {
               uint16_t geotag;
               uint16_t d2;
               uint16_t d3;
               uint16_t d4;
               file.Read(reinterpret_cast<char*>(&geotag), sizeof(geotag), readMode);
               file.Read(reinterpret_cast<char*>(&d2), sizeof(d2), readMode);
               file.Read(reinterpret_cast<char*>(&d3), sizeof(d3), readMode);
               file.Read(reinterpret_cast<char*>(&d4), sizeof(d4), readMode);
               if ((geotag == 1024) && (d4 == 2)) // GTModelTypeGeoKey, Geographic latitude-longitude System
               {
                  ip.mIsEquirectangular = true;
               }
            }
            file.SeekG(g);
         }
         break;
         case cModelPixelScaleTag:
            if ((dataType == cDouble) && (dataNum >= 2)) // get a pixel scale, we write three doubles
            {
               scaleValid = true;
               auto g     = file.TellG();
               file.SeekG(toffset);
               file.Read(reinterpret_cast<char*>(&scaleX), sizeof(scaleX), readMode);
               file.Read(reinterpret_cast<char*>(&scaleY), sizeof(scaleX), readMode);
               file.SeekG(g);
            }
            break;
         case cModelTiepointTag:
            if ((dataType == cDouble) && (dataNum == 6)) // get the tiepoint coordinates, we write six doubles
            {
               tiepointValid = true;
               auto g        = file.TellG();
               file.SeekG(toffset);
               double empty;
               file.Read(reinterpret_cast<char*>(&empty), sizeof(empty), readMode);
               file.Read(reinterpret_cast<char*>(&empty), sizeof(empty), readMode);
               file.Read(reinterpret_cast<char*>(&empty), sizeof(empty), readMode);
               file.Read(reinterpret_cast<char*>(&tiePointX), sizeof(tiePointX), readMode);
               file.Read(reinterpret_cast<char*>(&tiePointY), sizeof(tiePointY), readMode);
               file.SeekG(g);
            }
            break;
         case cCompression:
            if ((dataType == cShort) && (dataNum == 1))
            {
               file.SeekG(-4, std::ios_base::cur);
               file.Read(reinterpret_cast<char*>(&ip.mCompression), sizeof(ip.mCompression), readMode);
               long dummy;
               file.Read(reinterpret_cast<char*>(&dummy), 4 - sizeof(ip.mCompression), readMode);
            }
            break;
         case cImageWidth:
            if ((dataType == cShort) && (dataNum == 1))
            {
               file.SeekG(-4, std::ios_base::cur);
               short imageW;
               file.Read(reinterpret_cast<char*>(&imageW), sizeof(imageW), readMode);
               long dummy;
               file.Read(reinterpret_cast<char*>(&dummy), 4 - sizeof(imageW), readMode);
               ip.mImageWidth = imageW;
            }
            else if ((dataType == cLong) && (dataNum == 1))
            {
               file.SeekG(-4, std::ios_base::cur);
               file.Read(reinterpret_cast<char*>(&ip.mImageWidth), sizeof(ip.mImageWidth), readMode);
            }
            break;
         case cImageHeight:
            if ((dataType == cShort) && (dataNum == 1))
            {
               file.SeekG(-4, std::ios_base::cur);
               short imageH;
               file.Read(reinterpret_cast<char*>(&imageH), sizeof(imageH), readMode);
               long dummy;
               file.Read(reinterpret_cast<char*>(&dummy), 4 - sizeof(imageH), readMode);
               ip.mImageHeight = imageH;
            }
            else if ((dataType == cLong) && (dataNum == 1))
            {
               file.SeekG(-4, std::ios_base::cur);
               file.Read(reinterpret_cast<char*>(&ip.mImageHeight), sizeof(ip.mImageHeight), readMode);
            }
            break;
         default:
            break;
         }
      }
      if ((ip.mImageHeight > 0) && (ip.mImageWidth > 0))
      {
         ip.mIsAFSIM_Usable = true;
      }
      else
      {
         ip.mIsAFSIM_Usable = false;
      }
      if (scaleValid && tiepointValid && ip.mIsEquirectangular)
      {
         ip.mLonMin = tiePointX;
         ip.mLatMax = tiePointY;
         ip.mLonMax = tiePointX + scaleX * ip.mImageWidth;
         ip.mLatMin = tiePointY - scaleY * ip.mImageHeight;
      }
      else
      {
         ip.mIsEquirectangular = false;
      }
      retval.emplace_back(ip);
      file.Read(reinterpret_cast<char*>(&offset), sizeof(offset), readMode);
   }
   file.Close();


   return retval;
}

//! returns an int, 0 is ok, 1 is unopenable, 2 is error reading
//! systems expect a south to north configuration, while geotiffs are generally north to south, this method will flip
//! the data the method anticipates:
//!   TIFFTAG cImageWidth(256) > 0
//!   TIFFTAG cImageHeight(257) > 0
//!   TIFFTAG cBitsPerSample(258) = 2 (for int) | 4 (for ieeefp) | 8 (for ieeefp)
//!   TIFFTAG cCompression(259) = cNone(1)
//!   TIFFTAG cSamplesPerPixel(277) = 1
//!   TIFFTAG cSampleFormat(339) = cSampleFormat_int(2) | cSampleFormat_ieeefp(3)
//!   TIFFTAG cStripOffsets(273) & cStripByteCounts(279) have equally sized arrays
//!                                     OR
//!   TIFFTAG cTileWidth(322) & cTileLength(323) & cTileOffsets(324) are valid
int UtTiff::ReadElevationData(const std::string& aFilename,
                              unsigned int       aImageNumber,
                              ComponentFormat&   aFormat,
                              void*&             aData,
                              unsigned int&      aLatPoints,
                              unsigned int&      aLonPoints)
{
   unsigned int         image = 0;
   PreviewList          retval;
   CompatibleFileStream file(aFilename, std::fstream::in);
   uint16_t             endian;
   file.Read(reinterpret_cast<char*>(&endian), sizeof(endian), CompatibleFileStream::cNATIVE);
   CompatibleFileStream::Mode readMode =
      (endian == 18761) ? CompatibleFileStream::cLITTLE_ENDIAN : CompatibleFileStream::cBIG_ENDIAN;
   uint16_t magicnumber;
   file.Read(reinterpret_cast<char*>(&magicnumber), sizeof(magicnumber), readMode);
   uint32_t offset;
   file.Read(reinterpret_cast<char*>(&offset), sizeof(offset), readMode);
   while (offset != 0)
   {
      std::vector<uint32_t> stripOffset;
      std::vector<uint32_t> stripLength;
      size_t                tileWidth  = 0;
      size_t                tileLength = 0;
      std::vector<uint32_t> tileOffset;
      int                   bytesPerSample = 0;
      size_t                imageSizeBytes = 0;
      SampleFormat          sampleFormat   = cSampleFormat_undefined;
      bool                  valid          = true;
      uint16_t              numtags;
      file.SeekG(offset);
      file.Read(reinterpret_cast<char*>(&numtags), sizeof(numtags), readMode);
      std::function<void(void*)> CleanUp;
      for (uint16_t i = 0; i < numtags; ++i)
      {
         TagId tagId;
         file.Read(reinterpret_cast<char*>(&tagId), sizeof(tagId), readMode);
         DataType dataType;
         file.Read(reinterpret_cast<char*>(&dataType), sizeof(dataType), readMode);
         uint32_t dataNum;
         file.Read(reinterpret_cast<char*>(&dataNum), sizeof(dataNum), readMode);
         uint32_t toffset;
         file.Read(reinterpret_cast<char*>(&toffset), sizeof(toffset), readMode);

         if (image == aImageNumber)
         {
            switch (tagId)
            {
            case (cImageWidth):
               if ((dataType == cShort) && (dataNum == 1))
               {
                  file.SeekG(-4, std::ios_base::cur);
                  unsigned short imageW;
                  file.Read(reinterpret_cast<char*>(&imageW), sizeof(imageW), readMode);
                  long dummy;
                  file.Read(reinterpret_cast<char*>(&dummy), 4 - sizeof(imageW), readMode);
                  aLonPoints = imageW;
               }
               else if ((dataType == cLong) && (dataNum == 1))
               {
                  file.SeekG(-4, std::ios_base::cur);
                  unsigned long imageW;
                  file.Read(reinterpret_cast<char*>(&imageW), sizeof(imageW), readMode);
                  aLonPoints = imageW;
               }
               break;
            case (cImageHeight):
               if ((dataType == cShort) && (dataNum == 1))
               {
                  file.SeekG(-4, std::ios_base::cur);
                  unsigned short imageH;
                  file.Read(reinterpret_cast<char*>(&imageH), sizeof(imageH), readMode);
                  long dummy;
                  file.Read(reinterpret_cast<char*>(&dummy), 4 - sizeof(imageH), readMode);
                  aLatPoints = imageH;
               }
               else if ((dataType == cLong) && (dataNum == 1))
               {
                  file.SeekG(-4, std::ios_base::cur);
                  unsigned long imageH;
                  file.Read(reinterpret_cast<char*>(&imageH), sizeof(imageH), readMode);
                  aLatPoints = imageH;
               }
               break;
            case (cBitsPerSample):
               if ((dataType == cShort) && (dataNum == 1))
               {
                  file.SeekG(-4, std::ios_base::cur);
                  short bps;
                  file.Read(reinterpret_cast<char*>(&bps), sizeof(bps), readMode);
                  long dummy;
                  file.Read(reinterpret_cast<char*>(&dummy), 4 - sizeof(bps), readMode);
                  bytesPerSample = bps / 8;
               }
               break;
            case (cCompression):
               if ((dataType == cShort) && (dataNum == 1))
               {
                  short compression;
                  file.SeekG(-4, std::ios_base::cur);
                  file.Read(reinterpret_cast<char*>(&compression), sizeof(compression), readMode);
                  long dummy;
                  file.Read(reinterpret_cast<char*>(&dummy), 4 - sizeof(compression), readMode);
                  if (compression != 1)
                  {
                     valid = false;
                  }
               }
               break;
            case (cSamplesPerPixel):
               if ((dataType == cShort) && (dataNum == 1))
               {
                  short spp;
                  file.SeekG(-4, std::ios_base::cur);
                  file.Read(reinterpret_cast<char*>(&spp), sizeof(spp), readMode);
                  long dummy;
                  file.Read(reinterpret_cast<char*>(&dummy), 4 - sizeof(spp), readMode);
                  if (spp != 1)
                  {
                     valid = false;
                  }
               }
               break;
            case (cSampleFormat):
               if ((dataType == cShort) && (dataNum == 1))
               {
                  short sf;
                  file.SeekG(-4, std::ios_base::cur);
                  file.Read(reinterpret_cast<char*>(&sf), sizeof(sf), readMode);
                  long dummy;
                  file.Read(reinterpret_cast<char*>(&dummy), 4 - sizeof(sf), readMode);
                  sampleFormat = static_cast<SampleFormat>(sf);
               }
               break;
            case (cStripOffsets):
            {
               auto p = file.TellG();
               file.SeekG(toffset);
               if (dataType == cShort)
               {
                  uint16_t soffset;
                  for (unsigned int i = 0; i < dataNum; ++i)
                  {
                     file.Read(reinterpret_cast<char*>(&soffset), sizeof(soffset), readMode);
                     stripOffset.emplace_back(static_cast<uint32_t>(soffset));
                  }
               }
               else if (dataType == cLong)
               {
                  uint32_t loffset;
                  for (unsigned int i = 0; i < dataNum; ++i)
                  {
                     file.Read(reinterpret_cast<char*>(&loffset), sizeof(loffset), readMode);
                     stripOffset.emplace_back(static_cast<uint32_t>(loffset));
                  }
               }
               file.SeekG(p);
            }
            break;
            case (cStripByteCounts):
            {
               auto p = file.TellG();
               file.SeekG(toffset);
               if (dataType == cShort)
               {
                  uint16_t slen;
                  for (unsigned int i = 0; i < dataNum; ++i)
                  {
                     file.Read(reinterpret_cast<char*>(&slen), sizeof(slen), readMode);
                     stripLength.emplace_back(static_cast<uint32_t>(slen));
                  }
               }
               else if (dataType == cLong)
               {
                  uint32_t llen;
                  for (unsigned int i = 0; i < dataNum; ++i)
                  {
                     file.Read(reinterpret_cast<char*>(&llen), sizeof(llen), readMode);
                     stripLength.emplace_back(static_cast<uint32_t>(llen));
                  }
               }
               file.SeekG(p);
            }
            break;
            case (cTileWidth):
               if ((dataType == cShort) && (dataNum == 1))
               {
                  auto           p = file.TellG();
                  unsigned short tw;
                  file.SeekG(-4, std::ios_base::cur);
                  file.Read(reinterpret_cast<char*>(&tw), sizeof(tw), readMode);
                  tileWidth = tw;
                  file.SeekG(p);
               }
               else if ((dataType == cLong) && (dataNum == 1))
               {
                  tileWidth = toffset;
               }
               break;
            case (cTileLength):
               if ((dataType == cShort) && (dataNum == 1))
               {
                  auto           p = file.TellG();
                  unsigned short tl;
                  file.SeekG(-4, std::ios_base::cur);
                  file.Read(reinterpret_cast<char*>(&tl), sizeof(tl), readMode);
                  tileLength = tl;
                  file.SeekG(p);
               }
               else if ((dataType == cLong) && (dataNum == 1))
               {
                  tileLength = toffset;
               }
               break;
            case (cTileOffsets):
            {
               auto p = file.TellG();
               file.SeekG(toffset);
               if (dataType == cShort)
               {
                  uint16_t soffset;
                  for (unsigned int i = 0; i < dataNum; ++i)
                  {
                     file.Read(reinterpret_cast<char*>(&soffset), sizeof(soffset), readMode);
                     tileOffset.emplace_back(static_cast<uint32_t>(soffset));
                  }
               }
               else if (dataType == cLong)
               {
                  uint32_t loffset;
                  for (unsigned int i = 0; i < dataNum; ++i)
                  {
                     file.Read(reinterpret_cast<char*>(&loffset), sizeof(loffset), readMode);
                     tileOffset.emplace_back(static_cast<uint32_t>(loffset));
                  }
               }
               file.SeekG(p);
            }
            break;
            default:
               break;
            }
         }
      }
      if (image == aImageNumber)
      {
         if (valid)
         {
            valid = false;
            switch (sampleFormat) // the following are the formats supported by elevation data in the simulation
            {
            case (cSampleFormat_int):
               if (bytesPerSample == 2)
               {
                  aFormat        = cComponentFormatInt16;
                  imageSizeBytes = 2 * aLatPoints * aLonPoints;
                  aData          = NewArray<int16_t>(aLatPoints * aLonPoints);
                  CleanUp        = [](void* aData) { delete[] static_cast<int16_t*>(aData); };
                  valid          = true;
               }
               break;
            case (cSampleFormat_ieeefp):
               if (bytesPerSample == 4)
               {
                  aFormat        = cComponentFormatFloat;
                  imageSizeBytes = 4 * aLatPoints * aLonPoints;
                  aData          = NewArray<float>(aLatPoints * aLonPoints);
                  CleanUp        = [](void* aData) { delete[] static_cast<float*>(aData); };
                  valid          = true;
               }
               else if (bytesPerSample == 8)
               {
                  aFormat        = cComponentFormatDouble;
                  imageSizeBytes = 8 * aLatPoints * aLonPoints;
                  aData          = NewArray<double>(aLatPoints * aLonPoints);
                  CleanUp        = [](void* aData) { delete[] static_cast<double*>(aData); };
                  valid          = true;
               }
               break;
            default: // only support floating-point and int
               break;
            }
         }
         if (stripOffset.size() != stripLength.size())
         {
            valid = false;
         }
         if (!aData)
         {
            file.Close();
            return 2;
         }
         if (!valid)
         {
            if (imageSizeBytes)
            {
               CleanUp(aData);
            }
            file.Close();
            return 2;
         }
         if (!stripOffset.empty()) // this is a stripped tiff, otherwise we might be tiled
         {
            auto   g        = file.TellG();
            auto   offsetIt = stripOffset.begin();
            auto   lengthIt = stripLength.begin();
            size_t readLen  = bytesPerSample * aLonPoints; // this should minus off the image width overrun
            size_t row      = 0;
            while ((offsetIt != stripOffset.end()) && (lengthIt != stripLength.end()))
            {
               file.SeekG(*offsetIt);
               size_t bytesRead = 0;
               while (bytesRead < *lengthIt)
               {
                  if ((aLatPoints - row) * readLen > imageSizeBytes) // buffer overrun
                  {
                     if (imageSizeBytes)
                     {
                        CleanUp(aData);
                     }
                     file.Close();
                     return 2;
                  }
                  file.Read(&reinterpret_cast<char*>(aData)[(aLatPoints - row - 1) * readLen], readLen, readMode);
                  row++;
                  bytesRead += readLen;
               }
               ++offsetIt;
               ++lengthIt;
            }
            file.SeekG(g);
         }
         else if ((tileWidth > 0) && (tileLength > 0) && !tileOffset.empty())
         {
            std::vector<char> dummy(tileWidth);
            size_t            numTilesX = static_cast<size_t>(ceil(static_cast<float>(aLonPoints) / tileWidth));
            size_t            dataSpan  = tileWidth * bytesPerSample;
            size_t            tileNum   = 0;
            auto              g         = file.TellG();
            for (auto tileLoc : tileOffset)
            {
               size_t tileX = tileNum % numTilesX;
               size_t tileY = tileNum / numTilesX;
               file.SeekG(tileLoc);
               for (size_t i = 0; i < tileLength; ++i)
               {
                  // read a tile width of data into the appropriate location in the buffer
                  if (tileY * tileLength + i < aLatPoints)
                  {
                     size_t readLen =
                        std::min(dataSpan, bytesPerSample * (aLonPoints - tileX * tileWidth)); // this should minus off
                                                                                               // the image width overrun
                     size_t writeLoc =
                        bytesPerSample * (tileX * tileWidth + (aLatPoints - tileY * tileLength - i - 1) * aLonPoints);
                     if (writeLoc + readLen > imageSizeBytes) // buffer overrun
                     {
                        if (imageSizeBytes)
                        {
                           CleanUp(aData);
                        }
                        file.Close();
                        return 2;
                     }
                     file.Read(&reinterpret_cast<char*>(aData)[writeLoc], readLen, readMode);
                     if (readLen < dataSpan)
                     {
                        // read an addition dataSpace - readLen to finish the tile
                        file.Read(dummy.data(), dataSpan - readLen, readMode);
                     }
                  }
               }
               ++tileNum;
            }
            file.SeekG(g);
         }
         else
         {
            if (imageSizeBytes)
            {
               CleanUp(aData);
            }
            file.Close();
            return 2;
         }
         file.Close();
         return 0;
      }
      ++image;
      file.Read(reinterpret_cast<char*>(&offset), sizeof(offset), readMode);
   }
   file.Close();
   return 1;
}


UtTiff::Image& UtTiff::AddImage(const std::string& aAppName,
                                uint32_t           aWidth,
                                uint32_t           aHeight,
                                PixelFormat        aPf,
                                ComponentFormat    aCf,
                                int32_t            aMinValue,
                                int32_t            aMaxValue,
                                void*              aData)
{
   mImageList.emplace_back(aAppName, aWidth, aHeight, aPf, aCf, aMinValue, aMaxValue, aData);
   return mImageList.back();
}

UtTiff::Image::Image(const std::string& aAppName,
                     uint32_t           aWidth,
                     uint32_t           aHeight,
                     PixelFormat        aPf,
                     ComponentFormat    aCf,
                     int32_t            aMinValue,
                     int32_t            aMaxValue,
                     void*              aData)
   : mWidth(aWidth)
   , mHeight(aHeight)
   , mPixelFormat(aPf)
   , mComponentFormat(aCf)
   , mData(aData)
   , mMinValue(aMinValue)
   , mMaxValue(aMaxValue)
   , mAppName(aAppName)
{
}

void UtTiff::Image::SetGeodeticRectangle(double aSW_Lat, double aSW_Lon, double aNE_Lat, double aNE_Lon)
{
   mGeotiff    = true;
   mGeoData[0] = aSW_Lat;
   mGeoData[1] = aSW_Lon;
   mGeoData[2] = aNE_Lat;
   mGeoData[3] = aNE_Lon;
}

void UtTiff::Image::SetLabel(const std::string& aString)
{
   mLabel = aString;
}

void UtTiff::Image::WriteToFile(CompatibleFileStream& aStream, uint32_t& aOffset, bool aLast) const
{
   uint16_t numTags = 15;
   if (mGeotiff)
   {
      numTags += 3;
   }
   if (!mLabel.empty())
   {
      numTags += 1;
   }
   aStream.Write(reinterpret_cast<char*>(&numTags), sizeof(numTags));
   aOffset += sizeof(numTags); // write the num tags (uint16_t)

   std::stringstream buffer;
   uint16_t          bitsPerSample = 8;
   uint16_t          sampleFormat  = 1;
   switch (mComponentFormat)
   {
   case (cComponentFormatDouble):
      bitsPerSample = 64;
      sampleFormat  = 3; // IEEEFP
      break;
   case (cComponentFormatFloat):
      bitsPerSample = 32;
      sampleFormat  = 3; // IEEEFP
      break;
   case (cComponentFormatInt16):
      bitsPerSample = 16;
      sampleFormat  = 2;
      break;
   case (cComponentFormatInt32):
      bitsPerSample = 32;
      sampleFormat  = 2;
      break;
   default:
      break;
   }
   uint16_t samplesPerPixel   = 3;
   uint16_t photometricInterp = 2;
   if (mPixelFormat == cPixelFormatR)
   {
      photometricInterp = 1; // black is zero
      samplesPerPixel   = 1;
   }
   uint32_t imageSize = mHeight * mWidth * samplesPerPixel * (bitsPerSample / 8);

   WriteTagAndBufferData<uint32_t>(aStream, cNewSubfileType, cLong, 0, aOffset, buffer, --numTags);
   WriteTagAndBufferData<uint32_t>(aStream, cImageWidth, cLong, mWidth, aOffset, buffer, --numTags);
   WriteTagAndBufferData<uint32_t>(aStream, cImageHeight, cLong, mHeight, aOffset, buffer, --numTags);
   WriteTagAndBufferData<uint16_t>(aStream, cBitsPerSample, cShort, bitsPerSample, aOffset, buffer, --numTags);
   WriteTagAndBufferData<uint16_t>(aStream, cCompression, cShort, 1, aOffset, buffer, --numTags);
   WriteTagAndBufferData<uint16_t>(aStream, cPhotometricInterpretation, cShort, photometricInterp, aOffset, buffer, --numTags);
   if (!mLabel.empty())
   {
      WriteTagAndBufferString(aStream, cImageDescription, mLabel, aOffset, buffer, --numTags);
   }
   WriteTag(aStream, cStripOffsets, cLong, aOffset, buffer, --numTags);
   buffer.write(reinterpret_cast<const char*>(mData), imageSize);
   WriteTagAndBufferData<uint16_t>(aStream, cSamplesPerPixel, cShort, samplesPerPixel, aOffset, buffer, --numTags);
   WriteTagAndBufferData<uint32_t>(aStream, cRowsPerStrip, cLong, mHeight, aOffset, buffer, --numTags);
   WriteTagAndBufferData<uint32_t>(aStream, cStripByteCounts, cLong, imageSize, aOffset, buffer, --numTags);
   WriteTagAndBufferData<uint16_t>(aStream, cPlanarConfiguration, cShort, 1, aOffset, buffer, --numTags);
   WriteTagAndBufferString(aStream, cSoftware, mAppName, aOffset, buffer, --numTags);
   WriteTagAndBufferData<uint16_t>(aStream, cSampleFormat, cShort, sampleFormat, aOffset, buffer, --numTags);
   // ideally the next two should match the type in mData... I am not sure how to do that, so we'll use whole numbers
   WriteTagAndBufferData<int32_t>(aStream, cSMinSampleValue, cSLong, mMinValue, aOffset, buffer, --numTags);
   WriteTagAndBufferData<int32_t>(aStream, cSMaxSampleValue, cSLong, mMaxValue, aOffset, buffer, --numTags);
   if (mGeotiff)
   {
      double              pperlon    = (mGeoData[3] - mGeoData[1]) / mWidth;
      double              pperlat    = (mGeoData[2] - mGeoData[0]) / mHeight;
      std::vector<double> pixelScale = {pperlon, pperlat, 0.0}; // pixel per lon, pixel per lat, pixel per z
      WriteTagAndBufferVectoredData<double>(aStream, cModelPixelScaleTag, cDouble, pixelScale, aOffset, buffer, --numTags);
      std::vector<double> tiePoint = {0.0, 0.0, 0.0, mGeoData[1], mGeoData[2], 0.0}; // x, y, z, lon, lat, z
      WriteTagAndBufferVectoredData<double>(aStream, cModelTiepointTag, cDouble, tiePoint, aOffset, buffer, --numTags);
      std::vector<uint16_t> geokey = {1,
                                      0,
                                      2,
                                      3, // Header
                                      1024,
                                      0,
                                      1,
                                      2, // ModelTypeGeographic(2)
                                      1025,
                                      0,
                                      1,
                                      1, // RasterPixelIsArea(1)
                                      2048,
                                      0,
                                      1,
                                      4326}; // GeographicType(WGS_84)
      WriteTagAndBufferVectoredData<uint16_t>(aStream, cGeokeyDirectoryTag, cShort, geokey, aOffset, buffer, --numTags);
   }
   uint32_t n = 0;
   auto     p = buffer.tellp();
   if (!aLast)
   {
      n = aOffset + 4 + static_cast<uint32_t>(p);
   }
   aStream.Write(reinterpret_cast<const char*>(&n), sizeof(uint32_t));

   buffer.seekg(0);
   aStream.Write(buffer.str().c_str(), p);
   aOffset = n;
}

template<class T>
void UtTiff::Image::WriteTagAndBufferData(CompatibleFileStream& aStream,
                                          const uint16_t        aTagId,
                                          const uint16_t        aDataType,
                                          T                     aValue,
                                          uint32_t&             aOffset,
                                          std::iostream&        aBuffer,
                                          int                   aRemainingTags) const
{
   WriteTagStart(aStream, aTagId, aDataType, 1, aOffset);
   size_t st = sizeof(T);
   if (st <= 4)
   {
      aStream.Write(reinterpret_cast<const char*>(&aValue), sizeof(T));
      if (st < 4)
      {
         uint32_t zero = 0;
         aStream.Write(reinterpret_cast<const char*>(&zero), 4 - st);
      }
      aOffset += 4;
   }
   else
   {
      uint32_t offset = aOffset + 4 + 4 + 12 * aRemainingTags + static_cast<uint32_t>(aBuffer.tellp());
      aStream.Write(reinterpret_cast<const char*>(&offset), sizeof(uint32_t));
      aOffset += sizeof(uint32_t);
      // note that if we weren't writing in native endian-mode, we would need to flip what is going into the buffer as well
      aBuffer.write(reinterpret_cast<const char*>(&aValue), st);
   }
}

template<class T>
void UtTiff::Image::WriteTagAndBufferVectoredData(CompatibleFileStream& aStream,
                                                  const uint16_t        aTagId,
                                                  const uint16_t        aDataType,
                                                  std::vector<T>        aValues,
                                                  uint32_t&             aOffset,
                                                  std::iostream&        aBuffer,
                                                  int                   aRemainingTags) const
{
   WriteTagStart(aStream, aTagId, aDataType, static_cast<uint32_t>(aValues.size()), aOffset);
   size_t   st     = sizeof(T);
   auto     p      = aBuffer.tellp();
   uint32_t offset = aOffset + 4 + 4 + 12 * aRemainingTags + static_cast<uint32_t>(p);
   aStream.Write(reinterpret_cast<const char*>(&offset), sizeof(uint32_t));
   aOffset += sizeof(uint32_t);
   for (auto&& it : aValues)
   {
      // note that if we weren't writing in native endian-mode, we would need to flip what is going into the buffer as well
      aBuffer.write(reinterpret_cast<const char*>(&it), st);
   }
}

void UtTiff::Image::WriteTagAndBufferString(CompatibleFileStream& aStream,
                                            const uint16_t        aTagId,
                                            std::string           aString,
                                            uint32_t&             aOffset,
                                            std::iostream&        aBuffer,
                                            int                   aRemainingTags) const
{
   WriteTagStart(aStream, aTagId, cASCII, static_cast<uint32_t>(aString.length() + 1), aOffset);
   if (aString.length() < 4)
   {
      aStream.Write(aString.c_str(), aString.length());
      for (size_t i = aString.length(); i < 4; ++i)
      {
         aStream.Write("\0", 1);
      }
      aOffset += 4;
   }
   else
   {
      auto     p      = aBuffer.tellp();
      uint32_t offset = aOffset + 4 + 4 + 12 * aRemainingTags + static_cast<uint32_t>(p);
      aStream.Write(reinterpret_cast<const char*>(&offset), sizeof(uint32_t));
      aOffset += sizeof(uint32_t);
      aBuffer << aString;
      aBuffer << '\0';
   }
}

void UtTiff::Image::WriteTag(CompatibleFileStream& aStream,
                             const uint16_t        aTagId,
                             const uint16_t        aDataType,
                             uint32_t&             aOffset,
                             std::iostream&        aBuffer,
                             int                   aRemainingTags) const
{
   WriteTagStart(aStream, aTagId, aDataType, 1, aOffset);
   auto     p      = aBuffer.tellp();
   uint32_t offset = aOffset + 4 + 4 + 12 * aRemainingTags + static_cast<uint32_t>(p);
   aStream.Write(reinterpret_cast<const char*>(&offset), sizeof(uint32_t));
   aOffset += sizeof(uint32_t);
}

void UtTiff::Image::WriteTagStart(CompatibleFileStream& aStream,
                                  const uint16_t        aTagId,
                                  const uint16_t        aDataType,
                                  const uint32_t        aDataCount,
                                  uint32_t&             aOffset) const
{
   aStream.Write(reinterpret_cast<const char*>(&aTagId), sizeof(uint16_t));
   aOffset += sizeof(uint16_t);
   aStream.Write(reinterpret_cast<const char*>(&aDataType), sizeof(uint16_t));
   aOffset += sizeof(uint16_t);
   aStream.Write(reinterpret_cast<const char*>(&aDataCount), sizeof(uint32_t));
   aOffset += sizeof(uint32_t);
}

std::string UtTiff::ReadTagString(CompatibleFileStream& aStream, const uint32_t aOffset, const uint32_t aLength)
{
   std::string str;
   if (aLength <= 4) // read it directly from value
   {
      const char* cstr = reinterpret_cast<const char*>(&aOffset);
      for (size_t l = 0; l < 4; ++l)
      {
         str += cstr[l];
         if (cstr[l] == '\0')
         {
            break;
         }
      }
   }
   else // read from address
   {
      auto p = aStream.TellG();
      aStream.SeekG(aOffset);
      std::vector<char> cstr(aLength);
      aStream.Read(cstr.data(), aLength, CompatibleFileStream::cNATIVE);
      str.assign(cstr.begin(), cstr.end());
      aStream.SeekG(p);
   }
   str.erase(std::find(str.begin(), str.end(), '\0'),
             str.end()); // remove everything from the first explicit '\0' in the string
   return str;
}

UtTiff::CompatibleFileStream::CompatibleFileStream(const std::string aFilename, std::ios_base::openmode aMode)
   : std::fstream(aFilename, aMode | std::fstream::binary)
{
   size_t      one    = 0x01;
   const char* numPtr = reinterpret_cast<const char*>(&one);
   mNativeMode        = numPtr[0] == 1 ? cLITTLE_ENDIAN : cBIG_ENDIAN;
}

void UtTiff::CompatibleFileStream::Read(char* aIn, std::streamsize aSize, Mode aReadMode)
{
   if ((aReadMode == cNATIVE) || (aReadMode == mNativeMode))
   {
      read(aIn, aSize);
   }
   else
   {
      std::vector<char> temp(ut::safe_cast<size_t>(aSize));
      read(temp.data(), aSize);
      std::reverse_copy(temp.begin(), temp.end(), aIn);
   }
}

void UtTiff::CompatibleFileStream::Write(const char* aOut, std::streamsize aSize, Mode aWriteMode)
{
   if ((aWriteMode == cNATIVE) || (aWriteMode == mNativeMode))
   {
      write(aOut, aSize);
   }
   else
   {
      std::vector<char> temp(ut::safe_cast<size_t>(aSize));
      std::reverse_copy(aOut, aOut + aSize, temp.begin());
      write(temp.data(), aSize);
   }
}

void UtTiff::CompatibleFileStream::SeekG(std::streampos aOffset, std::ios_base::seekdir aWay)
{
   seekg(aOffset, aWay);
}

void UtTiff::CompatibleFileStream::SeekG(std::streampos aPos)
{
   seekg(aPos);
}
