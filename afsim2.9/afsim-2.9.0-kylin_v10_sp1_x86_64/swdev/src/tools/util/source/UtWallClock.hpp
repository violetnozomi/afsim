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

#ifndef UTWALLCLOCK_HPP
#define UTWALLCLOCK_HPP

/**
   WallClock Time Functions
   This class provides methods to determine the amount of wall clock time that has elapsed.
*/

#ifdef _WIN32
// Nothing to include for now
#else
#include <sys/time.h>
#endif

#include "ut_export.h"

class UT_EXPORT UtWallClock
{
public:
   UtWallClock();
   ~UtWallClock() = default;

   //! Choices for the underlying timer.
   enum TimingMethod
   {
      //! Select the default timing method.
      cDEFAULT,
      //! QueryPerformanceCounter() available on Windows.  Falls back to cSYSTEM_TIME when not available
      cPERFORMANCE_COUNTER,
      //! GetSystemTime() on Windows, gettimeofday() on linux.
      cSYSTEM_TIME,
      //! GetTickCount() ( GetTickCount64() when available ) on Windows.
      cTICK_COUNT
   };

   UtWallClock(TimingMethod aTimingMethod);

   /**
      Return the elapsed wall clock time in seconds since object creation or
      the last call to ResetClock(), or the last call to GetCycleTimeSec
   */
   double GetCycleTime();

   // Get the reference tine
#ifdef _WIN32
   double GetBaseRef() const;
#else
   timeval GetBaseRef() const;
#endif
   /**
      Return the elapsed wall clock time in seconds since object creation or
     the last call to ResetClock().
   */
   double GetClock() const;

   /**
      Return the raw wall clock time (in seconds) relative to some system-dependent reference.
      The return value is in seconds.
   */

   double GetRawClock() const;

   /**
      Reset the base time of the wall clock.
      Subsequent calls to GetWallClock() will return the number of seconds
      that have elapsed since this was called.
   */

   void ResetClock();

   //! Sets the underlying timer for UtWallClock.  Calls ResetClock() after changing the timing method.
   void SetTimingMethod(TimingMethod aMethod);

   TimingMethod GetTimingMethod() const { return mTimingMethod; }

private:
   TimingMethod mTimingMethod{cDEFAULT};
   /**
      The value of the raw wall clock at object creation or the last call to Rest
   */
#ifdef _WIN32
   double mSecondsPerTick{0.0};
   double mBaseRef{0.0};
   double mBaseCycleRef{0.0};
   // When using GetTickCount(), this stores an offset to correct for time wrapping to zero.
   mutable double mTimeCorrection{0.0};
#else
   timeval mBaseRef;
   timeval mBaseCycleRef;
#endif
};

#ifdef _WIN32
inline double UtWallClock::GetBaseRef() const
{
   return mBaseRef;
}
#else
inline timeval UtWallClock::GetBaseRef() const
{
   return mBaseRef;
}
#endif
#endif
