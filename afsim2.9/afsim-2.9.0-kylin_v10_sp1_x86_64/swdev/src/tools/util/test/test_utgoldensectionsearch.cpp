// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <cmath>

#include <gtest/gtest.h>

#include "UtGoldenSectionSearch.hpp"

TEST(UtGoldenSectionSearch, Minimum)
{
   // A function with a known minimum (at aParam = 1.0)
   auto quad = [](double aParam) -> double { return aParam * aParam - 2.0 * aParam; };

   constexpr double cTOLERANCE = 1.0e-7;

   // Basic usage
   double minParam = UtGoldenSectionSearch(quad, -1.0, 2.0, cTOLERANCE);
   EXPECT_LT(fabs(minParam - 1.0), cTOLERANCE);

   // Try again with range that does not enclose minimum
   double minMissed = UtGoldenSectionSearch(quad, 2.0, 10.0, cTOLERANCE);
   EXPECT_LT(fabs(minMissed - 2.0), cTOLERANCE);

   // Try again with reversed range
   double minReversed = UtGoldenSectionSearch(quad, 2.0, -1.0, cTOLERANCE);
   EXPECT_LT(fabs(minReversed - 1.0), cTOLERANCE);
}

TEST(UtGoldenSectionSearch, Tolerance)
{
   // a function with a known minimum (at aParam = 1.0)
   auto func = [](double aParam) -> double { return -aParam * exp(-aParam); };

   // Check that for we get the correct answer up to the given tolerance.
   for (int i = 1; i < 7; ++i)
   {
      double tolerance = pow(10.0, -i);
      double value     = UtGoldenSectionSearch(func, 0.0, 2.0, tolerance);
      EXPECT_LT(fabs(value - 1.0), tolerance);
   }
}