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

#include <gtest/gtest.h>

#include "UtCalendar.hpp"
#include "UtEarth.hpp"
#include "UtVec3.hpp"
#include "WsfNORAD_OrbitalPropagator.hpp"

TEST(NORAD_OrbitalPropagator, EquatorialInversion)
{
   UtCalendar time{};
   time.SetDate(2018, 3, 20);
   time.SetTime(0.0);

   constexpr double cRADIUS = 8000000.0; // in m
   UtVec3d          location(cRADIUS, 0.0, 0.0);

   constexpr double cVELOCITY = 7000.0; // in m/s
   UtVec3d          velocity(cVELOCITY / 10.0, cVELOCITY, 0.0);

   WsfNORAD_OrbitalPropagator prop{};
   prop.SetInitialOrbitalState(ut::OrbitalState(time,
                                                ut::EarthWGS84(),
                                                ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                                                ut::OrbitalState::ReferenceFrame::cECI,
                                                ut::OrbitalState::Vector(location, velocity)));
   prop.Initialize(time);

   constexpr double         cTOLERANCE   = 1.0e-7;
   ut::OrbitalState::Vector initVec      = prop.GetOrbitalState().GetOrbitalStateVectorInertial();
   UtVec3d                  initLocation = initVec.GetLocation();
   UtVec3d                  initVelocity = initVec.GetVelocity();

   EXPECT_NEAR(initLocation[0], location[0], cTOLERANCE);
   EXPECT_NEAR(initLocation[1], location[1], cTOLERANCE);
   EXPECT_NEAR(initLocation[2], location[2], cTOLERANCE);

   EXPECT_NEAR(initVelocity[0], velocity[0], cTOLERANCE);
   EXPECT_NEAR(initVelocity[1], velocity[1], cTOLERANCE);
   EXPECT_NEAR(initVelocity[2], velocity[2], cTOLERANCE);

   // propagate it to some future time
   UtCalendar middleTime(time);
   middleTime.AdvanceTimeBy(3600.0);
   prop.Update(middleTime);

   // initialize a second propagator with that

   WsfNORAD_OrbitalPropagator reinit{};
   reinit.SetInitialOrbitalState(prop.GetOrbitalState());
   reinit.Initialize(middleTime);

   // propagate both to another future time
   UtCalendar finalTime(middleTime);
   finalTime.AdvanceTimeBy(3600.0);
   prop.Update(finalTime);
   reinit.Update(finalTime);

   // compare positions and velocities
   location               = prop.GetOrbitalState().GetOrbitalStateVectorInertial().GetLocation();
   velocity               = prop.GetOrbitalState().GetOrbitalStateVectorInertial().GetVelocity();
   UtVec3d middleLocation = reinit.GetOrbitalState().GetOrbitalStateVectorInertial().GetLocation();
   UtVec3d middleVelocity = reinit.GetOrbitalState().GetOrbitalStateVectorInertial().GetVelocity();

   EXPECT_NEAR(location[0], middleLocation[0], cTOLERANCE);
   EXPECT_NEAR(location[1], middleLocation[1], cTOLERANCE);
   EXPECT_NEAR(location[2], middleLocation[2], cTOLERANCE);
   EXPECT_NEAR(velocity[0], middleVelocity[0], cTOLERANCE);
   EXPECT_NEAR(velocity[1], middleVelocity[1], cTOLERANCE);
   EXPECT_NEAR(velocity[2], middleVelocity[2], cTOLERANCE);
}

TEST(NORAD_OrbitalPropagator, CircularInversion)
{
   UtCalendar time{};
   time.SetDate(2018, 3, 20);
   time.SetTime(0.0);

   const double cRADIUS = 8000000.0; // in m
   UtVec3d      location(cRADIUS, 0.0, 0.0);

   ut::EarthWGS84 earth;
   double         mu        = earth.GetGravitationalParameter();
   const double   cVELOCITY = sqrt(mu / cRADIUS);

   for (double theta = 0.0; theta < 3.0; theta += 0.3)
   {
      UtVec3d velocity(0.0, cVELOCITY * cos(theta), cVELOCITY * sin(theta));

      ut::OrbitalState initialState(ut::EarthWGS84(),
                                    ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                                    ut::OrbitalState::ReferenceFrame::cECI);
      initialState.Set(time, ut::OrbitalState::Vector(location, velocity));

      WsfNORAD_OrbitalPropagator prop;
      prop.Initialize(initialState);

      constexpr double cTOLERANCE   = 1.0e-5;
      UtVec3d          initLocation = prop.GetOrbitalState().GetOrbitalStateVectorInertial().GetLocation();
      UtVec3d          initVelocity = prop.GetOrbitalState().GetOrbitalStateVectorInertial().GetVelocity();

      EXPECT_NEAR(initLocation[0], location[0], cTOLERANCE);
      EXPECT_NEAR(initLocation[1], location[1], cTOLERANCE);
      EXPECT_NEAR(initLocation[2], location[2], cTOLERANCE);

      EXPECT_NEAR(initVelocity[0], velocity[0], cTOLERANCE);
      EXPECT_NEAR(initVelocity[1], velocity[1], cTOLERANCE);
      EXPECT_NEAR(initVelocity[2], velocity[2], cTOLERANCE);
   }
}
