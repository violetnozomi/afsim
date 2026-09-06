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

#include "UtSleep.hpp"

#include <cerrno>
#include <cstring>

#include "UtLog.hpp"

#define USE_NANOSLEEP // Should work for all current releases (SGI, Sun, Linux)

#if defined(_WIN32)

#include <ctime>

#include <windows.h>

#include <winbase.h>

#else

#include <cstdlib>

#include <sys/time.h> // gettimeofday

#if defined(USE_NANOSLEEP)
#include <ctime> // nanosleep
#else

#if defined __linux__
#include <ctime>
#endif

#endif

#endif

/** The 'Sleep()' function is a replacement for the standard C function
 'sleep()' that allows for non-integral times.
 @param aSleepTime The time to sleep, in seconds.

 @note The input in seconds for WIN32 is different from the native function call,
 which is in milliseconds (Sleep(10) [native] -> UtSleep::Sleep(0.01)).

 @note Two different versions are supplied here.  I'm not sure if all systems
 implement 'nanosleep()' (I checked SGI, Sun, HP and DEC and they all have
 it).  I do know that Sun requires that you link with -lrt if you use it.
 The version based on 'setitimer()/pause()' should be more portable and is
 the default here.
*/
#if defined(_WIN32)

const double UtSleep::cMIN_SLEEP = 0.001; // One millisecond

void UtSleep::Sleep(double aSleepTime)
{
   if (SleepEx(static_cast<long>(aSleepTime * 1000.0), 0) != 0)
   {
      auto out = ut::log::error() << "Not enough Sleep time.";
      out.AddNote() << "Errno: " << std::strerror(errno);
   }
}

void UtSleep::SleepMinimum()
{
   SleepEx(1, 1);
}

#else
#if defined(USE_NANOSLEEP)

const double UtSleep::cMIN_SLEEP = 1.0e-9; // One Nanosecond

void UtSleep::Sleep(double aSleepTime)
{
   struct timespec ts;

   ts.tv_sec  = static_cast<time_t>(aSleepTime);
   ts.tv_nsec = static_cast<long int>((aSleepTime - ts.tv_sec) * 1.0e+9);
   nanosleep(&ts, nullptr);
}

void UtSleep::SleepMinimum()
{
   // The documentation indicates a sleep for zero may not rest at all.
   struct timespec ts;

   ts.tv_sec  = 0;
   ts.tv_nsec = 1;
   nanosleep(&ts, nullptr);
}

#else // Can't use nanosleep.

const double UtSleep::cMIN_SLEEP = 1.0e-6; // One Microsecond

/** Define a signal handler for SIGALRM.  When an interval timer expires it
 raises SIGALRM.  Our handler does nothing other than to prevent the process
 from being killed because the default action is to kill the process!

 @note One should not be tempted to insert a lot of processing within the
       signal handler.  Many standard C functions and user functions are
       not thread safe.  Unexpected results can occur in non-thread-safe
       code is invoke from within the signal handler.
*/
extern "C" void SleepSigAlrmHandler(int /* aSignal */) {}

void UtSleep::Sleep(double aSleepTime)
{
   struct sigaction act;
   struct sigaction oact;

   // Enable SIGALRM so the interval timer will not cause the process to exit.

   act.sa_handler = SleepSigAlrmHandler;
   sigemptyset(&act.sa_mask);
   act.sa_flags = 0;
   if (sigaction(SIGALRM, &act, &oact) == -1)
   {
      { // RAII block
         auto out = ut::log::fatal() << "sigaction(set):";
         out.AddNote() << "Errno: " << std::strerror(errno);
      }
      exit(1);
   }

   // Initiate an interval timer that will trigger after the specified time.

   long             sleepSec  = (long)aSleepTime;
   long             sleepUsec = (long)((aSleepTime - sleepSec) * 1.0e6);
   struct itimerval itv;

   itv.it_interval.tv_sec  = 0; // prevent timer reset ...
   itv.it_interval.tv_usec = 0; // ... when timer expires
   itv.it_value.tv_sec     = sleepSec;
   itv.it_value.tv_usec    = sleepUsec;
   if (setitimer(ITIMER_REAL, &itv, 0) == -1)
   {
      { // RAII block
         auto out = ut::log::fatal() << "setitimer():";
         out.AddNote() << "Errno: " << std::strerror(errno);
      }
      exit(1);
   }

   // 'pause()' waits for the process to receive a signal.  When the interval
   // timer expires it will raise SIGCHLD and cause 'pause()' to quit pausing.

   pause();

   // Restore SIGALRM processing to its previous state

   if (sigaction(SIGALRM, &oact, 0) == -1)
   {
      { // RAII block
         auto out = ut::log::fatal() << "sigaction(reset):";
         out.AddNote() << "Errno: " << std::strerror(errno);
      }
      exit(1);
   }
}

// if nanosleep isn't here, just do .001 seconds
void UtSleep::SleepMinimum()
{
   Sleep(.001);
}

#endif // USE_NANOSLEEP

#endif //_WIN32
