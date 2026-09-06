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

#include "UtColor.hpp"

#include <iomanip>

#include "UtLexicalCast.hpp"
#include "UtStringUtil.hpp"

namespace
{
// NOTE: If color mappings here are modified, the actions for the
// corresponding named colors in wsf grammar will need to be updated.
const std::unordered_map<std::string, UtColor> colorMappings = {
   {"black", {0, 0, 0, 1}},
   {"blue", {0, 168.0F / 255.0F, 220.0F / 255.0F, 1}},
   {"brown", {61.0F / 255.0F, 33.0F / 255.0F, 0, 1}},
   {"dark_blue", {0, 107.0F / 255.0F, 140.0F / 255.0F, 1}},
   {"dark_green", {0, 160.0F / 255.0F, 0, 1}},
   {"dark_purple", {80.0F / 255.0F, 0, 80.0F / 255.0F, 1}},
   {"dark_red", {200.0F / 255.0F, 0, 0, 1}},
   {"dark_yellow", {225.0F / 255.0F, 220.0F / 255.0F, 0, 1}},
   {"gray", {102.0F / 255.0F, 102.0F / 255.0F, 102.0F / 255.0F, 1}},
   {"grey", {102.0F / 255.0F, 102.0F / 255.0F, 102.0F / 255.0F, 1}},
   {"green", {0, 226.0F / 255.0F, 0, 1}},
   {"indigo", {74.0F / 255.0F, 0, 159.0F / 255.0F, 1}},
   {"light_blue", {128.0F / 255.0F, 224.0F / 255.0F, 1, 1}},
   {"light_green", {170.0F / 255.0F, 1, 170.0F / 255.0F, 1}},
   {"light_purple", {1, 161.0F / 255.0F, 1, 1}},
   {"light_red", {1, 128.0F / 255.0F, 128.0F / 255.0F, 1}},
   {"light_yellow", {1, 1, 128.0F / 255.0F, 1}},
   {"magenta", {1, 0, 1, 1}},
   {"orange", {1, 170.0F / 255.0F, 0, 1}},
   {"pink", {1, 0, 192.0F / 255.0F, 1}},
   {"purple", {128.0F / 255.0F, 0, 128.0F / 255.0F, 1}},
   {"red", {1, 48.0F / 255.0F, 49.0F / 255.0F, 1}},
   {"tan", {182.0F / 255.0F, 133.0F / 255.0F, 56.0F / 255.0F, 1}},
   {"violet", {192.0F / 255.0F, 128.0F / 255.0F, 1, 1}},
   {"white", {1, 1, 1, 1}},
   {"yellow", {1, 1, 0, 1}},
};
} // namespace

int UtColor::mDefaultFormat = UtColor::FmtFloatAlpha;

const UtColor& UtColor::GetColorMapping(const std::string& aColorName)
{
   auto colorIt = colorMappings.find(aColorName);
   return (colorIt != colorMappings.end()) ? colorIt->second : colorMappings.at("grey");
}

const std::unordered_map<std::string, UtColor>& UtColor::GetColorMap()
{
   return colorMappings;
}

bool UtColor::ColorExists(const std::string& aColorName)
{
   return colorMappings.find(aColorName) != colorMappings.end();
}

bool UtColor::operator<(const UtColor& aRhs) const
{
   return std::lexicographical_compare(mRGBA, mRGBA + 4, aRhs.mRGBA, aRhs.mRGBA + 4);
}

bool UtColor::operator!=(const UtColor& aRhs) const
{
   return !std::equal(mRGBA, mRGBA + 4, aRhs.mRGBA);
}

bool UtColor::operator==(const UtColor& aRhs) const
{
   return std::equal(mRGBA, mRGBA + 4, aRhs.mRGBA);
}

void UtColor::RGBAToHexString(const float aRGBA[4], std::string& aHexString)
{
   aHexString.resize(8);
   unsigned char comp[4] = {(unsigned char)(aRGBA[0] * 255),
                            (unsigned char)(aRGBA[1] * 255),
                            (unsigned char)(aRGBA[2] * 255),
                            (unsigned char)(aRGBA[3] * 255)};

   for (size_t i = 0; i < 4; ++i)
   {
      unsigned char high    = comp[i] >> 4;
      unsigned char low     = comp[i] & 0x0F;
      aHexString[i * 2]     = high <= 9 ? high + '0' : high - 10 + 'A';
      aHexString[i * 2 + 1] = low <= 9 ? low + '0' : low - 10 + 'A';
   }
}

void UtColor::HexStringToRGBA(const std::string& aHexString, float aRGBA[4])
{
   for (size_t i = 0; i < aHexString.size(); ++i)
   {
      char   v          = aHexString.c_str()[i];
      size_t index      = i / 2;
      float  multiplier = (i % 2) != 0u ? 1.0F : 16.0F;

      if (index < 4)
      {
         if ((v >= '0') && (v <= '9'))
         {
            aRGBA[index] += (v - '0') * multiplier / 255.0F;
         }
         else if ((v >= 'a') && (v <= 'f'))
         {
            aRGBA[index] += (v + 10 - 'a') * multiplier / 255.0F;
         }
         else if ((v >= 'A') && (v <= 'F'))
         {
            aRGBA[index] += (v + 10 - 'A') * multiplier / 255.0F;
         }
      }
   }
   if (aHexString.size() < 8)
   {
      aRGBA[3] = 1.0F;
   }
}

bool UtColor::IsHexColor(const std::string& aHexString)
{
   bool isHexColor = false;
   if ((aHexString.length() == 8) || (aHexString.length() == 6))
   {
      constexpr char hexValues[22] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A',
                                      'B', 'C', 'D', 'E', 'F', 'a', 'b', 'c', 'd', 'e', 'f'};

      isHexColor = true;
      for (char ch : aHexString)
      {
         if (std::find(std::begin(hexValues), std::end(hexValues), ch) == std::end(hexValues))
         {
            isHexColor = false;
            break;
         }
      }
   }
   return isHexColor;
}

void UtColor::SetDefaultFormat(int aFormat)
{
   mDefaultFormat = aFormat;
}

UT_EXPORT std::ostream& operator<<(std::ostream& aOut, const UtColor& aColor)
{
   int format = aColor.GetFormat();
   if (format == 0)
   {
      format = UtColor::GetDefaultFormat();
   }

   if ((format & UtColor::FmtFloat) != 0)
   {
      aOut << aColor.mRGBA[0];
      aOut << ' ' << aColor.mRGBA[1];
      aOut << ' ' << aColor.mRGBA[2];
      if ((format & UtColor::FmtAlpha) != 0)
      {
         aOut << ' ' << aColor.mRGBA[3];
      }
   }
   else if ((format & UtColor::FmtUChar) != 0)
   {
      aOut << std::fixed << std::setprecision(0);
      aOut << aColor.mRGBA[0] * 255;
      aOut << ' ' << aColor.mRGBA[1] * 255;
      aOut << ' ' << aColor.mRGBA[2] * 255;
      if ((format & UtColor::FmtAlpha) != 0)
      {
         aOut << ' ' << aColor.mRGBA[3] * 255;
      }
   }
   else if ((format & UtColor::FmtHex) != 0)
   {
      std::string hexString;
      aColor.Get(hexString);
      aOut << hexString;
   }
   return aOut;
}

//! Provides the ability to stream in the color's data.
UT_EXPORT std::istream& operator>>(std::istream& aIn, UtColor& aColor)
{
   std::string val;
   aIn >> val;
   UtStringUtil::ToLower(val);

   if (UtColor::IsHexColor(val))
   {
      aColor.Set(val);
   }
   else if (UtColor::ColorExists(val))
   {
      aColor = UtColor::GetColorMapping(val);
   }
   else if (val == "float")
   {
      aColor.mFormat = UtColor::FmtFloat;
      aIn >> aColor.mRGBA[0];
      aIn >> aColor.mRGBA[1];
      aIn >> aColor.mRGBA[2];
      aColor.mRGBA[3] = 1.F;

      if (!aIn.eof())
      {
         aIn >> aColor.mRGBA[3];
         aColor.mFormat |= UtColor::FmtAlpha;
      }
   }
   else
   {
      aColor.mFormat = UtColor::FmtUChar;
      unsigned rgba[4];
      try
      {
         rgba[0] = ut::lexical_cast<unsigned>(val);
      }
      catch (ut::bad_lexical_cast&)
      {
         aIn.setstate(std::ios::badbit);
      }
      aIn >> rgba[1];
      aIn >> rgba[2];
      rgba[3] = 255;

      if (!aIn.eof())
      {
         aIn >> rgba[3];
         aColor.mFormat |= UtColor::FmtAlpha;
      }

      if (aIn)
      {
         std::transform(std::begin(rgba), std::end(rgba), std::begin(aColor.mRGBA), [](unsigned c) { return c / 255.0F; });
      }
   }

   // Verify all values are in range [0, 1]
   if (std::any_of(std::begin(aColor.mRGBA), std::end(aColor.mRGBA), [](float val) { return (val < 0) || (val > 1); }))
   {
      aIn.setstate(std::ios::failbit);
   }
   return aIn;
}
