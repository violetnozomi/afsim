//*****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
//*****************************************************************************

#include <string>

#include "SpaceTestService.hpp"
#include "UtLocator.hpp"
#include "WsfApplication.hpp"
#include "WsfDateTime.hpp"
#include "WsfMeasurement.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfSpaceKinematicStateExtrapolation.hpp"
#include "WsfStandardApplication.hpp"
#include "ext/WsfExtInterface.hpp"
#include "gtest/gtest.h"

TEST(SpaceKinematicStateExtrapolation, Accuracy)
{
   wsf::KinematicStateInformation info;
   wsf::KinematicState            setValue;
   info.Set(setValue);
   auto& testDataService = ut::Locator<SpaceTestService>::get();
   testDataService.GetScenario().CompleteLoad();
   WsfSimulation sim(testDataService.GetScenario(), 1);
   sim.Initialize();
   wsf::space::KinematicStateExtrapolation ext;
   WsfMeasurement                          data;
   data.SetSimulation(sim);
   UtVec3d loc = UtVec3d(0.0, 0.0, 7000000.0);
   UtVec3d vel = UtVec3d(7546.053290107541848730144444018, 0.0, 0.0); // circular polar orbit.
   data.SetLocationWCS(loc.GetData());
   data.SetVelocityWCS(vel.GetData());
   double time = 0.0;
   data.SetUpdateTime(time);
   time += 2914.2583188430077919517215446533;                       // 1/2 revolution
   wsf::KinematicState ksInfo = ext.Extrapolate(time, data)->Get(); // extrapolate 1/2 rev into the future.
   // Results not more exact because of precession/nutation effects.
   EXPECT_NE(loc, ksInfo.GetLocationWCS());
   EXPECT_NEAR(ksInfo.GetLocationWCS()[0], 0.0, 1.0);
   EXPECT_NEAR(ksInfo.GetLocationWCS()[1], 0.0, 1.0);
   EXPECT_NEAR(ksInfo.GetLocationWCS()[2], -7000000.0, 1.0);
   EXPECT_NEAR(ksInfo.GetVelocityWCS()[2], 0.0, 0.01);
   EXPECT_NEAR(loc.Magnitude(), ksInfo.GetLocationWCS().Magnitude(), 1.0e-4);
   loc = UtVec3d(0.0, 0.0, -7000000.0);
   vel = UtVec3d(8000, 0.0, 0.0); // now elliptical polar orbit.
   data.SetLocationWCS(loc.GetData());
   data.SetVelocityWCS(vel.GetData());
   data.SetUpdateTime(time);
   time += 3554.0350581840661344937414204569;
   ksInfo = ext.Extrapolate(time, data)->Get(); // extrapolate 1/2 rev into the future.
   EXPECT_NEAR(ksInfo.GetLocationWCS()[0], 0.0, 1.0);
   EXPECT_NEAR(ksInfo.GetLocationWCS()[1], 0.0, 1.0);
   EXPECT_NEAR(ksInfo.GetLocationWCS()[2], 8980504.1948066823276503301493937, 1.0);
   EXPECT_NEAR(ksInfo.GetVelocityWCS()[2], 0.0, 0.01);
}
