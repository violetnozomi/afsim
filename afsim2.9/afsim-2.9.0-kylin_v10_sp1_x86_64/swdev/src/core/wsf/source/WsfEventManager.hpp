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

#ifndef WSFEVENTMANAGER_HPP
#define WSFEVENTMANAGER_HPP

#include "wsf_export.h"

#include <memory>
#include <mutex>
#include <queue>
#include <tuple>

#include "WsfEvent.hpp"
class WsfSimulation;

//! A manager for a time-ordered queue (strict weak ordering) of WsfEvent objects
//!
//! This class is used by WsfSimulation to implement an event queue.
//! It could also be used by other classes that want to retain their
//! own time-ordered event queues.
//!
//! The event manager operates as a queue with ordering primarily dictated by
//! the indicated simulation execution time of the event. Events occurring at the
//! same discrete time are further ordered by a priority value. In most use cases,
//! the priority value is default assigned such that all events added to the queue
//! at the same time observe a first-in-first-out behavior. However, priority may
//! be modified to change the relative ordering of same-time events.
class WSF_EXPORT WsfEventManager
{
public:
   WsfEventManager(WsfSimulation& aSimulation);
   WsfEventManager(const WsfEventManager& aSrc) = delete;
   WsfEventManager& operator=(const WsfEventManager& aRhs) = delete;
   WsfEventManager(WsfEventManager&&)                      = default;
   WsfEventManager& operator=(WsfEventManager&&) = default;
   virtual ~WsfEventManager()                    = default;

   //! Add an event to the event queue.
   //!
   //! @param aEventPtr Pointer to the event to be added to the queue.
   //! The WsfEvent::GetTime method will be used to determine when the event should be dispatched.
   //! The event manager becomes the owner of the event.
   //!
   //! Events added are ordered by simulation time. In the case of events with the same simulation
   //! time, events are then ordered by priority. Events with the same simulation time and priority
   //! are executed in a first-in-first-out manner via an incrementing counter.
   virtual void AddEvent(std::unique_ptr<WsfEvent> aEventPtr);

   //! Get, but do not remove, the next event that should be dispatched
   //!
   //! The next event is the one with the lowest time value.
   //! In cases of equivalent time value, the next event is the one with the lowest priority.
   //! In cases of equivalent time and priority, the next event is the one that was added
   //! to the event manager first.
   //!
   //! @return A non-owning pointer to the next event or nullptr if there is no next event.
   virtual WsfEvent* PeekEvent() const;

   //! Get and remove the next event that should be dispatched
   //!
   //! The next event is the one with the lowest time value.
   //! In cases of equivalent time value, the next event is the one with the lowest priority.
   //! In cases of equivalent time and priority, the next event is the one that was added
   //! to the event manager first.
   //!
   //! @return The pointer to the next event or nullptr if there is no next event. Caller assumes ownership of the event.
   virtual std::unique_ptr<WsfEvent> PopEvent();

   virtual void Reset();

   //! Event struct used for storing a key that allows for stable ordering in the priority_queue
   //! events are sorted by time first, then by priority, and then by the unsigned integer counter
   //! if times and priorities are equal.
   struct Event
   {
      using Key = std::tuple<double, int, unsigned int>;
      Event(const Key& aKey, std::unique_ptr<WsfEvent> aEventPtr)
         : mKey(aKey)
         , mEventPtr(std::move(aEventPtr))
      {
      }

      bool operator>(const Event& rhs) const { return mKey > rhs.mKey; }

      Key                               mKey;
      mutable std::unique_ptr<WsfEvent> mEventPtr; // Need mutable access through top() (which returns const reference)
                                                   // in order to move
   };

protected:
   mutable std::recursive_mutex mMutex{};

private:
   using EventQueue = std::priority_queue<Event, std::vector<Event>, std::greater<Event>>; // Smallest element at the top
   WsfSimulation& mSimulation;
   EventQueue     mEvents{};
   unsigned int   mCounter{0U};
};

#endif
