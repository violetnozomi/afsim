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

#include "WsfOrbitalIntegratorTypes.hpp"

#include "WsfPrinceDormand45OrbitalIntegrator.hpp"
#include "WsfPrinceDormand78OrbitalIntegrator.hpp"
#include "WsfScenario.hpp"

WsfOrbitalIntegratorTypes& WsfOrbitalIntegratorTypes::Get(const WsfScenario& aScenario)
{
   return static_cast<WsfOrbitalIntegratorTypes&>(*aScenario.GetTypes("orbital_integrator"));
}

WsfOrbitalIntegratorTypes::WsfOrbitalIntegratorTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfOrbitalIntegrator>(aScenario, cSINGULAR_BASE_TYPE, "orbital_integrator")
{
   Add(WsfPrinceDormand45OrbitalIntegrator::cTYPE, ut::make_unique<WsfPrinceDormand45OrbitalIntegrator>());
   Add(WsfPrinceDormand78OrbitalIntegrator::cTYPE, ut::make_unique<WsfPrinceDormand78OrbitalIntegrator>());
}
