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

#ifndef WSFEVENT_HPP
#define WSFEVENT_HPP

#include "wsf_export.h"

#include <functional>

class WsfSimulation;

//! A abstract base class for all events within an event driven simulation.
//!
//! WsfEvent represents events that are managed by WsfEventManager.
//!
//! @note The derived class <b>must</b> supply an Execute() method which will
//! be called when the event is dispatched.

class WSF_EXPORT WsfEvent
{
public:
   //! EventDisposition is the return value for the Execute method.
   //!
   //! The event disposition is used by WsfEventManager to reschedule events
   //! that need to execute on a regular basis. This prevents the constant
   //! deletion and reallocation of events that need to execute repeatedly.
   enum EventDisposition
   {
      cDELETE,    //!< Delete the event from the event queue
      cRESCHEDULE //!< Reschedule the event. WsfEvent::SetTime should have been called with the new time.
   };

   //! Create an event with no specified dispatch time.
   //! @note WsfEvent::SetTime must be called prior to adding the event to queue.
   WsfEvent() = default;

   //! Create an event with a specified dispatch time and optional priority.
   //! @param aSimTime The simulation time when the event should be dispatched.
   //! @param aPriority The priority associated with this event.
   WsfEvent(double aSimTime, int aPriority = 0)
      : mSimTime(aSimTime)
      , mPriority(aPriority)
   {
   }

   //! Copy constructor and copy assignment deleted to prevent copying.
   WsfEvent(const WsfEvent& aSrc) = delete;
   WsfEvent& operator=(const WsfEvent& aRhs) = delete;

   virtual ~WsfEvent() = default;

   //! Execute the specified event.
   //!
   //! @return The disposition of the event. This indicates to the event manager
   //! what should be done with the event. If cRESCHEDULE then Execute
   //! should have called SetTime to set the time for which the event manager
   //! should reschedule the event.
   virtual EventDisposition Execute() = 0;

   //! Get the time at which the event is to execute
   //! @return The simulation time at which the event should execute.
   double GetTime() const { return mSimTime; }

   //! Get the priority of the event, for correct execution of events occurring at the same discrete time.
   //! @return The priority of the event.
   int GetPriority() const { return mPriority; }

   //! Provides whether or not this event should be processed during simulation event execution.
   //! @return A boolean indicating if this event should be executed.
   bool ShouldExecute() const { return mExecute; }

   //! Set the time at which the event is to execute.
   //! @param aSimTime The time at which the event is to execute.
   //!
   //! @note This method should NEVER be called when the event is on the event
   //! queue as it will not get re-queued at the correct time.
   void SetTime(double aSimTime) { mSimTime = aSimTime; }

   //! Set the priority for correct order of execution of events at the same discrete time.
   //! @param aPriority The priority of the event.
   //!
   //! @note Priority is used to order events that occur at the same time in the event manager,
   //! and ordering is only enforced at the time the event is added. Changing priority
   //! after AddEvent() is called will not affect the order of execution of events.
   //! Changing priority will be enforced if an event is rescheduled after the resolution of Execute().
   void SetPriority(int aPriority) { mPriority = aPriority; }

   //! Sets whether or not this event should be executed, allowing an event to be effectively canceled.
   //! @param aExecute If this event should be executed.
   void SetShouldExecute(bool aExecute) { mExecute = aExecute; }

   //! Returns the simulation in which this event belongs.
   //! May be null if event was not added to a simulation
   WsfSimulation* GetSimulation() const { return mSimulationPtr; }

   //! Called when the event is added to the event queue of the specified simulation.
   void AddedToEventQueue(WsfSimulation& aSimulation) { mSimulationPtr = &aSimulation; }

private:
   double         mSimTime{0.0}; //!< The simulation time when the event should be dispatched
   WsfSimulation* mSimulationPtr{nullptr};
   int            mPriority{0};   //!< The priority associated with the event, for events occurring at the same time.
   bool           mExecute{true}; //!< Indicates if this event should be executed.
};

//! A class template that adapts the interface for WsfEvent to provide clients with a convenient way to create an event
//! by simply passing a std::function to the constructor. Sub-classed below for "one-shot" and recurring events.
template<typename RT, typename... Args>
class WsfEventAdapterT : public WsfEvent
{
protected:
   using ExecuteFn = std::function<RT(Args...)>;

   WsfEventAdapterT(double aSimTime, const ExecuteFn& aExecuteFn)
      : WsfEvent(aSimTime)
      , mExecuteFn(aExecuteFn)
   {
   }

   ~WsfEventAdapterT() override = default;

   ExecuteFn mExecuteFn;
};

//! A "one-shot" event that executes once and is deleted. Constructor takes a std::function<void()>
//! that will be called from Execute() when the event is dispatched.
//! Example usage:
//! simulationPtr->AddEvent(new WsfOneShotEvent(simTime, [=]() { ... }));
class WSF_EXPORT WsfOneShotEvent : public WsfEventAdapterT<void>
{
public:
   WsfOneShotEvent(double aSimTime, const ExecuteFn& aExecuteFn)
      : WsfEventAdapterT<void>(aSimTime, aExecuteFn)
   {
   }

   EventDisposition Execute() override
   {
      mExecuteFn();
      return cDELETE;
   }
};

//! An event that can be rescheduled. Constructor takes a std::function<EventDisposition(WsfEvent&)>.
//! A reference to "this" is passed to the std::function when called from Execute() to give it access
//! to the event. The std::function could then e.g. call SetTime to reschedule the event.
//! Example usage:
//! simulationPtr->AddEvent(new WsfRecurringEvent(simTime, [=](WsfEvent& e)
//! {
//!    ...
//!    e.SetTime(e.GetTime() + interval);
//!    return WsfEvent::cRESCHEDULE;
//! }));
class WSF_EXPORT WsfRecurringEvent : public WsfEventAdapterT<WsfEvent::EventDisposition, WsfEvent&>
{
public:
   WsfRecurringEvent(double aSimTime, const ExecuteFn& aExecuteFn)
      : WsfEventAdapterT<WsfEvent::EventDisposition, WsfEvent&>(aSimTime, aExecuteFn)
   {
   }

   EventDisposition Execute() override { return mExecuteFn(*this); }
};

#endif
