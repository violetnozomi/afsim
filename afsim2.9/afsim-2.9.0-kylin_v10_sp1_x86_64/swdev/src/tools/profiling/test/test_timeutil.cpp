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

#include "ProfilingTimeUtil.hpp"

namespace
{
TEST(ProfilingTimeUtil, DurationUnitString)
{
   using profiling::DurationUnitString;

   ASSERT_EQ(DurationUnitString(std::nano{}), "ns");
   ASSERT_EQ(DurationUnitString(std::micro{}), "μs");
   ASSERT_EQ(DurationUnitString(std::milli{}), "ms");
   ASSERT_EQ(DurationUnitString(std::ratio<1, 1>{}), "s");

   // We don't really care what it prints for weird ratios,
   // just that it doesn't throw and that it prints _something_.
   {
      std::string result;
      auto        doTest = [&result]() { result = DurationUnitString(std::ratio<2, 3>{}); };
      ASSERT_NO_THROW(doTest());
      ASSERT_TRUE(result != "");
   }
   {
      std::string result;
      auto        doTest = [&result]() { result = DurationUnitString(std::ratio<1, 2>{}); };
      ASSERT_NO_THROW(doTest());
      ASSERT_TRUE(result != "");
   }
   {
      std::string result;
      auto        doTest = [&result]() { result = DurationUnitString(std::ratio<5, 1>{}); };
      ASSERT_NO_THROW(doTest());
      ASSERT_TRUE(result != "");
   }
}

TEST(ProfilingTimeUtil, TimeString)
{
   using profiling::TimeString;

   const auto now = std::chrono::system_clock::now();

   // The string should have the following format:
   //
   // 2021-12-29 09:27:16
   //
   // We don't need to test the values, just the format.
   const std::string     stringTimeRep = TimeString(now);
   constexpr std::size_t expectedSize(19);
   ASSERT_EQ(stringTimeRep.size(), expectedSize);

   constexpr std::size_t yearStart  = 0;
   constexpr std::size_t yearLen    = 4;
   constexpr std::size_t monthStart = yearStart + yearLen + 1;
   constexpr std::size_t monthLen   = 2;
   constexpr std::size_t dayStart   = monthStart + monthLen + 1;
   constexpr std::size_t dayLen     = 2;
   constexpr std::size_t hourStart  = dayStart + dayLen + 1;
   constexpr std::size_t hourLen    = 2;
   constexpr std::size_t minStart   = hourStart + hourLen + 1;
   constexpr std::size_t minLen     = 2;
   constexpr std::size_t secStart   = minStart + minLen + 1;
   constexpr std::size_t secLen     = 2;
   static_assert(secStart + secLen == expectedSize, "Bug in test");

   {
      const std::string yearStr = stringTimeRep.substr(yearStart, yearLen);
      int               yearVal = 0;
      ASSERT_NO_THROW(yearVal = std::stoi(yearStr));

      // Different systems may have their clocks set differently,
      // but Windows' epoch is 1601 and Unix's is 1970.
      constexpr int minYearEpoch = 1601;
      ASSERT_TRUE(yearVal >= minYearEpoch);
   }
   {
      const std::string monthStr = stringTimeRep.substr(monthStart, monthLen);
      int               monthVal = 0;
      ASSERT_NO_THROW(monthVal = std::stoi(monthStr));
      ASSERT_TRUE(monthVal >= 1);
      ASSERT_TRUE(monthVal <= 12);
   }
   {
      const std::string dayStr = stringTimeRep.substr(dayStart, dayLen);
      int               dayVal = 0;
      ASSERT_NO_THROW(dayVal = std::stoi(dayStr));
      ASSERT_TRUE(dayVal >= 1);
      ASSERT_TRUE(dayVal <= 31);
   }
   {
      const std::string hourStr = stringTimeRep.substr(hourStart, hourLen);
      int               hourVal = 0;
      ASSERT_NO_THROW(hourVal = std::stoi(hourStr));
      ASSERT_TRUE(hourVal >= 0);
      ASSERT_TRUE(hourVal <= 23);
   }
   {
      const std::string minStr = stringTimeRep.substr(minStart, minLen);
      int               minVal = 0;
      ASSERT_NO_THROW(minVal = std::stoi(minStr));
      ASSERT_TRUE(minVal >= 0);
      ASSERT_TRUE(minVal <= 59);
   }
   {
      const std::string secStr = stringTimeRep.substr(secStart, secLen);
      int               secVal = 0;
      ASSERT_NO_THROW(secVal = std::stoi(secStr));
      ASSERT_TRUE(secVal >= 0);
      ASSERT_TRUE(secVal <= 59);
   }
}

} // namespace
