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

#include <cmath>
#include <limits>
#include <type_traits>

#include <gtest/gtest.h>

#include "WsfClockSource.hpp"
#include "WsfRealTimeClockSource.hpp"

namespace
{
// Common tests for WsfClockSource and WsfRealTimeClockSource.
template<class ClockSourceType>
void testClockSourceInterface()
{
   // Test that ClockSourceType is default constructible.
   ClockSourceType clockSource;

   // We only assume C++11; the "_v" constants are C++14 additions.
   static_assert(!std::is_copy_assignable<ClockSourceType>::value, "Copy assignment is supposed to be forbidden.");
   static_assert(!std::is_copy_constructible<ClockSourceType>::value, "Copy construction is supposed to be forbidden.");

   // Test that invoking SetClockRate with a (possibly nondefault) clock rate
   // results in GetClockRate returning the value given to SetClockRate.
   constexpr double fake_pi = 3.14;
   clockSource.SetClockRate(fake_pi);
   ASSERT_EQ(clockSource.GetClockRate(), fake_pi);

   // Test that SetMaximumClock is a method that takes one argument convertible from double.
   // We're not testing an invariant here; the point is to warn if the current public interface
   // of WsfClockSource changes.
   constexpr double maxClock = 100.0;
   clockSource.SetMaximumClock(maxClock);

   // Test that ResetClock is a method that takes zero or one arguments,
   // and that the latter's argument is convertible from double.
   // We're not testing an invariant here; the point is to warn if the current public interface
   // of WsfClockSource changes.
   clockSource.ResetClock();
   constexpr double accumulatedTime = 3.0;
   clockSource.ResetClock(accumulatedTime);

   // Test that calling StopClock() on a started clock results in IsStopped() returning true.
   bool isStopped = clockSource.IsStopped();
   if (isStopped)
   {
      // The interface does not promise whether the clock is initially stopped or not.
      // We also don't know if we are permitted to call StartClock() on an already started clock,
      // or StopClock() on an already stopped clock.
      clockSource.StartClock();
   }
   isStopped = clockSource.IsStopped();
   ASSERT_FALSE(isStopped);

   // Test that calling StartClock() on a stopped clock results on IsStopped() returning false.
   clockSource.StopClock();
   isStopped = clockSource.IsStopped();
   ASSERT_TRUE(isStopped);
   clockSource.StartClock();
   isStopped = clockSource.IsStopped();
   ASSERT_FALSE(isStopped);

   // Test that calling SetClock(value) on a stopped clock,
   // with value greater than or equal to the result of GetClock(Inf),
   // will make the result of the next call to GetClock(Inf) be greater than or equal to value.
   // (We're still only halfway done with the above test.)
   ClockSourceType clockSource2;
   isStopped = clockSource2.IsStopped();
   if (!isStopped)
   {
      clockSource2.StopClock();
   }
   constexpr double Inf               = std::numeric_limits<double>::infinity();
   const double     currentClockValue = clockSource2.GetClock(Inf);
   const double     newClockValue2    = std::nextafter(currentClockValue, Inf);
   clockSource2.SetClock(newClockValue2);
   const double currentClockValue2 = clockSource2.GetClock(Inf);
   ASSERT_GE(currentClockValue2, currentClockValue);
}
} // namespace

TEST(ClockSource, Interface)
{
   testClockSourceInterface<WsfClockSource>();
}

TEST(RealTimeClockSource, BaseClassInterface)
{
   testClockSourceInterface<WsfRealTimeClockSource>();
   // WsfSimulation keeps the clock source as a pointer to WsfClockSource,
   // and it expects this to be able to accept either an instance of WsfClockSource
   // or an instance of WsfRealTimeClockSource.  This is why we test inheritance.
   static_assert(std::is_base_of<WsfClockSource, WsfRealTimeClockSource>::value,
                 "WsfClockSource must be a base class of WsfRealTimeClockSource.");
}
