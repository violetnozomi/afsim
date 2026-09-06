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

#include "UtScriptSolarSystem.hpp"

#include "UtMath.hpp"
#include "UtScriptMethodDefine.hpp"
#include "UtSolarSystem.hpp"
#include "UtVec3.hpp"

UtScriptSolarSystem::UtScriptSolarSystem(UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass("SolarSystem", aScriptTypesPtr)
{
   UT_SCRIPT_NONWRAPPING_CLASS(SolarSystem);

   mConstructible = true;

   UT_SCRIPT_STATIC_METHOD(std::unique_ptr<UtVec3d>, PlanetLocationECI, (int planet, const UtCalendar& aEpoch))
   {
      planet -= 1; // zero-based indexing.
      if ((planet >= UtSolarSystem::Planet::cMERCURY) && (planet <= UtSolarSystem::Planet::cNEPTUNE))
      {
         auto planetLocECI = ut::make_unique<UtVec3d>();
         auto planetPtr    = UtSolarSystem::GetPlanet(planet);
         planetPtr->GetLocationECI(aEpoch, *planetLocECI);
         return planetLocECI;
      }
      return nullptr;
   };

   UT_SCRIPT_STATIC_METHOD(std::unique_ptr<UtVec3d>, PlanetLocationEcliptic, (int planet, const UtCalendar& aEpoch))
   {
      planet -= 1; // zero-based indexing.
      if ((planet >= UtSolarSystem::Planet::cMERCURY) && (planet <= UtSolarSystem::Planet::cNEPTUNE))
      {
         auto planetlocEcliptic = ut::make_unique<UtVec3d>();
         auto planetPtr         = UtSolarSystem::GetPlanet(planet);
         planetPtr->GetLocationEcliptic(aEpoch, *planetlocEcliptic);
         return planetlocEcliptic;
      }
      return nullptr;
   };
}

// virtual
void* UtScriptSolarSystem::Create(const UtScriptContext& /*aContext*/)
{
   // Since this acts as a static class, we just return the current instance.
   return this;
}

// virtual
void UtScriptSolarSystem::Destroy(void* aObjectPtr)
{
   // Do nothing.  This is a static class.
}
