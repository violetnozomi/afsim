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

#include "UtInputFile.hpp"
#include "UtLocator.hpp"
#include "UtMemory.hpp"
#include "UtTestDataService.hpp"
#include "WsfOpticalPath.hpp"

TEST(WsfOpticalPath, Coefficients)
{
   const auto& dataService = ut::Locator<ut::TestDataService>::get();

   UtInput input;
   auto    loadType = [&](const std::string& filename)
   {
      auto inputFile = ut::make_unique<UtInputFile>(dataService.getResource(filename).GetNormalizedPath());
      input.PushInput(std::move(inputFile));
   };

   loadType("COEFS1000_1.txt");
   loadType("COEFS1064_1.txt");
   loadType("COEFS1064_2.txt");
   loadType("COEFS1064_3.txt");
   loadType("COEFS1064_4.txt");
   loadType("COEFS1064_5.txt");
   loadType("COEFS1315_1.txt");
   loadType("COEFS3800_1.txt");

   WsfOpticalPathCoefficientTypes types;
   while (true)
   {
      try
      {
         std::string command;
         if (!types.LoadType(input))
         {
            input.ReadCommand(command);
         }
      }
      catch (UtInput::EndOfData)
      {
         break;
      }
   }

   double range          = 10000.0;
   double targetHeight   = 6000.0;
   double platformHeight = 100.0;

   WsfOpticalPath path(types, 1064e-9, 2, 1);

   // Test1
   double transmission = path.GetAttenuation(range, targetHeight, platformHeight);
   EXPECT_NEAR(8.1588e-1, transmission, 0.001);

   // Test2
   range        = 20000.0;
   transmission = path.GetAttenuation(range, targetHeight, platformHeight);
   EXPECT_NEAR(6.6515e-1, transmission, 0.001);

   // Test3
   platformHeight = 6000;
   transmission   = path.GetAttenuation(range, targetHeight, platformHeight);
   EXPECT_NEAR(9.3877e-001, transmission, 0.001);

   // Test 4
   WsfOpticalPath path2(types, 1000e-9, 2, 1);
   transmission = path2.GetAttenuation(range, targetHeight, platformHeight);
   EXPECT_NEAR(9.3877e-001, transmission, 0.01);
}

TEST(WsfOpticalPath, COEFS)
{
   const auto& dataService = ut::Locator<ut::TestDataService>::get();
   auto        file = ut::make_unique<UtInputFile>(dataService.getResource("COEFS3800_1.txt").GetNormalizedPath());
   UtInput     input;
   input.PushInput(std::move(file));
   WsfOpticalPathCoefficientTypes types;
   try
   {
      types.LoadType(input);
   }
   catch (std::exception&)
   {
   }

   const auto& coeffs = types.GetCoefficients(3800.0e-9, 2, 1);
   EXPECT_EQ(101, coeffs.GetAltitudes().size());
   EXPECT_NEAR(4.20792079207953E+3, coeffs.GetAltitudes()[8], 0.01);
   EXPECT_DOUBLE_EQ(2.56667375710870e-6, coeffs.GetAttenuationCoefficients()[8]);
   EXPECT_DOUBLE_EQ(180.270454427785e-9, coeffs.GetScatteringCoefficients()[8]);
}
