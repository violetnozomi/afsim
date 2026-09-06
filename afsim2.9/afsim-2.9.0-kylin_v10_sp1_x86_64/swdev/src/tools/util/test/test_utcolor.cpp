// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <gtest/gtest.h>

#include "UtColor.hpp"
#include "UtInput.hpp"

TEST(UtColor, StreamExtraction)
{
   UtColor expectedColor{0.2F, 0.4F, 0.6F, 1.0F};
   // Hex string RGBA
   {
      std::istringstream iss{"336699FF"};
      UtColor            hex;
      iss >> hex;
      EXPECT_EQ(hex.GetFormat(), UtColor::FmtHexAlpha);
      EXPECT_EQ(hex, expectedColor);
   }

   // Hex string RGB
   {
      std::istringstream iss{"336699"};
      UtColor            hex;
      iss >> hex;
      EXPECT_EQ(hex.GetFormat(), UtColor::FmtHex);
      EXPECT_EQ(hex, expectedColor);
   }

   // Floating point RGBA
   {
      std::istringstream iss{"float 0.2 0.4 0.6 1.0"};
      UtColor            rgba;
      iss >> rgba;
      EXPECT_EQ(rgba.GetFormat(), UtColor::FmtFloatAlpha);
      EXPECT_EQ(rgba, expectedColor);
   }

   // Floating point RGB
   {
      std::istringstream iss{"float 0.2 0.4 0.6"};
      UtColor            rgb;
      iss >> rgb;
      EXPECT_EQ(rgb.GetFormat(), UtColor::FmtFloat);
      EXPECT_EQ(rgb, expectedColor);
   }

   // Unsigned char RGBA
   {
      std::istringstream iss{"51 102 153 255"};
      UtColor            rgba;
      iss >> rgba;
      EXPECT_EQ(rgba.GetFormat(), UtColor::FmtUCharAlpha);
      EXPECT_EQ(rgba, expectedColor);
   }

   // Unsigned char RGB
   {
      std::istringstream iss{"51 102 153"};
      UtColor            rgb;
      iss >> rgb;
      EXPECT_EQ(rgb.GetFormat(), UtColor::FmtUChar);
      EXPECT_EQ(rgb, expectedColor);
   }

   // Named color
   {
      std::istringstream iss{"white"};
      UtColor            namedColor;
      iss >> namedColor;
      UtColor white{1.F, 1.F, 1.F};
      EXPECT_EQ(namedColor.GetFormat(), UtColor::FmtFloatAlpha);
      EXPECT_EQ(namedColor, white);
   }
}

TEST(UtColor, StreamExtractionInvalid)
{
   // Invalid hex string or named color
   {
      std::istringstream iss{"FFFF"};
      UtColor            color;
      EXPECT_FALSE(iss >> color);
   }

   // Too few floating point values
   {
      std::istringstream iss{"float 0.1 0.2"};
      UtColor            color;
      EXPECT_FALSE(iss >> color);
   }

   // Floating point value out of range (high)
   {
      std::istringstream iss{"float 1.0 0.1 1.1"};
      UtColor            color;
      EXPECT_FALSE(iss >> color);
   }

   // Floating point value out of range (low)
   {
      std::istringstream iss{"float 1.0 0.1 1.0 -0.1"};
      UtColor            color;
      EXPECT_FALSE(iss >> color);
   }

   // Too few integral values
   {
      std::istringstream iss{"127 255"};
      UtColor            color;
      EXPECT_FALSE(iss >> color);
   }

   // Integral value out of range (high)
   {
      std::istringstream iss{"127 255 255 300"};
      UtColor            color;
      EXPECT_FALSE(iss >> color);
   }

   // Integral value out of range (low)
   {
      std::istringstream iss{"-127 255 255 300"};
      UtColor            color;
      EXPECT_FALSE(iss >> color);
   }
}

TEST(UtColor, StreamInsertion)
{
   std::ostringstream oss;
   oss << std::fixed << std::setprecision(1);

   // Default constructed color. Format should be floating point with alpha.
   {
      UtColor defaultColor;
      oss << defaultColor;
      EXPECT_EQ(oss.str(), "0.0 0.0 0.0 1.0");
   }

   // Hex string RGBA
   {
      UtColor hex{"336699FF"};
      oss.str("");
      oss << hex;
      EXPECT_EQ(oss.str(), "336699FF");
   }

   // Hex string RGB
   {
      UtColor hex{"336699"};
      oss.str("");
      oss << hex;
      EXPECT_EQ(oss.str(), "336699");
   }

   // Floating point RGBA
   {
      UtColor rgba{0.2F, 0.4F, 0.6F, 1.0F};
      oss.str("");
      oss << rgba;
      EXPECT_EQ(oss.str(), "0.2 0.4 0.6 1.0");
   }

   // Floating point RGB
   {
      UtColor rgb{0.2F, 0.4F, 0.6F};
      oss.str("");
      oss << rgb;
      EXPECT_EQ(oss.str(), "0.2 0.4 0.6");
   }

   // Unsigned char RGBA
   {
      UtColor rgba;
      rgba.Set((unsigned char)51, 102, 153, 255);
      oss.str("");
      oss << rgba;
      EXPECT_EQ(oss.str(), "51 102 153 255");
   }

   // Unsigned char RGB
   {
      UtColor rgb;
      rgb.Set((unsigned char)51, 102, 153);
      oss.str("");
      oss << rgb;
      EXPECT_EQ(oss.str(), "51 102 153");
   }
}

TEST(UtColor, ReadFromUtInput)
{
   UtColor expectedColor{0.2F, 0.4F, 0.6F, 1.0F};
   // Hex string RGBA
   {
      UtInput input;
      input.PushInputString("336699FF");
      UtColor hex;
      input.ReadValue(hex);
      EXPECT_EQ(hex.GetFormat(), UtColor::FmtHexAlpha);
      EXPECT_EQ(hex, expectedColor);
   }

   // Hex string RGB
   {
      UtInput input;
      input.PushInputString("336699");
      UtColor hex;
      input.ReadValue(hex);
      EXPECT_EQ(hex.GetFormat(), UtColor::FmtHex);
      EXPECT_EQ(hex, expectedColor);
   }

   // Floating point RGBA
   {
      UtInput input;
      input.PushInputString("float 0.2 0.4 0.6 1.0");
      UtColor rgba;
      input.ReadValue(rgba);
      EXPECT_EQ(rgba.GetFormat(), UtColor::FmtFloatAlpha);
      EXPECT_EQ(rgba, expectedColor);
   }

   // Floating point RGB
   {
      UtInput input;
      input.PushInputString("float 0.2 0.4 0.6");
      UtColor rgb;
      input.ReadValue(rgb);
      EXPECT_EQ(rgb.GetFormat(), UtColor::FmtFloat);
      EXPECT_EQ(rgb, expectedColor);
   }

   // Unsigned char RGBA
   {
      UtInput input;
      input.PushInputString("51 102 153 255");
      UtColor rgba;
      input.ReadValue(rgba);
      EXPECT_EQ(rgba.GetFormat(), UtColor::FmtUCharAlpha);
      EXPECT_EQ(rgba, expectedColor);
   }

   // Unsigned char RGB
   {
      UtInput input;
      input.PushInputString("51 102 153");
      UtColor rgb;
      input.ReadValue(rgb);
      EXPECT_EQ(rgb.GetFormat(), UtColor::FmtUChar);
      EXPECT_EQ(rgb, expectedColor);
   }

   // Named color
   {
      UtInput input;
      input.PushInputString("magenta");
      UtColor namedColor;
      input.ReadValue(namedColor);
      UtColor magenta{1.F, 0.F, 1.F};
      EXPECT_EQ(namedColor.GetFormat(), UtColor::FmtFloatAlpha);
      EXPECT_EQ(namedColor, magenta);
   }
}

TEST(UtColor, ReadFromUtInputInvalid)
{
   // Invalid hex string or named color
   {
      UtInput input;
      input.PushInputString("FFFF");
      UtColor color;
      EXPECT_THROW(input.ReadValue(color), UtInput::ExceptionBase);
   }

   // Too few floating point values
   {
      UtInput input;
      input.PushInputString("float 0.1 0.2");
      UtColor color;
      EXPECT_THROW(input.ReadValue(color), UtInput::ExceptionBase);
   }

   // Floating point value out of range (high)
   {
      UtInput input;
      input.PushInputString("float 1.0 0.1 1.1");
      UtColor color;
      EXPECT_THROW(input.ReadValue(color), UtInput::ExceptionBase);
   }

   // Floating point value out of range (low)
   {
      UtInput input;
      input.PushInputString("float 1.0 0.1 1.0 -0.1");
      UtColor color;
      EXPECT_THROW(input.ReadValue(color), UtInput::ExceptionBase);
   }

   // Too few integral values
   {
      UtInput input;
      input.PushInputString("127 255");
      UtColor color;
      EXPECT_THROW(input.ReadValue(color), UtInput::ExceptionBase);
   }

   // Integral value out of range (high)
   {
      UtInput input;
      input.PushInputString("127 255 255 300");
      UtColor color;
      EXPECT_THROW(input.ReadValue(color), UtInput::ExceptionBase);
   }

   // Integral value out of range (low)
   {
      UtInput input;
      input.PushInputString("-127 255 255 300");
      UtColor color;
      EXPECT_THROW(input.ReadValue(color), UtInput::ExceptionBase);
   }
}

TEST(UtColor, IsHexColor)
{
   EXPECT_TRUE(UtColor::IsHexColor("12AABBCC"));   // RGBA
   EXPECT_TRUE(UtColor::IsHexColor("12aabbcc"));   // rgba
   EXPECT_TRUE(UtColor::IsHexColor("12AABB"));     // RGB
   EXPECT_TRUE(UtColor::IsHexColor("12aabb"));     // rgb
   EXPECT_FALSE(UtColor::IsHexColor("AABBCCDD0")); // Too long
   EXPECT_FALSE(UtColor::IsHexColor("AABBC"));     // Too short
   EXPECT_FALSE(UtColor::IsHexColor("12AABBCG"));  // Invalid character
   EXPECT_FALSE(UtColor::IsHexColor("12aabbcg"));  // Invalid character
}
