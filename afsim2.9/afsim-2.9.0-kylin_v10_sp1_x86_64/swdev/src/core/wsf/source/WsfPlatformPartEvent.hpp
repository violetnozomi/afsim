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

#ifndef WSFPLATFORMPARTEVENT_HPP
#define WSFPLATFORMPARTEVENT_HPP

#include "wsf_export.h"

#include <cstddef>

#include "UtReferenceCount.hpp"
#include "WsfEvent.hpp"
class WsfPlatformPart;

//! An event to process part updates and requests to turn a part on or off.
//!
//! This event is queued by WsfEventStepSimulation (or anything that wants to behave like it).
class WSF_EXPORT WsfPlatformPartEvent : public WsfEvent
{
public:
   enum EventType
   {
      cUPDATE,   //!< Call the sensors Update() method
      cTURN_OFF, //!< Turn the sensor off
      cTURN_ON   //!< Turn the sensor on
   };

   WsfPlatformPartEvent() = delete;
   WsfPlatformPartEvent(double aSimTime, EventType aEventType, WsfPlatformPart* aPartPtr);

   EventDisposition Execute() override;

private:
   EventType                        mEventType;
   size_t                           mPlatformIndex;
   UtWeakReference<WsfPlatformPart> mPartPtr;
   unsigned int                     mUpdateEventEpoch;
};

#endif
