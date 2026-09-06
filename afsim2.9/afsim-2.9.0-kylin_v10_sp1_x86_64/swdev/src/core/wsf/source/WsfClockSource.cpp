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

#include "WsfClockSource.hpp"

/**
  Return the current simulation clock.

  The return value will be the lesser of the supplied argument (aClock)
  and the physical simulation clock (for example, the elapsed real-time
  in a real-time simulation).

  This allows a simulation executive (like WsfEventStepSimulation) to
  interleave events with a real-time clock.
*/
// virtual
double WsfClockSource::GetClock(double aClock) const
{
   double clockNow = aClock;
   if (clockNow > mMaximumClock)
   {
      clockNow = mMaximumClock;
   }
   return clockNow;
}

/**
  Reset the simulation clock back to zero.
*/
// virtual
void WsfClockSource::ResetClock(double aAccumulatedTime) {}

/**
   Set the clock to a specified value.

   This method should be used with great caution!  It should only be used
   when the clock is in a stopped (paused) state and should not set the
   clock backwards!
*/
// virtual
void WsfClockSource::SetClock(double /* aClock */) {}

/**
   Set the clock rate multiplier.

   The clock rate multiplier allows SIMULATION time to flow faster or
   slower than the physical clock.  Note that this only makes sense if
   the derived class is based on some concept of physical time.
*/
// virtual
void WsfClockSource::SetClockRate(double aClockRate)
{
   mClockRate = aClockRate;
}

/**
   Get the clock rate multiplier.
*/
double WsfClockSource::GetClockRate() const
{
   return mClockRate;
}

/**
   Start the clock.
*/
// virtual
void WsfClockSource::StartClock()
{
   mClockPaused = false;
}

/**
   Stop (pause) the clock.
*/
// virtual
void WsfClockSource::StopClock()
{
   mClockPaused = true;
}

/**
   Set the clock's maximum time
*/
// virtual
void WsfClockSource::SetMaximumClock(double aClock)
{
   mMaximumClock = aClock;
}
