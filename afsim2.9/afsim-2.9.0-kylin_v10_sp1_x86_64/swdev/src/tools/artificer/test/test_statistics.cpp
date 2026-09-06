// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright (C) 2022 Stellar Science; U.S. Government has Unlimited Rights.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <gtest/gtest.h>

#include "statistics.hpp"

TEST(Statistics, Accumulate)
{
   EXPECT_EQ(6.0, artificer::Accumulate(std::vector<double>({1.0, 2.0, 3.0})));
}

TEST(Statistics, Average)
{
   EXPECT_EQ(2.0, artificer::Average(std::vector<double>({1.0, 2.0, 3.0})));
}

TEST(Statistics, StdDev)
{
   EXPECT_NEAR(0.816497, artificer::StdDev(std::vector<double>({1.0, 2.0, 3.0})), 0.000001);
}
