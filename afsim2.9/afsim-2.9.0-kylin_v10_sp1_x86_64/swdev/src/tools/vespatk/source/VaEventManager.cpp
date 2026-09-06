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

// VaEventManager maintains an event queue.

#include "VaEventManager.hpp"

#include <iostream>

#include "VaEvent.hpp"

namespace vespa
{
VaEventManager::VaEventManager()
   : mEvents()
{
}

// virtual
VaEventManager::~VaEventManager()
{
   Reset();
}

void VaEventManager::AddEvent(VaEvent* aEventPtr)
{
   mEvents.push(aEventPtr);
}

//! Reset the event queue back to an empty state.
void VaEventManager::Reset()
{
   while (!mEvents.empty())
   {
      VaEvent* top = mEvents.top();
      mEvents.pop();
      delete top;
   }
}

VaEvent* VaEventManager::PeekEvent()
{
   VaEvent* eventPtr = nullptr;
   if (!mEvents.empty())
   {
      eventPtr = mEvents.top();
   }
   return eventPtr;
}

VaEvent* VaEventManager::PopEvent()
{
   VaEvent* eventPtr = nullptr;
   if (!mEvents.empty())
   {
      eventPtr = mEvents.top();
      mEvents.pop();
   }
   return eventPtr;
}

VaSimTimeEventManager::VaSimTimeEventManager()
   : mPrevEventTime(0.0)
{
}

VaSimTimeEventManager::~VaSimTimeEventManager() {}

void VaSimTimeEventManager::DispatchEvents(double aSimTime)
{
   VaEvent* eventPtr = PeekEvent();

   std::vector<VaEvent*> pastEvents;
   std::vector<VaEvent*> rescheduledEvents;
   double                minTime = (aSimTime > mPrevEventTime) ? mPrevEventTime : aSimTime;
   double                maxTime = (aSimTime > mPrevEventTime) ? aSimTime : mPrevEventTime;

   while ((eventPtr != nullptr) && (eventPtr->GetTime() <= maxTime))
   {
      PopEvent();

      if (eventPtr->GetTime() > minTime)
      {
         if (aSimTime > mPrevEventTime)
         {
            if ((eventPtr->GetTimeType() == VaEvent::cFORWARD) || (eventPtr->GetTimeType() == VaEvent::cBIDIRECTIONAL))
            {
               VaEvent::EventDisposition disposition = eventPtr->Execute();
               if (disposition == VaEvent::cDELETE)
               {
                  delete eventPtr;
                  eventPtr = nullptr;
               }
               else if (disposition == VaEvent::cRESCHEDULE)
               {
                  rescheduledEvents.push_back(eventPtr);
               }
            }
            else
            {
               // reverse events are not handled when going forward
               rescheduledEvents.push_back(eventPtr);
            }
         }
         else
         {
            if ((eventPtr->GetTimeType() == VaEvent::cREVERSE) || (eventPtr->GetTimeType() == VaEvent::cBIDIRECTIONAL))
            {
               VaEvent::EventDisposition disposition = eventPtr->Execute();
               if (disposition == VaEvent::cDELETE)
               {
                  delete eventPtr;
                  eventPtr = nullptr;
               }
               else if (disposition == VaEvent::cRESCHEDULE)
               {
                  rescheduledEvents.push_back(eventPtr);
               }
            }
            else
            {
               // forward events are not handled when going reverse
               rescheduledEvents.push_back(eventPtr);
            }
         }
      }
      else
      {
         pastEvents.push_back(eventPtr);
      }
      eventPtr = PeekEvent();
   }

   mPrevEventTime = aSimTime;

   // Add each of the rescheduled events back to the event queue.
   for (std::vector<VaEvent*>::iterator it = rescheduledEvents.begin(); it != rescheduledEvents.end(); ++it)
   {
      AddEvent(*it);
   }

   // Add each of the past events back to the event queue.
   for (std::vector<VaEvent*>::iterator it = pastEvents.begin(); it != pastEvents.end(); ++it)
   {
      AddEvent(*it);
   }
}

VaRealTimeEventManager::VaRealTimeEventManager() {}

VaRealTimeEventManager::~VaRealTimeEventManager() {}

void VaRealTimeEventManager::DispatchEvents(double aTime)
{
   VaEvent* eventPtr = PeekEvent();

   while ((eventPtr != nullptr) && (eventPtr->GetTime() <= aTime))
   {
      PopEvent();
      VaEvent::EventDisposition disposition = eventPtr->Execute();
      if (disposition == VaEvent::cDELETE)
      {
         delete eventPtr;
         eventPtr = nullptr;
      }
      else if (disposition == VaEvent::cRESCHEDULE)
      {
         AddEvent(eventPtr);
      }
      eventPtr = PeekEvent();
   }
}
} // namespace vespa
