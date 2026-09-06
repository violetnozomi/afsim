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

#include "WsfWeaponTypes.hpp"

#include "UtMemory.hpp"
#include "WsfComponentFactory.hpp"
#include "WsfMil.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfWeaponPlatformExtension.hpp"

// Include files for built-in types
#include "WsfCuedLaserWeapon.hpp"
#include "WsfExplicitWeapon.hpp"
#include "WsfImplicitWeapon.hpp"
#include "WsfLaserWeapon.hpp"
#include "WsfRF_Jammer.hpp"

namespace
{
//! Component factory to process platform input.
class WeaponComponentFactory : public WsfComponentFactory<WsfPlatform>
{
public:
   bool ProcessAddOrEditCommand(UtInput& aInput, WsfPlatform& aPlatform, bool aIsAdding) override
   {
      WsfWeaponTypes& types(WsfWeaponTypes::Get(GetScenario()));
      return types.LoadNamedComponent(aInput, aPlatform, aIsAdding, cCOMPONENT_ROLE<WsfWeapon>());
   }

   bool ProcessDeleteCommand(UtInput& aInput, WsfPlatform& aPlatform) override
   {
      WsfWeaponTypes& types(WsfWeaponTypes::Get(GetScenario()));
      return types.DeleteNamedComponent(aInput, aPlatform, cCOMPONENT_ROLE<WsfWeapon>());
   }
};
} // namespace

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
WsfWeaponTypes& WsfWeaponTypes::Get(WsfScenario& aScenario)
{
   return WsfMilExtension::Get(aScenario).GetWeaponTypes();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const WsfWeaponTypes& WsfWeaponTypes::Get(const WsfScenario& aScenario)
{
   return WsfMilExtension::Get(aScenario).GetWeaponTypes();
}

// =================================================================================================
WsfWeaponTypes::WsfWeaponTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfWeapon>(aScenario, "weapon")
{
   aScenario.RegisterComponentFactory(ut::make_unique<WeaponComponentFactory>());
   WsfWeaponPlatformExtension::RegisterComponentFactory(aScenario);

   // Create 'type' entries for the built-in types.
   AddCoreType("WSF_EXPLICIT_WEAPON", ut::make_unique<WsfExplicitWeapon>(aScenario));
   AddCoreType("WSF_IMPLICIT_WEAPON", ut::make_unique<WsfImplicitWeapon>(aScenario));
   AddCoreType("WSF_RF_JAMMER", ut::make_unique<WsfRF_Jammer>(aScenario));
   AddCoreType("WSF_LASER_WEAPON", ut::make_unique<WsfLaserWeapon>(aScenario));
   AddCoreType("WSF_CUED_LASER_WEAPON", ut::make_unique<WsfCuedLaserWeapon>(aScenario));
}
