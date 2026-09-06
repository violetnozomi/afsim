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

#include "WsfCyberScenarioExtension.hpp"

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMemory.hpp"
#include "WsfCyberAttackTypes.hpp"
#include "WsfCyberConstraintTypes.hpp"
#include "WsfCyberEffectTypes.hpp"
#include "WsfCyberEventPipe.hpp"
#include "WsfCyberProtectTypes.hpp"
#include "WsfCyberSimulationExtension.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "triggers/WsfCyberTriggerTypes.hpp"

namespace wsf
{
namespace cyber
{
// =================================================================================================
//! Return a reference to the cyber scenario extension.
// static
ScenarioExtension& ScenarioExtension::Get(const WsfScenario& aScenario)
{
   WsfScenarioExtension* extensionPtr = aScenario.FindExtension("wsf_cyber");
   return static_cast<ScenarioExtension&>(*extensionPtr);
}

// =================================================================================================
void ScenarioExtension::AddedToScenario()
{
   // Register the type lists and component factories
   auto attackTypesPtr = ut::make_unique<AttackTypes>(GetScenario());
   mAttackTypesPtr     = attackTypesPtr.get();
   GetScenario().AddTypeList(std::move(attackTypesPtr));

   auto effectTypesPtr = ut::make_unique<EffectTypes>(GetScenario());
   mEffectTypesPtr     = effectTypesPtr.get();
   GetScenario().AddTypeList(std::move(effectTypesPtr));

   auto protectTypesPtr = ut::make_unique<ProtectTypes>(GetScenario());
   mProtectTypesPtr     = protectTypesPtr.get();
   GetScenario().AddTypeList(std::move(protectTypesPtr));
   Protect::RegisterComponentFactory(GetScenario());

   auto triggerTypesPtr = ut::make_unique<TriggerTypes>(GetScenario());
   mTriggerTypesPtr     = triggerTypesPtr.get();
   GetScenario().AddTypeList(std::move(triggerTypesPtr));
   TriggerTypes::RegisterComponentFactory(GetScenario());

   auto constraintTypesPtr = ut::make_unique<ConstraintTypes>(GetScenario());
   mConstraintTypesPtr     = constraintTypesPtr.get();
   GetScenario().AddTypeList(std::move(constraintTypesPtr));
   ConstraintTypes::RegisterComponentFactory(GetScenario());

   // If the "event_pipe" extension has been defined then hook things up so our events will also be logged.
   WsfEventPipeExtension* eventPipePtr = WsfEventPipeExtension::Find(GetScenario());
   if (eventPipePtr != nullptr)
   {
      wsf::cyber::EventPipe::RegisterEvents(*eventPipePtr);
   }
}

// =================================================================================================
bool ScenarioExtension::ProcessInput(UtInput& aInput)
{
   bool myCommand(true);
   auto command = aInput.GetCommand();

   if (command == "enable_cyber_wsfdraw")
   {
      mDrawEnabled = true;
   }
   else if (command == "disable_cyber_wsfdraw")
   {
      mDrawEnabled = false;
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

// =================================================================================================
void ScenarioExtension::SimulationCreated(WsfSimulation& aSimulation)
{
   aSimulation.RegisterExtension("wsf_cyber", ut::make_unique<SimulationExtension>());
}

// =================================================================================================
bool ScenarioExtension::GetAttackTypeExists(const std::string& aNamedAttack) const
{
   return mAttackTypesPtr->IsType(aNamedAttack);
}

// =================================================================================================
bool ScenarioExtension::GetEffectTypeExists(const std::string& aNamedEffect) const
{
   return mEffectTypesPtr->IsType(aNamedEffect);
}

// =================================================================================================
bool ScenarioExtension::GetProtectTypeExists(const std::string& aNamedProtection) const
{
   return mProtectTypesPtr->IsType(aNamedProtection);
}

// =================================================================================================
bool ScenarioExtension::GetTriggerTypeExists(const std::string& aNamedTrigger) const
{
   return mTriggerTypesPtr->IsType(aNamedTrigger);
}

// =================================================================================================
bool ScenarioExtension::GetAttackTypeInputRequirement(const std::string&       aNamedAttack,
                                                      Effect::InputRequirement aRequirement) const
{
   auto* curAttackType = mAttackTypesPtr->Find(aNamedAttack);
   if (curAttackType)
   {
      const auto& effectList = curAttackType->GetEffects();
      for (auto& effect : effectList)
      {
         auto* curEffectType = mEffectTypesPtr->Find(effect);
         if (curEffectType && (curEffectType->RequiredInput() == aRequirement))
         {
            return true;
         }
      }
   }
   return false;
}
// =================================================================================================
bool ScenarioExtension::GetConstraintTypesExists(const std::string& aNamedConstraint) const
{
   return mConstraintTypesPtr->IsType(aNamedConstraint);
}

} // namespace cyber
} // namespace wsf
