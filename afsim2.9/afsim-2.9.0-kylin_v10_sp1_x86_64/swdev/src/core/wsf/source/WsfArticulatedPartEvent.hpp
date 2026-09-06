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

#ifndef WSFARTICULATEDPARTEVENT_HPP
#define WSFARTICULATEDPARTEVENT_HPP

#include "wsf_export.h"

#include <cstddef> // for size_t

class WsfArticulatedPart;
#include "WsfEvent.hpp"

//! An event to process articulated part updates.
//!
//! This event is queued by WsfEventStepSimulation (or anything that wants to
//! behave like it).

class WSF_EXPORT WsfArticulatedPartEvent : public WsfEvent
{
public:
   enum EventType
   {
      cUPDATE_ARTICULATION //!< Call the UpdatePosition method on the articulated part
   };

   WsfArticulatedPartEvent(double aSimTime, EventType aEventType, WsfArticulatedPart* aArticulatedPartPtr);

   EventDisposition Execute() override;

private:
   EventType           mEventType;
   size_t              mPlatformIndex;
   WsfArticulatedPart* mArticulatedPartPtr;
   unsigned int        mArticulationUpdateEventEpoch;
};

#endif
