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

#include "WsfOrbitalDynamicsTermTypes.hpp"

#include "WsfAtmosphericDragTerm.hpp"
#include "WsfEarthJ2Term.hpp"
#include "WsfEarthMonopoleTerm.hpp"
#include "WsfJupiterMonopoleTerm.hpp"
#include "WsfMoonMonopoleTerm.hpp"
#include "WsfScenario.hpp"
#include "WsfScriptedOrbitalDynamicsTerm.hpp"
#include "WsfSunMonopoleTerm.hpp"

WsfOrbitalDynamicsTermTypes& WsfOrbitalDynamicsTermTypes::Get(const WsfScenario& aScenario)
{
   return static_cast<WsfOrbitalDynamicsTermTypes&>(*aScenario.GetTypes("orbital_dynamics_term"));
}

WsfOrbitalDynamicsTermTypes::WsfOrbitalDynamicsTermTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfOrbitalDynamicsTerm>(aScenario, cSINGULAR_BASE_TYPE, "orbital_dynamics_term")
{
   Add(WsfEarthMonopoleTerm::cTYPE, ut::make_unique<WsfEarthMonopoleTerm>());
   Add(WsfEarthJ2Term::cTYPE, ut::make_unique<WsfEarthJ2Term>());
   Add(WsfMoonMonopoleTerm::cTYPE, ut::make_unique<WsfMoonMonopoleTerm>());
   Add(WsfSunMonopoleTerm::cTYPE, ut::make_unique<WsfSunMonopoleTerm>());
   Add(WsfScriptedOrbitalDynamicsTerm::cTYPE, ut::make_unique<WsfScriptedOrbitalDynamicsTerm>());
   Add(WsfJupiterMonopoleTerm::cTYPE, ut::make_unique<WsfJupiterMonopoleTerm>());
   Add(WsfAtmosphericDragTerm::cTYPE, ut::make_unique<WsfAtmosphericDragTerm>());
}
