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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "WsfFuelTypes.hpp"

#include "UtMemory.hpp"
#include "WsfComponentFactory.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"

// Pre-defined types
#include "WsfTabularRateFuel.hpp"
#include "WsfTankedFuel.hpp"
#include "WsfVariableRateFuel.hpp"

namespace
{
//! Component factory to process platform input.
class FuelComponentFactory : public WsfComponentFactory<WsfPlatform>
{
public:
   bool ProcessAddOrEditCommand(UtInput& aInput, WsfPlatform& aPlatform, bool aIsAdding) override
   {
      WsfFuelTypes& types(WsfFuelTypes::Get(GetScenario()));
      return types.LoadUnnamedComponent(aInput, aPlatform, aIsAdding, cCOMPONENT_ROLE<WsfFuel>());
   }

   bool ProcessDeleteCommand(UtInput& aInput, WsfPlatform& aPlatform) override
   {
      WsfFuelTypes& types(WsfFuelTypes::Get(GetScenario()));
      return types.DeleteUnnamedComponent(aInput, aPlatform, cCOMPONENT_ROLE<WsfFuel>());
   }
};
} // namespace

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
WsfFuelTypes& WsfFuelTypes::Get(WsfScenario& aScenario)
{
   return aScenario.GetFuelTypes();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const WsfFuelTypes& WsfFuelTypes::Get(const WsfScenario& aScenario)
{
   return aScenario.GetFuelTypes();
}

// =================================================================================================
WsfFuelTypes::WsfFuelTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfFuel>(aScenario, "fuel")
{
   aScenario.RegisterComponentFactory(ut::make_unique<FuelComponentFactory>());

   AddCoreType("WSF_FUEL", ut::make_unique<WsfFuel>(aScenario));
   AddCoreType("WSF_TABULAR_RATE_FUEL", ut::make_unique<WsfTabularRateFuel>(aScenario));
   AddCoreType("WSF_TANKED_FUEL", ut::make_unique<WsfTankedFuel>(aScenario));
   AddCoreType("WSF_VARIABLE_RATE_FUEL", ut::make_unique<WsfVariableRateFuel>(aScenario));
}
