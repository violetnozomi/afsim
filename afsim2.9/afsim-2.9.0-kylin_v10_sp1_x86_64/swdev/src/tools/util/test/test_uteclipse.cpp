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

#include <cmath>

#include <gtest/gtest.h>

#include "UtEclipse.hpp"
#include "UtVec3.hpp"

namespace
{

UtVec3d          cPRIMARY_POS{0.0, 0.0, 0.0};
constexpr double cPRIMARY_RADIUS{1.0};
constexpr double cSEPARATION{4.0};
UtVec3d          cSECONDARY_POS{-cSEPARATION, 0.0, 0.0};
constexpr double cSECONDARY_RADIUS{2.0};
constexpr double cECLIPSE_TOLERANCE{1.0e-5};

void TestCase(const UtVec3d& aLocation, double aExpected)
{
   double value =
      UtEclipse::GetUmbralFraction(cPRIMARY_POS, cPRIMARY_RADIUS, cSECONDARY_POS, cSECONDARY_RADIUS, aLocation);
   EXPECT_NEAR(value, aExpected, cECLIPSE_TOLERANCE);
}

} // namespace

void TestPenumbraEdge()
{
   // This places the two objects so that the x axis is tangent to both, with the
   // primary below the axis, and the secondary above the axis. Their projection should
   // be just right so that there is no overlap.
   double value = UtEclipse::GetUmbralFraction(UtVec3d{0.0, -cPRIMARY_RADIUS, 0.0},
                                               cPRIMARY_RADIUS,
                                               UtVec3d{-cSEPARATION, cSECONDARY_RADIUS, 0.0},
                                               cSECONDARY_RADIUS,
                                               UtVec3d{5.0 * cSEPARATION, 0.0, 0.0});
   EXPECT_NEAR(value, 0.0, cECLIPSE_TOLERANCE);
}

void TestUmbraEdge()
{
   // To work out these relationships, draw two circles of difference sizes
   // separated with their centers on the x axis. Then draw the line that
   // is tangent to both near the circles' top side. Then use similar triangles
   // that include the radius drawn from the circles to the co-tangent line.
   double  V = cSECONDARY_RADIUS * cSEPARATION / (cSECONDARY_RADIUS - cPRIMARY_RADIUS);
   double  x = (cPRIMARY_RADIUS + V - cSEPARATION) / 2.0;
   double  y = (V - cSEPARATION - x) * cSECONDARY_RADIUS / sqrt(V * V - cSECONDARY_RADIUS * cSECONDARY_RADIUS);
   UtVec3d testLocation{x, y, 0.0};

   TestCase(testLocation, 1.0);
}

TEST(UtEclipse, GetUmbralFraction)
{
   // This test uses simple values for the positions and radii of the two objects to
   // generate cases that are easy to work out what the value should be.

   // Test obvious eclipse
   TestCase(UtVec3d{2.0, 0.0, 0.0}, 1.0);

   // Test obvious non-eclipse
   TestCase(UtVec3d{-2.0, 0.0, 0.0}, 0.0);

   // Test edge of penumbra
   TestPenumbraEdge();

   // Test limit of umbra
   TestUmbraEdge();

   // Test viewing location inside objects
   TestCase(UtVec3d{-0.5 * cPRIMARY_RADIUS, 0.0, 0.0}, 1.0);
   TestCase(UtVec3d{-cSEPARATION - 0.5 * cSECONDARY_RADIUS, 0.0, 0.0}, 0.0);

   // Test cylindrical symmetry
   {
      double value =
         UtEclipse::GetUmbralFraction(cPRIMARY_POS, cPRIMARY_RADIUS, cSECONDARY_POS, cSECONDARY_RADIUS, UtVec3d{3.0, 3.0, 0.0});
      for (double theta = 0.0; theta < UtMath::cTWO_PI; theta += UtMath::cTWO_PI / 20.0)
      {
         double test = UtEclipse::GetUmbralFraction(cPRIMARY_POS,
                                                    cPRIMARY_RADIUS,
                                                    cSECONDARY_POS,
                                                    cSECONDARY_RADIUS,
                                                    UtVec3d{3.0, 3.0 * cos(theta), 3.0 * sin(theta)});
         EXPECT_NEAR(test, value, cECLIPSE_TOLERANCE);
      }
   }

   // Test really far away on axis; should be about equal to the ratio of the areas
   constexpr double cFAR_AWAY = 1.0e4;
   double           cRp       = std::sqrt(1.0 - std::pow(cPRIMARY_RADIUS / cFAR_AWAY, 2.0));
   double           cRs       = std::sqrt(1.0 - std::pow(cSECONDARY_RADIUS / (cFAR_AWAY + cSEPARATION), 2.0));
   TestCase(UtVec3d{cFAR_AWAY, 0.0, 0.0}, (1.0 - cRp) / (1.0 - cRs));

   // Test exceptions thrown for:

   // out of bounds primary radius;
   EXPECT_THROW({ UtEclipse::GetUmbralFraction(cPRIMARY_POS, -1.0, cSECONDARY_POS, cSECONDARY_RADIUS, UtVec3d{}); },
                std::domain_error);

   // out of bounds secondary radius;
   EXPECT_THROW({ UtEclipse::GetUmbralFraction(cPRIMARY_POS, cPRIMARY_RADIUS, cSECONDARY_POS, -1.0, UtVec3d{}); },
                std::domain_error);

   // and intersecting primary and secondary objects.
   EXPECT_THROW(
      {
         UtEclipse::GetUmbralFraction(cPRIMARY_POS, cPRIMARY_RADIUS * 4.0, cSECONDARY_POS, cSECONDARY_RADIUS, UtVec3d{});
      },
      std::domain_error);
}
