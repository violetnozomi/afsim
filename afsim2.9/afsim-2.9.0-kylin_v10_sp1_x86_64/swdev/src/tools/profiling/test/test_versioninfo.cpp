// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright (C) 2022 Stellar Science; U.S. Government has Unlimited Rights.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <array>
#include <limits>

#include <gtest/gtest.h>

#include "ProfilingCommon.hpp"

namespace
{

TEST(ProfilingCommon, PackUnpackVersionInfo)
{
   using profiling::VersionInfo;
   static_assert(sizeof(VersionInfo) == sizeof(uint64_t), "VersionInfo has the wrong size");

   constexpr uint16_t versionValues[]{uint16_t(0),
                                      uint16_t(1),
                                      uint16_t(std::numeric_limits<uint8_t>::max()) + uint64_t(1),
                                      std::numeric_limits<uint16_t>::max()};
   for (uint16_t epoch : versionValues)
   {
      for (uint16_t major : versionValues)
      {
         for (uint16_t minor : versionValues)
         {
            for (uint16_t patch : versionValues)
            {
               const uint64_t               packed   = profiling::PackVersionInfo(epoch, major, minor, patch);
               const profiling::VersionInfo unpacked = profiling::UnpackVersionInfo(packed);
               ASSERT_EQ(unpacked.mEpoch, epoch);
               ASSERT_EQ(unpacked.mMajor, major);
               ASSERT_EQ(unpacked.mMinor, minor);
               ASSERT_EQ(unpacked.mPatch, patch);
            }
         }
      }
   }
}

} // namespace
