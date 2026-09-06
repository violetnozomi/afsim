// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright (C) 2021 Stellar Science; U.S. Government has Unlimited Rights.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <string>

#include <gtest/gtest.h>

#include "ProfilingCommon.hpp"
#include "ProfilingRegion.hpp"
#include "ProfilingSystem.hpp"
#include "UtPath.hpp"

namespace
{
constexpr const char cREGION_TEST_FILENAME[]  = "ProfilingRegion-DefaultOutput-unit-test-output.txt";
constexpr const char cSECTION_TEST_FILENAME[] = "ProfilingSection-DefaultOutput-unit-test-output.txt";

void demoFunction2()
{
   profiling::ProfilingRegion innerRegion1("demoFunction2");
}

void demoFunction1()
{
   profiling::ProfilingRegion innerRegion1("demoFunction1");
   {
      profiling::ProfilingRegion innerRegion1("inner1 demoFunction1");
      demoFunction2();
   }
   {
      profiling::ProfilingRegion innerRegion1("inner2 demoFunction1");
      demoFunction2();
   }
}
} // namespace

TEST(ProfilingRegion, DefaultOutput)
{
   const UtPath outFilePath{cREGION_TEST_FILENAME};
   { // Remove output file from previous tests, to prevent false positives.
      if (outFilePath.Exists())
      {
         (void)outFilePath.RemoveFile();
      }
   }

   {
      profiling::ProfilingSystemArguments args;
      const auto                          outFilePath = std::string("file:///") + cREGION_TEST_FILENAME;
      args.mProfilingOutputDest.emplace(outFilePath);
      profiling::ProfilingSystem profSystem(args);

      profiling::ProfilingRegion topLevelRegion("topLevelRegion");
      demoFunction1();
      {
         profiling::ProfilingRegion innerRegion("inner0");
         demoFunction1();
      }
   }

   EXPECT_TRUE(outFilePath.Exists());
   // Leave output file in place for human inspection.
}


TEST(ProfilingRegion, BadOutputURI)
{
   {
      profiling::ProfilingSystemArguments args;
      const auto                          outFilePath = std::string("file://") + cREGION_TEST_FILENAME;
      args.mProfilingOutputDest.emplace(outFilePath);

      EXPECT_THROW(profiling::ProfilingSystem profSystem(args), profiling::Exception);
   }
}

TEST(ProfilingSection, DefaultOutput)
{
   const UtPath outFilePath{cSECTION_TEST_FILENAME};
   { // Remove output file from previous tests, to prevent false positives.
      if (outFilePath.Exists())
      {
         (void)outFilePath.RemoveFile();
      }
   }

   {
      profiling::ProfilingSystemArguments args;
      const auto                          outFilePath = std::string("file:") + cSECTION_TEST_FILENAME;
      args.mProfilingOutputDest.emplace(outFilePath);
      profiling::ProfilingSystem profSystem(args);

      auto sectionId1 = profiling::CreateProfilingSection("Section1");
      profiling::BeginProfilingSection(sectionId1);

      // Regions and sections need to be able to coexist,
      // even though sections do not live on a stack like regions.
      demoFunction1();

      profiling::EndProfilingSection(sectionId1);
      profiling::DestroyProfilingSection(sectionId1);

      // Section labels need not be unique, but section IDs that exist at the same time must be unique.
      auto sectionId2a = profiling::CreateProfilingSection("Section2");
      auto sectionId2b = profiling::CreateProfilingSection("Section2");

      EXPECT_NE(sectionId2a, sectionId2b);

      // Sections' begin/end can overlap.
      profiling::BeginProfilingSection(sectionId2a);
      demoFunction1();
      profiling::BeginProfilingSection(sectionId2b);
      profiling::EndProfilingSection(sectionId2a);
      demoFunction1();
      profiling::EndProfilingSection(sectionId2b);

      // Sections can be begun and ended multiple times.
      profiling::BeginProfilingSection(sectionId2a);
      profiling::EndProfilingSection(sectionId2a);
      profiling::BeginProfilingSection(sectionId2a);
      profiling::EndProfilingSection(sectionId2a);

      // Section lifetimes don't have to be nested.
      profiling::DestroyProfilingSection(sectionId2b);
      profiling::DestroyProfilingSection(sectionId2a);
   }

   EXPECT_TRUE(outFilePath.Exists());
   // Leave output file in place for human inspection.
}
