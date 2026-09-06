// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <iostream>

#include <gtest/gtest.h>

#include "UtMath.hpp"

namespace
{
constexpr double epsDoubleSmall = 0.1 * std::numeric_limits<double>::epsilon();
constexpr double epsDoubleLarge = 10.0 * std::numeric_limits<double>::epsilon();
constexpr float  epsFloatSmall  = 0.1f * std::numeric_limits<float>::epsilon();
constexpr float  epsFloatLarge  = 10.0f * std::numeric_limits<float>::epsilon();
} // namespace

bool AngleIsBetweenHelper(double aAngle_deg, double aLower_deg, double aUpper_deg)
{
   return UtMath::AngleIsBetween(aAngle_deg * UtMath::cRAD_PER_DEG,
                                 aLower_deg * UtMath::cRAD_PER_DEG,
                                 aUpper_deg * UtMath::cRAD_PER_DEG);
}

TEST(UtMath, AngleIsBetween)
{
   // Tests for aLowerBound == aUpperBound
   EXPECT_TRUE(AngleIsBetweenHelper(0, 0, 0));
   EXPECT_TRUE(AngleIsBetweenHelper(360, 360, 360));
   EXPECT_TRUE(AngleIsBetweenHelper(180, 540, 540));
   EXPECT_TRUE(AngleIsBetweenHelper(-180, 180, 180));
   EXPECT_FALSE(AngleIsBetweenHelper(180, 360, 360));

   // Test for full circle
   EXPECT_TRUE(AngleIsBetweenHelper(0, 0, 360));
   EXPECT_TRUE(AngleIsBetweenHelper(90, 0, 360));
   EXPECT_TRUE(AngleIsBetweenHelper(-90, 0, 360));
   EXPECT_TRUE(AngleIsBetweenHelper(360, 0, 360));
   EXPECT_TRUE(AngleIsBetweenHelper(0, -180, 180));
   EXPECT_TRUE(AngleIsBetweenHelper(540, -180, 180));

   // "Normal" Use cases
   EXPECT_TRUE(AngleIsBetweenHelper(0, 0, 90));
   EXPECT_TRUE(AngleIsBetweenHelper(90, 0, 90));
   EXPECT_FALSE(AngleIsBetweenHelper(180, 0, 90));
   EXPECT_FALSE(AngleIsBetweenHelper(-90, 0, 90));
   EXPECT_FALSE(AngleIsBetweenHelper(-90, 0, 180));
   EXPECT_TRUE(AngleIsBetweenHelper(-90, 180, 360));
   EXPECT_TRUE(AngleIsBetweenHelper(360, -1, 361));
   EXPECT_FALSE(AngleIsBetweenHelper(2, -1, 361));
   EXPECT_TRUE(AngleIsBetweenHelper(360, 355, 365));
   EXPECT_TRUE(AngleIsBetweenHelper(-360, 355, 365));
   EXPECT_TRUE(AngleIsBetweenHelper(-180, 175, 185));
   EXPECT_FALSE(AngleIsBetweenHelper(90, 175, 185));
   EXPECT_TRUE(AngleIsBetweenHelper(375, 10, 20));

   // Inverted Bounds
   EXPECT_FALSE(AngleIsBetweenHelper(10, 90, 0));
   EXPECT_FALSE(AngleIsBetweenHelper(365, 370, 360));
   EXPECT_TRUE(AngleIsBetweenHelper(0, 359, 1));
   EXPECT_TRUE(AngleIsBetweenHelper(45, 360, 0));
}

TEST(UtMath, NearlyEqualDouble)
{
   double value1 = 10.5;
   double value2 = value1 + epsDoubleSmall;
   double value3 = value1 - epsDoubleSmall;
   double value4 = value1 + epsDoubleLarge;
   double value5 = value1 - epsDoubleLarge;

   EXPECT_TRUE(UtMath::NearlyEqual(value1, value2));
   EXPECT_TRUE(UtMath::NearlyEqual(value1, value3));
   EXPECT_TRUE(UtMath::NearlyEqual(value1, value1 + 0.09, 0.1));

   EXPECT_FALSE(UtMath::NearlyEqual(value1, value4));
   EXPECT_FALSE(UtMath::NearlyEqual(value1, value5));
   EXPECT_FALSE(UtMath::NearlyEqual(value1, value1 + 0.11, 0.1));
}

TEST(UtMath, NearlyEqualFloat)
{
   float value1 = 10.5;
   float value2 = value1 + epsFloatSmall;
   float value3 = value1 - epsFloatSmall;
   float value4 = value1 + epsFloatLarge;
   float value5 = value1 - epsFloatLarge;

   EXPECT_TRUE(UtMath::NearlyEqual(value1, value2));
   EXPECT_TRUE(UtMath::NearlyEqual(value1, value3));
   EXPECT_TRUE(UtMath::NearlyEqual(value1, value1 + 0.09f, 0.1f));

   EXPECT_FALSE(UtMath::NearlyEqual(value1, value4));
   EXPECT_FALSE(UtMath::NearlyEqual(value1, value5));
   EXPECT_FALSE(UtMath::NearlyEqual(value1, value1 + 0.11f, 0.1f));
}

TEST(UtMath, NearlyZeroDouble)
{
   EXPECT_TRUE(UtMath::NearlyZero(epsDoubleSmall));
   EXPECT_TRUE(UtMath::NearlyZero(-epsDoubleSmall));
   EXPECT_TRUE(UtMath::NearlyZero(0.09, 0.1));

   EXPECT_FALSE(UtMath::NearlyZero(epsDoubleLarge));
   EXPECT_FALSE(UtMath::NearlyZero(-epsDoubleLarge));
   EXPECT_FALSE(UtMath::NearlyZero(0.11, 0.1));
}

TEST(UtMath, NearlyZeroFloat)
{
   EXPECT_TRUE(UtMath::NearlyZero(epsFloatSmall));
   EXPECT_TRUE(UtMath::NearlyZero(-epsFloatSmall));
   EXPECT_TRUE(UtMath::NearlyZero(0.09f, 0.1f));

   EXPECT_FALSE(UtMath::NearlyZero(epsFloatLarge));
   EXPECT_FALSE(UtMath::NearlyZero(-epsFloatLarge));
   EXPECT_FALSE(UtMath::NearlyZero(0.11f, 0.1f));
}

TEST(UtMath, NearlyEqualInt)
{
   EXPECT_TRUE(UtMath::NearlyEqual(3, 3));
   EXPECT_TRUE(UtMath::NearlyEqual(2, 3, 2));
   EXPECT_TRUE(UtMath::NearlyEqual(-3, -3));
   EXPECT_TRUE(UtMath::NearlyEqual(-2, -3, 2));

   EXPECT_FALSE(UtMath::NearlyEqual(2, 3, 1));
   EXPECT_FALSE(UtMath::NearlyEqual(-2, -3, 1));
}

TEST(UtMath, NearlyZeroInt)
{
   EXPECT_TRUE(UtMath::NearlyZero(0));
   EXPECT_FALSE(UtMath::NearlyZero(1));
}

TEST(UtMath, NearlyEqualNegEps)
{
   EXPECT_FALSE(UtMath::NearlyEqual(10.5, 10.55, -0.1));
}

TEST(UtMath, NearlyEqualConstexpr)
{
   constexpr bool equalDouble = UtMath::NearlyEqual(10.5, 10.6, 0.05);
   constexpr bool equalInt    = UtMath::NearlyEqual(3, 3);
   EXPECT_FALSE(equalDouble);
   EXPECT_TRUE(equalInt);
}

TEST(UtMath, NearlyZeroConstexpr)
{
   constexpr bool zeroDouble = UtMath::NearlyZero(epsDoubleSmall);
   constexpr bool zeroInt    = UtMath::NearlyZero(1);
   EXPECT_TRUE(zeroDouble);
   EXPECT_FALSE(zeroInt);
}

TEST(UtMath, RotlRotatesLeft)
{
   uint32_t a = 0x80000000;
   EXPECT_EQ(UtMath::rotl(a, 1), 1);

   uint64_t b = 0x8000000000000000;
   EXPECT_EQ(UtMath::rotl(b, 1), 1);
}
