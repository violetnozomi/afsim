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

#include "profiling_test_dummy_library_export.h"

#include <cstdint>

#include "ProfilingCommon.hpp"

extern "C" PROFILING_TEST_DUMMY_LIBRARY_EXPORT uint64_t AfsimProfilingInterfaceVersion()
{
   // ProfilingSystem enforces version compliance
   return profiling::PackVersionInfo(0, 1, 0, 0);
}

extern "C" PROFILING_TEST_DUMMY_LIBRARY_EXPORT int32_t AfsimProfilingInitialize(const char* aOutputPath)
{
   return 0; // must return 0 for "OK"
}

extern "C" PROFILING_TEST_DUMMY_LIBRARY_EXPORT void AfsimProfilingBeginRegion(const char* aRegionName) {}

extern "C" PROFILING_TEST_DUMMY_LIBRARY_EXPORT void AfsimProfilingEndRegion() {}

extern "C" PROFILING_TEST_DUMMY_LIBRARY_EXPORT uint32_t AfsimProfilingCreateSection(const char label[])
{
   return 0xDA5DA5; // return a distinct number to allow us to verify we found and called this lib.
}

extern "C" PROFILING_TEST_DUMMY_LIBRARY_EXPORT void AfsimProfilingDestroySection(uint32_t sectionId) {}

extern "C" PROFILING_TEST_DUMMY_LIBRARY_EXPORT void AfsimProfilingBeginSection(uint32_t sectionId) {}

extern "C" PROFILING_TEST_DUMMY_LIBRARY_EXPORT void AfsimProfilingEndSection(uint32_t sectionId) {}

extern "C" PROFILING_TEST_DUMMY_LIBRARY_EXPORT void AfsimProfilingFinalize() {}
