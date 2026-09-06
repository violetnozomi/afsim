// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCYBEREVENTMANAGER_HPP
#define WSFCYBEREVENTMANAGER_HPP

#include "wsf_cyber_export.h"

#include <memory>
#include <unordered_map>

#include "WsfCyberEvent.hpp"
class WsfSimulation;

namespace wsf
{
namespace cyber
{

//! @name EventManager
//! The cyber event manager tracks scheduled cyber events related to engagements.
//! During these delays, this interface allows the canceling of these
//! events to preemptively stop an engagement from progressing. Attacks and scans
//! are tracked separately, as both of these types of engagements are allowed
//! simultaneously. Furthermore, this object allows the query of which events
//! and their corresponding delay types are being managed.
//!
//! @note This class is only intended for usage with events associated with the
//! cyber engagement progression. Only a single event is ever scheduled at any
//! given time for this use case for scans or attacks.
//! Other events, such as those that might be used
//! by effects directly, are not handled nor intended to be handled by this object.
class WSF_CYBER_EXPORT EventManager final
{
public:
   EventManager(WsfSimulation& aSimulation);
   ~EventManager()                        = default;
   EventManager(const EventManager& aSrc) = delete;
   EventManager& operator=(const EventManager& aRhs) = delete;
   EventManager(EventManager&& aSrc)                 = default;
   EventManager& operator=(EventManager&& aRhs) = default;

   //! Add an event.
   //! This call will schedule the event with the simulation, and
   //! conduct tracking of the event for usage within the cyber library.
   void AddEvent(std::unique_ptr<Event> aEventPtr);

   //! End an event.
   //! The EndEvent method only removes the event from management by the
   //! cyber event manager. This call is intended to be called only by
   //! the event itself upon impending destruction.
   void EndEvent(bool aAttack, size_t aEngagementKey);

   //! Cancel an event.
   //! The CancelEvent method removes the event from management, as well
   //! as ensures that the simulation event manager does not execute
   //! this event. The engagement corresponding to an event canceled in
   //! this manner is immediately available for reuse.
   bool CancelEvent(bool aAttack, size_t aEngagementKey);

   //! EventExists
   //! Query for the existence of a managed event.
   bool EventExists(bool aAttack, size_t aEngagementKey) const;

   //! Retrieve the type of the event.
   Event::Type GetEventType(bool aAttack, size_t aEngagementKey) const;

private:
   struct EventKey
   {
      EventKey(bool aAttack, size_t aEngagementKey)
         : mAttack(aAttack)
         , mEngagementKey(aEngagementKey)
      {
      }

      bool operator==(const EventKey& aRhs) const
      {
         return ((mAttack == aRhs.mAttack) && (mEngagementKey == aRhs.mEngagementKey));
      }

      bool   mAttack;
      size_t mEngagementKey;
   };

   struct EventKeyHash
   {
      size_t operator()(const EventKey& aKey) const
      {
         auto val = std::hash<size_t>{}(aKey.mEngagementKey);
         val ^= std::hash<bool>{}(aKey.mAttack) + 0x9e3779b9 + (val << 6) + (val >> 2);
         return val;
      }
   };

   using EventMap = std::unordered_map<EventKey, Event*, EventKeyHash>;

   WsfSimulation& mSimulation;
   EventMap       mEventMap{};
};

} // namespace cyber
} // namespace wsf

#endif
