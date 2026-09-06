// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfRealTimeClockSource.hpp"

#include <algorithm>
#include <cassert>

#include "UtLog.hpp"

// =================================================================================================
WsfRealTimeClockSource::WsfRealTimeClockSource()
   : mTimeAccumulated(0.0)
   , mQuietMode(true)
{
   ResetClock();
}

// =================================================================================================
// virtual
WsfRealTimeClockSource::~WsfRealTimeClockSource() {}

// =================================================================================================
// virtual
double WsfRealTimeClockSource::GetClock(double aClock) const
{
   double simulationClock = mTimeAccumulated;
   if (!mClockPaused)
   {
      simulationClock += mWallClock.GetClock() * mClockRate;
      if (simulationClock > mMaximumClock)
      {
         simulationClock = mMaximumClock;
      }
   }
   if (aClock < simulationClock)
   {
      simulationClock = aClock;
   }
   return simulationClock;
}

// =================================================================================================
// virtual
void WsfRealTimeClockSource::ResetClock(double aAccumulatedTime)
{
   if (!mQuietMode)
   {
      ut::log::debug() << "Clock reset.";
   }
   assert(mClockRate >= 0.0);
   mClockPaused     = true;
   mTimeAccumulated = aAccumulatedTime;
   mWallClock.ResetClock();
}

// =================================================================================================
// virtual
void WsfRealTimeClockSource::SetClock(double aClock)
{
   assert(aClock >= 0.0);
   if (!mClockPaused)
   {
      // Close out the current time interval and start a new one.
      mWallClock.ResetClock();
   }
   mTimeAccumulated = aClock;
   if (mTimeAccumulated > mMaximumClock)
   {
      mTimeAccumulated = mMaximumClock;
   }
   WsfClockSource::SetClock(aClock);
}

// =================================================================================================
// virtual
void WsfRealTimeClockSource::SetClockRate(double aClockRate)
{
   assert(aClockRate >= 0.0);
   if (!mClockPaused)
   {
      // Close out the current time interval and start a new one.
      mTimeAccumulated = GetClock(1.0E300);
      mWallClock.ResetClock();
   }
   WsfClockSource::SetClockRate(aClockRate);
}

// =================================================================================================
// virtual
void WsfRealTimeClockSource::StartClock()
{
   assert(mClockRate >= 0.0);
   if (mClockPaused)
   {
      if (!mQuietMode)
      {
         auto out = ut::log::debug() << "Clock started.";
         out.AddNote() << "T = " << mTimeAccumulated;
      }
      mClockPaused = false;
      mWallClock.ResetClock();
   }
}

// =================================================================================================
// virtual
void WsfRealTimeClockSource::StopClock()
{
   if (!mClockPaused)
   {
      mTimeAccumulated += mWallClock.GetClock() * mClockRate;
      mClockPaused = true;
      if (!mQuietMode)
      {
         auto out = ut::log::debug() << "Clock stopped.";
         out.AddNote() << "T = " << mTimeAccumulated;
      }
   }
}

// =================================================================================================
// virtual
void WsfRealTimeClockSource::SetMaximumClock(double aClock)
{
   double clockNow = GetClock(mMaximumClock);
   if (clockNow == mMaximumClock)
   {
      mTimeAccumulated = mMaximumClock;
      mWallClock.ResetClock();
   }
   WsfClockSource::SetMaximumClock(aClock);
}

// =================================================================================================
//! Suppresses output of messages to the screen.
void WsfRealTimeClockSource::SetQuietMode(bool aQuietMode)
{
   mQuietMode = aQuietMode;
}

// =================================================================================================
//! Get the Elapsed wallclock time since the last reset (start, stop, ratechange, etc)
double WsfRealTimeClockSource::GetElapsedWallTime()
{
   return mWallClock.GetClock();
}

// =================================================================================================
//! Set the timing method used by the underlying clock
void WsfRealTimeClockSource::SetTimingMethod(UtWallClock::TimingMethod aTimingMethod)
{
   mWallClock.SetTimingMethod(aTimingMethod);
}
