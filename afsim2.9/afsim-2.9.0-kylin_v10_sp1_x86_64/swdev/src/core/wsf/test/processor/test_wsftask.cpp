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

#include "WsfTask.hpp"

// Test equivalence operator for WsfTask. Both task id and assigner processor should be compared.
TEST(WsfTask, Equivalence)
{
   WsfTask task1;
   task1.mTaskId                = 1;
   task1.mAssignerProcessorName = "assigner1";

   WsfTask task2;
   task2.mTaskId = 1;
   task2.SetAssigner(task1);
   EXPECT_EQ(task1, task2); // Same task id and assigner, should be equal

   task2.mAssignerProcessorName = "assigner2";
   EXPECT_NE(task1, task2); // Same task id, different assigner, should be not equal

   task2.SetAssigner(task1);
   ++task2.mTaskId;
   EXPECT_NE(task1, task2); // Same assigner, different task ids, should be not equal
}
