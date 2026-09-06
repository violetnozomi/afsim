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

#include "WsfOrbitalManeuveringTypes.hpp"

#include "UtMemory.hpp"
#include "WsfComponentFactory.hpp"
#include "WsfScenario.hpp"
#include "WsfSpaceMoverBase.hpp"

namespace
{
//! Component factory to process platform input.
class OrbitalManeuveringComponentFactory : public WsfComponentFactory<WsfSpaceMoverBase>
{
public:
};
} // namespace

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
WsfOrbitalManeuveringTypes& WsfOrbitalManeuveringTypes::Get(const WsfScenario& aScenario)
{
   return static_cast<WsfOrbitalManeuveringTypes&>(*aScenario.GetTypes("orbital_maneuvering"));
}

// =================================================================================================
WsfOrbitalManeuveringTypes::WsfOrbitalManeuveringTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfOrbitalManeuvering>(aScenario, cSINGULAR_BASE_TYPE, "orbital_maneuvering")
{
   aScenario.RegisterComponentFactory(
      ut::make_unique<OrbitalManeuveringComponentFactory>()); // Allows for definition inside
                                                              // platform, platform_type blocks.
}
