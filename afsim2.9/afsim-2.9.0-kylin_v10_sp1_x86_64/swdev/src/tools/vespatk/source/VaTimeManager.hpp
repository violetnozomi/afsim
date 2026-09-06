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

#ifndef VaTimeManager_hpp
#define VaTimeManager_hpp

#include "vespatk_export.h"

#include "UtWallClock.hpp"
#include "VaCallbackHolder.hpp"

namespace vespa
{
class Viewer;

// Represents the state of the time manager
struct VESPATK_EXPORT VaTimeManagerState
{
   enum Mode
   {
      cPLAY,
      cREWIND,
      cPAUSE
   };

   VaTimeManagerState();
   bool operator==(const VaTimeManagerState& aState) const
   {
      return mMode == aState.mMode && mLoopEnabled == aState.mLoopEnabled && mRealtimeClock == aState.mRealtimeClock &&
             mCurrentTime == aState.mCurrentTime && mMinTime == aState.mMinTime && mMaxTime == aState.mMaxTime &&
             mRate == aState.mRate;
   }
   double GetDirection() const
   {
      switch (mMode)
      {
      case cPLAY:
         return 1.0;
      case cREWIND:
         return -1.0;
      case cPAUSE:
      default:
         return 0.0;
      }
   }
   // returns the rate negative when rewinding, zero when paused
   double GetActualRate() const { return mRate * GetDirection(); }
   Mode   mMode;
   bool   mLoopEnabled;
   bool   mRealtimeClock;
   double mCurrentTime;
   double mMinTime;
   double mMaxTime;
   // Absolute value of time rate.  If paused, this is the rate to be used after resuming.
   double mRate;
};

class VESPATK_EXPORT VaTimeManager : private VaTimeManagerState
{
public:
   typedef VaTimeManagerState::Mode Mode;
   using VaTimeManagerState::cPAUSE;
   using VaTimeManagerState::cPLAY;
   using VaTimeManagerState::cREWIND;

   VaTimeManager();
   virtual ~VaTimeManager();

   virtual void Update();

   Mode GetMode() const { return mMode; }
   bool GetLoopEnabled() const { return mLoopEnabled; }
   bool GetRealtimeClock() const { return mRealtimeClock; }
#undef GetCurrentTime // Avoid conflict with Windows macro
   double GetCurrentTime() const { return mCurrentTime; }
   double GetMinTime() const { return mMinTime; }
   double GetMaxTime() const { return mMaxTime; }
   double GetActualRate() const { return VaTimeManagerState::GetActualRate(); }
   double GetRate() const { return mRate; }
   double GetBaseClockTime() const { return mBaseClockTime; }
   double GetNormalizedTime() const;
   double GetNormalizedTime(double aTime) const;
   double GetWallClockTime() { return mClock.GetClock(); }

   void Play();
   void Rewind();
   void Pause();
   void Reset();
   void SetLoopEnabled(bool aLoopEnabled);
   void SetRealtimeClock(bool aRealtimeClock);
   void SetTime(double aTime, bool aForceUpdate = false);
   void SetTimeSticky(double aTime);

   void SetMinTime(double aMinTime);
   void SetMaxTime(double aMaxTime);
   void SetRate(double aRate);
   void SetSimulationRate(double aRate);
   void SetSimulationPause();
   void SetSimulationGo();
   void SetNormalizedTime(double aNormalizedTime);
   void UpdateFrame();

   VaTimeManagerState GetState() const;

   void SetState(const VaTimeManagerState& aState);

protected:
   void   BroadcastTimeUpdated();
   void   BroadcastTimeDirty();
   void   BroadcastTimeModeChange();
   bool   Clamp();
   double ClampTime(double aTime) const;

   void UpdateBaseClockTime(double aTime);

   bool             mFirstTime;
   double           mTime;
   UtWallClock      mClock;
   double           mBaseClockTime;
   VaCallbackHolder mCallbacks;
   double           mSimulationRate; // the rate at which a live simulation is updating.
   double           mSimulationPauseGoRate;

   // Sticky time: for SetTimeSticky()
   double mStickyTime; // valid if non-zero
   double mClockAtSetStickyTime;
};
} // end namespace vespa

#endif
