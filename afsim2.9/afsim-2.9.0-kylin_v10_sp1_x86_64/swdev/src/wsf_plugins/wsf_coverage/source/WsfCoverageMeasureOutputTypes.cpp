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

#include "WsfCoverageMeasureOutputTypes.hpp"

#include "WsfCoverageMeasureCSV_Output.hpp"
#include "WsfCoverageMeasureGridStatsOutput.hpp"
#include "WsfCoverageMeasureLatLonStatsOutput.hpp"
#include "WsfScenario.hpp"

namespace wsf
{
namespace coverage
{

MeasureOutputTypes& MeasureOutputTypes::Get(const WsfScenario& aScenario)
{
   return static_cast<MeasureOutputTypes&>(*aScenario.GetTypes("coverage_measure_output"));
}

MeasureOutputTypes::MeasureOutputTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<MeasureOutput>(aScenario, cSINGULAR_BASE_TYPE, "coverage_measure_output")
{
   Add(MeasureCSV_Output::cTYPE, ut::make_unique<MeasureCSV_Output>());
   Add(MeasureGridStatsOutput::cTYPE, ut::make_unique<MeasureGridStatsOutput>());
   Add(MeasureLatLonStatsOutput::cTYPE, ut::make_unique<MeasureLatLonStatsOutput>());
}

} // namespace coverage
} // namespace wsf
