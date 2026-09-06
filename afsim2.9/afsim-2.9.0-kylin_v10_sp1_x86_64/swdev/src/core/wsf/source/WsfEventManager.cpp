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

// WsfEventManager maintains an event queue.

#include "WsfEventManager.hpp"

WsfEventManager::WsfEventManager(WsfSimulation& aSimulation)
   : mSimulation(aSimulation)
{
}

//! Add an event to the event queue.
void WsfEventManager::AddEvent(std::unique_ptr<WsfEvent> aEventPtr)
{
   aEventPtr->AddedToEventQueue(mSimulation);
   auto                                  time     = aEventPtr->GetTime();
   auto                                  priority = aEventPtr->GetPriority();
   std::lock_guard<std::recursive_mutex> lock(mMutex);
   mEvents.emplace(std::make_tuple(time, priority, mCounter++), std::move(aEventPtr));
}

//! Get, but do not remove, the next event that should be dispatched
WsfEvent* WsfEventManager::PeekEvent() const
{
   WsfEvent*                             eventPtr = nullptr;
   std::lock_guard<std::recursive_mutex> lock(mMutex);
   if (!mEvents.empty())
   {
      eventPtr = mEvents.top().mEventPtr.get();
   }
   return eventPtr;
}

//! Get and remove the next event that should be dispatched
std::unique_ptr<WsfEvent> WsfEventManager::PopEvent()
{
   std::unique_ptr<WsfEvent>             eventPtr{nullptr};
   std::lock_guard<std::recursive_mutex> lock(mMutex);
   if (!mEvents.empty())
   {
      eventPtr = std::move(mEvents.top().mEventPtr); // Can move only because mEventPtr is declared mutable
      mEvents.pop();
   }
   return eventPtr;
}

//! Reset the event queue back to an empty state.
void WsfEventManager::Reset()
{
   std::lock_guard<std::recursive_mutex> lock(mMutex);
   mEvents = EventQueue();
}
