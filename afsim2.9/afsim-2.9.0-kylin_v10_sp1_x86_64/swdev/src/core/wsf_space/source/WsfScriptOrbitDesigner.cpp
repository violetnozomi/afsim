// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfScriptOrbitDesigner.hpp"

#include "UtMemory.hpp"
#include "UtSun.hpp"
#include "WsfEarthJ2Term.hpp"
#include "WsfIntegratingSpaceMover.hpp"
#include "WsfOrbitalDynamics.hpp"
#include "WsfOrbitalDynamicsTerm.hpp"
#include "WsfOrbitalDynamicsTermTypes.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfSpaceMoverBase.hpp"
#include "script/WsfScriptDefs.hpp"

namespace wsf
{
namespace space
{
namespace script
{

OrbitDesigner::OrbitDesigner(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfOrbitDesigner");

   mConstructible = false;
   mCloneable     = false;

   AddStaticMethod(ut::make_unique<SunSynchronous>());
}


UT_DEFINE_SCRIPT_METHOD(OrbitDesigner, OrbitDesigner, SunSynchronous, 4, "WsfPlatform", "string, Calendar, double, double")
{
   constexpr double default_j2 = 0.0010826267; // default j2
   double           j2         = default_j2;

   std::unique_ptr<WsfPlatform> platformPtr{
      static_cast<WsfPlatform*>(SCENARIO->CloneType("platform_type", aVarArgs[0].GetString()))};

   if (platformPtr != nullptr)
   {
      WsfMover*          moverPtr          = platformPtr->GetMover();
      WsfSpaceMoverBase* spaceMoverBasePtr = dynamic_cast<WsfSpaceMoverBase*>(moverPtr);
      if (!spaceMoverBasePtr)
      {
         UT_SCRIPT_ABORT("Specified platform_type does not contain a space mover");
      }

      auto*  calPtr             = aVarArgs[1].GetPointer()->GetAppObject<UtCalendar>();
      double localSunHour       = aVarArgs[2].GetDouble();
      double circularAltitude_m = aVarArgs[3].GetDouble();

      WsfIntegratingSpaceMover* integratingSpaceMover = dynamic_cast<WsfIntegratingSpaceMover*>(spaceMoverBasePtr);
      if (integratingSpaceMover)
      {
         j2               = 0.0;
         auto dynamicsPtr = integratingSpaceMover->GetOrbitalDynamicsClone();
         auto j2Terms     = dynamicsPtr->GetTerms<WsfEarthJ2Term>();
         if (j2Terms.empty())
         {
            UT_SCRIPT_ABORT("Sun-synchronous orbits require an earth_j2 dynamical term.");
         }
         // Specifying multiple j2 terms is equivalent to specifying the sum of the j2 values of the j2 terms.
         for (const auto& term : j2Terms)
         {
            j2 += term->GetJ2();
         }
      }
      else if (!spaceMoverBasePtr->GetPropagator().GetPerturbation())
      {
         UT_SCRIPT_ABORT("Sun-synchronous orbits require oblate_earth.");
      }

      UtOrbitalElements elements(spaceMoverBasePtr->GetPropagator().GetInitialOrbitalState().GetCentralBody());
      elements.SetSunSynchronous(*calPtr, localSunHour, circularAltitude_m, j2);

      ut::OrbitalState state(ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                             ut::OrbitalState::ReferenceFrame::cECI,
                             elements);
      spaceMoverBasePtr->SetInitialOrbitalState(state);

      // Allows manipulation of the platform before it is really added to the simulation
      platformPtr->AssignToSimulation(SIMULATION);
   }
   // The memory management option on the script reference is defined so that the platform created by
   // this method will be deleted when the last reference is deleted. This ensures the platform object
   // will be deleted if AddPlatform is called or if AddPlatform fails to add it to the simulation.
   aReturnVal.SetPointer(new UtScriptRef(platformPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}
} // namespace script
} // namespace space
} // namespace wsf
