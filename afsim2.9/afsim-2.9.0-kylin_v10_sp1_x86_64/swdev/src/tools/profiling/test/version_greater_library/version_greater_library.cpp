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

#include "profiling_test_version_greater_library_export.h"

#include <cinttypes>
#include <cstdint>
#include <random>

#include "ProfilingCommon.hpp"

// This implementation intentionally does not contain any profiling hooks except the interface version.
// This helps ensure that version is checked before any other usage of the library.

extern "C" PROFILING_TEST_VERSION_GREATER_LIBRARY_EXPORT uint64_t AfsimProfilingInterfaceVersion()
{
   using int16 = std::uint16_t;
   // Use quasi-fuzz testing by generation a version greater in various values than current assumed profiling system.
   std::mt19937 rand(std::random_device{}());

   // TODO: It'd be nice if these values could be deterministically obtained instead of require updating
   //       when the actual profiling system version changes.
   constexpr int16 assumedActualEpoch        = 0;
   constexpr int16 assumedActualMajorVersion = 1;
   constexpr int16 assumedActualMinorVersion = 0;

   // Only use a small range on epoch to frequently test major compatibility.
   std::uniform_int_distribution<int16> epochRange(assumedActualEpoch, assumedActualEpoch + 1);
   const int16                          testEpoch = epochRange(rand);

   const int16 minMajor = testEpoch == assumedActualEpoch ? assumedActualMajorVersion : 0;
   // Use a range from minMajor to ten later in order to:
   //   a) occasionally test minor version compat checking
   //   b) occasionally test two digit values
   std::uniform_int_distribution<int16> majorRange(minMajor, minMajor + 10);
   const int16                          testMajor = majorRange(rand);

   const int16 minMinor = testMajor == assumedActualMajorVersion ? assumedActualMinorVersion + 1 : 0;
   std::uniform_int_distribution<int16> minorRange(minMinor, minMinor + 20);

   std::uniform_int_distribution<int16> anyVer(0, 20);

   return profiling::PackVersionInfo(testEpoch, testMajor, minorRange(rand), anyVer(rand));
}
