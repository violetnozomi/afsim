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

#include "WsfCoverageScenarioExtension.hpp"

#include "UtMemory.hpp"
#include "WsfCoverageGridTypes.hpp"
#include "WsfCoverageMeasureOutputTypes.hpp"
#include "WsfCoverageMeasureTypes.hpp"
#include "WsfCoverageSimulationExtension.hpp"
#include "WsfCoverageTypes.hpp"
#include "WsfScenario.hpp"

namespace wsf
{
namespace coverage
{

ScenarioExtension::ScenarioExtension()
   : WsfCloneableScenarioExtension(new SimulationExtension{*this})
{
}

void ScenarioExtension::AddedToScenario()
{
   WsfScenario& scenario = GetScenario();

   scenario.AddTypeList(ut::make_unique<GridTypes>(scenario));
   scenario.AddTypeList(ut::make_unique<CoverageTypes>(scenario));
   scenario.AddTypeList(ut::make_unique<MeasureTypes>(scenario));
   scenario.AddTypeList(ut::make_unique<MeasureOutputTypes>(scenario));
}

} // namespace coverage
} // namespace wsf
