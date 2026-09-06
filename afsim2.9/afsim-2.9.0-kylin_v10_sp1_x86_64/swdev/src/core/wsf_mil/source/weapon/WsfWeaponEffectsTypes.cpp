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

#include "WsfWeaponEffectsTypes.hpp"

#include "UtMemory.hpp"
#include "WsfCarltonLethality.hpp"
#include "WsfExoAtmosphericLethality.hpp"
#include "WsfExplicitWeaponEffects.hpp"
#include "WsfGraduatedLethality.hpp"
#include "WsfHEL_Lethality.hpp"
#include "WsfMil.hpp"
#include "WsfMobilityAndFirepowerLethality.hpp"
#include "WsfSphericalLethality.hpp"
#include "WsfTabulatedLethality.hpp"
#include "WsfWeaponEffectsTypes.hpp"


// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
WsfWeaponEffectsTypes& WsfWeaponEffectsTypes::Get(WsfScenario& aScenario)
{
   return WsfMilExtension::Get(aScenario).GetWeaponEffectsTypes();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const WsfWeaponEffectsTypes& WsfWeaponEffectsTypes::Get(const WsfScenario& aScenario)
{
   return WsfMilExtension::Get(aScenario).GetWeaponEffectsTypes();
}

// =================================================================================================
WsfWeaponEffectsTypes::WsfWeaponEffectsTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfWeaponEffects>(aScenario, cREDEFINITION_ALLOWED, "weapon_effects")
{
   auto defaultLethality = ut::make_unique<WsfSphericalLethality>(aScenario);
   defaultLethality->SetMaxiumEffectiveRadius(3.0);
   AddCoreType("#DEFAULT_LETHALITY", std::move(defaultLethality));

   AddCoreType("WSF_CARLTON_LETHALITY", ut::make_unique<WsfCarltonLethality>(aScenario));
   AddCoreType("WSF_EXOATMOSPHERIC_LETHALITY", ut::make_unique<WsfExoAtmosphericLethality>(aScenario));
   AddCoreType("WSF_EXPLICIT_WEAPON_EFFECT", ut::make_unique<WsfExplicitWeaponEffects>(aScenario));
   AddCoreType("WSF_GRADUATED_LETHALITY", ut::make_unique<WsfGraduatedLethality>(aScenario));
   AddCoreType("WSF_HEL_LETHALITY", ut::make_unique<WsfHEL_Lethality>(aScenario));
   AddCoreType("WSF_MOBILITY_AND_FIREPOWER_LETHALITY", ut::make_unique<WsfMobilityAndFirepowerLethality>(aScenario));
   AddCoreType("WSF_SPHERICAL_LETHALITY", ut::make_unique<WsfSphericalLethality>(aScenario));
   AddCoreType("WSF_TABULATED_LETHALITY", ut::make_unique<WsfTabulatedLethality>(aScenario));
}

bool WsfWeaponEffectsTypes::InitializeType(WsfWeaponEffects* aTypePtr)
{
   return aTypePtr->PreInitialize();
}
