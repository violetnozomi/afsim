// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfCyberEvent.hpp"

#include "WsfCyberEngagement.hpp"
#include "WsfCyberEngagementManager.hpp"
#include "WsfCyberSimulationExtension.hpp"
#include "WsfSimulation.hpp"

namespace wsf
{

namespace cyber
{

Event::Event(double aSimTime, Type aEventType, std::string aVictim, size_t aKey)
   : WsfEvent(aSimTime)
   , mEventType(aEventType)
   , mVictim(std::move(aVictim))
   , mKey(aKey)
{
}

// =================================================================================================
Event::EventDisposition Event::Execute()
{
   auto& manager = EngagementManager::Get(*GetSimulation());

   auto& eventManager = SimulationExtension::Get(*GetSimulation()).GetCyberEventManager();
   auto  attack       = (mEventType != Type::cSCAN_DELAY);
   eventManager.EndEvent(attack, mKey);

   //! Check if the target platform still exists in the simulation, since we've delayed.
   if (!GetSimulation()->GetPlatformByName(mVictim))
   {
      return EventDisposition::cDELETE;
   }

   //! Do a check for the engagement object to ensure it hasn't been removed since event scheduling
   if (!manager.EngagementExists(mKey))
   {
      return EventDisposition::cDELETE;
   }

   if (mEventType == Type::cSCAN_DELAY)
   {
      auto engagementDataPtr = manager.FindEngagementData(mKey);
      if (engagementDataPtr)
      {
         manager.CyberScan(*engagementDataPtr);
      }
   }
   else if (mEventType == Type::cATTACK_DELAY)
   {
      auto engagementDataPtr = manager.FindEngagementData(mKey);
      if (engagementDataPtr)
      {
         manager.CyberAttack(*engagementDataPtr);
      }
   }
   else if (mEventType == Type::cATTACK_DETECTION_DELAY)
   {
      auto engagementDataPtr = manager.FindEngagementData(mKey);
      if (engagementDataPtr)
      {
         manager.CyberAttackDetectionDelay(*engagementDataPtr);
      }
   }
   else if (mEventType == Type::cATTACK_RECOVERY_DELAY)
   {
      auto engagementDataPtr = manager.FindEngagementData(mKey);
      if (engagementDataPtr)
      {
         manager.CyberAttackRecoveryDelay(*engagementDataPtr);
      }
   }

   // Once this event fires, it is no longer needed. Remove it
   return EventDisposition::cDELETE;
}

} // namespace cyber

} // namespace wsf
