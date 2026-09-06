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

#include "VaTimeManager.hpp"

#include <iostream>
#include <math.h>

#include "VaObserver.hpp"

namespace vespa
{

// max time between SetTimeSticky() and actually changing the time
const double cMAX_STICKY_TIME_DELAY = 10.0;

VaTimeManagerState::VaTimeManagerState()
   : mMode(cPAUSE)
   , mLoopEnabled(true)
   , mRealtimeClock(false)
   , mCurrentTime(0.0)
   , mMinTime(0.0)
   , mMaxTime(0.0)
   , mRate(1.0)
{
}

VaTimeManager::VaTimeManager()
   : mFirstTime(true)
   , mTime(0.0)
   , mClock()
   , mBaseClockTime(0.0)
   , mCallbacks()
   , mSimulationRate(1.0)
   , mSimulationPauseGoRate(1.0)
   , mStickyTime(0)
{
   // If we clamped the time, send a 'TimeDirty' event.
   BroadcastTimeDirty();
   BroadcastTimeModeChange();
}

VaTimeManager::~VaTimeManager()
{
   mCallbacks.Clear();
}

//! Starts the time manager so that time advances in the positive direction.
// private
void VaTimeManager::Play()
{
   mMode = cPLAY;

   // Update the reference time.
   UpdateBaseClockTime(mClock.GetClock());

   BroadcastTimeDirty();
   BroadcastTimeModeChange();

   // Generate an 'UpdateDisplay' event.
   VaObserver::RedrawWindow["viewer"]();
}

//! Starts the time manager so that time advances in the negative direction.
// private
void VaTimeManager::Rewind()
{
   mMode = cREWIND;

   // Update the reference time.
   UpdateBaseClockTime(mClock.GetClock());

   BroadcastTimeDirty();
   BroadcastTimeModeChange();

   // Generate an 'UpdateDisplay' event.
   VaObserver::RedrawWindow["viewer"]();
}

//! Pauses the time manager so that time doesn't advance
// private
void VaTimeManager::Pause()
{
   mMode = cPAUSE;

   BroadcastTimeDirty();
   BroadcastTimeModeChange();

   // Generate an 'UpdateDisplay' event.
   VaObserver::RedrawWindow["viewer"]();
}


//! Resets the time manager.
// private
void VaTimeManager::Reset()
{
   mTime        = 0.0;
   mCurrentTime = 0.0;
   mMinTime     = 0.0;
   mMaxTime     = 0.0;
   mRate        = 1.0;
   Pause();
}

//! Clamps the current time to be within the minimum and maximum time.
// private
bool VaTimeManager::Clamp()
{
   bool clamped = false;

   // Clamp the current time to lie within the current limits.
   if (mCurrentTime > mMaxTime)
   {
      if (mLoopEnabled && !mRealtimeClock)
      {
         mCurrentTime = mMinTime; // Loop back to the beginning
         clamped      = true;
      }
      else
      {
         mCurrentTime = mMaxTime;
         clamped      = true;
      }
   }
   else if (mCurrentTime < mMinTime)
   {
      mCurrentTime = mMinTime;
      clamped      = true;
   }
   return clamped;
}

double VaTimeManager::ClampTime(double aTime) const
{
   if (aTime > mMaxTime)
      return mMaxTime;
   else if (aTime < mMinTime)
      return mMinTime;
   return aTime;
}


//! Sets whether the time should loop back to the beginning once
//! end has been reached.  This flag only has effect when NOT using realtime clock
//! @param aLoopEnabled whether to enable looping.
void VaTimeManager::SetLoopEnabled(bool aLoopEnabled)
{
   mLoopEnabled = aLoopEnabled;
}

//! Sets whether a realtime clock should be used to increase the time window
void VaTimeManager::SetRealtimeClock(bool aRealtimeClock)
{
   mRealtimeClock = aRealtimeClock;
}

//! Explicitly sets the current time.
//! @param aTime The time to set.
//! @param aForceUpdate Forces and update even if the time hasn't changed
// private
void VaTimeManager::SetTime(double aTime, bool aForceUpdate)
{
   if ((mCurrentTime != aTime) || (aForceUpdate))
   {
      mStickyTime  = 0;
      mCurrentTime = aTime;
      Clamp();
      BroadcastTimeDirty();
      BroadcastTimeUpdated();

      VaObserver::RedrawWindow["viewer"]();
   }
}

//! Same as SetTime(t, true), except that if the specified time is
//! outside of the min/max time range, the time will be applied later.
//! Useful for restoring time while reloading a replay
void VaTimeManager::SetTimeSticky(double aTime)
{
   if (ClampTime(aTime) == aTime)
   {
      SetTime(aTime, true);
   }
   else
   {
      mStickyTime           = aTime;
      mClockAtSetStickyTime = mClock.GetClock();
   }
}

//! Sets the minimum allowed time.
//! @param aMinTime The minimum time to set.
// private
void VaTimeManager::SetMinTime(double aMinTime)
{
   if (mMinTime != aMinTime)
   {
      mMinTime = aMinTime;
      BroadcastTimeDirty();
   }
}

//! Sets the maximum allowed time.
//! @param aMaxTime The maximum time to set.
// private
void VaTimeManager::SetMaxTime(double aMaxTime)
{
   if (mMaxTime != aMaxTime)
   {
      mMaxTime = aMaxTime;
      BroadcastTimeDirty();
   }
}

//! Sets the rate at which time should play.
//! @param aRate The rate to set.
// private
void VaTimeManager::SetRate(double aRate)
{
   if (mRate != aRate)
   {
      mRate = aRate;
      BroadcastTimeDirty();
   }
}

//! Sets the rate at which the simulation is updating
//! @param aRate The rate to set
// private
void VaTimeManager::SetSimulationRate(double aRate)
{
   if (mSimulationRate != aRate)
   {
      mSimulationRate = aRate;
      BroadcastTimeDirty();
   }
}

void VaTimeManager::SetSimulationPause()
{
   mSimulationPauseGoRate = 0.0;
   BroadcastTimeDirty();
}

void VaTimeManager::SetSimulationGo()
{
   mSimulationPauseGoRate = 1.0;
   BroadcastTimeDirty();
}

//! Sets the time given a value in the range [0, 1].
//! @param aNormalizedTime The time normalized time value to set.
// private
void VaTimeManager::SetNormalizedTime(double aNormalizedTime)
{
   if ((aNormalizedTime >= 0.0) && (aNormalizedTime <= 1.0))
   {
      SetTime(mMinTime + (aNormalizedTime * (mMaxTime - mMinTime)));
   }
}

//! Returns the current time as a value in the range [0, 1].
double VaTimeManager::GetNormalizedTime() const
{
   return GetNormalizedTime(mCurrentTime);
}

//! Returns the provided time as a value in the range [0, 1].
//! @param aTime The time to normalize.
double VaTimeManager::GetNormalizedTime(double aTime) const
{
   double interpolatedValue = 0.0;
   if (mMaxTime > mMinTime)
   {
      interpolatedValue = (aTime - mMinTime) / (mMaxTime - mMinTime);
   }
   return interpolatedValue;
}

//! Updates the current time based on the current rate.
// private virtual
void VaTimeManager::Update()
{
   const double curClockTime = mClock.GetClock();

   // If a sticky time has been set, check if it can be applied now
   if (mStickyTime)
   {
      if (curClockTime - mClockAtSetStickyTime > cMAX_STICKY_TIME_DELAY)
      {
         mStickyTime = 0;
      }
      else
      {
         if (ClampTime(mStickyTime) == mStickyTime)
         {
            SetTime(mStickyTime, true);
         }
      }
   }


   double prevTime = mCurrentTime;

   if (mTime == mCurrentTime)
   {
      SetRate(mSimulationRate);
   }

   // Update the real time.
   mTime += (curClockTime - mBaseClockTime) * mSimulationRate * mSimulationPauseGoRate;

   // Update the current time.
   mCurrentTime += (curClockTime - mBaseClockTime) * GetActualRate();

   // If doing realtime clock, we need to move the maximum time forward
   // as real time progresses.
   if (mRealtimeClock)
   {
      if (mTime > mMaxTime)
      {
         SetMaxTime(mTime);
      }
      if (mCurrentTime >= mMaxTime)
      {
         mCurrentTime = mMaxTime;
         mRate        = mSimulationRate;
      }
   }

   // Clamp the current time.
   if (Clamp())
   {
      // If we clamped the time, send a 'TimeDirty' event.
      BroadcastTimeDirty();
   }

   UpdateBaseClockTime(curClockTime);

   // If the time changed, send 'TimeUpdated'
   if (mFirstTime || (mCurrentTime != prevTime))
   {
      BroadcastTimeUpdated();
   }

   // Generate an 'RedrawWindow' event.
   VaObserver::RedrawWindow["viewer"]();

   mFirstTime = false;
}

//! Updates the base time to the current wall clock time.
// private
void VaTimeManager::UpdateBaseClockTime(double aTime)
{
   mBaseClockTime = aTime;
}

void VaTimeManager::UpdateFrame()
{
   VaObserver::RedrawWindow["frame_update"]();
   Update();
}

vespa::VaTimeManagerState VaTimeManager::GetState() const
{
   return *this;
}

void VaTimeManager::SetState(const VaTimeManagerState& aState)
{
   if (!(*this == aState))
   {
      bool modeChanged            = mMode != aState.mMode;
      bool timeChanged            = mCurrentTime != aState.mCurrentTime;
      (VaTimeManagerState&)* this = aState;
      if (modeChanged)
      {
         UpdateBaseClockTime(mClock.GetClock());
         BroadcastTimeModeChange();
      }
      BroadcastTimeDirty();
      if (timeChanged)
      {
         mStickyTime = 0;
         BroadcastTimeUpdated();
      }
   }
}

void VaTimeManager::BroadcastTimeDirty()
{
   VaObserver::TimeDirty(mCurrentTime, GetActualRate(), mMinTime, mMaxTime);
}

void VaTimeManager::BroadcastTimeModeChange()
{
   VaObserver::TimeModeChanged((int)mMode);
}

void VaTimeManager::BroadcastTimeUpdated()
{
   VaObserver::TimeUpdated(mCurrentTime, GetActualRate(), mMinTime, mMaxTime);
}


} // namespace vespa
