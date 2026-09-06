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

// this class allows software to write to a tiff file, or to preview a tiff header for
// tags of interest to AFSIM applications.
// Reference: TIFF File Format (https://www.fileformat.info/format/tiff/egff.htm)

#ifndef UTTIFF_HPP
#define UTTIFF_HPP

#include "ut_export.h"

#include <cstdint>
#include <fstream>
#include <map>
#include <string>
#include <vector>

class UT_EXPORT UtTiff
{
public:
   struct ImagePreview
   {
      bool        mIsGeotiff{false};
      bool        mIsEquirectangular{false};
      bool        mIsSensorPlot{false};
      bool        mIsAFSIM_Usable{false};
      short       mCompression{0};
      int32_t     mMinValue{0};
      int32_t     mMaxValue{255};
      int32_t     mImageWidth{0};
      int32_t     mImageHeight{0};
      std::string mDescription;
      double      mLatMin{0.0};
      double      mLatMax{0.0};
      double      mLonMin{0.0};
      double      mLonMax{0.0};
   };
   using PreviewList = std::vector<ImagePreview>;

   void               WriteToFile(const std::string& aFilename) const;
   static bool        IsTiffFile(const std::string& aFilename);
   static PreviewList PreviewHeaderInformation(const std::string& aFilename);

   enum PixelFormat
   {
      cPixelFormatR,
      cPixelFormatRGB
   };
   enum ComponentFormat
   {
      cComponentFormatByte,
      cComponentFormatInt16,
      cComponentFormatInt32,
      cComponentFormatFloat,
      cComponentFormatDouble
   };

   // returns an int, 0 is ok, 1 is unopenable, 2 is error reading
   static int ReadElevationData(const std::string& aFilename,
                                unsigned int       aImageNumber,
                                ComponentFormat&   aFormat,
                                void*&             aData,
                                unsigned int&      aLatPoints,
                                unsigned int&      aLonPoints);

   enum TagId : uint16_t
   {
      cNewSubfileType            = 254,   // LONG
      cImageWidth                = 256,   // SHORT OR LONG
      cImageHeight               = 257,   // SHORT OR LONG
      cBitsPerSample             = 258,   // SHORT
      cCompression               = 259,   // SHORT
      cPhotometricInterpretation = 262,   // SHORT
      cImageDescription          = 270,   // ASCII
      cStripOffsets              = 273,   // SHORT OR LONG
      cOrientation               = 274,   // SHORT
      cSamplesPerPixel           = 277,   // SHORT
      cRowsPerStrip              = 278,   // SHORT OR LONG
      cStripByteCounts           = 279,   // LONG OR SHORT
      cXResolution               = 282,   // RATIONAL
      cYResolution               = 283,   // RATIONAL
      cPlanarConfiguration       = 284,   // SHORT
      cResolutionUnit            = 296,   // SHORT
      cSoftware                  = 305,   // ASCII
      cTileWidth                 = 322,   // SHORT OR LONG
      cTileLength                = 323,   // SHORT OR LONG
      cTileOffsets               = 324,   // LONG
      cTileByteCounts            = 325,   // SHORT OR LONG
      cSampleFormat              = 339,   // SHORT
      cSMinSampleValue           = 340,   // ANY
      cSMaxSampleValue           = 341,   // ANY
      cModelPixelScaleTag        = 33550, // DOUBLE
      cModelTiepointTag          = 33922, // DOUBLE
      cModelTransformationTag    = 34264, // DOUBLE
      cGeokeyDirectoryTag        = 34735, // SHORT
      cGeoDoubleParamsTag        = 34736, // DOUBLE
      cGeoAsciiParamsTag         = 34737  // ASCII
   };
   enum DataType : uint16_t
   {
      cByte      = 1,
      cASCII     = 2,
      cShort     = 3,
      cLong      = 4,
      cRational  = 5,
      cSByte     = 6,
      cUndefine  = 7,
      cSShort    = 8,
      cSLong     = 9,
      cSRational = 10,
      cFloat     = 11,
      cDouble    = 12
   };
   enum SampleFormat : uint16_t
   {
      cSampleFormat_undefined     = 0,
      cSampleFormat_uint          = 1,
      cSampleFormat_int           = 2,
      cSampleFormat_ieeefp        = 3,
      cSampleFormat_void          = 4,
      cSampleForamt_complexint    = 5,
      cSampleFormat_complexieeefp = 6
   };
   enum Compression : uint16_t
   {
      cDefault       = 0, // TiffBitmapEncoder
      cNone          = 1,
      cCCITTRLE      = 2,
      cCCITTFAX3     = 3,
      cCCITTFAX4     = 4,
      cLZW           = 5,
      cOJPEG         = 6,
      cJPEG          = 7,
      cADOBE_DEFLATE = 8
   };

   static constexpr const char* cAPPLICATION_NAME = "AFSIM_UtTiff";

private:
   class CompatibleFileStream;

public:
   class UT_EXPORT Image
   {
   public:
      // note that this will not own your data memory
      Image(const std::string& aAppName,
            uint32_t           aWidth,
            uint32_t           aHeight,
            PixelFormat        aPf,
            ComponentFormat    aCf,
            int32_t            aMinValue,
            int32_t            aMaxValue,
            void*              aData);
      void SetGeodeticRectangle(double aSW_Lat, double aSW_Lon, double aNE_Lat, double aNE_Lon);
      void SetLabel(const std::string& aString);

      void WriteToFile(CompatibleFileStream& aStream, uint32_t& aOffset, bool aLast) const;

   private:
      template<class T>
      void WriteTagAndBufferData(CompatibleFileStream& aStream,
                                 const uint16_t        aTagId,
                                 const uint16_t        aDataType,
                                 T                     aValue,
                                 uint32_t&             aOffset,
                                 std::iostream&        aBuffer,
                                 int                   aRemainingTags) const;
      template<class T>
      void WriteTagAndBufferVectoredData(CompatibleFileStream& aStream,
                                         const uint16_t        aTagId,
                                         const uint16_t        aDataType,
                                         std::vector<T>        aValues,
                                         uint32_t&             aOffset,
                                         std::iostream&        aBuffer,
                                         int                   aRemainingTags) const;
      void WriteTagAndBufferString(CompatibleFileStream& aStream,
                                   const uint16_t        aTagId,
                                   std::string           aString,
                                   uint32_t&             aOffset,
                                   std::iostream&        aBuffer,
                                   int                   aRemainingTags) const;
      // we'll populate the buffer ourselves with this one...
      void WriteTag(CompatibleFileStream& aStream,
                    const uint16_t        aTagId,
                    const uint16_t        aDataType,
                    uint32_t&             aOffset,
                    std::iostream&        aBuffer,
                    int                   aRemainingTags) const;
      void WriteTagStart(CompatibleFileStream& aStream,
                         const uint16_t        aTagId,
                         const uint16_t        aDataType,
                         const uint32_t        aDataCount,
                         uint32_t&             aOffset) const;

      uint32_t        mWidth{0};
      uint32_t        mHeight{0};
      PixelFormat     mPixelFormat{cPixelFormatRGB};
      ComponentFormat mComponentFormat{cComponentFormatByte};
      void*           mData{nullptr};
      bool            mGeotiff{false};
      double          mGeoData[4]{0, 0, 0, 0};
      std::string     mLabel{""};
      int32_t         mMinValue{0};
      int32_t         mMaxValue{0};
      std::string     mAppName{cAPPLICATION_NAME};
   };

   Image& AddImage(const std::string& aAppName,
                   uint32_t           aWidth,
                   uint32_t           aHeight,
                   PixelFormat        aPf,
                   ComponentFormat    aCf,
                   int32_t            aMinValue,
                   int32_t            aMaxValue,
                   void*              aData);

private:
   // CompatibleFileStream is a wrapper around fstream that presents an interface for dealing with
   // little-endian/big-endian issues Note that this class also exists within GeoData's GeoShapeFile class.  There
   // wasn't a clean and sensible way to dissect this and given the small amount of code in question it made more sense
   // to keep both.
   class CompatibleFileStream : private std::fstream
   {
   public:
      enum Mode
      {
         cLITTLE_ENDIAN = 0,
         cBIG_ENDIAN    = 1,
         cNATIVE        = 2
      };
      CompatibleFileStream(const std::string aFilename, std::ios_base::openmode aMode);
      ~CompatibleFileStream() override = default;

      bool           IsNativeLittleEndian() const { return mNativeMode == cLITTLE_ENDIAN; }
      bool           Good() const { return good(); }
      bool           EndOfFile() const { return eof(); }
      void           Read(char* aIn, std::streamsize aSize, Mode aReadMode);
      void           Write(const char* aOut, std::streamsize aSize, Mode aWriteMode = cNATIVE);
      void           Close() { close(); }
      std::streampos TellG() { return tellg(); }
      std::streampos TellP() { return tellp(); }
      void           SeekG(std::streampos aPos);
      void           SeekG(std::streampos aOffset, std::ios_base::seekdir aWay);

   private:
      Mode mNativeMode;
   };

   static std::string ReadTagString(CompatibleFileStream& aStream, const uint32_t aOffset, const uint32_t aLength);
   std::vector<Image> mImageList;
};

#endif
