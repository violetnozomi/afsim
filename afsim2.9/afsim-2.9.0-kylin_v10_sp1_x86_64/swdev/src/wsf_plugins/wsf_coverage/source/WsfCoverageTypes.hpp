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

#ifndef WSFCOVERAGETYPES_HPP
#define WSFCOVERAGETYPES_HPP

#include "wsf_coverage_export.h"

#include "WsfCoverage.hpp"
#include "WsfObjectTypeList.hpp"

namespace wsf
{
namespace coverage
{

class CoverageTypes : public WsfObjectTypeList<wsf::coverage::Coverage>
{
public:
   static WSF_COVERAGE_EXPORT CoverageTypes& Get(const WsfScenario& aScenario);

   WSF_COVERAGE_EXPORT explicit CoverageTypes(WsfScenario& aScenario);
};

} // namespace coverage
} // namespace wsf

#endif // WSFCOVERAGETYPES_HPP
