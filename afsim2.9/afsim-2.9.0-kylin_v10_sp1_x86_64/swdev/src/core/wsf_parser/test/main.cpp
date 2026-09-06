// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <iostream>

#include <gtest/gtest.h>

#include "UnitTestFileHelper.h"

int main(int argc, char* argv[])
{
   ::testing::InitGoogleTest(&argc, argv);
   if (argc < 2)
   {
      std::cerr << "No project root directory supplied" << std::endl;
      return 1;
   }
   // Initialize the file path instance with the input root directory
   UnitTestFilePathHelper::Instance(argv[1]);
   return RUN_ALL_TESTS();
}
