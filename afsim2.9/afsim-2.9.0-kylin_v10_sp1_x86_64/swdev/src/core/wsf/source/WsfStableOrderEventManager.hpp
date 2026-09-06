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

#ifndef WSFSTABLEORDEREVENTMANAGER_HPP
#define WSFSTABLEORDEREVENTMANAGER_HPP

#include "wsf_export.h"

#include <mutex>
#include <queue>
#include <vector>

#include "WsfEventManager.hpp"

class WsfEvent;

//! A manager for a stable time-ordered queue of WsfEvent objects
//!
//! This class is used by WsfSimulation to implement an event queue.
//! It could also be used by other classes that want to retain their
//! own time-ordered event queues.
class WSF_EXPORT WsfStableOrderEventManager : public WsfEventManager
{
public:
   WsfStableOrderEventManager()
      : WsfEventManager()
      , mStableEvents()
   {
   }

   virtual ~WsfStableOrderEventManager() { Reset(); }

   typedef std::pair<WsfEvent*, unsigned int> StableEvent;

   //! Add an event to the event queue.
   //!
   //! @param aEventPtr Pointer to the event to be added to the queue.
   //! The WsfEvent::GetTime method will be used to determine when the event should be dispatched.
   //! The event manager becomes the owner of the event.
   //!
   //! If multiple events are queued with the same time, the order in which
   //! those events are dispatched is not guaranteed to be the same as the
   //! order of insertion.  The only guarantee is that events with a smaller
   //! time will be dispatched before those with a greater time.
   virtual void AddEvent(WsfEvent* aEventPtr)
   {
      mMutex.lock();
      mStableEvents.push(std::make_pair<WsfEvent*, unsigned int>(aEventPtr, mNextCount++));
      mMutex.unlock();
   }

   //! Get, but do not remove, the next event that should be dispatched
   //!
   //! The next event is the one with the lowest time value.
   //!
   //! @return The pointer to the next event or 0 if there is no next event.
   virtual WsfEvent* PeekEvent()
   {
      WsfEvent* eventPtr = 0;
      if (!mStableEvents.empty())
      {
         mMutex.lock();
         eventPtr = mStableEvents.top().first;
         mMutex.unlock();
      }
      return eventPtr;
   }

   //! Get and remove the next event that should be dispatched
   //!
   //! The next event is the one with the lowest time value.
   //!
   //! @return The pointer to the next event or 0 if there is no next event.
   virtual WsfEvent* PopEvent()
   {
      WsfEvent* eventPtr = 0;
      if (!mStableEvents.empty())
      {
         mMutex.lock();
         eventPtr = mStableEvents.top().first;
         mStableEvents.pop();
         mMutex.unlock();
      }
      return eventPtr;
   }

   //! Reset the event queue back to an empty state.
   virtual void Reset()
   {
      mMutex.lock();
      while (!mStableEvents.empty())
      {
         WsfEvent* eventPtr = mStableEvents.top().first;
         mStableEvents.pop();
         delete eventPtr;
      }
      mMutex.unlock();
      mNextCount = 0;
   }

private:
   //! Copy constructor declared but not defined to prevent use.
   WsfStableOrderEventManager(const WsfStableOrderEventManager& aSrc);

   //! Assignment operator declared but not defined to prevent use.
   WsfStableOrderEventManager& operator=(const WsfStableOrderEventManager& aRhs) = delete;

   //! Binary predicate for insert events into the priority queue in the proper order.
   //! The queue is ordered in increasing time (use unique counter id when time equal).
   //!
   //! @note Events with the same time ARE dispatched in order.
   //!
   //! The predicate should return 'true' if the left hand side has less
   //! priority (greater time) than the right hand side.

   class StableCompare
   {
   public:
      StableCompare()
         : mDummy(0)
      {
      }
      bool operator()(StableEvent aLhs, StableEvent aRhs) const
      {
         if (aLhs.first->GetTime() > aRhs.first->GetTime())
         {
            return true;
         }
         else if (aLhs.first->GetTime() < aRhs.first->GetTime())
         {
            return false;
         }
         else
         {
            return (aLhs.second > aRhs.second);
         }
      }

   private:
      int mDummy;
   };

   // std::priority_queue<WsfEvent*, std::vector<WsfEvent*>, StableCompare>    mEvents;
   // std::recursive_mutex mMutex;

   std::priority_queue<StableEvent, std::vector<StableEvent>, StableCompare> mStableEvents;
   static unsigned int                                                       mNextCount;
};

unsigned int WsfStableOrderEventManager::mNextCount = 0;

#endif
