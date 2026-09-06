//*****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
//*****************************************************************************

#include <algorithm>
#include <array>
#include <iostream>
#include <string>
#include <utility>

#include <gtest/gtest.h>

#include "UtBitmapImage.hpp"
#include "UtLocator.hpp"
#include "UtTestDataService.hpp"

using RGBA = UtBitmapImage::BGRA;
using RGB  = UtBitmapImage::BGR;

class UtBitmapImageTest : public ::testing::Test
{
protected:
   bool InitAndLoad(const std::string& filename)
   {
      // locate and load bmp file
      const auto& dataService = ut::Locator<ut::TestDataService>::get();
      return mBitMap.Load(dataService.getResource(filename).GetNormalizedPath());
   }
   UtBitmapImage mBitMap;
};

TEST_F(UtBitmapImageTest, BMP32Bit_StarTilePattern_ValidPixelsExpected)
{
   UtBitmapImage::Type type     = UtBitmapImage::cTHIRTY_TWO_BIT;
   const std::string   filename = "wizard32.bmp";

   std::array<RGBA, 4>                pixel_data_out;
   std::array<std::pair<int, int>, 4> pixels_in           = {{{42, 26}, {43, 26}, {44, 26}, {45, 26}}};
   const std::array<RGBA, 4>          pixel_data_expected = {
      {{244, 4, 117, 255}, {247, 9, 121, 255}, {146, 57, 91, 255}, {29, 105, 112, 255}}};

   ASSERT_TRUE(InitAndLoad(filename));
   ASSERT_EQ(mBitMap.GetType(), type);

   // get bgra value given a the row,column pair for a pixel
   auto get_BGRA = [&](std::pair<int, int>& pixel) { return *((mBitMap.GetPixel(pixel.first, pixel.second).mBGRA_Ptr)); };

   // for each pixel location, get bgra value and store in pixel_data_out array
   std::transform(pixels_in.begin(), pixels_in.end(), pixel_data_out.begin(), get_BGRA);

   ASSERT_EQ(pixel_data_out, pixel_data_expected);
}
TEST_F(UtBitmapImageTest, BMP24Bit_NoTilePattern_ValidPixelsExpected)
{
   UtBitmapImage::Type type     = UtBitmapImage::cTWENTY_FOUR_BIT;
   const std::string   filename = "wizard24.bmp";

   std::array<RGB, 5>                 pixel_data_out;
   std::array<std::pair<int, int>, 5> pixels_in           = {{{36, 25}, {20, 36}, {32, 45}, {48, 27}, {25, 27}}};
   const std::array<RGB, 5>           pixel_data_expected = {
      {{43, 177, 193}, {255, 41, 142}, {92, 7, 47}, {239, 19, 123}, {97, 1, 46}}};

   ASSERT_TRUE(InitAndLoad(filename));
   ASSERT_EQ(mBitMap.GetType(), type);

   // get bgra value given a the row,column pair for a pixel
   auto get_BGR = [&](std::pair<int, int>& pixel) { return *((mBitMap.GetPixel(pixel.first, pixel.second).mBGR_Ptr)); };

   // for each pixel location, get bgr value and store in pixel_data_out array
   std::transform(pixels_in.begin(), pixels_in.end(), pixel_data_out.begin(), get_BGR);

   ASSERT_EQ(pixel_data_out, pixel_data_expected);
}
