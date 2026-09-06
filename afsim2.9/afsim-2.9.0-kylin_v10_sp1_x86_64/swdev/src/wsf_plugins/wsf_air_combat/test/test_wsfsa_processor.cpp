//*****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
//*****************************************************************************

#include <vector>

#include "WsfSA_Processor.hpp"
#include "gtest/gtest.h"

TEST(WsfSA_Processor, RoundToBaseRateMultiple)
{
   // number to be rounded using a multiple
   std::vector<double> numToRound = {0.1, 1.0, 0.01, -1.0, 0.0, 1.51, 13.49};

   // number to be used as a multiple
   std::vector<double> multipleToUse = {0.1, 0.25, 1.0, 0.01, -1.0, 0.0};

   // expected values for each combination of numToRound and multipleToUse
   std::vector<std::vector<double>> roundedValues = {{0.1, 1.0, 0.0, 0.0, 0.0, 1.5, 13.5},
                                                     {0.0, 1.0, 0.0, 0.0, 0.0, 1.5, 13.5},
                                                     {0.0, 1.0, 0.0, 0.0, 0.0, 2.0, 13.0},
                                                     {0.1, 1.0, 0.01, 0.0, 0.0, 1.51, 13.49},
                                                     {0.1, 1.0, 0.01, 0.0, 0.0, 1.51, 13.49},
                                                     {0.1, 1.0, 0.01, 0.0, 0.0, 1.51, 13.49}};

   for (size_t a = 0; a < multipleToUse.size(); ++a)
   {
      for (size_t i = 0; i < numToRound.size(); ++i)
      {
         EXPECT_EQ(WsfSA_Processor::RoundToBaseRateMultiple(numToRound[i], multipleToUse[a]), roundedValues[a][i]);
      }
   }
}
