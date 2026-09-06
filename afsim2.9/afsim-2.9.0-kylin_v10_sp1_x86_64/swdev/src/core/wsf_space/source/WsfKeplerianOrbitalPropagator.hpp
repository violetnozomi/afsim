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

#ifndef WSFKEPLERIANORBITALPROPAGATOR_HPP
#define WSFKEPLERIANORBITALPROPAGATOR_HPP


#include "wsf_space_export.h"

#include "UtOrbitalPropagator.hpp"
#include "WsfOrbitalPropagatorTypes.hpp"
class WsfScenario;

namespace wsf
{
namespace space
{
//! An orbital propagator that implements Keplerian, ellipsoidal, two-body motion.
class WSF_SPACE_EXPORT KeplerianOrbitalPropagator : public UtOrbitalPropagator
{
public:
   explicit KeplerianOrbitalPropagator(std::unique_ptr<ut::OrbitalState> aInitialOrbitalState);
   KeplerianOrbitalPropagator()                                         = default;
   KeplerianOrbitalPropagator(const KeplerianOrbitalPropagator& aOther) = default;
   ~KeplerianOrbitalPropagator() override                               = default;

   KeplerianOrbitalPropagator* Clone() const override { return new KeplerianOrbitalPropagator(*this); }
   KeplerianOrbitalPropagator& operator=(const KeplerianOrbitalPropagator& aOther) = delete;

   static WsfOrbitalPropagatorTypes::FactoryPtr ObjectFactory(WsfScenario& aScenario);
};

} // namespace space
} // namespace wsf

#endif
