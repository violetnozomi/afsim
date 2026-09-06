// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "IntervalUpdater.hpp"

#include "WsfEvent.hpp"
#include "WsfSimulation.hpp"
#include "WsfSimulationObserver.hpp"

namespace WsfL16
{
namespace IntervalUpdaterPrivate
{
class UpdateEvent : public WsfEvent
{
public:
   UpdateEvent(IntervalUpdater* aIntervalUpdater, double aSimTime, double aUpdateInterval)
      : mIntervalUpdaterPtr(aIntervalUpdater)
      , mIsDisabled(false)
   {
      SetTime(aSimTime + aUpdateInterval);
   }

   EventDisposition Execute() override
   {
      if (mIsDisabled)
      {
         return WsfEvent::cDELETE;
      }
      SetTime(mIntervalUpdaterPtr->UpdatePrivate(GetTime()));
      return WsfEvent::cRESCHEDULE;
   }

   void Disable() { mIsDisabled = true; }

private:
   IntervalUpdater* mIntervalUpdaterPtr;
   bool             mIsDisabled;
};
} // namespace IntervalUpdaterPrivate
} // namespace WsfL16

namespace WsfL16
{

IntervalUpdater::IntervalUpdater()
   : mIsInitialized(false)
   , mLastUpdateTime(-1.0)
   , mUpdateInterval(1.0)
   , mEventPtr(nullptr)
   , mSimCompleteCallbackPtr(nullptr)
   , mSimulationPtr(nullptr)
{
}

IntervalUpdater::IntervalUpdater(double aSimTime)
   : mIsInitialized(false)
   , mUpdateInterval(aSimTime)
   , mEventPtr(nullptr)
   , mSimCompleteCallbackPtr(nullptr)
   , mSimulationPtr(nullptr)
{
}

IntervalUpdater::IntervalUpdater(const IntervalUpdater& aSrc)
   : mIsInitialized(false)
   , mUpdateInterval(aSrc.mUpdateInterval)
   , mEventPtr(nullptr)
   , mSimCompleteCallbackPtr(nullptr)
   , mSimulationPtr(aSrc.mSimulationPtr)
{
}

IntervalUpdater::~IntervalUpdater()
{
   DeleteEvent();
}

//! Begins creating the update events
void IntervalUpdater::Initialize(double aSimTime, WsfSimulation& aSimulation, bool aAutoStart /* =true */)
{
   mSimulationPtr  = &aSimulation;
   mIsInitialized  = true;
   mLastUpdateTime = aSimTime;
   if (aAutoStart)
   {
      StartUpdates(aSimTime);
   }
   mSimCompleteCallbackPtr =
      WsfObserver::SimulationComplete(mSimulationPtr).Connect(&IntervalUpdater::HandleSimulationComplete, this);
}

bool IntervalUpdater::ProcessInput(UtInput& aInput)
{
   bool myCommand = false;
   if (aInput.GetCommand() == "update_interval")
   {
      aInput.ReadValueOfType(mUpdateInterval, UtInput::cTIME);
      myCommand = true;
   }
   return myCommand;
}

// virtual
//  To be overridden
void IntervalUpdater::Update(double aSimTime) {}

double IntervalUpdater::UpdatePrivate(double aSimTime)
{
   OnUpdate(aSimTime);
   Update(aSimTime);
   mLastUpdateTime = aSimTime;
   if (mUpdateInterval <= 0)
   {
      DeleteEvent();
   }
   return aSimTime + mUpdateInterval;
}

void IntervalUpdater::StartUpdates(double aSimTime)
{
   auto updateEventPtr = ut::make_unique<WsfL16::IntervalUpdaterPrivate::UpdateEvent>(this, aSimTime, mUpdateInterval);
   mEventPtr           = updateEventPtr.get();
   mSimulationPtr->AddEvent(std::move(updateEventPtr));
}

void IntervalUpdater::SetUpdateInterval(double aUpdateInterval)
{
   mUpdateInterval = aUpdateInterval;
   if (mIsInitialized)
   {
      if (mUpdateInterval <= 0)
      {
         DeleteEvent();
      }
      else
      {
         if (mEventPtr == nullptr)
         {
            StartUpdates(mSimulationPtr->GetSimTime());
         }
      }
   }
}

void IntervalUpdater::DeleteEvent()
{
   if (mEventPtr != nullptr)
   {
      mEventPtr->Disable();
      mEventPtr = nullptr;
   }
}

//! Events are deleted when the simulation is complete, so dont try to
//! access mEventPtr
void IntervalUpdater::HandleSimulationComplete(double aSimTime)
{
   mEventPtr = nullptr;
}

void IntervalUpdater::TurnOn(double aSimTime)
{
   if (mEventPtr == nullptr)
   {
      StartUpdates(aSimTime);
   }
}

void IntervalUpdater::TurnOff(double aSimTime)
{
   DeleteEvent();
}

} // namespace WsfL16
