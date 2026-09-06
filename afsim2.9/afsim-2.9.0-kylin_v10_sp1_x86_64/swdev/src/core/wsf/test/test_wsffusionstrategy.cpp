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

#include "WsfFusionStrategy.hpp"

class TestableWsfFusionStrategy : WsfFusionStrategy
{
public:
   using WsfFusionStrategy::FuseEstimates;
   using WsfFusionStrategy::NormalizeAzRangeEstimate;
   using WsfFusionStrategy::NormalizeElRangeEstimate;
};

TEST(WsfFusionStrategy, FuseEstimates)
{
   // following is a test of the FuseEstimates method.
   UtMatrixd p1(2, 2);
   UtMatrixd p2(2, 2);
   p1(0, 0) = 10;
   p1(0, 1) = 5;
   p1(1, 0) = 5;
   p1(1, 1) = 10;

   p2(0, 0) = 10;
   p2(0, 1) = -5;
   p2(1, 0) = -5;
   p2(1, 1) = 10;

   UtMatrixd e1(2, 1);
   UtMatrixd e2(2, 1);

   // all zeros, so no need to set.
   UtMatrixd eo(2, 1);
   UtMatrixd po(2, 2);

   TestableWsfFusionStrategy::FuseEstimates(e1, e2, p1, p2, eo, po);

   EXPECT_DOUBLE_EQ(0.0, eo(0, 0));
   EXPECT_DOUBLE_EQ(0.0, eo(1, 0));
   EXPECT_DOUBLE_EQ(3.75, po(0, 0));
   EXPECT_DOUBLE_EQ(0.0, po(0, 1));
   EXPECT_DOUBLE_EQ(0.0, po(1, 0));
   EXPECT_DOUBLE_EQ(3.75, po(1, 1));

   UtMatrixd p3(2, 2);
   p3(0, 0) = 9999;
   p3(0, 1) = 0.0;
   p3(1, 0) = 0.0;
   p3(1, 1) = 9999;

   UtMatrixd e3(2, 1);
   e3(0, 0) = 1.0;
   e3(1, 0) = 1.0;

   TestableWsfFusionStrategy::FuseEstimates(e1, e3, p1, p3, eo, po);
   EXPECT_DOUBLE_EQ(0.001497902935889754323650624, eo(0, 0));
   EXPECT_DOUBLE_EQ(0.001497902935889754323650624, eo(1, 0));
   EXPECT_DOUBLE_EQ(9.987516227780906419297935, po(0, 0));
   EXPECT_DOUBLE_EQ(4.990015228180746831299075, po(0, 1));
   EXPECT_DOUBLE_EQ(4.990015228180746831299075, po(1, 0));
   EXPECT_DOUBLE_EQ(9.987516227780906419297935, po(1, 1));
}

TEST(WsfFusionStrategy, NormalizeAzElEstimates)
{
   double az    = UtMath::cPI_OVER_4;
   double range = 1000.0;
   double sinAz = sin(az);
   double cosAz = cos(az);

   UtMatrixd azRangeEstimate(2, 1);
   azRangeEstimate(0, 0) = range * cosAz;
   azRangeEstimate(1, 0) = range * sinAz;
   UtMatrixd covar(2, 2);
   covar(0, 0) = 0.1 * azRangeEstimate(0, 0);
   covar(1, 1) = 0.05 * azRangeEstimate(1, 0);

   UtMatrixd azNormEstimate(3, 1);
   UtMatrixd azNormCovar(3, 3);
   double    elevation = 0.0;
   TestableWsfFusionStrategy::NormalizeAzRangeEstimate(azRangeEstimate, covar, elevation, azNormEstimate, azNormCovar);

   EXPECT_DOUBLE_EQ(range * cosAz, azNormEstimate(0, 0)); // 1.0e-6
   EXPECT_DOUBLE_EQ(range * sinAz, azNormEstimate(1, 0)); // 1.0e-6
   EXPECT_DOUBLE_EQ(0.0, azNormEstimate(2, 0));           // 1.0e-6

   elevation = UtMath::cPI_OVER_4;

   double sinEl = sin(elevation);
   double cosEl = cos(elevation);

   TestableWsfFusionStrategy::NormalizeAzRangeEstimate(azRangeEstimate, covar, elevation, azNormEstimate, azNormCovar);
   EXPECT_DOUBLE_EQ(range * cosAz * cosEl, azNormEstimate(0, 0)); // 1.0e-6
   EXPECT_DOUBLE_EQ(range * sinAz * cosEl, azNormEstimate(1, 0)); // 1.0e-6
   EXPECT_DOUBLE_EQ(range * sinEl, azNormEstimate(2, 0));         // 1.0e-6

   EXPECT_NEAR(35.355339, azNormCovar(0, 0), 1.0e-6);
   EXPECT_DOUBLE_EQ(0.0, azNormCovar(0, 1));
   EXPECT_DOUBLE_EQ(0.0, azNormCovar(0, 2));
   EXPECT_DOUBLE_EQ(0.0, azNormCovar(1, 0));
   EXPECT_NEAR(17.677669, azNormCovar(1, 1), 1.0e-6);
   EXPECT_DOUBLE_EQ(0.0, azNormCovar(1, 2));
   EXPECT_DOUBLE_EQ(0.0, azNormCovar(2, 0));
   EXPECT_DOUBLE_EQ(0.0, azNormCovar(2, 1));
   EXPECT_DOUBLE_EQ(10000000000, azNormCovar(2, 2));

   UtMatrixd elRangeEstimate(2, 1);
   elRangeEstimate(0, 0) = range * cosEl;
   elRangeEstimate(1, 0) = range * sinEl;

   UtMatrixd elCovar(2, 2);
   elCovar(0, 0) = 0.1 * azRangeEstimate(0, 0);
   elCovar(1, 1) = 0.05 * azRangeEstimate(1, 0);
   EXPECT_NEAR(70.710678, elCovar(0, 0), 1.0e-6);
   EXPECT_DOUBLE_EQ(0.0, elCovar(0, 1));
   EXPECT_DOUBLE_EQ(0.0, elCovar(1, 0));
   EXPECT_NEAR(35.355339, elCovar(1, 1), 1.0e-6);

   UtMatrixd elNormEstimate(3, 1);
   UtMatrixd elNormCovar(3, 3);

   TestableWsfFusionStrategy::NormalizeElRangeEstimate(elRangeEstimate, elCovar, 0.0, elNormEstimate, elNormCovar);
   TestableWsfFusionStrategy::NormalizeElRangeEstimate(elRangeEstimate, elCovar, UtMath::cPI_OVER_2, elNormEstimate, elNormCovar);
   TestableWsfFusionStrategy::NormalizeElRangeEstimate(elRangeEstimate, elCovar, az, elNormEstimate, elNormCovar);
   EXPECT_DOUBLE_EQ(range * cosAz * cosEl, elNormEstimate(0, 0)); // 1.0e-6
   EXPECT_DOUBLE_EQ(range * sinAz * cosEl, elNormEstimate(1, 0)); // 1.0e-6
   EXPECT_DOUBLE_EQ(range * sinEl, elNormEstimate(2, 0));         // 1.0e-6

   EXPECT_NEAR(35.355339, elNormCovar(0, 0), 1.0e-6);
   EXPECT_DOUBLE_EQ(0.0, elNormCovar(0, 1));
   EXPECT_DOUBLE_EQ(0.0, elNormCovar(0, 2));
   EXPECT_DOUBLE_EQ(0.0, elNormCovar(1, 0));
   EXPECT_NEAR(35.355339, elNormCovar(1, 1), 1.0e-6);
   EXPECT_DOUBLE_EQ(0.0, elNormCovar(1, 2));
   EXPECT_DOUBLE_EQ(0.0, elNormCovar(2, 0));
   EXPECT_DOUBLE_EQ(0.0, elNormCovar(2, 1));
   EXPECT_NEAR(35.355339, elNormCovar(2, 2), 1.0e-6);

   UtMatrixd combinedEstimate(3, 1);
   UtMatrixd combinedCovar(3, 3);

   TestableWsfFusionStrategy::FuseEstimates(azNormEstimate, elNormEstimate, azNormCovar, elNormCovar, combinedEstimate, combinedCovar);
   EXPECT_NEAR(500.0, combinedEstimate(0, 0), 1.0e-6);
   EXPECT_NEAR(500.0, combinedEstimate(1, 0), 1.0e-6);
   EXPECT_NEAR(707.106781, combinedEstimate(2, 0), 1.0e-6);

   EXPECT_NEAR(17.677669, combinedCovar(0, 0), 1.0e-6);
   EXPECT_DOUBLE_EQ(0.0, combinedCovar(0, 1));
   EXPECT_DOUBLE_EQ(0.0, combinedCovar(0, 2));
   EXPECT_DOUBLE_EQ(0.0, combinedCovar(1, 0));
   EXPECT_NEAR(11.785113, combinedCovar(1, 1), 1.0e-6);
   EXPECT_DOUBLE_EQ(0.0, combinedCovar(1, 2));
   EXPECT_DOUBLE_EQ(0.0, combinedCovar(2, 0));
   EXPECT_DOUBLE_EQ(0.0, combinedCovar(2, 1));
   EXPECT_NEAR(35.355339, elNormCovar(2, 2), 1.0e-6);
}
