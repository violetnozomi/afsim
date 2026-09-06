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

#include "WsfCoverageMeasureTypes.hpp"

#include "UtMemory.hpp"
#include "WsfCoverageAccessDuration.hpp"
#include "WsfCoverageCoverageTime.hpp"
#include "WsfCoverageN_AssetCoverage.hpp"
#include "WsfCoverageNumberOfAccesses.hpp"
#include "WsfCoverageNumberOfGaps.hpp"
#include "WsfCoverageRevisitTime.hpp"
#include "WsfCoverageTimeAverageGap.hpp"
#include "WsfScenario.hpp"
#include "WsfSimpleCoverage.hpp"

namespace wsf
{
namespace coverage
{

MeasureTypes& MeasureTypes::Get(const WsfScenario& aScenario)
{
   return static_cast<MeasureTypes&>(*aScenario.GetTypes("coverage_measure"));
}

MeasureTypes::MeasureTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<Measure>(aScenario, cSINGULAR_BASE_TYPE, "coverage_measure")
{
   Add(AccessDuration::cTYPE, ut::make_unique<AccessDuration>(aScenario));
   Add(CoverageTime::cTYPE, ut::make_unique<CoverageTime>(aScenario));
   Add(N_AssetCoverage::cTYPE, ut::make_unique<N_AssetCoverage>(aScenario));
   Add(NumberOfAccesses::cTYPE, ut::make_unique<NumberOfAccesses>(aScenario));
   Add(NumberOfGaps::cTYPE, ut::make_unique<NumberOfGaps>(aScenario));
   Add(SimpleCoverage::cTYPE, ut::make_unique<SimpleCoverage>(aScenario));
   Add(RevisitTime::cTYPE, ut::make_unique<RevisitTime>(aScenario));
   Add(TimeAverageGap::cTYPE, ut::make_unique<TimeAverageGap>(aScenario));
}

} // namespace coverage
} // namespace wsf
