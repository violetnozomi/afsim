// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "UtMemory.hpp"
#include "WsfComponentFactory.hpp"
#include "WsfOrbitalMissionEventTypes.hpp"
#include "WsfScenario.hpp"
#include "WsfSpaceMoverBase.hpp"

namespace
{
//! Component factory to process platform input.
class OrbitalMissionEventComponentFactory : public WsfComponentFactory<WsfSpaceMoverBase>
{
};
} // namespace

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
WsfOrbitalMissionEventTypes& WsfOrbitalMissionEventTypes::Get(const WsfScenario& aScenario)
{
   return static_cast<WsfOrbitalMissionEventTypes&>(*aScenario.GetTypes("orbital_mission_event"));
}

// =================================================================================================
WsfOrbitalMissionEventTypes::WsfOrbitalMissionEventTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfOrbitalEvent>(aScenario, cSINGULAR_BASE_TYPE, "orbital_mission_event")
{
   aScenario.RegisterComponentFactory(
      ut::make_unique<OrbitalMissionEventComponentFactory>()); // Allows for definition inside
                                                               // platform, platform_type blocks.
}
