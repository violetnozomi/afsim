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

#include "WsfCoverageTypes.hpp"

#include "WsfScenario.hpp"
#include "WsfSensorCoverage.hpp"

namespace wsf
{
namespace coverage
{

CoverageTypes& CoverageTypes::Get(const WsfScenario& aScenario)
{
   return static_cast<CoverageTypes&>(*aScenario.GetTypes("coverage_type"));
}

CoverageTypes::CoverageTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<wsf::coverage::Coverage>(aScenario, cSINGULAR_BASE_TYPE, "coverage_type")
{
   Add(SensorCoverage::cTYPE, ut::make_unique<SensorCoverage>(aScenario));
}

} // namespace coverage
} // namespace wsf
