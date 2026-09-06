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

#ifndef WSFJ2_PERTURBATIONORBITALPROPAGATOR_HPP
#define WSFJ2_PERTURBATIONORBITALPROPAGATOR_HPP


#include "wsf_space_export.h"

#include "WsfKeplerianOrbitalPropagator.hpp"
#include "WsfOrbitalPropagatorTypes.hpp"
class WsfScenario;

namespace wsf
{
namespace space
{

//! A configuration of wsf::KeplerianOrbitalPropagator that sets the J2 gravitational perturbation option in UtOrbitalPropagator.
class WSF_SPACE_EXPORT J2_PerturbationOrbitalPropagator : public KeplerianOrbitalPropagator
{
public:
   explicit J2_PerturbationOrbitalPropagator(std::unique_ptr<ut::OrbitalState> aInitialOrbitalState);
   J2_PerturbationOrbitalPropagator();
   J2_PerturbationOrbitalPropagator(const J2_PerturbationOrbitalPropagator& aOther) = default;
   ~J2_PerturbationOrbitalPropagator() override                                     = default;

   J2_PerturbationOrbitalPropagator* Clone() const override { return new J2_PerturbationOrbitalPropagator(*this); }
   J2_PerturbationOrbitalPropagator& operator=(const J2_PerturbationOrbitalPropagator& aOther) = delete;

   bool Initialize(const UtCalendar& aEpoch) override;

   static WsfOrbitalPropagatorTypes::FactoryPtr ObjectFactory(WsfScenario& aScenario);
};

} // namespace space
} // namespace wsf
#endif
