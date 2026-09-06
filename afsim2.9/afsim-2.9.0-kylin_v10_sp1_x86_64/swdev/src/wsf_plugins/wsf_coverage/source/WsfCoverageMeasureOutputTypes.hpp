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

#ifndef WSFCOVERAGEMEASUREOUTPUTTYPES_HPP
#define WSFCOVERAGEMEASUREOUTPUTTYPES_HPP

#include "wsf_coverage_export.h"

#include "WsfCoverageMeasureOutput.hpp"
#include "WsfObjectTypeList.hpp"

namespace wsf
{
namespace coverage
{

class MeasureOutputTypes : public WsfObjectTypeList<MeasureOutput>
{
public:
   static WSF_COVERAGE_EXPORT MeasureOutputTypes& Get(const WsfScenario& aScenario);

   WSF_COVERAGE_EXPORT explicit MeasureOutputTypes(WsfScenario& aScenario);
};

} // namespace coverage
} // namespace wsf

#endif // WSFCOVERAGEMEASUREOUTPUTTYPES_HPP
