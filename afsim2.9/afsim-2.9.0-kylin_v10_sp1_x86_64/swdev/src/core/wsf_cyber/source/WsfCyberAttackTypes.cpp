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

#include "WsfCyberAttackTypes.hpp"

#include "UtMemory.hpp"
#include "WsfCyberScenarioExtension.hpp"

namespace wsf
{
namespace cyber
{

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
AttackTypes& AttackTypes::Get(WsfScenario& aScenario)
{
   return ScenarioExtension::Get(aScenario).GetAttackTypes();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const AttackTypes& AttackTypes::Get(const WsfScenario& aScenario)
{
   return ScenarioExtension::Get(aScenario).GetAttackTypes();
}

// =================================================================================================
AttackTypes::AttackTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<Attack>(aScenario, "cyber_attack")
{
   // Register an empty attack type as a starting point.
   AddCoreType("WSF_CYBER_ATTACK", ut::make_unique<Attack>());
}

} // namespace cyber
} // namespace wsf
