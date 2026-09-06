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

#include "WsfCyberEventManager.hpp"

#include <algorithm>

#include "WsfCyberEvent.hpp"
#include "WsfCyberSimulationExtension.hpp"
#include "WsfEvent.hpp"
#include "WsfSimulation.hpp"


namespace wsf
{
namespace cyber
{

// =================================================================================================
EventManager::EventManager(WsfSimulation& aSimulation)
   : mSimulation(aSimulation)
{
}

// =================================================================================================
void EventManager::AddEvent(std::unique_ptr<Event> aEventPtr)
{
   if (!aEventPtr)
   {
      throw UtException("Attempting to add a nullptr cyber event.");
   }

   bool     attackType = (aEventPtr->GetType() != Event::Type::cSCAN_DELAY);
   EventKey key(attackType, aEventPtr->GetKey());
   mEventMap.emplace(key, aEventPtr.get());
   mSimulation.AddEvent(std::move(aEventPtr));
}

// =================================================================================================
void EventManager::EndEvent(bool aAttack, size_t aEngagementKey)
{
   EventKey key(aAttack, aEngagementKey);
   auto     it = mEventMap.find(key);

   if (it != std::end(mEventMap))
   {
      mEventMap.erase(it);
   }
}

// =================================================================================================
bool EventManager::CancelEvent(bool aAttack, size_t aEngagementKey)
{
   EventKey key(aAttack, aEngagementKey);
   auto     it = mEventMap.find(key);

   if (it != std::end(mEventMap))
   {
      it->second->SetShouldExecute(false);
      SimulationExtension::Get(mSimulation).GetCyberEngagementManager().Cancel(aEngagementKey);
      mEventMap.erase(it);
      return true;
   }

   return false;
}

// =================================================================================================
bool EventManager::EventExists(bool aAttack, size_t aEngagementKey) const
{
   EventKey key(aAttack, aEngagementKey);
   auto     it = mEventMap.find(key);

   return (it != std::end(mEventMap));
}

// =================================================================================================
Event::Type EventManager::GetEventType(bool aAttack, size_t aEngagementKey) const
{
   EventKey key(aAttack, aEngagementKey);
   auto     it = mEventMap.find(key);

   if (it == std::end(mEventMap))
   {
      return Event::Type::cNONE;
   }

   return it->second->GetType();
}

} // namespace cyber
} // namespace wsf
