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

#include "WsfFrameStepSimulation.hpp"

#include <algorithm>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtSleep.hpp"
#include "WsfComm.hpp"
#include "WsfEvent.hpp"
#include "WsfEventManager.hpp"
#include "WsfMover.hpp"
#include "WsfMultiThreadManager.hpp"
#include "WsfPlatform.hpp"
#include "WsfProcessor.hpp"
#include "WsfRealTimeClockSource.hpp"
#include "WsfScenario.hpp"
#include "WsfSensor.hpp"
#include "WsfSimulation.hpp"
#include "WsfSimulationInput.hpp"
#include "WsfSimulationObserver.hpp"

namespace
{
template<typename T>
void Erase(std::vector<T>& aVector, T aElement)
{
   typename std::vector<T>::iterator iter = find(aVector.begin(), aVector.end(), aElement);
   if (iter != aVector.end())
   {
      aVector.erase(iter);
   }
}
template<typename T>
void Insert(std::vector<T>& aVector, T aElement)
{
   if (find(aVector.begin(), aVector.end(), aElement) == aVector.end())
   {
      aVector.push_back(aElement);
   }
}
} // namespace


// =================================================================================================
//! Constructor
//! @param aScenario The scenario
//! @param aRunNumber The Monte-Carlo run number
//! If 'false' then every call to AdvanceTime will result in a frame advance.
WsfFrameStepSimulation::WsfFrameStepSimulation(WsfScenario& aScenario, unsigned int aRunNumber)
   : WsfSimulation(aScenario, aRunNumber)
   , mFrameCount(0.0)
   , mNextFrameTime(0.0)
   , mTimeUntilNextFrame(0.0)
   , mTotalFrameUnderTime(0.0)
   , mTotalFrameUnderCount(0.0)
   , mTotalFrameOverTime(0.0)
   , mTotalFrameOverCount(0.0)
   , mSkippedFrameCount(0.0)
   , mWorstFrameOverTime(0.0)
   , mPlatforms()
   , mComms()
   , mProcessors()
   , mSensors()
{
   mFrameStepInput = &dynamic_cast<WsfFrameStepSimulationInput&>(aScenario.GetSimulationInput());
}

// =================================================================================================
// virtual
void WsfFrameStepSimulation::AddEvent(std::unique_ptr<WsfEvent> aEventPtr)
{
   // In cases where events are added to execute in the current frame,
   // it is possible to proceed in an infinite loop if the subsequent evaluation
   // triggers adding the same event again.
   if (aEventPtr->GetTime() < mNextFrameTime)
   {
      aEventPtr->SetTime(mNextFrameTime);
   }
   mEventManager.AddEvent(std::move(aEventPtr));
}

// =================================================================================================
// protected virtual
void WsfFrameStepSimulation::CreateClock()
{
   if (mIsRealTime)
   {
      SetClockSource(ut::make_unique<WsfRealTimeClockSource>());
   }
   else
   {
      // When not using the real time clock we simply let time flow as fast as frames can run.
      // With WsfClockSource, calling GetClock(T) will simply return T.
      SetClockSource(ut::make_unique<WsfClockSource>());
   }
}

// =================================================================================================
// protected virtual
double WsfFrameStepSimulation::AdvanceFrame()
{
   // The current time is the current value of 'next frame time'.
   double currentFrameTime = mNextFrameTime;

   // Notify simulation observers of frame start
   WsfObserver::FrameStarting(this)(currentFrameTime);

   // ... and move the 'next frame time' forward.
   mFrameCount += 1.0;
   mNextFrameTime = mFrameCount * GetFrameTime();

   // First move all of the platforms forward starting at the current time.

   unsigned int i;
   if (MultiThreaded())
   {
      GetMultiThreadManager().UpdatePlatforms(currentFrameTime);
   }
   else
   {
      for (i = 0; i < GetPlatformCount(); ++i)
      {
         GetPlatformEntry(i)->Update(currentFrameTime);
      }
      WsfObserver::FramePlatformsUpdated(this)(currentFrameTime);
   }

   // Now update subsystems.

   for (i = 0; i < mComms.size(); ++i)
   {
      mComms[i]->Update(currentFrameTime);
   }

   for (i = 0; i < mProcessors.size(); ++i)
   {
      mProcessors[i]->Update(currentFrameTime);
   }

   if (MultiThreaded())
   {
      GetMultiThreadManager().UpdateSensors(currentFrameTime);
   }
   else
   {
      for (i = 0; i < mSensors.size(); ++i)
      {
         mSensors[i]->Update(currentFrameTime);
      }
   }

   AdvanceFrameObjects(currentFrameTime);


   // Dispatch all events from the current time up until the start of the next frame.
   // Dispatch pending events up to and including the current time.

   WsfEvent* peekEventPtr = mEventManager.PeekEvent();
   while (peekEventPtr && (peekEventPtr->GetTime() < mNextFrameTime))
   {
      auto eventPtr = mEventManager.PopEvent();

      // Note: The event time is modified to be the actual dispatch time of the event
      // and not the time for which it was actually queued.  By definition we are
      // running a clock which can possess only discrete values.

      double originalEventTime = eventPtr->GetTime();
      eventPtr->SetTime(currentFrameTime);
      WsfEvent::EventDisposition disposition = eventPtr->Execute();
      if (disposition == WsfEvent::cRESCHEDULE)
      {
         // We modified the event time above to the current time.  The event may try to
         // reschedule the event and that is generally a delta of the event time.
         // That may be less than the original time, or it may keep rescheduling
         // within the current frame.  The following will prevent this problem.

         double newEventTime = originalEventTime + (eventPtr->GetTime() - currentFrameTime);
         eventPtr->SetTime(newEventTime);
         mEventManager.AddEvent(std::move(eventPtr));
      }
      peekEventPtr = mEventManager.PeekEvent();
   }

   //! Determine if we've exceeded the frame time.

   double clockTime = mClockSourcePtr->GetClock(1.0E+37);
   mRealTime        = currentFrameTime;
   double timeLeft  = 0.0;
   if (mIsRealTime)
   {
      timeLeft  = mNextFrameTime - clockTime;
      mRealTime = clockTime;
   }

   if (timeLeft >= 0.0)
   {
      mTotalFrameUnderTime += timeLeft;
      mTotalFrameUnderCount += 1.0;
      mTimeUntilNextFrame = timeLeft;
   }
   else
   {
      timeLeft = -timeLeft;
      mTotalFrameOverTime += timeLeft;
      mTotalFrameOverCount += 1.0;
      mTimeUntilNextFrame = 0.0;

      if (timeLeft > mWorstFrameOverTime)
      {
         mWorstFrameOverTime = timeLeft;
      }
      { // RAII block
         auto out = ut::log::warning() << "Exceeded frame time.";
         out.AddNote() << "T = " << mNextFrameTime;
         out.AddNote() << "Wall T = " << clockTime;
         out.AddNote() << "Exceeded By: " << 100.0 * timeLeft / GetFrameTime() << "%";
      }
      // Adjust the frame counter to account for the lost frames.
      // If we're only a little over (<= 10%) then we don't skip the frame.

      if ((timeLeft / GetFrameTime()) > 0.10)
      {
         int skippedFrameCount = static_cast<int>((timeLeft + GetFrameTime()) / GetFrameTime());
         mFrameCount += skippedFrameCount;
         mSkippedFrameCount += skippedFrameCount;
      }

      // It's almost always the sensors that cause us to miss the frame time, and because
      // of the implementation, once it's behind it just keeps getting worse if you don't
      // do the following...

      double nextUpdateTime = mFrameCount * GetFrameTime();
      for (i = 0; i < mSensors.size(); ++i)
      {
         mSensors[i]->AdjustNextUpdateTime(nextUpdateTime);
      }
   }
   WsfObserver::FrameComplete(this)(currentFrameTime);

   return currentFrameTime;
}

// =================================================================================================
// virtual
double WsfFrameStepSimulation::AdvanceTime()
{
   // Get the current simulation time.
   // For non-realtime the small delta is added to the argument to force the 'if' check to be true.
   mSimTime = mClockSourcePtr->GetClock(mNextFrameTime + 0.000001);

   if (mSimTime > mNextFrameTime)
   {
      mSimTime = AdvanceFrame();

      WsfObserver::AdvanceTime(this)(mSimTime);

      // If it is still past the end time,
      // Change the simulation's state.
      if (mSimTime > GetEndTime())
      {
         mState = cPENDING_COMPLETE;
      }
   }
   else
   {
      // Even when the sim time does not advance, check for wall clock events.
      //( Tried merely WsfSimulation::DispatchWallEvents(), and this was not sufficient.)
      WsfSimulation::AdvanceTime();
   }

   return mSimTime;
}

// =================================================================================================
// virtual
double WsfFrameStepSimulation::AdvanceTime(double aSimTime)
{
   // Get the current simulation time.
   // For non-realtime the small delta is added to the argument to force the 'if' check to be true.
   double simTime = mClockSourcePtr->GetClock(mNextFrameTime + 0.000001);

   // If the caller is requesting a smaller time then use it.
   // If the caller is requesting a larger time then we just use the current clock
   // time because we aren't allowed to advance past that time.
   if (aSimTime < simTime)
   {
      simTime = aSimTime;
   }

   // Advance to the next frame only when it is time.
   if (simTime > mNextFrameTime)
   {
      simTime = AdvanceFrame();
   }
   WsfObserver::AdvanceTime(this)(simTime);
   return simTime;
}

// =================================================================================================
// virtual
bool WsfFrameStepSimulation::AddPlatform(double aSimTime, WsfPlatform* aPlatformPtr)
{
   bool ok = WsfSimulation::AddPlatform(aSimTime, aPlatformPtr);
   if (ok)
   {
      // Add the platform the our local list.
      std::vector<WsfPlatform*>::iterator iter = find(mPlatforms.begin(), mPlatforms.end(), aPlatformPtr);
      if (iter == mPlatforms.end())
      {
         mPlatforms.push_back(aPlatformPtr);
      }
      if (MultiThreaded())
      {
         GetMultiThreadManager().AddPlatform(aSimTime, aPlatformPtr);
      }
   }
   return ok;
}

// =================================================================================================
// virtual
bool WsfFrameStepSimulation::AddPlatform(WsfPlatform* aPlatformPtr)
{
   // provided to quiet compiler warning
   return WsfSimulation::AddPlatform(aPlatformPtr);
}


// =================================================================================================
//! Delete a platform from the simulation
//! @param aSimTime      Current simulation time
//! @param aPlatformPtr  Pointer to platform to be deleted
//! @param aDeleteMemory If true (default), platform's memory will be cleaned up. Otherwise, platform
//!                      is simply removed from the simulation and ownership of platform is released.
// virtual
void WsfFrameStepSimulation::DeletePlatform(double aSimTime, WsfPlatform* aPlatformPtr, bool aDeleteMemory)
{
   // Remove the platform and associated subsystems from any list we are maintaining.

   std::vector<WsfPlatform*>::iterator pli = find(mPlatforms.begin(), mPlatforms.end(), aPlatformPtr);
   if (pli != mPlatforms.end())
   {
      mPlatforms.erase(pli);
   }

   // Make sure other lists are updated. We do this even if the platform wasn't being tracked
   // by us... just to make sure...

   for (WsfComponentList::RoleIterator<wsf::comm::Comm> iter(*aPlatformPtr); !iter.AtEnd(); ++iter)
   {
      Erase(mComms, *iter);
   }
   for (WsfComponentList::RoleIterator<WsfProcessor> iter(*aPlatformPtr); !iter.AtEnd(); ++iter)
   {
      Erase(mProcessors, *iter);
   }
   for (WsfComponentList::RoleIterator<WsfSensor> iter(*aPlatformPtr); !iter.AtEnd(); ++iter)
   {
      Erase(mSensors, *iter);
   }

   // Now let the base class delete the platform.
   WsfSimulation::DeletePlatform(aSimTime, aPlatformPtr, aDeleteMemory);

   if (MultiThreaded())
   {
      GetMultiThreadManager().PlatformDeleted(aSimTime, aPlatformPtr);
   }
}

// =================================================================================================
// virtual
void WsfFrameStepSimulation::Initialize()
{
   // Our member data must be called before the base class Initialize() because the
   // base class adds the initial platforms to the simulation, which causes our member
   // data to be updated!

   mPlatforms.clear();
   mComms.clear();
   mProcessors.clear();
   mSensors.clear();

   mTimestep = GetFrameTime(); // Define the timestep in the base class
   // Disable the minimum mover timestep because the simulation timestep effectively defines it.
   mMinimumMoverTimestep = 0.0;
   mFrameCount           = 0.0;
   mNextFrameTime        = 0.0;
   mTimeUntilNextFrame   = 0.0;
   mTotalFrameUnderTime  = 0.0;
   mTotalFrameUnderCount = 0.0;
   mTotalFrameOverTime   = 0.0;
   mTotalFrameOverCount  = 0.0;
   mSkippedFrameCount    = 0.0;
   mWorstFrameOverTime   = 0.0;

   if (MultiThreaded())
   {
      // Initialize the thread manager
      GetMultiThreadManager().Initialize();
   }

   // Now we can call the base class.
   WsfSimulation::Initialize();
}

// =================================================================================================
// virtual
void WsfFrameStepSimulation::Start()
{
   WsfSimulation::Start();
   if (!IsExternallyStarted())
   {
      mClockSourcePtr->StartClock();
   }
   mFrameCount           = 0.0;
   mTotalFrameUnderTime  = 0.0;
   mTotalFrameUnderCount = 0.0;
   mTotalFrameOverTime   = 0.0;
   mTotalFrameOverCount  = 0.0;
   mSkippedFrameCount    = 0.0;
   mWorstFrameOverTime   = 0.0;
}

// =================================================================================================
// virtual
void WsfFrameStepSimulation::Complete(double aSimTime)
{
   if (MultiThreaded())
   {
      GetMultiThreadManager().Complete(aSimTime);
   }

   WsfSimulation::Complete(aSimTime);

   if (mIsRealTime && (mFrameCount > 0.0))
   {
      double avgTimeUnder = mTotalFrameUnderTime / std::max(mTotalFrameUnderCount, 1.0);
      double avgPctUtil   = (GetFrameTime() - avgTimeUnder) / GetFrameTime();
      auto   out          = ut::log::info() << "Frame step simulation complete.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Total Frames: " << mFrameCount;
      out.AddNote() << "Avg. Frame Utilization: " << avgPctUtil * 100.0 << "%";
      if (mTotalFrameOverCount > 0.0)
      {
         out.AddNote() << "Average Time Over /Frame: " << mTotalFrameOverTime / mTotalFrameOverCount;
         out.AddNote() << "# Frames Over: " << mTotalFrameOverCount;
         out.AddNote() << "# Frames Skipped: " << mSkippedFrameCount;
         out.AddNote() << "Worst Frame Over: " << mWorstFrameOverTime;
      }
   }
}

// =================================================================================================
// virtual
bool WsfFrameStepSimulation::TurnPartOffP(double aSimTime, WsfPlatformPart* aPartPtr)
{
   if (WsfSimulation::TurnPartOffP(aSimTime, aPartPtr))
   {
      switch (aPartPtr->GetPartType())
      {
      case cCOMPONENT_ROLE<wsf::comm::Comm>():
         Erase(mComms, static_cast<wsf::comm::Comm*>(aPartPtr));
         break;
      case cCOMPONENT_ROLE<WsfProcessor>():
         Erase(mProcessors, static_cast<WsfProcessor*>(aPartPtr));
         break;
      case cCOMPONENT_ROLE<WsfSensor>():
         Erase(mSensors, static_cast<WsfSensor*>(aPartPtr));
         if (MultiThreaded())
         {
            GetMultiThreadManager().TurnSensorOff(aSimTime, static_cast<WsfSensor*>(aPartPtr));
         }
         break;
      default:
         break;
      }
      return true;
   }
   return false;
}

// =================================================================================================
// virtual
bool WsfFrameStepSimulation::TurnPartOnP(double aSimTime, WsfPlatformPart* aPartPtr)
{
   if (WsfSimulation::TurnPartOnP(aSimTime, aPartPtr))
   {
      switch (aPartPtr->GetPartType())
      {
      case cCOMPONENT_ROLE<wsf::comm::Comm>():
         Insert(mComms, static_cast<wsf::comm::Comm*>(aPartPtr));
         break;
      case cCOMPONENT_ROLE<WsfProcessor>():
         Insert(mProcessors, static_cast<WsfProcessor*>(aPartPtr));
         break;
      case cCOMPONENT_ROLE<WsfSensor>():
         if (!static_cast<WsfSensor*>(aPartPtr)->IsSlave())
         {
            Insert(mSensors, static_cast<WsfSensor*>(aPartPtr));
         }
         if (MultiThreaded())
         {
            GetMultiThreadManager().TurnSensorOn(aSimTime, static_cast<WsfSensor*>(aPartPtr));
         }
         break;
      default:
         break;
      }
      return true;
   }
   return false;
}

// =================================================================================================
//! Set the optional real-time mode of the simulation.
//! @param aSimTime    The current simulation time.
//! @param aIsRealTime  Whether we desire real-time behavior.
// virtual
void WsfFrameStepSimulation::SetRealtime(double aSimTime, bool aIsRealTime)
{
   if (mIsRealTime != aIsRealTime)
   {
      mIsRealTime = aIsRealTime;
      CreateClock();

      mClockSourcePtr->SetClock(aSimTime);

      if (!IsExternallyStarted())
      {
         mClockSourcePtr->StartClock();
      }
      else
      {
         ut::log::info() << "Waiting for simulation clock to start...";
      }
   }
}

// =================================================================================================
//! Wait from the start of the frame.
//!
//! The caller has done all the processing they require for the current frame and
//! simply wants to relinquish the processor until the start of the next frame.
// virtual
void WsfFrameStepSimulation::WaitForAdvanceTime()
{
   if (!mClockSourcePtr->IsStopped())
   {
      double clockTime = mClockSourcePtr->GetClock(mNextFrameTime);
      double sleepTime = mNextFrameTime - clockTime;
      if (sleepTime > 0.0)
      {
         // The following logic provides better timing when running on a system that
         // isn't really set up for 'real-time'.  In Windows, relinquishing the CPU
         // means that you may not get it back EXACTLY when you want to (it may give
         // control to a higher priority thread and it may not give it back in time).
         //
         // So, what we do is sleep for all but the last 4 ms and then burn up the
         // remainder in a loop.  This isn't very elegant and is a waste of CPU but it
         // gives better results on a machine that may not be dedicated to real-time work.

         if (sleepTime > 0.004)
         {
            sleepTime -= 0.004;
            UtSleep::Sleep(sleepTime);
         }

         // Burn off any residual time by using the CPU.
         while (mClockSourcePtr->GetClock(mNextFrameTime) < mNextFrameTime)
         {
         }
      }
      mRealTime = mNextFrameTime;
   }
   else
   {
      UtSleep::Sleep(0.004);
   }
}

double WsfFrameStepSimulation::GetFrameTime() const
{
   return mFrameStepInput->mFrameTime;
}
