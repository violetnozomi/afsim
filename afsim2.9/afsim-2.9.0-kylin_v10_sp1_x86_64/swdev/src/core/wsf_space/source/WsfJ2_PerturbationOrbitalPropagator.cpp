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

#include "WsfJ2_PerturbationOrbitalPropagator.hpp"

#include <string>

#include "UtLog.hpp"

namespace wsf
{
namespace space
{

J2_PerturbationOrbitalPropagator::J2_PerturbationOrbitalPropagator(std::unique_ptr<ut::OrbitalState> aInitialOrbitalState)
   : KeplerianOrbitalPropagator(std::move(aInitialOrbitalState))
{
   SetPrecession(true);
}

J2_PerturbationOrbitalPropagator::J2_PerturbationOrbitalPropagator()
{
   SetPrecession(true);
}

bool J2_PerturbationOrbitalPropagator::Initialize(const UtCalendar& aEpoch)
{
   if (!GetInitialOrbitalState().GetCentralPoint().IsEarth())
   {
      auto out = ut::log::error() << "(WSF_J2_PERTURBATION_PROPAGATOR) Earth must be the central_body.";
      out.AddNote() << GetInitialOrbitalState().GetCentralPoint().GetName() << " is the currently selected central_body.";
      return false;
   }
   return KeplerianOrbitalPropagator::Initialize(aEpoch);
}

WsfOrbitalPropagatorTypes::FactoryPtr J2_PerturbationOrbitalPropagator::ObjectFactory(WsfScenario& aScenario)
{
   return [&aScenario](const std::string& aTypeName) -> std::unique_ptr<UtOrbitalPropagatorBase>
   {
      if (aTypeName == "WSF_J2_PERTURBATION_PROPAGATOR")
      {
         return ut::make_unique<J2_PerturbationOrbitalPropagator>();
      }
      return nullptr;
   };
}

} // namespace space
} // namespace wsf
