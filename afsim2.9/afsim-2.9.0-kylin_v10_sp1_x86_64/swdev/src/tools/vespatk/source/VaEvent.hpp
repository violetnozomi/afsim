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

#ifndef VaEvent_hpp
#define VaEvent_hpp

#include "vespatk_export.h"

namespace vespa
{
//! A abstract base class for all events within an event driven simulation.
//!
//! VaEvent represents events that are managed by VaEventManager.
//!
//! @note The derived class <b>must</b> supply an Execute() method which will
//! be called when the event is dispatched.

class VESPATK_EXPORT VaEvent
{
public:
   //! EventDisposition is the return value for the Execute method.
   //!
   //! The event disposition is used by VaEventManager to reschedule events
   //! that need to execute on a regular basis. This prevents the constant
   //! deletion and reallocation of events that need to execute repeatedly.
   enum EventDisposition
   {
      cDELETE,    //!< Delete the event from the event queue
      cRESCHEDULE //!< Reschedule the event. VaEvent::SetTime should have been called with the new time.
   };

   enum TimeType
   {
      cFORWARD,
      cREVERSE,
      cBIDIRECTIONAL
   };

   VaEvent();
   VaEvent(double aSimTime, TimeType aTimeType = cFORWARD);
   virtual ~VaEvent();

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

   //! Set the time at which the event is to execute.
   //! @param aSimTime The time at which the event is to execute.
   //!
   //! @note This method should NEVER be called when the event is on the event
   //! queue as it will not get requeued at the correct time.
   void SetTime(double aSimTime) { mSimTime = aSimTime; }

   //! Sets whether the current event is considered valid.
   //! @param aValid A flag indicating whether the event is valid.
   void SetValid(bool aValid) { mValid = aValid; }

   //! Returns whether the event is valid.
   bool IsValid() const { return mValid; }

   TimeType GetTimeType() { return mTimeType; }

private:
   //! Copy constructor declared but not defined to prevent use.
   VaEvent(const VaEvent& aSrc);

   //! Assignment operator declared but not defined to prevent use.
   VaEvent& operator=(const VaEvent& aRhs);

   double mSimTime; //!< The simulation time when the event should be dispatched

   bool mValid;

   TimeType mTimeType;
};
} // end namespace vespa
#endif
