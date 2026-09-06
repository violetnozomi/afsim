// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfCyberTriggerEvent.hpp"

#include "WsfCyberSimulationExtension.hpp"
#include "WsfCyberTriggerManager.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"

namespace wsf
{
namespace cyber
{
namespace trigger
{

// =================================================================================================
GlobalEvent::GlobalEvent(double aSimTime, std::string aTriggerName)
   : WsfEvent(aSimTime)
   , mTriggerName(std::move(aTriggerName))
{
}

// =================================================================================================
WsfEvent::EventDisposition GlobalEvent::Execute()
{
   if (!mTriggerName.empty())
   {
      // Get a reference of the trigger from the trigger manager, if possible.
      auto& cyberSim       = SimulationExtension::Get(*GetSimulation());
      auto& triggerManager = cyberSim.GetCyberTriggerManager();
      auto  triggerPtr     = triggerManager.GetTrigger(mTriggerName);

      if (triggerPtr)
      {
         auto active = triggerPtr->Evaluate(GetTime());
         if (active)
         {
            triggerPtr->Execute(GetTime());
         }

         auto nextTime   = triggerPtr->NextIntervalTime(GetTime());
         auto reschedule = ((!triggerPtr->HasTriggered()) || (triggerPtr->IsExtrapolated()));

         if (nextTime.first && reschedule)
         {
            SetTime(nextTime.second);
            return EventDisposition::cRESCHEDULE;
         }
      }

      // If we didn't return a reschedule, this trigger is no longer functional.
      // Remove it from management.
      triggerManager.RemoveManagedTrigger(mTriggerName);
   }

   return EventDisposition::cDELETE;
}

// =================================================================================================
PlatformEvent::PlatformEvent(double aSimTime, std::string aPlatformName, std::string aTriggerName)
   : GlobalEvent(aSimTime, std::move(aTriggerName))
   , mPlatformName(std::move(aPlatformName))
{
}

// =================================================================================================
WsfEvent::EventDisposition PlatformEvent::Execute()
{
   auto platformPtr = GetSimulation()->GetPlatformByName(mPlatformName);
   if (platformPtr)
   {
      auto triggerPtr = platformPtr->GetComponent<Trigger>(mTriggerName);
      if (triggerPtr)
      {
         auto active = triggerPtr->Evaluate(GetTime());
         if (active)
         {
            triggerPtr->Execute(GetTime());
         }

         auto nextTime   = triggerPtr->NextIntervalTime(GetTime());
         auto reschedule = ((!triggerPtr->HasTriggered()) || (triggerPtr->IsExtrapolated()));

         if (nextTime.first && reschedule)
         {
            SetTime(nextTime.second);
            return EventDisposition::cRESCHEDULE;
         }
         else
         {
            platformPtr->DeleteComponent<Trigger>(mTriggerName);
            return EventDisposition::cDELETE;
         }
      }
   }

   return EventDisposition::cDELETE;
}

} // namespace trigger
} // namespace cyber
} // namespace wsf
