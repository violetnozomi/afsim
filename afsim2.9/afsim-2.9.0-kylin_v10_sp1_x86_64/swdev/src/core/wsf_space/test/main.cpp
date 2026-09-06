// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2022 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <gtest/gtest.h>

#include "SpaceTestService.hpp"
#include "UtLocator.hpp"
#include "UtMemory.hpp"

int main(int argc, char* argv[])
{
   ::testing::InitGoogleTest(&argc, argv);
   auto testDataService = std::make_shared<SpaceTestService>();
   ut::Locator<SpaceTestService>::provide(testDataService);
   return RUN_ALL_TESTS();
}
