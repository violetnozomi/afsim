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

#include "UtInput.hpp"
#include "Utils.hpp"
#include "WsfJacchiaRobertsAtmosphere.hpp"

TEST(WsfJacchiaRobertsAtmosphere, ProcessInput)
{
   // NOTE: The central body options are tested in the piecewise exponential class.

   wsf::space::JacchiaRobertsAtmosphere atmo{};

   // Test default values
   EXPECT_NEAR(atmo.GetSolarFlux(), 150.0, 1.0e-12);
   EXPECT_NEAR(atmo.GetAverageSolarFlux(), 150.0, 1.0e-12);
   EXPECT_NEAR(atmo.GetGeomagneticIndex(), 0.0, 1.0e-12);


   TestEndOfDataThrow(atmo, "solar_flux");
   TestBadValueThrow(atmo, "solar_flux -10");
   TestBadValueThrow(atmo, "solar_flux string");

   TestEndOfDataThrow(atmo, "average_solar_flux");
   TestBadValueThrow(atmo, "average_solar_flux -10");
   TestBadValueThrow(atmo, "average_solar_flux string");

   TestEndOfDataThrow(atmo, "geomagnetic_index");
   TestBadValueThrow(atmo, "geomagnetic_index -1");
   TestBadValueThrow(atmo, "geomagnetic_index 10");
   TestBadValueThrow(atmo, "geomagnetic_index string");

   UtInput input{};
   input.PushInputString("solar_flux 120\naverage_solar_flux 140\ngeomagnetic_index 4.0");

   std::string command;
   input.ReadCommand(command);
   atmo.ProcessInput(input);
   EXPECT_NEAR(atmo.GetSolarFlux(), 120.0, 1.0e-12);

   input.ReadCommand(command);
   atmo.ProcessInput(input);
   EXPECT_NEAR(atmo.GetAverageSolarFlux(), 140.0, 1.0e-12);

   input.ReadCommand(command);
   atmo.ProcessInput(input);
   EXPECT_NEAR(atmo.GetGeomagneticIndex(), 4.0, 1.0e-12);
}
