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

// VaEvent is an abstract base class for an event.  The derived class MUST
// supply the Execute() method which will be called when the event is dispatched.

#include "VaEvent.hpp"

namespace vespa
{
//! Create an event with no specified dispatch time.
//! @note VaEvent::SetTime must be called prior to adding the event to queue.
VaEvent::VaEvent()
   : mSimTime(0.0)
   , mValid(true)
   , mTimeType(cFORWARD)
{
}

//! Create an event with a specified dispatch time.
//! @param aSimTime The simulation time when the event should be dispatched.
VaEvent::VaEvent(double aSimTime, TimeType aTimeType)
   : mSimTime(aSimTime)
   , mValid(true)
   , mTimeType(aTimeType)
{
}

// virtual
VaEvent::~VaEvent() {}
} // namespace vespa
