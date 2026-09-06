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

#include "WsfKeplerianOrbitalPropagator.hpp"

namespace wsf
{
namespace space
{

KeplerianOrbitalPropagator::KeplerianOrbitalPropagator(std::unique_ptr<ut::OrbitalState> aInitialOrbitalState)
   : UtOrbitalPropagator(std::move(aInitialOrbitalState))
{
}

WsfOrbitalPropagatorTypes::FactoryPtr KeplerianOrbitalPropagator::ObjectFactory(WsfScenario& aScenario)
{
   return [&aScenario](const std::string& aTypeName) -> std::unique_ptr<UtOrbitalPropagatorBase>
   {
      if (aTypeName == "WSF_KEPLERIAN_PROPAGATOR")
      {
         return ut::make_unique<KeplerianOrbitalPropagator>();
      }
      return nullptr;
   };
}

} // namespace space
} // namespace wsf
