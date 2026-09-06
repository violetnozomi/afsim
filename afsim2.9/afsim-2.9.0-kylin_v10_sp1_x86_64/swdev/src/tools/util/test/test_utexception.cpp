// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <vector>

#include <gtest/gtest.h>

#include "UtException.hpp"

#ifdef PROMOTE_HARDWARE_EXCEPTIONS

static void WriteToNull()
{
   std::vector<int> preventFunctionInlining{1, 2, 3, 4, 5};
   int* volatile ptr = nullptr;
   *ptr              = 0;
}

static void ReadFromNull()
{
   std::vector<int> preventFunctionInlining{1, 2, 3, 4, 5};
   int* volatile ptr = nullptr;
   volatile int x    = *ptr;
}

static void IntDivideByZero()
{
   std::vector<int> preventFunctionInlining{1, 2, 3, 4, 5};
   volatile int     x = 0;
   volatile int     y = 100 / x;
}

TEST(UtHardwareException, WriteToNull)
{
   ut::PromoteHardwareExceptions(true);
   EXPECT_THROW(::WriteToNull(), ut::HardwareException);
}

TEST(UtHardwareException, ReadFromNull)
{
   ut::PromoteHardwareExceptions(true);
   EXPECT_THROW(::ReadFromNull(), ut::HardwareException);
}

TEST(UtHardwareException, IntDivideByZero)
{
   ut::PromoteHardwareExceptions(true);
   EXPECT_THROW(::IntDivideByZero(), ut::HardwareException);
}

#endif
