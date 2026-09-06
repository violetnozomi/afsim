// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <gtest/gtest.h>

#include <fstream>
#include <string>

#include "Configuration.hpp"
#include "DSV_Output.hpp"
#include "Options.hpp"
#include "UtLocator.hpp"
#include "UtPath.hpp"
#include "UtTestDataService.hpp"

class DSVReportTest : public ::testing::Test
{
   protected:
      void TearDown() override
      {
         for(const auto& path : mTempFiles)
         {
            UtPath(path).RemoveFile();
         }
         for(const auto& path : mTempDirs)
         {
            UtPath(path).RemoveTree();
         }
      }
      void AddTempFile(const std::string& path)
      {
         mTempFiles.push_back(path);
      }
      void AddTempDir(const std::string& path)
      {
         mTempDirs.push_back(path);
      }

   private:
      std::vector<std::string> mTempFiles;
      std::vector<std::string> mTempDirs;

};

std::vector<std::string> readFile(const std::string& aFile)
{
   std::vector<std::string> result;
   std::ifstream in(aFile);
   std::string line;
   while(std::getline(in, line, '\n'))
   {
      result.push_back(line);
   }
   return result;
}

void writeFile(const std::string& aFile, const std::vector<std::string>& aContent)
{
   std::ofstream out(aFile);
   for(const auto& line : aContent)
   {
      out << line << std::endl;
   }
}

void CheckFileContentsEqual(const std::string& expected, const std::string& observed)
{
   auto expectedContent = readFile(expected);
   auto observedContent = readFile(observed);
   ASSERT_EQ(expectedContent.size(), observedContent.size());
   for(size_t i = 0; i < expectedContent.size(); ++i) {
      EXPECT_EQ(expectedContent[i], observedContent[i]) << "Mismatch line in " << observed << ": " << i;
   }
}

TEST_F(DSVReportTest, FullRun)
{
   auto& dataService = ut::Locator<ut::TestDataService>::get();
   const std::vector<std::string> dsvConfig = {
      "# post_processor configuration input file",
      "# post_processor - c <path - to - this - file>",
      "report DSV",
      "report_name test_report",
      std::string("data_file ") + dataService.getResource("comm.csv").GetNormalizedPath(),
      "output_directory ./run",
      "monte_carlo_start 1",
      "monte_carlo_end 999999",
      "single_output_file false",
      "write_header_information true",
      "delimiter comma",
      "lat_lon_format d:m:s.2",
      "length_units meters",
      "angle_units degrees",
      "track_number_options numerical",
      "start_time 0 seconds",
      "end_time 2400 seconds",
      "interval 1 seconds",
      "time_format s.5"
   };
   // Write temp config file:
   writeFile("test_dsv_config.txt", dsvConfig);
   AddTempFile("test_dsv_config.txt");
   AddTempDir("run");
   Configuration config(std::string("test_dsv_config.txt"));
   ASSERT_TRUE(config.Execute());
   ASSERT_TRUE(UtPath("run").Exists());
   ASSERT_TRUE(UtPath("run").IsDirectory());
   ASSERT_TRUE(UtPath("run/test_report/COMM_FREQUENCY_CHANGED.csv").Exists());
   ASSERT_TRUE(UtPath("run/test_report/COMM_STATUS.csv").Exists());
   CheckFileContentsEqual(dataService.getResource("expected/COMM_FREQUENCY_CHANGED.csv").GetNormalizedPath(), "run/test_report/COMM_FREQUENCY_CHANGED.csv");
   CheckFileContentsEqual(dataService.getResource("expected/COMM_STATUS.csv").GetNormalizedPath(), "run/test_report/COMM_STATUS.csv");
}

TEST_F(DSVReportTest, FullRunMC)
{
   auto& dataService = ut::Locator<ut::TestDataService>::get();
   auto inputPath = dataService.getResource("").GetNormalizedPath();
   std::vector<std::string> dsvConfig = {
      "# post_processor configuration input file",
      "# post_processor - c <path - to - this - file>",
      "report DSV",
      "report_name test_mc",
      std::string("data_file ") + inputPath + "/comm_%d.csv",
      "output_directory ./run_mc_%d",
      "monte_carlo_start 1",
      "monte_carlo_end 999999",
      "use_monte_carlo true",
      "single_output_file false",
      "write_header_information true",
      "delimiter comma",
      "lat_lon_format d:m:s.2",
      "length_units meters",
      "angle_units degrees",
      "track_number_options numerical",
      "start_time 0 seconds",
      "end_time 2400 seconds",
      "interval 1 seconds",
      "time_format s.5"
   };
   writeFile("test_dsv_mc_config.txt", dsvConfig);
   AddTempFile("test_dsv_mc_config.txt");
   AddTempDir("./run_mc_1");
   AddTempDir("./run_mc_2");
   Configuration config(std::string("test_dsv_mc_config.txt"));
   ASSERT_TRUE(config.Execute());
   ASSERT_TRUE(UtPath("run_mc_1").Exists());
   ASSERT_TRUE(UtPath("run_mc_2").Exists());
   ASSERT_TRUE(UtPath("run_mc_1").IsDirectory());
   ASSERT_TRUE(UtPath("run_mc_2").IsDirectory());

   ASSERT_TRUE(UtPath("run_mc_1/test_mc/COMM_FREQUENCY_CHANGED.csv").Exists());
   ASSERT_TRUE(UtPath("run_mc_1/test_mc/COMM_STATUS.csv").Exists());
   CheckFileContentsEqual(dataService.getResource("expected_mc/run_mc_1/COMM_FREQUENCY_CHANGED.csv").GetNormalizedPath(), "run_mc_1/test_mc/COMM_FREQUENCY_CHANGED.csv");
   CheckFileContentsEqual(dataService.getResource("expected_mc/run_mc_1/COMM_STATUS.csv").GetNormalizedPath(), "run_mc_1/test_mc/COMM_STATUS.csv");

   ASSERT_TRUE(UtPath("run_mc_2/test_mc/COMM_FREQUENCY_CHANGED.csv").Exists());
   ASSERT_TRUE(UtPath("run_mc_2/test_mc/COMM_STATUS.csv").Exists());
   CheckFileContentsEqual(dataService.getResource("expected_mc/run_mc_2/COMM_FREQUENCY_CHANGED.csv").GetNormalizedPath(), "run_mc_2/test_mc/COMM_FREQUENCY_CHANGED.csv");
   CheckFileContentsEqual(dataService.getResource("expected_mc/run_mc_2/COMM_STATUS.csv").GetNormalizedPath(), "run_mc_2/test_mc/COMM_STATUS.csv");
}
