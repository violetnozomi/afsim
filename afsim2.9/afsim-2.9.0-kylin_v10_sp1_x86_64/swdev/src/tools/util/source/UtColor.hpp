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

#ifndef UTCOLOR_HPP
#define UTCOLOR_HPP

#include "ut_export.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>

//---------------------------------------------------------------------------------
//! UtColor represents a color in the form of a red/green/blue triple along
//! with an alpha channel. By default the alpha channel is set to full intensity
//! with a value of 1. UtColor provides the bracket "[]" operator for easy
//! read/write access to each of the color components. There are also various
//! Set/Get methods defined.
//---------------------------------------------------------------------------------

class UT_EXPORT UtColor
{
public:
   enum
   {
      FmtFloat = 0x01, // Floating point values [0, 1]
      FmtUChar = 0x02, // Unsigned char values  [0, 255]
      FmtHex   = 0x04, // Hex string [00, FF]
      FmtAlpha = 0x08, // Alpha component included (RGBA)

      FmtFloatAlpha = FmtFloat | FmtAlpha, // Convenience enums
      FmtUCharAlpha = FmtUChar | FmtAlpha,
      FmtHexAlpha   = FmtHex | FmtAlpha
   };

   // Constructors.

   UtColor() = default;

   UtColor(float aR, float aG, float aB, float aA)
   {
      mRGBA[0] = aR;
      mRGBA[1] = aG;
      mRGBA[2] = aB;
      mRGBA[3] = aA;
      mFormat  = FmtFloatAlpha;
   }

   UtColor(float aR, float aG, float aB)
      : UtColor(aR, aG, aB, 1.F)
   {
      mFormat = FmtFloat;
   }

   UtColor(const float aRGB[3], float aA = 1.F)
      : UtColor(aRGB[0], aRGB[1], aRGB[2], aA)
   {
   }

   UtColor(const std::string& aHexString)
   {
      if (!IsHexColor(aHexString))
      {
         throw std::invalid_argument("Invalid hex string provided to UtColor");
      }
      mRGBA[0] = 0;
      mRGBA[1] = 0;
      mRGBA[2] = 0;
      mRGBA[3] = 0;
      mFormat  = FmtHex;
      if (aHexString.length() == 8)
      {
         mFormat |= FmtAlpha;
      }
      HexStringToRGBA(aHexString, mRGBA);
   }

   // Mutators.

   void SetFormat(int aFormat) { mFormat = aFormat; }

   static void SetDefaultFormat(int aFormat);

   UtColor& Set(float aR, float aG, float aB, float aA = 1.F)
   {
      mRGBA[0] = aR;
      mRGBA[1] = aG;
      mRGBA[2] = aB;
      mRGBA[3] = aA;
      return *this;
   }

   UtColor& Set(const float aRGB[3], float aA = 1.F)
   {
      mRGBA[0] = aRGB[0];
      mRGBA[1] = aRGB[1];
      mRGBA[2] = aRGB[2];
      mRGBA[3] = aA;
      return *this;
   }

   UtColor& Set(const UtColor& aColor)
   {
      mRGBA[0] = aColor.mRGBA[0];
      mRGBA[1] = aColor.mRGBA[1];
      mRGBA[2] = aColor.mRGBA[2];
      mRGBA[3] = aColor.mRGBA[3];
      return *this;
   }

   UtColor& Set(const std::string& aHexString)
   {
      if (!IsHexColor(aHexString))
      {
         throw std::invalid_argument("Invalid hex string provided to UtColor");
      }
      mRGBA[0] = 0;
      mRGBA[1] = 0;
      mRGBA[2] = 0;
      mRGBA[3] = 0;
      mFormat  = FmtHex;
      if (aHexString.length() == 8)
      {
         mFormat |= FmtAlpha;
      }
      else
      {
         mFormat &= ~FmtAlpha;
      }
      HexStringToRGBA(aHexString, mRGBA);
      return *this;
   }

   UtColor& Set(unsigned char aR, unsigned char aG, unsigned char aB, unsigned char aA)
   {
      mRGBA[0] = aR / 255.0F;
      mRGBA[1] = aG / 255.0F;
      mRGBA[2] = aB / 255.0F;
      mRGBA[3] = aA / 255.0F;
      mFormat  = FmtUCharAlpha;
      return *this;
   }

   UtColor& Set(unsigned char aR, unsigned char aG, unsigned char aB)
   {
      Set(aR, aG, aB, 255);
      mFormat = FmtUChar;
      return *this;
   }

   // Accessors.

   int GetFormat() const { return mFormat; }

   static int GetDefaultFormat() { return mDefaultFormat; }

   void Get(float& aR, float& aG, float& aB) const
   {
      aR = mRGBA[0];
      aG = mRGBA[1];
      aB = mRGBA[2];
   }

   void Get(float& aR, float& aG, float& aB, float& aA) const
   {
      aR = mRGBA[0];
      aG = mRGBA[1];
      aB = mRGBA[2];
      aA = mRGBA[3];
   }

   void Get(float aRGBA[4]) const
   {
      aRGBA[0] = mRGBA[0];
      aRGBA[1] = mRGBA[1];
      aRGBA[2] = mRGBA[2];
      aRGBA[3] = mRGBA[3];
   }

   void Get(float aRGB[3], float& aA) const
   {
      aRGB[0] = mRGBA[0];
      aRGB[1] = mRGBA[1];
      aRGB[2] = mRGBA[2];
      aA      = mRGBA[3];
   }

   void Get(UtColor& aColor) const
   {
      aColor[0] = mRGBA[0];
      aColor[1] = mRGBA[1];
      aColor[2] = mRGBA[2];
      aColor[3] = mRGBA[3];
   }

   // Returns an integer with one byte for red, green, blue, and alpha in increasing bit order
   unsigned int GetRGBA32() const
   {
      unsigned int rgbaBytes[] = {(unsigned int)(unsigned char)(mRGBA[0] * 255),
                                  (unsigned int)(unsigned char)(mRGBA[1] * 255),
                                  (unsigned int)(unsigned char)(mRGBA[2] * 255),
                                  (unsigned int)(unsigned char)(mRGBA[3] * 255)};
      return rgbaBytes[0] | (rgbaBytes[1] << 8) | (rgbaBytes[2] << 16) | (rgbaBytes[3] << 24);
   }
   void Get(std::string& aHexString) const
   {
      RGBAToHexString(mRGBA, aHexString);
      if ((mFormat & FmtAlpha) == 0)
      {
         aHexString.resize(6);
      }
   }

   static bool IsHexColor(const std::string& aHexString);

   static void HexStringToRGBA(const std::string& aHexString, float aRGBA[4]);


   static void RGBAToHexString(const float aRGBA[4], std::string& aHexString);

   //! Returns a pointer to the internal, data which is useful for
   //! methods that use arrays (example: glColor4fv).
   const float* GetData() const { return mRGBA; }

   //! Provides read access to the color's components.
   const float& operator[](unsigned int aIndex) const { return mRGBA[aIndex]; }

   //! Provides read access to the color's components.
   const float& operator[](int aIndex) const { return mRGBA[aIndex]; }


   //! Provides read/write access to the color's components.
   float& operator[](unsigned int aIndex) { return mRGBA[aIndex]; }

   //! Provides read/write access to the color's components.
   float& operator[](int aIndex) { return mRGBA[aIndex]; }

   static const UtColor&                                  GetColorMapping(const std::string& aColorName);
   static const std::unordered_map<std::string, UtColor>& GetColorMap();
   static bool                                            ColorExists(const std::string& aColorName);

   friend UT_EXPORT std::ostream& operator<<(std::ostream& aOut, const UtColor& aColor);
   friend UT_EXPORT std::istream& operator>>(std::istream& aIn, UtColor& aColor);

   bool operator==(const UtColor& aRhs) const;
   bool operator!=(const UtColor& aRhs) const;
   bool operator<(const UtColor& aRhs) const;

private:
   static int mDefaultFormat;

   float mRGBA[4]{0.F, 0.F, 0.F, 1.F};
   int   mFormat{0};
};

#endif
