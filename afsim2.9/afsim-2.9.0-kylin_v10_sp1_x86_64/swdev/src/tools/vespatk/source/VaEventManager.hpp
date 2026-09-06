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

#ifndef VaEventManager_hpp
#define VaEventManager_hpp

#include "vespatk_export.h"

#include <queue>
#include <stddef.h> // for size_t
#include <vector>

#include "VaEvent.hpp"

namespace vespa
{
//! A manager for a time-ordered queue of VaEvent objects
//!
//! This class is used by VaEnvironment to implement an event queue.
//! It could also be used by other classes that want to retain their
//! own time-ordered event queues.
class VESPATK_EXPORT VaEventManager
{
public:
   VaEventManager();
   virtual ~VaEventManager();

   //! Add an event to the event queue.
   //!
   //! @param aEventPtr Pointer to the event to be added to the queue.
   //! The VaEvent::GetTime method will be used to determine when the event should be dispatched.
   //! The event manager becomes the owner of the event.
   //!
   //! If multiple events are queued with the same time, the order in which
   //! those events are dispatched is not guaranteed to be the same as the
   //! order of insertion.  The only guarantee is that events with a smaller
   //! time will be dispatched before those with a greater time.
   void AddEvent(VaEvent* aEventPtr);

   //! Get, but do not remove, the next event that should be dispatched
   //!
   //! The next event is the one with the lowest time value.
   //!
   //! @return The pointer to the next event or 0 if there is no next event.
   VaEvent* PeekEvent();

   //! Get and remove the next event that should be dispatched
   //!
   //! The next event is the one with the lowest time value.
   //!
   //! @return The pointer to the next event or 0 if there is no next event.
   VaEvent* PopEvent();

   void Reset();

   virtual void DispatchEvents(double aTime) = 0;

   size_t Size() { return mEvents.size(); }

protected:
   //! Copy constructor declared protected to prevent use.
   VaEventManager(const VaEventManager& aSrc) {}

   //! Assignment operator declared protected to prevent use.
   VaEventManager& operator=(const VaEventManager& aRhs) { return *this; }

   //! Binary predicate for insert events into the priority queue in the proper order.
   //! The queue is ordered in increasing time.
   //!
   //! @note Events with the same time are NOT dispatched in order.
   //!
   //! The predicate should return 'true' if the left hand side has less
   //! priority (greater time) than the right hand side.
   class TimeCompare
   {
   public:
      TimeCompare()
         : mDummy(0)
      {
      }
      bool operator()(VaEvent* aLhsPtr, VaEvent* aRhsPtr) const { return (aLhsPtr->GetTime() > aRhsPtr->GetTime()); }

   private:
      int mDummy;
   };

   std::priority_queue<VaEvent*, std::vector<VaEvent*>, TimeCompare> mEvents;
};

class VESPATK_EXPORT VaSimTimeEventManager : public VaEventManager
{
public:
   VaSimTimeEventManager();
   ~VaSimTimeEventManager() override;

   void DispatchEvents(double aSimTime) override;

private:
   double mPrevEventTime;
};

class VESPATK_EXPORT VaRealTimeEventManager : public VaEventManager
{
public:
   VaRealTimeEventManager();
   ~VaRealTimeEventManager() override;

   void DispatchEvents(double aTime) override;
};
} // end namespace vespa
#endif
