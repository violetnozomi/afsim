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

#include <sstream>
#include <string>

#include <gtest/gtest.h>

#include "UtInput.hpp"
#include "Utils.hpp"
#include "WsfCoverageAsset.hpp"
#include "WsfCoverageIntervalConstraint.hpp"

TEST(WsfCoverageIntervalConsraint, ProcessInput)
{
   wsf::coverage::IntervalConstraint ic{};

   TestEndOfDataThrow(ic, "output_file");

   TestEndOfDataThrow(ic, "platform");
   TestEndOfDataThrow(ic, "platform name");
   TestEndOfDataThrow(ic, "platform name minimum");
   TestBadValueThrow(ic, "platform name unknown");
   TestBadValueThrow(ic, "platform name minimum 7 km");
   TestBadValueThrow(ic, "platform name minimum -10 min");
   TestBadValueThrow(ic, "platform name minimum 0 s");
   TestEndOfDataThrow(ic, "platform name maximum");
   TestBadValueThrow(ic, "platform name maximum 7 km");
   TestBadValueThrow(ic, "platform name maximum -10 hr");
   TestBadValueThrow(ic, "platform name maximum 0 s");
   TestEndOfDataThrow(ic, "platform name interval");
   TestEndOfDataThrow(ic, "platform name interval 1 min");
   TestBadValueThrow(ic, "platform name interval -10 s");
   TestBadValueThrow(ic, "platform name interval 0 s");
   TestBadValueThrow(ic, "platform name interval 10 min 10 s");
   TestBadValueThrow(ic, "platform name interval 10 s 10 s");

   TestEndOfDataThrow(ic, "platform_type");
   TestEndOfDataThrow(ic, "platform_type TYPE");
   TestEndOfDataThrow(ic, "platform_type TYPE minimum");
   TestBadValueThrow(ic, "platform_type TYPE unknown");
   TestBadValueThrow(ic, "platform_type TYPE minimum 7 km");
   TestBadValueThrow(ic, "platform_type TYPE minimum -10 min");
   TestBadValueThrow(ic, "platform_type TYPE minimum 0 s");
   TestEndOfDataThrow(ic, "platform_type TYPE maximum");
   TestBadValueThrow(ic, "platform_type TYPE maximum 7 km");
   TestBadValueThrow(ic, "platform_type TYPE maximum -10 hr");
   TestBadValueThrow(ic, "platform_type TYPE maximum 0 s");
   TestEndOfDataThrow(ic, "platform_type TYPE interval");
   TestEndOfDataThrow(ic, "platform_type TYPE interval 1 min");
   TestBadValueThrow(ic, "platform_type TYPE interval -10 s");
   TestBadValueThrow(ic, "platform_type TYPE interval 0 s");
   TestBadValueThrow(ic, "platform_type TYPE interval 10 min 10 s");
   TestBadValueThrow(ic, "platform_type TYPE interval 10 s 10 s");

   TestEndOfDataThrow(ic, "device");
   TestEndOfDataThrow(ic, "device name");
   TestEndOfDataThrow(ic, "device name minimum");
   TestBadValueThrow(ic, "device name unknown");
   TestBadValueThrow(ic, "device name minimum 7 km");
   TestBadValueThrow(ic, "device name minimum -10 min");
   TestBadValueThrow(ic, "device name minimum 0 s");
   TestEndOfDataThrow(ic, "device name maximum");
   TestBadValueThrow(ic, "device name maximum 7 km");
   TestBadValueThrow(ic, "device name maximum -10 hr");
   TestBadValueThrow(ic, "device name maximum 0 s");
   TestEndOfDataThrow(ic, "device name interval");
   TestEndOfDataThrow(ic, "device name interval 1 min");
   TestBadValueThrow(ic, "device name interval -10 s");
   TestBadValueThrow(ic, "device name interval 0 s");
   TestBadValueThrow(ic, "device name interval 10 min 10 s");
   TestBadValueThrow(ic, "device name interval 10 s 10 s");

   TestEndOfDataThrow(ic, "device_type");
   TestEndOfDataThrow(ic, "device_type TYPE");
   TestEndOfDataThrow(ic, "device_type TYPE minimum");
   TestBadValueThrow(ic, "device_type TYPE unknown");
   TestBadValueThrow(ic, "device_type TYPE minimum 7 km");
   TestBadValueThrow(ic, "device_type TYPE minimum -10 min");
   TestBadValueThrow(ic, "device_type TYPE minimum 0 s");
   TestEndOfDataThrow(ic, "device_type TYPE maximum");
   TestBadValueThrow(ic, "device_type TYPE maximum 7 km");
   TestBadValueThrow(ic, "device_type TYPE maximum -10 hr");
   TestBadValueThrow(ic, "device_type TYPE maximum 0 s");
   TestEndOfDataThrow(ic, "device_type TYPE interval");
   TestEndOfDataThrow(ic, "device_type TYPE interval 1 min");
   TestBadValueThrow(ic, "device_type TYPE interval -10 s");
   TestBadValueThrow(ic, "device_type TYPE interval 0 s");
   TestBadValueThrow(ic, "device_type TYPE interval 10 min 10 s");
   TestBadValueThrow(ic, "device_type TYPE interval 10 s 10 s");

   TestFalseReturn(ic, "unknown");
}

TEST(WsfCoverageIntervalConstraint, OutputFileName)
{
   std::string expectedFileName = "FileName.csv";

   std::stringstream stream{};
   stream << "output_file " << expectedFileName;
   UtInput input{};
   input.PushInputString(stream.str());

   wsf::coverage::IntervalConstraint ic{};

   std::string command;
   input.ReadCommand(command);
   EXPECT_TRUE(ic.ProcessInput(input));
   EXPECT_EQ(ic.GetOutputFileName(), expectedFileName);
}

TEST(WsfCoverageIntervalConstraint, PlatformFiltering)
{
   // NOTE: We can only test 'platform' or 'device' filters here.
   //   The other two require a simulation that can be queried to
   //   see if the platform or devices have the correct types.

   // Setup three kinds of filters
   std::stringstream stream{};
   stream << "platform alice minimum 10 s\n";
   stream << "platform bob maximum 100 s\n";
   stream << "platform charlie interval 10 s 100 s\n";
   UtInput input{};
   input.PushInputString(stream.str());

   wsf::coverage::IntervalConstraint ic{};
   std::string                       command;
   input.ReadCommand(command);
   EXPECT_TRUE(ic.ProcessInput(input));
   input.ReadCommand(command);
   EXPECT_TRUE(ic.ProcessInput(input));
   input.ReadCommand(command);
   EXPECT_TRUE(ic.ProcessInput(input));

   // Create assets referred to above, and a grid asset.
   wsf::coverage::FreeAsset alice{"alice", "eye"};
   wsf::coverage::FreeAsset bob{"bob", "jay"};
   wsf::coverage::FreeAsset charlie{"charlie", "kay"};
   wsf::coverage::GridAsset target{"target", 0, "none"};

   // Create access intervals that will be sometimes subject to the filters defined
   wsf::coverage::AccessInterval small{0.0, 9.0};
   wsf::coverage::AccessInterval medium{0.0, 40.0};
   wsf::coverage::AccessInterval large{0.0, 200.0};

   // Test each combination
   EXPECT_TRUE(ic.FilterInterval(target, alice, small));
   EXPECT_FALSE(ic.FilterInterval(target, alice, medium));
   EXPECT_FALSE(ic.FilterInterval(target, alice, large));

   EXPECT_FALSE(ic.FilterInterval(target, bob, small));
   EXPECT_FALSE(ic.FilterInterval(target, bob, medium));
   EXPECT_TRUE(ic.FilterInterval(target, bob, large));

   EXPECT_TRUE(ic.FilterInterval(target, charlie, small));
   EXPECT_FALSE(ic.FilterInterval(target, charlie, medium));
   EXPECT_TRUE(ic.FilterInterval(target, charlie, large));
}

TEST(WsfCoverageIntervalConstraint, DeviceFiltering)
{
   // NOTE: We can only test 'platform' or 'device' filters here.
   //   The other two require a simulation that can be queried to
   //   see if the platform or devices have the correct types.

   // Setup three kinds of filters
   std::stringstream stream{};
   stream << "device eye minimum 10 s\n";
   stream << "device jay maximum 100 s\n";
   stream << "device kay interval 10 s 100 s\n";
   UtInput input{};
   input.PushInputString(stream.str());

   wsf::coverage::IntervalConstraint ic{};
   std::string                       command;
   input.ReadCommand(command);
   EXPECT_TRUE(ic.ProcessInput(input));
   input.ReadCommand(command);
   EXPECT_TRUE(ic.ProcessInput(input));
   input.ReadCommand(command);
   EXPECT_TRUE(ic.ProcessInput(input));

   // Create assets referred to above, and a grid asset.
   wsf::coverage::FreeAsset eye{"alice", "eye"};
   wsf::coverage::FreeAsset jay{"bob", "jay"};
   wsf::coverage::FreeAsset kay{"charlie", "kay"};
   wsf::coverage::GridAsset target{"target", 0, "none"};

   // Create access intervals that will be sometimes subject to the filters defined
   wsf::coverage::AccessInterval small{0.0, 9.0};
   wsf::coverage::AccessInterval medium{0.0, 40.0};
   wsf::coverage::AccessInterval large{0.0, 200.0};

   // Test each combination
   EXPECT_TRUE(ic.FilterInterval(target, eye, small));
   EXPECT_FALSE(ic.FilterInterval(target, eye, medium));
   EXPECT_FALSE(ic.FilterInterval(target, eye, large));

   EXPECT_FALSE(ic.FilterInterval(target, jay, small));
   EXPECT_FALSE(ic.FilterInterval(target, jay, medium));
   EXPECT_TRUE(ic.FilterInterval(target, jay, large));

   EXPECT_TRUE(ic.FilterInterval(target, kay, small));
   EXPECT_FALSE(ic.FilterInterval(target, kay, medium));
   EXPECT_TRUE(ic.FilterInterval(target, kay, large));
}
