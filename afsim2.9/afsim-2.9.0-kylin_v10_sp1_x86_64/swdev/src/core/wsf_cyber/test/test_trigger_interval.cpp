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

#include "UtInput.hpp"
#include "triggers/WsfCyberTriggerInterval.hpp"

TEST(TriggerIntervalTest, Construction)
{
   wsf::cyber::trigger::Interval interval;
   EXPECT_FALSE(interval.IsFromSet());
   EXPECT_FALSE(interval.IsToSet());
   EXPECT_FALSE(interval.IsEverySet());

   EXPECT_TRUE(interval.GetFromTime() == 0.0);
   EXPECT_TRUE(interval.GetToTime() == 0.0);
   EXPECT_TRUE(interval.GetEveryTime() == 0.0);
}

TEST(TriggerIntervalTest, ChangeInterval)
{
   UtInput input1, input2, input3;
   input1.PushInputString("from 0.0 s");
   input2.PushInputString("to 3.4 min");
   input3.PushInputString("every 2.2 s");
   wsf::cyber::trigger::Interval interval;
   ASSERT_TRUE(interval.ProcessInput(input1));
   ASSERT_TRUE(interval.ProcessInput(input2));
   ASSERT_TRUE(interval.ProcessInput(input3));
   interval.Initialize();

   EXPECT_TRUE(interval.IsFromSet());
   EXPECT_TRUE(interval.IsToSet());
   EXPECT_TRUE(interval.IsEverySet());

   EXPECT_TRUE(interval.GetFromTime() == 0.0);
   EXPECT_TRUE(interval.GetToTime() == 204.0);
   EXPECT_TRUE(interval.GetEveryTime() == 2.2);
}

TEST(TriggerIntervalTest, NextInterval)
{
   UtInput input1, input2, input3;
   input1.PushInputString("from 0.0 s");
   input2.PushInputString("to 3.4 min");
   input3.PushInputString("every 2.2 s");
   wsf::cyber::trigger::Interval interval;
   ASSERT_TRUE(interval.ProcessInput(input1));
   ASSERT_TRUE(interval.ProcessInput(input2));
   ASSERT_TRUE(interval.ProcessInput(input3));
   interval.Initialize();

   auto nextInterval = interval.NextEvaluationTime(0.0);

   EXPECT_TRUE(nextInterval.first);
   EXPECT_TRUE(nextInterval.second == 2.2);

   nextInterval = interval.NextEvaluationTime(2.2);

   EXPECT_TRUE(nextInterval.first);
   EXPECT_TRUE(nextInterval.second == 4.4);

   nextInterval = interval.NextEvaluationTime(202.0);

   EXPECT_TRUE(nextInterval.first);
   EXPECT_TRUE(nextInterval.second == 202.4);

   nextInterval = interval.NextEvaluationTime(203.0);

   EXPECT_FALSE(nextInterval.first);
   EXPECT_TRUE(nextInterval.second == 0.0);
}
