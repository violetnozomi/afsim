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

#include "UtEntity.hpp"
#include "UtRelativeGeometry.hpp"
#include "UtVec3.hpp"
TEST(UtRelativeGeometry, RangeRate)
{
   const double tolerance    = 1.0e-9;
   double       rangeRate    = -1.0;
   const auto   cEARTH       = ut::EarthEGM96();
   const double cEARTHRADIUS = cEARTH.GetMeanRadius();

   // locations
   UtVec3d loc_1(1.0, 0.0, 0.0);
   UtVec3d loc_2(1.0, 1.0e-6, 0.0);

   loc_2.Normalize();

   loc_1 = loc_1 * cEARTHRADIUS;
   loc_2 = loc_2 * cEARTHRADIUS;

   // velocities
   UtVec3d vel_1(0.0, 0.0, 0.0);
   UtVec3d vel_2(0.0, 20.0, 0.0);
   UtVec3d vel_3(0.0, 30.0, 0.0);

   // Initialize two entities
   UtEntity e1;
   UtEntity e2;

   // test two entities, both not moving
   e1.SetLocationWCS(loc_1.GetData());
   e2.SetLocationWCS(loc_2.GetData());
   e1.SetVelocityWCS(vel_1.GetData());
   e2.SetVelocityWCS(vel_1.GetData());

   rangeRate = UtRelativeGeometry::CalculateRangeRate(e1, e2);
   EXPECT_NEAR(rangeRate, 0.0, tolerance);

   // test two entities, one moving from another, one locked at location
   e1.SetLocationWCS(loc_1.GetData());
   e2.SetLocationWCS(loc_2.GetData());
   e1.SetVelocityWCS(vel_1.GetData());
   e2.SetVelocityWCS(vel_2.GetData());

   rangeRate = UtRelativeGeometry::CalculateRangeRate(e1, e2);
   EXPECT_NEAR(rangeRate, 20.0, tolerance);

   // test two entities, one moving towards another, one locked at location
   e1.SetLocationWCS(loc_1.GetData());
   e2.SetLocationWCS(loc_2.GetData());
   e1.SetVelocityWCS(vel_2.GetData());
   e2.SetVelocityWCS(vel_1.GetData());

   rangeRate = UtRelativeGeometry::CalculateRangeRate(e1, e2);
   EXPECT_NEAR(rangeRate, -20.0, tolerance);

   // test two entities, both moving in the same direction but the one in front is moving faster
   e1.SetLocationWCS(loc_1.GetData());
   e2.SetLocationWCS(loc_2.GetData());
   e1.SetVelocityWCS(vel_2.GetData());
   e2.SetVelocityWCS(vel_3.GetData());

   rangeRate = UtRelativeGeometry::CalculateRangeRate(e1, e2);
   EXPECT_NEAR(rangeRate, 10.0, tolerance);

   // test two entities, both moving in the same direction but the one in front is moving slower
   e1.SetLocationWCS(loc_1.GetData());
   e2.SetLocationWCS(loc_2.GetData());
   e1.SetVelocityWCS(vel_3.GetData());
   e2.SetVelocityWCS(vel_2.GetData());

   rangeRate = UtRelativeGeometry::CalculateRangeRate(e1, e2);
   EXPECT_NEAR(rangeRate, -10.0, tolerance);

   // test two entities, both moving in the same direction at the same speed
   e1.SetLocationWCS(loc_1.GetData());
   e2.SetLocationWCS(loc_2.GetData());
   e1.SetVelocityWCS(vel_3.GetData());
   e2.SetVelocityWCS(vel_3.GetData());

   rangeRate = UtRelativeGeometry::CalculateRangeRate(e1, e2);
   EXPECT_NEAR(rangeRate, 0.0, tolerance);
}
