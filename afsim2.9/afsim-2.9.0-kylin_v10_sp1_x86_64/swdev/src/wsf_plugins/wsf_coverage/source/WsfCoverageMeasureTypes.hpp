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

#ifndef WSFCOVERAGEMEASURETYPES_HPP
#define WSFCOVERAGEMEASURETYPES_HPP

#include "wsf_coverage_export.h"

#include "WsfCoverageMeasure.hpp"
#include "WsfObjectTypeList.hpp"

namespace wsf
{
namespace coverage
{

class MeasureTypes : public WsfObjectTypeList<Measure>
{
public:
   static WSF_COVERAGE_EXPORT MeasureTypes& Get(const WsfScenario& aScenario);

   WSF_COVERAGE_EXPORT explicit MeasureTypes(WsfScenario& aScenario);
};

} // namespace coverage
} // namespace wsf

#endif // WSFCOVERAGEMEASURETYPES_HPP
