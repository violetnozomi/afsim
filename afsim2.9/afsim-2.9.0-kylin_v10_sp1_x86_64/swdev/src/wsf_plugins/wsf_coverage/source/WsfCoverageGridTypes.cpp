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

#include "WsfCoverageGridTypes.hpp"

#include "WsfCoverageCompositeGrid.hpp"
#include "WsfCoverageDistanceSteppedGrid.hpp"
#include "WsfCoverageExistingPlatformGrid.hpp"
#include "WsfCoverageLatLonGrid.hpp"
#include "WsfCoverageZoneBasedGrid.hpp"
#include "WsfScenario.hpp"

namespace wsf
{
namespace coverage
{

GridTypes& GridTypes::Get(const WsfScenario& aScenario)
{
   return static_cast<GridTypes&>(*aScenario.GetTypes("coverage_grid"));
}

GridTypes::GridTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<Grid>(aScenario, cSINGULAR_BASE_TYPE, "coverage_grid")
{
   Add(CompositeGrid::cTYPE, ut::make_unique<CompositeGrid>(aScenario));
   Add(DistanceSteppedGrid::cTYPE, ut::make_unique<DistanceSteppedGrid>(aScenario));
   Add(ExistingPlatformGrid::cTYPE, ut::make_unique<ExistingPlatformGrid>(aScenario));
   Add(LatLonGrid::cTYPE, ut::make_unique<LatLonGrid>(aScenario));
   Add(ZoneBasedGrid::cTYPE, ut::make_unique<ZoneBasedGrid>(aScenario));
}

} // namespace coverage
} // namespace wsf
