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

#ifndef WSFCLOCKSOURCE_HPP
#define WSFCLOCKSOURCE_HPP

#include "wsf_export.h"

/**
   Maintain the current simulation clock.
*/

class WSF_EXPORT WsfClockSource
{
public:
   WsfClockSource()          = default;
   virtual ~WsfClockSource() = default;

   WsfClockSource(const WsfClockSource& aSrc) = delete;
   WsfClockSource& operator=(const WsfClockSource& aRhs) = delete;
   WsfClockSource(WsfClockSource&& aSrc)                 = delete;
   WsfClockSource& operator=(WsfClockSource&& aRhs) = delete;

   virtual double GetClock(double aClock) const;

   virtual void ResetClock(double aAccumulatedTime = 0.0);

   virtual void SetClock(double aClock);

   virtual void SetClockRate(double aClockRate);

   virtual double GetClockRate() const;

   virtual void StartClock();

   virtual void StopClock();

   virtual void SetMaximumClock(double aClock);

   /**
      Return whether the clock is paused.
   */
   bool IsStopped() const { return mClockPaused; }

protected:
   double mClockRate{1.0};
   bool   mClockPaused{false};
   // Rationale for usage of this specific "magic number" value has not been identified.
   // Usage of std::numeric_limits<double>::max() would be preferred, subject to deeper investigation
   // of clock interactions and adequate supporting unit test coverage.
   double mMaximumClock{1.0E300};
};

#endif
