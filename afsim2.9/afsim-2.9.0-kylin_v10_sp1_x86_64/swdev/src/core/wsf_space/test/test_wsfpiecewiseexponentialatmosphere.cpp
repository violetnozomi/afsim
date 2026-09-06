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
#include "UtInput.hpp"
#include "UtLLAPos.hpp"
#include "Utils.hpp"
#include "WsfPiecewiseExponentialAtmosphere.hpp"

TEST(WsfSpacePiecewiseExponentialAtmosphere, ProcessInput)
{
   wsf::space::PiecewiseExponentialAtmosphere atmo{};

   TestFalseReturn(atmo, "unknown_command");
   TestBadValueThrow(atmo, "central_body krypton");
   TestEndOfDataThrow(atmo, "central_body earth_egm96");

   // Test default
   EXPECT_EQ(std::string{"earth_egm96"}, atmo.GetCentralBody().GetName());

   UtInput input{};
   input.PushInputString("central_body moon end_central_body");

   std::string command;
   input.ReadCommand(command);
   EXPECT_TRUE(atmo.ProcessInput(input));
   EXPECT_EQ(std::string{atmo.GetCentralBody().GetName()}, "moon");
}

TEST(WsfSpacePiecewiseExponentialAtmosphere, Continuity)
{
   wsf::space::PiecewiseExponentialAtmosphere atmo{};

   UtCalendar now{};

   UtLLAPos pos{};
   pos.SetLat(0.0);
   pos.SetLon(0.0);
   for (double alt = 1.0; alt < 1001; alt += 1.0)
   {
      pos.SetAlt(alt * 1000.0 - 0.01);
      double dminus = atmo.GetDensity(now, pos);

      pos.SetAlt(alt * 1000.0 + 0.01);
      double dplus = atmo.GetDensity(now, pos);

      double mean = (dminus + dplus) / 2.0;
      double diff = fabs(dminus - dplus);
      // Values in the table are given to three places.
      EXPECT_NEAR(diff / mean, 0.0, 5.0e-3);
   }
}
