// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfThermalSystemTypes.hpp"

#include "UtMemory.hpp"
#include "WsfComponentFactory.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"

namespace
{
class ThermalSystemComponentFactory : public WsfComponentFactory<WsfPlatform>
{
public:
   bool ProcessAddOrEditCommand(UtInput& aInput, WsfPlatform& aPlatform, bool aIsAdding) override
   {
      WsfThermalSystemTypes& types(WsfThermalSystemTypes::Get(GetScenario()));
      return types.LoadNamedComponent(aInput, aPlatform, aIsAdding, cCOMPONENT_ROLE<WsfThermalSystem>());
   }
   bool ProcessDeleteCommand(UtInput& aInput, WsfPlatform& aPlatform) override
   {
      WsfThermalSystemTypes& types(WsfThermalSystemTypes::Get(GetScenario()));
      return types.DeleteNamedComponent(aInput, aPlatform, cCOMPONENT_ROLE<WsfThermalSystem>());
   }
};
} // namespace

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
WsfThermalSystemTypes& WsfThermalSystemTypes::Get(WsfScenario& aScenario)
{
   return aScenario.GetThermalSystemTypes();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const WsfThermalSystemTypes& WsfThermalSystemTypes::Get(const WsfScenario& aScenario)
{
   return aScenario.GetThermalSystemTypes();
}

// =================================================================================================
WsfThermalSystemTypes::WsfThermalSystemTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfThermalSystem>(aScenario, cNO_DELAY_LOAD, "thermal_system")
{
   aScenario.RegisterComponentFactory(ut::make_unique<ThermalSystemComponentFactory>()); // Allows for definition inside
   // platform, platform_type blocks.
   Add("WSF_THERMAL_SYSTEM", ut::make_unique<WsfThermalSystem>(aScenario));
}
