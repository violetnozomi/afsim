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

#ifndef WSFREALTIMECLOCKSOURCE_HPP
#define WSFREALTIMECLOCKSOURCE_HPP

#include "wsf_export.h"

#include "UtWallClock.hpp"
#include "WsfClockSource.hpp"

/**
   A specialization of WsfClockSource to support a real-time clock.

   See the base class for documentation.
*/

class WSF_EXPORT WsfRealTimeClockSource : public WsfClockSource
{
public:
   WsfRealTimeClockSource();
   ~WsfRealTimeClockSource() override;

   double GetClock(double aClock) const override;

   void ResetClock(double aAccumulatedTime = 0.0) override;

   void SetClock(double aClock) override;

   void SetClockRate(double aClockRate) override;

   void StartClock() override;

   void StopClock() override;

   void SetMaximumClock(double aClock) override;

   void SetQuietMode(bool aQuietMode);

   double GetElapsedWallTime();

   void SetTimingMethod(UtWallClock::TimingMethod aTimingMethod);

private:
   // The wall clock object is used to maintain the elapsed REAL time
   // since the clock was last started.

   UtWallClock mWallClock;

   // The accumulated time is the elapsed SIMULATION time when the clock
   // was last started.  This value is updated with the current SIMULATION
   // time whenever the clock is stopped.

   double mTimeAccumulated;

   bool mQuietMode;
};

#endif
