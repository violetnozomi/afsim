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

#include "WsfCyberProtectTypes.hpp"

#include "UtMemory.hpp"
#include "WsfCyberScenarioExtension.hpp"
#include "WsfScenario.hpp"

namespace wsf
{
namespace cyber
{

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
ProtectTypes& ProtectTypes::Get(WsfScenario& aScenario)
{
   return ScenarioExtension::Get(aScenario).GetProtectTypes();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const ProtectTypes& ProtectTypes::Get(const WsfScenario& aScenario)
{
   return ScenarioExtension::Get(aScenario).GetProtectTypes();
}

// =================================================================================================
ProtectTypes::ProtectTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<Protect>(aScenario, "cyber_protect")
{
   // Load the prototype 'cyber_protect' instance from which users can inherit.
   AddCoreType("WSF_CYBER_PROTECT", ut::make_unique<Protect>(aScenario));
}

} // namespace cyber
} // namespace wsf
