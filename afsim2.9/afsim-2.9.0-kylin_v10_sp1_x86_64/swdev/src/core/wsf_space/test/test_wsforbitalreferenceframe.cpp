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

#include "WsfOrbitalReferenceFrame.hpp"

TEST(WsfOrbitalReferenceFrame, ToIdentifier)
{
   EXPECT_EQ(wsf::space::OrbitalReferenceFrameToIdentifier(wsf::space::OrbitalReferenceFrame::cINERTIAL), "inertial");
   EXPECT_EQ(wsf::space::OrbitalReferenceFrameToIdentifier(wsf::space::OrbitalReferenceFrame::cRIC), "ric");
   EXPECT_EQ(wsf::space::OrbitalReferenceFrameToIdentifier(wsf::space::OrbitalReferenceFrame::cNTW), "ntw");
   EXPECT_TRUE(wsf::space::OrbitalReferenceFrameToIdentifier(wsf::space::OrbitalReferenceFrame::cUNKNOWN).empty());
}

TEST(WsfOrbitalReferenceFrame, FromIdentifier)
{
   EXPECT_EQ(wsf::space::OrbitalReferenceFrameFromIdentifier("inertial"), wsf::space::OrbitalReferenceFrame::cINERTIAL);
   EXPECT_EQ(wsf::space::OrbitalReferenceFrameFromIdentifier("ric"), wsf::space::OrbitalReferenceFrame::cRIC);
   EXPECT_EQ(wsf::space::OrbitalReferenceFrameFromIdentifier("ntw"), wsf::space::OrbitalReferenceFrame::cNTW);
   EXPECT_EQ(wsf::space::OrbitalReferenceFrameFromIdentifier("unknown"), wsf::space::OrbitalReferenceFrame::cUNKNOWN);
}
