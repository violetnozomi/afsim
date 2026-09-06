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

#include "TimeScale.hpp"

TEST(TimeScale, rescale)
{
   struct TestCase
   {
      std::pair<int64_t, artificer::TimeScale> input;
      std::pair<int64_t, artificer::TimeScale> result;
   };
   TestCase cases[] = {{{10000, artificer::TimeScale::Nanoseconds}, {10, artificer::TimeScale::Microseconds}},
                       {{10000, artificer::TimeScale::Microseconds}, {10, artificer::TimeScale::Milliseconds}},
                       {{10000, artificer::TimeScale::Milliseconds}, {10, artificer::TimeScale::Seconds}},
                       {{10000, artificer::TimeScale::Seconds}, {10000, artificer::TimeScale::Seconds}}};

   for (const auto& curCase : cases)
   {
      EXPECT_EQ(curCase.result, artificer::RescaleDuration(curCase.input.first, curCase.input.second));
   }
}
