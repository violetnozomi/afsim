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

#include "UtCalendar.hpp"
#include "UtEarth.hpp"
#include "UtMoon.hpp"
#include "UtSun.hpp"
#include "UtVec3.hpp"
#include "WsfLibrationPoint.hpp"

TEST(WsfLibrationPoint, EarthMoon)
{
   static constexpr double cTOLERANCE = 1.0e-6; // micro meter tolerance

   wsf::space::LibrationPoint eml{wsf::space::LibrationPoint::System::cEARTH_MOON};
   EXPECT_EQ(eml.GetSystem(), wsf::space::LibrationPoint::System::cEARTH_MOON);

   // Reference time
   UtCalendar epoch{};
   epoch.SetDate(2020, 4, 1);
   epoch.SetTime(12, 0, 0.0);

   // Reference positions of Earth and Moon
   UtVec3d moonPos    = ut::Moon().GetLocationECI(epoch);
   UtVec3d earthPos   = ut::EarthEGM96().GetLocationECI(epoch);
   UtVec3d posDiff    = moonPos - earthPos;
   double  posDiffMag = posDiff.Magnitude();

   // L4, Earth, Moon form an equilateral triangle
   UtVec3d l4Pos = eml.GetPositionECI(wsf::space::LibrationPoint::Point::cL4, epoch);
   EXPECT_NEAR((l4Pos - moonPos).Magnitude(), posDiffMag, cTOLERANCE);
   EXPECT_NEAR((l4Pos - earthPos).Magnitude(), posDiffMag, cTOLERANCE);

   // Transform known L4 location to ECI frame and compare
   UtVec3d l4inRF{0.5, sqrt(3.0) / 2.0, 0.0};
   l4inRF *= posDiffMag;
   UtVec3d l4inECI = eml.TransformPositionRFtoECI(epoch, l4inRF);
   EXPECT_NEAR(l4Pos[0], l4inECI[0], cTOLERANCE);
   EXPECT_NEAR(l4Pos[1], l4inECI[1], cTOLERANCE);
   EXPECT_NEAR(l4Pos[2], l4inECI[2], cTOLERANCE);

   // Transform L4 position to RF and compare to known L4 location
   UtVec3d l4FromECI = eml.TransformPositionECItoRF(epoch, l4Pos);
   EXPECT_NEAR(l4FromECI[0], l4inRF[0], cTOLERANCE);
   EXPECT_NEAR(l4FromECI[1], l4inRF[1], cTOLERANCE);
   EXPECT_NEAR(l4FromECI[2], l4inRF[2], cTOLERANCE);

   // There is no simple way to write down the velocity of the libration points,
   // so instead this tests the consistency of the transformations to and from the rotating
   // frame.
   UtVec3d l4Vel    = eml.GetVelocityECI(wsf::space::LibrationPoint::Point::cL4, epoch);
   UtVec3d l4VelRF  = eml.TransformVelocityECItoRF(epoch, l4Pos, l4Vel);
   UtVec3d l4VelECI = eml.TransformVelocityRFtoECI(epoch, l4FromECI, l4VelRF);
   EXPECT_NEAR(l4Vel[0], l4VelECI[0], cTOLERANCE);
   EXPECT_NEAR(l4Vel[1], l4VelECI[1], cTOLERANCE);
   EXPECT_NEAR(l4Vel[2], l4VelECI[2], cTOLERANCE);

   // Verify the collinear points are collinear
   UtVec3d sepVec = posDiff.GetNormal();

   UtVec3d l1Pos = eml.GetPositionECI(wsf::space::LibrationPoint::Point::cL1, epoch);
   EXPECT_NEAR(l1Pos.DotProduct(sepVec), l1Pos.Magnitude(), cTOLERANCE);

   UtVec3d l2Pos = eml.GetPositionECI(wsf::space::LibrationPoint::Point::cL2, epoch);
   EXPECT_NEAR(l2Pos.DotProduct(sepVec), l2Pos.Magnitude(), cTOLERANCE);

   UtVec3d l3Pos = eml.GetPositionECI(wsf::space::LibrationPoint::Point::cL3, epoch);
   EXPECT_NEAR(l3Pos.DotProduct(sepVec), -l3Pos.Magnitude(), cTOLERANCE);
}

TEST(WsfLibrationPoint, SunEarth)
{
   static constexpr double cTOLERANCE = 1.0e-4; // sub-millimeter tolerance

   wsf::space::LibrationPoint eml{wsf::space::LibrationPoint::System::cSUN_EARTH};
   EXPECT_EQ(eml.GetSystem(), wsf::space::LibrationPoint::System::cSUN_EARTH);

   // Reference time
   UtCalendar epoch{};
   epoch.SetDate(2020, 4, 1);
   epoch.SetTime(12, 0, 0.0);

   // Reference positions of Earth and Moon
   UtVec3d sunPos     = ut::Sun().GetLocationECI(epoch);
   UtVec3d earthPos   = ut::EarthEGM96().GetLocationECI(epoch);
   UtVec3d posDiff    = sunPos - earthPos;
   double  posDiffMag = posDiff.Magnitude();

   // L4, Earth, Moon form an equilateral triangle
   UtVec3d l4Pos = eml.GetPositionECI(wsf::space::LibrationPoint::Point::cL4, epoch);
   EXPECT_NEAR((l4Pos - sunPos).Magnitude(), posDiffMag, cTOLERANCE);
   EXPECT_NEAR((l4Pos - earthPos).Magnitude(), posDiffMag, cTOLERANCE);

   // Transform known L4 location to ECI frame and compare
   UtVec3d l4inRF{0.5, sqrt(3.0) / 2.0, 0.0};
   l4inRF *= posDiffMag;
   UtVec3d l4inECI = eml.TransformPositionRFtoECI(epoch, l4inRF);
   EXPECT_NEAR(l4Pos[0], l4inECI[0], cTOLERANCE);
   EXPECT_NEAR(l4Pos[1], l4inECI[1], cTOLERANCE);
   EXPECT_NEAR(l4Pos[2], l4inECI[2], cTOLERANCE);

   // Transform L4 position to RF and compare to known L4 location
   UtVec3d l4FromECI = eml.TransformPositionECItoRF(epoch, l4Pos);
   EXPECT_NEAR(l4FromECI[0], l4inRF[0], cTOLERANCE);
   EXPECT_NEAR(l4FromECI[1], l4inRF[1], cTOLERANCE);
   EXPECT_NEAR(l4FromECI[2], l4inRF[2], cTOLERANCE);

   // There is no simple way to write down the velocity of the libration points,
   // so instead this tests the consistency of the transformations to and from the rotating
   // frame.
   UtVec3d l4Vel    = eml.GetVelocityECI(wsf::space::LibrationPoint::Point::cL4, epoch);
   UtVec3d l4VelRF  = eml.TransformVelocityECItoRF(epoch, l4Pos, l4Vel);
   UtVec3d l4VelECI = eml.TransformVelocityRFtoECI(epoch, l4FromECI, l4VelRF);
   EXPECT_NEAR(l4Vel[0], l4VelECI[0], cTOLERANCE);
   EXPECT_NEAR(l4Vel[1], l4VelECI[1], cTOLERANCE);
   EXPECT_NEAR(l4Vel[2], l4VelECI[2], cTOLERANCE);
}

TEST(WsfLibrationPoint, SunJupiter)
{
   static constexpr double cTOLERANCE = 1.0e-4; // sub-millimeter tolerance

   wsf::space::LibrationPoint eml{wsf::space::LibrationPoint::System::cSUN_EARTH};
   EXPECT_EQ(eml.GetSystem(), wsf::space::LibrationPoint::System::cSUN_EARTH);

   // Reference time
   UtCalendar epoch{};
   epoch.SetDate(2020, 4, 1);
   epoch.SetTime(12, 0, 0.0);

   // Reference positions of Earth and Moon
   UtVec3d sunPos     = ut::Sun().GetLocationECI(epoch);
   UtVec3d jupiterPos = ut::EarthEGM96().GetLocationECI(epoch);
   UtVec3d posDiff    = sunPos - jupiterPos;
   double  posDiffMag = posDiff.Magnitude();

   // L4, Earth, Moon form an equilateral triangle
   UtVec3d l4Pos = eml.GetPositionECI(wsf::space::LibrationPoint::Point::cL4, epoch);
   EXPECT_NEAR((l4Pos - sunPos).Magnitude(), posDiffMag, cTOLERANCE);
   EXPECT_NEAR((l4Pos - jupiterPos).Magnitude(), posDiffMag, cTOLERANCE);

   // Transform known L4 location to ECI frame and compare
   UtVec3d l4inRF{0.5, sqrt(3.0) / 2.0, 0.0};
   l4inRF *= posDiffMag;
   UtVec3d l4inECI = eml.TransformPositionRFtoECI(epoch, l4inRF);
   EXPECT_NEAR(l4Pos[0], l4inECI[0], cTOLERANCE);
   EXPECT_NEAR(l4Pos[1], l4inECI[1], cTOLERANCE);
   EXPECT_NEAR(l4Pos[2], l4inECI[2], cTOLERANCE);

   // Transform L4 position to RF and compare to known L4 location
   UtVec3d l4FromECI = eml.TransformPositionECItoRF(epoch, l4Pos);
   EXPECT_NEAR(l4FromECI[0], l4inRF[0], cTOLERANCE);
   EXPECT_NEAR(l4FromECI[1], l4inRF[1], cTOLERANCE);
   EXPECT_NEAR(l4FromECI[2], l4inRF[2], cTOLERANCE);

   // There is no simple way to write down the velocity of the libration points,
   // so instead this tests the consistency of the transformations to and from the rotating
   // frame.
   UtVec3d l4Vel    = eml.GetVelocityECI(wsf::space::LibrationPoint::Point::cL4, epoch);
   UtVec3d l4VelRF  = eml.TransformVelocityECItoRF(epoch, l4Pos, l4Vel);
   UtVec3d l4VelECI = eml.TransformVelocityRFtoECI(epoch, l4FromECI, l4VelRF);
   EXPECT_NEAR(l4Vel[0], l4VelECI[0], cTOLERANCE);
   EXPECT_NEAR(l4Vel[1], l4VelECI[1], cTOLERANCE);
   EXPECT_NEAR(l4Vel[2], l4VelECI[2], cTOLERANCE);
}

TEST(WsfLibrationPoint, StaticMembers)
{
   // GetSystemFromIdentifier
   EXPECT_EQ(wsf::space::LibrationPoint::GetSystemFromIdentifier("earth_moon"),
             wsf::space::LibrationPoint::System::cEARTH_MOON);
   EXPECT_EQ(wsf::space::LibrationPoint::GetSystemFromIdentifier("sun_earth"),
             wsf::space::LibrationPoint::System::cSUN_EARTH);
   EXPECT_EQ(wsf::space::LibrationPoint::GetSystemFromIdentifier("sun_jupiter"),
             wsf::space::LibrationPoint::System::cSUN_JUPITER);
   EXPECT_EQ(wsf::space::LibrationPoint::GetSystemFromIdentifier("anything_else"),
             wsf::space::LibrationPoint::System::cUNKNOWN);

   // GetPointFromIdentifier
   EXPECT_EQ(wsf::space::LibrationPoint::GetPointFromIdentifier("l1"), wsf::space::LibrationPoint::Point::cL1);
   EXPECT_EQ(wsf::space::LibrationPoint::GetPointFromIdentifier("l2"), wsf::space::LibrationPoint::Point::cL2);
   EXPECT_EQ(wsf::space::LibrationPoint::GetPointFromIdentifier("l3"), wsf::space::LibrationPoint::Point::cL3);
   EXPECT_EQ(wsf::space::LibrationPoint::GetPointFromIdentifier("l4"), wsf::space::LibrationPoint::Point::cL4);
   EXPECT_EQ(wsf::space::LibrationPoint::GetPointFromIdentifier("l5"), wsf::space::LibrationPoint::Point::cL5);
   EXPECT_EQ(wsf::space::LibrationPoint::GetPointFromIdentifier("anything_else"),
             wsf::space::LibrationPoint::Point::cUNKNOWN);

   // GetIdentifierFromSystem
   EXPECT_EQ(wsf::space::LibrationPoint::GetIdentifierFromSystem(wsf::space::LibrationPoint::System::cEARTH_MOON),
             "earth_moon");
   EXPECT_EQ(wsf::space::LibrationPoint::GetIdentifierFromSystem(wsf::space::LibrationPoint::System::cSUN_EARTH),
             "sun_earth");
   EXPECT_EQ(wsf::space::LibrationPoint::GetIdentifierFromSystem(wsf::space::LibrationPoint::System::cSUN_JUPITER),
             "sun_jupiter");
   EXPECT_EQ(wsf::space::LibrationPoint::GetIdentifierFromSystem(wsf::space::LibrationPoint::System::cUNKNOWN),
             "unknown_system");

   // GetIdentifierFromPoint
   EXPECT_EQ(wsf::space::LibrationPoint::GetIdentifierFromPoint(wsf::space::LibrationPoint::Point::cL1), "l1");
   EXPECT_EQ(wsf::space::LibrationPoint::GetIdentifierFromPoint(wsf::space::LibrationPoint::Point::cL2), "l2");
   EXPECT_EQ(wsf::space::LibrationPoint::GetIdentifierFromPoint(wsf::space::LibrationPoint::Point::cL3), "l3");
   EXPECT_EQ(wsf::space::LibrationPoint::GetIdentifierFromPoint(wsf::space::LibrationPoint::Point::cL4), "l4");
   EXPECT_EQ(wsf::space::LibrationPoint::GetIdentifierFromPoint(wsf::space::LibrationPoint::Point::cL5), "l5");
   EXPECT_EQ(wsf::space::LibrationPoint::GetIdentifierFromPoint(wsf::space::LibrationPoint::Point::cUNKNOWN),
             "unknown_point");

   // ValidLibrationPoint
   EXPECT_TRUE(wsf::space::LibrationPoint::ValidLibrationPoint(wsf::space::LibrationPoint::System::cEARTH_MOON,
                                                               wsf::space::LibrationPoint::Point::cL1));
   EXPECT_TRUE(wsf::space::LibrationPoint::ValidLibrationPoint(wsf::space::LibrationPoint::System::cEARTH_MOON,
                                                               wsf::space::LibrationPoint::Point::cL2));
   EXPECT_TRUE(wsf::space::LibrationPoint::ValidLibrationPoint(wsf::space::LibrationPoint::System::cEARTH_MOON,
                                                               wsf::space::LibrationPoint::Point::cL3));
   EXPECT_TRUE(wsf::space::LibrationPoint::ValidLibrationPoint(wsf::space::LibrationPoint::System::cEARTH_MOON,
                                                               wsf::space::LibrationPoint::Point::cL4));
   EXPECT_TRUE(wsf::space::LibrationPoint::ValidLibrationPoint(wsf::space::LibrationPoint::System::cEARTH_MOON,
                                                               wsf::space::LibrationPoint::Point::cL5));
   EXPECT_TRUE(wsf::space::LibrationPoint::ValidLibrationPoint(wsf::space::LibrationPoint::System::cSUN_EARTH,
                                                               wsf::space::LibrationPoint::Point::cL1));
   EXPECT_TRUE(wsf::space::LibrationPoint::ValidLibrationPoint(wsf::space::LibrationPoint::System::cSUN_EARTH,
                                                               wsf::space::LibrationPoint::Point::cL2));
   EXPECT_TRUE(wsf::space::LibrationPoint::ValidLibrationPoint(wsf::space::LibrationPoint::System::cSUN_EARTH,
                                                               wsf::space::LibrationPoint::Point::cL3));
   EXPECT_TRUE(wsf::space::LibrationPoint::ValidLibrationPoint(wsf::space::LibrationPoint::System::cSUN_EARTH,
                                                               wsf::space::LibrationPoint::Point::cL4));
   EXPECT_TRUE(wsf::space::LibrationPoint::ValidLibrationPoint(wsf::space::LibrationPoint::System::cSUN_EARTH,
                                                               wsf::space::LibrationPoint::Point::cL5));
   EXPECT_TRUE(wsf::space::LibrationPoint::ValidLibrationPoint(wsf::space::LibrationPoint::System::cSUN_JUPITER,
                                                               wsf::space::LibrationPoint::Point::cL1));
   EXPECT_TRUE(wsf::space::LibrationPoint::ValidLibrationPoint(wsf::space::LibrationPoint::System::cSUN_JUPITER,
                                                               wsf::space::LibrationPoint::Point::cL2));
   EXPECT_TRUE(wsf::space::LibrationPoint::ValidLibrationPoint(wsf::space::LibrationPoint::System::cSUN_JUPITER,
                                                               wsf::space::LibrationPoint::Point::cL3));
   EXPECT_TRUE(wsf::space::LibrationPoint::ValidLibrationPoint(wsf::space::LibrationPoint::System::cSUN_JUPITER,
                                                               wsf::space::LibrationPoint::Point::cL4));
   EXPECT_TRUE(wsf::space::LibrationPoint::ValidLibrationPoint(wsf::space::LibrationPoint::System::cSUN_JUPITER,
                                                               wsf::space::LibrationPoint::Point::cL5));
   EXPECT_FALSE(wsf::space::LibrationPoint::ValidLibrationPoint(wsf::space::LibrationPoint::System::cUNKNOWN,
                                                                wsf::space::LibrationPoint::Point::cL1));
   EXPECT_FALSE(wsf::space::LibrationPoint::ValidLibrationPoint(wsf::space::LibrationPoint::System::cUNKNOWN,
                                                                wsf::space::LibrationPoint::Point::cL2));
   EXPECT_FALSE(wsf::space::LibrationPoint::ValidLibrationPoint(wsf::space::LibrationPoint::System::cUNKNOWN,
                                                                wsf::space::LibrationPoint::Point::cL3));
   EXPECT_FALSE(wsf::space::LibrationPoint::ValidLibrationPoint(wsf::space::LibrationPoint::System::cUNKNOWN,
                                                                wsf::space::LibrationPoint::Point::cL4));
   EXPECT_FALSE(wsf::space::LibrationPoint::ValidLibrationPoint(wsf::space::LibrationPoint::System::cUNKNOWN,
                                                                wsf::space::LibrationPoint::Point::cL5));
   EXPECT_FALSE(wsf::space::LibrationPoint::ValidLibrationPoint(wsf::space::LibrationPoint::System::cEARTH_MOON,
                                                                wsf::space::LibrationPoint::Point::cUNKNOWN));
   EXPECT_FALSE(wsf::space::LibrationPoint::ValidLibrationPoint(wsf::space::LibrationPoint::System::cSUN_EARTH,
                                                                wsf::space::LibrationPoint::Point::cUNKNOWN));
   EXPECT_FALSE(wsf::space::LibrationPoint::ValidLibrationPoint(wsf::space::LibrationPoint::System::cSUN_JUPITER,
                                                                wsf::space::LibrationPoint::Point::cUNKNOWN));

   EXPECT_FALSE(wsf::space::LibrationPoint::ValidLibrationPoint(static_cast<wsf::space::LibrationPoint::System>(-1),
                                                                wsf::space::LibrationPoint::Point::cL1));
   EXPECT_FALSE(wsf::space::LibrationPoint::ValidLibrationPoint(wsf::space::LibrationPoint::System::cEARTH_MOON,
                                                                static_cast<wsf::space::LibrationPoint::Point>(-1)));
}
