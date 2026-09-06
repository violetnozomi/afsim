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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "UtProcessorTime.hpp"

#if defined(_WIN32)
#include <ctime>

#include <windows.h>

#include <winbase.h>
#elif defined(__unix__)
#include <ctime>

#include <sys/times.h>
#include <unistd.h>
#endif

UtProcessorTime::UtProcessorTime()
   : mUserBase(0.0)
   , mSystemBase(0.0)
   , mChildUserBase(0.0)
   , mChildSystemBase(0.0)
{
   // Establish the initial clock reference.
   ResetTime();
}

double UtProcessorTime::GetTime()
{
   double user;
   double system;
   double childUser;
   double childSystem;

   GetRawTime(user, system, childUser, childSystem);
   return (user - mUserBase) + (system - mSystemBase);
}

void UtProcessorTime::GetTime(double& aUser, double& aSystem)
{
   double childUser;
   double childSystem;

   GetRawTime(aUser, aSystem, childUser, childSystem);
   aUser -= mUserBase;
   aSystem -= mSystemBase;
}

void UtProcessorTime::GetTime(double& aUser, double& aSystem, double& aChildUser, double& aChildSystem)
{
   GetRawTime(aUser, aSystem, aChildUser, aChildSystem);
   aUser -= mUserBase;
   aSystem -= mSystemBase;
   aChildUser -= mChildUserBase;
   aChildSystem -= mChildSystemBase;
}

void UtProcessorTime::ResetTime()
{
   GetRawTime(mUserBase, mSystemBase, mChildUserBase, mChildSystemBase);
}

void UtProcessorTime::GetRawTime(double& aUser, double& aSystem, double& aChildUser, double& aChildSystem) const
{
#if defined(_WIN32)
   FILETIME      creationTime;
   FILETIME      exitTime;
   FILETIME      kernelTime;
   FILETIME      userTime;
   LARGE_INTEGER largeTime;
   BOOL          success;

   aChildUser   = 0.0;
   aChildSystem = 0.0;

   /* First try to use the GetProcessTimes function.  This should work on NT */
   /* but fail on 9x.  Under 9x we simply return the elapsed time.           */

   success = GetProcessTimes(GetCurrentProcess(), &creationTime, &exitTime, &kernelTime, &userTime);
   if (success != 0)
   {
      largeTime.u.LowPart  = userTime.dwLowDateTime;
      largeTime.u.HighPart = userTime.dwHighDateTime;
      aUser                = largeTime.QuadPart * 0.0000001;
      largeTime.u.LowPart  = kernelTime.dwLowDateTime;
      largeTime.u.HighPart = kernelTime.dwHighDateTime;
      aSystem              = largeTime.QuadPart * 0.0000001;
   }
   else
   {
      clock_t userTicks   = clock();
      double  secsPerTick = 1.0 / static_cast<double>(CLOCKS_PER_SEC);
      aUser               = static_cast<double>(userTicks) * secsPerTick;
      aSystem             = 0.0;
   }

#elif defined(__unix__)
   struct tms    timeBuffer;
   static double secsPerTick = 0.0;

   if (secsPerTick == 0.0)
   {
      long int clkTck = sysconf(_SC_CLK_TCK);
      if (clkTck == -1)
      {
#if defined(CLK_TCK)
         clkTck = CLK_TCK;
#elif defined(CLOCKS_PER_SEC)
         clkTck = CLOCKS_PER_SEC;
#else
         clkTck = 1;
#endif
      }
      secsPerTick = 1.0 / static_cast<double>(clkTck);
   }

   (void)times(&timeBuffer);

   aUser        = static_cast<double>(timeBuffer.tms_utime) * secsPerTick;
   aSystem      = static_cast<double>(timeBuffer.tms_stime) * secsPerTick;
   aChildUser   = static_cast<double>(timeBuffer.tms_cutime) * secsPerTick;
   aChildSystem = static_cast<double>(timeBuffer.tms_cstime) * secsPerTick;
#else
   aUser        = 0.0;
   aSystem      = 0.0;
   aChildUser   = 0.0;
   aChildSystem = 0.0;
#endif
}
