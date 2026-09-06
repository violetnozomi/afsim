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

#include "UtAtmosphere.hpp"
#include "UtInput.hpp"

namespace
{
UtAtmosphere     baseAtm        = UtAtmosphere::CreateNewAtmosphereTable();
constexpr double cEPSILON_K     = 0.05;
constexpr double cEPSILON_SIGMA = 0.00001; // Density Ratio
constexpr double SEA_LEVEL      = 0.0;
constexpr double AT_ALTITUDE    = 10000;
constexpr double ABOVE_US1976   = 61000.01;
} // namespace

TEST(UtAtmosphere, StandardDay)
{
   UtAtmosphere stdAtm{baseAtm, UtAtmosphere::cSTANDARD_DAY};
   // stdAtm.PrintAll(true, true);

   // Check a few temperatures
   EXPECT_NEAR(stdAtm.Temperature(SEA_LEVEL), 288.15, cEPSILON_K);
   EXPECT_NEAR(stdAtm.Temperature(AT_ALTITUDE), 223.15, cEPSILON_K);
   EXPECT_NEAR(stdAtm.Temperature(ABOVE_US1976), 0.0, cEPSILON_K);

   // Check a few density ratios
   EXPECT_NEAR(stdAtm.DensityRatio(0.0), 1.0, cEPSILON_SIGMA);
   EXPECT_NEAR(stdAtm.DensityRatio(AT_ALTITUDE), 0.3369, cEPSILON_SIGMA);
   EXPECT_NEAR(stdAtm.DensityRatio(ABOVE_US1976), 0.0, cEPSILON_SIGMA);

   // Check the different altitudes for contrailing
   EXPECT_FALSE(stdAtm.IsWithinContrailAltitudeBand(SEA_LEVEL));    // Should not be contrailing at sea level
   EXPECT_TRUE(stdAtm.IsWithinContrailAltitudeBand(AT_ALTITUDE));   // Should be contrailing at 10 Km MSL
   EXPECT_FALSE(stdAtm.IsWithinContrailAltitudeBand(ABOVE_US1976)); // Should not be contrailing at 61 Km MSL
}

TEST(UtAtmosphere, HotDay)
{
   UtAtmosphere hotAtm{baseAtm, UtAtmosphere::cHOT_DAY};
   // hotAtm.PrintAll(true, true);

   // Check a few temperatures
   EXPECT_NEAR(hotAtm.Temperature(SEA_LEVEL), 312.61, cEPSILON_K);
   EXPECT_NEAR(hotAtm.Temperature(AT_ALTITUDE), 242.88, cEPSILON_K);
   EXPECT_NEAR(hotAtm.Temperature(ABOVE_US1976), 0.0, cEPSILON_K);

   // Check a couple of density ratios
   EXPECT_NEAR(hotAtm.DensityRatio(SEA_LEVEL), 0.9217555, cEPSILON_SIGMA);
   EXPECT_NEAR(hotAtm.DensityRatio(AT_ALTITUDE), 0.30953, cEPSILON_SIGMA);
   EXPECT_NEAR(hotAtm.DensityRatio(ABOVE_US1976), 0.0, cEPSILON_SIGMA);

   // Check the different altitudes for contrailing
   EXPECT_FALSE(hotAtm.IsWithinContrailAltitudeBand(SEA_LEVEL));    // Should not be contrailing at sea level
   EXPECT_TRUE(hotAtm.IsWithinContrailAltitudeBand(AT_ALTITUDE));   // Should be contrailing at 10 Km MSL
   EXPECT_FALSE(hotAtm.IsWithinContrailAltitudeBand(ABOVE_US1976)); // Should not be contrailing at 61 Km MSL
}

TEST(UtAtmosphere, TropicalDay)
{
   UtAtmosphere tropicalAtm{baseAtm, UtAtmosphere::cTROPICAL_DAY};
   // tropicalAtm.PrintAll(true, true);

   // Check a few temperatures
   EXPECT_NEAR(tropicalAtm.Temperature(SEA_LEVEL), 305.28, cEPSILON_K);
   EXPECT_NEAR(tropicalAtm.Temperature(AT_ALTITUDE), 234.74, cEPSILON_K);
   EXPECT_NEAR(tropicalAtm.Temperature(ABOVE_US1976), 0.0, cEPSILON_K);

   // Check a couple of density ratios
   EXPECT_NEAR(tropicalAtm.DensityRatio(SEA_LEVEL), 0.9438876, cEPSILON_SIGMA);
   EXPECT_NEAR(tropicalAtm.DensityRatio(AT_ALTITUDE), 0.3202638, cEPSILON_SIGMA);
   EXPECT_NEAR(tropicalAtm.DensityRatio(ABOVE_US1976), 0.0, cEPSILON_SIGMA);

   // Check the different altitudes for contrailing
   EXPECT_FALSE(tropicalAtm.IsWithinContrailAltitudeBand(SEA_LEVEL));    // Should not be contrailing at sea level
   EXPECT_TRUE(tropicalAtm.IsWithinContrailAltitudeBand(AT_ALTITUDE));   // Should be contrailing at 10 Km MSL
   EXPECT_FALSE(tropicalAtm.IsWithinContrailAltitudeBand(ABOVE_US1976)); // Should not be contrailing at 61 Km MSL
}

TEST(UtAtmosphere, ColdDay)
{
   UtAtmosphere coldAtm{baseAtm, UtAtmosphere::cCOLD_DAY};
   // coldAtm.PrintAll(true, true);

   // Check a few temperatures
   EXPECT_NEAR(coldAtm.Temperature(SEA_LEVEL), 222.06, cEPSILON_K);
   EXPECT_NEAR(coldAtm.Temperature(AT_ALTITUDE), 208.17, cEPSILON_K);
   EXPECT_NEAR(coldAtm.Temperature(ABOVE_US1976), 0.0, cEPSILON_K);

   // Check a couple of density ratios
   EXPECT_NEAR(coldAtm.DensityRatio(SEA_LEVEL), 1.297623, cEPSILON_SIGMA);
   EXPECT_NEAR(coldAtm.DensityRatio(AT_ALTITUDE), 0.3611470, cEPSILON_SIGMA);
   EXPECT_NEAR(coldAtm.DensityRatio(ABOVE_US1976), 0.0, cEPSILON_SIGMA);

   // Check the different altitudes for contrailing
   EXPECT_FALSE(coldAtm.IsWithinContrailAltitudeBand(SEA_LEVEL));    // Should not be contrailing at sea level
   EXPECT_TRUE(coldAtm.IsWithinContrailAltitudeBand(AT_ALTITUDE));   // Should be contrailing at 10 Km MSL
   EXPECT_FALSE(coldAtm.IsWithinContrailAltitudeBand(ABOVE_US1976)); // Should not be contrailing at 61 Km MSL
}

TEST(UtAtmosphere, PolarDay)
{
   UtAtmosphere polarAtm{baseAtm, UtAtmosphere::cPOLAR_DAY};
   // polarAtm.PrintAll(true, true);

   // Check a few temperatures
   EXPECT_NEAR(polarAtm.Temperature(SEA_LEVEL), 246.67, cEPSILON_K);
   EXPECT_NEAR(polarAtm.Temperature(AT_ALTITUDE), 217.81, cEPSILON_K);
   EXPECT_NEAR(polarAtm.Temperature(ABOVE_US1976), 0.0, cEPSILON_K);

   // Check a couple of density ratios
   EXPECT_NEAR(polarAtm.DensityRatio(SEA_LEVEL), 1.168160, cEPSILON_SIGMA);
   EXPECT_NEAR(polarAtm.DensityRatio(AT_ALTITUDE), 0.3451590, cEPSILON_SIGMA);
   EXPECT_NEAR(polarAtm.DensityRatio(ABOVE_US1976), 0.0, cEPSILON_SIGMA);

   // Check the different altitudes for contrailing
   EXPECT_FALSE(polarAtm.IsWithinContrailAltitudeBand(SEA_LEVEL));    // Should not be contrailing at sea level
   EXPECT_TRUE(polarAtm.IsWithinContrailAltitudeBand(AT_ALTITUDE));   // Should be contrailing at 10 Km MSL
   EXPECT_FALSE(polarAtm.IsWithinContrailAltitudeBand(ABOVE_US1976)); // Should not be contrailing at 61 Km MSL
}

TEST(UtAtmosphere, SimpleDT)
{
   UtAtmosphere simpleDT_Atm{baseAtm, UtAtmosphere::cSIMPLE_DT};
   // simpleDT_Atm.PrintAll(true, true);

   // Check a few temperatures
   EXPECT_NEAR(simpleDT_Atm.Temperature(SEA_LEVEL), 298.15, cEPSILON_K);
   EXPECT_NEAR(simpleDT_Atm.Temperature(AT_ALTITUDE), 233.15, cEPSILON_K);
   EXPECT_NEAR(simpleDT_Atm.Temperature(ABOVE_US1976), 0.0, cEPSILON_K);

   // Check a couple of density ratios
   EXPECT_NEAR(simpleDT_Atm.DensityRatio(SEA_LEVEL), 0.9664598, cEPSILON_SIGMA);
   EXPECT_NEAR(simpleDT_Atm.DensityRatio(AT_ALTITUDE), 0.3224532, cEPSILON_SIGMA);
   EXPECT_NEAR(simpleDT_Atm.DensityRatio(ABOVE_US1976), 0.0, cEPSILON_SIGMA);

   // Check the different altitudes for contrailing
   EXPECT_FALSE(simpleDT_Atm.IsWithinContrailAltitudeBand(SEA_LEVEL));    // Should not be contrailing at sea level
   EXPECT_TRUE(simpleDT_Atm.IsWithinContrailAltitudeBand(AT_ALTITUDE));   // Should be contrailing at 10 Km MSL
   EXPECT_FALSE(simpleDT_Atm.IsWithinContrailAltitudeBand(ABOVE_US1976)); // Should not be contrailing at 61 Km MSL
}

TEST(UtAtmosphere, Custom)
{
   UtAtmosphere customAtm{baseAtm, UtAtmosphere::cCUSTOM};
   UtInput      input;
   input.PushInputString("contrailing_altitude_floor   60000 m\
                          contrailing_altitude_ceiling 62 Km\
                          atmosphere_table\
                             0.000  300.0  100000.0  1.30\n\
                             10000  400.0   50000.0  0.65\n\
                            112000  500.0       0.0  0.00\n\
                          end_atmosphere_table");
   std::string command;
   while (input.TryReadCommand(command))
   {
      ASSERT_TRUE(customAtm.ProcessGlobalInput(input));
   }
   // customAtm.PrintAll(true, true);

   // Check a few temperatures
   EXPECT_NEAR(customAtm.Temperature(SEA_LEVEL), 300.0, cEPSILON_K);
   EXPECT_NEAR(customAtm.Temperature(AT_ALTITUDE), 400.0, cEPSILON_K);
   EXPECT_NEAR(customAtm.Temperature(ABOVE_US1976), 450.0, cEPSILON_K);

   // Check a couple of density ratios
   EXPECT_NEAR(customAtm.DensityRatio(SEA_LEVEL), 1.06122, cEPSILON_SIGMA);
   EXPECT_NEAR(customAtm.DensityRatio(AT_ALTITUDE), 0.53061, cEPSILON_SIGMA);
   EXPECT_NEAR(customAtm.DensityRatio(ABOVE_US1976), 0.265306, cEPSILON_SIGMA);

   // Check the different altitudes for contrailing
   EXPECT_FALSE(customAtm.IsWithinContrailAltitudeBand(SEA_LEVEL));   // Should not be contrailing at sea level
   EXPECT_FALSE(customAtm.IsWithinContrailAltitudeBand(AT_ALTITUDE)); // Should not be contrailing at 10 Km MSL
   EXPECT_TRUE(customAtm.IsWithinContrailAltitudeBand(ABOVE_US1976)); // Should be contrailing at 61 Km MSL
}
