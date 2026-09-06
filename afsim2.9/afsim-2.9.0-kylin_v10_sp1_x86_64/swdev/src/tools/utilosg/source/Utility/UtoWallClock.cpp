// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "UtoWallClock.hpp"

#ifdef _WIN32
#include <windows.h>

#include <winbase.h>
#else
#include <sys/time.h>
#endif

#include <iostream>

#include "UtLog.hpp"

namespace UtoWallClock
{
// InitClock() will be called to initialize initTime once, it also initializes
// other variables for windows.  This was to avoid requiring something to
// call an initialization function before the first GetClock() call.
#ifdef _WIN32
#define INIT_RETURN double
#else
#define INIT_RETURN timeval
#endif
static INIT_RETURN InitClock();

// The value of the raw wall clock at initialization.
#ifdef _WIN32
static double secsPerRollover; // seconds per timer rollover
static double secsPerTick;     // seconds per timer tick
static double initTime = InitClock();
static double maxTime;
#else
static timeval initTime = InitClock();
#endif

static INIT_RETURN InitClock()
{
#ifdef _WIN32
   double        frequency;
   unsigned int  i;
   LARGE_INTEGER PerfFreq;
   if (QueryPerformanceCounter(&PerfFreq))
   {
      QueryPerformanceFrequency(&PerfFreq);
      frequency = (double)PerfFreq.HighPart;
      for (i = 0; i < 8 * sizeof(PerfFreq.LowPart); i++)
         frequency *= 2.0;
      frequency += (double)PerfFreq.LowPart;

#ifdef VERBOSE
      auto out = ut::log::info() << "Timer:";
      out.AddNote() << "Frequency: " << frequency / 1000000.0 << " MHz";
      out.AddNote() << " (" << PerfFreq.HighPart << "," << PerfFreq.LowPart << ")";
#endif

      secsPerTick = 1.0 / frequency;

      secsPerRollover = secsPerTick;
      for (i = 0; i < 8 * sizeof(PerfFreq.LowPart); i++)
         secsPerRollover *= 2.0;
      maxTime = secsPerRollover;
      for (i = 0; i < 8 * sizeof(PerfFreq.HighPart); i++)
         maxTime *= 2.0;

#ifdef VERBOSE
      out.AddNote() << "Max Time: " << maxTime;
#endif
   }
   else
   {
      ut::log::info() << "No Perf Counter!";
      secsPerTick = 0;
   }
   return GetClock();
#else
   struct timeval tv1;
   gettimeofday(&tv1, nullptr);
   return tv1;
#endif
}

double GetClock()
{
#ifdef _WIN32
   if (secsPerTick)
   {
      LARGE_INTEGER intTime;
      QueryPerformanceCounter(&intTime);

      double currTime = (double)((intTime.HighPart * secsPerRollover) + (intTime.LowPart * secsPerTick));
      if (currTime < initTime)
         currTime += maxTime;
      return currTime - initTime;
   }
   else
   {
      return GetTickCount() / 1000.0 - initTime;
   }
#else
   struct timeval current;
   gettimeofday(&current, nullptr);
   return current.tv_sec - initTime.tv_sec + (current.tv_usec - initTime.tv_usec) * 1E-6;
#endif
}

} // namespace UtoWallClock
