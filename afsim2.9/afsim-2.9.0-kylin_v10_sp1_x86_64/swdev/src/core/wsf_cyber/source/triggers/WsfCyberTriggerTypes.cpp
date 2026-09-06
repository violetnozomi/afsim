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

#include "triggers/WsfCyberTriggerTypes.hpp"

#include "UtMemory.hpp"
#include "WsfComponentFactory.hpp"
#include "WsfCyberScenarioExtension.hpp"
#include "WsfPlatform.hpp"
#include "triggers/WsfCyberCompositeTrigger.hpp"

namespace wsf
{
namespace cyber
{

namespace
{
//! Component factory to process platform input.
//! Note that we only allow the addition, edit, or removal of triggers
//! on platforms. All triggers attached to a platform instance must already
//! be defined in the global context.
class CyberTriggerComponentFactory : public WsfComponentFactory<WsfPlatform>
{
public:
   bool ProcessAddOrEditCommand(UtInput& aInput, WsfPlatform& aPlatform, bool aIsAdding) override
   {
      TriggerTypes& types(TriggerTypes::Get(GetScenario()));
      return types.LoadNamedComponent(aInput, aPlatform, aIsAdding, cCOMPONENT_ROLE<trigger::Trigger>());
   }

   bool ProcessDeleteCommand(UtInput& aInput, WsfPlatform& aPlatform) override
   {
      TriggerTypes& types(TriggerTypes::Get(GetScenario()));
      return types.DeleteNamedComponent(aInput, aPlatform, cCOMPONENT_ROLE<trigger::Trigger>());
   }
};
} // anonymous namespace

// =================================================================================================
TriggerTypes& TriggerTypes::Get(WsfScenario& aScenario)
{
   return ScenarioExtension::Get(aScenario).GetTriggerTypes();
}

// =================================================================================================
const TriggerTypes& TriggerTypes::Get(const WsfScenario& aScenario)
{
   return ScenarioExtension::Get(aScenario).GetTriggerTypes();
}

// =================================================================================================
TriggerTypes::TriggerTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<trigger::Trigger>(aScenario, "cyber_trigger")
{
   AddCoreType("WSF_CYBER_TRIGGER", ut::make_unique<trigger::Trigger>(aScenario));
   AddCoreType("WSF_CYBER_COMPOSITE_TRIGGER",
               ut::make_unique<trigger::CompositeTrigger>(aScenario,
                                                          [=](const std::string& aTriggerName) -> std::unique_ptr<trigger::Trigger>
                                                          {
                                                             auto* trigger = Find(aTriggerName);
                                                             if (trigger)
                                                             {
                                                                return ut::clone(trigger);
                                                             }
                                                             return nullptr;
                                                          }));
}

// =================================================================================================
void TriggerTypes::RegisterComponentFactory(WsfScenario& aScenario)
{
   aScenario.RegisterComponentFactory(ut::make_unique<CyberTriggerComponentFactory>());
}

} // namespace cyber
} // namespace wsf
