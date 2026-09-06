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

#include "WsfAtmosphereScenarioExtension.hpp"

#include "UtMemory.hpp"
#include "WsfAtmosphereSimulationExtension.hpp"
#include "WsfAtmosphereTypes.hpp"
#include "WsfScenario.hpp"

namespace wsf
{
namespace space
{

AtmosphereScenarioExtension::AtmosphereScenarioExtension()
   : WsfCloneableScenarioExtension(new AtmosphereSimulationExtension{*this})
{
}

void AtmosphereScenarioExtension::AddedToScenario()
{
   WsfScenario& scenario = GetScenario();

   scenario.AddTypeList(ut::make_unique<wsf::space::AtmosphereTypes>(scenario));
}

} // namespace space
} // namespace wsf
