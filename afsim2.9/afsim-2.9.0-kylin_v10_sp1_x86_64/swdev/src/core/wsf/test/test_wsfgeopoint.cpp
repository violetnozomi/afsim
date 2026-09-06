// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <gtest/gtest.h>

#include "UtCalendar.hpp"
#include "UtSun.hpp"
#include "WsfGeoPoint.hpp"

TEST(WsfGeoPoint, TestEachHour)
{
   constexpr double cREFERENCE_LONGITUDE{0.0};

   // Get a single reference location
   WsfGeoPoint center{0.0, cREFERENCE_LONGITUDE, 0.0};
   UtCalendar  epoch{};
   double      refValue = center.GetApparentTime(epoch);

   for (int hour = 1; hour <= 24; ++hour)
   {
      double      lon = cREFERENCE_LONGITUDE + UtCalendar::cDEGREES_IN_HOUR * hour;
      WsfGeoPoint testPoint{0.0, lon, 0.0};
      double      testValue = testPoint.GetApparentTime(epoch);
      EXPECT_LE(testValue, 24.0);
      EXPECT_GE(testValue, 0.0);
      double expectedValue = refValue + hour;
      if (expectedValue > 24.0)
      {
         expectedValue -= 24.0;
      }
      EXPECT_NEAR(testValue, expectedValue, 1.0e-6);
   }
}
