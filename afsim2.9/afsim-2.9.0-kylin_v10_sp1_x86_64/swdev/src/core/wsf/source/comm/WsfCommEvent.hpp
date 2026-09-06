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

#ifndef WSFCOMMEVENT_HPP
#define WSFCOMMEVENT_HPP

#include "wsf_export.h"

#include <cstddef> // for size_t
#include <functional>
#include <memory>

#include "WsfEvent.hpp"

namespace wsf
{
namespace comm
{

class Comm;
class Message;

/** An event associated with communications (transfer complete, etc). */

class WSF_EXPORT Event : public WsfEvent
{
public:
   Event(double aSimTime, Comm* aXmtrPtr);
   ~Event() override;

   EventDisposition Execute() override;

   static void ScheduleDeliverMessageEvent(double aSimTime, Comm* aXmtrPtr, Comm* aRcvrPtr, Message* aCommMessagePtr);

private:
   //! For cDELIVER_MESSAGE this is the platform index of the platform that owns
   //! the transmitting comm device. For other events this is the platform index of
   //! comm device to which the event applies.
   size_t mXmtrPlatformIndex;

   //! For cDELIVER_MESSAGE this is the platform index of the platform that owns
   //! the receiving comm device.  For other events this is zero.
   size_t mRcvrPlatformIndex;

   //! For cDELIVER_MESSAGE this is the pointer to the transmitting comm device.
   //! For other events this is the comm device to which the event applies.
   Comm* mXmtrPtr;

   //! For cDELIVER_MESSAGE this is the pointer to the receiving comm device.
   //! For other events this is zero.
   Comm* mRcvrPtr;

   //! For cDELIVER_MESSAGE this is the value of the transmitters 'last status
   //! change time' at the time the event was queued. When the event is dispatched
   //! this value is compared to the current value in the transmitter. If it has
   //! changed then we know the transmitter has been turned off and the message is
   //! no longer valid.
   double mLastStatusChangeTime;

   //! This is the pointer to the message being transmitted. This is a clone.
   Message* mCommMessagePtr;
};

template<class T>
class WSF_EXPORT GenericEvent : public WsfEvent
{
public:
   GenericEvent(double aSimTime, const std::shared_ptr<T>& aContextPtr, const std::function<void()>& aFunc)
      : WsfEvent(aSimTime)
      , mContextPtr{aContextPtr}
      , mFunction{aFunc}
   {
   }
   ~GenericEvent() override = default;

   EventDisposition Execute() override
   {
      if (!mContextPtr.expired())
      {
         mFunction();
      }
      return WsfEvent::cDELETE;
   }

private:
   std::weak_ptr<T>      mContextPtr;
   std::function<void()> mFunction;
};


} // namespace comm
} // namespace wsf

#endif
