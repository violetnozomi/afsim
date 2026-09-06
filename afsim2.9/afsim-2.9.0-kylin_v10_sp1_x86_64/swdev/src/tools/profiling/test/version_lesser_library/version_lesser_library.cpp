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

#include "profiling_test_version_lesser_library_export.h"

#include <cinttypes>
#include <cstdint>
#include <random>

#include "ProfilingCommon.hpp"

// This implementation intentionally does not contain any profiling hooks except the interface version.
// This helps ensure that version is checked before any other usage of the library.

extern "C" PROFILING_TEST_VERSION_LESSER_LIBRARY_EXPORT uint64_t AfsimProfilingInterfaceVersion()
{
   using int16 = std::uint16_t;
   // Use quasi-fuzz testing by generation a version lesser in various values than current assumed profiling system.
   std::mt19937 rand(std::random_device{}());

   // TODO: It'd be nice if these values could be deterministically obtained instead of require updating
   //       when the actual profiling system version changes.
   constexpr int16 assumedActualEpoch        = 0;
   constexpr int16 assumedActualMajorVersion = 1;
   constexpr int16 assumedActualMinorVersion = 0;

   std::uniform_int_distribution<int16> epochRange(0, assumedActualEpoch);
   const int16                          testEpoch = epochRange(rand);

   static_assert(assumedActualMajorVersion >= 1, "No profiling test support for major version of zero");
   const int16 maxMajor = testEpoch == assumedActualEpoch ? assumedActualMajorVersion - 1 : 20;

   std::uniform_int_distribution<int16> majorRange(0, maxMajor);
   std::uniform_int_distribution<int16> anyVer(0, 20);

   return profiling::PackVersionInfo(testEpoch, majorRange(rand), anyVer(rand), anyVer(rand));
}
