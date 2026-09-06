// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <string>

#include <gtest/gtest.h>

#include "TestWriter.hpp"
#include "UtCalendar.hpp"
#include "WsfCoverageRawDataOutput.hpp"

namespace // {anonymous}
{

constexpr static const char* cRAWDATA_OUTPUT =
   "    Grid Asset ID, Grid Asset Device,   Free Asset Name, Free Asset Device,              Start Epoch,"
   "                End Epoch,         Start [s],           End [s],       Duration [s]\n"
   "                1,              none,         asset_one,        device_one, 2000-01-02T03:04:06.000Z,"
   " 2000-01-02T03:04:07.000Z,          1.000000,          2.000000,           1.000000\n"
   "                2,              none,         asset_one,        device_one, 2000-01-02T03:04:15.000Z,"
   " 2000-01-02T03:04:25.000Z,         10.000000,         20.000000,          10.000000\n"
   "                3,              none,         asset_two,        device_two, 2000-01-02T03:04:16.000Z,"
   " 2000-01-02T03:04:26.000Z,         11.000000,         21.000000,          10.000000\n"
   "                4,              none,         asset_two,        device_two, 2000-01-02T03:05:45.000Z,"
   " 2000-01-02T03:07:25.000Z,        100.000000,        200.000000,         100.000000\n";
} // namespace

TEST(WsfCoverageRawDataOutput, TestOutput)
{
   std::string name{"test_file_name.csv"};
   std::string anotherName{"another_file_name.csv"};
   std::string dirName{"test/directory"};

   TestWriter writer{};

   wsf::coverage::RawDataOutput rdo{name};
   EXPECT_EQ(rdo.GetFileName(), name);
   EXPECT_EQ(rdo.GetNumIntervals(), 0u);

   rdo.SetFileName(anotherName);
   EXPECT_EQ(rdo.GetFileName(), anotherName);

   rdo.SetOutputDir(dirName);
   EXPECT_EQ(rdo.GetOutputDir(), dirName);

   // Add some intervals, then test the output
   rdo.AddInterval(wsf::coverage::GridAsset{"coverage", 1, "none"},
                   wsf::coverage::FreeAsset{"asset_one", "device_one"},
                   wsf::coverage::AccessInterval{1.0, 2.0});
   rdo.AddInterval(wsf::coverage::GridAsset{"coverage", 2, "none"},
                   wsf::coverage::FreeAsset{"asset_one", "device_one"},
                   wsf::coverage::AccessInterval{10.0, 20.0});
   rdo.AddInterval(wsf::coverage::GridAsset{"coverage", 3, "none"},
                   wsf::coverage::FreeAsset{"asset_two", "device_two"},
                   wsf::coverage::AccessInterval{11.0, 21.0});
   rdo.AddInterval(wsf::coverage::GridAsset{"coverage", 4, "none"},
                   wsf::coverage::FreeAsset{"asset_two", "device_two"},
                   wsf::coverage::AccessInterval{100.0, 200.0});
   EXPECT_EQ(rdo.GetNumIntervals(), 4u);

   UtCalendar epoch{};
   epoch.SetDate(2000, 1, 2);
   epoch.SetTime(3, 4, 5.0);
   rdo.Write(writer, epoch);
   EXPECT_EQ(writer.GetStreamContents(), cRAWDATA_OUTPUT);
}
