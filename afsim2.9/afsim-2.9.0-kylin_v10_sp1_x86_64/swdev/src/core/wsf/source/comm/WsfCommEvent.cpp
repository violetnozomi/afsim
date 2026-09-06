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

#include "WsfCommEvent.hpp"

#include <cassert>
#include <memory>

#include "UtMemory.hpp"
#include "WsfComm.hpp"
#include "WsfCommMessage.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"

namespace wsf
{
namespace comm
{

//! Create a Comm event.
//!
//! @param aSimTime   Time at which the event should dispatch.
//! @param aXmtrPtr   If aEventType == cDELIVER_MESSAGE, this must be the
//! pointer to the transmitting comm device. For all other events this must
//! be the pointer to the comm device to which the event applies.
Event::Event(double aSimTime, Comm* aXmtrPtr)
   : WsfEvent(aSimTime)
   , mXmtrPlatformIndex(0)
   , mRcvrPlatformIndex(0)
   , mXmtrPtr(aXmtrPtr)
   , mRcvrPtr(nullptr)
   , mCommMessagePtr(nullptr)
{
   assert(mXmtrPtr != nullptr);
   mXmtrPlatformIndex    = mXmtrPtr->GetPlatform()->GetIndex();
   mLastStatusChangeTime = mXmtrPtr->GetLastStatusChangeTime();
}

// virtual
Event::~Event()
{
   // Cleaned up here on the delivery
   delete mCommMessagePtr;
}

// virtual
WsfEvent::EventDisposition Event::Execute()
{
   EventDisposition eventDisposition = cDELETE;
   WsfPlatform*     xmtrPlatformPtr  = GetSimulation()->GetPlatformByIndex(mXmtrPlatformIndex);
   if (xmtrPlatformPtr != nullptr)
   {
      double simTime = GetTime();

      WsfPlatform* rcvrPlatformPtr = GetSimulation()->GetPlatformByIndex(mRcvrPlatformIndex);
      if ((rcvrPlatformPtr != nullptr) && (mLastStatusChangeTime == mXmtrPtr->GetLastStatusChangeTime()))
      {
         if (mRcvrPtr->IsTurnedOn())
         {
            mRcvrPtr->Receive(simTime, mXmtrPtr, *mCommMessagePtr);
         }
         else
         {
            mXmtrPtr->ProcessLayerMessage(simTime, 1, layer::cUP_NACK_SEND, mCommMessagePtr);
         }
      }
   }

   return eventDisposition;
}

void Event::ScheduleDeliverMessageEvent(double aSimTime, Comm* aXmtrPtr, Comm* aRcvrPtr, Message* aCommMessagePtr)
{
   auto eventPtr = ut::make_unique<Event>(aSimTime, aXmtrPtr);
   assert(aRcvrPtr != nullptr);

   // Add the additional members for receiver.
   eventPtr->mRcvrPtr           = aRcvrPtr;
   eventPtr->mRcvrPlatformIndex = aRcvrPtr->GetPlatform()->GetIndex();
   eventPtr->mCommMessagePtr    = aCommMessagePtr->Clone(); // Clone from the sender; delete in this class

   // Queue the event
   aXmtrPtr->GetSimulation()->AddEvent(std::move(eventPtr));
}

} // namespace comm
} // namespace wsf
