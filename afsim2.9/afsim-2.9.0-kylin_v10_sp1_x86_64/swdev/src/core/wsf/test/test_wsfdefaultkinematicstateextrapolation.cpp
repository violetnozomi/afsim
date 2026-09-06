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

#include "UtInformation.hpp"
#include "WsfDefaultKinematicStateExtrapolation.hpp"
#include "WsfTrack.hpp"
#include "gtest/gtest.h"

TEST(DefaultKinematicStateExtrapolation, Accuracy)
{
   wsf::KinematicStateInformation info;
   wsf::KinematicState            setValue;
   info.Set(setValue);
   wsf::DefaultKinematicStateExtrapolation ext;
   WsfTrack                                data;
   UtVec3d                                 loc = UtVec3d(6378000, 0.0, 0.0);
   UtVec3d                                 vel = UtVec3d(0.0, 0.0, 100.0);
   data.SetLocationWCS(loc.GetData());
   data.SetVelocityWCS(vel.GetData());
   double time = 10.0;
   data.SetUpdateTime(time);
   time += 10.0;
   wsf::KinematicState ksInfo = ext.Extrapolate(time, data)->Get(); // extrapolate 10 s into the future.
   UtVec3d             diff;
   EXPECT_NEAR(ksInfo.GetLocationWCS()[2], 1000.0, 0.001);

   WsfTrack noData;
   EXPECT_FALSE(ext.Extrapolate(time, noData));
}
