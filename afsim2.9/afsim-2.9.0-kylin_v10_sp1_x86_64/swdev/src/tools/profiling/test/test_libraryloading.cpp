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

#include <gtest/gtest.h>

#include "ProfilingCommon.hpp"
#include "ProfilingMacros.hpp"
#include "ProfilingRegion.hpp"
#include "ProfilingSystem.hpp"
#include "UtBuildInfo.hpp"

namespace
{

const std::string unusedLogFile = "unused-log-filename.txt";
}

TEST(LibraryLoading, ImplicitAfperf)
{
   profiling::ProfilingSystemArguments args;
   EXPECT_NO_THROW(profiling::ProfilingSystem profSystem(args));
}

TEST(LibraryLoading, AlternateLibrary)
{
   profiling::ProfilingSystemArguments args;


   args.mProfilingLibraryPath = ut::BuildInfo::GetSharedLibraryFilePath("profiling_test_dummy_library");
   args.mProfilingOutputDest  = unusedLogFile;
   try
   {
      profiling::ProfilingSystem profSystem(args);

      // Verify we actually got the dummy library.
      EXPECT_EQ(0xDA5DA5, profiling::CreateProfilingSection("unused"));
   }
   catch (...)
   {
      GTEST_NONFATAL_FAILURE_("Expected no throw.  May need to build the dummy_library?");
   }
}

TEST(LibraryLoading, ExplicitAfperf)
{
   profiling::ProfilingSystemArguments args;
   args.mProfilingLibraryPath = ut::BuildInfo::GetSharedLibraryFilePath("afperf");
   args.mProfilingOutputDest  = unusedLogFile;
   EXPECT_NO_THROW(profiling::ProfilingSystem profSystem(args));
}

TEST(LibraryLoading, MissingLibrary)
{
   profiling::ProfilingSystemArguments args;
   args.mProfilingLibraryPath = ut::BuildInfo::GetSharedLibraryFilePath("this-doesnt-exist");
   args.mProfilingOutputDest  = unusedLogFile;
   EXPECT_THROW(profiling::ProfilingSystem profSystem(args), profiling::Exception);
}

TEST(LibraryLoading, MissingOutputFileArgument)
{
   profiling::ProfilingSystemArguments args;
   args.mProfilingLibraryPath = ut::BuildInfo::GetSharedLibraryFilePath("afperf");
   EXPECT_THROW(profiling::ProfilingSystem profSystem(args), profiling::Exception);
}

TEST(LibraryVersionCheck, IncompatVersionLesser)
{
   profiling::ProfilingSystemArguments args;
   args.mProfilingLibraryPath = ut::BuildInfo::GetSharedLibraryFilePath("profiling_test_version_lesser_library");
   args.mProfilingOutputDest  = unusedLogFile;
   EXPECT_THROW(profiling::ProfilingSystem profSystem(args), profiling::Exception);
}

TEST(LibraryVersionCheck, IncompatVersionGreater)
{
   profiling::ProfilingSystemArguments args;
   args.mProfilingLibraryPath = ut::BuildInfo::GetSharedLibraryFilePath("profiling_test_version_greater_library");
   args.mProfilingOutputDest  = unusedLogFile;
   EXPECT_THROW(profiling::ProfilingSystem profSystem(args), profiling::Exception);
}
