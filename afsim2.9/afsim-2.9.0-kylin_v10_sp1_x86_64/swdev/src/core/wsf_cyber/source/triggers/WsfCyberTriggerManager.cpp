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

#include "triggers/WsfCyberTriggerManager.hpp"

#include "WsfCyberScenarioExtension.hpp"
#include "WsfSimulation.hpp"
#include "triggers/WsfCyberTriggerEvent.hpp"
#include "triggers/WsfCyberTriggerTypes.hpp"

namespace wsf
{
namespace cyber
{
namespace trigger
{

// =================================================================================================
Manager::Manager(WsfSimulation& aSimulation)
   : mSimulation(aSimulation)
{
}

// =================================================================================================
bool Manager::Initialize()
{
   if (mSimulation.GetState() == WsfSimulation::cINITIALIZING)
   {
      auto&                          triggerTypes = ScenarioExtension::Get(mSimulation.GetScenario()).GetTriggerTypes();
      std::vector<trigger::Trigger*> userTriggerTypes;
      triggerTypes.GetCurrentUserTypes(userTriggerTypes);

      bool ok = true;
      for (auto triggerPtr : userTriggerTypes)
      {
         ok &= AddManagedTrigger(0.0, ut::clone(triggerPtr));
      }

      return ok;
   }
   return false;
}

// =================================================================================================
Trigger* Manager::GetTrigger(const std::string& aTriggerName) const
{
   auto it = mMap.find(aTriggerName);
   if (it != std::end(mMap))
   {
      return it->second.get();
   }

   return nullptr;
}

// =================================================================================================
bool Manager::AddManagedTrigger(double aSimTime, std::unique_ptr<Trigger> aTriggerPtr)
{
   if (InitializeTrigger(aSimTime, *aTriggerPtr))
   {
      auto result = mMap.emplace(aTriggerPtr->GetType(), std::move(aTriggerPtr));
      return result.second;
   }
   return false;
}

// =================================================================================================
bool Manager::RemoveManagedTrigger(const std::string& aTriggerName)
{
   auto num_erased = mMap.erase(aTriggerName);
   if (num_erased != 0U)
   {
      return true;
   }

   return false;
}

// =================================================================================================
bool Manager::InitializeTrigger(double aSimTime, Trigger& aTrigger) const
{
   if (aTrigger.GetPlatform())
   {
      return false;
   }

   if (aTrigger.ShouldInstance())
   {
      if (!aTrigger.InitializeGlobal(aSimTime, mSimulation))
      {
         return false;
      }

      auto timeResult = aTrigger.NextIntervalTime(aSimTime);
      if (timeResult.first)
      {
         mSimulation.AddEvent(ut::make_unique<GlobalEvent>(timeResult.second, aTrigger.GetType()));
      }
   }

   return true;
}

} // namespace trigger
} // namespace cyber
} // namespace wsf
