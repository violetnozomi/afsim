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

#include "UtEarth.hpp"
#include "UtMath.hpp"
#include "WsfPathComputer.hpp"

// The test incrementally checks method by setting a increasingly restrictive case.
// The turn radius that can be achieved gets larger after each step.

TEST(WsfPathComputer, GetTurnRadius)
{
   WsfPathComputer pathComputer;

   // Test speed  m/s
   double speed(100.0);

   // Maximum radial acceleration constraint (m/s^2)
   // Default is 6 g's or approximately 58.8399 m/s^2
   // a_r = V^2 / R
   // R = a_r / V^2
   EXPECT_DOUBLE_EQ((speed * speed / (6.0 * UtEarth::cACCEL_OF_GRAVITY)), pathComputer.GetTurnRadius(speed)); // 169.953 m

   // Turn rate limit constraint (radians/s)
   // V = R * W
   // R = V / W
   double turnRateLimit(30.0 * UtMath::cRAD_PER_DEG); // 30 deg/s
   pathComputer.GetConstraints().mTurnRateLimit = turnRateLimit;
   EXPECT_DOUBLE_EQ((speed / turnRateLimit), pathComputer.GetTurnRadius(speed)); // 190.986 m

   // Maximum turn rate constraint (radians/s)
   // V = R * W
   // R = V / W
   double maximumTurnRate(25.0 * UtMath::cRAD_PER_DEG); // 25 deg/s
   pathComputer.GetConstraints().mMaxTurnRate = maximumTurnRate;
   EXPECT_DOUBLE_EQ((speed / maximumTurnRate), pathComputer.GetTurnRadius(speed)); // 229.183 m

   // Body Vertical limit constraint (m/s^2)
   // Using a different form of the equation for testing
   // In aircraft turning equations of motion, the relationship between radial acceleration and body g limit is
   // a_r = g * sqrt(n^2 - 1) = sqrt((n*g)^2 - g^2) where radial acceleration is a_r and body g limit is n*g.
   // R = V^2 / sqrt((n*g)^2 - g^2)
   // The code being tested uses the following equation (avoids the sqrt and is consistent with the same calculation in
   // other locations in the code base) a_r = body g limit * sin(roll angle) = body g limit * sin(acos(g / body g
   // limit)) R = V^2 / (body g limit * sin(acos(g / body g limit)))
   double bodyVertLimit(2.0 * UtEarth::cACCEL_OF_GRAVITY); // 2 g's
   pathComputer.GetConstraints().mBodyVertLimit = bodyVertLimit;
   EXPECT_DOUBLE_EQ((speed * speed / sqrt(pow(bodyVertLimit, 2) - pow(UtEarth::cACCEL_OF_GRAVITY, 2))),
                    pathComputer.GetTurnRadius(speed)); // 588.733 m

   // Test zero speed case now that all contraints are non-zero
   // All cases will be tested
   EXPECT_DOUBLE_EQ(0.0, pathComputer.GetTurnRadius(0.0));
}
