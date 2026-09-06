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

#include "UtWallClock.hpp"

#if defined(_WIN32)
#include <windows.h>

#include <winbase.h>

// GetTickCount64 is only available on later versions of windows.
namespace
{
using TickCount64_FunctionPtr             = ULONGLONG (*)();
TickCount64_FunctionPtr g_TickCount64_Ptr = nullptr;
} // namespace
#endif

UtWallClock::UtWallClock()
{
   // Establish the initial clock timing method and reference.
   SetTimingMethod(cDEFAULT);
}

UtWallClock::UtWallClock(TimingMethod aTimingMethod)
   : mTimingMethod(aTimingMethod)
{
}

double UtWallClock::GetCycleTime()
{
#ifdef _WIN32
   double current = GetRawClock();
   double time    = current - mBaseCycleRef;
   mBaseCycleRef  = current;
   return time;
#else
   timeval         current;
   struct timezone notUsed;
   gettimeofday(&current, &notUsed);
   double time = static_cast<double>(current.tv_sec - mBaseCycleRef.tv_sec) +
                 static_cast<double>(current.tv_usec - mBaseCycleRef.tv_usec) * 1.0E-6;
   mBaseCycleRef = current;
   return time;
#endif
}

double UtWallClock::GetClock() const
{
#ifdef _WIN32
   return GetRawClock() - mBaseRef;
#else
   timeval         current;
   struct timezone notUsed;
   gettimeofday(&current, &notUsed);
   return static_cast<double>(current.tv_sec - mBaseRef.tv_sec) +
          static_cast<double>(current.tv_usec - mBaseRef.tv_usec) * 1.0E-6;
#endif
}

void UtWallClock::ResetClock()
{
#ifdef _WIN32
   mBaseRef      = GetRawClock();
   mBaseCycleRef = mBaseRef;
#else
   struct timezone notUsed;
   gettimeofday(&mBaseRef, &notUsed);
   mBaseCycleRef = mBaseRef;
#endif
}

// private
double UtWallClock::GetRawClock() const
{
#if defined(_WIN32)
   switch (mTimingMethod)
   {
   case cPERFORMANCE_COUNTER:
   {
      LARGE_INTEGER tickCount;
      QueryPerformanceCounter(&tickCount);
      return tickCount.QuadPart * mSecondsPerTick;
   }
   case cTICK_COUNT:
   {
      if (g_TickCount64_Ptr != nullptr)
      {
         ULONGLONG tc = (*g_TickCount64_Ptr)();
         return tc / 1000.0;
      }
      else
      {
         DWORD  tc     = GetTickCount();
         double tcTime = tc / 1000.0 + mTimeCorrection;
         if (tcTime < mBaseRef)
         {
            // GetTickCount() wraps to zero after 49.7 days
            // In the unlikely event this happens, store an adjustment to correct for it.
            mTimeCorrection += (mBaseRef - tcTime);
            tcTime = tc / 1000.0 + mTimeCorrection;
         }
         return tcTime;
      }
   }
   case cSYSTEM_TIME:
   default:
   {
      // The return value is the elapsed time since 00:00:00 UTC, January 1, 1601
      SYSTEMTIME     systemTime;
      FILETIME       fileTime;
      ULARGE_INTEGER largeTime;

      GetSystemTime(&systemTime);
      (void)SystemTimeToFileTime(&systemTime, &fileTime);
      largeTime.LowPart  = fileTime.dwLowDateTime;
      largeTime.HighPart = fileTime.dwHighDateTime;

      double sysTime;
      // The file time is in units of 100 nanosecond intervals.
#if _MSC_VER < 1300
      sysTime = (static_cast<signed __int64>(largeTime.QuadPart) * 1.0E-7);
#else
      sysTime = (static_cast<double>(largeTime.QuadPart) * 1.0E-7);
#endif
      return sysTime;
   }
   }
#else

   // The return value is the elapsed time since 00:00:00 UTC, January 1, 1970
   timeval         current;
   struct timezone notUsed;
   gettimeofday(&current, &notUsed);
   return static_cast<double>(current.tv_sec) + static_cast<double>(current.tv_usec) * 1.0E-6;
#endif
}

void UtWallClock::SetTimingMethod(TimingMethod aMethod)
{
   mTimingMethod = aMethod;

#if defined(_WIN32)
   if (mTimingMethod == cDEFAULT || mTimingMethod == cPERFORMANCE_COUNTER)
   {
      // Attempt to use the high resolution counter if it is available.
      mSecondsPerTick = 0.0;
      LARGE_INTEGER ticksPerSecond;
      LARGE_INTEGER tickCount;
      if ((QueryPerformanceFrequency(&ticksPerSecond) != 0) && (QueryPerformanceCounter(&tickCount) != 0))
      {
         mTimingMethod   = cPERFORMANCE_COUNTER;
         mSecondsPerTick = 1.0 / ticksPerSecond.QuadPart;
      }
      else
      {
         mTimingMethod   = cSYSTEM_TIME;
         mTimeCorrection = 0.0;
      }
   }
   else if (mTimingMethod == cTICK_COUNT)
   {
      if (g_TickCount64_Ptr == nullptr)
      {
         g_TickCount64_Ptr = (TickCount64_FunctionPtr)GetProcAddress(GetModuleHandle("kernel32.dll"), "GetTickCount64");
      }
      mTimeCorrection = 0.0;
   }
#else
   // Currently only system time is supported
   mTimingMethod = cSYSTEM_TIME;
#endif
   ResetClock();
}
