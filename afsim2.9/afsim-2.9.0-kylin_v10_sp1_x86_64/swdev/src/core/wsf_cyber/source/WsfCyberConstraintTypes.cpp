// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Radiance Technologies. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfCyberConstraintTypes.hpp"

#include "UtMemory.hpp"
#include "WsfComponentFactory.hpp"
#include "WsfCyberScenarioExtension.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"

namespace wsf
{
namespace cyber
{
namespace
{
//! Component factory to process platform input.
class ComponentFactory : public WsfComponentFactory<WsfPlatform>
{
public:
   bool PreInitialize(double aSimTime, WsfPlatform& aPlatform) override
   {
      // If the platform does not have a cyber_protect component then force a default one on to the platform.
      if (Constraint::Find(aPlatform) == nullptr)
      {
         aPlatform.AddComponent(new Constraint());
      }
      return true;
   }

   bool ProcessAddOrEditCommand(UtInput& aInput, WsfPlatform& aPlatform, bool aIsAdding) override
   {
      ConstraintTypes& types(ConstraintTypes::Get(GetScenario()));
      return types.LoadUnnamedComponent(aInput, aPlatform, aIsAdding, cCOMPONENT_ROLE<Constraint>());
   }

   bool ProcessDeleteCommand(UtInput& aInput, WsfPlatform& aPlatform) override
   {
      ConstraintTypes& types(ConstraintTypes::Get(GetScenario()));
      return types.DeleteUnnamedComponent(aInput, aPlatform, cCOMPONENT_ROLE<Constraint>());
   }
}; // ComponentFactory

} // namespace

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
ConstraintTypes& ConstraintTypes::Get(WsfScenario& aScenario)
{
   return ScenarioExtension::Get(aScenario).GetConstraintTypes();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const ConstraintTypes& ConstraintTypes::Get(const WsfScenario& aScenario)
{
   return ScenarioExtension::Get(aScenario).GetConstraintTypes();
}

// =================================================================================================
ConstraintTypes::ConstraintTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<Constraint>(aScenario, "cyber_constraint")
{
   // Load the prototype 'cyber_constraint' instance from which users can inherit.
   AddCoreType("WSF_CYBER_CONSTRAINT", ut::make_unique<Constraint>());
}

// =================================================================================================
void ConstraintTypes::RegisterComponentFactory(WsfScenario& aScenario)
{
   aScenario.RegisterComponentFactory(ut::make_unique<ComponentFactory>());
}

} // namespace cyber

} // namespace wsf
