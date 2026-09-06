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

#include <sstream>
#include <string>

#include <gtest/gtest.h>

#include "SpaceTestService.hpp"
#include "UtInput.hpp"
#include "UtLocator.hpp"
#include "Utils.hpp"
#include "WsfOrbitalReferenceFrame.hpp"
#include "maneuvers/WsfOrbitalManeuversDeltaV.hpp"

TEST(DeltaV_Maneuver, DefaultConstruction)
{
   auto&                       testDataService = ut::Locator<SpaceTestService>::get();
   WsfOrbitalManeuvers::DeltaV mvr{testDataService.GetScenario()};

   EXPECT_EQ(mvr.GetDeltaV_Frame(), wsf::space::OrbitalReferenceFrame::cUNKNOWN);
   UtVec3d dvExpected{};
   EXPECT_EQ(mvr.GetConfiguredDeltaV(), dvExpected);
}

TEST(DeltaV_Maneuver, Construction)
{
   auto& testDataService = ut::Locator<SpaceTestService>::get();

   UtVec3d                     dv{1.0, 2.0, 3.0};
   auto                        frame = wsf::space::OrbitalReferenceFrame::cINERTIAL;
   WsfOrbitalManeuvers::DeltaV mvr{testDataService.GetScenario(), dv, frame};

   EXPECT_EQ(mvr.GetDeltaV_Frame(), frame);
   EXPECT_EQ(mvr.GetConfiguredDeltaV(), dv);
}

TEST(DeltaV_Maneuver, ProcessInputFailures)
{
   auto& testDataService = ut::Locator<SpaceTestService>::get();

   WsfOrbitalManeuvers::DeltaV mvr{testDataService.GetScenario()};

   TestBadValueThrow(mvr, "delta_v unknown_frame");
   TestBadValueThrow(mvr, "delta_v inertial 1 m");
   TestBadValueThrow(mvr, "delta_v inertial 1 m/s 1 m");
   TestBadValueThrow(mvr, "delta_v inertial 1 m/s 1 m/s 1 m");

   TestEndOfDataThrow(mvr, "delta_v");
   TestEndOfDataThrow(mvr, "delta_v ric");
   TestEndOfDataThrow(mvr, "delta_v ric 1 m/s");
   TestEndOfDataThrow(mvr, "delta_v ric 1 m/s 1 m/s");
}

TEST(DeltaV_Maneuver, ProcessInput)
{
   auto& testDataService = ut::Locator<SpaceTestService>::get();

   WsfOrbitalManeuvers::DeltaV mvr{testDataService.GetScenario()};

   std::stringstream inputStream{};
   inputStream << "delta_v inertial 1.0 m/s 2.0 m/s 3.0 m/s\n";
   inputStream << "delta_v ric 4.0 km/s 5.0 km/s 6.0 km/s";
   UtInput input{};
   input.PushInputString(inputStream.str());

   std::string command;
   input.ReadCommand(command);
   EXPECT_TRUE(mvr.ProcessInput(input));
   EXPECT_EQ(mvr.GetDeltaV_Frame(), wsf::space::OrbitalReferenceFrame::cINERTIAL);
   UtVec3d dvExpectedECI{1.0, 2.0, 3.0};
   EXPECT_EQ(mvr.GetConfiguredDeltaV(), dvExpectedECI);

   input.ReadCommand(command);
   EXPECT_TRUE(mvr.ProcessInput(input));
   EXPECT_EQ(mvr.GetDeltaV_Frame(), wsf::space::OrbitalReferenceFrame::cRIC);
   UtVec3d dvExpectedRIC{4000.0, 5000.0, 6000.0};
   EXPECT_EQ(mvr.GetConfiguredDeltaV(), dvExpectedRIC);
}

TEST(DeltaV_Maneuver, SetDeltaV_AndFrame)
{
   auto& testDataService = ut::Locator<SpaceTestService>::get();

   WsfOrbitalManeuvers::DeltaV mvr{testDataService.GetScenario()};
   mvr.SetDeltaV_Frame(wsf::space::OrbitalReferenceFrame::cRIC);
   EXPECT_EQ(mvr.GetDeltaV_Frame(), wsf::space::OrbitalReferenceFrame::cRIC);

   mvr.SetDeltaV_Component(0, 1.0);
   mvr.SetDeltaV_Component(1, 2.0);
   mvr.SetDeltaV_Component(2, 3.0);
   EXPECT_EQ(mvr.GetDeltaV_Component(0).GetAsUnit(UtUnitSpeed::cMETERS_PER_SECOND), 1.0);
   EXPECT_EQ(mvr.GetDeltaV_Component(1).GetAsUnit(UtUnitSpeed::cMETERS_PER_SECOND), 2.0);
   EXPECT_EQ(mvr.GetDeltaV_Component(2).GetAsUnit(UtUnitSpeed::cMETERS_PER_SECOND), 3.0);
   UtVec3d dv{1.0, 2.0, 3.0};
   EXPECT_EQ(mvr.GetConfiguredDeltaV(), dv);
}

TEST(DeltaV_Maneuver, IsFrameSupported)
{
   EXPECT_TRUE(WsfOrbitalManeuvers::DeltaV::IsFrameSupported(wsf::space::OrbitalReferenceFrame::cINERTIAL));
   EXPECT_TRUE(WsfOrbitalManeuvers::DeltaV::IsFrameSupported(wsf::space::OrbitalReferenceFrame::cRIC));
   EXPECT_FALSE(WsfOrbitalManeuvers::DeltaV::IsFrameSupported(wsf::space::OrbitalReferenceFrame::cNTW));
}
