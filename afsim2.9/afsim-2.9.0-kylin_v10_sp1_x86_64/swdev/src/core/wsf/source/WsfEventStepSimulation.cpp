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

// WsfEventStepSimulation is a specialization of WsfSimulation that implements
// an event-stepped simulation.

#include "WsfEventStepSimulation.hpp"

#include <cassert>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtRandom.hpp"
#include "UtSleep.hpp"
#include "WsfClockSource.hpp"
#include "WsfComm.hpp"
#include "WsfCommEvent.hpp"
#include "WsfEventManager.hpp"
#include "WsfMover.hpp"
#include "WsfMoverUpdateEvent.hpp"
#include "WsfMultiThreadManager.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformPartEvent.hpp"
#include "WsfProcessor.hpp"
#include "WsfRealTimeClockSource.hpp"
#include "WsfScenario.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSimulationInput.hpp"
#include "WsfSimulationObserver.hpp"

namespace
{
//! An internal event scheduled to perform a multi-threaded simulation update.

class ThreadUpdateEvent : public WsfEvent
{
public:
   ThreadUpdateEvent(double aSimTime, double aUpdateInterval, int aPlatformUpdateMultiplier, int aSensorUpdateMultiplier)
      : WsfEvent(aSimTime)
      , mUpdateInterval(aUpdateInterval)
      , mPlatformUpdateMultiplier(aPlatformUpdateMultiplier)
      , mSensorUpdateMultiplier(aSensorUpdateMultiplier)
      , mPlatformUpdateCounter(0)
      , mSensorUpdateCounter(0)
   {
   }

   ~ThreadUpdateEvent() override {}

   EventDisposition Execute() override
   {
      WsfObserver::FrameStarting(GetSimulation())(GetTime());
      if ((mPlatformUpdateCounter < 1) || (mPlatformUpdateCounter >= mPlatformUpdateMultiplier))
      {
         GetSimulation()->GetMultiThreadManager().UpdatePlatforms(GetTime());
         mPlatformUpdateCounter = 0;
      }

      if ((mSensorUpdateCounter < 1) || (mSensorUpdateCounter >= mSensorUpdateMultiplier))
      {
         GetSimulation()->GetMultiThreadManager().UpdateSensors(GetTime());
         mSensorUpdateCounter = 0;
      }
      ++mPlatformUpdateCounter;
      ++mSensorUpdateCounter;
      WsfObserver::FrameComplete(GetSimulation())(GetTime());

      SetTime(GetTime() + mUpdateInterval);
      if (GetSimulation()->IsRealTime() && ((GetTime() + mUpdateInterval) < GetSimulation()->GetRealTime()))
      {
         ut::log::info() << "Reset Thread Update Event to real-time";
         SetTime(GetSimulation()->GetRealTime() + mUpdateInterval);
      }
      return cRESCHEDULE;
   }

private:
   double mUpdateInterval;
   int    mPlatformUpdateMultiplier;
   int    mSensorUpdateMultiplier;

   int mPlatformUpdateCounter;
   int mSensorUpdateCounter;
};
} // namespace

WsfEventStepSimulation::WsfEventStepSimulation(WsfScenario& aScenario, unsigned int aRunNumber)
   : WsfSimulation(aScenario, aRunNumber)
   , mPreviouslyBehind(false)
{
   mEventStepInput = &dynamic_cast<WsfEventStepSimulationInput&>(aScenario.GetSimulationInput());
   SetAmAnEventStepSimulation(true);
}

// virtual
WsfEventStepSimulation::~WsfEventStepSimulation() {}

// virtual
void WsfEventStepSimulation::Complete(double aSimTime)
{
   if (MultiThreaded())
   {
      GetMultiThreadManager().Complete(aSimTime);
   }
   WsfSimulation::Complete(aSimTime);
}

// protected virtual
void WsfEventStepSimulation::CreateClock()
{
   if (mIsRealTime)
   {
      SetClockSource(ut::make_unique<WsfRealTimeClockSource>());
   }
   else
   {
      SetClockSource(ut::make_unique<WsfClockSource>());
   }
}

// virtual
void WsfEventStepSimulation::Initialize()
{
   // If running real-time, set the minimum mover timestep if one hasn't been defined.
   if (mIsRealTime && (mMinimumMoverTimestep < 0.0))
   {
      mMinimumMoverTimestep = 0.050; // 20 Hz
   }

   WsfSimulation::Initialize();

   if (MultiThreaded() && mEventStepInput->mThreadUpdateInterval > 0.0)
   {
      // Initialize the thread manager
      GetMultiThreadManager().Initialize();

      // Add event to update the thread manager, delay a bit so the platform and sensors get added.
      AddEvent(ut::make_unique<ThreadUpdateEvent>(GetSimTime() + 1.0E-15,
                                                  mEventStepInput->mThreadUpdateInterval,
                                                  mEventStepInput->mPlatformThreadUpdateMultiplier,
                                                  mEventStepInput->mSensorThreadUpdateMultiplier));
   }
}

/** Set the optional real-time mode of the simulation.
  @param aSimTime    The current simulation time.
  @param aIsRealTime  Whether we desire real-time behavior.
*/

// virtual
void WsfEventStepSimulation::SetRealtime(double aSimTime, bool aIsRealTime)
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

// protected virtual
bool WsfEventStepSimulation::PlatformInitialized(double aSimTime, WsfPlatform* aNewPlatformPtr)
{
   if (!MultiThreaded())
   {
      // If the mover has a required update interval then schedule the event to perform the update.
      WsfMover* moverPtr = aNewPlatformPtr->GetMover();
      if ((moverPtr != nullptr) && (moverPtr->GetUpdateInterval() > 0.0))
      {
         AddEvent(ut::make_unique<WsfMoverUpdateEvent>(aSimTime, aNewPlatformPtr));
      }
   }
   return true;
}

// protected virtual
bool WsfEventStepSimulation::PlatformIntroduced(double aSimTime, WsfPlatform* aNewPlatformPtr)
{
   if (MultiThreaded())
   {
      GetMultiThreadManager().PlatformIntroduced(aSimTime, aNewPlatformPtr);
   }
   return WsfSimulation::PlatformIntroduced(aSimTime, aNewPlatformPtr);
}

bool WsfEventStepSimulation::PlatformDeleted(double aSimTime, WsfPlatform* aOldPlatformPtr)
{
   if (MultiThreaded())
   {
      GetMultiThreadManager().PlatformDeleted(aSimTime, aOldPlatformPtr);
   }
   return WsfSimulation::PlatformDeleted(aSimTime, aOldPlatformPtr);
}

// virtual
bool WsfEventStepSimulation::TurnPartOff(double aSimTime, WsfPlatformPart* aPartPtr)
{
   bool ok = false;
   if (aPartPtr->IsTurnedOn())
   {
      AddEvent(ut::make_unique<WsfPlatformPartEvent>(aSimTime, WsfPlatformPartEvent::cTURN_OFF, aPartPtr));
      ok = true;
   }
   return ok;
}

// virtual
bool WsfEventStepSimulation::TurnPartOn(double aSimTime, WsfPlatformPart* aPartPtr)
{
   bool ok = false;
   if (aPartPtr->IsOperational() && (!aPartPtr->IsTurnedOn()))
   {
      // Randomize sensor turn on time for sensors added at simulation start, when
      // running real-time and multi-threaded to level out sensor updates over time.
      if ((aPartPtr->GetPartType() == cCOMPONENT_ROLE<WsfSensor>()) && MultiThreaded() && (aSimTime <= 1.0E-5) &&
          IsRealTime())
      {
         WsfSensor* sensorPtr = static_cast<WsfSensor*>(aPartPtr);
         aSimTime += GetRandom().Uniform(0.0, sensorPtr->GetCurrentMode()->GetFrameTime());
      }
      AddEvent(ut::make_unique<WsfPlatformPartEvent>(aSimTime, WsfPlatformPartEvent::cTURN_ON, aPartPtr));
      ok = true;
   }
   return ok;
}


// virtual
void WsfEventStepSimulation::SetPartUpdateInterval(double aSimTime, WsfPlatformPart* aPartPtr, double aUpdateInterval)
{
   aPartPtr->SetUpdateInterval(aUpdateInterval);

   // If the processor is off there is no reason to schedule an event because it will get scheduled when the
   // processor gets turned on. If the call is coming from within the periodic call to Update() there is no reason
   // schedule a new event because the current event will reschedule with the proper interval (see WsfPlatformPartEvent).
   if (aPartPtr->IsTurnedOn() && (!aPartPtr->UpdateEventActive()))
   {
      aPartPtr->IncrementUpdateEventEpoch(); // Invalidate outstanding 'update' events
      if (aUpdateInterval > 0.0)
      {
         if ((aPartPtr->GetPartType() != cCOMPONENT_ROLE<WsfSensor>()) || !static_cast<WsfSensor*>(aPartPtr)->IsSlave())
         {
            AddEvent(ut::make_unique<WsfPlatformPartEvent>(aSimTime, WsfPlatformPartEvent::cUPDATE, aPartPtr));
         }
      }
   }
}

//! Wait for the next event to occur.
//! If running real-time and the real-time clock is running, impose a delay if the next event is too far in the future.
//! The purpose of this routine is simply to avoid having a real-time application consume all the processor time while
//! simply waiting for events. If the next event is 'too far' in the future, it delays a 'short' period of time and
//! returns to the caller. It does not try to burn up all the time because external activities may cause new events to
//! be inserted earlier in the queue.
//!
//! @note This routine does nothing unless the running real-time and the real-time clock is running.
// virtual
void WsfEventStepSimulation::WaitForAdvanceTime()
{
   if (mIsRealTime)
   {
      //       if (sClockSourcePtr->IsStopped())
      //       {
      //          // Clock is not running... just sleep a bit and return.
      //          UtSleep::Sleep(0.001);
      //       }
      //       else
      {
         double    nextEventTime = 1.0E+30;
         WsfEvent* eventPtr      = mEventManager.PeekEvent();
         if (eventPtr != nullptr)
         {
            nextEventTime = eventPtr->GetTime();
         }
         mRealTime        = mClockSourcePtr->GetClock(1.0E+37);
         double sleepTime = nextEventTime - mRealTime;
         if (sleepTime >= 0.0)
         {
            mTimeBehind = mRealTime - GetSimTime(); // sim time is no longer behind real time

            // Convert to real-time
            sleepTime /= GetClockRate();

            if (mClockSourcePtr->IsStopped())
            {
               sleepTime = std::max(0.001, sleepTime);
            }

            // The following logic provides better timing when running on a system that
            // isn't really set up for 'real-time'.  In Windows, relinquishing the CPU
            // means that you may not get it back EXACTLY when you want to (it may give
            // control to a higher priority thread and it may not give it back in time).
            //
            // If the sleep time is greater than 4 ms then we go ahead and give up the
            // CPU for 1 ms. This isn't very elegant and is a waste of CPU but it give
            // better results on a machine that may not be dedicated to real-time work.

            if (sleepTime > 0.004)
            {
               UtSleep::SleepMinimum();
               mRealTime = mClockSourcePtr->GetClock(1.0E+37);
            }

            if (mPreviouslyBehind)
            {
               // Notify observers we are no longer behind
               WsfObserver::SimulationTimeBehind(this)(0.0);
               mPreviouslyBehind = false;
            }
         }
         else
         {
            mTimeBehind = -sleepTime;
            // Notify observers we are behind
            WsfObserver::SimulationTimeBehind(this)(mTimeBehind);
            mPreviouslyBehind = true;
         }
      }
   }
}
