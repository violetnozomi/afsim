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

#include "UtCalendar.hpp"
#include "UtEarth.hpp"
#include "UtInput.hpp"
#include "UtVec3.hpp"
#include "WsfNORAD_OrbitalPropagator.hpp"

TEST(NORAD_OrbitalPropagator, SetInitialOrbitalElementsFromPoint)
{
   // Setup the initial time
   UtCalendar initialTime{};
   initialTime.SetDate(2018, 3, 21);
   initialTime.SetTime(0.0);

   constexpr double cRADIUS = 8000000.0; // in m
   UtVec3d          location(cRADIUS, 0.0, 0.0);

   constexpr double cVELOCITY    = 7000.0; // in m/s
   constexpr double cINCLINATION = 0.3;    // in radians
   UtVec3d          velocity(0.0, cVELOCITY * cos(cINCLINATION), cVELOCITY * sin(cINCLINATION));

   // create a norad propagator with some given state
   WsfNORAD_OrbitalPropagator baseline{};
   ut::OrbitalState           initialState(ut::EarthWGS84(),
                                 ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                                 ut::OrbitalState::ReferenceFrame::cECI);
   initialState.Set(initialTime, ut::OrbitalState::Vector(location, velocity));
   baseline.SetInitialOrbitalState(initialState);
   baseline.Initialize(initialTime);

   // TEST: Now check that it is in the right place.
   constexpr double cTOLERANCE   = 5.0e-7;
   UtVec3d          initLocation = baseline.GetOrbitalState().GetOrbitalStateVectorInertial().GetLocation();
   UtVec3d          initVelocity = baseline.GetOrbitalState().GetOrbitalStateVectorInertial().GetVelocity();
   EXPECT_NEAR(initLocation[0], location[0], cTOLERANCE);
   EXPECT_NEAR(initLocation[1], location[1], cTOLERANCE);
   EXPECT_NEAR(initLocation[2], location[2], cTOLERANCE);
   EXPECT_NEAR(initVelocity[0], velocity[0], cTOLERANCE);
   EXPECT_NEAR(initVelocity[1], velocity[1], cTOLERANCE);
   EXPECT_NEAR(initVelocity[2], velocity[2], cTOLERANCE);

   // propagate it to some future time
   UtCalendar middleTime(initialTime);
   middleTime.AdvanceTimeBy(3600.0);
   baseline.Update(middleTime);

   // initialize a second propagator with that

   WsfNORAD_OrbitalPropagator reinit{};
   reinit.SetInitialOrbitalState(baseline.GetOrbitalState());
   reinit.Initialize(middleTime);

   // compare positions and velocities
   location               = baseline.GetOrbitalState().GetOrbitalStateVectorInertial().GetLocation();
   velocity               = baseline.GetOrbitalState().GetOrbitalStateVectorInertial().GetVelocity();
   UtVec3d middleLocation = reinit.GetOrbitalState().GetOrbitalStateVectorInertial().GetLocation();
   UtVec3d middleVelocity = reinit.GetOrbitalState().GetOrbitalStateVectorInertial().GetVelocity();

   EXPECT_NEAR(location[0], middleLocation[0], cTOLERANCE);
   EXPECT_NEAR(location[1], middleLocation[1], cTOLERANCE);
   EXPECT_NEAR(location[2], middleLocation[2], cTOLERANCE);
   EXPECT_NEAR(velocity[0], middleVelocity[0], cTOLERANCE);
   EXPECT_NEAR(velocity[1], middleVelocity[1], cTOLERANCE);
   EXPECT_NEAR(velocity[2], middleVelocity[2], cTOLERANCE);

   // propagate both to another future time
   UtCalendar finalTime(middleTime);
   finalTime.AdvanceTimeBy(3600.0);
   baseline.Update(finalTime);
   reinit.Update(finalTime);

   ut::OrbitalState state      = baseline.GetOrbitalState();
   ut::OrbitalState finalState = reinit.GetOrbitalState();

   // compare positions and velocities
   location              = baseline.GetOrbitalState().GetOrbitalStateVectorInertial().GetLocation();
   velocity              = baseline.GetOrbitalState().GetOrbitalStateVectorInertial().GetVelocity();
   UtVec3d finalLocation = reinit.GetOrbitalState().GetOrbitalStateVectorInertial().GetLocation();
   UtVec3d finalVelocity = reinit.GetOrbitalState().GetOrbitalStateVectorInertial().GetVelocity();

   constexpr double cFINAL_TOLERANCE = 5.0e-5;
   EXPECT_NEAR(location[0], finalLocation[0], cFINAL_TOLERANCE);
   EXPECT_NEAR(location[1], finalLocation[1], cFINAL_TOLERANCE);
   EXPECT_NEAR(location[2], finalLocation[2], cFINAL_TOLERANCE);
   EXPECT_NEAR(velocity[0], finalVelocity[0], cFINAL_TOLERANCE);
   EXPECT_NEAR(velocity[1], finalVelocity[1], cFINAL_TOLERANCE);
   EXPECT_NEAR(velocity[2], finalVelocity[2], cFINAL_TOLERANCE);
}

TEST(NORAD_OrbitalPropagator, CoordinateReductionTEME)
{
   // Example 3-15 from Vallado, Fundamentals of Astrodynamics and Applications, 4th ed.

   WsfNORAD_OrbitalPropagator prop;
   std::string                tle = "orbit\n";
   tle += "1 00005U 58002B   00179.78495062  .00000023  00000-0  28098-4 0  4753\n";
   tle += "2 00005  34.2682 348.7242 1859667 331.7664  19.3264 10.82419157413667\n";
   tle += "end_orbit\n";

   UtInput          input;
   ut::OrbitalState initialState(ut::EarthWGS84(),
                                 ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                                 ut::OrbitalState::ReferenceFrame::cTEME);
   input.PushInputString(tle);

   while (true)
   {
      try
      {
         std::string command;
         input.ReadCommand(command);
         initialState.ProcessInput(input);
      }
      catch (std::exception&)
      {
         break;
      }
   }

   int        deltaAT  = 32;   // s
   double     deltaUT1 = 0.22; // s
   UtCalendar epoch(deltaUT1, deltaAT);
   epoch.SetEpoch(00179.78495062);
   double ejd = epoch.GetJ2000_Date();
   initialState.Initialize(epoch);
   initialState.SetEpoch(epoch);
   prop.Initialize(initialState);
   double propjd = prop.GetOrbitalState().GetEpoch().GetJ2000_Date();
   EXPECT_NEAR(ejd, propjd, 1.0e-8);
   UtCalendar updateTime = epoch;
   updateTime.AdvanceTimeBy(3.0 * 86400.0); // 3 days
   prop.Update(updateTime);
   propjd = prop.GetOrbitalState().GetEpoch().GetJ2000_Date();
   EXPECT_NEAR(prop.GetOrbitalState().GetEpoch().GetTimeSince(epoch), 3 * 86400.0, 1.0e-6);

   ut::OrbitalState::Vector vecECI = prop.GetOrbitalState().GetOrbitalStateVectorInertial();
   UtVec3d                  r_eci  = vecECI.GetLocation();
   UtVec3d                  v_eci  = vecECI.GetVelocity();

   UtVec3d r_eci_expected(-9059.941386, 4659.697200, 813.9588875); // km
   r_eci_expected.Multiply(1000.0);                                // m
   double tolerance = 2.0;
   EXPECT_NEAR(r_eci[0], r_eci_expected[0], tolerance);
   EXPECT_NEAR(r_eci[1], r_eci_expected[1], tolerance);
   EXPECT_NEAR(r_eci[2], r_eci_expected[2], tolerance);

   UtVec3d v_eci_expected(-2.233348094, -4.110136162, -3.157394074);
   v_eci_expected.Multiply(1000.0); // m
   EXPECT_NEAR(v_eci[0], v_eci_expected[0], tolerance);
   EXPECT_NEAR(v_eci[1], v_eci_expected[1], tolerance);
   EXPECT_NEAR(v_eci[2], v_eci_expected[2], tolerance);

   // Following are output by the propagator
   UtVec3d r_teme_expected(-9060473.6889481898, 4658709.5728056030, 813686.97748389025);
   UtVec3d v_teme_expected(-2232.8328797508593, -4110.4534492821249, -3157.3454171376557);

   ut::OrbitalState::Vector vecTEME = prop.GetOrbitalState().GetOrbitalStateVectorTEME();
   UtVec3d                  r_teme  = vecTEME.GetLocation();
   UtVec3d                  v_teme  = vecTEME.GetVelocity();

   // now test conversions outside propagator.
   UtECI_Conversion conv(epoch, ut::EarthWGS84());
   conv.SetTime(3.0 * 86400.0);
   UtVec3d locTEME = conv.ConvertECI_ToTEME(r_eci);
   UtVec3d velTEME = conv.ConvertVelocityECI_ToTEME(v_eci);
   conv.SetLocationECI(r_eci);
   conv.SetVelocityECI(v_eci);
   UtVec3d compLocTEME = conv.GetLocationTEME();
   UtVec3d compVelTEME = conv.GetVelocityTEME();
   conv.SetLocationTEME(r_teme_expected);
   tolerance = 1.0e-7;

   EXPECT_NEAR(locTEME[0], r_teme[0], tolerance);
   EXPECT_NEAR(locTEME[1], r_teme[1], tolerance);
   EXPECT_NEAR(locTEME[2], r_teme[2], tolerance);

   EXPECT_NEAR(velTEME[0], v_teme[0], tolerance);
   EXPECT_NEAR(velTEME[1], v_teme[1], tolerance);
   EXPECT_NEAR(velTEME[2], v_teme[2], tolerance);

   EXPECT_NEAR(compLocTEME[0], r_teme[0], tolerance);
   EXPECT_NEAR(compLocTEME[1], r_teme[1], tolerance);
   EXPECT_NEAR(compLocTEME[2], r_teme[2], tolerance);

   EXPECT_NEAR(compVelTEME[0], v_teme[0], tolerance);
   EXPECT_NEAR(compVelTEME[1], v_teme[1], tolerance);
   EXPECT_NEAR(compVelTEME[2], v_teme[2], tolerance);
}
