// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2022 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfSpaceOrbitalPropagatorConditionTypes.hpp"

#include "WsfScenario.hpp"

namespace wsf
{
namespace space
{
OrbitalPropagatorConditionTypes& OrbitalPropagatorConditionTypes::Get(const WsfScenario& aScenario)
{
   return static_cast<OrbitalPropagatorConditionTypes&>(*aScenario.GetTypes("orbital_propagator_condition"));
}

OrbitalPropagatorConditionTypes::OrbitalPropagatorConditionTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<OrbitalPropagatorCondition>(aScenario, cSINGULAR_BASE_TYPE, "orbital_propagator_condition")
{
   Add(ApoapsisCondition::cTYPE, ut::make_unique<ApoapsisCondition>());
   Add(AscendingNodeCondition::cTYPE, ut::make_unique<AscendingNodeCondition>());
   Add(AscendingRadiusCondition::cTYPE, ut::make_unique<AscendingRadiusCondition>());
   Add(DescendingNodeCondition::cTYPE, ut::make_unique<DescendingNodeCondition>());
   Add(DescendingRadiusCondition::cTYPE, ut::make_unique<DescendingRadiusCondition>());
   Add(EclipseEntryCondition::cTYPE, ut::make_unique<EclipseEntryCondition>());
   Add(EclipseExitCondition::cTYPE, ut::make_unique<EclipseExitCondition>());
   Add(NoneCondition::cTYPE, ut::make_unique<NoneCondition>());
   Add(NorthernIntersectionCondition::cTYPE, ut::make_unique<NorthernIntersectionCondition>());
   Add(PeriapsisCondition::cTYPE, ut::make_unique<PeriapsisCondition>());
   Add(RelativeTimeCondition::cTYPE, ut::make_unique<RelativeTimeCondition>());
   Add(SouthernIntersectionCondition::cTYPE, ut::make_unique<SouthernIntersectionCondition>());
}

} // namespace space
} // namespace wsf
